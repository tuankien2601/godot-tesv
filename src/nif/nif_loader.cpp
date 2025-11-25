#include "nif_loader.h"
#include "NifFile.hpp"
#include "Object3d.hpp"
#include "bhk.hpp"
#include "utils.h"
#include "resource_manager.h"
#include "byte_array_stream.h"
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/sphere_shape3d.hpp>
#include <godot_cpp/classes/box_shape3d.hpp>
#include <godot_cpp/classes/capsule_shape3d.hpp>
#include <godot_cpp/classes/convex_polygon_shape3d.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;

const float HAVOK_SCALE = 0.6999125;

constexpr std::array<std::string_view, 9> TextureSlots = {
	"Diffuse",
	"Normal",
	"Glow",
	"Parallax",
	"Environment",
	"EnvironmentMask",
	"Unused",
	"Unused",
	"Unused",
};

void nif_to_godot_transform(nifly::MatTransform &source, Transform3D &target)
{
	target.origin.x = source.translation.x;
	target.origin.y = source.translation.z;
	target.origin.z = -source.translation.y;
	Basis rotation(
		Vector3(source.rotation[0][0], source.rotation[1][0], source.rotation[2][0]),
		Vector3(source.rotation[0][1], source.rotation[1][1], source.rotation[2][1]),
		Vector3(source.rotation[0][2], source.rotation[1][2], source.rotation[2][2]));

	Basis M(
		Vector3(1, 0, 0),
		Vector3(0, 0, 1),
		Vector3(0, -1, 0));

	target.basis = M * rotation * M.transposed();
}

float normalize_uint8(uint8_t v)
{
	return (static_cast<float>(v) / 255.0f) * 2.0f - 1.0f;
}

Skeleton3D *create_skeleton(nifly::NifFile &nif, nifly::BSTriShape *shape)
{
	auto skin_instance = nif.GetHeader().GetBlock<nifly::NiSkinInstance>(shape->SkinInstanceRef());

	Skeleton3D *skeleton = memnew(Skeleton3D);
	skeleton->set_name(sanitize_string(shape->name.get() + "_skel").c_str());

	for (auto bone_ref : skin_instance->boneRefs)
	{
		auto bone = nif.GetHeader().GetBlock<nifly::NiNode>(bone_ref);
		Transform3D transform;
		nif_to_godot_transform(bone->transform, transform);
		auto bone_idx = skeleton->add_bone(shape->name.get().c_str());
		skeleton->set_bone_rest(bone_idx, transform);
		skeleton->set_bone_pose(bone_idx, transform);
	}

	return skeleton;
}

