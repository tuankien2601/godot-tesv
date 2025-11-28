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

enum QuantizationType { k8Bit = 0, k16Bit, k32Bit, k40Bit, k48Bit };

struct TransformTrackInfo {
  uint8 quantizationTypes;
  uint8 translationTypes;
  uint8 rotationTypes;
  uint8 scaleTypes;
};

Quaternion read40BitQuaternion(uint8 *data) {
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

  const uint64_t value = *data;

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

  for (int i = 0; i < 3 - resultShift; ++i) {
    std::swap(tmp2[3 - i], tmp2[2 - i]);
  }

  result.x = tmp2.x;
  result.y = tmp2.y;
  result.z = tmp2.z;
  result.w = tmp2.w;

  return result;
}

Ref<Animation>
parse_spline_compressed_animation(hkaSplineCompressedAnimation *animation) {
  Ref<Animation> new_anim;
  new_anim.instantiate();
  new_anim->set_length(animation->m_duration);
  String rigName("Skeleton3D");
  for (int i = 0; i < animation->m_numberOfTransformTracks; i++) {
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

  for (int i = 0; i < animation->m_blockOffsets.size; i++) {
    auto block = animation->m_data.data + animation->m_blockOffsets.data[i];
    auto pendingFrames = animation->m_numFrames;
    std::vector<TransformTrackInfo> trackInfos(
        animation->m_numberOfTransformTracks);
    for (auto &trackInfo : trackInfos) {
      trackInfo.quantizationTypes = *block++;
      trackInfo.translationTypes = *block++;
      trackInfo.rotationTypes = *block++;
      trackInfo.scaleTypes = *block++;
    }

    auto begin = block;
    auto numBlockFrames =
        std::min(pendingFrames, animation->m_maxFramesPerBlock);
    pendingFrames -= numBlockFrames;

    for (int trackIdx = 0; trackIdx < trackInfos.size(); trackIdx++) {
      const auto &trackInfo = trackInfos[trackIdx];
      const bool transformSplineX = (trackInfo.translationTypes & 0x10) != 0;
      const bool transformSplineY = (trackInfo.translationTypes & 0x20) != 0;
      const bool transformSplineZ = (trackInfo.translationTypes & 0x40) != 0;
      const bool transformStaticX = (trackInfo.translationTypes & 0x01) != 0;
      const bool transformStaticY = (trackInfo.translationTypes & 0x02) != 0;
      const bool transformStaticZ = (trackInfo.translationTypes & 0x04) != 0;
      const bool rotationTypeSpline = (trackInfo.rotationTypes & 0xf0) != 0;
      const bool rotationTypeStatic = (trackInfo.rotationTypes & 0x0f) != 0;
      const bool scaleSplineX = (trackInfo.scaleTypes & 0x10) != 0;
      const bool scaleSplineY = (trackInfo.scaleTypes & 0x20) != 0;
      const bool scaleSplineZ = (trackInfo.scaleTypes & 0x40) != 0;
      const bool scaleStaticX = (trackInfo.scaleTypes & 0x01) != 0;
      const bool scaleStaticY = (trackInfo.scaleTypes & 0x02) != 0;
      const bool scaleStaticZ = (trackInfo.scaleTypes & 0x04) != 0;

      const bool transformSpline =
          transformSplineX || transformSplineY || transformSplineZ;
      const bool transformStatic =
          transformStaticX || transformStaticY || transformStaticZ;
      const bool scaleSpline = scaleSplineX || scaleSplineY || scaleSplineZ;
      const bool scaleStatic = scaleStaticX || scaleStaticY || scaleStaticZ;

      auto positionQuantizationType =
          QuantizationType(trackInfo.quantizationTypes & 0x03);
      auto rotationQuantizationType =
          QuantizationType(((trackInfo.quantizationTypes >> 2u) & 0x0f) + 2);
      auto scaleQuantizationType =
          QuantizationType((trackInfo.quantizationTypes >> 6u) & 0x03);

      if (transformSpline) {
        uint16 numItems = *block;
        block += 2;
        uint8 degree = *block++;

        std::vector<uint8_t> knots(numItems + degree + 2);
        for (auto &knot : knots) {
          knot = *block++;
        }

        if ((block - begin) % 4 != 0)
          block += 4 - (block - begin) % 4;

        float minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
        float staticx = 0, staticy = 0, staticz = 0;
        if (transformSplineX) {
          minx = *block;
          block += 4;
          maxx = *block;
          block += 4;
        } else if (transformStaticX) {
          staticx = *block;
          block += 4;
        }

        if (transformSplineY) {
          miny = *block;
          block += 4;
          maxy = *block;
          block += 4;
        } else if (transformStaticY) {
          staticy = *block;
          block += 4;
        }

        if (transformSplineZ) {
          minz = *block;
          block += 4;
          maxz = *block;
          block += 4;
        } else if (transformStaticZ) {
          staticz = *block;
          block += 4;
        }

        std::vector<Vector3> positionControlPoints;
        for (int i = 0; i <= numItems; ++i) {
          Vector3 position;
          switch (positionQuantizationType) {
          case k8Bit:
            if (transformSplineX)
              position.x = static_cast<float>(*block++) * (1.0f / 255.0f);
            if (transformSplineY)
              position.y = static_cast<float>(*block++) * (1.0f / 255.0f);
            if (transformSplineZ)
              position.z = static_cast<float>(*block++) * (1.0f / 255.0f);
            break;

          case k16Bit:
            if (transformSplineX) {
              position.x = static_cast<float>(*block) * (1.0f / 65535.0f);
              block += 2;
            }
            if (transformSplineY) {
              position.y = static_cast<float>(*block) * (1.0f / 65535.0f);
              block += 2;
            }
            if (transformSplineZ) {
              position.z = static_cast<float>(*block) * (1.0f / 65535.0f);
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
        for (int frameIdx = 0; frameIdx < numBlockFrames; frameIdx++) {
          auto pos = nurbs.interpolate(frameIdx);
          new_anim->track_insert_key(
              trackIdx * 2, blockStartTime + frameIdx * frameDuration, pos);
        }

        if ((block - begin) % 4 != 0)
          block += 4 - (block - begin) % 4;
      } else if (transformStatic) {
        Vector3 position;
        if (transformStaticX) {
          position.x = *block;
          block += 4;
        }
        if (transformStaticY) {
          position.y = *block;
          block += 4;
        }
        if (transformStaticZ) {
          position.z = *block;
          block += 4;
        }

        new_anim->track_insert_key(trackIdx * 2, blockStartTime, position);
      }

      if (rotationTypeSpline) {
        uint16_t numItems = *block;
        block += 2;
        uint8_t degree = *block++;

        std::vector<uint8_t> knots(numItems + degree + 2);
        for (auto &knot : knots) {
          knot = *block++;
        }

        std::vector<Quaternion> rotationControlPoints;
        for (int i = 0; i <= numItems; ++i) {
          Quaternion rotation;

          switch (rotationQuantizationType) {
          case k40Bit:
            rotation = read40BitQuaternion(block);
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
        for (int frameIdx = 0; frameIdx < numBlockFrames; frameIdx++) {
          new_anim->track_insert_key(trackIdx * 2 + 1,
                                     blockStartTime + frameIdx * frameDuration,
                                     nurbs.interpolate(frameIdx));
        }
      } else if (rotationTypeStatic) {
        Quaternion rotation;
        switch (rotationQuantizationType) {
        case k40Bit:
          rotation = read40BitQuaternion(block);
          block += 5;
          break;
        default:
          UtilityFunctions::printerr(
              "Unsupported rotation quantization type: ",
              rotationQuantizationType);
          return Ref<Animation>();
        }
        new_anim->track_insert_key(trackIdx * 2 + 1, blockStartTime, rotation);
      }

      if ((block - begin) % 4 != 0)
        block += 4 - (block - begin) % 4;

      if (scaleSpline) {
        UtilityFunctions::printerr("Scale spline not supported");
        return Ref<Animation>();
      } else if (scaleStatic) {
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

Ref<Animation> HKXLoader::load_animation(PackedByteArray animation_data, PackedByteArray skeleton_data) {
  HKX animHKX(reinterpret_cast<char *>(animation_data.ptrw()));
  hkaAnimationContainer *animContainer = nullptr;
  for (int i = 0; i < animHKX.rootContainer->m_namedVariants.size; i++) {
    auto &namedVar = animHKX.rootContainer->m_namedVariants.data[i];
    if (strcmp(namedVar.m_className, "hkaAnimationContainer") == 0) {
      animContainer = static_cast<hkaAnimationContainer *>(namedVar.m_variant);
      break;
    }
  }

  if (!animContainer || animContainer->m_animations.size == 0 || animContainer->m_bindings.size == 0) {
    UtilityFunctions::printerr("Invalid animation file");
    return Ref<Animation>();
  }

  auto skeletonName = animContainer->m_bindings.data[0]->m_originalSkeletonName;
  hkaSkeleton *skeleton = nullptr;
  HKX skeletonHKX(reinterpret_cast<char *>(skeleton_data.ptrw()));

  for (int i = 0; i < skeletonHKX.rootContainer->m_namedVariants.size; i++) {
    auto &namedVar = skeletonHKX.rootContainer->m_namedVariants.data[i];
    if (strcmp(namedVar.m_className, "hkaAnimationContainer") == 0) {
      auto skeletons = static_cast<hkaAnimationContainer *>(namedVar.m_variant)->m_skeletons;
      if (skeletons.size > 0) {
        for (int j = 0; j < skeletons.size; j++) {
          if (strcmp(skeletons.data[j]->m_name, skeletonName) == 0) {
            skeleton = skeletons.data[j];
            break;
          }
        }
      }
      break;
    }
  }

  hkaAnimation *animation = animContainer->m_animations.data[0];

  if (!skeleton || skeleton->m_bones.size != animation->m_annotationTracks.size) {
    UtilityFunctions::printerr("No matching skeleton found");
    return Ref<Animation>();
  }

  for (int i = 0; i < animation->m_annotationTracks.size; i++) {
    auto &track = animation->m_annotationTracks.data[i];
    track.m_trackName = skeleton->m_bones.data[i].m_name;
  }
  
  switch (animation->m_type) {
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

