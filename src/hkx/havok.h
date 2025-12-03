#ifndef HAVOK_H
#define HAVOK_H

#include <map>
#include <string>
#include <functional>

typedef char *hkStringPtr;
typedef unsigned long long uint64;
typedef int int32;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef short int16;
typedef char int8;
typedef unsigned char uint8;
typedef float hkReal;
typedef unsigned int hkFlags;
typedef unsigned short hkHalf;

class hkClass;

struct hkVariant
{
	void *m_object;
	hkClass *m_class;
};

struct hkVector3
{
	float x, y, z;
};

struct hkVector4
{
	float x, y, z, w;
};

struct hkMatrix3
{
	struct hkVector4 col0; // 00
	struct hkVector4 col1; // 10
	struct hkVector4 col2; // 20
};

struct hkMatrix4
{
	struct hkVector4 col0;
	struct hkVector4 col1;
	struct hkVector4 col2;
	struct hkVector4 col3;
};

struct hkMatrix3x4
{
	float cr[3][4];
};

struct hkRotation : hkMatrix3
{
};

struct hkQuaternion
{
	float x, y, z, w;
};

struct hkTransform
{
	struct hkMatrix3x4 rotation;
	struct hkVector4 translation;
};

struct hkQsTransform
{
	struct hkVector4 translation;
	struct hkQuaternion rotation;
	struct hkVector4 scale;
};

template <typename T>
class hkArrayBase
{
public:
	T *data;
	int32 size;
	int32 capacityAndFlags;

	template <typename PtrType>
	class Iterator
	{
	public:
		// Required type aliases
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = PtrType;
		using reference = typename std::iterator_traits<PtrType>::reference;
		using iterator_category = std::forward_iterator_tag;

		Iterator(PtrType ptr) : m_ptr(ptr) {}

		// Operators must work with the generic PtrType
		reference operator*() const { return *m_ptr; }
		pointer operator->() const { return m_ptr; }

		Iterator &operator++()
		{
			m_ptr++;
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator temp = *this;
			m_ptr++;
			return temp;
		}

		friend bool operator==(const Iterator &a, const Iterator &b) { return a.m_ptr == b.m_ptr; }
		friend bool operator!=(const Iterator &a, const Iterator &b) { return a.m_ptr != b.m_ptr; }

		// Optional: Allows conversion from Iterator<T*> to Iterator<const T*>
		operator Iterator<const T *>() const
		{
			return Iterator<const T *>(m_ptr);
		}

	private:
		PtrType m_ptr;
	};

	// Define standard iterator type aliases for the container
	using iterator = Iterator<T *>;
	using const_iterator = Iterator<const T *>;

	// Provide begin/end methods returning the correct iterator types
	iterator begin() { return iterator(data.data()); }
	iterator end() { return iterator(data.data() + data.size()); }

	const_iterator begin() const { return const_iterator(data.data()); }
	const_iterator end() const { return const_iterator(data.data() + data.size()); }
	const_iterator cbegin() const { return const_iterator(data.data()); }
	const_iterator cend() const { return const_iterator(data.data() + data.size()); }
};

template <typename T, typename Allocator = void>
class hkArray : public hkArrayBase<T>
{
};

template <typename T, int N, typename Allocator = void>
class hkInplaceArray : public hkArray<T, Allocator>
{
public:
	T storage[N];
};

class hkBaseObject
{
public:
	virtual ~hkBaseObject();
};

class hkReferencedObject : public hkBaseObject
{
public:
	virtual ~hkReferencedObject();
	uint16 m_memSizeAndFlags;
	int16 m_referenceCount;
};

class hkLocalFrameGroup : public hkReferencedObject
{
public:
	virtual ~hkLocalFrameGroup();
	hkStringPtr m_name;
};

class hkLocalFrame : public hkReferencedObject
{
public:
};

class hkSimpleLocalFrame : public hkLocalFrame
{
public:
	virtual ~hkSimpleLocalFrame();
	hkTransform m_transform;
	hkArray<hkLocalFrame *> m_children;
	hkLocalFrame *m_parentFrame;
	hkLocalFrameGroup *m_group;
	hkStringPtr m_name;
};

class hkTrackerSerializableScanSnapshotAllocation
{
public:
	uint64 m_start;
	uint64 m_size;
	int32 m_traceId;
};

class hkTrackerSerializableScanSnapshotBlock
{
public:
	int32 m_typeIndex;
	uint64 m_start;
	uint64 m_size;
	int32 m_arraySize;
	int32 m_startReferenceIndex;
	int32 m_numReferences;
};

class hkTrackerSerializableScanSnapshot : public hkReferencedObject
{
public:
	virtual ~hkTrackerSerializableScanSnapshot();
	hkArray<hkTrackerSerializableScanSnapshotAllocation> m_allocations;
	hkArray<hkTrackerSerializableScanSnapshotBlock> m_blocks;
	hkArray<int32> m_refs;
	hkArray<uint8> m_typeNames;
	hkArray<uint8> m_traceText;
	hkArray<uint64> m_traceAddrs;
	hkArray<int32> m_traceParents;
};

class hkMonitorStreamColorTableColorPair
{
public:
	hkStringPtr m_colorName;
	uint32 m_color;
};

class hkMonitorStreamColorTable : public hkReferencedObject
{
public:
	virtual ~hkMonitorStreamColorTable();
	hkArray<hkMonitorStreamColorTableColorPair> m_colorPairs;
	uint32 m_defaultColor;
};

class hkResourceBase : public hkReferencedObject
{
public:
};

class hkResourceContainer : public hkResourceBase
{
public:
};

class hkMemoryResourceHandleExternalLink
{
public:
	hkStringPtr m_memberName;
	hkStringPtr m_externalId;
};

class hkResourceHandle : public hkResourceBase
{
public:
};

class hkMemoryResourceHandle : public hkResourceHandle
{
public:
	virtual ~hkMemoryResourceHandle();
	hkReferencedObject *m_variant;
	hkStringPtr m_name;
	hkArray<hkMemoryResourceHandleExternalLink> m_references;
};

class hkMemoryResourceContainer : public hkResourceContainer
{
public:
	virtual ~hkMemoryResourceContainer();
	hkStringPtr m_name;
	hkMemoryResourceContainer *m_parent;
	hkArray<hkMemoryResourceHandle *> m_resourceHandles;
	hkArray<hkMemoryResourceContainer *> m_children;
};

class hkAlignSceneToNodeOptions : public hkReferencedObject
{
public:
	virtual ~hkAlignSceneToNodeOptions();
	bool m_invert;
	bool m_transformPositionX;
	bool m_transformPositionY;
	bool m_transformPositionZ;
	bool m_transformRotation;
	bool m_transformScale;
	bool m_transformSkew;
	int32 m_keyframe;
	hkStringPtr m_nodeName;
};

class hkxAnimatedFloat : public hkReferencedObject
{
public:
	virtual ~hkxAnimatedFloat();
	hkArray<float> m_floats;
	uint8 m_hint;
};

class hkxAnimatedMatrix : public hkReferencedObject
{
public:
	virtual ~hkxAnimatedMatrix();
	hkArray<hkMatrix4> m_matrices;
	uint8 m_hint;
};

class hkxAnimatedQuaternion : public hkReferencedObject
{
public:
	virtual ~hkxAnimatedQuaternion();
	hkArray<hkQuaternion> m_quaternions;
};

class hkxAnimatedVector : public hkReferencedObject
{
public:
	virtual ~hkxAnimatedVector();
	hkArray<hkVector4> m_vectors;
	uint8 m_hint;
};

class hkxAttribute
{
public:
	hkStringPtr m_name;
	hkReferencedObject *m_value;
};

class hkxAttributeGroup
{
public:
	hkStringPtr m_name;
	hkArray<hkxAttribute> m_attributes;
};

class hkxAttributeHolder : public hkReferencedObject
{
public:
	virtual ~hkxAttributeHolder();
	hkArray<hkxAttributeGroup> m_attributeGroups;
};

class hkxCamera : public hkReferencedObject
{
public:
	virtual ~hkxCamera();
	hkVector4 m_from;
	hkVector4 m_focus;
	hkVector4 m_up;
	float m_fov;
	float m_far;
	float m_near;
	bool m_leftHanded;
};

class hkxEdgeSelectionChannel : public hkReferencedObject
{
public:
	virtual ~hkxEdgeSelectionChannel();
	hkArray<int32> m_selectedEdges;
};

class hkxEnumItem
{
public:
	int32 m_value;
	hkStringPtr m_name;
};

class hkxEnum : public hkReferencedObject
{
public:
	virtual ~hkxEnum();
	hkArray<hkxEnumItem> m_items;
};

class hkxEnvironmentVariable
{
public:
	hkStringPtr m_name;
	hkStringPtr m_value;
};

class hkxEnvironment : public hkReferencedObject
{
public:
	virtual ~hkxEnvironment();
	hkArray<hkxEnvironmentVariable> m_variables;
};

class hkxIndexBuffer : public hkReferencedObject
{
public:
	virtual ~hkxIndexBuffer();
	int8 m_indexType;
	hkArray<uint16> m_indices16;
	hkArray<uint32> m_indices32;
	uint32 m_vertexBaseOffset;
	uint32 m_length;
};

class hkxLight : public hkReferencedObject
{
public:
	virtual ~hkxLight();
	int8 m_type;
	hkVector4 m_position;
	hkVector4 m_direction;
	uint32 m_color;
	float m_angle;
};

class hkxMaterialTextureStage
{
public:
	hkReferencedObject *m_texture;
	int32 m_usageHint;
	int32 m_tcoordChannel;
};

class hkxMaterialProperty
{
public:
	uint32 m_key;
	uint32 m_value;
};

class hkxMaterial : public hkxAttributeHolder
{
public:
	virtual ~hkxMaterial();
	hkStringPtr m_name;
	hkArray<hkxMaterialTextureStage> m_stages;
	hkVector4 m_diffuseColor;
	hkVector4 m_ambientColor;
	hkVector4 m_specularColor;
	hkVector4 m_emissiveColor;
	hkArray<hkxMaterial *> m_subMaterials;
	hkReferencedObject *m_extraData;
	hkArray<hkxMaterialProperty> m_properties;
};

class hkxMaterialEffect : public hkReferencedObject
{
public:
	virtual ~hkxMaterialEffect();
	hkStringPtr m_name;
	uint8 m_type;
	hkArray<uint8> m_data;
};

class hkxMaterialShader : public hkReferencedObject
{
public:
	virtual ~hkxMaterialShader();
	hkStringPtr m_name;
	uint8 m_type;
	hkStringPtr m_vertexEntryName;
	hkStringPtr m_geomEntryName;
	hkStringPtr m_pixelEntryName;
	hkArray<uint8> m_data;
};

class hkxMaterialShaderSet : public hkReferencedObject
{
public:
	virtual ~hkxMaterialShaderSet();
	hkArray<hkxMaterialShader *> m_shaders;
};

class hkxMeshUserChannelInfo : public hkxAttributeHolder
{
public:
	virtual ~hkxMeshUserChannelInfo();
	hkStringPtr m_name;
	hkStringPtr m_className;
};

class hkxVertexDescriptionElementDecl
{
public:
	uint32 m_byteOffset;
	uint16 m_type;
	uint16 m_usage;
	uint32 m_byteStride;
	uint8 m_numElements;
};

class hkxVertexDescription
{
public:
	hkArray<hkxVertexDescriptionElementDecl> m_decls;
};

class hkxVertexBufferVertexData
{
public:
	hkArray<hkVector4> m_vectorData;
	hkArray<float> m_floatData;
	hkArray<uint32> m_uint32Data;
	hkArray<uint16> m_uint16Data;
	hkArray<uint8> m_uint8Data;
	uint32 m_numVerts;
	uint32 m_vectorStride;
	uint32 m_floatStride;
	uint32 m_uint32Stride;
	uint32 m_uint16Stride;
	uint32 m_uint8Stride;
};

class hkxVertexBuffer : public hkReferencedObject
{
public:
	virtual ~hkxVertexBuffer();
	hkxVertexBufferVertexData m_data;
	hkxVertexDescription m_desc;
};

class hkxMeshSection : public hkReferencedObject
{
public:
	virtual ~hkxMeshSection();
	hkxVertexBuffer *m_vertexBuffer;
	hkArray<hkxIndexBuffer *> m_indexBuffers;
	hkxMaterial *m_material;
	hkArray<hkReferencedObject *> m_userChannels;
};

class hkxMesh : public hkReferencedObject
{
public:
	virtual ~hkxMesh();
	hkArray<hkxMeshSection *> m_sections;
	hkArray<hkxMeshUserChannelInfo *> m_userChannelInfos;
};

class hkxNodeAnnotationData
{
public:
	float m_time;
	hkStringPtr m_description;
};

class hkxNode : public hkxAttributeHolder
{
public:
	virtual ~hkxNode();
	hkStringPtr m_name;
	hkReferencedObject *m_object;
	hkArray<hkMatrix4> m_keyFrames;
	hkArray<hkxNode *> m_children;
	hkArray<hkxNodeAnnotationData> m_annotations;
	hkStringPtr m_userProperties;
	bool m_selected;
};

class hkxNodeSelectionSet : public hkxAttributeHolder
{
public:
	virtual ~hkxNodeSelectionSet();
	hkArray<hkxNode *> m_selectedNodes;
	hkStringPtr m_name;
};

class hkxSkinBinding : public hkReferencedObject
{
public:
	virtual ~hkxSkinBinding();
	hkxMesh *m_mesh;
	hkArray<hkStringPtr> m_nodeNames;
	hkArray<hkMatrix4> m_bindPose;
	hkMatrix4 m_initSkinTransform;
};

class hkxTextureFile : public hkReferencedObject
{
public:
	virtual ~hkxTextureFile();
	hkStringPtr m_filename;
	hkStringPtr m_name;
	hkStringPtr m_originalFilename;
};

class hkxTextureInplace : public hkReferencedObject
{
public:
	virtual ~hkxTextureInplace();
	char m_fileType;
	hkArray<uint8> m_data;
	hkStringPtr m_name;
	hkStringPtr m_originalFilename;
};

class hkxScene : public hkReferencedObject
{
public:
	virtual ~hkxScene();
	hkStringPtr m_modeller;
	hkStringPtr m_asset;
	float m_sceneLength;
	hkxNode *m_rootNode;
	hkArray<hkxNodeSelectionSet *> m_selectionSets;
	hkArray<hkxCamera *> m_cameras;
	hkArray<hkxLight *> m_lights;
	hkArray<hkxMesh *> m_meshes;
	hkArray<hkxMaterial *> m_materials;
	hkArray<hkxTextureInplace *> m_inplaceTextures;
	hkArray<hkxTextureFile *> m_externalTextures;
	hkArray<hkxSkinBinding *> m_skinBindings;
	hkMatrix3 m_appliedTransform;
};

class hkxSparselyAnimatedBool : public hkReferencedObject
{
public:
	virtual ~hkxSparselyAnimatedBool();
	hkArray<bool> m_bools;
	hkArray<float> m_times;
};

class hkxSparselyAnimatedInt : public hkReferencedObject
{
public:
	virtual ~hkxSparselyAnimatedInt();
	hkArray<int32> m_ints;
	hkArray<float> m_times;
};

class hkxSparselyAnimatedEnum : public hkxSparselyAnimatedInt
{
public:
	virtual ~hkxSparselyAnimatedEnum();
	hkxEnum *m_enum;
};

class hkxSparselyAnimatedString : public hkReferencedObject
{
public:
	virtual ~hkxSparselyAnimatedString();
	hkArray<hkStringPtr> m_strings;
	hkArray<float> m_times;
};

class hkxTriangleSelectionChannel : public hkReferencedObject
{
public:
	virtual ~hkxTriangleSelectionChannel();
	hkArray<int32> m_selectedTriangles;
};

class hkxVertexFloatDataChannel : public hkReferencedObject
{
public:
	virtual ~hkxVertexFloatDataChannel();
	hkArray<float> m_perVertexFloats;
	uint8 m_dimensions;
};

class hkxVertexIntDataChannel : public hkReferencedObject
{
public:
	virtual ~hkxVertexIntDataChannel();
	hkArray<int32> m_perVertexInts;
};

class hkxVertexSelectionChannel : public hkReferencedObject
{
public:
	virtual ~hkxVertexSelectionChannel();
	hkArray<int32> m_selectedVertices;
};

class hkxVertexVectorDataChannel : public hkReferencedObject
{
public:
	virtual ~hkxVertexVectorDataChannel();
	hkArray<hkVector4> m_perVertexVectors;
};

class hkMeshBoneIndexMapping
{
public:
	hkArray<int16> m_mapping;
};

class hkIndexedTransformSet : public hkReferencedObject
{
public:
	virtual ~hkIndexedTransformSet();
	hkArray<hkMatrix4> m_matrices;
	hkArray<hkMatrix4> m_inverseMatrices;
	hkArray<int16> m_matricesOrder;
	hkArray<hkStringPtr> m_matricesNames;
	hkArray<hkMeshBoneIndexMapping> m_indexMappings;
	bool m_allMatricesAreAffine;
};

class hkMeshVertexBuffer : public hkReferencedObject
{
public:
};

class hkMeshShape : public hkReferencedObject
{
public:
};

class hkMeshBody : public hkReferencedObject
{
public:
};

class hkMemoryMeshBody : public hkMeshBody
{
public:
	virtual ~hkMemoryMeshBody();
	hkMatrix4 m_transform;
	hkIndexedTransformSet *m_transformSet;
	hkMeshShape *m_shape;
	hkArray<hkMeshVertexBuffer *> m_vertexBuffers;
	hkStringPtr m_name;
};

class hkMeshTexture : public hkReferencedObject
{
public:
};

class hkMeshMaterial : public hkReferencedObject
{
public:
};

class hkMemoryMeshMaterial : public hkMeshMaterial
{
public:
	virtual ~hkMemoryMeshMaterial();
	hkStringPtr m_materialName;
	hkArray<hkMeshTexture *> m_textures;
};

class hkMeshSectionCinfo
{
public:
	hkMeshVertexBuffer *m_vertexBuffer;
	hkMeshMaterial *m_material;
	uint8 m_primitiveType;
	int32 m_numPrimitives;
	uint8 m_indexType;
	void *m_indices;
	int32 m_vertexStartIndex;
	int32 m_transformIndex;
};

class hkMemoryMeshShape : public hkMeshShape
{
public:
	virtual ~hkMemoryMeshShape();
	hkArray<hkMeshSectionCinfo> m_sections;
	hkArray<uint16> m_indices16;
	hkArray<uint32> m_indices32;
	hkStringPtr m_name;
};

class hkMemoryMeshTexture : public hkMeshTexture
{
public:
	virtual ~hkMemoryMeshTexture();
	hkStringPtr m_filename;
	hkArray<uint8> m_data;
	int8 m_format;
	bool m_hasMipMaps;
	int8 m_filterMode;
	int8 m_usageHint;
	int32 m_textureCoordChannel;
};

class hkVertexFormatElement
{
public:
	uint8 m_dataType;
	uint8 m_numValues;
	uint8 m_usage;
	uint8 m_subUsage;
	hkFlags m_flags;
	uint8 m_pad;
};

class hkVertexFormat
{
public:
	hkVertexFormatElement m_elements;
	int32 m_numElements;
};

class hkMemoryMeshVertexBuffer : public hkMeshVertexBuffer
{
public:
	virtual ~hkMemoryMeshVertexBuffer();
	hkVertexFormat m_format;
	int32 m_elementOffsets;
	hkArray<uint8> m_memory;
	int32 m_vertexStride;
	bool m_locked;
	int32 m_numVertices;
	bool m_isBigEndian;
	bool m_isSharable;
};

class hkMultipleVertexBufferElementInfo
{
public:
	uint8 m_vertexBufferIndex;
	uint8 m_elementIndex;
};

class hkMultipleVertexBufferVertexBufferInfo
{
public:
	hkMeshVertexBuffer *m_vertexBuffer;
	void *m_lockedVertices;
	bool m_isLocked;
};

class hkMultipleVertexBufferLockedElement
{
public:
	uint8 m_vertexBufferIndex;
	uint8 m_elementIndex;
	uint8 m_lockedBufferIndex;
	uint8 m_vertexFormatIndex;
	uint8 m_lockFlags;
	uint8 m_outputBufferIndex;
	int8 m_emulatedIndex;
};

class hkMultipleVertexBuffer : public hkMeshVertexBuffer
{
public:
	virtual ~hkMultipleVertexBuffer();
	hkVertexFormat m_vertexFormat;
	hkArray<hkMultipleVertexBufferLockedElement> m_lockedElements;
	hkMemoryMeshVertexBuffer *m_lockedBuffer;
	hkArray<hkMultipleVertexBufferElementInfo> m_elementInfos;
	hkArray<hkMultipleVertexBufferVertexBufferInfo> m_vertexBufferInfos;
	int32 m_numVertices;
	bool m_isLocked;
	uint32 m_updateCount;
	bool m_writeLock;
	bool m_isSharable;
	bool m_constructionComplete;
};

class hkpShape : public hkReferencedObject
{
public:
	uint64 m_userData;
	uint32 m_type;
};

class hkpSphereRepShape : public hkpShape
{
public:
};

class hkpConvexShape : public hkpSphereRepShape
{
public:
	float m_radius;
};

class hkpBoxShape : public hkpConvexShape
{
public:
	virtual ~hkpBoxShape();
	hkVector4 m_halfExtents;
};

class hkpShapeContainer
{
public:
};

class hkpSingleShapeContainer : public hkpShapeContainer
{
public:
	virtual ~hkpSingleShapeContainer();
	hkpShape *m_childShape;
};

class hkpBvShape : public hkpShape
{
public:
	virtual ~hkpBvShape();
	hkpShape *m_boundingVolumeShape;
	hkpSingleShapeContainer m_childShape;
};

class hkpCapsuleShape : public hkpConvexShape
{
public:
	virtual ~hkpCapsuleShape();
	hkVector4 m_vertexA;
	hkVector4 m_vertexB;
};

class hkpCollisionFilter : public hkReferencedObject
{
public:
	uint32 m_prepad;
	uint32 m_type;
	uint32 m_postpad;
};

class hkpCollisionFilterList : public hkpCollisionFilter
{
public:
	virtual ~hkpCollisionFilterList();
	hkArray<hkpCollisionFilter *> m_collisionFilters;
};

class hkpConvexListShape : public hkpConvexShape
{
public:
	virtual ~hkpConvexListShape();
	float m_minDistanceToUseConvexHullForGetClosestPoints;
	hkVector4 m_aabbHalfExtents;
	hkVector4 m_aabbCenter;
	bool m_useCachedAabb;
	hkArray<hkpConvexShape *> m_childShapes;
};

class hkpConvexTransformShapeBase : public hkpConvexShape
{
public:
	hkpSingleShapeContainer m_childShape;
	int32 m_childShapeSize;
};

class hkpConvexTransformShape : public hkpConvexTransformShapeBase
{
public:
	virtual ~hkpConvexTransformShape();
	hkTransform m_transform;
};

class hkpConvexTranslateShape : public hkpConvexTransformShapeBase
{
public:
	virtual ~hkpConvexTranslateShape();
	hkVector4 m_translation;
};

class hkpConvexVerticesConnectivity : public hkReferencedObject
{
public:
	virtual ~hkpConvexVerticesConnectivity();
	hkArray<uint16> m_vertexIndices;
	hkArray<uint8> m_numVerticesPerFace;
};

class hkpCylinderShape : public hkpConvexShape
{
public:
	virtual ~hkpCylinderShape();
	float m_cylRadius;
	float m_cylBaseRadiusFactorForHeightFieldCollisions;
	hkVector4 m_vertexA;
	hkVector4 m_vertexB;
	hkVector4 m_perpendicular1;
	hkVector4 m_perpendicular2;
};

class hkpConvexListFilter : public hkReferencedObject
{
public:
};

class hkpDefaultConvexListFilter : public hkpConvexListFilter
{
public:
	virtual ~hkpDefaultConvexListFilter();
};

class hkpGroupFilter : public hkpCollisionFilter
{
public:
	virtual ~hkpGroupFilter();
	int32 m_nextFreeSystemGroup;
	uint32 m_collisionLookupTable;
	hkVector4 m_pad256;
};

class hkpListShapeChildInfo
{
public:
	hkpShape *m_shape;
	uint32 m_collisionFilterInfo;
	int32 m_shapeSize;
	int32 m_numChildShapes;
};

class hkpShapeCollection : public hkpShape
{
public:
	bool m_disableWelding;
	uint8 m_collectionType;
};

class hkpListShape : public hkpShapeCollection
{
public:
	virtual ~hkpListShape();
	hkArray<hkpListShapeChildInfo> m_childInfo;
	uint16 m_flags;
	uint16 m_numDisabledChildren;
	hkVector4 m_aabbHalfExtents;
	hkVector4 m_aabbCenter;
	uint32 m_enabledChildren;
};

class hkpMoppCodeCodeInfo
{
public:
	hkVector4 m_offset;
};

class hkpMoppCode : public hkReferencedObject
{
public:
	virtual ~hkpMoppCode();
	hkpMoppCodeCodeInfo m_info;
	hkArray<uint8> m_data;
	int8 m_buildType;
};

class hkpBvTreeShape : public hkpShape
{
public:
	uint8 m_bvTreeType;
};

class hkMoppBvTreeShapeBase : public hkpBvTreeShape
{
public:
	hkpMoppCode *m_code;
	void *m_moppData;
	uint32 m_moppDataSize;
	hkVector4 m_codeInfoCopy;
};

class hkpMoppBvTreeShape : public hkMoppBvTreeShapeBase
{
public:
	virtual ~hkpMoppBvTreeShape();
	hkpSingleShapeContainer m_child;
	int32 m_childSize;
};

class hkpMultiRayShapeRay
{
public:
	hkVector4 m_start;
	hkVector4 m_end;
};

class hkpMultiRayShape : public hkpShape
{
public:
	virtual ~hkpMultiRayShape();
	hkArray<hkpMultiRayShapeRay> m_rays;
	float m_rayPenetrationDistance;
};

class hkpMultiSphereShape : public hkpSphereRepShape
{
public:
	virtual ~hkpMultiSphereShape();
	int32 m_numSpheres;
	hkVector4 m_spheres;
};

class hkpNullCollisionFilter : public hkpCollisionFilter
{
public:
	virtual ~hkpNullCollisionFilter();
};

class hkpRemoveTerminalsMoppModifier : public hkReferencedObject
{
public:
	virtual ~hkpRemoveTerminalsMoppModifier();
	hkArray<uint32> m_removeInfo;
	void *m_tempShapesToRemove;
};

class hkpShapeInfo : public hkReferencedObject
{
public:
	virtual ~hkpShapeInfo();
	hkpShape *m_shape;
	bool m_isHierarchicalCompound;
	bool m_hkdShapesCollected;
	hkArray<hkStringPtr> m_childShapeNames;
	hkArray<hkTransform> m_childTransforms;
	hkTransform m_transform;
};

class hkpSphereShape : public hkpConvexShape
{
public:
	virtual ~hkpSphereShape();
	uint32 m_pad16;
};

class hkpTransformShape : public hkpShape
{
public:
	virtual ~hkpTransformShape();
	hkpSingleShapeContainer m_childShape;
	int32 m_childShapeSize;
	hkQuaternion m_rotation;
	hkTransform m_transform;
};

class hkpTriangleShape : public hkpConvexShape
{
public:
	virtual ~hkpTriangleShape();
	uint16 m_weldingInfo;
	uint8 m_weldingType;
	uint8 m_isExtruded;
	hkVector4 m_vertexA;
	hkVector4 m_vertexB;
	hkVector4 m_vertexC;
	hkVector4 m_extrusion;
};

class hkAabb
{
public:
	hkVector4 m_min;
	hkVector4 m_max;
};

class hkpCompressedMeshShapeChunk
{
public:
	hkVector4 m_offset;
	hkArray<uint16> m_vertices;
	hkArray<uint16> m_indices;
	hkArray<uint16> m_stripLengths;
	hkArray<uint16> m_weldingInfo;
	uint32 m_materialInfo;
	uint16 m_reference;
	uint16 m_transformIndex;
};

class hkpCompressedMeshShapeConvexPiece
{
public:
	hkVector4 m_offset;
	hkArray<uint16> m_vertices;
	hkArray<uint8> m_faceVertices;
	hkArray<uint16> m_faceOffsets;
	uint16 m_reference;
	uint16 m_transformIndex;
};

class hkpMeshMaterial
{
public:
	uint32 m_filterInfo;
};

class hkpNamedMeshMaterial : public hkpMeshMaterial
{
public:
	hkStringPtr m_name;
};

class hkpCompressedMeshShapeBigTriangle
{
public:
	uint16 m_a;
	uint16 m_b;
	uint16 m_c;
	uint32 m_material;
	uint16 m_weldingInfo;
	uint16 m_transformIndex;
};

class hkpCompressedMeshShape : public hkpShapeCollection
{
public:
	virtual ~hkpCompressedMeshShape();
	int32 m_bitsPerIndex;
	int32 m_bitsPerWIndex;
	int32 m_wIndexMask;
	int32 m_indexMask;
	float m_radius;
	uint8 m_weldingType;
	uint8 m_materialType;
	hkArray<uint32> m_materials;
	hkArray<uint16> m_materials16;
	hkArray<uint8> m_materials8;
	hkArray<hkQsTransform> m_transforms;
	hkArray<hkVector4> m_bigVertices;
	hkArray<hkpCompressedMeshShapeBigTriangle> m_bigTriangles;
	hkArray<hkpCompressedMeshShapeChunk> m_chunks;
	hkArray<hkpCompressedMeshShapeConvexPiece> m_convexPieces;
	float m_error;
	hkAabb m_bounds;
	uint32 m_defaultCollisionFilterInfo;
	void *m_meshMaterials;
	uint16 m_materialStriding;
	uint16 m_numMaterials;
	hkArray<hkpNamedMeshMaterial> m_namedMaterials;
};

class hkpConvexVerticesShapeFourVectors
{
public:
	hkVector4 m_x;
	hkVector4 m_y;
	hkVector4 m_z;
};