Node3D *create_mesh(nifly::NifFile &nif, nifly::BSTriShape *shape)
{
	PackedVector3Array positions;
	PackedVector3Array normals;
	PackedColorArray vertex_colors;
	PackedVector2Array uvs;
	PackedFloat32Array bone_weights;
	PackedInt32Array bone_indices;
	PackedInt32Array indices;

	for (auto vertData : shape->vertData)
	{
		if (shape->vertexDesc.HasFlag(nifly::VertexFlags::VF_VERTEX))
		{
			Vector3 position;
			position.x = vertData.vert.x;
			position.y = vertData.vert.z;
			position.z = -vertData.vert.y;
			position /= 100.0;
			positions.append(position);
		}
		if (shape->vertexDesc.HasFlag(nifly::VertexFlags::VF_NORMAL))
		{
			Vector3 normal;
			normal.x = normalize_uint8(vertData.normal[0]);
			normal.y = normalize_uint8(vertData.normal[2]);
			normal.z = -normalize_uint8(vertData.normal[1]);
			normals.append(normal);
		}
		if (shape->vertexDesc.HasFlag(nifly::VertexFlags::VF_COLORS))
		{
			auto color = Color::from_rgba8(vertData.colorData[0], vertData.colorData[1], vertData.colorData[2], vertData.colorData[3]);
		}
		if (shape->vertexDesc.HasFlag(nifly::VertexFlags::VF_UV))
		{
			Vector2 uv;
			uv.x = vertData.uv.u;
			uv.y = vertData.uv.v;
			uvs.append(uv);
		}
		if (shape->vertexDesc.HasFlag(nifly::VertexFlags::VF_SKINNED))
		{
			for (int i = 0; i < 4; i++)
			{
				bone_indices.append(vertData.weightBones[i]);
				bone_weights.append(vertData.weights[i]);
			}
		}
	}

	Ref<ArrayMesh> mesh = memnew(ArrayMesh);

	Array arrays;
	arrays.resize(Mesh::ARRAY_MAX);

	if (positions.size() > 0)
		arrays[Mesh::ARRAY_VERTEX] = positions;
	if (normals.size() > 0)
		arrays[Mesh::ARRAY_NORMAL] = normals;
	if (vertex_colors.size() > 0)
		arrays[Mesh::ARRAY_COLOR] = vertex_colors;
	if (uvs.size() > 0)
		arrays[Mesh::ARRAY_TEX_UV] = uvs;
	if (shape->vertexDesc.HasFlag(nifly::VertexFlags::VF_SKINNED))
	{
		arrays[Mesh::ARRAY_BONES] = bone_indices;
		arrays[Mesh::ARRAY_WEIGHTS] = bone_weights;
	}

	for (auto triangle : shape->triangles)
	{
		indices.append(triangle.p1);
		indices.append(triangle.p3);
		indices.append(triangle.p2);
	}

	arrays[Mesh::ARRAY_INDEX] = indices;

	mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
	mesh->regen_normal_maps();

	auto shader_property = nif.GetHeader().GetBlock<nifly::BSLightingShaderProperty>(shape->ShaderPropertyRef());

	if (shader_property)
	{
		Ref<ShaderMaterial> mat;
		mat.instantiate();
		auto alpha_property = nif.GetAlphaProperty(shape);
		float alpha_test_threshold = 0.0f;
		bool alpha_blend_enable = false;
		bool alpha_test_enable = false;
		if (alpha_property)
		{
			alpha_blend_enable = (alpha_property->flags & (1 << 0)) != 0;
			alpha_test_enable = (alpha_property->flags & (1 << 9)) != 0;
			alpha_test_threshold = static_cast<float>(alpha_property->threshold) / 255.0f;
		}

		if (alpha_blend_enable || alpha_test_enable)
		{
			Ref<Shader> shader = ResourceLoader::get_singleton()->load("res://shaders/alpha.tres");
			mat->set_shader(shader);
			if (alpha_test_enable)
				mat->set_shader_parameter("AlphaTestThreshold", alpha_test_threshold);
		}
		else
		{
			Ref<Shader> shader = ResourceLoader::get_singleton()->load("res://shaders/basic.tres");
			mat->set_shader(shader);
		}

		auto texture_set = nif.GetHeader().GetBlock<nifly::BSShaderTextureSet>(shader_property->TextureSetRef());
		for (int i = 0; i < texture_set->textures.size(); i++)
		{
			auto tex_path = texture_set->textures[i].get();
			if (tex_path.empty())
				continue;
			auto tex = TESVResourceManager::load_texture(tex_path.c_str());
			mat->set_shader_parameter(TextureSlots[i].data(), tex);
		}

		Color spec_color(shader_property->specularColor.x, shader_property->specularColor.y, shader_property->specularColor.z);
		mat->set_shader_parameter("SpecularColor", spec_color);
		mat->set_shader_parameter("SpecularStrength", shader_property->specularStrength);
		mat->set_shader_parameter("Glossiness", shader_property->glossiness);
		Color emissive_color(shader_property->emissiveColor.x, shader_property->emissiveColor.y, shader_property->emissiveColor.z);
		mat->set_shader_parameter("EmissiveColor", emissive_color);
		mat->set_shader_parameter("EmissiveMultiple", shader_property->emissiveMultiple);
		mat->set_shader_parameter("LightingEffect1", shader_property->rimlightPower);
		mat->set_shader_parameter("LightingEffect2", shader_property->rimlightPower2);
		Vector2 uvOffset(shader_property->uvOffset.u, shader_property->uvOffset.v);
		mat->set_shader_parameter("UVOffset", uvOffset);
		Vector2 uvScale(shader_property->uvScale.u, shader_property->uvScale.v);
		mat->set_shader_parameter("UVScale", uvScale);

		mesh->surface_set_material(0, mat);
	}

	MeshInstance3D *mesh_instance = memnew(MeshInstance3D);
	mesh_instance->set_name(sanitize_string(shape->name.get()).c_str());
	mesh_instance->set_mesh(mesh);

	return mesh_instance;
}

