#include "hkx_loader.h"
#include "godot_cpp/classes/animation.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/resource_saver.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/quaternion.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "havok.h"
#include "hkx.h"
#include "nurbs.h"
#include "resource_manager.h"

using namespace godot;

enum QuantizationType
{
	k8Bit = 0,
	k16Bit,
	k32Bit,
	k40Bit,
	k48Bit
};

Quaternion read40BitQuaternion(uint64_t value)
{
	/*
	 * 40 bit Quaternion structure
	 * - 12 bit x signed integer
	 * - 12 bit y signed integer
	 * - 12 bit z signed integer
	 * - 2 bit shift
	 * - 1 bit invert sign
	 * - 1 bit unused?
	 */
	constexpr float fractal = 0.000345436f;

	Quaternion result;

	int16 x = value & 0x0000000000000FFFu;
	int16 y = (value >> 12u) & 0x0000000000000FFFu;
	int16 z = (value >> 24u) & 0x0000000000000FFFu;
	int16 w = 0;

	const unsigned int resultShift = (value >> 36u) & 3u;
	const bool invertSign = (value >> 38u) & 1;

	x -= 0x0000000000000801;
	y -= 0x0000000000000801;
	z -= 0x0000000000000801;
	w -= 0x0000000000000801;

	Vector4 tmp2(x * fractal, y * fractal, z * fractal, 0.0f);
	tmp2.w = std::sqrt(1.0f - tmp2.dot(tmp2));
	if (invertSign)
		tmp2.w = -tmp2.w;

	for (int i = 0; i < 3 - resultShift; ++i)
	{
		std::swap(tmp2[3 - i], tmp2[2 - i]);
	}

	result.x = tmp2.x;
	result.y = tmp2.y;
	result.z = tmp2.z;
	result.w = tmp2.w;

	return result;
}