class hkpConvexVerticesShape : public hkpConvexShape
{
public:
	virtual ~hkpConvexVerticesShape();
	hkVector4 m_aabbHalfExtents;
	hkVector4 m_aabbCenter;
	hkArray<hkpConvexVerticesShapeFourVectors> m_rotatedVertices;
	int32 m_numVertices;
	void *m_externalObject;
	void *m_getFaceNormals;
	hkArray<hkVector4> m_planeEquations;
	hkpConvexVerticesConnectivity *m_connectivity;
};

class hkpConvexPieceStreamData : public hkReferencedObject
{
public:
	virtual ~hkpConvexPieceStreamData();
	hkArray<uint32> m_convexPieceStream;
	hkArray<uint32> m_convexPieceOffsets;
	hkArray<uint32> m_convexPieceSingleTriangles;
};

class hkpConvexPieceMeshShape : public hkpShapeCollection
{
public:
	virtual ~hkpConvexPieceMeshShape();
	hkpConvexPieceStreamData *m_convexPieceStream;
	hkpShapeCollection *m_displayMesh;
	float m_radius;
};

class hkpExtendedMeshShapeSubpart
{
public:
	int8 m_type;
	int8 m_materialIndexStridingType;
	int16 m_materialStriding;
	void *m_materialIndexBase;
	uint16 m_materialIndexStriding;
	uint16 m_numMaterials;
	void *m_materialBase;
	uint64 m_userData;
};

class hkpExtendedMeshShapeShapesSubpart : public hkpExtendedMeshShapeSubpart
{
public:
	hkArray<hkpConvexShape *> m_childShapes;
	hkQuaternion m_rotation;
	hkVector4 m_translation;
};

class hkpExtendedMeshShapeTrianglesSubpart : public hkpExtendedMeshShapeSubpart
{
public:
	int32 m_numTriangleShapes;
	void *m_vertexBase;
	int32 m_numVertices;
	void *m_indexBase;
	uint16 m_vertexStriding;
	int32 m_triangleOffset;
	uint16 m_indexStriding;
	int8 m_stridingType;
	int8 m_flipAlternateTriangles;
	hkVector4 m_extrusion;
	hkQsTransform m_transform;
};

class hkpExtendedMeshShape : public hkpShapeCollection
{
public:
	virtual ~hkpExtendedMeshShape();
	hkpExtendedMeshShapeTrianglesSubpart m_embeddedTrianglesSubpart;
	hkVector4 m_aabbHalfExtents;
	hkVector4 m_aabbCenter;
	void *m_materialClass;
	int32 m_numBitsForSubpartIndex;
	hkArray<hkpExtendedMeshShapeTrianglesSubpart> m_trianglesSubparts;
	hkArray<hkpExtendedMeshShapeShapesSubpart> m_shapesSubparts;
	hkArray<uint16> m_weldingInfo;
	uint8 m_weldingType;
	uint32 m_defaultCollisionFilterInfo;
	int32 m_cachedNumChildShapes;
	float m_triangleRadius;
	int32 m_padding;
};

class hkpStorageExtendedMeshShapeMaterial : public hkpMeshMaterial
{
public:
	hkHalf m_restitution;
	hkHalf m_friction;
	uint64 m_userData;
};

class hkpStorageExtendedMeshShapeShapeSubpartStorage : public hkReferencedObject
{
public:
	virtual ~hkpStorageExtendedMeshShapeShapeSubpartStorage();
	hkArray<uint8> m_materialIndices;
	hkArray<hkpStorageExtendedMeshShapeMaterial> m_materials;
	hkArray<uint16> m_materialIndices16;
};

class hkpStorageExtendedMeshShapeMeshSubpartStorage : public hkReferencedObject
{
public:
	virtual ~hkpStorageExtendedMeshShapeMeshSubpartStorage();
	hkArray<hkVector4> m_vertices;
	hkArray<uint8> m_indices8;
	hkArray<uint16> m_indices16;
	hkArray<uint32> m_indices32;
	hkArray<uint8> m_materialIndices;
	hkArray<hkpStorageExtendedMeshShapeMaterial> m_materials;
	hkArray<hkpNamedMeshMaterial> m_namedMaterials;
	hkArray<uint16> m_materialIndices16;
};

class hkpStorageExtendedMeshShape : public hkpExtendedMeshShape
{
public:
	virtual ~hkpStorageExtendedMeshShape();
	hkArray<hkpStorageExtendedMeshShapeMeshSubpartStorage *> m_meshstorage;
	hkArray<hkpStorageExtendedMeshShapeShapeSubpartStorage *> m_shapestorage;
};

class hkpHeightFieldShape : public hkpShape
{
public:
};

class hkpSampledHeightFieldShape : public hkpHeightFieldShape
{
public:
	int32 m_xRes;
	int32 m_zRes;
	float m_heightCenter;
	bool m_useProjectionBasedHeight;
	uint8 m_heightfieldType;
	hkVector4 m_intToFloatScale;
	hkVector4 m_floatToIntScale;
	hkVector4 m_floatToIntOffsetFloorCorrected;
	hkVector4 m_extents;
};

class hkpCompressedSampledHeightFieldShape : public hkpSampledHeightFieldShape
{
public:
	virtual ~hkpCompressedSampledHeightFieldShape();
	hkArray<uint16> m_storage;
	bool m_triangleFlip;
	float m_offset;
	float m_scale;
};

class hkpPlaneShape : public hkpHeightFieldShape
{
public:
	virtual ~hkpPlaneShape();
	hkVector4 m_plane;
	hkVector4 m_aabbCenter;
	hkVector4 m_aabbHalfExtents;
};

class hkpStorageSampledHeightFieldShape : public hkpSampledHeightFieldShape
{
public:
	virtual ~hkpStorageSampledHeightFieldShape();
	hkArray<float> m_storage;
	bool m_triangleFlip;
};

class hkpTriSampledHeightFieldBvTreeShape : public hkpBvTreeShape
{
public:
	virtual ~hkpTriSampledHeightFieldBvTreeShape();
	hkpSingleShapeContainer m_childContainer;
	int32 m_childSize;
	bool m_wantAabbRejectionTest;
	uint8 m_padding;
};

class hkpTriSampledHeightFieldCollection : public hkpShapeCollection
{
public:
	virtual ~hkpTriSampledHeightFieldCollection();
	hkpSampledHeightFieldShape *m_heightfield;
	int32 m_childSize;
	float m_radius;
	hkArray<uint16> m_weldingInfo;
	hkVector4 m_triangleExtrusion;
};

class hkpMeshShapeSubpart
{
public:
	void *m_vertexBase;
	int32 m_vertexStriding;
	int32 m_numVertices;
	void *m_indexBase;
	int8 m_stridingType;
	int8 m_materialIndexStridingType;
	int32 m_indexStriding;
	int32 m_flipAlternateTriangles;
	int32 m_numTriangles;
	void *m_materialIndexBase;
	int32 m_materialIndexStriding;
	void *m_materialBase;
	int32 m_materialStriding;
	int32 m_numMaterials;
	int32 m_triangleOffset;
};

class hkpMeshShape : public hkpShapeCollection
{
public:
	virtual ~hkpMeshShape();
	hkVector4 m_scaling;
	int32 m_numBitsForSubpartIndex;
	hkArray<hkpMeshShapeSubpart> m_subparts;
	hkArray<uint16> m_weldingInfo;
	uint8 m_weldingType;
	float m_radius;
	int32 m_pad;
};

class hkpFastMeshShape : public hkpMeshShape
{
public:
	virtual ~hkpFastMeshShape();
};

class hkpStorageMeshShapeSubpartStorage : public hkReferencedObject
{
public:
	virtual ~hkpStorageMeshShapeSubpartStorage();
	hkArray<float> m_vertices;
	hkArray<uint16> m_indices16;
	hkArray<uint32> m_indices32;
	hkArray<uint8> m_materialIndices;
	hkArray<uint32> m_materials;
	hkArray<uint16> m_materialIndices16;
};

class hkpStorageMeshShape : public hkpMeshShape
{
public:
	virtual ~hkpStorageMeshShape();
	hkArray<hkpStorageMeshShapeSubpartStorage *> m_storage;
};

class hkpSimpleMeshShapeTriangle
{
public:
	int32 m_a;
	int32 m_b;
	int32 m_c;
	uint16 m_weldingInfo;
};

class hkpSimpleMeshShape : public hkpShapeCollection
{
public:
	virtual ~hkpSimpleMeshShape();
	hkArray<hkVector4> m_vertices;
	hkArray<hkpSimpleMeshShapeTriangle> m_triangles;
	hkArray<uint8> m_materialIndices;
	float m_radius;
	uint8 m_weldingType;
};

class hkMultiThreadCheck
{
public:
	uint32 m_threadId;
	int32 m_stackTraceId;
	uint16 m_markCount;
	uint16 m_markBitStack;
};

class hkpCollidableBoundingVolumeData
{
public:
	uint32 m_min;
	uint8 m_expansionMin;
	uint8 m_expansionShift;
	uint32 m_max;
	uint8 m_expansionMax;
	uint8 m_padding;
	uint16 m_numChildShapeAabbs;
	uint16 m_capacityChildShapeAabbs;
	void *m_childShapeAabbs;
	void *m_childShapeKeys;
};

class hkpCdBody
{
public:
	hkpShape *m_shape;
	uint32 m_shapeKey;
	void *m_motion;
	hkpCdBody *m_parent;
};

class hkpBroadPhaseHandle
{
public:
	uint32 m_id;
};

class hkpTypedBroadPhaseHandle : public hkpBroadPhaseHandle
{
public:
	int8 m_type;
	int8 m_ownerOffset;
	int8 m_objectQualityType;
	uint32 m_collisionFilterInfo;
};

class hkpCollidable : public hkpCdBody
{
public:
	int8 m_ownerOffset;
	uint8 m_forceCollideOntoPpu;
	uint16 m_shapeSizeOnSpu;
	hkpTypedBroadPhaseHandle m_broadPhaseHandle;
	hkpCollidableBoundingVolumeData m_boundingVolumeData;
	float m_allowedPenetrationDepth;
};

class hkpLinkedCollidable : public hkpCollidable
{
public:
	hkArray<void> m_collisionEntries;
};

class hkpPropertyValue
{
public:
	uint64 m_data;
};

class hkpProperty
{
public:
	uint32 m_key;
	uint32 m_alignmentPadding;
	hkpPropertyValue m_value;
};

class hkpWorldObject : public hkReferencedObject
{
public:
	void *m_world;
	uint64 m_userData;
	hkpLinkedCollidable m_collidable;
	hkMultiThreadCheck m_multiThreadCheck;
	hkStringPtr m_name;
	hkArray<hkpProperty> m_properties;
	void *m_treeData;
};

class hkpPhantom : public hkpWorldObject
{
public:
	hkArray<void *> m_overlapListeners;
	hkArray<void *> m_phantomListeners;
};

class hkpAabbPhantom : public hkpPhantom
{
public:
	virtual ~hkpAabbPhantom();
	hkAabb m_aabb;
	hkArray<void *> m_overlappingCollidables;
	bool m_orderDirty;
};

class hkpConstraintData : public hkReferencedObject
{
public:
	uint64 m_userData;
};

class hkpConstraintAtom
{
public:
	uint16 m_type;
};

class hkpSetupStabilizationAtom : public hkpConstraintAtom
{
public:
	bool m_enabled;
	float m_maxAngle;
	uint8 m_padding;
};

class hkpBallSocketConstraintAtom : public hkpConstraintAtom
{
public:
	uint8 m_solvingMethod;
	uint8 m_bodiesToNotify;
	uint8 m_velocityStabilizationFactor;
	float m_maxImpulse;
	float m_inertiaStabilizationFactor;
};

class hkpSetLocalTranslationsConstraintAtom : public hkpConstraintAtom
{
public:
	hkVector4 m_translationA;
	hkVector4 m_translationB;
};

class hkpBallAndSocketConstraintDataAtoms
{
public:
	hkpSetLocalTranslationsConstraintAtom m_pivots;
	hkpSetupStabilizationAtom m_setupStabilization;
	hkpBallSocketConstraintAtom m_ballSocket;
};

class hkpBallAndSocketConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpBallAndSocketConstraintData();
	hkpBallAndSocketConstraintDataAtoms m_atoms;
};

class hkpBridgeConstraintAtom : public hkpConstraintAtom
{
public:
	void *m_buildJacobianFunc;
	hkpConstraintData *m_constraintData;
};

class hkpBridgeAtoms
{
public:
	hkpBridgeConstraintAtom m_bridgeAtom;
};

class hkpConstraintChainData : public hkpConstraintData
{
public:
};

class hkpBallSocketChainDataConstraintInfo
{
public:
	hkVector4 m_pivotInA;
	hkVector4 m_pivotInB;
};

class hkpBallSocketChainData : public hkpConstraintChainData
{
public:
	virtual ~hkpBallSocketChainData();
	hkpBridgeAtoms m_atoms;
	hkArray<hkpBallSocketChainDataConstraintInfo> m_infos;
	float m_tau;
	float m_damping;
	float m_cfm;
	float m_maxErrorDistance;
};

class hkSweptTransform
{
public:
	hkVector4 m_centerOfMass0;
	hkVector4 m_centerOfMass1;
	hkQuaternion m_rotation0;
	hkQuaternion m_rotation1;
	hkVector4 m_centerOfMassLocal;
};

class hkMotionState
{
public:
	hkTransform m_transform;
	hkSweptTransform m_sweptTransform;
	hkVector4 m_deltaAngle;
	float m_objectRadius;
	hkHalf m_linearDamping;
	hkHalf m_angularDamping;
	hkHalf m_timeFactor;
	uint8 m_maxLinearVelocity;
	uint8 m_maxAngularVelocity;
	uint8 m_deactivationClass;
};

class hkpMotion : public hkReferencedObject
{
public:
	uint8 m_type;
	uint8 m_deactivationIntegrateCounter;
	uint16 m_deactivationNumInactiveFrames;
	hkMotionState m_motionState;
	hkVector4 m_inertiaAndMassInv;
	hkVector4 m_linearVelocity;
	hkVector4 m_angularVelocity;
	hkVector4 m_deactivationRefPosition;
	uint32 m_deactivationRefOrientation;
	class hkpMaxSizeMotion *m_savedMotion;
	uint16 m_savedQualityTypeIndex;
	hkHalf m_gravityFactor;
};

class hkpKeyframedRigidMotion : public hkpMotion
{
public:
	virtual ~hkpKeyframedRigidMotion();
};

class hkpMaxSizeMotion : public hkpKeyframedRigidMotion
{
public:
	virtual ~hkpMaxSizeMotion();
};

class hkpBoxMotion : public hkpMotion
{
public:
	virtual ~hkpBoxMotion();
};

class hkpBreakableConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpBreakableConstraintData();
	hkpBridgeAtoms m_atoms;
	hkpConstraintData *m_constraintData;
	uint16 m_childRuntimeSize;
	uint16 m_childNumSolverResults;
	float m_solverResultLimit;
	bool m_removeWhenBroken;
	bool m_revertBackVelocityOnBreak;
};

class hkpShapePhantom : public hkpPhantom
{
public:
	hkMotionState m_motionState;
};

class hkpCachingShapePhantom : public hkpShapePhantom
{
public:
	virtual ~hkpCachingShapePhantom();
	hkArray<void> m_collisionDetails;
	bool m_orderDirty;
};

class hkpConstraintMotor : public hkReferencedObject
{
public:
	int8 m_type;
};

class hkpLimitedForceConstraintMotor : public hkpConstraintMotor
{
public:
	float m_minForce;
	float m_maxForce;
};

class hkpCallbackConstraintMotor : public hkpLimitedForceConstraintMotor
{
public:
	virtual ~hkpCallbackConstraintMotor();
	void *m_callbackFunc;
	uint32 m_callbackType;
	uint64 m_userData0;
	uint64 m_userData1;
	uint64 m_userData2;
};

class hkpCharacterMotion : public hkpMotion
{
public:
	virtual ~hkpCharacterMotion();
};

class hkpSetLocalTransformsConstraintAtom : public hkpConstraintAtom
{
public:
	hkTransform m_transformA;
	hkTransform m_transformB;
};

class hkpCogWheelConstraintAtom : public hkpConstraintAtom
{
public:
	float m_cogWheelRadiusA;
	float m_cogWheelRadiusB;
	bool m_isScrew;
	int8 m_memOffsetToInitialAngleOffset;
	int8 m_memOffsetToPrevAngle;
	int8 m_memOffsetToRevolutionCounter;
};

class hkpCogWheelConstraintDataAtoms
{
public:
	hkpSetLocalTransformsConstraintAtom m_transforms;
	hkpCogWheelConstraintAtom m_cogWheels;
};

class hkpCogWheelConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpCogWheelConstraintData();
	hkpCogWheelConstraintDataAtoms m_atoms;
};

class hkpAction : public hkReferencedObject
{
public:
	void *m_world;
	void *m_island;
	uint64 m_userData;
	hkStringPtr m_name;
};

class hkpConstraintChainInstanceAction : public hkpAction
{
public:
	virtual ~hkpConstraintChainInstanceAction();
	class hkpConstraintChainInstance *m_constraintInstance;
};

class hkpEntitySmallArraySerializeOverrideType
{
public:
	void *m_data;
	uint16 m_size;
	uint16 m_capacityAndFlags;
};

class hkpEntityExtendedListeners
{
public:
	hkpEntitySmallArraySerializeOverrideType m_activationListeners;
	hkpEntitySmallArraySerializeOverrideType m_entityListeners;
};

class hkpMaterial
{
public:
	int8 m_responseType;
	hkHalf m_rollingFrictionMultiplier;
	float m_friction;
	float m_restitution;
};

class hkpModifierConstraintAtom : public hkpConstraintAtom
{
public:
	uint16 m_modifierAtomSize;
	uint16 m_childSize;
	hkpConstraintAtom *m_child;
	uint32 m_pad;
};

class hkpConstraintInstanceSmallArraySerializeOverrideType
{
public:
	void *m_data;
	uint16 m_size;
	uint16 m_capacityAndFlags;
};

class hkpConstraintInstance : public hkReferencedObject
{
public:
	virtual ~hkpConstraintInstance();
	void *m_owner;
	hkpConstraintData *m_data;
	hkpModifierConstraintAtom *m_constraintModifiers;
	class hkpEntity *m_entities;
	uint8 m_priority;
	bool m_wantRuntime;
	uint8 m_destructionRemapInfo;
	hkpConstraintInstanceSmallArraySerializeOverrideType m_listeners;
	hkStringPtr m_name;
	uint64 m_userData;
	void *m_internal;
	uint32 m_uid;
};

class hkpEntitySpuCollisionCallback
{
public:
	void *m_util;
	uint16 m_capacity;
	uint8 m_eventFilter;
	uint8 m_userFilter;
};

class hkpEntity : public hkpWorldObject
{
public:
	virtual ~hkpEntity();
	hkpMaterial m_material;
	void *m_limitContactImpulseUtilAndFlag;
	float m_damageMultiplier;
	void *m_breakableBody;
	uint32 m_solverData;
	uint16 m_storageIndex;
	uint16 m_contactPointCallbackDelay;
	hkpEntitySmallArraySerializeOverrideType m_constraintsMaster;
	hkArray<hkpConstraintInstance *> m_constraintsSlave;
	hkArray<uint8> m_constraintRuntime;
	void *m_simulationIsland;
	int8 m_autoRemoveLevel;
	uint8 m_numShapeKeysInContactPointProperties;
	uint8 m_responseModifierFlags;
	uint32 m_uid;
	hkpEntitySpuCollisionCallback m_spuCollisionCallback;
	hkpMaxSizeMotion m_motion;
	hkpEntitySmallArraySerializeOverrideType m_contactListeners;
	hkpEntitySmallArraySerializeOverrideType m_actions;
	hkLocalFrame *m_localFrame;
	hkpEntityExtendedListeners *m_extendedListeners;
	uint32 m_npData;
};

class hkpConstraintChainInstance : public hkpConstraintInstance
{
public:
	virtual ~hkpConstraintChainInstance();
	hkArray<hkpEntity *> m_chainedEntities;
	hkpConstraintChainInstanceAction *m_action;
};

class hkpPairCollisionFilterMapPairFilterKeyOverrideType
{
public:
	void *m_elem;
	int32 m_numElems;
	int32 m_hashMod;
};

class hkpPairCollisionFilter : public hkpCollisionFilter
{
public:
	virtual ~hkpPairCollisionFilter();
	hkpPairCollisionFilterMapPairFilterKeyOverrideType m_disabledPairs;
	hkpCollisionFilter *m_childFilter;
};

class hkpConstraintCollisionFilter : public hkpPairCollisionFilter
{
public:
	virtual ~hkpConstraintCollisionFilter();
};

class hkWorldMemoryAvailableWatchDog : public hkReferencedObject
{
public:
};

class hkpDefaultWorldMemoryWatchDog : public hkWorldMemoryAvailableWatchDog
{
public:
	virtual ~hkpDefaultWorldMemoryWatchDog();
	int32 m_freeHeapMemoryRequested;
};

class hkpFixedRigidMotion : public hkpKeyframedRigidMotion
{
public:
	virtual ~hkpFixedRigidMotion();
};

class hkpGenericConstraintDataSchemeConstraintInfo
{
public:
	int32 m_maxSizeOfSchema;
	int32 m_sizeOfSchemas;
	int32 m_numSolverResults;
	int32 m_numSolverElemTemps;
};

class hkpGenericConstraintDataScheme
{
public:
	hkpGenericConstraintDataSchemeConstraintInfo m_info;
	hkArray<hkVector4> m_data;
	hkArray<int32> m_commands;
	hkArray<void *> m_modifiers;
	hkArray<hkpConstraintMotor *> m_motors;
};

class hkpGenericConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpGenericConstraintData();
	hkpBridgeAtoms m_atoms;
	hkpGenericConstraintDataScheme m_scheme;
};

class hkp2dAngConstraintAtom : public hkpConstraintAtom
{
public:
	uint8 m_freeRotationAxis;
};

class hkpHingeConstraintDataAtoms
{
public:
	hkpSetLocalTransformsConstraintAtom m_transforms;
	hkpSetupStabilizationAtom m_setupStabilization;
	hkp2dAngConstraintAtom m_2dAng;
	hkpBallSocketConstraintAtom m_ballSocket;
};

class hkpHingeConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpHingeConstraintData();
	hkpHingeConstraintDataAtoms m_atoms;
};

class hkpAngLimitConstraintAtom : public hkpConstraintAtom
{
public:
	uint8 m_isEnabled;
	uint8 m_limitAxis;
	float m_minAngle;
	float m_maxAngle;
	float m_angularLimitsTauFactor;
};

class hkpSetLocalRotationsConstraintAtom : public hkpConstraintAtom
{
public:
	hkRotation m_rotationA;
	hkRotation m_rotationB;
};

class hkpHingeLimitsDataAtoms
{
public:
	hkpSetLocalRotationsConstraintAtom m_rotations;
	hkpAngLimitConstraintAtom m_angLimit;
	hkp2dAngConstraintAtom m_2dAng;
};

class hkpHingeLimitsData : public hkpConstraintData
{
public:
	virtual ~hkpHingeLimitsData();
	hkpHingeLimitsDataAtoms m_atoms;
};

class hkpAngMotorConstraintAtom : public hkpConstraintAtom
{
public:
	bool m_isEnabled;
	uint8 m_motorAxis;
	int16 m_initializedOffset;
	int16 m_previousTargetAngleOffset;
	int16 m_correspondingAngLimitSolverResultOffset;
	float m_targetAngle;
	hkpConstraintMotor *m_motor;
};

class hkpAngFrictionConstraintAtom : public hkpConstraintAtom
{
public:
	uint8 m_isEnabled;
	uint8 m_firstFrictionAxis;
	uint8 m_numFrictionAxes;
	float m_maxFrictionTorque;
};

class hkpLimitedHingeConstraintDataAtoms
{
public:
	hkpSetLocalTransformsConstraintAtom m_transforms;
	hkpSetupStabilizationAtom m_setupStabilization;
	hkpAngMotorConstraintAtom m_angMotor;
	hkpAngFrictionConstraintAtom m_angFriction;
	hkpAngLimitConstraintAtom m_angLimit;
	hkp2dAngConstraintAtom m_2dAng;
	hkpBallSocketConstraintAtom m_ballSocket;
};

class hkpLimitedHingeConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpLimitedHingeConstraintData();
	hkpLimitedHingeConstraintDataAtoms m_atoms;
};

class hkpParametricCurve : public hkReferencedObject
{
public:
};

class hkpLinearParametricCurve : public hkpParametricCurve
{
public:
	virtual ~hkpLinearParametricCurve();
	float m_smoothingFactor;
	bool m_closedLoop;
	hkVector4 m_dirNotParallelToTangentAlongWholePath;
	hkArray<hkVector4> m_points;
	hkArray<float> m_distance;
};

class hkpMalleableConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpMalleableConstraintData();
	hkpConstraintData *m_constraintData;
	hkpBridgeAtoms m_atoms;
	float m_strength;
};

class hkpRigidBody : public hkpEntity
{
public:
	virtual ~hkpRigidBody();
};

class hkpPhysicsSystem : public hkReferencedObject
{
public:
	virtual ~hkpPhysicsSystem();
	hkArray<hkpRigidBody *> m_rigidBodies;
	hkArray<hkpConstraintInstance *> m_constraints;
	hkArray<hkpAction *> m_actions;
	hkArray<hkpPhantom *> m_phantoms;
	hkStringPtr m_name;
	uint64 m_userData;
	bool m_active;
};

class hkpPointToPathConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpPointToPathConstraintData();
	hkpBridgeAtoms m_atoms;
	hkpParametricCurve *m_path;
	float m_maxFrictionForce;
	int8 m_angularConstrainedDOF;
	hkTransform m_transform_OS_KS;
};

class hkpLinConstraintAtom : public hkpConstraintAtom
{
public:
	uint8 m_axisIndex;
};

class hkpPointToPlaneConstraintDataAtoms
{
public:
	hkpSetLocalTransformsConstraintAtom m_transforms;
	hkpLinConstraintAtom m_lin;
};

class hkpPointToPlaneConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpPointToPlaneConstraintData();
	hkpPointToPlaneConstraintDataAtoms m_atoms;
};

class hkpPositionConstraintMotor : public hkpLimitedForceConstraintMotor
{
public:
	virtual ~hkpPositionConstraintMotor();
	float m_tau;
	float m_damping;
	float m_proportionalRecoveryVelocity;
	float m_constantRecoveryVelocity;
};

class hkpLinLimitConstraintAtom : public hkpConstraintAtom
{
public:
	uint8 m_axisIndex;
	float m_min;
	float m_max;
};

class hkpAngConstraintAtom : public hkpConstraintAtom
{
public:
	uint8 m_firstConstrainedAxis;
	uint8 m_numConstrainedAxes;
};

class hkpLinFrictionConstraintAtom : public hkpConstraintAtom
{
public:
	uint8 m_isEnabled;
	uint8 m_frictionAxis;
	float m_maxFrictionForce;
};

class hkpLinMotorConstraintAtom : public hkpConstraintAtom
{
public:
	bool m_isEnabled;
	uint8 m_motorAxis;
	int16 m_initializedOffset;
	int16 m_previousTargetPositionOffset;
	float m_targetPosition;
	hkpConstraintMotor *m_motor;
};

class hkpPrismaticConstraintDataAtoms
{
public:
	hkpSetLocalTransformsConstraintAtom m_transforms;
	hkpLinMotorConstraintAtom m_motor;
	hkpLinFrictionConstraintAtom m_friction;
	hkpAngConstraintAtom m_ang;
	hkpLinConstraintAtom m_lin0;
	hkpLinConstraintAtom m_lin1;
	hkpLinLimitConstraintAtom m_linLimit;
};

class hkpPrismaticConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpPrismaticConstraintData();
	hkpPrismaticConstraintDataAtoms m_atoms;
};

class hkpPulleyConstraintAtom : public hkpConstraintAtom
{
public:
	hkVector4 m_fixedPivotAinWorld;
	hkVector4 m_fixedPivotBinWorld;
	float m_ropeLength;
	float m_leverageOnBodyB;
};

class hkpPulleyConstraintDataAtoms
{
public:
	hkpSetLocalTranslationsConstraintAtom m_translations;
	hkpPulleyConstraintAtom m_pulley;
};

class hkpPulleyConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpPulleyConstraintData();
	hkpPulleyConstraintDataAtoms m_atoms;
};

class hkpRackAndPinionConstraintAtom : public hkpConstraintAtom
{
public:
	float m_pinionRadiusOrScrewPitch;
	bool m_isScrew;
	int8 m_memOffsetToInitialAngleOffset;
	int8 m_memOffsetToPrevAngle;
	int8 m_memOffsetToRevolutionCounter;
};

class hkpRackAndPinionConstraintDataAtoms
{
public:
	hkpSetLocalTransformsConstraintAtom m_transforms;
	hkpRackAndPinionConstraintAtom m_rackAndPinion;
};

class hkpRackAndPinionConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpRackAndPinionConstraintData();
	hkpRackAndPinionConstraintDataAtoms m_atoms;
};

class hkpRagdollMotorConstraintAtom : public hkpConstraintAtom
{
public:
	bool m_isEnabled;
	int16 m_initializedOffset;
	int16 m_previousTargetAnglesOffset;
	hkMatrix3 m_target_bRca;
	hkpConstraintMotor *m_motors;
};

class hkpTwistLimitConstraintAtom : public hkpConstraintAtom
{
public:
	uint8 m_isEnabled;
	uint8 m_twistAxis;
	uint8 m_refAxis;
	float m_minAngle;
	float m_maxAngle;
	float m_angularLimitsTauFactor;
};

class hkpConeLimitConstraintAtom : public hkpConstraintAtom
{
public:
	uint8 m_isEnabled;
	uint8 m_twistAxisInA;
	uint8 m_refAxisInB;
	uint8 m_angleMeasurementMode;
	uint8 m_memOffsetToAngleOffset;
	float m_minAngle;
	float m_maxAngle;
	float m_angularLimitsTauFactor;
};