void create_collision_shape(nifly::NifFile &nif, nifly::NiRef &shapeRef, PhysicsBody3D *physicsBody, Transform3D transform = Transform3D())
{
	if (auto block = nif.GetHeader().GetBlock<nifly::bhkListShape>(shapeRef))
	{
		for (auto subShapeRef : block->subShapeRefs)
		{
			create_collision_shape(nif, subShapeRef, physicsBody);
		}
	}
	else if (auto block = nif.GetHeader().GetBlock<nifly::bhkConvexListShape>(shapeRef))
	{
		for (auto subShapeRef : block->shapeRefs)
		{
			create_collision_shape(nif, subShapeRef, physicsBody);
		}
	}
	else if (auto block = nif.GetHeader().GetBlock<nifly::bhkTransformShape>(shapeRef))
	{
		auto m = block->xform;
		Transform3D local_transform;
		local_transform.basis = Basis(
			Vector3(m[0], m[1], m[2]),
			Vector3(m[4], m[5], m[6]),
			Vector3(m[8], m[9], m[10]));
		local_transform.origin = Vector3(m[3], m[7], m[11]);
		create_collision_shape(nif, block->shapeRef, physicsBody, transform * local_transform);
	}
	else if (auto block = nif.GetHeader().GetBlock<nifly::bhkSphereShape>(shapeRef))
	{
		SphereShape3D *sphere_shape = memnew(SphereShape3D);
		sphere_shape->set_radius(block->radius * HAVOK_SCALE);
		CollisionShape3D *collision_shape = memnew(CollisionShape3D);
		collision_shape->set_shape(sphere_shape);
		physicsBody->add_child(collision_shape);
	}
	else if (auto block = nif.GetHeader().GetBlock<nifly::bhkMultiSphereShape>(shapeRef))
	{
		for (auto sphere : block->spheres)
		{
			SphereShape3D *sphere_shape = memnew(SphereShape3D);
			sphere_shape->set_radius(sphere.radius * HAVOK_SCALE);
			CollisionShape3D *collision_shape = memnew(CollisionShape3D);
			Transform3D local_transform;
			local_transform.origin = Vector3(sphere.center.x, sphere.center.z, -sphere.center.y) * HAVOK_SCALE;
			collision_shape->set_transform(transform * local_transform);
			collision_shape->set_shape(sphere_shape);
			physicsBody->add_child(collision_shape);
		}
	}
	else if (auto block = nif.GetHeader().GetBlock<nifly::bhkBoxShape>(shapeRef))
	{
		BoxShape3D *box_shape = memnew(BoxShape3D);
		box_shape->set_size(Vector3(block->dimensions.x, block->dimensions.z, block->dimensions.y) * HAVOK_SCALE * 2.0f);
		CollisionShape3D *collision_shape = memnew(CollisionShape3D);
		collision_shape->set_shape(box_shape);
		physicsBody->add_child(collision_shape);
	}
	else if (auto block = nif.GetHeader().GetBlock<nifly::bhkCapsuleShape>(shapeRef))
	{
		CapsuleShape3D *capsule_shape = memnew(CapsuleShape3D);
		Vector3 point1(block->point1.x, block->point1.z, -block->point1.y);
		Vector3 point2(block->point2.x, block->point2.z, -block->point2.y);
		float height = (point2 - point1).length() * HAVOK_SCALE;
		capsule_shape->set_height(height);
		capsule_shape->set_radius(block->radius * HAVOK_SCALE);
		CollisionShape3D *collision_shape = memnew(CollisionShape3D);
		Transform3D local_transform;
		local_transform.origin = ((point1 + point2) / 2.0f) * HAVOK_SCALE;
		collision_shape->set_transform(transform * local_transform);
		collision_shape->set_shape(capsule_shape);
		physicsBody->add_child(collision_shape);
	}
	else if (auto block = nif.GetHeader().GetBlock<nifly::bhkConvexVerticesShape>(shapeRef))
	{
		PackedVector3Array vertices;
		for (auto vert : block->verts)
		{
			Vector3 position;
			position.x = vert.x * HAVOK_SCALE;
			position.y = vert.z * HAVOK_SCALE;
			position.z = -vert.y * HAVOK_SCALE;
			vertices.append(position);
		}
		if (vertices.size() > 0)
		{
			ConvexPolygonShape3D *convex_shape = memnew(ConvexPolygonShape3D);
			convex_shape->set_points(vertices);
			CollisionShape3D *collision_shape = memnew(CollisionShape3D);
			collision_shape->set_shape(convex_shape);
			collision_shape->set_transform(transform);
			physicsBody->add_child(collision_shape);
		}
	}
	else if (auto block = nif.GetHeader().GetBlock<nifly::bhkNiTriStripsShape>(shapeRef))
	{
		for (auto partRef : block->partRefs)
		{
			auto niTriData = nif.GetHeader().GetBlock<nifly::NiTriStripsData>(partRef);
			if (niTriData)
			{
				PackedVector3Array faces;

				std::vector<nifly::Triangle> tris;
				niTriData->GetTriangles(tris);
				for (auto tri : tris)
				{
					Vector3 ver1;
					ver1.x = niTriData->vertices[tri.p1].x * HAVOK_SCALE;
					ver1.y = niTriData->vertices[tri.p1].z * HAVOK_SCALE;
					ver1.z = -niTriData->vertices[tri.p1].y * HAVOK_SCALE;

					Vector3 ver2;
					ver2.x = niTriData->vertices[tri.p1].x * HAVOK_SCALE;
					ver2.y = niTriData->vertices[tri.p1].z * HAVOK_SCALE;
					ver2.z = -niTriData->vertices[tri.p1].y * HAVOK_SCALE;

					Vector3 ver3;
					ver3.x = niTriData->vertices[tri.p1].x * HAVOK_SCALE;
					ver3.y = niTriData->vertices[tri.p1].z * HAVOK_SCALE;
					ver3.z = -niTriData->vertices[tri.p1].y * HAVOK_SCALE;
					faces.append(ver1);
					faces.append(ver2);
					faces.append(ver3);
				}
				ConcavePolygonShape3D *concave_shape = memnew(ConcavePolygonShape3D);
				concave_shape->set_faces(faces);
				CollisionShape3D *collision_shape = memnew(CollisionShape3D);
				collision_shape->set_shape(concave_shape);
				collision_shape->set_transform(transform);
				physicsBody->add_child(collision_shape);
			}
		}
	}
	else if (auto block = nif.GetHeader().GetBlock<nifly::bhkMoppBvTreeShape>(shapeRef))
	{
		create_collision_shape(nif, block->shapeRef, physicsBody, transform);
	}
	else if (auto block = nif.GetHeader().GetBlock<nifly::bhkPackedNiTriStripsShape>(shapeRef))
	{
		auto data = nif.GetHeader().GetBlock<nifly::hkPackedNiTriStripsData>(block->dataRef);
	}
	else if (auto block = nif.GetHeader().GetBlock<nifly::hkPackedNiTriStripsData>(shapeRef))
	{
	}
	else if (auto block = nif.GetHeader().GetBlock<nifly::bhkCompressedMeshShape>(shapeRef))
	{
		auto shape_data = nif.GetHeader().GetBlock<nifly::bhkCompressedMeshShapeData>(block->dataRef);
		if (shape_data)
		{
			{
				PackedVector3Array big_tris;

				for (auto tri : shape_data->bigTris)
				{
					Vector3 vert1(
						shape_data->bigVerts[tri.triangle1].x,
						shape_data->bigVerts[tri.triangle1].z,
						-shape_data->bigVerts[tri.triangle1].y);

					vert1 /= 1000.0;
					vert1 *= HAVOK_SCALE;

					Vector3 vert2(
						shape_data->bigVerts[tri.triangle2].x,
						shape_data->bigVerts[tri.triangle2].z,
						-shape_data->bigVerts[tri.triangle2].y);

					vert2 /= 1000.0;
					vert2 *= HAVOK_SCALE;

					Vector3 vert3(
						shape_data->bigVerts[tri.triangle3].x,
						shape_data->bigVerts[tri.triangle3].z,
						-shape_data->bigVerts[tri.triangle3].y);

					vert3 /= 1000.0;
					vert3 *= HAVOK_SCALE;

					big_tris.append(vert1);
					big_tris.append(vert2);
					big_tris.append(vert3);
				}

				CollisionShape3D *col_shape = memnew(CollisionShape3D);
				ConcavePolygonShape3D *concave_shape = memnew(ConcavePolygonShape3D);
				concave_shape->set_faces(big_tris);
				col_shape->set_shape(concave_shape);
				col_shape->set_meta("material", shape_data->materialType);
				physicsBody->add_child(col_shape);
			}

			for (auto chunk : shape_data->chunks)
			{
				CollisionShape3D *col_shape = memnew(CollisionShape3D);
				ConcavePolygonShape3D *concave_shape = memnew(ConcavePolygonShape3D);
				PackedVector3Array vertices;
				PackedInt32Array indices;
				PackedVector3Array faces;

				for (int i = 0; i < chunk.verts.size(); i++)
				{
					Vector3 vertex(
						static_cast<float>(chunk.verts[i * 3]),
						static_cast<float>(chunk.verts[i * 3 + 2]),
						static_cast<float>(-chunk.verts[i * 3 + 1]));

					vertex /= 1000.0;
					vertex *= HAVOK_SCALE;
					vertices.append(vertex);
				}

				for (auto index : chunk.indices)
				{
					indices.append(index);
				}

				int offset = 0;

				for (int strip_idx = 0; strip_idx < chunk.strips.size(); strip_idx++)
				{
					for (int idx = 0; idx < chunk.strips[strip_idx] - 2; idx++)
					{
						faces.append(vertices[indices[offset + idx]]);
						faces.append(vertices[indices[offset + idx + 2]]);
						faces.append(vertices[indices[offset + idx + 1]]);
					}

					offset += chunk.strips[strip_idx];
				}

				for (int idx = 0; idx < indices.size() - offset; idx += 3)
				{
					faces.append(vertices[indices[offset + idx]]);
					faces.append(vertices[indices[offset + idx + 2]]);
					faces.append(vertices[indices[offset + idx + 1]]);
				}

				Transform3D transform;
				auto chunk_transform = shape_data->transforms[chunk.transformIndex];

				transform.origin.x = (chunk_transform.translation.x + chunk.translation.x) * HAVOK_SCALE;
				transform.origin.y = (chunk_transform.translation.z + chunk.translation.z) * HAVOK_SCALE;
				transform.origin.z = -(chunk_transform.translation.y + chunk.translation.y) * HAVOK_SCALE;

				Quaternion rotation;
				rotation.x = chunk_transform.rotation.x;
				rotation.y = chunk_transform.rotation.z;
				rotation.z = -chunk_transform.rotation.y;
				rotation.w = chunk_transform.rotation.w;
				transform.basis = Basis(rotation);
				concave_shape->set_faces(faces);
				col_shape->set_shape(concave_shape);
				col_shape->set_transform(transform);
				col_shape->set_meta("material", shape_data->materials[chunk.matIndex].material);
				physicsBody->add_child(col_shape);
			}
		}
	}
	else
	{
		UtilityFunctions::printerr("unknown collision shape: ", block->BlockName);
	}
}