Ref<Animation>
parse_spline_compressed_animation(hkaSplineCompressedAnimation *animation)
{
	Ref<Animation> new_anim;
	new_anim.instantiate();
	new_anim->set_length(animation->m_duration);
	String rigName("Skeleton3D");
	auto numTracks = animation->m_numberOfTransformTracks;
	for (int i = 0; i < numTracks; i++)
	{
		new_anim->add_track(Animation::TrackType::TYPE_POSITION_3D, i * 2);
		auto boneName = animation->m_annotationTracks.data[i].m_trackName;
		new_anim->track_set_path(i * 2, NodePath(rigName + ":" + boneName));
		new_anim->add_track(Animation::TrackType::TYPE_ROTATION_3D,
							i * 2 + 1);
		new_anim->track_set_path(i * 2 + 1,
								 NodePath(rigName + ":" + boneName));
	}

	float blockStartTime = 0.0f;
	float frameDuration = animation->m_frameDuration;

	for (int blockIdx = 0; blockIdx < animation->m_blockOffsets.size; blockIdx++)
	{
		auto block = animation->m_data.data + animation->m_blockOffsets.data[blockIdx];
		auto pendingFrames = animation->m_numFrames;
		uint8 *transformMasks = block;
		block += (numTracks * 4 + animation->m_numberOfFloatTracks);


		auto begin = block;
		auto numBlockFrames =
			std::min(pendingFrames, animation->m_maxFramesPerBlock);
		pendingFrames -= numBlockFrames;

		for (int trackIdx = 0; trackIdx < numTracks; trackIdx++)
		{
			uint8 quantizationTypes = transformMasks[trackIdx * 4 + 0];
			uint8 translationTypes = transformMasks[trackIdx * 4 + 1];
			uint8 rotationTypes = transformMasks[trackIdx * 4 + 2];
			uint8 scaleTypes = transformMasks[trackIdx * 4 + 3];
			const bool transformSplineX = (translationTypes & 0x10) != 0;
			const bool transformSplineY = (translationTypes & 0x20) != 0;
			const bool transformSplineZ = (translationTypes & 0x40) != 0;
			const bool transformStaticX = (translationTypes & 0x01) != 0;
			const bool transformStaticY = (translationTypes & 0x02) != 0;
			const bool transformStaticZ = (translationTypes & 0x04) != 0;
			const bool rotationTypeSpline = (rotationTypes & 0xf0) != 0;
			const bool rotationTypeStatic = (rotationTypes & 0x0f) != 0;
			const bool scaleSplineX = (scaleTypes & 0x10) != 0;
			const bool scaleSplineY = (scaleTypes & 0x20) != 0;
			const bool scaleSplineZ = (scaleTypes & 0x40) != 0;
			const bool scaleStaticX = (scaleTypes & 0x01) != 0;
			const bool scaleStaticY = (scaleTypes & 0x02) != 0;
			const bool scaleStaticZ = (scaleTypes & 0x04) != 0;

			const bool transformSpline =
				transformSplineX || transformSplineY || transformSplineZ;
			const bool transformStatic =
				transformStaticX || transformStaticY || transformStaticZ;
			const bool scaleSpline = scaleSplineX || scaleSplineY || scaleSplineZ;
			const bool scaleStatic = scaleStaticX || scaleStaticY || scaleStaticZ;

			auto positionQuantizationType =
				QuantizationType(quantizationTypes & 0x03);
			auto rotationQuantizationType =
				QuantizationType(((quantizationTypes >> 2u) & 0x0f) + 2);
			auto scaleQuantizationType =
				QuantizationType((quantizationTypes >> 6u) & 0x03);

			if (transformSpline)
			{
				uint16 numItems = reinterpret_cast<uint16 *>(block)[0];
				block += 2;
				uint8 degree = *block++;

				uint8 *knots = block;
				block += numItems + degree + 2;

				if ((block - begin) % 4 != 0)
					block += 4 - (block - begin) % 4;

				float minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
				float staticx = 0, staticy = 0, staticz = 0;
				if (transformSplineX)
				{
					minx = reinterpret_cast<float *>(block)[0];
					maxx = reinterpret_cast<float *>(block)[1];
					block += 8;
				}
				else if (transformStaticX)
				{
					staticx = reinterpret_cast<float *>(block)[0];
					block += 4;
				}

				if (transformSplineY)
				{
					miny = reinterpret_cast<float *>(block)[0];
					maxy = reinterpret_cast<float *>(block)[1];
					block += 8;
				}
				else if (transformStaticY)
				{
					staticy = reinterpret_cast<float *>(block)[0];
					block += 4;
				}

				if (transformSplineZ)
				{
					minz = reinterpret_cast<float *>(block)[0];
					maxz = reinterpret_cast<float *>(block)[1];
					block += 8;
				}
				else if (transformStaticZ)
				{
					staticz = reinterpret_cast<float *>(block)[0];
					block += 4;
				}

				std::vector<Vector3> positionControlPoints;
				for (int i = 0; i <= numItems; ++i)
				{
					Vector3 position;
					switch (positionQuantizationType)
					{
					case k8Bit:
						if (transformSplineX)
							position.x = static_cast<float>(*block++) * (1.0f / 255.0f);
						if (transformSplineY)
							position.y = static_cast<float>(*block++) * (1.0f / 255.0f);
						if (transformSplineZ)
							position.z = static_cast<float>(*block++) * (1.0f / 255.0f);
						break;

					case k16Bit:
						if (transformSplineX)
						{
							uint16 val = *reinterpret_cast<uint16 *>(block);
							position.x = static_cast<float>(val) * (1.0f / 65535.0f);
							block += 2;
						}
						if (transformSplineY)
						{
							uint16 val = *reinterpret_cast<uint16 *>(block);
							position.y = static_cast<float>(val) * (1.0f / 65535.0f);
							block += 2;
						}
						if (transformSplineZ)
						{
							uint16 val = *reinterpret_cast<uint16 *>(block);
							position.z = static_cast<float>(val) * (1.0f / 65535.0f);
							block += 2;
						}
						break;

					default:
						UtilityFunctions::printerr(
							"Unsupported position quantization type: ",
							positionQuantizationType);
						return Ref<Animation>();
					}

					position.x = minx + (maxx - minx) * position.x;
					position.y = miny + (maxy - miny) * position.y;
					position.z = minz + (maxz - minz) * position.z;

					if (!transformSplineX)
						position.x = staticx;
					if (!transformSplineY)
						position.y = staticy;
					if (!transformSplineZ)
						position.z = staticz;

					positionControlPoints.emplace_back(position);
				}

				NURBS<Vector3> nurbs(positionControlPoints, knots, degree);
				for (int frameIdx = 0; frameIdx < numBlockFrames; frameIdx++)
				{
					auto pos = nurbs.interpolate(frameIdx);
					new_anim->track_insert_key(
						trackIdx * 2, blockStartTime + frameIdx * frameDuration, pos);
				}

				if ((block - begin) % 4 != 0)
					block += 4 - (block - begin) % 4;
			}
			else if (transformStatic)
			{
				Vector3 position;
				if (transformStaticX)
				{
					position.x = reinterpret_cast<float *>(block)[0];
					block += 4;
				}
				if (transformStaticY)
				{
					position.y = reinterpret_cast<float *>(block)[0];
					block += 4;
				}
				if (transformStaticZ)
				{
					position.z = reinterpret_cast<float *>(block)[0];
					block += 4;
				}

				new_anim->track_insert_key(trackIdx * 2, blockStartTime, position);
			}

			if (rotationTypeSpline)
			{
				uint16_t numItems = reinterpret_cast<uint16_t *>(block)[0];
				block += 2;
				uint8_t degree = *block++;

				uint8 *knots = block;
				block += numItems + degree + 2;

				std::vector<Quaternion> rotationControlPoints;
				for (int i = 0; i <= numItems; ++i)
				{
					Quaternion rotation;

					switch (rotationQuantizationType)
					{
					case k40Bit:
						rotation = read40BitQuaternion(reinterpret_cast<uint64_t *>(block)[0]);
						block += 5;
						break;
					default:
						UtilityFunctions::printerr(
							"Unsupported rotation quantization type: ",
							rotationQuantizationType);
						return Ref<Animation>();
					}

					rotationControlPoints.emplace_back(rotation);
				}

				NURBS<Quaternion> nurbs(rotationControlPoints, knots, degree);
				for (int frameIdx = 0; frameIdx < numBlockFrames; frameIdx++)
				{
					auto rot = nurbs.interpolate(frameIdx);
					new_anim->track_insert_key(trackIdx * 2 + 1,
											   blockStartTime + frameIdx * frameDuration,
											   rot.normalized());
				}
			}
			else if (rotationTypeStatic)
			{
				Quaternion rotation;
				switch (rotationQuantizationType)
				{
				case k40Bit:
					rotation = read40BitQuaternion(reinterpret_cast<uint64_t *>(block)[0]);
					block += 5;
					break;
				default:
					UtilityFunctions::printerr(
						"Unsupported rotation quantization type: ",
						rotationQuantizationType);
					return Ref<Animation>();
				}

				new_anim->track_insert_key(trackIdx * 2 + 1, blockStartTime, rotation.normalized());
			}

			if ((block - begin) % 4 != 0)
				block += 4 - (block - begin) % 4;

			if (scaleSpline)
			{
				UtilityFunctions::printerr("Scale spline not supported");
				return Ref<Animation>();
			}
			else if (scaleStatic)
			{
				if (scaleStaticX)
					block += 4;
				if (scaleStaticY)
					block += 4;
				if (scaleStaticZ)
					block += 4;
			}
		}
		blockStartTime += numBlockFrames * frameDuration;
	}

	return new_anim;
}