class hkpRagdollConstraintDataAtoms
{
public:
	hkpSetLocalTransformsConstraintAtom m_transforms;
	hkpSetupStabilizationAtom m_setupStabilization;
	hkpRagdollMotorConstraintAtom m_ragdollMotors;
	hkpAngFrictionConstraintAtom m_angFriction;
	hkpTwistLimitConstraintAtom m_twistLimit;
	hkpConeLimitConstraintAtom m_coneLimit;
	hkpConeLimitConstraintAtom m_planesLimit;
	hkpBallSocketConstraintAtom m_ballSocket;
};

class hkpRagdollConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpRagdollConstraintData();
	hkpRagdollConstraintDataAtoms m_atoms;
};

class hkpRagdollLimitsDataAtoms
{
public:
	hkpSetLocalRotationsConstraintAtom m_rotations;
	hkpTwistLimitConstraintAtom m_twistLimit;
	hkpConeLimitConstraintAtom m_coneLimit;
	hkpConeLimitConstraintAtom m_planesLimit;
};

class hkpRagdollLimitsData : public hkpConstraintData
{
public:
	virtual ~hkpRagdollLimitsData();
	hkpRagdollLimitsDataAtoms m_atoms;
};

class hkpRotationalConstraintDataAtoms
{
public:
	hkpSetLocalRotationsConstraintAtom m_rotations;
	hkpAngConstraintAtom m_ang;
};

class hkpRotationalConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpRotationalConstraintData();
	hkpRotationalConstraintDataAtoms m_atoms;
};

class hkpSimpleShapePhantomCollisionDetail
{
public:
	hkpCollidable *m_collidable;
};

class hkpSimpleShapePhantom : public hkpShapePhantom
{
public:
	virtual ~hkpSimpleShapePhantom();
	hkArray<hkpSimpleShapePhantomCollisionDetail> m_collisionDetails;
	bool m_orderDirty;
};

class hkpWorld : public hkReferencedObject
{
public:
	virtual ~hkpWorld();
	class hkpSimulation *m_simulation;
	hkVector4 m_gravity;
	void *m_fixedIsland;
	hkpRigidBody *m_fixedRigidBody;
	hkArray<void *> m_activeSimulationIslands;
	hkArray<void *> m_inactiveSimulationIslands;
	hkArray<void *> m_dirtySimulationIslands;
	void *m_maintenanceMgr;
	void *m_memoryWatchDog;
	bool m_assertOnRunningOutOfSolverMemory;
	void *m_broadPhase;
	void *m_kdTreeManager;
	bool m_autoUpdateTree;
	void *m_broadPhaseDispatcher;
	void *m_phantomBroadPhaseListener;
	void *m_entityEntityBroadPhaseListener;
	void *m_broadPhaseBorderListener;
	void *m_multithreadedSimulationJobData;
	void *m_collisionInput;
	void *m_collisionFilter;
	void *m_collisionDispatcher;
	void *m_convexListFilter;
	void *m_pendingOperations;
	int32 m_pendingOperationsCount;
	int32 m_pendingBodyOperationsCount;
	int32 m_criticalOperationsLockCount;
	int32 m_criticalOperationsLockCountForPhantoms;
	bool m_blockExecutingPendingOperations;
	bool m_criticalOperationsAllowed;
	void *m_pendingOperationQueues;
	int32 m_pendingOperationQueueCount;
	hkMultiThreadCheck m_multiThreadCheck;
	bool m_processActionsInSingleThread;
	bool m_allowIntegrationOfIslandsWithoutConstraintsInASeparateJob;
	uint32 m_minDesiredIslandSize;
	void *m_modifyConstraintCriticalSection;
	int32 m_isLocked;
	void *m_islandDirtyListCriticalSection;
	void *m_propertyMasterLock;
	bool m_wantSimulationIslands;
	bool m_useHybridBroadphase;
	float m_snapCollisionToConvexEdgeThreshold;
	float m_snapCollisionToConcaveEdgeThreshold;
	bool m_enableToiWeldRejection;
	bool m_wantDeactivation;
	bool m_shouldActivateOnRigidBodyTransformChange;
	float m_deactivationReferenceDistance;
	float m_toiCollisionResponseRotateNormal;
	int32 m_maxSectorsPerMidphaseCollideTask;
	int32 m_maxSectorsPerNarrowphaseCollideTask;
	bool m_processToisMultithreaded;
	int32 m_maxEntriesPerToiMidphaseCollideTask;
	int32 m_maxEntriesPerToiNarrowphaseCollideTask;
	int32 m_maxNumToiCollisionPairsSinglethreaded;
	int32 m_simulationType;
	float m_numToisTillAllowedPenetrationSimplifiedToi;
	float m_numToisTillAllowedPenetrationToi;
	float m_numToisTillAllowedPenetrationToiHigher;
	float m_numToisTillAllowedPenetrationToiForced;
	uint32 m_lastEntityUid;
	uint32 m_lastIslandUid;
	uint32 m_lastConstraintUid;
	hkArray<hkpPhantom *> m_phantoms;
	hkArray<void *> m_actionListeners;
	hkArray<void *> m_entityListeners;
	hkArray<void *> m_phantomListeners;
	hkArray<void *> m_constraintListeners;
	hkArray<void *> m_worldDeletionListeners;
	hkArray<void *> m_islandActivationListeners;
	hkArray<void *> m_worldPostSimulationListeners;
	hkArray<void *> m_worldPostIntegrateListeners;
	hkArray<void *> m_worldPostCollideListeners;
	hkArray<void *> m_islandPostIntegrateListeners;
	hkArray<void *> m_islandPostCollideListeners;
	hkArray<void *> m_contactListeners;
	hkArray<void *> m_contactImpulseLimitBreachedListeners;
	hkArray<void *> m_worldExtensions;
	void *m_violatedConstraintArray;
	void *m_broadPhaseBorder;
	void *m_destructionWorld;
	void *m_npWorld;
	hkVector4 m_broadPhaseExtents;
	int32 m_broadPhaseNumMarkers;
	int32 m_sizeOfToiEventQueue;
	int32 m_broadPhaseQuerySize;
	int32 m_broadPhaseUpdateSize;
	int8 m_contactPointGeneration;
};

class hkpSimulation : public hkReferencedObject
{
public:
	virtual ~hkpSimulation();
	uint32 m_determinismCheckFrameCounter;
	hkpWorld *m_world;
	uint8 m_lastProcessingStep;
	float m_currentTime;
	float m_currentPsiTime;
	float m_physicsDeltaTime;
	float m_simulateUntilTime;
	float m_frameMarkerPsiSnap;
	uint32 m_previousStepResult;
};

class hkpSphereMotion : public hkpMotion
{
public:
	virtual ~hkpSphereMotion();
};

class hkpSpringDamperConstraintMotor : public hkpLimitedForceConstraintMotor
{
public:
	virtual ~hkpSpringDamperConstraintMotor();
	float m_springConstant;
	float m_springDamping;
};

class hkpStiffSpringChainDataConstraintInfo
{
public:
	hkVector4 m_pivotInA;
	hkVector4 m_pivotInB;
	float m_springLength;
};

class hkpStiffSpringChainData : public hkpConstraintChainData
{
public:
	virtual ~hkpStiffSpringChainData();
	hkpBridgeAtoms m_atoms;
	hkArray<hkpStiffSpringChainDataConstraintInfo> m_infos;
	float m_tau;
	float m_damping;
	float m_cfm;
};

class hkpStiffSpringConstraintAtom : public hkpConstraintAtom
{
public:
	float m_length;
};

class hkpStiffSpringConstraintDataAtoms
{
public:
	hkpSetLocalTranslationsConstraintAtom m_pivots;
	hkpStiffSpringConstraintAtom m_spring;
};

class hkpStiffSpringConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpStiffSpringConstraintData();
	hkpStiffSpringConstraintDataAtoms m_atoms;
};

class hkpThinBoxMotion : public hkpBoxMotion
{
public:
	virtual ~hkpThinBoxMotion();
};

class hkpVelocityConstraintMotor : public hkpLimitedForceConstraintMotor
{
public:
	virtual ~hkpVelocityConstraintMotor();
	float m_tau;
	float m_velocityTarget;
	bool m_useVelocityTargetFromConstraintTargets;
};

class hkpLinSoftConstraintAtom : public hkpConstraintAtom
{
public:
	uint8 m_axisIndex;
	float m_tau;
	float m_damping;
};

class hkpWheelConstraintDataAtoms
{
public:
	hkpSetLocalTransformsConstraintAtom m_suspensionBase;
	hkpLinLimitConstraintAtom m_lin0Limit;
	hkpLinSoftConstraintAtom m_lin0Soft;
	hkpLinConstraintAtom m_lin1;
	hkpLinConstraintAtom m_lin2;
	hkpSetLocalRotationsConstraintAtom m_steeringBase;
	hkp2dAngConstraintAtom m_2dAng;
};

class hkpWheelConstraintData : public hkpConstraintData
{
public:
	virtual ~hkpWheelConstraintData();
	hkpWheelConstraintDataAtoms m_atoms;
	hkVector4 m_initialAxleInB;
	hkVector4 m_initialSteeringAxisInB;
};

class hkpWorldCinfo : public hkReferencedObject
{
public:
	virtual ~hkpWorldCinfo();
	hkVector4 m_gravity;
	int32 m_broadPhaseQuerySize;
	float m_contactRestingVelocity;
	int8 m_broadPhaseBorderBehaviour;
	bool m_mtPostponeAndSortBroadPhaseBorderCallbacks;
	hkAabb m_broadPhaseWorldAabb;
	bool m_useKdTree;
	bool m_useMultipleTree;
	int8 m_treeUpdateType;
	bool m_autoUpdateKdTree;
	float m_collisionTolerance;
	hkpCollisionFilter *m_collisionFilter;
	hkpConvexListFilter *m_convexListFilter;
	float m_expectedMaxLinearVelocity;
	int32 m_sizeOfToiEventQueue;
	float m_expectedMinPsiDeltaTime;
	hkWorldMemoryAvailableWatchDog *m_memoryWatchDog;
	int32 m_broadPhaseNumMarkers;
	int8 m_contactPointGeneration;
	bool m_allowToSkipConfirmedCallbacks;
	bool m_useHybridBroadphase;
	float m_solverTau;
	float m_solverDamp;
	int32 m_solverIterations;
	int32 m_solverMicrosteps;
	float m_maxConstraintViolation;
	bool m_forceCoherentConstraintOrderingInSolver;
	float m_snapCollisionToConvexEdgeThreshold;
	float m_snapCollisionToConcaveEdgeThreshold;
	bool m_enableToiWeldRejection;
	bool m_enableDeprecatedWelding;
	float m_iterativeLinearCastEarlyOutDistance;
	int32 m_iterativeLinearCastMaxIterations;
	uint8 m_deactivationNumInactiveFramesSelectFlag0;
	uint8 m_deactivationNumInactiveFramesSelectFlag1;
	uint8 m_deactivationIntegrateCounter;
	bool m_shouldActivateOnRigidBodyTransformChange;
	float m_deactivationReferenceDistance;
	float m_toiCollisionResponseRotateNormal;
	int32 m_maxSectorsPerMidphaseCollideTask;
	int32 m_maxSectorsPerNarrowphaseCollideTask;
	bool m_processToisMultithreaded;
	int32 m_maxEntriesPerToiMidphaseCollideTask;
	int32 m_maxEntriesPerToiNarrowphaseCollideTask;
	int32 m_maxNumToiCollisionPairsSinglethreaded;
	float m_numToisTillAllowedPenetrationSimplifiedToi;
	float m_numToisTillAllowedPenetrationToi;
	float m_numToisTillAllowedPenetrationToiHigher;
	float m_numToisTillAllowedPenetrationToiForced;
	bool m_enableDeactivation;
	int8 m_simulationType;
	bool m_enableSimulationIslands;
	uint32 m_minDesiredIslandSize;
	bool m_processActionsInSingleThread;
	bool m_allowIntegrationOfIslandsWithoutConstraintsInASeparateJob;
	float m_frameMarkerPsiSnap;
	bool m_fireCollisionCallbacks;
};

class hkpPoweredChainDataConstraintInfo
{
public:
	hkVector4 m_pivotInA;
	hkVector4 m_pivotInB;
	hkQuaternion m_aTc;
	hkQuaternion m_bTc;
	hkpConstraintMotor *m_motors;
	bool m_switchBodies;
};

class hkpPoweredChainData : public hkpConstraintChainData
{
public:
	virtual ~hkpPoweredChainData();
	hkpBridgeAtoms m_atoms;
	hkArray<hkpPoweredChainDataConstraintInfo> m_infos;
	float m_tau;
	float m_damping;
	float m_cfmLinAdd;
	float m_cfmLinMul;
	float m_cfmAngAdd;
	float m_cfmAngMul;
	float m_maxErrorDistance;
};

class hkpBinaryAction : public hkpAction
{
public:
	hkpEntity *m_entityA;
	hkpEntity *m_entityB;
};

class hkpAngularDashpotAction : public hkpBinaryAction
{
public:
	virtual ~hkpAngularDashpotAction();
	hkQuaternion m_rotation;
	float m_strength;
	float m_damping;
};

class hkpFirstPersonGun : public hkReferencedObject
{
public:
	virtual ~hkpFirstPersonGun();
	uint8 m_type;
	hkStringPtr m_name;
	uint8 m_keyboardKey;
	hkArray<void *> m_listeners;
};

class hkpBallGun : public hkpFirstPersonGun
{
public:
	virtual ~hkpBallGun();
	float m_bulletRadius;
	float m_bulletVelocity;
	float m_bulletMass;
	float m_damageMultiplier;
	int32 m_maxBulletsInWorld;
	hkVector4 m_bulletOffsetFromCenter;
	void *m_addedBodies;
};

class hkpCharacterControllerCinfo : public hkReferencedObject
{
public:
	virtual ~hkpCharacterControllerCinfo();
};

class hkpCharacterProxyCinfo : public hkpCharacterControllerCinfo
{
public:
	virtual ~hkpCharacterProxyCinfo();
	hkVector4 m_position;
	hkVector4 m_velocity;
	float m_dynamicFriction;
	float m_staticFriction;
	float m_keepContactTolerance;
	hkVector4 m_up;
	float m_extraUpStaticFriction;
	float m_extraDownStaticFriction;
	hkpShapePhantom *m_shapePhantom;
	float m_keepDistance;
	float m_contactAngleSensitivity;
	uint32 m_userPlanes;
	float m_maxCharacterSpeedForSolver;
	float m_characterStrength;
	float m_characterMass;
	float m_maxSlope;
	float m_penetrationRecoverySpeed;
	int32 m_maxCastIterations;
	bool m_refreshManifoldInCheckSupport;
};

class hkpCharacterRigidBodyCinfo : public hkpCharacterControllerCinfo
{
public:
	virtual ~hkpCharacterRigidBodyCinfo();
	uint32 m_collisionFilterInfo;
	hkpShape *m_shape;
	hkVector4 m_position;
	hkQuaternion m_rotation;
	float m_mass;
	float m_friction;
	float m_maxLinearVelocity;
	float m_allowedPenetrationDepth;
	hkVector4 m_up;
	float m_maxSlope;
	float m_maxForce;
	float m_unweldingHeightOffsetFactor;
	float m_maxSpeedForSimplexSolver;
	float m_supportDistance;
	float m_hardSupportDistance;
	int32 m_vdbColor;
};

class hkpConstrainedSystemFilter : public hkpCollisionFilter
{
public:
	virtual ~hkpConstrainedSystemFilter();
	hkpCollisionFilter *m_otherFilter;
};

class hkpDashpotAction : public hkpBinaryAction
{
public:
	virtual ~hkpDashpotAction();
	hkVector4 m_point;
	float m_strength;
	float m_damping;
	hkVector4 m_impulse;
};

class hkpDisableEntityCollisionFilter : public hkpCollisionFilter
{
public:
	virtual ~hkpDisableEntityCollisionFilter();
	hkArray<hkpEntity *> m_disabledEntities;
};

class hkpDisplayBindingDataRigidBody : public hkReferencedObject
{
public:
	virtual ~hkpDisplayBindingDataRigidBody();
	hkpRigidBody *m_rigidBody;
	hkReferencedObject *m_displayObjectPtr;
	hkMatrix4 m_rigidBodyFromDisplayObjectTransform;
};

class hkpDisplayBindingDataPhysicsSystem : public hkReferencedObject
{
public:
	virtual ~hkpDisplayBindingDataPhysicsSystem();
	hkArray<hkpDisplayBindingDataRigidBody *> m_bindings;
	hkpPhysicsSystem *m_system;
};

class hkpDisplayBindingData : public hkReferencedObject
{
public:
	virtual ~hkpDisplayBindingData();
	hkArray<hkpDisplayBindingDataRigidBody *> m_rigidBodyBindings;
	hkArray<hkpDisplayBindingDataPhysicsSystem *> m_physicsSystemBindings;
};

class hkpGravityGun : public hkpFirstPersonGun
{
public:
	virtual ~hkpGravityGun();
	hkArray<void *> m_grabbedBodies;
	int32 m_maxNumObjectsPicked;
	float m_maxMassOfObjectPicked;
	float m_maxDistOfObjectPicked;
	float m_impulseAppliedWhenObjectNotPicked;
	float m_throwVelocity;
	hkVector4 m_capturedObjectPosition;
	hkVector4 m_capturedObjectsOffset;
};

class hkpGroupCollisionFilter : public hkpCollisionFilter
{
public:
	virtual ~hkpGroupCollisionFilter();
	bool m_noGroupCollisionEnabled;
	uint32 m_collisionGroups;
};

class hkpUnaryAction : public hkpAction
{
public:
	hkpEntity *m_entity;
};

class hkpMotorAction : public hkpUnaryAction
{
public:
	virtual ~hkpMotorAction();
	hkVector4 m_axis;
	float m_spinRate;
	float m_gain;
	bool m_active;
};

class hkpMountedBallGun : public hkpBallGun
{
public:
	virtual ~hkpMountedBallGun();
	hkVector4 m_position;
};

class hkpMouseSpringAction : public hkpUnaryAction
{
public:
	virtual ~hkpMouseSpringAction();
	hkVector4 m_positionInRbLocal;
	hkVector4 m_mousePositionInWorld;
	float m_springDamping;
	float m_springElasticity;
	float m_maxRelativeForce;
	float m_objectDamping;
	uint32 m_shapeKey;
	hkArray<void *> m_applyCallbacks;
};

class hkpPhysicsData : public hkReferencedObject
{
public:
	virtual ~hkpPhysicsData();
	hkpWorldCinfo *m_worldCinfo;
	hkArray<hkpPhysicsSystem *> m_systems;
};

class hkContactPoint
{
public:
	hkVector4 m_position;
	hkVector4 m_separatingNormal;
};

class hkpAgent1nSector;
class hkpSerializedSubTrack1nInfo;

class hkpSerializedTrack1nInfo
{
public:
	hkArray<hkpAgent1nSector *> m_sectors;
	hkArray<hkpSerializedSubTrack1nInfo *> m_subTracks;
};

class hkpSerializedSubTrack1nInfo : public hkpSerializedTrack1nInfo
{
public:
	int32 m_sectorIndex;
	int32 m_offsetInSector;
};

class hkpAgent1nSector
{
public:
	uint32 m_bytesAllocated;
	uint32 m_pad0;
	uint32 m_pad1;
	uint32 m_pad2;
	uint8 m_data;
};

class hkpSimpleContactConstraintDataInfo
{
public:
	uint16 m_flags;
	uint16 m_index;
	float m_internalData0;
	hkHalf m_rollingFrictionMultiplier;
	hkHalf m_internalData1;
	uint32 m_data;
};

class hkpSimpleContactConstraintAtom : public hkpConstraintAtom
{
public:
	uint16 m_sizeOfAllAtoms;
	uint16 m_numContactPoints;
	uint16 m_numReservedContactPoints;
	uint8 m_numUserDatasForBodyA;
	uint8 m_numUserDatasForBodyB;
	uint8 m_contactPointPropertiesStriding;
	uint16 m_maxNumContactPoints;
	hkpSimpleContactConstraintDataInfo m_info;
};

class hkpSerializedAgentNnEntry : public hkReferencedObject
{
public:
	virtual ~hkpSerializedAgentNnEntry();
	hkpEntity *m_bodyA;
	hkpEntity *m_bodyB;
	uint64 m_bodyAId;
	uint64 m_bodyBId;
	bool m_useEntityIds;
	int8 m_agentType;
	hkpSimpleContactConstraintAtom m_atom;
	hkArray<uint8> m_propertiesStream;
	hkArray<hkContactPoint> m_contactPoints;
	hkArray<uint8> m_cpIdMgr;
	uint8 m_nnEntryData;
	hkpSerializedTrack1nInfo m_trackInfo;
	uint8 m_endianCheckBuffer;
	uint32 m_version;
};

class hkpPhysicsSystemWithContacts : public hkpPhysicsSystem
{
public:
	virtual ~hkpPhysicsSystemWithContacts();
	hkArray<hkpSerializedAgentNnEntry *> m_contacts;
};

class hkpPoweredChainMapperLinkInfo
{
public:
	int32 m_firstTargetIdx;
	int32 m_numTargets;
	hkpConstraintInstance *m_limitConstraint;
};

class hkpPoweredChainMapperTarget
{
public:
	hkpPoweredChainData *m_chain;
	int32 m_infoIndex;
};

class hkpPoweredChainMapper : public hkReferencedObject
{
public:
	virtual ~hkpPoweredChainMapper();
	hkArray<hkpPoweredChainMapperLinkInfo> m_links;
	hkArray<hkpPoweredChainMapperTarget> m_targets;
	hkArray<hkpConstraintChainInstance *> m_chains;
};

class hkpProjectileGun : public hkpFirstPersonGun
{
public:
	virtual ~hkpProjectileGun();
	int32 m_maxProjectiles;
	float m_reloadTime;
	float m_reload;
	hkArray<void *> m_projectiles;
	void *m_world;
	void *m_destructionWorld;
};

class hkpReorientAction : public hkpUnaryAction
{
public:
	virtual ~hkpReorientAction();
	hkVector4 m_rotationAxis;
	hkVector4 m_upAxis;
	float m_strength;
	float m_damping;
};

class hkpSerializedDisplayMarker : public hkReferencedObject
{
public:
	virtual ~hkpSerializedDisplayMarker();
	hkTransform m_transform;
};

class hkpSerializedDisplayMarkerList : public hkReferencedObject
{
public:
	virtual ~hkpSerializedDisplayMarkerList();
	hkArray<hkpSerializedDisplayMarker *> m_markers;
};

class hkpSerializedDisplayRbTransformsDisplayTransformPair
{
public:
	hkpRigidBody *m_rb;
	hkTransform m_localToDisplay;
};

class hkpSerializedDisplayRbTransforms : public hkReferencedObject
{
public:
	virtual ~hkpSerializedDisplayRbTransforms();
	hkArray<hkpSerializedDisplayRbTransformsDisplayTransformPair> m_transforms;
};

class hkpSpringAction : public hkpBinaryAction
{
public:
	virtual ~hkpSpringAction();
	hkVector4 m_lastForce;
	hkVector4 m_positionAinA;
	hkVector4 m_positionBinB;
	float m_restLength;
	float m_strength;
	float m_damping;
	bool m_onCompression;
	bool m_onExtension;
};

class hkpTriggerVolumeEventInfo
{
public:
	uint64 m_sortValue;
	hkpRigidBody *m_body;
	int32 m_operation;
};

class hkpTriggerVolume : public hkReferencedObject
{
public:
	virtual ~hkpTriggerVolume();
	hkArray<hkpRigidBody *> m_overlappingBodies;
	hkArray<hkpTriggerVolumeEventInfo> m_eventQueue;
	hkpRigidBody *m_triggerBody;
	uint32 m_sequenceNumber;
};

class hkaAnimatedReferenceFrame : public hkReferencedObject
{
public:
};

class hkaAnnotationTrackAnnotation
{
public:
	float m_time;
	hkStringPtr m_text;
};

class hkaAnnotationTrack
{
public:
	hkStringPtr m_trackName;
	hkArray<hkaAnnotationTrackAnnotation> m_annotations;
};

class hkaAnimation : public hkReferencedObject
{
public:
	enum AnimationType
	{
		HK_UNKNOWN_ANIMATION,
		HK_INTERLEAVED_ANIMATION,
		HK_DELTA_COMPRESSED_ANIMATION,
		HK_WAVELET_COMPRESSED_ANIMATION,
		HK_MIRRORED_ANIMATION,
		HK_SPLINE_COMPRESSED_ANIMATION,
		HK_QUANTIZED_COMPRESSED_ANIMATION
	};
	int32 m_type;
	float m_duration;
	int32 m_numberOfTransformTracks;
	int32 m_numberOfFloatTracks;
	hkaAnimatedReferenceFrame *m_extractedMotion;
	hkArray<hkaAnnotationTrack> m_annotationTracks;
};

class hkaAnimationBinding : public hkReferencedObject
{
public:
	virtual ~hkaAnimationBinding();
	hkStringPtr m_originalSkeletonName;
	hkaAnimation *m_animation;
	hkArray<int16> m_transformTrackToBoneIndices;
	hkArray<int16> m_floatTrackToFloatSlotIndices;
	int8 m_blendHint;
};

class hkaSkeletonLocalFrameOnBone
{
public:
	hkLocalFrame *m_localFrame;
	int32 m_boneIndex;
};

class hkaBone
{
public:
	hkStringPtr m_name;
	bool m_lockTranslation;
};

class hkaSkeleton : public hkReferencedObject
{
public:
	virtual ~hkaSkeleton();
	hkStringPtr m_name;
	hkArray<int16> m_parentIndices;
	hkArray<hkaBone> m_bones;
	hkArray<hkQsTransform> m_referencePose;
	hkArray<float> m_referenceFloats;
	hkArray<hkStringPtr> m_floatSlots;
	hkArray<hkaSkeletonLocalFrameOnBone> m_localFrames;
};

class hkaMeshBindingMapping
{
public:
	hkArray<int16> m_mapping;
};

class hkaMeshBinding : public hkReferencedObject
{
public:
	virtual ~hkaMeshBinding();
	hkxMesh *m_mesh;
	hkStringPtr m_originalSkeletonName;
	hkaSkeleton *m_skeleton;
	hkArray<hkaMeshBindingMapping> m_mappings;
	hkArray<hkTransform> m_boneFromSkinMeshTransforms;
};

class hkaBoneAttachment : public hkReferencedObject
{
public:
	virtual ~hkaBoneAttachment();
	hkStringPtr m_originalSkeletonName;
	hkMatrix4 m_boneFromAttachment;
	hkReferencedObject *m_attachment;
	hkStringPtr m_name;
	int16 m_boneIndex;
};

class hkaAnimationContainer : public hkReferencedObject
{
public:
	virtual ~hkaAnimationContainer();
	hkArray<hkaSkeleton *> m_skeletons;
	hkArray<hkaAnimation *> m_animations;
	hkArray<hkaAnimationBinding *> m_bindings;
	hkArray<hkaBoneAttachment *> m_attachments;
	hkArray<hkaMeshBinding *> m_skins;
};

class hkaAnimationPreviewColorContainer : public hkReferencedObject
{
public:
	virtual ~hkaAnimationPreviewColorContainer();
	hkArray<uint32> m_previewColor;
};

class hkaDefaultAnimatedReferenceFrame : public hkaAnimatedReferenceFrame
{
public:
	virtual ~hkaDefaultAnimatedReferenceFrame();
	hkVector4 m_up;
	hkVector4 m_forward;
	float m_duration;
	hkArray<hkVector4> m_referenceFrameSamples;
};

class hkaDeltaCompressedAnimationQuantizationFormat
{
public:
	uint8 m_maxBitWidth;
	uint8 m_preserved;
	uint32 m_numD;
	uint32 m_offsetIdx;
	uint32 m_scaleIdx;
	uint32 m_bitWidthIdx;
};

class hkaDeltaCompressedAnimation : public hkaAnimation
{
public:
	virtual ~hkaDeltaCompressedAnimation();
	int32 m_numberOfPoses;
	int32 m_blockSize;
	hkaDeltaCompressedAnimationQuantizationFormat m_qFormat;
	uint32 m_quantizedDataIdx;
	uint32 m_quantizedDataSize;
	uint32 m_staticMaskIdx;
	uint32 m_staticMaskSize;
	uint32 m_staticDOFsIdx;
	uint32 m_staticDOFsSize;
	uint32 m_numStaticTransformDOFs;
	uint32 m_numDynamicTransformDOFs;
	uint32 m_totalBlockSize;
	uint32 m_lastBlockSize;
	hkArray<uint8> m_dataBuffer;
};

class hkaFootstepAnalysisInfo : public hkReferencedObject
{
public:
	virtual ~hkaFootstepAnalysisInfo();
	hkArray<char> m_name;
	hkArray<char> m_nameStrike;
	hkArray<char> m_nameLift;
	hkArray<char> m_nameLock;
	hkArray<char> m_nameUnlock;
	hkArray<float> m_minPos;
	hkArray<float> m_maxPos;
	hkArray<float> m_minVel;
	hkArray<float> m_maxVel;
	hkArray<float> m_allBonesDown;
	hkArray<float> m_anyBonesDown;
	float m_posTol;
	float m_velTol;
	float m_duration;
};

class hkaFootstepAnalysisInfoContainer : public hkReferencedObject
{
public:
	virtual ~hkaFootstepAnalysisInfoContainer();
	hkArray<hkaFootstepAnalysisInfo *> m_previewInfo;
};

class hkaInterleavedUncompressedAnimation : public hkaAnimation
{
public:
	virtual ~hkaInterleavedUncompressedAnimation();
	hkArray<hkQsTransform> m_transforms;
	hkArray<float> m_floats;
};

class hkaQuantizedAnimation : public hkaAnimation
{
public:
	virtual ~hkaQuantizedAnimation();
	hkArray<uint8> m_data;
	uint32 m_endian;
	void *m_skeleton;
};