PhysicsBody3D *create_physics_body(nifly::NifFile &nif, nifly::NiCollisionObject *collision, Node3D *parent = nullptr)
{
	PhysicsBody3D *physics_body;
	auto collision_obj = dynamic_cast<nifly::bhkCollisionObject *>(collision);
	if (collision_obj)
	{
		auto bhk_body = nif.GetHeader().GetBlock<nifly::bhkWorldObject>(collision_obj->bodyRef);
		if (bhk_body)
		{
			if (bhk_body->collisionFilter.layer == 8)
			{
				physics_body = memnew(RigidBody3D);
			}
			else
			{
				physics_body = memnew(StaticBody3D);
			}

			create_collision_shape(nif, bhk_body->shapeRef, physics_body);
		}
	}

	return physics_body;
}

Node3D *create_node(nifly::NifFile &nif, nifly::NiNode *node)
{
	Node3D *new_node = memnew(Node3D);
	new_node->set_name(String::utf8(node->name.get().c_str()));
	Transform3D transform;
	nif_to_godot_transform(node->transform, transform);

	for (auto &child : node->childRefs)
	{
		if (auto block = nif.GetHeader().GetBlock<nifly::NiNode>(child))
			new_node->add_child(create_node(nif, block));
		else if (auto block = nif.GetHeader().GetBlock<nifly::BSTriShape>(child))
			new_node->add_child(create_mesh(nif, block));
	}

	if (auto block = nif.GetHeader().GetBlock<nifly::NiCollisionObject>(node->collisionRef))
		new_node->add_child(create_physics_body(nif, block, new_node));
	return new_node;
}

void NifLoader::_bind_methods()
{
	ClassDB::bind_static_method("NifLoader", D_METHOD("load", "data"), &NifLoader::load);
}

Node3D *NifLoader::load(PackedByteArray data)
{
	ByteArrayStream bytestream(reinterpret_cast<const char *>(data.ptr()), data.size());

	nifly::NifFile nif;
	std::istream stream(&bytestream);
	if (int code = nif.Load(stream) != 0)
	{
		UtilityFunctions::printerr("Invalid nif file ", code);
		return nullptr;
	}

	auto root = nif.GetRootNode();
	return create_node(nif, root);
}

void NifLoader::load_skeleton(Node3D *mesh, PackedByteArray skeleton)
{
	ByteArrayStream bytestream(reinterpret_cast<const char *>(skeleton.ptr()), skeleton.size());

	nifly::NifFile skel_nif;
	std::istream stream(&bytestream);
	if (int code = skel_nif.Load(stream) != 0)
	{
		UtilityFunctions::printerr("Invalid nif file ", code);
		return;
	}
}