Ref<Animation> HKXLoader::load_animation(PackedByteArray animation_data, PackedByteArray skeleton_data)
{
	HKX animHKX(reinterpret_cast<char *>(animation_data.ptrw()));
	hkaAnimationContainer *animContainer = nullptr;
	for (int i = 0; i < animHKX.rootContainer->m_namedVariants.size; i++)
	{
		auto &namedVar = animHKX.rootContainer->m_namedVariants.data[i];
		if (strcmp(namedVar.m_className, "hkaAnimationContainer") == 0)
		{
			animContainer = static_cast<hkaAnimationContainer *>(namedVar.m_variant);
			break;
		}
	}

	if (!animContainer || animContainer->m_animations.size == 0 || animContainer->m_bindings.size == 0)
	{
		UtilityFunctions::printerr("Invalid animation file");
		return Ref<Animation>();
	}

	auto skeletonName = animContainer->m_bindings.data[0]->m_originalSkeletonName;
	hkaSkeleton *skeleton = nullptr;
	HKX skeletonHKX(reinterpret_cast<char *>(skeleton_data.ptrw()));

	for (int i = 0; i < skeletonHKX.rootContainer->m_namedVariants.size; i++)
	{
		auto &namedVar = skeletonHKX.rootContainer->m_namedVariants.data[i];
		if (strcmp(namedVar.m_className, "hkaAnimationContainer") == 0)
		{
			auto skeletons = static_cast<hkaAnimationContainer *>(namedVar.m_variant)->m_skeletons;
			if (skeletons.size > 0)
			{
				for (int j = 0; j < skeletons.size; j++)
				{
					if (strcmp(skeletons.data[j]->m_name, skeletonName) == 0)
					{
						skeleton = skeletons.data[j];
						break;
					}
				}
			}
			break;
		}
	}

	hkaAnimation *animation = animContainer->m_animations.data[0];

	if (!skeleton || skeleton->m_bones.size != animation->m_annotationTracks.size)
	{
		UtilityFunctions::printerr("No matching skeleton found");
		return Ref<Animation>();
	}

	for (int i = 0; i < animation->m_annotationTracks.size; i++)
	{
		auto &track = animation->m_annotationTracks.data[i];
		track.m_trackName = skeleton->m_bones.data[i].m_name;
	}

	switch (animation->m_type)
	{
	case hkaAnimation::HK_SPLINE_COMPRESSED_ANIMATION:
		return parse_spline_compressed_animation(
			static_cast<hkaSplineCompressedAnimation *>(animation));
	default:
		UtilityFunctions::print("Unsupported animation type: ",
								animation->m_type);
		break;
	}

	return Ref<Animation>();
}

void HKXLoader::_bind_methods()
{
	ClassDB::bind_static_method("HKXLoader", D_METHOD("load_animation", "animation_data", "skeleton_data"), &HKXLoader::load_animation);
}