class hkaSkeletonMapperDataChainMapping
{
public:
	int16 m_startBoneA;
	int16 m_endBoneA;
	int16 m_startBoneB;
	int16 m_endBoneB;
	hkQsTransform m_startAFromBTransform;
	hkQsTransform m_endAFromBTransform;
};

class hkaSkeletonMapperDataSimpleMapping
{
public:
	int16 m_boneA;
	int16 m_boneB;
	hkQsTransform m_aFromBTransform;
};

class hkaSkeletonMapperData
{
public:
	hkaSkeleton *m_skeletonA;
	hkaSkeleton *m_skeletonB;
	hkArray<hkaSkeletonMapperDataSimpleMapping> m_simpleMappings;
	hkArray<hkaSkeletonMapperDataChainMapping> m_chainMappings;
	hkArray<int16> m_unmappedBones;
	hkQsTransform m_extractedMotionMapping;
	bool m_keepUnmappedLocal;
	int32 m_mappingType;
};

class hkaSkeletonMapper : public hkReferencedObject
{
public:
	virtual ~hkaSkeletonMapper();
	hkaSkeletonMapperData m_mapping;
};

class hkaSplineCompressedAnimation : public hkaAnimation
{
public:
	virtual ~hkaSplineCompressedAnimation();
	int32 m_numFrames;
	int32 m_numBlocks;
	int32 m_maxFramesPerBlock;
	int32 m_maskAndQuantizationSize;
	float m_blockDuration;
	float m_blockInverseDuration;
	float m_frameDuration;
	hkArray<uint32> m_blockOffsets;
	hkArray<uint32> m_floatBlockOffsets;
	hkArray<uint32> m_transformOffsets;
	hkArray<uint32> m_floatOffsets;
	hkArray<uint8> m_data;
	int32 m_endian;
};

class hkaWaveletCompressedAnimationQuantizationFormat
{
public:
	uint8 m_maxBitWidth;
	uint8 m_preserved;
	uint32 m_numD;
	uint32 m_offsetIdx;
	uint32 m_scaleIdx;
	uint32 m_bitWidthIdx;
};

class hkaWaveletCompressedAnimation : public hkaAnimation
{
public:
	virtual ~hkaWaveletCompressedAnimation();
	int32 m_numberOfPoses;
	int32 m_blockSize;
	hkaWaveletCompressedAnimationQuantizationFormat m_qFormat;
	uint32 m_staticMaskIdx;
	uint32 m_staticDOFsIdx;
	uint32 m_numStaticTransformDOFs;
	uint32 m_numDynamicTransformDOFs;
	uint32 m_blockIndexIdx;
	uint32 m_blockIndexSize;
	uint32 m_quantizedDataIdx;
	uint32 m_quantizedDataSize;
	hkArray<uint8> m_dataBuffer;
};

class hkaRagdollInstance : public hkReferencedObject
{
public:
	virtual ~hkaRagdollInstance();
	hkArray<hkpRigidBody *> m_rigidBodies;
	hkArray<hkpConstraintInstance *> m_constraints;
	hkArray<int32> m_boneToRigidBodyMap;
	hkaSkeleton *m_skeleton;
};

class hkbVariableBindingSetBinding
{
public:
	hkStringPtr m_memberPath;
	void *m_memberClass;
	int32 m_offsetInObjectPlusOne;
	int32 m_offsetInArrayPlusOne;
	int32 m_rootVariableIndex;
	int32 m_variableIndex;
	int8 m_bitIndex;
	int8 m_bindingType;
	uint8 m_memberType;
	int8 m_variableType;
	hkFlags m_flags;
};

class hkbVariableBindingSet : public hkReferencedObject
{
public:
	virtual ~hkbVariableBindingSet();
	hkArray<hkbVariableBindingSetBinding> m_bindings;
	int32 m_indexOfBindingToEnable;
	bool m_hasOutputBinding;
};

class hkbBindable : public hkReferencedObject
{
public:
	virtual ~hkbBindable();
	hkbVariableBindingSet *m_variableBindingSet;
	hkArray<void> m_cachedBindables;
	bool m_areBindablesCached;
};

class hkbNode : public hkbBindable
{
public:
	virtual ~hkbNode();
	uint64 m_userData;
	hkStringPtr m_name;
	int16 m_id;
	int8 m_cloneState;
	bool m_padNode;
};

class hkbModifier : public hkbNode
{
public:
	virtual ~hkbModifier();
	bool m_enable;
	bool m_padModifier;
};

class hkbEventPayload : public hkReferencedObject
{
public:
	virtual ~hkbEventPayload();
};

class hkbEventBase
{
public:
	int32 m_id;
	hkbEventPayload *m_payload;
};

class hkbEventProperty : public hkbEventBase
{
public:
};

class hkbProjectStringData : public hkReferencedObject
{
public:
	virtual ~hkbProjectStringData();
	hkArray<hkStringPtr> m_animationFilenames;
	hkArray<hkStringPtr> m_behaviorFilenames;
	hkArray<hkStringPtr> m_characterFilenames;
	hkArray<hkStringPtr> m_eventNames;
	hkStringPtr m_animationPath;
	hkStringPtr m_behaviorPath;
	hkStringPtr m_characterPath;
	hkStringPtr m_fullPathToSource;
	hkStringPtr m_rootPath;
};

class hkbProjectData : public hkReferencedObject
{
public:
	virtual ~hkbProjectData();
	hkVector4 m_worldUpWS;
	hkbProjectStringData *m_stringData;
	int8 m_defaultEventMode;
};

class hkbHandIkDriverInfoHand
{
public:
	hkVector4 m_elbowAxisLS;
	hkVector4 m_backHandNormalLS;
	hkVector4 m_handOffsetLS;
	hkQuaternion m_handOrienationOffsetLS;
	float m_maxElbowAngleDegrees;
	float m_minElbowAngleDegrees;
	int16 m_shoulderIndex;
	int16 m_shoulderSiblingIndex;
	int16 m_elbowIndex;
	int16 m_elbowSiblingIndex;
	int16 m_wristIndex;
	bool m_enforceEndPosition;
	bool m_enforceEndRotation;
	hkStringPtr m_localFrameName;
};

class hkbHandIkDriverInfo : public hkReferencedObject
{
public:
	virtual ~hkbHandIkDriverInfo();
	hkArray<hkbHandIkDriverInfoHand> m_hands;
	int8 m_fadeInOutCurve;
};

class hkbRoleAttribute
{
public:
	int16 m_role;
	hkFlags m_flags;
};

class hkbVariableInfo
{
public:
	hkbRoleAttribute m_role;
	int8 m_type;
};

class hkbCharacterDataCharacterControllerInfo
{
public:
	float m_capsuleHeight;
	float m_capsuleRadius;
	uint32 m_collisionFilterInfo;
	hkpCharacterControllerCinfo *m_characterControllerCinfo;
};

class hkbFootIkDriverInfoLeg
{
public:
	hkQuaternion m_prevAnkleRotLS;
	hkVector4 m_kneeAxisLS;
	hkVector4 m_footEndLS;
	float m_footPlantedAnkleHeightMS;
	float m_footRaisedAnkleHeightMS;
	float m_maxAnkleHeightMS;
	float m_minAnkleHeightMS;
	float m_maxKneeAngleDegrees;
	float m_minKneeAngleDegrees;
	float m_maxAnkleAngleDegrees;
	int16 m_hipIndex;
	int16 m_kneeIndex;
	int16 m_ankleIndex;
};

class hkbFootIkDriverInfo : public hkReferencedObject
{
public:
	virtual ~hkbFootIkDriverInfo();
	hkArray<hkbFootIkDriverInfoLeg> m_legs;
	float m_raycastDistanceUp;
	float m_raycastDistanceDown;
	float m_originalGroundHeightMS;
	float m_verticalOffset;
	uint32 m_collisionFilterInfo;
	float m_forwardAlignFraction;
	float m_sidewaysAlignFraction;
	float m_sidewaysSampleWidth;
	bool m_lockFeetWhenPlanted;
	bool m_useCharacterUpVector;
	bool m_isQuadrupedNarrow;
};

class hkbMirroredSkeletonInfo : public hkReferencedObject
{
public:
	virtual ~hkbMirroredSkeletonInfo();
	hkVector4 m_mirrorAxis;
	hkArray<int16> m_bonePairMap;
};

class hkbCharacterStringData : public hkReferencedObject
{
public:
	virtual ~hkbCharacterStringData();
	hkArray<hkStringPtr> m_deformableSkinNames;
	hkArray<hkStringPtr> m_rigidSkinNames;
	hkArray<hkStringPtr> m_animationNames;
	hkArray<hkStringPtr> m_animationFilenames;
	hkArray<hkStringPtr> m_characterPropertyNames;
	hkArray<hkStringPtr> m_retargetingSkeletonMapperFilenames;
	hkArray<hkStringPtr> m_lodNames;
	hkArray<hkStringPtr> m_mirroredSyncPointSubstringsA;
	hkArray<hkStringPtr> m_mirroredSyncPointSubstringsB;
	hkStringPtr m_name;
	hkStringPtr m_rigName;
	hkStringPtr m_ragdollName;
	hkStringPtr m_behaviorFilename;
};

class hkbVariableValue
{
public:
	int32 m_value;
};

class hkbVariableValueSet : public hkReferencedObject
{
public:
	virtual ~hkbVariableValueSet();
	hkArray<hkbVariableValue> m_wordVariableValues;
	hkArray<hkVector4> m_quadVariableValues;
	hkArray<hkReferencedObject *> m_variantVariableValues;
};

class hkbCharacterData : public hkReferencedObject
{
public:
	virtual ~hkbCharacterData();
	hkbCharacterDataCharacterControllerInfo m_characterControllerInfo;
	hkVector4 m_modelUpMS;
	hkVector4 m_modelForwardMS;
	hkVector4 m_modelRightMS;
	hkArray<hkbVariableInfo> m_characterPropertyInfos;
	hkArray<int32> m_numBonesPerLod;
	hkbVariableValueSet *m_characterPropertyValues;
	hkbFootIkDriverInfo *m_footIkDriverInfo;
	hkbHandIkDriverInfo *m_handIkDriverInfo;
	hkbCharacterStringData *m_stringData;
	hkbMirroredSkeletonInfo *m_mirroredSkeletonInfo;
	float m_scale;
	int16 m_numHands;
	int16 m_numFloatSlots;
};

class hkbCharacterSetup : public hkReferencedObject
{
public:
	virtual ~hkbCharacterSetup();
	hkArray<hkaSkeletonMapper *> m_retargetingSkeletonMappers;
	hkaSkeleton *m_animationSkeleton;
	hkaSkeletonMapper *m_ragdollToAnimationSkeletonMapper;
	hkaSkeletonMapper *m_animationToRagdollSkeletonMapper;
	void *m_animationBindingSet;
	hkbCharacterData *m_data;
	void *m_mirroredSkeleton;
	void *m_characterPropertyIdMap;
};

class hkbGenerator : public hkbNode
{
public:
};

class hkbBehaviorGraphStringData : public hkReferencedObject
{
public:
	virtual ~hkbBehaviorGraphStringData();
	hkArray<hkStringPtr> m_eventNames;
	hkArray<hkStringPtr> m_attributeNames;
	hkArray<hkStringPtr> m_variableNames;
	hkArray<hkStringPtr> m_characterPropertyNames;
};

class hkbEventInfo
{
public:
	hkFlags m_flags;
};

class hkbBehaviorGraphData : public hkReferencedObject
{
public:
	virtual ~hkbBehaviorGraphData();
	hkArray<float> m_attributeDefaults;
	hkArray<hkbVariableInfo> m_variableInfos;
	hkArray<hkbVariableInfo> m_characterPropertyInfos;
	hkArray<hkbEventInfo> m_eventInfos;
	hkArray<hkbVariableValue> m_wordMinVariableValues;
	hkArray<hkbVariableValue> m_wordMaxVariableValues;
	hkbVariableValueSet *m_variableInitialValues;
	hkbBehaviorGraphStringData *m_stringData;
};

class hkbBehaviorGraph : public hkbGenerator
{
public:
	virtual ~hkbBehaviorGraph();
	int8 m_variableMode;
	hkArray<void> m_uniqueIdPool;
	void *m_idToStateMachineTemplateMap;
	hkArray<void> m_mirroredExternalIdMap;
	void *m_pseudoRandomGenerator;
	hkbGenerator *m_rootGenerator;
	hkbBehaviorGraphData *m_data;
	void *m_rootGeneratorClone;
	void *m_activeNodes;
	void *m_activeNodeTemplateToIndexMap;
	void *m_activeNodesChildrenIndices;
	void *m_globalTransitionData;
	void *m_eventIdMap;
	void *m_attributeIdMap;
	void *m_variableIdMap;
	void *m_characterPropertyIdMap;
	void *m_variableValueSet;
	void *m_nodeTemplateToCloneMap;
	void *m_nodeCloneToTemplateMap;
	void *m_stateListenerTemplateToCloneMap;
	void *m_nodePartitionInfo;
	int32 m_numIntermediateOutputs;
	hkArray<void *> m_jobs;
	hkArray<void *> m_allPartitionMemory;
	int16 m_numStaticNodes;
	int16 m_nextUniqueId;
	bool m_isActive;
	bool m_isLinked;
	bool m_updateActiveNodes;
	bool m_stateOrTransitionChanged;
};

class hkbCharacter : public hkReferencedObject
{
public:
	virtual ~hkbCharacter();
	hkArray<hkbCharacter *> m_nearbyCharacters;
	int16 m_currentLod;
	int16 m_numTracksInLod;
	hkStringPtr m_name;
	void *m_ragdollDriver;
	void *m_characterControllerDriver;
	void *m_footIkDriver;
	void *m_handIkDriver;
	hkbCharacterSetup *m_setup;
	hkbBehaviorGraph *m_behaviorGraph;
	hkbProjectData *m_projectData;
	void *m_animationBindingSet;
	void *m_raycastInterface;
	void *m_world;
	void *m_eventQueue;
	void *m_worldFromModel;
	void *m_poseLocal;
	bool m_deleteWorldFromModel;
	bool m_deletePoseLocal;
};

class hkbHandle : public hkReferencedObject
{
public:
	virtual ~hkbHandle();
	hkLocalFrame *m_frame;
	hkpRigidBody *m_rigidBody;
	hkbCharacter *m_character;
	int16 m_animationBoneIndex;
};

class hkbAttachmentSetup : public hkReferencedObject
{
public:
	virtual ~hkbAttachmentSetup();
	float m_blendInTime;
	float m_moveAttacherFraction;
	float m_gain;
	float m_extrapolationTimeStep;
	float m_fixUpGain;
	float m_maxLinearDistance;
	float m_maxAngularDistance;
	int8 m_attachmentType;
};

class hkbAttachmentModifier : public hkbModifier
{
public:
	virtual ~hkbAttachmentModifier();
	hkbEventProperty m_sendToAttacherOnAttach;
	hkbEventProperty m_sendToAttacheeOnAttach;
	hkbEventProperty m_sendToAttacherOnDetach;
	hkbEventProperty m_sendToAttacheeOnDetach;
	hkbAttachmentSetup *m_attachmentSetup;
	hkbHandle *m_attacherHandle;
	hkbHandle *m_attacheeHandle;
	int32 m_attacheeLayer;
	void *m_attacheeRB;
	uint8 m_oldMotionType;
	int32 m_oldFilterInfo;
	void *m_attachment;
};

class hkbAttributeModifierAssignment
{
public:
	int32 m_attributeIndex;
	float m_attributeValue;
};

class hkbAttributeModifier : public hkbModifier
{
public:
	virtual ~hkbAttributeModifier();
	hkArray<hkbAttributeModifierAssignment> m_assignments;
};

class hkbGeneratorSyncInfoSyncPoint
{
public:
	int32 m_id;
	float m_time;
};

class hkbGeneratorSyncInfo
{
public:
	hkbGeneratorSyncInfoSyncPoint m_syncPoints;
	float m_baseFrequency;
	float m_localTime;
	float m_playbackSpeed;
	int8 m_numSyncPoints;
	bool m_isCyclic;
	bool m_isMirrored;
	bool m_isAdditive;
};

class hkbNodeInternalStateInfo : public hkReferencedObject
{
public:
	virtual ~hkbNodeInternalStateInfo();
	hkbGeneratorSyncInfo m_syncInfo;
	hkStringPtr m_name;
	hkReferencedObject *m_internalState;
	int16 m_nodeId;
	bool m_hasActivateBeenCalled;
};

class hkbBehaviorGraphInternalState : public hkReferencedObject
{
public:
	virtual ~hkbBehaviorGraphInternalState();
	hkArray<hkbNodeInternalStateInfo *> m_nodeInternalStateInfos;
	hkbVariableValueSet *m_variableValueSet;
};

class hkbBehaviorReferenceGenerator : public hkbGenerator
{
public:
	virtual ~hkbBehaviorReferenceGenerator();
	hkStringPtr m_behaviorName;
	void *m_behavior;
};

class hkbBoneWeightArray : public hkbBindable
{
public:
	virtual ~hkbBoneWeightArray();
	hkArray<float> m_boneWeights;
};

class hkbBlenderGeneratorChild : public hkbBindable
{
public:
	virtual ~hkbBlenderGeneratorChild();
	hkbGenerator *m_generator;
	hkbBoneWeightArray *m_boneWeights;
	float m_weight;
	float m_worldFromModelWeight;
};

class hkbBlenderGenerator : public hkbGenerator
{
public:
	virtual ~hkbBlenderGenerator();
	float m_referencePoseWeightThreshold;
	float m_blendParameter;
	float m_minCyclicBlendParameter;
	float m_maxCyclicBlendParameter;
	int16 m_indexOfSyncMasterChild;
	int16 m_flags;
	bool m_subtractLastChild;
	hkArray<hkbBlenderGeneratorChild *> m_children;
	hkArray<void> m_childrenInternalStates;
	hkArray<void> m_sortedChildren;
	float m_endIntervalWeight;
	int32 m_numActiveChildren;
	int16 m_beginIntervalIndex;
	int16 m_endIntervalIndex;
	bool m_initSync;
	bool m_doSubtractiveBlend;
};

class hkbBlenderGeneratorChildInternalState
{
public:
	bool m_isActive;
	bool m_syncNextFrame;
};

class hkbBlenderGeneratorInternalState : public hkReferencedObject
{
public:
	virtual ~hkbBlenderGeneratorInternalState();
	hkArray<hkbBlenderGeneratorChildInternalState> m_childrenInternalStates;
	hkArray<int16> m_sortedChildren;
	float m_endIntervalWeight;
	int32 m_numActiveChildren;
	int16 m_beginIntervalIndex;
	int16 m_endIntervalIndex;
	bool m_initSync;
	bool m_doSubtractiveBlend;
};

class hkbTransitionEffect : public hkbGenerator
{
public:
	int8 m_selfTransitionMode;
	int8 m_eventMode;
	int8 m_defaultEventMode;
};

class hkbBlendingTransitionEffect : public hkbTransitionEffect
{
public:
	virtual ~hkbBlendingTransitionEffect();
	float m_duration;
	float m_toGeneratorStartTimeFraction;
	hkFlags m_flags;
	int8 m_endMode;
	int8 m_blendCurve;
	void *m_fromGenerator;
	void *m_toGenerator;
	hkArray<void> m_characterPoseAtBeginningOfTransition;
	float m_timeRemaining;
	float m_timeInTransition;
	bool m_applySelfTransition;
	bool m_initializeCharacterPose;
};

class hkbBlendingTransitionEffectInternalState : public hkReferencedObject
{
public:
	virtual ~hkbBlendingTransitionEffectInternalState();
	hkArray<hkQsTransform> m_characterPoseAtBeginningOfTransition;
	float m_timeRemaining;
	float m_timeInTransition;
	bool m_applySelfTransition;
	bool m_initializeCharacterPose;
};

class hkbBoneIndexArray : public hkbBindable
{
public:
	virtual ~hkbBoneIndexArray();
	hkArray<int16> m_boneIndices;
};

class hkbBoolVariableSequencedDataSample
{
public:
	float m_time;
	bool m_value;
};

class hkbSequencedData : public hkReferencedObject
{
public:
};

class hkbBoolVariableSequencedData : public hkbSequencedData
{
public:
	virtual ~hkbBoolVariableSequencedData();
	hkArray<hkbBoolVariableSequencedDataSample> m_samples;
	int32 m_variableIndex;
};

class hkbCharacterControllerControlData
{
public:
	hkVector4 m_desiredVelocity;
	float m_verticalGain;
	float m_horizontalCatchUpGain;
	float m_maxVerticalSeparation;
	float m_maxHorizontalSeparation;
};

class hkbCharacterControllerModifier : public hkbModifier
{
public:
	virtual ~hkbCharacterControllerModifier();
	hkbCharacterControllerControlData m_controlData;
	hkVector4 m_initialVelocity;
	int8 m_initialVelocityCoordinates;
	int8 m_motionMode;
	bool m_forceDownwardMomentum;
	bool m_applyGravity;
	bool m_setInitialVelocity;
	bool m_isTouchingGround;
	hkVector4 m_gravity;
	float m_timestep;
	bool m_isInitialVelocityAdded;
};

class hkbCharacterControllerModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbCharacterControllerModifierInternalState();
	hkVector4 m_gravity;
	float m_timestep;
	bool m_isInitialVelocityAdded;
	bool m_isTouchingGround;
};

class hkbClipTrigger
{
public:
	float m_localTime;
	hkbEventProperty m_event;
	bool m_relativeToEndOfClip;
	bool m_acyclic;
	bool m_isAnnotation;
};

class hkbClipTriggerArray : public hkReferencedObject
{
public:
	virtual ~hkbClipTriggerArray();
	hkArray<hkbClipTrigger> m_triggers;
};

class hkbClipGenerator : public hkbGenerator
{
public:
	virtual ~hkbClipGenerator();
	hkStringPtr m_animationName;
	hkbClipTriggerArray *m_triggers;
	float m_cropStartAmountLocalTime;
	float m_cropEndAmountLocalTime;
	float m_startTime;
	float m_playbackSpeed;
	float m_enforcedDuration;
	float m_userControlledTimeFraction;
	int16 m_animationBindingIndex;
	int8 m_mode;
	int8 m_flags;
	hkArray<void> m_animDatas;
	void *m_animationControl;
	void *m_originalTriggers;
	void *m_mapperData;
	void *m_binding;
	void *m_mirroredAnimation;
	hkQsTransform m_extractedMotion;
	hkArray<void> m_echos;
	float m_localTime;
	float m_time;
	float m_previousUserControlledTimeFraction;
	int32 m_bufferSize;
	int32 m_echoBufferSize;
	bool m_atEnd;
	bool m_ignoreStartTime;
	bool m_pingPongBackward;
};

class hkbClipGeneratorEcho
{
public:
	float m_offsetLocalTime;
	float m_weight;
	float m_dwdt;
};

class hkbClipGeneratorInternalState : public hkReferencedObject
{
public:
	virtual ~hkbClipGeneratorInternalState();
	hkQsTransform m_extractedMotion;
	hkArray<hkbClipGeneratorEcho> m_echos;
	float m_localTime;
	float m_time;
	float m_previousUserControlledTimeFraction;
	int32 m_bufferSize;
	int32 m_echoBufferSize;
	bool m_atEnd;
	bool m_ignoreStartTime;
	bool m_pingPongBackward;
};

class hkbCombineTransformsModifier : public hkbModifier
{
public:
	virtual ~hkbCombineTransformsModifier();
	hkVector4 m_translationOut;
	hkQuaternion m_rotationOut;
	hkVector4 m_leftTranslation;
	hkQuaternion m_leftRotation;
	hkVector4 m_rightTranslation;
	hkQuaternion m_rightRotation;
	bool m_invertLeftTransform;
	bool m_invertRightTransform;
	bool m_invertResult;
};

class hkbCombineTransformsModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbCombineTransformsModifierInternalState();
	hkVector4 m_translationOut;
	hkQuaternion m_rotationOut;
};

class hkbCompiledExpressionSetToken
{
public:
	float m_data;
	int8 m_type;
	int8 m_operator;
};

class hkbCompiledExpressionSet : public hkReferencedObject
{
public:
	virtual ~hkbCompiledExpressionSet();
	hkArray<hkbCompiledExpressionSetToken> m_rpn;
	hkArray<int32> m_expressionToRpnIndex;
	int8 m_numExpressions;
};

class hkbComputeDirectionModifier : public hkbModifier
{
public:
	virtual ~hkbComputeDirectionModifier();
	hkVector4 m_pointIn;
	hkVector4 m_pointOut;
	float m_groundAngleOut;
	float m_upAngleOut;
	float m_verticalOffset;
	bool m_reverseGroundAngle;
	bool m_reverseUpAngle;
	bool m_projectPoint;
	bool m_normalizePoint;
	bool m_computeOnlyOnce;
	bool m_computedOutput;
};

class hkbComputeDirectionModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbComputeDirectionModifierInternalState();
	hkVector4 m_pointOut;
	float m_groundAngleOut;
	float m_upAngleOut;
	bool m_computedOutput;
};

class hkbComputeRotationFromAxisAngleModifier : public hkbModifier
{
public:
	virtual ~hkbComputeRotationFromAxisAngleModifier();
	hkQuaternion m_rotationOut;
	hkVector4 m_axis;
	float m_angleDegrees;
};

class hkbComputeRotationFromAxisAngleModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbComputeRotationFromAxisAngleModifierInternalState();
	hkQuaternion m_rotationOut;
};

class hkbComputeRotationToTargetModifier : public hkbModifier
{
public:
	virtual ~hkbComputeRotationToTargetModifier();
	hkQuaternion m_rotationOut;
	hkVector4 m_targetPosition;
	hkVector4 m_currentPosition;
	hkQuaternion m_currentRotation;
	hkVector4 m_localAxisOfRotation;
	hkVector4 m_localFacingDirection;
	bool m_resultIsDelta;
};

class hkbComputeRotationToTargetModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbComputeRotationToTargetModifierInternalState();
	hkQuaternion m_rotationOut;
};

class hkbDampingModifier : public hkbModifier
{
public:
	virtual ~hkbDampingModifier();
	float m_kP;
	float m_kI;
	float m_kD;
	bool m_enableScalarDamping;
	bool m_enableVectorDamping;
	float m_rawValue;
	float m_dampedValue;
	hkVector4 m_rawVector;
	hkVector4 m_dampedVector;
	hkVector4 m_vecErrorSum;
	hkVector4 m_vecPreviousError;
	float m_errorSum;
	float m_previousError;
};

class hkbDampingModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbDampingModifierInternalState();
	hkVector4 m_dampedVector;
	hkVector4 m_vecErrorSum;
	hkVector4 m_vecPreviousError;
	float m_dampedValue;
	float m_errorSum;
	float m_previousError;
};

class hkbModifierWrapper : public hkbModifier
{
public:
	virtual ~hkbModifierWrapper();
	hkbModifier *m_modifier;
};

class hkbDelayedModifier : public hkbModifierWrapper
{
public:
	virtual ~hkbDelayedModifier();
	float m_delaySeconds;
	float m_durationSeconds;
	float m_secondsElapsed;
	bool m_isActive;
};

class hkbDelayedModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbDelayedModifierInternalState();
	float m_secondsElapsed;
	bool m_isActive;
};

class hkbDetectCloseToGroundModifier : public hkbModifier
{
public:
	virtual ~hkbDetectCloseToGroundModifier();
	hkbEventProperty m_closeToGroundEvent;
	float m_closeToGroundHeight;
	float m_raycastDistanceDown;
	uint32 m_collisionFilterInfo;
	int16 m_boneIndex;
	int16 m_animBoneIndex;
	bool m_isCloseToGround;
};

class hkbDetectCloseToGroundModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbDetectCloseToGroundModifierInternalState();
	bool m_isCloseToGround;
};

class hkbExpressionData
{
public:
	hkStringPtr m_expression;
	int32 m_assignmentVariableIndex;
	int32 m_assignmentEventIndex;
	int8 m_eventMode;
	bool m_raisedEvent;
	bool m_wasTrueInPreviousFrame;
};

class hkbExpressionDataArray : public hkReferencedObject
{
public:
	virtual ~hkbExpressionDataArray();
	hkArray<hkbExpressionData> m_expressionsData;
};

class hkbEvaluateExpressionModifier : public hkbModifier
{
public:
	virtual ~hkbEvaluateExpressionModifier();
	hkbExpressionDataArray *m_expressions;
	void *m_compiledExpressionSet;
	hkArray<void> m_internalExpressionsData;
};

class hkbEvaluateExpressionModifierInternalExpressionData
{
public:
	bool m_raisedEvent;
	bool m_wasTrueInPreviousFrame;
};

class hkbEvaluateExpressionModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbEvaluateExpressionModifierInternalState();
	hkArray<hkbEvaluateExpressionModifierInternalExpressionData> m_internalExpressionsData;
};

class hkbEvaluateHandleModifier : public hkbModifier
{
public:
	virtual ~hkbEvaluateHandleModifier();
	hkbHandle *m_handle;
	hkVector4 m_handlePositionOut;
	hkQuaternion m_handleRotationOut;
	bool m_isValidOut;
	float m_extrapolationTimeStep;
	float m_handleChangeSpeed;
	int8 m_handleChangeMode;
	hkbHandle m_oldHandle;
	hkVector4 m_oldHandlePosition;
	hkQuaternion m_oldHandleRotation;
	float m_timeSinceLastModify;
	bool m_smoothlyChangingHandles;
};

class hkbEventDrivenModifier : public hkbModifierWrapper
{
public:
	virtual ~hkbEventDrivenModifier();
	int32 m_activateEventId;
	int32 m_deactivateEventId;
	bool m_activeByDefault;
	bool m_isActive;
};

class hkbEventDrivenModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbEventDrivenModifierInternalState();
	bool m_isActive;
};

class hkbEventPayloadList : public hkbEventPayload
{
public:
	virtual ~hkbEventPayloadList();
	hkArray<hkbEventPayload *> m_payloads;
};

class hkbEventRangeData
{
public:
	float m_upperBound;
	hkbEventProperty m_event;
	int8 m_eventMode;
};

class hkbEventRangeDataArray : public hkReferencedObject
{
public:
	virtual ~hkbEventRangeDataArray();
	hkArray<hkbEventRangeData> m_eventData;
};

class hkbEvent : public hkbEventBase
{
public:
	void *m_sender;
};

class hkbEventSequencedDataSequencedEvent
{
public:
	hkbEvent m_event;
	float m_time;
};

class hkbEventSequencedData : public hkbSequencedData
{
public:
	virtual ~hkbEventSequencedData();
	hkArray<hkbEventSequencedDataSequencedEvent> m_events;
};

class hkbEventsFromRangeModifier : public hkbModifier
{
public:
	virtual ~hkbEventsFromRangeModifier();
	float m_inputValue;
	float m_lowerBound;
	hkbEventRangeDataArray *m_eventRanges;
	hkArray<void> m_wasActiveInPreviousFrame;
};

class hkbEventsFromRangeModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbEventsFromRangeModifierInternalState();
	hkArray<bool> m_wasActiveInPreviousFrame;
};

class hkbCondition : public hkReferencedObject
{
public:
};

class hkbExpressionCondition : public hkbCondition
{
public:
	virtual ~hkbExpressionCondition();
	hkStringPtr m_expression;
	void *m_compiledExpressionSet;
};

class hkbExtractRagdollPoseModifier : public hkbModifier
{
public:
	virtual ~hkbExtractRagdollPoseModifier();
	int16 m_poseMatchingBone0;
	int16 m_poseMatchingBone1;
	int16 m_poseMatchingBone2;
	bool m_enableComputeWorldFromModel;
};

class hkbExtrapolatingTransitionEffect : public hkbBlendingTransitionEffect
{
public:
	virtual ~hkbExtrapolatingTransitionEffect();
	hkbGeneratorSyncInfo m_fromGeneratorSyncInfo;
	hkQsTransform m_worldFromModel;
	hkQsTransform m_motion;
	hkArray<void> m_pose;
	hkArray<void> m_additivePose;
	float m_toGeneratorDuration;
	bool m_isFromGeneratorActive;
	bool m_gotPose;
	bool m_gotAdditivePose;
};

class hkbExtrapolatingTransitionEffectInternalState : public hkReferencedObject
{
public:
	virtual ~hkbExtrapolatingTransitionEffectInternalState();
	hkbGeneratorSyncInfo m_fromGeneratorSyncInfo;
	hkQsTransform m_worldFromModel;
	hkQsTransform m_motion;
	hkArray<hkQsTransform> m_pose;
	hkArray<hkQsTransform> m_additivePose;
	float m_toGeneratorDuration;
	bool m_isFromGeneratorActive;
	bool m_gotPose;
	bool m_gotAdditivePose;
};

class hkbFootIkGains
{
public:
	float m_onOffGain;
	float m_groundAscendingGain;
	float m_groundDescendingGain;
	float m_footPlantedGain;
	float m_footRaisedGain;
	float m_footUnlockGain;
	float m_worldFromModelFeedbackGain;
	float m_errorUpDownBias;
	float m_alignWorldFromModelGain;
	float m_hipOrientationGain;
	float m_maxKneeAngleDifference;
	float m_ankleOrientationGain;
};

class hkbFootIkControlData
{
public:
	hkbFootIkGains m_gains;
};

class hkbFootIkControlsModifierLeg
{
public:
	hkVector4 m_groundPosition;
	hkbEventProperty m_ungroundedEvent;
	float m_verticalError;
	bool m_hitSomething;
	bool m_isPlantedMS;
};

class hkbFootIkControlsModifier : public hkbModifier
{
public:
	virtual ~hkbFootIkControlsModifier();
	hkbFootIkControlData m_controlData;
	hkArray<hkbFootIkControlsModifierLeg> m_legs;
	hkVector4 m_errorOutTranslation;
	hkQuaternion m_alignWithGroundRotation;
};

class hkbFootIkModifierLeg
{
public:
	hkQsTransform m_originalAnkleTransformMS;
	hkQuaternion m_prevAnkleRotLS;
	hkVector4 m_kneeAxisLS;
	hkVector4 m_footEndLS;
	hkbEventProperty m_ungroundedEvent;
	float m_footPlantedAnkleHeightMS;
	float m_footRaisedAnkleHeightMS;
	float m_maxAnkleHeightMS;
	float m_minAnkleHeightMS;
	float m_maxKneeAngleDegrees;
	float m_minKneeAngleDegrees;
	float m_verticalError;
	float m_maxAnkleAngleDegrees;
	int16 m_hipIndex;
	int16 m_kneeIndex;
	int16 m_ankleIndex;
	bool m_hitSomething;
	bool m_isPlantedMS;
	bool m_isOriginalAnkleTransformMSSet;
};

class hkbFootIkModifierInternalLegData
{
public:
	hkVector4 m_groundPosition;
	void *m_footIkSolver;
};

class hkbFootIkModifier : public hkbModifier
{
public:
	virtual ~hkbFootIkModifier();
	hkbFootIkGains m_gains;
	hkArray<hkbFootIkModifierLeg> m_legs;
	float m_raycastDistanceUp;
	float m_raycastDistanceDown;
	float m_originalGroundHeightMS;
	float m_errorOut;
	hkVector4 m_errorOutTranslation;
	hkQuaternion m_alignWithGroundRotation;
	float m_verticalOffset;
	uint32 m_collisionFilterInfo;
	float m_forwardAlignFraction;
	float m_sidewaysAlignFraction;
	float m_sidewaysSampleWidth;
	bool m_useTrackData;
	bool m_lockFeetWhenPlanted;
	bool m_useCharacterUpVector;
	int8 m_alignMode;
	hkArray<hkbFootIkModifierInternalLegData> m_internalLegData;
	float m_prevIsFootIkEnabled;
	bool m_isSetUp;
	bool m_isGroundPositionValid;
	float m_timeStep;
};

class hkbGeneratorTransitionEffect : public hkbTransitionEffect
{
public:
	virtual ~hkbGeneratorTransitionEffect();
	hkbGenerator *m_transitionGenerator;
	float m_blendInDuration;
	float m_blendOutDuration;
	bool m_syncToGeneratorStartTime;
	void *m_fromGenerator;
	void *m_toGenerator;
	float m_timeInTransition;
	float m_duration;
	float m_effectiveBlendInDuration;
	float m_effectiveBlendOutDuration;
	int8 m_toGeneratorState;
	bool m_echoTransitionGenerator;
	bool m_echoToGenerator;
	bool m_justActivated;
	bool m_updateActiveNodes;
	int8 m_stage;
};

class hkbGeneratorTransitionEffectInternalState : public hkReferencedObject
{
public:
	virtual ~hkbGeneratorTransitionEffectInternalState();
	float m_timeInTransition;
	float m_duration;
	float m_effectiveBlendInDuration;
	float m_effectiveBlendOutDuration;
	int8 m_toGeneratorState;
	bool m_echoTransitionGenerator;
	bool m_echoToGenerator;
	bool m_justActivated;
	bool m_updateActiveNodes;
	int8 m_stage;
};

class hkbGetHandleOnBoneModifier : public hkbModifier
{
public:
	virtual ~hkbGetHandleOnBoneModifier();
	hkbHandle *m_handleOut;
	hkStringPtr m_localFrameName;
	int16 m_ragdollBoneIndex;
	int16 m_animationBoneIndex;
};

class hkbGetUpModifier : public hkbModifier
{
public:
	virtual ~hkbGetUpModifier();
	hkVector4 m_groundNormal;
	float m_duration;
	float m_alignWithGroundDuration;
	int16 m_rootBoneIndex;
	int16 m_otherBoneIndex;
	int16 m_anotherBoneIndex;
	float m_timeSinceBegin;
	float m_timeStep;
	bool m_initNextModify;
};

class hkbGetUpModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbGetUpModifierInternalState();
	float m_timeSinceBegin;
	float m_timeStep;
	bool m_initNextModify;
};

class hkbGetWorldFromModelModifier : public hkbModifier
{
public:
	virtual ~hkbGetWorldFromModelModifier();
	hkVector4 m_translationOut;
	hkQuaternion m_rotationOut;
};

class hkbGetWorldFromModelModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbGetWorldFromModelModifierInternalState();
	hkVector4 m_translationOut;
	hkQuaternion m_rotationOut;
};

class hkbHandIkControlData
{
public:
	hkVector4 m_targetPosition;
	hkQuaternion m_targetRotation;
	hkVector4 m_targetNormal;
	hkbHandle *m_targetHandle;
	float m_transformOnFraction;
	float m_normalOnFraction;
	float m_fadeInDuration;
	float m_fadeOutDuration;
	float m_extrapolationTimeStep;
	float m_handleChangeSpeed;
	int8 m_handleChangeMode;
	bool m_fixUp;
};

class hkbHandIkControlsModifierHand
{
public:
	hkbHandIkControlData m_controlData;
	int32 m_handIndex;
	bool m_enable;
};

class hkbHandIkControlsModifier : public hkbModifier
{
public:
	virtual ~hkbHandIkControlsModifier();
	hkArray<hkbHandIkControlsModifierHand> m_hands;
};

class hkbHandIkModifierHand
{
public:
	hkVector4 m_elbowAxisLS;
	hkVector4 m_backHandNormalLS;
	hkVector4 m_handOffsetLS;
	hkQuaternion m_handOrienationOffsetLS;
	float m_maxElbowAngleDegrees;
	float m_minElbowAngleDegrees;
	int16 m_shoulderIndex;
	int16 m_shoulderSiblingIndex;
	int16 m_elbowIndex;
	int16 m_elbowSiblingIndex;
	int16 m_wristIndex;
	bool m_enforceEndPosition;
	bool m_enforceEndRotation;
	hkStringPtr m_localFrameName;
};

class hkbHandIkModifier : public hkbModifier
{
public:
	virtual ~hkbHandIkModifier();
	hkArray<hkbHandIkModifierHand> m_hands;
	int8 m_fadeInOutCurve;
	hkArray<void> m_internalHandData;
};

class hkbIntEventPayload : public hkbEventPayload
{
public:
	virtual ~hkbIntEventPayload();
	int32 m_data;
};

class hkbIntVariableSequencedDataSample
{
public:
	float m_time;
	int32 m_value;
};

class hkbIntVariableSequencedData : public hkbSequencedData
{
public:
	virtual ~hkbIntVariableSequencedData();
	hkArray<hkbIntVariableSequencedDataSample> m_samples;
	int32 m_variableIndex;
};

class hkbJigglerGroup : public hkbBindable
{
public:
	virtual ~hkbJigglerGroup();
	hkbBoneIndexArray *m_boneIndices;
	float m_mass;
	float m_stiffness;
	float m_damping;
	float m_maxElongation;
	float m_maxCompression;
	bool m_propagateToChildren;
	bool m_affectSiblings;
	bool m_rotateBonesForSkinning;
	bool m_pad;
};

class hkbJigglerModifier : public hkbModifier
{
public:
	virtual ~hkbJigglerModifier();
	hkArray<hkbJigglerGroup *> m_jigglerGroups;
	int8 m_jiggleCoordinates;
	hkArray<void> m_currentVelocitiesWS;
	hkArray<void> m_currentPositions;
	float m_timeStep;
	bool m_initNextModify;
};

class hkbJigglerModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbJigglerModifierInternalState();
	hkArray<hkVector4> m_currentVelocitiesWS;
	hkArray<hkVector4> m_currentPositions;
	float m_timeStep;
	bool m_initNextModify;
};

class hkbKeyframeBonesModifierKeyframeInfo
{
public:
	hkVector4 m_keyframedPosition;
	hkQuaternion m_keyframedRotation;
	int16 m_boneIndex;
	bool m_isValid;
};

class hkbKeyframeBonesModifier : public hkbModifier
{
public:
	virtual ~hkbKeyframeBonesModifier();
	hkArray<hkbKeyframeBonesModifierKeyframeInfo> m_keyframeInfo;
	hkbBoneIndexArray *m_keyframedBonesList;
};

class hkbLookAtModifier : public hkbModifier
{
public:
	virtual ~hkbLookAtModifier();
	hkVector4 m_targetWS;
	hkVector4 m_headForwardLS;
	hkVector4 m_neckForwardLS;
	hkVector4 m_neckRightLS;
	hkVector4 m_eyePositionHS;
	float m_newTargetGain;
	float m_onGain;
	float m_offGain;
	float m_limitAngleDegrees;
	float m_limitAngleLeft;
	float m_limitAngleRight;
	float m_limitAngleUp;
	float m_limitAngleDown;
	int16 m_headIndex;
	int16 m_neckIndex;
	bool m_isOn;
	bool m_individualLimitsOn;
	bool m_isTargetInsideLimitCone;
	hkVector4 m_lookAtLastTargetWS;
	float m_lookAtWeight;
};

class hkbLookAtModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbLookAtModifierInternalState();
	hkVector4 m_lookAtLastTargetWS;
	float m_lookAtWeight;
	bool m_isTargetInsideLimitCone;
};

class hkbManualSelectorGenerator : public hkbGenerator
{
public:
	virtual ~hkbManualSelectorGenerator();
	hkArray<hkbGenerator *> m_generators;
	int8 m_selectedGeneratorIndex;
	int8 m_currentGeneratorIndex;
};

class hkbManualSelectorGeneratorInternalState : public hkReferencedObject
{
public:
	virtual ~hkbManualSelectorGeneratorInternalState();
	int8 m_currentGeneratorIndex;
};

class hkbMirrorModifier : public hkbModifier
{
public:
	virtual ~hkbMirrorModifier();
	bool m_isAdditive;
};

class hkbModifierGenerator : public hkbGenerator
{
public:
	virtual ~hkbModifierGenerator();
	hkbModifier *m_modifier;
	hkbGenerator *m_generator;
};

class hkbModifierList : public hkbModifier
{
public:
	virtual ~hkbModifierList();
	hkArray<hkbModifier *> m_modifiers;
};

class hkbMoveCharacterModifier : public hkbModifier
{
public:
	virtual ~hkbMoveCharacterModifier();
	hkVector4 m_offsetPerSecondMS;
	float m_timeSinceLastModify;
};

class hkbMoveCharacterModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbMoveCharacterModifierInternalState();
	float m_timeSinceLastModify;
};

class hkbNamedEventPayload : public hkbEventPayload
{
public:
	virtual ~hkbNamedEventPayload();
	hkStringPtr m_name;
};

class hkbNamedIntEventPayload : public hkbNamedEventPayload
{
public:
	virtual ~hkbNamedIntEventPayload();
	int32 m_data;
};

class hkbNamedRealEventPayload : public hkbNamedEventPayload
{
public:
	virtual ~hkbNamedRealEventPayload();
	float m_data;
};

class hkbNamedStringEventPayload : public hkbNamedEventPayload
{
public:
	virtual ~hkbNamedStringEventPayload();
	hkStringPtr m_data;
};

class hkbPoseMatchingGenerator : public hkbBlenderGenerator
{
public:
	virtual ~hkbPoseMatchingGenerator();
	hkQuaternion m_worldFromModelRotation;
	float m_blendSpeed;
	float m_minSpeedToSwitch;
	float m_minSwitchTimeNoError;
	float m_minSwitchTimeFullError;
	int32 m_startPlayingEventId;
	int32 m_startMatchingEventId;
	int16 m_rootBoneIndex;
	int16 m_otherBoneIndex;
	int16 m_anotherBoneIndex;
	int16 m_pelvisIndex;
	int8 m_mode;
	int32 m_currentMatch;
	int32 m_bestMatch;
	float m_timeSinceBetterMatch;
	float m_error;
	bool m_resetCurrentMatchLocalTime;
	void *m_poseMatchingUtility;
};

class hkbPoseMatchingGeneratorInternalState : public hkReferencedObject
{
public:
	virtual ~hkbPoseMatchingGeneratorInternalState();
	int32 m_currentMatch;
	int32 m_bestMatch;
	float m_timeSinceBetterMatch;
	float m_error;
	bool m_resetCurrentMatchLocalTime;
};

class hkbRegisteredGenerator : public hkbBindable
{
public:
	virtual ~hkbRegisteredGenerator();
	hkbGenerator *m_generator;
	hkVector4 m_relativePosition;
	hkVector4 m_relativeDirection;
};

class hkbPositionRelativeSelectorGenerator : public hkbGenerator
{
public:
	virtual ~hkbPositionRelativeSelectorGenerator();
	hkArray<hkbRegisteredGenerator *> m_registeredGenerators;
	hkbGenerator *m_blendToFixPositionGenerator;
	bool m_autoComputeEntryPoints;
	float m_transitionTime;
	bool m_useCharacterForward;
	hkVector4 m_characterForward;
	hkVector4 m_targetPosition;
	hkQuaternion m_targetRotation;
	float m_positionTolerance;
	float m_fixPositionTolerance;
	hkbEventProperty m_fixPositionEvent;
	int32 m_endFixPositionEventId;
	bool m_useManualSelection;
	int8 m_selectedGeneratorIndex;
	hkVector4 m_entryPosition;
	hkVector4 m_entryForward;
	hkVector4 m_lastTargetPos;
	hkQuaternion m_lastTargetRot;
	hkVector4 m_targetLinearDisp;
	hkQuaternion m_targetAngularDisp;
	float m_clipTime;
	int8 m_currentGeneratorIndex;
	bool m_usingBlendToFixPositionGenerator;
	bool m_doLeadInFixup;
};

class hkbPositionRelativeSelectorGeneratorInternalState : public hkReferencedObject
{
public:
	virtual ~hkbPositionRelativeSelectorGeneratorInternalState();
	hkVector4 m_entryPosition;
	hkVector4 m_entryForward;
	hkVector4 m_lastTargetPos;
	hkQuaternion m_lastTargetRot;
	hkVector4 m_targetLinearDisp;
	hkQuaternion m_targetAngularDisp;
	float m_clipTime;
	int8 m_currentGeneratorIndex;
	bool m_usingBlendToFixPositionGenerator;
	bool m_doLeadInFixup;
};

class hkbPoweredRagdollControlData
{
public:
	float m_maxForce;
	float m_tau;
	float m_damping;
	float m_proportionalRecoveryVelocity;
	float m_constantRecoveryVelocity;
};

class hkbWorldFromModelModeData
{
public:
	int16 m_poseMatchingBone0;
	int16 m_poseMatchingBone1;
	int16 m_poseMatchingBone2;
	int8 m_mode;
};

class hkbPoweredRagdollControlsModifier : public hkbModifier
{
public:
	virtual ~hkbPoweredRagdollControlsModifier();
	hkbPoweredRagdollControlData m_controlData;
	hkbBoneIndexArray *m_bones;
	hkbWorldFromModelModeData m_worldFromModelModeData;
	hkbBoneWeightArray *m_boneWeights;
};

class hkbProxyModifierProxyInfo
{
public:
	float m_dynamicFriction;
	float m_staticFriction;
	float m_keepContactTolerance;
	hkVector4 m_up;
	float m_keepDistance;
	float m_contactAngleSensitivity;
	uint32 m_userPlanes;
	float m_maxCharacterSpeedForSolver;
	float m_characterStrength;
	float m_characterMass;
	float m_maxSlope;
	float m_penetrationRecoverySpeed;
	int32 m_maxCastIterations;
	bool m_refreshManifoldInCheckSupport;
};

class hkbProxyModifier : public hkbModifier
{
public:
	virtual ~hkbProxyModifier();
	hkbProxyModifierProxyInfo m_proxyInfo;
	hkVector4 m_linearVelocity;
	float m_horizontalGain;
	float m_verticalGain;
	float m_maxHorizontalSeparation;
	float m_maxVerticalSeparation;
	float m_verticalDisplacementError;
	float m_verticalDisplacementErrorGain;
	float m_maxVerticalDisplacement;
	float m_minVerticalDisplacement;
	float m_capsuleHeight;
	float m_capsuleRadius;
	float m_maxSlopeForRotation;
	uint32 m_collisionFilterInfo;
	int8 m_phantomType;
	int8 m_linearVelocityMode;
	bool m_ignoreIncomingRotation;
	bool m_ignoreCollisionDuringRotation;
	bool m_ignoreIncomingTranslation;
	bool m_includeDownwardMomentum;
	bool m_followWorldFromModel;
	bool m_isTouchingGround;
	void *m_characterProxy;
	void *m_phantom;
	void *m_phantomShape;
	hkVector4 m_horizontalDisplacement;
	float m_verticalDisplacement;
	float m_timestep;
	bool m_previousFrameFollowWorldFromModel;
};

class hkbRealEventPayload : public hkbEventPayload
{
public:
	virtual ~hkbRealEventPayload();
	float m_data;
};

class hkbRealVariableSequencedDataSample
{
public:
	float m_time;
	float m_value;
};

class hkbRealVariableSequencedData : public hkbSequencedData
{
public:
	virtual ~hkbRealVariableSequencedData();
	hkArray<hkbRealVariableSequencedDataSample> m_samples;
	int32 m_variableIndex;
};

class hkbReferencePoseGenerator : public hkbGenerator
{
public:
	virtual ~hkbReferencePoseGenerator();
	void *m_skeleton;
};

class hkaKeyFrameHierarchyUtilityControlData
{
public:
	float m_hierarchyGain;
	float m_velocityDamping;
	float m_accelerationGain;
	float m_velocityGain;
	float m_positionGain;
	float m_positionMaxLinearVelocity;
	float m_positionMaxAngularVelocity;
	float m_snapGain;
	float m_snapMaxLinearVelocity;
	float m_snapMaxAngularVelocity;
	float m_snapMaxLinearDistance;
	float m_snapMaxAngularDistance;
};

class hkbRigidBodyRagdollControlData
{
public:
	hkaKeyFrameHierarchyUtilityControlData m_keyFrameHierarchyControlData;
	float m_durationToBlend;
};

class hkbRigidBodyRagdollControlsModifier : public hkbModifier
{
public:
	virtual ~hkbRigidBodyRagdollControlsModifier();
	hkbRigidBodyRagdollControlData m_controlData;
	hkbBoneIndexArray *m_bones;
};

class hkbRotateCharacterModifier : public hkbModifier
{
public:
	virtual ~hkbRotateCharacterModifier();
	float m_degreesPerSecond;
	float m_speedMultiplier;
	hkVector4 m_axisOfRotation;
	float m_angle;
};

class hkbRotateCharacterModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbRotateCharacterModifierInternalState();
	float m_angle;
};

class hkbSenseHandleModifierRange
{
public:
	hkbEventProperty m_event;
	float m_minDistance;
	float m_maxDistance;
	bool m_ignoreHandle;
};

class hkbSenseHandleModifier : public hkbModifier
{
public:
	virtual ~hkbSenseHandleModifier();
	hkbHandle m_handle;
	hkVector4 m_sensorLocalOffset;
	hkArray<hkbSenseHandleModifierRange> m_ranges;
	hkbHandle *m_handleOut;
	hkbHandle *m_handleIn;
	hkStringPtr m_localFrameName;
	hkStringPtr m_sensorLocalFrameName;
	float m_minDistance;
	float m_maxDistance;
	float m_distanceOut;
	uint32 m_collisionFilterInfo;
	int16 m_sensorRagdollBoneIndex;
	int16 m_sensorAnimationBoneIndex;
	int8 m_sensingMode;
	bool m_extrapolateSensorPosition;
	bool m_keepFirstSensedHandle;
	bool m_foundHandleOut;
	float m_timeSinceLastModify;
	int32 m_rangeIndexForEventToSendNextUpdate;
};

class hkbSequenceStringData : public hkReferencedObject
{
public:
	virtual ~hkbSequenceStringData();
	hkArray<hkStringPtr> m_eventNames;
	hkArray<hkStringPtr> m_variableNames;
};

class hkbSequence : public hkbModifier
{
public:
	virtual ~hkbSequence();
	hkArray<hkbEventSequencedData *> m_eventSequencedData;
	hkArray<hkbRealVariableSequencedData *> m_realVariableSequencedData;
	hkArray<hkbBoolVariableSequencedData *> m_boolVariableSequencedData;
	hkArray<hkbIntVariableSequencedData *> m_intVariableSequencedData;
	int32 m_enableEventId;
	int32 m_disableEventId;
	hkbSequenceStringData *m_stringData;
	void *m_variableIdMap;
	void *m_eventIdMap;
	hkArray<void> m_nextSampleEvents;
	hkArray<void> m_nextSampleReals;
	hkArray<void> m_nextSampleBools;
	hkArray<void> m_nextSampleInts;
	float m_time;
	bool m_isEnabled;
};

class hkbSequenceInternalState : public hkReferencedObject
{
public:
	virtual ~hkbSequenceInternalState();
	hkArray<int32> m_nextSampleEvents;
	hkArray<int32> m_nextSampleReals;
	hkArray<int32> m_nextSampleBools;
	hkArray<int32> m_nextSampleInts;
	float m_time;
	bool m_isEnabled;
};

class hkbSetWorldFromModelModifier : public hkbModifier
{
public:
	virtual ~hkbSetWorldFromModelModifier();
	hkVector4 m_translation;
	hkQuaternion m_rotation;
	bool m_setTranslation;
	bool m_setRotation;
};

class hkbSplinePathGenerator : public hkbGenerator
{
public:
	virtual ~hkbSplinePathGenerator();
	hkArray<hkbRegisteredGenerator *> m_registeredGenerators;
	hkVector4 m_characterForward;
	hkVector4 m_targetPosition;
	hkVector4 m_targetDirection;
	hkbEventProperty m_pathEndEvent;
	float m_leadInGain;
	float m_leadOutGain;
	bool m_useProximityTrigger;
	float m_endEventProximity;
	float m_endEventTime;
	int8 m_selectedGeneratorIndex;
	bool m_useManualSelection;
	bool m_trackPosition;
	bool m_usePathEstimation;
	bool m_useCharacterForward;
	hkVector4 m_entryPosition;
	hkVector4 m_entryForward;
	hkVector4 m_exitPosition;
	hkVector4 m_exitForward;
	float m_averageSpeed;
	float m_pathTime;
	float m_curTime;
	float m_pathParam;
	int8 m_currentGeneratorIndex;
	bool m_doLeadInFixup;
	bool m_eventTriggered;
};

class hkbSplinePathGeneratorInternalState : public hkReferencedObject
{
public:
	virtual ~hkbSplinePathGeneratorInternalState();
	hkVector4 m_entryPosition;
	hkVector4 m_entryForward;
	hkVector4 m_exitPosition;
	hkVector4 m_exitForward;
	float m_averageSpeed;
	float m_pathTime;
	float m_curTime;
	float m_pathParam;
	int8 m_currentGeneratorIndex;
	bool m_doLeadInFixup;
	bool m_eventTriggered;
};

class hkbStateListener : public hkReferencedObject
{
public:
	virtual ~hkbStateListener();
};

class hkbStateChooser : public hkReferencedObject
{
public:
};

class hkbStateMachineEventPropertyArray : public hkReferencedObject
{
public:
	virtual ~hkbStateMachineEventPropertyArray();
	hkArray<hkbEventProperty> m_events;
};

class hkbStateMachineTimeInterval
{
public:
	int32 m_enterEventId;
	int32 m_exitEventId;
	float m_enterTime;
	float m_exitTime;
};

class hkbStateMachineTransitionInfo
{
public:
	hkbStateMachineTimeInterval m_triggerInterval;
	hkbStateMachineTimeInterval m_initiateInterval;
	hkbTransitionEffect *m_transition;
	hkbCondition *m_condition;
	int32 m_eventId;
	int32 m_toStateId;
	int32 m_fromNestedStateId;
	int32 m_toNestedStateId;
	int16 m_priority;
	hkFlags m_flags;
};

class hkbStateMachineTransitionInfoArray : public hkReferencedObject
{
public:
	virtual ~hkbStateMachineTransitionInfoArray();
	hkArray<hkbStateMachineTransitionInfo> m_transitions;
};

class hkbStateMachineStateInfo : public hkbBindable
{
public:
	virtual ~hkbStateMachineStateInfo();
	hkArray<hkbStateListener *> m_listeners;
	hkbStateMachineEventPropertyArray *m_enterNotifyEvents;
	hkbStateMachineEventPropertyArray *m_exitNotifyEvents;
	hkbStateMachineTransitionInfoArray *m_transitions;
	hkbGenerator *m_generator;
	hkStringPtr m_name;
	int32 m_stateId;
	float m_probability;
	bool m_enable;
};

class hkbStateMachine : public hkbGenerator
{
public:
	virtual ~hkbStateMachine();
	hkbEvent m_eventToSendWhenStateOrTransitionChanges;
	hkbStateChooser *m_startStateChooser;
	int32 m_startStateId;
	int32 m_returnToPreviousStateEventId;
	int32 m_randomTransitionEventId;
	int32 m_transitionToNextHigherStateEventId;
	int32 m_transitionToNextLowerStateEventId;
	int32 m_syncVariableIndex;
	int32 m_currentStateId;
	bool m_wrapAroundStateId;
	int8 m_maxSimultaneousTransitions;
	int8 m_startStateMode;
	int8 m_selfTransitionMode;
	bool m_isActive;
	hkArray<hkbStateMachineStateInfo *> m_states;
	hkbStateMachineTransitionInfoArray *m_wildcardTransitions;
	void *m_stateIdToIndexMap;
	hkArray<void> m_activeTransitions;
	hkArray<void> m_transitionFlags;
	hkArray<void> m_wildcardTransitionFlags;
	hkArray<void> m_delayedTransitions;
	float m_timeInState;
	float m_lastLocalTime;
	int32 m_previousStateId;
	int32 m_nextStartStateIndexOverride;
	bool m_stateOrTransitionChanged;
	bool m_echoNextUpdate;
	uint16 m_sCurrentStateIndexAndEntered;
};

class hkbStateMachineTransitionInfoReference
{
public:
	int16 m_fromStateIndex;
	int16 m_transitionIndex;
	int16 m_stateMachineId;
};

class hkbStateMachineProspectiveTransitionInfo
{
public:
	hkbStateMachineTransitionInfoReference m_transitionInfoReference;
	hkbStateMachineTransitionInfoReference m_transitionInfoReferenceForTE;
	int32 m_toStateId;
};

class hkbStateMachineDelayedTransitionInfo
{
public:
	hkbStateMachineProspectiveTransitionInfo m_delayedTransition;
	float m_timeDelayed;
	bool m_isDelayedTransitionReturnToPreviousState;
	bool m_wasInAbutRangeLastFrame;
};

class hkbStateMachineActiveTransitionInfo
{
public:
	void *m_transitionEffect;
	hkbNodeInternalStateInfo *m_transitionEffectInternalStateInfo;
	hkbStateMachineTransitionInfoReference m_transitionInfoReference;
	hkbStateMachineTransitionInfoReference m_transitionInfoReferenceForTE;
	int32 m_fromStateId;
	int32 m_toStateId;
	bool m_isReturnToPreviousState;
};

class hkbStateMachineInternalState : public hkReferencedObject
{
public:
	virtual ~hkbStateMachineInternalState();
	hkArray<hkbStateMachineActiveTransitionInfo> m_activeTransitions;
	hkArray<uint8> m_transitionFlags;
	hkArray<uint8> m_wildcardTransitionFlags;
	hkArray<hkbStateMachineDelayedTransitionInfo> m_delayedTransitions;
	float m_timeInState;
	float m_lastLocalTime;
	int32 m_currentStateId;
	int32 m_previousStateId;
	int32 m_nextStartStateIndexOverride;
	bool m_stateOrTransitionChanged;
	bool m_echoNextUpdate;
};

class hkbStringCondition : public hkbCondition
{
public:
	virtual ~hkbStringCondition();
	hkStringPtr m_conditionString;
};

class hkbStringEventPayload : public hkbEventPayload
{
public:
	virtual ~hkbStringEventPayload();
	hkStringPtr m_data;
};

class hkbTimerModifier : public hkbModifier
{
public:
	virtual ~hkbTimerModifier();
	float m_alarmTimeSeconds;
	hkbEventProperty m_alarmEvent;
	float m_secondsElapsed;
};

class hkbTimerModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbTimerModifierInternalState();
	float m_secondsElapsed;
};

class hkbTransformVectorModifier : public hkbModifier
{
public:
	virtual ~hkbTransformVectorModifier();
	hkQuaternion m_rotation;
	hkVector4 m_translation;
	hkVector4 m_vectorIn;
	hkVector4 m_vectorOut;
	bool m_rotateOnly;
	bool m_inverse;
	bool m_computeOnActivate;
	bool m_computeOnModify;
};

class hkbTransformVectorModifierInternalState : public hkReferencedObject
{
public:
	virtual ~hkbTransformVectorModifierInternalState();
	hkVector4 m_vectorOut;
};

class hkbTwistModifier : public hkbModifier
{
public:
	virtual ~hkbTwistModifier();
	hkVector4 m_axisOfRotation;
	float m_twistAngle;
	int16 m_startBoneIndex;
	int16 m_endBoneIndex;
	int8 m_setAngleMethod;
	int8 m_rotationAxisCoordinates;
	bool m_isAdditive;
	hkArray<void> m_boneChainIndices;
	hkArray<void> m_parentBoneIndices;
};

class hkbAlignBoneModifier : public hkbModifier
{
public:
	virtual ~hkbAlignBoneModifier();
	int8 m_alignMode;
	int8 m_alignTargetMode;
	bool m_alignSingleAxis;
	hkVector4 m_alignAxis;
	hkVector4 m_alignTargetAxis;
	hkQuaternion m_frameOfReference;
	float m_duration;
	int32 m_alignModeIndex;
	int32 m_alignTargetModeIndex;
	float m_time;
	float m_timeSinceLastModify;
};

class hkbAnimatedSkeletonGenerator : public hkbGenerator
{
public:
	virtual ~hkbAnimatedSkeletonGenerator();
	void *m_animatedSkeleton;
};

class hkbAuxiliaryNodeInfo : public hkReferencedObject
{
public:
	virtual ~hkbAuxiliaryNodeInfo();
	uint8 m_type;
	uint8 m_depth;
	hkStringPtr m_referenceBehaviorName;
	hkArray<hkStringPtr> m_selfTransitionNames;
};

class hkbBalanceModifierStepInfo
{
public:
	int16 m_boneIndex;
	float m_fractionOfSolution;
};

class hkbBalanceModifier : public hkbModifier
{
public:
	virtual ~hkbBalanceModifier();
	bool m_giveUp;
	float m_comDistThreshold;
	bool m_passThrough;
	int16 m_ragdollLeftFootBoneIndex;
	int16 m_ragdollRightFootBoneIndex;
	float m_balanceOnAnklesFraction;
	int32 m_upAxis;
	float m_fadeInTime;
	float m_comBiasX;
	hkArray<hkbBalanceModifierStepInfo> m_stepInfo;
	float m_timeLapsed;
};

class hkbRadialSelectorGeneratorGeneratorInfo
{
public:
	hkbGenerator *m_generator;
	float m_angle;
	float m_radialSpeed;
};

class hkbRadialSelectorGeneratorGeneratorPair
{
public:
	hkbRadialSelectorGeneratorGeneratorInfo m_generators;
	float m_minAngle;
	float m_maxAngle;
};

class hkbRadialSelectorGenerator : public hkbGenerator
{
public:
	virtual ~hkbRadialSelectorGenerator();
	hkArray<hkbRadialSelectorGeneratorGeneratorPair> m_generatorPairs;
	float m_angle;
	float m_radius;
	int32 m_currentGeneratorPairIndex;
	int32 m_currentEndpointIndex;
	float m_currentFraction;
	bool m_hasSetLocalTime;
};

class hkbCheckBalanceModifier : public hkbModifier
{
public:
	virtual ~hkbCheckBalanceModifier();
	int16 m_ragdollLeftFootBoneIndex;
	int16 m_ragdollRightFootBoneIndex;
	float m_balanceOnAnklesFraction;
	hkbEvent m_eventToSendWhenOffBalance;
	float m_offBalanceEventThreshold;
	int32 m_worldUpAxisIndex;
	float m_comBiasX;
	bool m_extractRagdollPose;
	float m_mass;
	hkVector4 m_comWS;
	hkVector4 m_desiredComWS;
	float m_offBalanceDistance;
	hkVector4 m_errorMS;
};

class hkbBalanceRadialSelectorGenerator : public hkbRadialSelectorGenerator
{
public:
	virtual ~hkbBalanceRadialSelectorGenerator();
	int32 m_xAxisMS;
	int32 m_yAxisMS;
	hkbCheckBalanceModifier *m_checkBalanceModifier;
};

class hkbBehaviorEventsInfo : public hkReferencedObject
{
public:
	virtual ~hkbBehaviorEventsInfo();
	uint64 m_characterId;
	hkArray<int16> m_externalEventIds;
	int32 m_padding;
};

class hkbBehaviorGraphInternalStateInfo : public hkReferencedObject
{
public:
	virtual ~hkbBehaviorGraphInternalStateInfo();
	uint64 m_characterId;
	hkbBehaviorGraphInternalState *m_internalState;
	hkArray<hkbAuxiliaryNodeInfo *> m_auxiliaryNodeInfo;
	hkArray<int16> m_activeEventIds;
	hkArray<int16> m_activeVariableIds;
};

class hkbBehaviorInfoIdToNamePair
{
public:
	hkStringPtr m_behaviorName;
	hkStringPtr m_nodeName;
	uint8 m_toolType;
	int16 m_id;
};

class hkbBehaviorInfo : public hkReferencedObject
{
public:
	virtual ~hkbBehaviorInfo();
	uint64 m_characterId;
	hkbBehaviorGraphData *m_data;
	hkArray<hkbBehaviorInfoIdToNamePair> m_idToNamePairs;
};

class hkbCameraShakeEventPayload : public hkbEventPayload
{
public:
	virtual ~hkbCameraShakeEventPayload();
	float m_amplitude;
	float m_halfLife;
};

class hkbCatchFallModifierHand
{
public:
	int16 m_animShoulderIndex;
	int16 m_ragdollShoulderIndex;
	int16 m_ragdollAnkleIndex;
};

class hkbCatchFallModifier : public hkbModifier
{
public:
	virtual ~hkbCatchFallModifier();
	hkVector4 m_directionOfFallForwardLS;
	hkVector4 m_directionOfFallRightLS;
	hkVector4 m_directionOfFallUpLS;
	hkbBoneIndexArray *m_spineIndices;
	hkbCatchFallModifierHand m_leftHand;
	hkbCatchFallModifierHand m_rightHand;
	hkbEventProperty m_catchFallDoneEvent;
	float m_spreadHandsMultiplier;
	float m_radarRange;
	float m_previousTargetBlendWeight;
	float m_handsBendDistance;
	float m_maxReachDistanceForward;
	float m_maxReachDistanceBackward;
	float m_fadeInReachGainSpeed;
	float m_fadeOutReachGainSpeed;
	float m_fadeOutDuration;
	float m_fadeInTwistSpeed;
	float m_fadeOutTwistSpeed;
	int16 m_raycastLayer;
	int16 m_velocityRagdollBoneIndex;
	int16 m_directionOfFallRagdollBoneIndex;
	bool m_orientHands;
	hkVector4 m_catchFallPosInBS;
	float m_currentReachGain;
	float m_timeSinceLastModify;
	float m_currentTwistGain;
	int16 m_currentTwistDirection;
	bool m_catchFallPosIsValid;
	bool m_catchFallBegin;
	bool m_catchFallEnd;
	int8 m_fadeState;
};

class hkbCharacterAddedInfo : public hkReferencedObject
{
public:
	virtual ~hkbCharacterAddedInfo();
	uint64 m_characterId;
	hkStringPtr m_instanceName;
	hkStringPtr m_templateName;
	hkStringPtr m_fullPathToProject;
	hkaSkeleton *m_skeleton;
	hkQsTransform m_worldFromModel;
	hkArray<hkQsTransform> m_poseModelSpace;
};

class hkbCharacterControlCommand : public hkReferencedObject
{
public:
	virtual ~hkbCharacterControlCommand();
	uint64 m_characterId;
	uint8 m_command;
	int32 m_padding;
};

class hkbCharacterInfo : public hkReferencedObject
{
public:
	virtual ~hkbCharacterInfo();
	uint64 m_characterId;
	uint8 m_event;
	int32 m_padding;
};

class hkbCharacterSkinInfo : public hkReferencedObject
{
public:
	virtual ~hkbCharacterSkinInfo();
	uint64 m_characterId;
	hkArray<uint64> m_deformableSkins;
	hkArray<uint64> m_rigidSkins;
};

class hkbCharacterSteppedInfo : public hkReferencedObject
{
public:
	virtual ~hkbCharacterSteppedInfo();
	uint64 m_characterId;
	float m_deltaTime;
	hkQsTransform m_worldFromModel;
	hkArray<hkQsTransform> m_poseModelSpace;
	hkArray<hkQsTransform> m_rigidAttachmentTransforms;
};

class hkbCheckRagdollSpeedModifier : public hkbModifier
{
public:
	virtual ~hkbCheckRagdollSpeedModifier();
	hkbEventProperty m_eventToSend;
	float m_minSpeedThreshold;
	float m_maxSpeedThreshold;
};

class hkbClientCharacterState : public hkReferencedObject
{
public:
	virtual ~hkbClientCharacterState();
	hkArray<uint64> m_deformableSkinIds;
	hkArray<uint64> m_rigidSkinIds;
	hkArray<int16> m_externalEventIds;
	hkArray<hkbAuxiliaryNodeInfo *> m_auxiliaryInfo;
	hkArray<int16> m_activeEventIds;
	hkArray<int16> m_activeVariableIds;
	uint64 m_characterId;
	hkStringPtr m_instanceName;
	hkStringPtr m_templateName;
	hkStringPtr m_fullPathToProject;
	hkbBehaviorGraphData *m_behaviorData;
	hkbBehaviorGraphInternalState *m_behaviorInternalState;
	void *m_nodeIdToInternalStateMap;
	bool m_visible;
	float m_elapsedSimulationTime;
	hkaSkeleton *m_skeleton;
	hkQsTransform m_worldFromModel;
	hkArray<hkQsTransform> m_poseModelSpace;
	hkArray<hkQsTransform> m_rigidAttachmentTransforms;
};

class hkbComputeWorldFromModelModifier : public hkbModifier
{
public:
	virtual ~hkbComputeWorldFromModelModifier();
	int16 m_poseMatchingRootBoneIndex;
	int16 m_poseMatchingOtherBoneIndex;
	int16 m_poseMatchingAnotherBoneIndex;
};

class hkbTarget : public hkReferencedObject
{
public:
	virtual ~hkbTarget();
	void *m_target;
	hkVector4 m_contactPointTS;
	hkVector4 m_contactNormalTS;
	hkVector4 m_shapeCentroidTS;
	float m_distance;
	int8 m_targetPriority;
};

class hkbConstrainRigidBodyModifier : public hkbModifier
{
public:
	virtual ~hkbConstrainRigidBodyModifier();
	hkbTarget *m_targetIn;
	float m_breakThreshold;
	int16 m_ragdollBoneToConstrain;
	bool m_breakable;
	int8 m_pivotPlacement;
	int8 m_boneToConstrainPlacement;
	int8 m_constraintType;
	bool m_clearTargetData;
	bool m_isConstraintHinge;
	void *m_constraint;
};

class hkbControlledReachModifier : public hkbModifier
{
public:
	virtual ~hkbControlledReachModifier();
	void *m_target;
	float m_time;
	float m_fadeInStart;
	float m_fadeInEnd;
	float m_fadeOutStart;
	float m_fadeOutEnd;
	float m_fadeOutDuration;
	float m_sensorAngle;
	int16 m_handIndex;
	bool m_isHandEnabled;
};

class hkbCustomTestGeneratorStruck
{
public:
	bool m_hkBool;
	hkStringPtr m_string;
	int32 m_int;
	int8 m_hkInt8;
	int16 m_hkInt16;
	int32 m_hkInt32;
	uint8 m_hkUint8;
	uint16 m_hkUint16;
	uint32 m_hkUint32;
	float m_hkReal;
	int8 m_mode_hkInt8;
	int16 m_mode_hkInt16;
	int32 m_mode_hkInt32;
	uint8 m_mode_hkUint8;
	uint16 m_mode_hkUint16;
	uint32 m_mode_hkUint32;
	hkFlags m_flags_hkInt8;
	hkFlags m_flags_hkInt16;
	hkFlags m_flags_hkInt32;
	hkFlags m_flags_hkUint8;
	hkFlags m_flags_hkUint16;
	hkFlags m_flags_hkUint32;
	hkbGenerator *m_generator1;
	hkbGenerator *m_generator2;
	hkbModifier *m_modifier1;
	hkbModifier *m_modifier2;
};

class hkbCustomTestGenerator : public hkbReferencePoseGenerator
{
public:
	virtual ~hkbCustomTestGenerator();
	bool m_hkBool;
	hkStringPtr m_string;
	int32 m_int;
	int8 m_hkInt8;
	int16 m_hkInt16;
	int32 m_hkInt32;
	uint8 m_hkUint8;
	uint16 m_hkUint16;
	uint32 m_hkUint32;
	float m_hkReal;
	hkVector4 m_hkVector4;
	hkQuaternion m_hkQuaternion;
	hkpRigidBody *m_hkRigidBody;
	int8 m_mode_hkInt8;
	int16 m_mode_hkInt16;
	int32 m_mode_hkInt32;
	uint8 m_mode_hkUint8;
	uint16 m_mode_hkUint16;
	uint32 m_mode_hkUint32;
	hkFlags m_flags_hkInt8;
	hkFlags m_flags_hkInt16;
	hkFlags m_flags_hkInt32;
	hkFlags m_flags_hkUint8;
	hkFlags m_flags_hkUint16;
	hkFlags m_flags_hkUint32;
	int32 m_myInt;
	hkbGenerator *m_generator1;
	hkbGenerator *m_generator2;
	hkbModifier *m_modifier1;
	hkbModifier *m_modifier2;
	hkArray<bool> m_array_hkBool;
	hkArray<int32> m_array_int;
	hkArray<int8> m_array_hkInt8;
	hkArray<int16> m_array_hkInt16;
	hkArray<int32> m_array_hkInt32;
	hkArray<int8> m_array_hkUint8;
	hkArray<int16> m_array_hkUint16;
	hkArray<int32> m_array_hkUint32;
	hkArray<float> m_array_hkReal;
	hkArray<hkbGenerator *> m_array_hkbGenerator;
	hkArray<hkbModifier *> m_array_hkbModifier;
	hkbCustomTestGeneratorStruck m_Struck;
	hkArray<hkbCustomTestGeneratorStruck> m_array_Struck;
};

class hkbDemoConfigCharacterInfo : public hkReferencedObject
{
public:
	virtual ~hkbDemoConfigCharacterInfo();
	hkStringPtr m_overrideCharacterDataFilename;
	hkStringPtr m_overrideBehaviorFilename;
	hkVector4 m_initialPosition;
	hkQuaternion m_initialRotation;
	int32 m_modelUpAxis;
	hkArray<int32> m_ragdollBoneLayers;
};

class hkbDemoConfigStickVariableInfo
{
public:
	hkStringPtr m_variableName;
	float m_minValue;
	float m_maxValue;
	float m_minStickValue;
	float m_maxStickValue;
	int8 m_stickAxis;
	int8 m_stick;
	bool m_complimentVariableValue;
	bool m_negateVariableValue;
};

class hkbDemoConfigTerrainInfo
{
public:
	hkStringPtr m_filename;
	int32 m_layer;
	int32 m_systemGroup;
	bool m_createDisplayObjects;
	hkpRigidBody *m_terrainRigidBody;
};

class hkbDemoConfig : public hkReferencedObject
{
public:
	virtual ~hkbDemoConfig();
	hkArray<hkbDemoConfigCharacterInfo *> m_characterInfo;
	hkArray<hkbDemoConfigTerrainInfo> m_terrainInfo;
	hkArray<int32> m_skinAttributeIndices;
	int32 m_buttonPressToEventMap;
	int32 m_buttonReleaseToEventMap;
	int32 m_worldUpAxis;
	int32 m_extraCharacterClones;
	int32 m_numTracks;
	float m_proxyHeight;
	float m_proxyRadius;
	float m_proxyOffset;
	hkStringPtr m_rootPath;
	hkStringPtr m_projectDataFilename;
	bool m_useAttachments;
	bool m_useProxy;
	bool m_useSkyBox;
	bool m_useTrackingCamera;
	bool m_accumulateMotion;
	bool m_testCloning;
	bool m_useSplineCompression;
	bool m_forceLoad;
	hkArray<hkbDemoConfigStickVariableInfo> m_stickVariables;
	int32 m_gamePadToRotateTerrainAboutItsAxisMap;
	int32 m_gamePadToAddRemoveCharacterMap;
	hkpGroupFilter *m_filter;
};

class hkbEventRaisedInfo : public hkReferencedObject
{
public:
	virtual ~hkbEventRaisedInfo();
	uint64 m_characterId;
	hkStringPtr m_eventName;
	bool m_raisedBySdk;
	int32 m_senderId;
	int32 m_padding;
};

class hkbFaceTargetModifier : public hkbModifier
{
public:
	virtual ~hkbFaceTargetModifier();
	hkbTarget *m_targetIn;
	float m_offsetAngle;
	bool m_onlyOnce;
	bool m_done;
};

class hkbGravityModifier : public hkbModifier
{
public:
	virtual ~hkbGravityModifier();
	hkVector4 m_initialVelocityInMS;
	float m_gravityConstant;
	hkVector4 m_currentVelocityInMS;
	float m_secondsElapsed;
};

class hkbHoldFromBlendingTransitionEffect : public hkbBlendingTransitionEffect
{
public:
	virtual ~hkbHoldFromBlendingTransitionEffect();
	void *m_heldFromPose;
	int32 m_heldFromPoseSize;
	hkQsTransform m_heldWorldFromModel;
	void *m_heldFromSkeleton;
};

class hkbLinkedSymbolInfo : public hkReferencedObject
{
public:
	virtual ~hkbLinkedSymbolInfo();
	hkArray<hkStringPtr> m_eventNames;
	hkArray<hkStringPtr> m_variableNames;
};

class hkbMoveBoneTowardTargetModifier : public hkbModifier
{
public:
	virtual ~hkbMoveBoneTowardTargetModifier();
	hkbTarget *m_targetIn;
	hkVector4 m_offsetInBoneSpace;
	hkVector4 m_alignAxisBS;
	hkVector4 m_targetAlignAxisTS;
	hkVector4 m_alignWithCharacterForwardBS;
	hkVector4 m_currentBonePositionOut;
	hkQuaternion m_currentBoneRotationOut;
	hkbEventProperty m_eventToSendWhenTargetReached;
	hkbGenerator *m_childGenerator;
	float m_duration;
	int16 m_ragdollBoneIndex;
	int16 m_animationBoneIndex;
	int8 m_targetMode;
	int8 m_alignMode;
	bool m_useVelocityPrediction;
	bool m_affectOrientation;
	bool m_currentBoneIsValidOut;
	void *m_targetInternal;
	hkVector4 m_targetPointTS;
	float m_time;
	float m_timeSinceLastModify;
	hkVector4 m_lastAnimBonePositionMS;
	hkVector4 m_finalAnimBonePositionMS;
	hkVector4 m_initialAnimBonePositionMS;
	hkQuaternion m_finalAnimBoneOrientationMS;
	hkQuaternion m_animationFromRagdoll;
	hkQsTransform m_totalMotion;
	hkQsTransform m_accumulatedMotion;
	bool m_useAnimationData;
};

class hkbParticleSystemEventPayload : public hkbEventPayload
{
public:
	virtual ~hkbParticleSystemEventPayload();
	uint8 m_type;
	int16 m_emitBoneIndex;
	hkVector4 m_offset;
	hkVector4 m_direction;
	int32 m_numParticles;
	float m_speed;
};

class hkbGeneratorOutputListener : public hkReferencedObject
{
public:
};

class hkbPoseStoringGeneratorOutputListenerStoredPose : public hkReferencedObject
{
public:
	virtual ~hkbPoseStoringGeneratorOutputListenerStoredPose();
	hkbNode *m_node;
	hkArray<hkQsTransform> m_pose;
	hkQsTransform m_worldFromModel;
	bool m_isPoseValid;
};

class hkbPoseStoringGeneratorOutputListener : public hkbGeneratorOutputListener
{
public:
	virtual ~hkbPoseStoringGeneratorOutputListener();
	hkArray<hkbPoseStoringGeneratorOutputListenerStoredPose *> m_storedPoses;
	bool m_dirty;
	void *m_nodeToIndexMap;
};

class hkbRaiseEventCommand : public hkReferencedObject
{
public:
	virtual ~hkbRaiseEventCommand();
	uint64 m_characterId;
	bool m_global;
	int32 m_externalId;
};

class hkbReachModifierHand
{
public:
	hkVector4 m_targetOrSensingPosition;
	hkVector4 m_targetBackHandNormal;
	float m_sensingRadius;
	int16 m_boneIndex;
	int16 m_handIkTrackIndex;
};

class hkbReachModifier : public hkbModifier
{
public:
	virtual ~hkbReachModifier();
	hkArray<hkbReachModifierHand> m_hands;
	float m_newTargetGain;
	float m_noTargetGain;
	float m_targetGain;
	float m_fadeOutDuration;
	int32 m_raycastLayer;
	uint32 m_sensingPropertyKey;
	int8 m_reachMode;
	bool m_ignoreMySystemGroup;
	float m_extrapolate;
	hkArray<void> m_internalHandData;
	float m_timeLapse;
};

class hkbReachTowardTargetModifierHand
{
public:
	int16 m_shoulderIndex;
	bool m_isEnabled;
};

class hkbReachTowardTargetModifier : public hkbModifier
{
public:
	virtual ~hkbReachTowardTargetModifier();
	hkbReachTowardTargetModifierHand m_leftHand;
	hkbReachTowardTargetModifierHand m_rightHand;
	hkbTarget *m_targetIn;
	float m_distanceBetweenHands;
	float m_reachDistance;
	float m_fadeInGainSpeed;
	float m_fadeOutGainSpeed;
	float m_fadeOutDuration;
	float m_targetChangeSpeed;
	bool m_holdTarget;
	bool m_reachPastTarget;
	bool m_giveUpIfNoTarget;
	void *m_targetRB;
	hkVector4 m_prevTargetInMS;
	float m_currentGain;
	float m_timeSinceLastModify;
	int8 m_fadeState;
	bool m_haveGivenUp;
	bool m_isTherePrevTarget;
};

class hkbSetBehaviorCommand : public hkReferencedObject
{
public:
	virtual ~hkbSetBehaviorCommand();
	uint64 m_characterId;
	hkbBehaviorGraph *m_behavior;
	hkbGenerator *m_rootGenerator;
	hkArray<hkbBehaviorGraph *> m_referencedBehaviors;
	int32 m_startStateIndex;
	bool m_randomizeSimulation;
	int32 m_padding;
};

class hkbSetLocalTimeOfClipGeneratorCommand : public hkReferencedObject
{
public:
	virtual ~hkbSetLocalTimeOfClipGeneratorCommand();
	uint64 m_characterId;
	float m_localTime;
	int16 m_nodeId;
};

class hkbSetNodePropertyCommand : public hkReferencedObject
{
public:
	virtual ~hkbSetNodePropertyCommand();
	uint64 m_characterId;
	hkStringPtr m_nodeName;
	hkStringPtr m_propertyName;
	hkbVariableValue m_propertyValue;
	int32 m_padding;
};

class hkbSetWordVariableCommand : public hkReferencedObject
{
public:
	virtual ~hkbSetWordVariableCommand();
	hkVector4 m_quadValue;
	uint64 m_characterId;
	int32 m_variableId;
	hkbVariableValue m_value;
	uint8 m_type;
	bool m_global;
};

class hkbSimulationControlCommand : public hkReferencedObject
{
public:
	virtual ~hkbSimulationControlCommand();
	uint8 m_command;
};

class hkbSimulationStateInfo : public hkReferencedObject
{
public:
	virtual ~hkbSimulationStateInfo();
	uint8 m_simulationState;
};

class hkbStateDependentModifier : public hkbModifier
{
public:
	virtual ~hkbStateDependentModifier();
	bool m_applyModifierDuringTransition;
	hkArray<int32> m_stateIds;
	hkbModifier *m_modifier;
	bool m_isActive;
	hkbStateMachine *m_stateMachine;
};

class hkbTargetRigidBodyModifier : public hkbModifier
{
public:
	virtual ~hkbTargetRigidBodyModifier();
	hkbTarget *m_targetOut;
	int8 m_targetMode;
	int32 m_sensingLayer;
	bool m_targetOnlyOnce;
	bool m_ignoreMySystemGroup;
	float m_maxTargetDistance;
	float m_maxTargetHeightAboveSensor;
	float m_closeToTargetDistanceThreshold;
	int8 m_targetAngleMode;
	int8 m_targetDistanceMode;
	float m_maxAngleToTarget;
	int16 m_sensorRagdollBoneIndex;
	int16 m_sensorAnimationBoneIndex;
	int16 m_closeToTargetRagdollBoneIndex;
	int16 m_closeToTargetAnimationBoneIndex;
	hkVector4 m_sensorOffsetInBoneSpace;
	hkVector4 m_closeToTargetOffsetInBoneSpace;
	hkVector4 m_sensorDirectionBS;
	int8 m_eventMode;
	uint32 m_sensingPropertyKey;
	bool m_sensorInWS;
	hkbEventProperty m_eventToSend;
	hkbEventProperty m_eventToSendToTarget;
	hkbEventProperty m_closeToTargetEvent;
	bool m_useVelocityPrediction;
	bool m_targetOnlySpheres;
	bool m_isCloseToTargetOut;
	int8 m_targetPriority;
	void *m_targetRb;
	float m_targetDistance;
	float m_timeSinceLastModify;
	bool m_eventHasBeenSent;
	bool m_closeToTargetEventHasBeenSent;
	bool m_isActive;
};

class hkbTestStateChooser : public hkbStateChooser
{
public:
	virtual ~hkbTestStateChooser();
	int32 m_int;
	float m_real;
	hkStringPtr m_string;
};

class BGSGamebryoSequenceGenerator : public hkbGenerator
{
public:
	virtual ~BGSGamebryoSequenceGenerator();
	char *m_pSequence;
	int8 m_eBlendModeFunction;
	float m_fPercent;
	hkArray<void> m_events;
	float m_fTime;
	bool m_bDelayedActivate;
	bool m_bLooping;
};

class BSBoneSwitchGeneratorBoneData : public hkbBindable
{
public:
	virtual ~BSBoneSwitchGeneratorBoneData();
	hkbGenerator *m_pGenerator;
	hkbBoneWeightArray *m_spBoneWeight;
};

class BSBoneSwitchGenerator : public hkbGenerator
{
public:
	virtual ~BSBoneSwitchGenerator();
	hkbGenerator *m_pDefaultGenerator;
	hkArray<BSBoneSwitchGeneratorBoneData *> m_ChildrenA;
};

class BSComputeAddBoneAnimModifier : public hkbModifier
{
public:
	virtual ~BSComputeAddBoneAnimModifier();
	int16 m_boneIndex;
	hkVector4 m_translationLSOut;
	hkQuaternion m_rotationLSOut;
	hkVector4 m_scaleLSOut;
	void *m_pSkeletonMemory;
};

class BSCyclicBlendTransitionGenerator : public hkbGenerator
{
public:
	virtual ~BSCyclicBlendTransitionGenerator();
	hkbGenerator *m_pBlenderGenerator;
	hkbEventProperty m_EventToFreezeBlendValue;
	hkbEventProperty m_EventToCrossBlend;
	float m_fBlendParameter;
	float m_fTransitionDuration;
	int8 m_eBlendCurve;
	void *m_pTransitionBlenderGenerator;
	void *m_pTransitionEffect;
	int8 m_currentMode;
};

class BSDecomposeVectorModifier : public hkbModifier
{
public:
	virtual ~BSDecomposeVectorModifier();
	hkVector4 m_vector;
	float m_x;
	float m_y;
	float m_z;
	float m_w;
};

class BSDirectAtModifier : public hkbModifier
{
public:
	virtual ~BSDirectAtModifier();
	bool m_directAtTarget;
	int16 m_sourceBoneIndex;
	int16 m_startBoneIndex;
	int16 m_endBoneIndex;
	float m_limitHeadingDegrees;
	float m_limitPitchDegrees;
	float m_offsetHeadingDegrees;
	float m_offsetPitchDegrees;
	float m_onGain;
	float m_offGain;
	hkVector4 m_targetLocation;
	uint32 m_userInfo;
	bool m_directAtCamera;
	float m_directAtCameraX;
	float m_directAtCameraY;
	float m_directAtCameraZ;
	bool m_active;
	float m_currentHeadingOffset;
	float m_currentPitchOffset;
	float m_timeStep;
	void *m_pSkeletonMemory;
	bool m_hasTarget;
	hkVector4 m_directAtTargetLocation;
	hkArray<void> m_boneChainIndices;
};

class BSDistTriggerModifier : public hkbModifier
{
public:
	virtual ~BSDistTriggerModifier();
	hkVector4 m_targetPosition;
	float m_distance;
	float m_distanceTrigger;
	hkbEventProperty m_triggerEvent;
};

class BSEventEveryNEventsModifier : public hkbModifier
{
public:
	virtual ~BSEventEveryNEventsModifier();
	hkbEventProperty m_eventToCheckFor;
	hkbEventProperty m_eventToSend;
	int8 m_numberOfEventsBeforeSend;
	int8 m_minimumNumberOfEventsBeforeSend;
	bool m_randomizeNumberOfEvents;
	int32 m_numberOfEventsSeen;
	int8 m_calculatedNumberOfEventsBeforeSend;
};

class BSEventOnDeactivateModifier : public hkbModifier
{
public:
	virtual ~BSEventOnDeactivateModifier();
	hkbEventProperty m_event;
};

class BSEventOnFalseToTrueModifier : public hkbModifier
{
public:
	virtual ~BSEventOnFalseToTrueModifier();
	bool m_bEnableEvent1;
	bool m_bVariableToTest1;
	hkbEventProperty m_EventToSend1;
	bool m_bEnableEvent2;
	bool m_bVariableToTest2;
	hkbEventProperty m_EventToSend2;
	bool m_bEnableEvent3;
	bool m_bVariableToTest3;
	hkbEventProperty m_EventToSend3;
	bool m_bSlot1ActivatedLastFrame;
	bool m_bSlot2ActivatedLastFrame;
	bool m_bSlot3ActivatedLastFrame;
};

class BSGetTimeStepModifier : public hkbModifier
{
public:
	virtual ~BSGetTimeStepModifier();
	float m_timeStep;
};

class BSIStateManagerModifierBSiStateData
{
public:
	hkbGenerator *m_pStateMachine;
	int32 m_StateID;
	int32 m_iStateToSetAs;
};

class BSIStateManagerModifierBSIStateManagerStateListener : public hkbStateListener
{
public:
	virtual ~BSIStateManagerModifierBSIStateManagerStateListener();
	void *m_pStateManager;
};

class BSIStateManagerModifier : public hkbModifier
{
public:
	virtual ~BSIStateManagerModifier();
	int32 m_iStateVar;
	hkArray<BSIStateManagerModifierBSiStateData> m_stateData;
	BSIStateManagerModifierBSIStateManagerStateListener m_myStateListener;
};

class BSInterpValueModifier : public hkbModifier
{
public:
	virtual ~BSInterpValueModifier();
	float m_source;
	float m_target;
	float m_result;
	float m_gain;
	float m_timeStep;
};

class BSIsActiveModifier : public hkbModifier
{
public:
	virtual ~BSIsActiveModifier();
	bool m_bIsActive0;
	bool m_bInvertActive0;
	bool m_bIsActive1;
	bool m_bInvertActive1;
	bool m_bIsActive2;
	bool m_bInvertActive2;
	bool m_bIsActive3;
	bool m_bInvertActive3;
	bool m_bIsActive4;
	bool m_bInvertActive4;
};

class BSLimbIKModifier : public hkbModifier
{
public:
	virtual ~BSLimbIKModifier();
	float m_limitAngleDegrees;
	float m_currentAngle;
	int16 m_startBoneIndex;
	int16 m_endBoneIndex;
	float m_gain;
	float m_boneRadius;
	float m_castOffset;
	float m_timeStep;
	void *m_pSkeletonMemory;
};

class BSLookAtModifierBoneData
{
public:
	int16 m_index;
	hkVector4 m_fwdAxisLS;
	float m_limitAngleDegrees;
	float m_onGain;
	float m_offGain;
	bool m_enabled;
	hkVector4 m_currentFwdAxisLS;
};

class BSLookAtModifier : public hkbModifier
{
public:
	virtual ~BSLookAtModifier();
	bool m_lookAtTarget;
	hkArray<BSLookAtModifierBoneData> m_bones;
	hkArray<BSLookAtModifierBoneData> m_eyeBones;
	float m_limitAngleDegrees;
	float m_limitAngleThresholdDegrees;
	bool m_continueLookOutsideOfLimit;
	float m_onGain;
	float m_offGain;
	bool m_useBoneGains;
	hkVector4 m_targetLocation;
	bool m_targetOutsideLimits;
	hkbEventProperty m_targetOutOfLimitEvent;
	bool m_lookAtCamera;
	float m_lookAtCameraX;
	float m_lookAtCameraY;
	float m_lookAtCameraZ;
	float m_timeStep;
	bool m_ballBonesValid;
	void *m_pSkeletonMemory;
};

class BSModifyOnceModifier : public hkbModifier
{
public:
	virtual ~BSModifyOnceModifier();
	hkbModifier *m_pOnActivateModifier;
	hkbModifier *m_pOnDeactivateModifier;
};

class BSOffsetAnimationGenerator : public hkbGenerator
{
public:
	virtual ~BSOffsetAnimationGenerator();
	hkbGenerator *m_pDefaultGenerator;
	hkbGenerator *m_pOffsetClipGenerator;
	float m_fOffsetVariable;
	float m_fOffsetRangeStart;
	float m_fOffsetRangeEnd;
	hkArray<void> m_BoneOffsetA;
	hkArray<void> m_BoneIndexA;
	float m_fCurrentPercentage;
	uint32 m_iCurrentFrame;
	bool m_bZeroOffset;
	bool m_bOffsetValid;
};

class BSPassByTargetTriggerModifier : public hkbModifier
{
public:
	virtual ~BSPassByTargetTriggerModifier();
	hkVector4 m_targetPosition;
	float m_radius;
	hkVector4 m_movementDirection;
	hkbEventProperty m_triggerEvent;
	bool m_targetPassed;
};

class BSRagdollContactListenerModifier : public hkbModifier
{
public:
	virtual ~BSRagdollContactListenerModifier();
	hkbEventProperty m_contactEvent;
	hkbBoneIndexArray *m_bones;
	bool m_throwEvent;
	hkArray<void *> m_ragdollRigidBodies;
};

class BSSpeedSamplerModifier : public hkbModifier
{
public:
	virtual ~BSSpeedSamplerModifier();
	int32 m_state;
	float m_direction;
	float m_goalSpeed;
	float m_speedOut;
};

class BSSynchronizedClipGenerator : public hkbGenerator
{
public:
	virtual ~BSSynchronizedClipGenerator();
	hkbGenerator *m_pClipGenerator;
	char *m_SyncAnimPrefix;
	bool m_bSyncClipIgnoreMarkPlacement;
	float m_fGetToMarkTime;
	float m_fMarkErrorThreshold;
	bool m_bLeadCharacter;
	bool m_bReorientSupportChar;
	bool m_bApplyMotionFromRoot;
	void *m_pSyncScene;
	hkQsTransform m_StartMarkWS;
	hkQsTransform m_EndMarkWS;
	hkQsTransform m_StartMarkMS;
	float m_fCurrentLerp;
	void *m_pLocalSyncBinding;
	void *m_pEventMap;
	int16 m_sAnimationBindingIndex;
	bool m_bAtMark;
	bool m_bAllCharactersInScene;
	bool m_bAllCharactersAtMarks;
};

class BSTimerModifier : public hkbModifier
{
public:
	virtual ~BSTimerModifier();
	float m_alarmTimeSeconds;
	hkbEventProperty m_alarmEvent;
	bool m_resetAlarm;
	float m_secondsElapsed;
};

class BSTweenerModifier : public hkbModifier
{
public:
	virtual ~BSTweenerModifier();
	bool m_tweenPosition;
	bool m_tweenRotation;
	bool m_useTweenDuration;
	float m_tweenDuration;
	hkVector4 m_targetPosition;
	hkQuaternion m_targetRotation;
	float m_duration;
	hkQsTransform m_startTransform;
	float m_time;
};

class BSiStateTaggingGenerator : public hkbGenerator
{
public:
	virtual ~BSiStateTaggingGenerator();
	hkbGenerator *m_pDefaultGenerator;
	int32 m_iStateToSetAs;
	int32 m_iPriority;
};

class hkAabbHalf
{
public:
	uint16 m_data;
	uint16 m_extras;
};

class hkAabbUint32
{
public:
	uint32 m_min;
	uint8 m_expansionMin;
	uint8 m_expansionShift;
	uint32 m_max;
	uint8 m_expansionMax;
	uint8 m_shapeKeyByte;
};

class hkArrayTypeAttribute
{
public:
	int8 m_type;
};

class hkBitField
{
public:
	hkArray<uint32> m_words;
	int32 m_numBits;
};

class hkCustomAttributesAttribute
{
public:
	char *m_name;
	hkVariant m_value;
};

class hkCustomAttributes
{
public:
	hkCustomAttributesAttribute *m_attributes;
};

class hkClassEnumItem
{
public:
	int32 m_value;
	char *m_name;
};

class hkClassEnum
{
public:
	char *m_name;
	hkClassEnumItem *m_items;
	hkCustomAttributes *m_attributes;
	hkFlags m_flags;
};

class hkClassMember
{
public:
	char *m_name;
	hkClass *m_class;
	hkClassEnum *m_enum;
	uint8 m_type;
	uint8 m_subtype;
	int16 m_cArraySize;
	hkFlags m_flags;
	uint16 m_offset;
	hkCustomAttributes *m_attributes;
};

class hkClass
{
public:
	char *m_name;
	hkClass *m_parent;
	int32 m_objectSize;
	int32 m_numImplementedInterfaces;
	hkClassEnum *m_declaredEnums;
	hkClassMember *m_declaredMembers;
	void *m_defaults;
	hkCustomAttributes *m_attributes;
	hkFlags m_flags;
	int32 m_describedVersion;
};

class hkColor
{
public:
};

class hkContactPointMaterial
{
public:
	uint64 m_userData;
	uint8 m_friction;
	uint8 m_restitution;
	uint8 m_maxImpulse;
	uint8 m_flags;
};

class hkDataObjectTypeAttribute
{
public:
	char *m_typeName;
};

class hkDescriptionAttribute
{
public:
	char *m_string;
};

class hkDocumentationAttribute
{
public:
	char *m_docsSectionTag;
};

class hkGeometryTriangle
{
public:
	int32 m_a;
	int32 m_b;
	int32 m_c;
	int32 m_material;
};

class hkGeometry
{
public:
	hkArray<hkVector4> m_vertices;
	hkArray<hkGeometryTriangle> m_triangles;
};

class hkGizmoAttribute
{
public:
	bool m_visible;
	char *m_label;
	int8 m_type;
};

class hkHalf8
{
public:
	hkHalf m_quad;
};

class hkLinkAttribute
{
public:
	int8 m_type;
};

class hkMemoryTrackerAttribute
{
public:
};

class hkModelerNodeTypeAttribute
{
public:
	int8 m_type;
};

class hkPackedVector3
{
public:
	int16 m_values;
};

class hkPostFinishAttribute
{
public:
	void *m_postFinishFunction;
};

class hkQTransform
{
public:
	hkQuaternion m_rotation;
	hkVector4 m_translation;
};

class hkRangeInt32Attribute
{
public:
	int32 m_absmin;
	int32 m_absmax;
	int32 m_softmin;
	int32 m_softmax;
};

class hkRangeRealAttribute
{
public:
	float m_absmin;
	float m_absmax;
	float m_softmin;
	float m_softmax;
};

class hkReflectedFileAttribute
{
public:
	char *m_value;
};

class hkSemanticsAttribute
{
public:
	int8 m_type;
};

class hkSphere
{
public:
	hkVector4 m_pos;
};

class hkTraceStreamTitle
{
public:
	char m_value;
};

class hkUiAttribute
{
public:
	bool m_visible;
	int8 m_hideInModeler;
	char *m_label;
	char *m_group;
	char *m_hideBaseClassMembers;
	bool m_endGroup;
	bool m_endGroup2;
	bool m_advanced;
};

class hkMonitorStreamFrameInfo
{
public:
	hkStringPtr m_heading;
	int32 m_indexOfTimer0;
	int32 m_indexOfTimer1;
	uint32 m_absoluteTimeCounter;
	float m_timerFactor0;
	float m_timerFactor1;
	int32 m_threadId;
	int32 m_frameStreamStart;
	int32 m_frameStreamEnd;
};

class hkMonitorStreamStringMapStringMap
{
public:
	uint64 m_id;
	hkStringPtr m_string;
};

class hkMonitorStreamStringMap
{
public:
	hkArray<hkMonitorStreamStringMapStringMap> m_map;
};

class hkPackfileHeader
{
public:
	int32 m_magic;
	int32 m_userTag;
	int32 m_fileVersion;
	uint8 m_layoutRules;
	int32 m_numSections;
	int32 m_contentsSectionIndex;
	int32 m_contentsSectionOffset;
	int32 m_contentsClassNameSectionIndex;
	int32 m_contentsClassNameSectionOffset;
	char m_contentsVersion;
	int32 m_flags;
	int32 m_pad;
};

class hkPackfileSectionHeader
{
public:
	char m_sectionTag;
	char m_nullByte;
	int32 m_absoluteDataStart;
	int32 m_localFixupsOffset;
	int32 m_globalFixupsOffset;
	int32 m_virtualFixupsOffset;
	int32 m_exportsOffset;
	int32 m_importsOffset;
	int32 m_endOffset;
};

class hkRootLevelContainerNamedVariant
{
public:
	hkStringPtr m_name;
	hkStringPtr m_className;
	hkReferencedObject *m_variant;
};

class hkRootLevelContainer
{
public:
	hkArray<hkRootLevelContainerNamedVariant> m_namedVariants;
};

class hkMeshSection
{
public:
	uint8 m_primitiveType;
	int32 m_numPrimitives;
	int32 m_numIndices;
	int32 m_vertexStartIndex;
	int32 m_transformIndex;
	uint8 m_indexType;
	void *m_indices;
	hkMeshVertexBuffer *m_vertexBuffer;
	hkMeshMaterial *m_material;
	int32 m_sectionIndex;
};

class hkpMassProperties
{
public:
	float m_volume;
	float m_mass;
	hkVector4 m_centerOfMass;
	hkMatrix3 m_inertiaTensor;
};

class hkpWeldingUtility
{
public:
};

class hkpCenterOfMassChangerModifierConstraintAtom : public hkpModifierConstraintAtom
{
public:
	hkVector4 m_displacementA;
	hkVector4 m_displacementB;
};

class hkpIgnoreModifierConstraintAtom : public hkpModifierConstraintAtom
{
public:
};

class hkpMassChangerModifierConstraintAtom : public hkpModifierConstraintAtom
{
public:
	hkVector4 m_factorA;
	hkVector4 m_factorB;
};

class hkpMovingSurfaceModifierConstraintAtom : public hkpModifierConstraintAtom
{
public:
	hkVector4 m_velocity;
};

class hkpOverwritePivotConstraintAtom : public hkpConstraintAtom
{
public:
	uint8 m_copyToPivotBFromPivotA;
};

class hkpSoftContactModifierConstraintAtom : public hkpModifierConstraintAtom
{
public:
	float m_tau;
	float m_maxAcceleration;
};

class hkpViscousSurfaceModifierConstraintAtom : public hkpModifierConstraintAtom
{
public:
};

class hkpMoppCodeReindexedTerminal
{
public:
	uint32 m_origShapeKey;
	uint32 m_reindexedShapeKey;
};

class hkaQuantizedAnimationTrackCompressionParams
{
public:
	float m_rotationTolerance;
	float m_translationTolerance;
	float m_scaleTolerance;
	float m_floatingTolerance;
};

class hkaSplineCompressedAnimationAnimationCompressionParams
{
public:
	uint16 m_maxFramesPerBlock;
	bool m_enableSampleSingleTracks;
};

class hkaSplineCompressedAnimationTrackCompressionParams
{
public:
	float m_rotationTolerance;
	float m_translationTolerance;
	float m_scaleTolerance;
	float m_floatingTolerance;
	uint16 m_rotationDegree;
	uint16 m_translationDegree;
	uint16 m_scaleDegree;
	uint16 m_floatingDegree;
	uint8 m_rotationQuantizationType;
	uint8 m_translationQuantizationType;
	uint8 m_scaleQuantizationType;
	uint8 m_floatQuantizationType;
};

class hkaWaveletCompressedAnimationCompressionParams
{
public:
	uint16 m_quantizationBits;
	uint16 m_blockSize;
	uint16 m_preserve;
	float m_truncProp;
	bool m_useOldStyleTruncation;
	float m_absolutePositionTolerance;
	float m_relativePositionTolerance;
	float m_rotationTolerance;
	float m_scaleTolerance;
	float m_absoluteFloatTolerance;
};

class hkaKeyFrameHierarchyUtility
{
public:
};

class hkbBlendCurveUtils
{
public:
};

class hkbContext
{
public:
	void *m_character;
	void *m_behavior;
	void *m_nodeToIndexMap;
	void *m_eventQueue;
	void *m_sharedEventQueue;
	hkbGeneratorOutputListener *m_generatorOutputListener;
	bool m_eventTriggeredTransition;
	void *m_world;
	void *m_attachmentManager;
	void *m_animationCache;
};

class hkbDefaultMessageLog
{
public:
};

class hkbMessageLog
{
public:
	void *m_messages;
	int32 m_maxMessages;
};

class hkbStateMachineNestedStateMachineData
{
public:
	void *m_nestedStateMachine;
	void *m_eventIdMap;
};

class hkbWorldEnums
{
public:
};

class hkVariableTweakingHelperBoolVariableInfo
{
public:
	hkStringPtr m_name;
	bool m_value;
	bool m_tweakOn;
};

class hkVariableTweakingHelperVector4VariableInfo
{
public:
	hkStringPtr m_name;
	float m_x;
	float m_y;
	float m_z;
	float m_w;
	bool m_tweakOn;
};

class hkVariableTweakingHelperRealVariableInfo
{
public:
	hkStringPtr m_name;
	float m_value;
	bool m_tweakOn;
};

class hkVariableTweakingHelperIntVariableInfo
{
public:
	hkStringPtr m_name;
	int32 m_value;
	bool m_tweakOn;
};

class hkVariableTweakingHelper
{
public:
	hkArray<hkVariableTweakingHelperBoolVariableInfo> m_boolVariableInfo;
	hkArray<hkVariableTweakingHelperIntVariableInfo> m_intVariableInfo;
	hkArray<hkVariableTweakingHelperRealVariableInfo> m_realVariableInfo;
	hkArray<hkVariableTweakingHelperVector4VariableInfo> m_vector4VariableInfo;
	void *m_behavior;
	hkArray<void> m_boolIndices;
	hkArray<void> m_intIndices;
	hkArray<void> m_realIndices;
	hkArray<void> m_vector4Indices;
};

class hkpCollidableCollidableFilter
{
public:
};

class hkpPhantomCallbackShape : public hkpShape
{
public:
};

class hkpRayCollidableFilter
{
public:
};

class hkpRayShapeCollectionFilter
{
public:
};

class hkpShapeCollectionFilter
{
public:
};

class hkpShapeModifier : public hkReferencedObject
{
public:
};

class hkpArrayAction : public hkpAction
{
public:
	hkArray<hkpEntity *> m_entities;
};

class hkpBreakableBody : public hkReferencedObject
{
public:
};

using CreatorFunction = std::function<void*(void*)>;

struct typeinfo
{
	uint32 vtable;
	CreatorFunction creator;
};

std::map<std::string, typeinfo> typeinfos = {
	{
		"hkBaseObject",
		{
			4294967295,
			[](void *mem) -> void* { return new (mem) hkBaseObject(); }
		}
	},
	{
		"hkReferencedObject", 
		{
			4294967295,
			[](void *mem) -> void* { return new (mem) hkReferencedObject(); }
		}
	},
	{
		"hkLocalFrameGroup", 
		{
			4294967295,
			[](void *mem) -> void* { return new (mem) hkLocalFrameGroup(); }
		}
	},
	{"hkLocalFrame", {0}},
	{"hkSimpleLocalFrame", {4294967295}},
	{"hkTrackerSerializableScanSnapshotAllocation", {0}},
	{"hkTrackerSerializableScanSnapshotBlock", {0}},
	{"hkTrackerSerializableScanSnapshot", {4294967295}},
	{"hkMonitorStreamColorTableColorPair", {0}},
	{"hkMonitorStreamColorTable", {4294967295}},
	{"hkResourceBase", {0}},
	{"hkResourceContainer", {0}},
	{"hkMemoryResourceHandleExternalLink", {0}},
	{"hkResourceHandle", {0}},
	{"hkMemoryResourceHandle", {4294967295}},
	{"hkMemoryResourceContainer", {4294967295}},
	{"hkAlignSceneToNodeOptions", {4294967295}},
	{"hkxAnimatedFloat", {4294967295}},
	{"hkxAnimatedMatrix", {4294967295}},
	{"hkxAnimatedQuaternion", {4294967295}},
	{"hkxAnimatedVector", {4294967295}},
	{"hkxAttribute", {0}},
	{"hkxAttributeGroup", {0}},
	{"hkxAttributeHolder", {4294967295}},
	{"hkxCamera", {4294967295}},
	{"hkxEdgeSelectionChannel", {4294967295}},
	{"hkxEnumItem", {0}},
	{"hkxEnum", {4294967295}},
	{"hkxEnvironmentVariable", {0}},
	{"hkxEnvironment", {4294967295}},
	{"hkxIndexBuffer", {4294967295}},
	{"hkxLight", {4294967295}},
	{"hkxMaterialTextureStage", {0}},
	{"hkxMaterialProperty", {0}},
	{"hkxMaterial", {4294967295}},
	{"hkxMaterialEffect", {4294967295}},
	{"hkxMaterialShader", {4294967295}},
	{"hkxMaterialShaderSet", {4294967295}},
	{"hkxMeshUserChannelInfo", {4294967295}},
	{"hkxVertexDescriptionElementDecl", {0}},
	{"hkxVertexDescription", {0}},
	{"hkxVertexBufferVertexData", {0}},
	{"hkxVertexBuffer", {4294967295}},
	{"hkxMeshSection", {4294967295}},
	{"hkxMesh", {4294967295}},
	{"hkxNodeAnnotationData", {0}},
	{"hkxNode", {4294967295}},
	{"hkxNodeSelectionSet", {4294967295}},
	{"hkxSkinBinding", {4294967295}},
	{"hkxTextureFile", {4294967295}},
	{"hkxTextureInplace", {4294967295}},
	{"hkxScene", {4294967295}},
	{"hkxSparselyAnimatedBool", {4294967295}},
	{"hkxSparselyAnimatedInt", {4294967295}},
	{"hkxSparselyAnimatedEnum", {4294967295}},
	{"hkxSparselyAnimatedString", {4294967295}},
	{"hkxTriangleSelectionChannel", {4294967295}},
	{"hkxVertexFloatDataChannel", {4294967295}},
	{"hkxVertexIntDataChannel", {4294967295}},
	{"hkxVertexSelectionChannel", {4294967295}},
	{"hkxVertexVectorDataChannel", {4294967295}},
	{"hkMeshBoneIndexMapping", {0}},
	{"hkIndexedTransformSet", {4294967295}},
	{"hkMeshVertexBuffer", {0}},
	{"hkMeshShape", {0}},
	{"hkMeshBody", {0}},
	{"hkMemoryMeshBody", {4294967295}},
	{"hkMeshTexture", {0}},
	{"hkMeshMaterial", {0}},
	{"hkMemoryMeshMaterial", {4294967295}},
	{"hkMeshSectionCinfo", {0}},
	{"hkMemoryMeshShape", {4294967295}},
	{"hkMemoryMeshTexture", {4294967295}},
	{"hkVertexFormatElement", {0}},
	{"hkVertexFormat", {0}},
	{"hkMemoryMeshVertexBuffer", {4294967295}},
	{"hkMultipleVertexBufferElementInfo", {0}},
	{"hkMultipleVertexBufferVertexBufferInfo", {0}},
	{"hkMultipleVertexBufferLockedElement", {0}},
	{"hkMultipleVertexBuffer", {4294967295}},
	{"hkpShape", {0}},
	{"hkpSphereRepShape", {0}},
	{"hkpConvexShape", {0}},
	{"hkpBoxShape", {4294967295}},
	{"hkpShapeContainer", {0}},
	{"hkpSingleShapeContainer", {4294967295}},
	{"hkpBvShape", {4294967295}},
	{"hkpCapsuleShape", {4294967295}},
	{"hkpCollisionFilter", {0}},
	{"hkpCollisionFilterList", {4294967295}},
	{"hkpConvexListShape", {4294967295}},
	{"hkpConvexTransformShapeBase", {0}},
	{"hkpConvexTransformShape", {4294967295}},
	{"hkpConvexTranslateShape", {4294967295}},
	{"hkpConvexVerticesConnectivity", {4294967295}},
	{"hkpCylinderShape", {4294967295}},
	{"hkpConvexListFilter", {0}},
	{"hkpDefaultConvexListFilter", {4294967295}},
	{"hkpGroupFilter", {4294967295}},
	{"hkpListShapeChildInfo", {0}},
	{"hkpShapeCollection", {0}},
	{"hkpListShape", {4294967295}},
	{"hkpMoppCodeCodeInfo", {0}},
	{"hkpMoppCode", {4294967295}},
	{"hkpBvTreeShape", {0}},
	{"hkMoppBvTreeShapeBase", {0}},
	{"hkpMoppBvTreeShape", {4294967295}},
	{"hkpMultiRayShapeRay", {0}},
	{"hkpMultiRayShape", {4294967295}},
	{"hkpMultiSphereShape", {4294967295}},
	{"hkpNullCollisionFilter", {4294967295}},
	{"hkpRemoveTerminalsMoppModifier", {4294967295}},
	{"hkpShapeInfo", {4294967295}},
	{"hkpSphereShape", {4294967295}},
	{"hkpTransformShape", {4294967295}},
	{"hkpTriangleShape", {4294967295}},
	{"hkAabb", {0}},
	{"hkpCompressedMeshShapeChunk", {0}},
	{"hkpCompressedMeshShapeConvexPiece", {0}},
	{"hkpMeshMaterial", {0}},
	{"hkpNamedMeshMaterial", {0}},
	{"hkpCompressedMeshShapeBigTriangle", {0}},
	{"hkpCompressedMeshShape", {4294967295}},
	{"hkpConvexVerticesShapeFourVectors", {0}},
	{"hkpConvexVerticesShape", {4294967295}},
	{"hkpConvexPieceStreamData", {4294967295}},
	{"hkpConvexPieceMeshShape", {4294967295}},
	{"hkpExtendedMeshShapeSubpart", {0}},
	{"hkpExtendedMeshShapeShapesSubpart", {0}},
	{"hkpExtendedMeshShapeTrianglesSubpart", {0}},
	{"hkpExtendedMeshShape", {4294967295}},
	{"hkpStorageExtendedMeshShapeMaterial", {0}},
	{"hkpStorageExtendedMeshShapeShapeSubpartStorage", {4294967295}},
	{"hkpStorageExtendedMeshShapeMeshSubpartStorage", {4294967295}},
	{"hkpStorageExtendedMeshShape", {4294967295}},
	{"hkpHeightFieldShape", {0}},
	{"hkpSampledHeightFieldShape", {0}},
	{"hkpCompressedSampledHeightFieldShape", {4294967295}},
	{"hkpPlaneShape", {4294967295}},
	{"hkpStorageSampledHeightFieldShape", {4294967295}},
	{"hkpTriSampledHeightFieldBvTreeShape", {4294967295}},
	{"hkpTriSampledHeightFieldCollection", {4294967295}},
	{"hkpMeshShapeSubpart", {0}},
	{"hkpMeshShape", {4294967295}},
	{"hkpFastMeshShape", {4294967295}},
	{"hkpStorageMeshShapeSubpartStorage", {4294967295}},
	{"hkpStorageMeshShape", {4294967295}},
	{"hkpSimpleMeshShapeTriangle", {0}},
	{"hkpSimpleMeshShape", {4294967295}},
	{"hkMultiThreadCheck", {0}},
	{"hkpCollidableBoundingVolumeData", {0}},
	{"hkpCdBody", {0}},
	{"hkpBroadPhaseHandle", {0}},
	{"hkpTypedBroadPhaseHandle", {0}},
	{"hkpCollidable", {0}},
	{"hkpLinkedCollidable", {0}},
	{"hkpPropertyValue", {0}},
	{"hkpProperty", {0}},
	{"hkpWorldObject", {0}},
	{"hkpPhantom", {0}},
	{"hkpAabbPhantom", {4294967295}},
	{"hkpConstraintData", {0}},
	{"hkpConstraintAtom", {0}},
	{"hkpSetupStabilizationAtom", {0}},
	{"hkpBallSocketConstraintAtom", {0}},
	{"hkpSetLocalTranslationsConstraintAtom", {0}},
	{"hkpBallAndSocketConstraintDataAtoms", {0}},
	{"hkpBallAndSocketConstraintData", {4294967295}},
	{"hkpBridgeConstraintAtom", {0}},
	{"hkpBridgeAtoms", {0}},
	{"hkpConstraintChainData", {0}},
	{"hkpBallSocketChainDataConstraintInfo", {0}},
	{"hkpBallSocketChainData", {4294967295}},
	{"hkSweptTransform", {0}},
	{"hkMotionState", {0}},
	{"hkpKeyframedRigidMotion", {4294967295}},
	{"hkpMaxSizeMotion", {4294967295}},
	{"hkpMotion", {0}},
	{"hkpBoxMotion", {4294967295}},
	{"hkpBreakableConstraintData", {4294967295}},
	{"hkpShapePhantom", {0}},
	{"hkpCachingShapePhantom", {4294967295}},
	{"hkpConstraintMotor", {0}},
	{"hkpLimitedForceConstraintMotor", {0}},
	{"hkpCallbackConstraintMotor", {4294967295}},
	{"hkpCharacterMotion", {4294967295}},
	{"hkpSetLocalTransformsConstraintAtom", {0}},
	{"hkpCogWheelConstraintAtom", {0}},
	{"hkpCogWheelConstraintDataAtoms", {0}},
	{"hkpCogWheelConstraintData", {4294967295}},
	{"hkpAction", {0}},
	{"hkpConstraintChainInstanceAction", {4294967295}},
	{"hkpEntitySmallArraySerializeOverrideType", {0}},
	{"hkpEntityExtendedListeners", {0}},
	{"hkpMaterial", {0}},
	{"hkpModifierConstraintAtom", {0}},
	{"hkpConstraintInstanceSmallArraySerializeOverrideType", {0}},
	{"hkpConstraintInstance", {4294967295}},
	{"hkpEntitySpuCollisionCallback", {0}},
	{"hkpEntity", {4294967295}},
	{"hkpConstraintChainInstance", {4294967295}},
	{"hkpPairCollisionFilterMapPairFilterKeyOverrideType", {0}},
	{"hkpPairCollisionFilter", {4294967295}},
	{"hkpConstraintCollisionFilter", {4294967295}},
	{"hkWorldMemoryAvailableWatchDog", {0}},
	{"hkpDefaultWorldMemoryWatchDog", {4294967295}},
	{"hkpFixedRigidMotion", {4294967295}},
	{"hkpGenericConstraintDataSchemeConstraintInfo", {0}},
	{"hkpGenericConstraintDataScheme", {0}},
	{"hkpGenericConstraintData", {4294967295}},
	{"hkp2dAngConstraintAtom", {0}},
	{"hkpHingeConstraintDataAtoms", {0}},
	{"hkpHingeConstraintData", {4294967295}},
	{"hkpAngLimitConstraintAtom", {0}},
	{"hkpSetLocalRotationsConstraintAtom", {0}},
	{"hkpHingeLimitsDataAtoms", {0}},
	{"hkpHingeLimitsData", {4294967295}},
	{"hkpAngMotorConstraintAtom", {0}},
	{"hkpAngFrictionConstraintAtom", {0}},
	{"hkpLimitedHingeConstraintDataAtoms", {0}},
	{"hkpLimitedHingeConstraintData", {4294967295}},
	{"hkpParametricCurve", {0}},
	{"hkpLinearParametricCurve", {4294967295}},
	{"hkpMalleableConstraintData", {4294967295}},
	{"hkpRigidBody", {4294967295}},
	{"hkpPhysicsSystem", {4294967295}},
	{"hkpPointToPathConstraintData", {4294967295}},
	{"hkpLinConstraintAtom", {0}},
	{"hkpPointToPlaneConstraintDataAtoms", {0}},
	{"hkpPointToPlaneConstraintData", {4294967295}},
	{"hkpPositionConstraintMotor", {4294967295}},
	{"hkpLinLimitConstraintAtom", {0}},
	{"hkpAngConstraintAtom", {0}},
	{"hkpLinFrictionConstraintAtom", {0}},
	{"hkpLinMotorConstraintAtom", {0}},
	{"hkpPrismaticConstraintDataAtoms", {0}},
	{"hkpPrismaticConstraintData", {4294967295}},
	{"hkpPulleyConstraintAtom", {0}},
	{"hkpPulleyConstraintDataAtoms", {0}},
	{"hkpPulleyConstraintData", {4294967295}},
	{"hkpRackAndPinionConstraintAtom", {0}},
	{"hkpRackAndPinionConstraintDataAtoms", {0}},
	{"hkpRackAndPinionConstraintData", {4294967295}},
	{"hkpRagdollMotorConstraintAtom", {0}},
	{"hkpTwistLimitConstraintAtom", {0}},
	{"hkpConeLimitConstraintAtom", {0}},
	{"hkpRagdollConstraintDataAtoms", {0}},
	{"hkpRagdollConstraintData", {4294967295}},
	{"hkpRagdollLimitsDataAtoms", {0}},
	{"hkpRagdollLimitsData", {4294967295}},
	{"hkpRotationalConstraintDataAtoms", {0}},
	{"hkpRotationalConstraintData", {4294967295}},
	{"hkpSimpleShapePhantomCollisionDetail", {0}},
	{"hkpSimpleShapePhantom", {4294967295}},
	{"hkpWorld", {4294967295}},
	{"hkpSimulation", {4294967295}},
	{"hkpSphereMotion", {4294967295}},
	{"hkpSpringDamperConstraintMotor", {4294967295}},
	{"hkpStiffSpringChainDataConstraintInfo", {0}},
	{"hkpStiffSpringChainData", {4294967295}},
	{"hkpStiffSpringConstraintAtom", {0}},
	{"hkpStiffSpringConstraintDataAtoms", {0}},
	{"hkpStiffSpringConstraintData", {4294967295}},
	{"hkpThinBoxMotion", {4294967295}},
	{"hkpVelocityConstraintMotor", {4294967295}},
	{"hkpLinSoftConstraintAtom", {0}},
	{"hkpWheelConstraintDataAtoms", {0}},
	{"hkpWheelConstraintData", {4294967295}},
	{"hkpWorldCinfo", {4294967295}},
	{"hkpPoweredChainDataConstraintInfo", {0}},
	{"hkpPoweredChainData", {4294967295}},
	{"hkpBinaryAction", {0}},
	{"hkpAngularDashpotAction", {4294967295}},
	{"hkpFirstPersonGun", {4294967295}},
	{"hkpBallGun", {4294967295}},
	{"hkpCharacterControllerCinfo", {4294967295}},
	{"hkpCharacterProxyCinfo", {4294967295}},
	{"hkpCharacterRigidBodyCinfo", {4294967295}},
	{"hkpConstrainedSystemFilter", {4294967295}},
	{"hkpDashpotAction", {4294967295}},
	{"hkpDisableEntityCollisionFilter", {4294967295}},
	{"hkpDisplayBindingDataRigidBody", {4294967295}},
	{"hkpDisplayBindingDataPhysicsSystem", {4294967295}},
	{"hkpDisplayBindingData", {4294967295}},
	{"hkpGravityGun", {4294967295}},
	{"hkpGroupCollisionFilter", {4294967295}},
	{"hkpUnaryAction", {0}},
	{"hkpMotorAction", {4294967295}},
	{"hkpMountedBallGun", {4294967295}},
	{"hkpMouseSpringAction", {4294967295}},
	{"hkpPhysicsData", {4294967295}},
	{"hkContactPoint", {0}},
	{"hkpSerializedSubTrack1nInfo", {0}},
	{"hkpAgent1nSector", {0}},
	{"hkpSerializedTrack1nInfo", {0}},
	{"hkpSimpleContactConstraintDataInfo", {0}},
	{"hkpSimpleContactConstraintAtom", {0}},
	{"hkpSerializedAgentNnEntry", {4294967295}},
	{"hkpPhysicsSystemWithContacts", {4294967295}},
	{"hkpPoweredChainMapperLinkInfo", {0}},
	{"hkpPoweredChainMapperTarget", {0}},
	{"hkpPoweredChainMapper", {4294967295}},
	{"hkpProjectileGun", {4294967295}},
	{"hkpReorientAction", {4294967295}},
	{"hkpSerializedDisplayMarker", {4294967295}},
	{"hkpSerializedDisplayMarkerList", {4294967295}},
	{"hkpSerializedDisplayRbTransformsDisplayTransformPair", {0}},
	{"hkpSerializedDisplayRbTransforms", {4294967295}},
	{"hkpSpringAction", {4294967295}},
	{"hkpTriggerVolumeEventInfo", {0}},
	{"hkpTriggerVolume", {4294967295}},
	{"hkaAnimatedReferenceFrame", {0}},
	{"hkaAnnotationTrackAnnotation", {0}},
	{"hkaAnnotationTrack", {0}},
	{"hkaAnimation", {0}},
	{"hkaAnimationBinding", {4294967295}},
	{"hkaSkeletonLocalFrameOnBone", {0}},
	{"hkaBone", {0}},
	{"hkaSkeleton", {4294967295}},
	{"hkaMeshBindingMapping", {0}},
	{"hkaMeshBinding", {4294967295}},
	{"hkaBoneAttachment", {4294967295}},
	{"hkaAnimationContainer", {4294967295}},
	{"hkaAnimationPreviewColorContainer", {4294967295}},
	{"hkaDefaultAnimatedReferenceFrame", {4294967295}},
	{"hkaDeltaCompressedAnimationQuantizationFormat", {0}},
	{"hkaDeltaCompressedAnimation", {4294967295}},
	{"hkaFootstepAnalysisInfo", {4294967295}},
	{"hkaFootstepAnalysisInfoContainer", {4294967295}},
	{"hkaInterleavedUncompressedAnimation", {4294967295}},
	{"hkaQuantizedAnimation", {4294967295}},
	{"hkaSkeletonMapperDataChainMapping", {0}},
	{"hkaSkeletonMapperDataSimpleMapping", {0}},
	{"hkaSkeletonMapperData", {0}},
	{"hkaSkeletonMapper", {4294967295}},
	{"hkaSplineCompressedAnimation", {4294967295}},
	{"hkaWaveletCompressedAnimationQuantizationFormat", {0}},
	{"hkaWaveletCompressedAnimation", {4294967295}},
	{"hkaRagdollInstance", {4294967295}},
	{"hkbVariableBindingSetBinding", {0}},
	{"hkbVariableBindingSet", {4294967295}},
	{"hkbBindable", {4294967295}},
	{"hkbNode", {4294967295}},
	{"hkbModifier", {4294967295}},
	{"hkbEventPayload", {4294967295}},
	{"hkbEventBase", {0}},
	{"hkbEventProperty", {0}},
	{"hkbProjectStringData", {4294967295}},
	{"hkbProjectData", {4294967295}},
	{"hkbHandIkDriverInfoHand", {0}},
	{"hkbHandIkDriverInfo", {4294967295}},
	{"hkbRoleAttribute", {0}},
	{"hkbVariableInfo", {0}},
	{"hkbCharacterDataCharacterControllerInfo", {0}},
	{"hkbFootIkDriverInfoLeg", {0}},
	{"hkbFootIkDriverInfo", {4294967295}},
	{"hkbMirroredSkeletonInfo", {4294967295}},
	{"hkbCharacterStringData", {4294967295}},
	{"hkbVariableValue", {0}},
	{"hkbVariableValueSet", {4294967295}},
	{"hkbCharacterData", {4294967295}},
	{"hkbCharacterSetup", {4294967295}},
	{"hkbGenerator", {0}},
	{"hkbBehaviorGraphStringData", {4294967295}},
	{"hkbEventInfo", {0}},
	{"hkbBehaviorGraphData", {4294967295}},
	{"hkbBehaviorGraph", {4294967295}},
	{"hkbCharacter", {4294967295}},
	{"hkbHandle", {4294967295}},
	{"hkbAttachmentSetup", {4294967295}},
	{"hkbAttachmentModifier", {4294967295}},
	{"hkbAttributeModifierAssignment", {0}},
	{"hkbAttributeModifier", {4294967295}},
	{"hkbGeneratorSyncInfoSyncPoint", {0}},
	{"hkbGeneratorSyncInfo", {0}},
	{"hkbNodeInternalStateInfo", {4294967295}},
	{"hkbBehaviorGraphInternalState", {4294967295}},
	{"hkbBehaviorReferenceGenerator", {4294967295}},
	{"hkbBoneWeightArray", {4294967295}},
	{"hkbBlenderGeneratorChild", {4294967295}},
	{"hkbBlenderGenerator", {4294967295}},
	{"hkbBlenderGeneratorChildInternalState", {0}},
	{"hkbBlenderGeneratorInternalState", {4294967295}},
	{"hkbTransitionEffect", {0}},
	{"hkbBlendingTransitionEffect", {4294967295}},
	{"hkbBlendingTransitionEffectInternalState", {4294967295}},
	{"hkbBoneIndexArray", {4294967295}},
	{"hkbBoolVariableSequencedDataSample", {0}},
	{"hkbSequencedData", {0}},
	{"hkbBoolVariableSequencedData", {4294967295}},
	{"hkbCharacterControllerControlData", {0}},
	{"hkbCharacterControllerModifier", {4294967295}},
	{"hkbCharacterControllerModifierInternalState", {4294967295}},
	{"hkbClipTrigger", {0}},
	{"hkbClipTriggerArray", {4294967295}},
	{"hkbClipGenerator", {4294967295}},
	{"hkbClipGeneratorEcho", {0}},
	{"hkbClipGeneratorInternalState", {4294967295}},
	{"hkbCombineTransformsModifier", {4294967295}},
	{"hkbCombineTransformsModifierInternalState", {4294967295}},
	{"hkbCompiledExpressionSetToken", {0}},
	{"hkbCompiledExpressionSet", {4294967295}},
	{"hkbComputeDirectionModifier", {4294967295}},
	{"hkbComputeDirectionModifierInternalState", {4294967295}},
	{"hkbComputeRotationFromAxisAngleModifier", {4294967295}},
	{"hkbComputeRotationFromAxisAngleModifierInternalState", {4294967295}},
	{"hkbComputeRotationToTargetModifier", {4294967295}},
	{"hkbComputeRotationToTargetModifierInternalState", {4294967295}},
	{"hkbDampingModifier", {4294967295}},
	{"hkbDampingModifierInternalState", {4294967295}},
	{"hkbModifierWrapper", {4294967295}},
	{"hkbDelayedModifier", {4294967295}},
	{"hkbDelayedModifierInternalState", {4294967295}},
	{"hkbDetectCloseToGroundModifier", {4294967295}},
	{"hkbDetectCloseToGroundModifierInternalState", {4294967295}},
	{"hkbExpressionData", {0}},
	{"hkbExpressionDataArray", {4294967295}},
	{"hkbEvaluateExpressionModifier", {4294967295}},
	{"hkbEvaluateExpressionModifierInternalExpressionData", {0}},
	{"hkbEvaluateExpressionModifierInternalState", {4294967295}},
	{"hkbEvaluateHandleModifier", {4294967295}},
	{"hkbEventDrivenModifier", {4294967295}},
	{"hkbEventDrivenModifierInternalState", {4294967295}},
	{"hkbEventPayloadList", {4294967295}},
	{"hkbEventRangeData", {0}},
	{"hkbEventRangeDataArray", {4294967295}},
	{"hkbEvent", {0}},
	{"hkbEventSequencedDataSequencedEvent", {0}},
	{"hkbEventSequencedData", {4294967295}},
	{"hkbEventsFromRangeModifier", {4294967295}},
	{"hkbEventsFromRangeModifierInternalState", {4294967295}},
	{"hkbCondition", {0}},
	{"hkbExpressionCondition", {4294967295}},
	{"hkbExtractRagdollPoseModifier", {4294967295}},
	{"hkbExtrapolatingTransitionEffect", {4294967295}},
	{"hkbExtrapolatingTransitionEffectInternalState", {4294967295}},
	{"hkbFootIkGains", {0}},
	{"hkbFootIkControlData", {0}},
	{"hkbFootIkControlsModifierLeg", {0}},
	{"hkbFootIkControlsModifier", {4294967295}},
	{"hkbFootIkModifierLeg", {0}},
	{"hkbFootIkModifierInternalLegData", {0}},
	{"hkbFootIkModifier", {4294967295}},
	{"hkbGeneratorTransitionEffect", {4294967295}},
	{"hkbGeneratorTransitionEffectInternalState", {4294967295}},
	{"hkbGetHandleOnBoneModifier", {4294967295}},
	{"hkbGetUpModifier", {4294967295}},
	{"hkbGetUpModifierInternalState", {4294967295}},
	{"hkbGetWorldFromModelModifier", {4294967295}},
	{"hkbGetWorldFromModelModifierInternalState", {4294967295}},
	{"hkbHandIkControlData", {0}},
	{"hkbHandIkControlsModifierHand", {0}},
	{"hkbHandIkControlsModifier", {4294967295}},
	{"hkbHandIkModifierHand", {0}},
	{"hkbHandIkModifier", {4294967295}},
	{"hkbIntEventPayload", {4294967295}},
	{"hkbIntVariableSequencedDataSample", {0}},
	{"hkbIntVariableSequencedData", {4294967295}},
	{"hkbJigglerGroup", {4294967295}},
	{"hkbJigglerModifier", {4294967295}},
	{"hkbJigglerModifierInternalState", {4294967295}},
	{"hkbKeyframeBonesModifierKeyframeInfo", {0}},
	{"hkbKeyframeBonesModifier", {4294967295}},
	{"hkbLookAtModifier", {4294967295}},
	{"hkbLookAtModifierInternalState", {4294967295}},
	{"hkbManualSelectorGenerator", {4294967295}},
	{"hkbManualSelectorGeneratorInternalState", {4294967295}},
	{"hkbMirrorModifier", {4294967295}},
	{"hkbModifierGenerator", {4294967295}},
	{"hkbModifierList", {4294967295}},
	{"hkbMoveCharacterModifier", {4294967295}},
	{"hkbMoveCharacterModifierInternalState", {4294967295}},
	{"hkbNamedEventPayload", {4294967295}},
	{"hkbNamedIntEventPayload", {4294967295}},
	{"hkbNamedRealEventPayload", {4294967295}},
	{"hkbNamedStringEventPayload", {4294967295}},
	{"hkbPoseMatchingGenerator", {4294967295}},
	{"hkbPoseMatchingGeneratorInternalState", {4294967295}},
	{"hkbRegisteredGenerator", {4294967295}},
	{"hkbPositionRelativeSelectorGenerator", {4294967295}},
	{"hkbPositionRelativeSelectorGeneratorInternalState", {4294967295}},
	{"hkbPoweredRagdollControlData", {0}},
	{"hkbWorldFromModelModeData", {0}},
	{"hkbPoweredRagdollControlsModifier", {4294967295}},
	{"hkbProxyModifierProxyInfo", {0}},
	{"hkbProxyModifier", {4294967295}},
	{"hkbRealEventPayload", {4294967295}},
	{"hkbRealVariableSequencedDataSample", {0}},
	{"hkbRealVariableSequencedData", {4294967295}},
	{"hkbReferencePoseGenerator", {4294967295}},
	{"hkaKeyFrameHierarchyUtilityControlData", {0}},
	{"hkbRigidBodyRagdollControlData", {0}},
	{"hkbRigidBodyRagdollControlsModifier", {4294967295}},
	{"hkbRotateCharacterModifier", {4294967295}},
	{"hkbRotateCharacterModifierInternalState", {4294967295}},
	{"hkbSenseHandleModifierRange", {0}},
	{"hkbSenseHandleModifier", {4294967295}},
	{"hkbSequenceStringData", {4294967295}},
	{"hkbSequence", {4294967295}},
	{"hkbSequenceInternalState", {4294967295}},
	{"hkbSetWorldFromModelModifier", {4294967295}},
	{"hkbSplinePathGenerator", {4294967295}},
	{"hkbSplinePathGeneratorInternalState", {4294967295}},
	{"hkbStateListener", {4294967295}},
	{"hkbStateChooser", {0}},
	{"hkbStateMachineEventPropertyArray", {4294967295}},
	{"hkbStateMachineTimeInterval", {0}},
	{"hkbStateMachineTransitionInfo", {0}},
	{"hkbStateMachineTransitionInfoArray", {4294967295}},
	{"hkbStateMachineStateInfo", {4294967295}},
	{"hkbStateMachine", {4294967295}},
	{"hkbStateMachineTransitionInfoReference", {0}},
	{"hkbStateMachineProspectiveTransitionInfo", {0}},
	{"hkbStateMachineDelayedTransitionInfo", {0}},
	{"hkbStateMachineActiveTransitionInfo", {0}},
	{"hkbStateMachineInternalState", {4294967295}},
	{"hkbStringCondition", {4294967295}},
	{"hkbStringEventPayload", {4294967295}},
	{"hkbTimerModifier", {4294967295}},
	{"hkbTimerModifierInternalState", {4294967295}},
	{"hkbTransformVectorModifier", {4294967295}},
	{"hkbTransformVectorModifierInternalState", {4294967295}},
	{"hkbTwistModifier", {4294967295}},
	{"hkbAlignBoneModifier", {4294967295}},
	{"hkbAnimatedSkeletonGenerator", {4294967295}},
	{"hkbAuxiliaryNodeInfo", {4294967295}},
	{"hkbBalanceModifierStepInfo", {0}},
	{"hkbBalanceModifier", {4294967295}},
	{"hkbRadialSelectorGeneratorGeneratorInfo", {0}},
	{"hkbRadialSelectorGeneratorGeneratorPair", {0}},
	{"hkbRadialSelectorGenerator", {4294967295}},
	{"hkbCheckBalanceModifier", {4294967295}},
	{"hkbBalanceRadialSelectorGenerator", {4294967295}},
	{"hkbBehaviorEventsInfo", {4294967295}},
	{"hkbBehaviorGraphInternalStateInfo", {4294967295}},
	{"hkbBehaviorInfoIdToNamePair", {0}},
	{"hkbBehaviorInfo", {4294967295}},
	{"hkbCameraShakeEventPayload", {4294967295}},
	{"hkbCatchFallModifierHand", {0}},
	{"hkbCatchFallModifier", {4294967295}},
	{"hkbCharacterAddedInfo", {4294967295}},
	{"hkbCharacterControlCommand", {4294967295}},
	{"hkbCharacterInfo", {4294967295}},
	{"hkbCharacterSkinInfo", {4294967295}},
	{"hkbCharacterSteppedInfo", {4294967295}},
	{"hkbCheckRagdollSpeedModifier", {4294967295}},
	{"hkbClientCharacterState", {4294967295}},
	{"hkbComputeWorldFromModelModifier", {4294967295}},
	{"hkbTarget", {4294967295}},
	{"hkbConstrainRigidBodyModifier", {4294967295}},
	{"hkbControlledReachModifier", {4294967295}},
	{"hkbCustomTestGeneratorStruck", {0}},
	{"hkbCustomTestGenerator", {4294967295}},
	{"hkbDemoConfigCharacterInfo", {4294967295}},
	{"hkbDemoConfigStickVariableInfo", {0}},
	{"hkbDemoConfigTerrainInfo", {0}},
	{"hkbDemoConfig", {4294967295}},
	{"hkbEventRaisedInfo", {4294967295}},
	{"hkbFaceTargetModifier", {4294967295}},
	{"hkbGravityModifier", {4294967295}},
	{"hkbHoldFromBlendingTransitionEffect", {4294967295}},
	{"hkbLinkedSymbolInfo", {4294967295}},
	{"hkbMoveBoneTowardTargetModifier", {4294967295}},
	{"hkbParticleSystemEventPayload", {4294967295}},
	{"hkbGeneratorOutputListener", {0}},
	{"hkbPoseStoringGeneratorOutputListenerStoredPose", {4294967295}},
	{"hkbPoseStoringGeneratorOutputListener", {4294967295}},
	{"hkbRaiseEventCommand", {4294967295}},
	{"hkbReachModifierHand", {0}},
	{"hkbReachModifier", {4294967295}},
	{"hkbReachTowardTargetModifierHand", {0}},
	{"hkbReachTowardTargetModifier", {4294967295}},
	{"hkbSetBehaviorCommand", {4294967295}},
	{"hkbSetLocalTimeOfClipGeneratorCommand", {4294967295}},
	{"hkbSetNodePropertyCommand", {4294967295}},
	{"hkbSetWordVariableCommand", {4294967295}},
	{"hkbSimulationControlCommand", {4294967295}},
	{"hkbSimulationStateInfo", {4294967295}},
	{"hkbStateDependentModifier", {4294967295}},
	{"hkbTargetRigidBodyModifier", {4294967295}},
	{"hkbTestStateChooser", {4294967295}},
	{"BGSGamebryoSequenceGenerator", {4294967295}},
	{"BSBoneSwitchGeneratorBoneData", {4294967295}},
	{"BSBoneSwitchGenerator", {4294967295}},
	{"BSComputeAddBoneAnimModifier", {4294967295}},
	{"BSCyclicBlendTransitionGenerator", {4294967295}},
	{"BSDecomposeVectorModifier", {4294967295}},
	{"BSDirectAtModifier", {4294967295}},
	{"BSDistTriggerModifier", {4294967295}},
	{"BSEventEveryNEventsModifier", {4294967295}},
	{"BSEventOnDeactivateModifier", {4294967295}},
	{"BSEventOnFalseToTrueModifier", {4294967295}},
	{"BSGetTimeStepModifier", {4294967295}},
	{"BSIStateManagerModifierBSiStateData", {0}},
	{"BSIStateManagerModifierBSIStateManagerStateListener", {4294967295}},
	{"BSIStateManagerModifier", {4294967295}},
	{"BSInterpValueModifier", {4294967295}},
	{"BSIsActiveModifier", {4294967295}},
	{"BSLimbIKModifier", {4294967295}},
	{"BSLookAtModifierBoneData", {0}},
	{"BSLookAtModifier", {4294967295}},
	{"BSModifyOnceModifier", {4294967295}},
	{"BSOffsetAnimationGenerator", {4294967295}},
	{"BSPassByTargetTriggerModifier", {4294967295}},
	{"BSRagdollContactListenerModifier", {4294967295}},
	{"BSSpeedSamplerModifier", {4294967295}},
	{"BSSynchronizedClipGenerator", {4294967295}},
	{"BSTimerModifier", {4294967295}},
	{"BSTweenerModifier", {4294967295}},
	{"BSiStateTaggingGenerator", {4294967295}},
	{"hkAabbHalf", {0}},
	{"hkAabbUint32", {0}},
	{"hkArrayTypeAttribute", {0}},
	{"hkBitField", {0}},
	{"hkCustomAttributesAttribute", {0}},
	{"hkCustomAttributes", {0}},
	{"hkClassEnumItem", {0}},
	{"hkClassEnum", {0}},
	{"hkClassMember", {0}},
	{"hkClass", {0}},
	{"hkColor", {0}},
	{"hkContactPointMaterial", {0}},
	{"hkDataObjectTypeAttribute", {0}},
	{"hkDescriptionAttribute", {0}},
	{"hkDocumentationAttribute", {0}},
	{"hkGeometryTriangle", {0}},
	{"hkGeometry", {0}},
	{"hkGizmoAttribute", {0}},
	{"hkHalf8", {0}},
	{"hkLinkAttribute", {0}},
	{"hkMemoryTrackerAttribute", {0}},
	{"hkModelerNodeTypeAttribute", {0}},
	{"hkPackedVector3", {0}},
	{"hkPostFinishAttribute", {0}},
	{"hkQTransform", {0}},
	{"hkRangeInt32Attribute", {0}},
	{"hkRangeRealAttribute", {0}},
	{"hkReflectedFileAttribute", {0}},
	{"hkSemanticsAttribute", {0}},
	{"hkSphere", {0}},
	{"hkTraceStreamTitle", {0}},
	{"hkUiAttribute", {0}},
	{"hkMonitorStreamFrameInfo", {0}},
	{"hkMonitorStreamStringMapStringMap", {0}},
	{"hkMonitorStreamStringMap", {0}},
	{"hkPackfileHeader", {0}},
	{"hkPackfileSectionHeader", {0}},
	{"hkRootLevelContainerNamedVariant", {0}},
	{"hkRootLevelContainer", {0}},
	{"hkMeshSection", {0}},
	{"hkpMassProperties", {0}},
	{"hkpWeldingUtility", {0}},
	{"hkpCenterOfMassChangerModifierConstraintAtom", {0}},
	{"hkpIgnoreModifierConstraintAtom", {0}},
	{"hkpMassChangerModifierConstraintAtom", {0}},
	{"hkpMovingSurfaceModifierConstraintAtom", {0}},
	{"hkpOverwritePivotConstraintAtom", {0}},
	{"hkpSoftContactModifierConstraintAtom", {0}},
	{"hkpViscousSurfaceModifierConstraintAtom", {0}},
	{"hkpMoppCodeReindexedTerminal", {0}},
	{"hkaQuantizedAnimationTrackCompressionParams", {0}},
	{"hkaSplineCompressedAnimationAnimationCompressionParams", {0}},
	{"hkaSplineCompressedAnimationTrackCompressionParams", {0}},
	{"hkaWaveletCompressedAnimationCompressionParams", {0}},
	{"hkaKeyFrameHierarchyUtility", {0}},
	{"hkbBlendCurveUtils", {0}},
	{"hkbContext", {0}},
	{"hkbDefaultMessageLog", {0}},
	{"hkbMessageLog", {0}},
	{"hkbStateMachineNestedStateMachineData", {0}},
	{"hkbWorldEnums", {0}},
	{"hkVariableTweakingHelperBoolVariableInfo", {0}},
	{"hkVariableTweakingHelperVector4VariableInfo", {0}},
	{"hkVariableTweakingHelperRealVariableInfo", {0}},
	{"hkVariableTweakingHelperIntVariableInfo", {0}},
	{"hkVariableTweakingHelper", {0}},
	{"hkpCollidableCollidableFilter", {0}},
	{"hkpPhantomCallbackShape", {0}},
	{"hkpRayCollidableFilter", {0}},
	{"hkpRayShapeCollectionFilter", {0}},
	{"hkpShapeCollectionFilter", {0}},
	{"hkpShapeModifier", {0}},
	{"hkpArrayAction", {0}},
	{"hkpBreakableBody", {0}},
};

bool has_vtable(std::string className)
{
	auto it = typeinfos.find(className);
	if (it != typeinfos.end())
	{
		return it->second.vtable != 0;
	}
	return false;
}

#endif