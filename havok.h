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

struct hkVariant
{
    void *m_object;
    hkClass *m_class;
};

struct hkVector4
{
    float x, y, z, w;
};

struct hkMatrix3
{
    hkVector4 col0; // 00
    hkVector4 col1; // 10
    hkVector4 col2; // 20
};

struct hkMatrix4
{
    hkVector4 col0;
    hkVector4 col1;
    hkVector4 col2;
    hkVector4 col3;
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
    hkVector4 vec;
};

struct hkTransform
{
    hkMatrix3x4 rotation;
	hkVector4	translation;
};

struct hkQsTransform
{
    hkVector4		translation;
	hkQuaternion	rotation;
	hkVector4		scale;
};

template <typename T>
class hkArrayBase
{
public:
    T *data;
    int32 size;
    int32 capacityAndFlags;
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
class hkLocalFrameGroup : public hkReferencedObject
{
public:
    virtual ~hkLocalFrameGroup();
    hkStringPtr name;
};
class hkReferencedObject : public hkBaseObject
{
public:
    virtual ~hkReferencedObject();
    uint16 memSizeAndFlags;
    int16 referenceCount;
};
class hkSimpleLocalFrame : public hkLocalFrame
{
public:
    virtual ~hkSimpleLocalFrame();
    hkTransform transform;
    hkArray<hkLocalFrame *> children;
    struct hkLocalFramehkLocalFrame *parentFrame;
    struct hkLocalFrameGrouphkLocalFrameGroup *group;
    hkStringPtr name;
};
class hkTrackerSerializableScanSnapshot : public hkReferencedObject
{
public:
    virtual ~hkTrackerSerializableScanSnapshot();
    hkArray<struct hkTrackerSerializableScanSnapshotAllocation> allocations;
    hkArray<struct hkTrackerSerializableScanSnapshotBlock> blocks;
    hkArray<int32> refs;
    hkArray<uint8> typeNames;
    hkArray<uint8> traceText;
    hkArray<uint64> traceAddrs;
    hkArray<int32> traceParents;
};
class hkMonitorStreamColorTable : public hkReferencedObject
{
public:
    virtual ~hkMonitorStreamColorTable();
    hkArray<struct hkMonitorStreamColorTableColorPair> colorPairs;
    uint32 defaultColor;
};
class hkMemoryResourceContainer : public hkResourceContainer
{
public:
    virtual ~hkMemoryResourceContainer();
    hkStringPtr name;
    struct hkMemoryResourceContainerhkMemoryResourceContainer *parent;
    hkArray<hkMemoryResourceHandle *> resourceHandles;
    hkArray<hkMemoryResourceContainer *> children;
};
class hkMemoryResourceHandle : public hkResourceHandle
{
public:
    virtual ~hkMemoryResourceHandle();
    struct hkReferencedObjecthkReferencedObject *variant;
    hkStringPtr name;
    hkArray<struct hkMemoryResourceHandleExternalLink> references;
};
class hkAlignSceneToNodeOptions : public hkReferencedObject
{
public:
    virtual ~hkAlignSceneToNodeOptions();
    bool invert;
    bool transformPositionX;
    bool transformPositionY;
    bool transformPositionZ;
    bool transformRotation;
    bool transformScale;
    bool transformSkew;
    int32 keyframe;
    hkStringPtr nodeName;
};
class hkxAnimatedFloat : public hkReferencedObject
{
public:
    virtual ~hkxAnimatedFloat();
    hkArray<float> floats;
    uint8 hint;
};
class hkxAnimatedMatrix : public hkReferencedObject
{
public:
    virtual ~hkxAnimatedMatrix();
    hkArray<hkMatrix4> matrices;
    uint8 hint;
};
class hkxAnimatedQuaternion : public hkReferencedObject
{
public:
    virtual ~hkxAnimatedQuaternion();
    hkArray<hkQuaternion> quaternions;
};
class hkxAnimatedVector : public hkReferencedObject
{
public:
    virtual ~hkxAnimatedVector();
    hkArray<hkVector4> vectors;
    uint8 hint;
};
class hkxAttributeHolder : public hkReferencedObject
{
public:
    virtual ~hkxAttributeHolder();
    hkArray<struct hkxAttributeGroup> attributeGroups;
};
class hkxCamera : public hkReferencedObject
{
public:
    virtual ~hkxCamera();
    hkVector4 from;
    hkVector4 focus;
    hkVector4 up;
    float fov;
    float far;
    float near;
    bool leftHanded;
};
class hkxEdgeSelectionChannel : public hkReferencedObject
{
public:
    virtual ~hkxEdgeSelectionChannel();
    hkArray<int32> selectedEdges;
};
class hkxEnum : public hkReferencedObject
{
public:
    virtual ~hkxEnum();
    hkArray<struct hkxEnumItem> items;
};
class hkxEnvironment : public hkReferencedObject
{
public:
    virtual ~hkxEnvironment();
    hkArray<struct hkxEnvironmentVariable> variables;
};
class hkxIndexBuffer : public hkReferencedObject
{
public:
    virtual ~hkxIndexBuffer();
    int8 indexType;
    hkArray<uint16> indices16;
    hkArray<uint32> indices32;
    uint32 vertexBaseOffset;
    uint32 length;
};
class hkxLight : public hkReferencedObject
{
public:
    virtual ~hkxLight();
    int8 type;
    hkVector4 position;
    hkVector4 direction;
    uint32 color;
    float angle;
};
class hkxMaterial : public hkxAttributeHolder
{
public:
    virtual ~hkxMaterial();
    hkStringPtr name;
    hkArray<struct hkxMaterialTextureStage> stages;
    hkVector4 diffuseColor;
    hkVector4 ambientColor;
    hkVector4 specularColor;
    hkVector4 emissiveColor;
    hkArray<hkxMaterial *> subMaterials;
    struct hkReferencedObjecthkReferencedObject *extraData;
    hkArray<struct hkxMaterialProperty> properties;
};
class hkxMaterialEffect : public hkReferencedObject
{
public:
    virtual ~hkxMaterialEffect();
    hkStringPtr name;
    uint8 type;
    hkArray<uint8> data;
};
class hkxMaterialShader : public hkReferencedObject
{
public:
    virtual ~hkxMaterialShader();
    hkStringPtr name;
    uint8 type;
    hkStringPtr vertexEntryName;
    hkStringPtr geomEntryName;
    hkStringPtr pixelEntryName;
    hkArray<uint8> data;
};
class hkxMaterialShaderSet : public hkReferencedObject
{
public:
    virtual ~hkxMaterialShaderSet();
    hkArray<hkxMaterialShader *> shaders;
};
class hkxMesh : public hkReferencedObject
{
public:
    virtual ~hkxMesh();
    hkArray<hkxMeshSection *> sections;
    hkArray<hkxMeshUserChannelInfo *> userChannelInfos;
};
class hkxMeshSection : public hkReferencedObject
{
public:
    virtual ~hkxMeshSection();
    struct hkxVertexBufferhkxVertexBuffer *vertexBuffer;
    hkArray<hkxIndexBuffer *> indexBuffers;
    struct hkxMaterialhkxMaterial *material;
    hkArray<hkReferencedObject *> userChannels;
};
class hkxMeshUserChannelInfo : public hkxAttributeHolder
{
public:
    virtual ~hkxMeshUserChannelInfo();
    hkStringPtr name;
    hkStringPtr className;
};
class hkxNode : public hkxAttributeHolder
{
public:
    virtual ~hkxNode();
    hkStringPtr name;
    struct hkReferencedObjecthkReferencedObject *object;
    hkArray<hkMatrix4> keyFrames;
    hkArray<hkxNode *> children;
    hkArray<struct hkxNodeAnnotationData> annotations;
    hkStringPtr userProperties;
    bool selected;
};
class hkxNodeSelectionSet : public hkxAttributeHolder
{
public:
    virtual ~hkxNodeSelectionSet();
    hkArray<hkxNode *> selectedNodes;
    hkStringPtr name;
};
class hkxScene : public hkReferencedObject
{
public:
    virtual ~hkxScene();
    hkStringPtr modeller;
    hkStringPtr asset;
    float sceneLength;
    struct hkxNodehkxNode *rootNode;
    hkArray<hkxNodeSelectionSet *> selectionSets;
    hkArray<hkxCamera *> cameras;
    hkArray<hkxLight *> lights;
    hkArray<hkxMesh *> meshes;
    hkArray<hkxMaterial *> materials;
    hkArray<hkxTextureInplace *> inplaceTextures;
    hkArray<hkxTextureFile *> externalTextures;
    hkArray<hkxSkinBinding *> skinBindings;
    hkMatrix3 appliedTransform;
};
class hkxSkinBinding : public hkReferencedObject
{
public:
    virtual ~hkxSkinBinding();
    struct hkxMeshhkxMesh *mesh;
    hkArray<hkStringPtr> nodeNames;
    hkArray<hkMatrix4> bindPose;
    hkMatrix4 initSkinTransform;
};
class hkxSparselyAnimatedBool : public hkReferencedObject
{
public:
    virtual ~hkxSparselyAnimatedBool();
    hkArray<bool> bools;
    hkArray<float> times;
};

class hkxSparselyAnimatedInt : public hkReferencedObject
{
public:
    virtual ~hkxSparselyAnimatedInt();
    hkArray<int32> ints;
    hkArray<float> times;
};
class hkxSparselyAnimatedString : public hkReferencedObject
{
public:
    virtual ~hkxSparselyAnimatedString();
    hkArray<hkStringPtr> strings;
    hkArray<float> times;
};
class hkxTextureFile : public hkReferencedObject
{
public:
    virtual ~hkxTextureFile();
    hkStringPtr filename;
    hkStringPtr name;
    hkStringPtr originalFilename;
};
class hkxTextureInplace : public hkReferencedObject
{
public:
    virtual ~hkxTextureInplace();
    char fileType;
    hkArray<uint8> data;
    hkStringPtr name;
    hkStringPtr originalFilename;
};
class hkxTriangleSelectionChannel : public hkReferencedObject
{
public:
    virtual ~hkxTriangleSelectionChannel();
    hkArray<int32> selectedTriangles;
};
class hkxVertexBuffer : public hkReferencedObject
{
public:
    virtual ~hkxVertexBuffer();
    struct hkxVertexBufferVertexData data;
    struct hkxVertexDescription desc;
};
class hkxVertexFloatDataChannel : public hkReferencedObject
{
public:
    virtual ~hkxVertexFloatDataChannel();
    hkArray<float> perVertexFloats;
    uint8 dimensions;
};
class hkxVertexIntDataChannel : public hkReferencedObject
{
public:
    virtual ~hkxVertexIntDataChannel();
    hkArray<int32> perVertexInts;
};
class hkxVertexSelectionChannel : public hkReferencedObject
{
public:
    virtual ~hkxVertexSelectionChannel();
    hkArray<int32> selectedVertices;
};
class hkxVertexVectorDataChannel : public hkReferencedObject
{
public:
    virtual ~hkxVertexVectorDataChannel();
    hkArray<hkVector4> perVertexVectors;
};
class hkIndexedTransformSet : public hkReferencedObject
{
public:
    virtual ~hkIndexedTransformSet();
    hkArray<hkMatrix4> matrices;
    hkArray<hkMatrix4> inverseMatrices;
    hkArray<int16> matricesOrder;
    hkArray<hkStringPtr> matricesNames;
    hkArray<struct hkMeshBoneIndexMapping> indexMappings;
    bool allMatricesAreAffine;
};
class hkMemoryMeshBody : public hkMeshBody
{
public:
    virtual ~hkMemoryMeshBody();
    hkMatrix4 transform;
    struct hkIndexedTransformSethkIndexedTransformSet *transformSet;
    struct hkMeshShapehkMeshShape *shape;
    hkArray<hkMeshVertexBuffer *> vertexBuffers;
    hkStringPtr name;
};
class hkMemoryMeshMaterial : public hkMeshMaterial
{
public:
    virtual ~hkMemoryMeshMaterial();
    hkStringPtr materialName;
    hkArray<hkMeshTexture *> textures;
};
class hkMemoryMeshShape : public hkMeshShape
{
public:
    virtual ~hkMemoryMeshShape();
    hkArray<struct hkMeshSectionCinfo> sections;
    hkArray<uint16> indices16;
    hkArray<uint32> indices32;
    hkStringPtr name;
};
class hkMemoryMeshTexture : public hkMeshTexture
{
public:
    virtual ~hkMemoryMeshTexture();
    hkStringPtr filename;
    hkArray<uint8> data;
    int8 format;
    bool hasMipMaps;
    int8 filterMode;
    int8 usageHint;
    int32 textureCoordChannel;
};
class hkMemoryMeshVertexBuffer : public hkMeshVertexBuffer
{
public:
    virtual ~hkMemoryMeshVertexBuffer();
    struct hkVertexFormat format;
    int32 elementOffsets;
    hkArray<uint8> memory;
    int32 vertexStride;
    bool locked;
    int32 numVertices;
    bool isBigEndian;
    bool isSharable;
};
class hkMultipleVertexBuffer : public hkMeshVertexBuffer
{
public:
    virtual ~hkMultipleVertexBuffer();
    struct hkVertexFormat vertexFormat;
    hkArray<struct hkMultipleVertexBufferLockedElement> lockedElements;
    struct hkMemoryMeshVertexBufferhkMemoryMeshVertexBuffer *lockedBuffer;
    hkArray<struct hkMultipleVertexBufferElementInfo> elementInfos;
    hkArray<struct hkMultipleVertexBufferVertexBufferInfo> vertexBufferInfos;
    int32 numVertices;
    bool isLocked;
    uint32 updateCount;
    bool writeLock;
    bool isSharable;
    bool constructionComplete;
};
class hkpBoxShape : public hkpConvexShape
{
public:
    virtual ~hkpBoxShape();
    hkVector4 halfExtents;
};
class hkpBvShape : public hkpShape
{
public:
    virtual ~hkpBvShape();
    struct hkpShapehkpShape *boundingVolumeShape;
    struct hkpSingleShapeContainer childShape;
};
class hkpCapsuleShape : public hkpConvexShape
{
public:
    virtual ~hkpCapsuleShape();
    hkVector4 vertexA;
    hkVector4 vertexB;
};
class hkpCollisionFilterList : public hkpCollisionFilter
{
public:
    virtual ~hkpCollisionFilterList();
    hkArray<hkpCollisionFilter *> collisionFilters;
};
class hkpConvexListShape : public hkpConvexShape
{
public:
    virtual ~hkpConvexListShape();
    float minDistanceToUseConvexHullForGetClosestPoints;
    hkVector4 aabbHalfExtents;
    hkVector4 aabbCenter;
    bool useCachedAabb;
    hkArray<hkpConvexShape *> childShapes;
};
class hkpConvexTransformShape : public hkpConvexTransformShapeBase
{
public:
    virtual ~hkpConvexTransformShape();
    hkTransform transform;
};
class hkpConvexTranslateShape : public hkpConvexTransformShapeBase
{
public:
    virtual ~hkpConvexTranslateShape();
    hkVector4 translation;
};
class hkpConvexVerticesConnectivity : public hkReferencedObject
{
public:
    virtual ~hkpConvexVerticesConnectivity();
    hkArray<uint16> vertexIndices;
    hkArray<uint8> numVerticesPerFace;
};
class hkpCylinderShape : public hkpConvexShape
{
public:
    virtual ~hkpCylinderShape();
    float cylRadius;
    float cylBaseRadiusFactorForHeightFieldCollisions;
    hkVector4 vertexA;
    hkVector4 vertexB;
    hkVector4 perpendicular1;
    hkVector4 perpendicular2;
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
    int32 nextFreeSystemGroup;
    uint32 collisionLookupTable;
    hkVector4 pad256;
};
class hkpListShape : public hkpShapeCollection
{
public:
    virtual ~hkpListShape();
    hkArray<struct hkpListShapeChildInfo> childInfo;
    uint16 flags;
    uint16 numDisabledChildren;
    hkVector4 aabbHalfExtents;
    hkVector4 aabbCenter;
    uint32 enabledChildren;
};
class hkpMoppBvTreeShape : public hkMoppBvTreeShapeBase
{
public:
    virtual ~hkpMoppBvTreeShape();
    struct hkpSingleShapeContainer child;
    int32 childSize;
};
class hkpMultiRayShape : public hkpShape
{
public:
    virtual ~hkpMultiRayShape();
    hkArray<struct hkpMultiRayShapeRay> rays;
    float rayPenetrationDistance;
};
class hkpMultiSphereShape : public hkpSphereRepShape
{
public:
    virtual ~hkpMultiSphereShape();
    int32 numSpheres;
    hkVector4 spheres;
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
    hkArray<uint32> removeInfo;
    void *tempShapesToRemove;
};
class hkpShapeInfo : public hkReferencedObject
{
public:
    virtual ~hkpShapeInfo();
    struct hkpShapehkpShape *shape;
    bool isHierarchicalCompound;
    bool hkdShapesCollected;
    hkArray<hkStringPtr> childShapeNames;
    hkArray<hkTransform> childTransforms;
    hkTransform transform;
};
class hkpSingleShapeContainer : public hkpShapeContainer
{
public:
    virtual ~hkpSingleShapeContainer();
    struct hkpShapehkpShape *childShape;
};
class hkpSphereShape : public hkpConvexShape
{
public:
    virtual ~hkpSphereShape();
    uint32 pad16;
};
class hkpTransformShape : public hkpShape
{
public:
    virtual ~hkpTransformShape();
    struct hkpSingleShapeContainer childShape;
    int32 childShapeSize;
    hkQuaternion rotation;
    hkTransform transform;
};
class hkpTriangleShape : public hkpConvexShape
{
public:
    virtual ~hkpTriangleShape();
    uint16 weldingInfo;
    uint8 weldingType;
    uint8 isExtruded;
    hkVector4 vertexA;
    hkVector4 vertexB;
    hkVector4 vertexC;
    hkVector4 extrusion;
};
class hkpCompressedMeshShape : public hkpShapeCollection
{
public:
    virtual ~hkpCompressedMeshShape();
    int32 bitsPerIndex;
    int32 bitsPerWIndex;
    int32 wIndexMask;
    int32 indexMask;
    float radius;
    uint8 weldingType;
    uint8 materialType;
    hkArray<uint32> materials;
    hkArray<uint16> materials16;
    hkArray<uint8> materials8;
    hkArray<hkQsTransform> transforms;
    hkArray<hkVector4> bigVertices;
    hkArray<struct hkpCompressedMeshShapeBigTriangle> bigTriangles;
    hkArray<struct hkpCompressedMeshShapeChunk> chunks;
    hkArray<struct hkpCompressedMeshShapeConvexPiece> convexPieces;
    float error;
    struct hkAabb bounds;
    uint32 defaultCollisionFilterInfo;
    void *meshMaterials;
    uint16 materialStriding;
    uint16 numMaterials;
    hkArray<struct hkpNamedMeshMaterial> namedMaterials;
};
class hkpConvexVerticesShape : public hkpConvexShape
{
public:
    virtual ~hkpConvexVerticesShape();
    hkVector4 aabbHalfExtents;
    hkVector4 aabbCenter;
    hkArray<struct hkpConvexVerticesShapeFourVectors> rotatedVertices;
    int32 numVertices;
    void *externalObject;
    void *getFaceNormals;
    hkArray<hkVector4> planeEquations;
    struct hkpConvexVerticesConnectivityhkpConvexVerticesConnectivity *connectivity;
};
class hkpConvexPieceMeshShape : public hkpShapeCollection
{
public:
    virtual ~hkpConvexPieceMeshShape();
    struct hkpConvexPieceStreamDatahkpConvexPieceStreamData *convexPieceStream;
    struct hkpShapeCollectionhkpShapeCollection *displayMesh;
    float radius;
};
class hkpExtendedMeshShape : public hkpShapeCollection
{
public:
    virtual ~hkpExtendedMeshShape();
    struct hkpExtendedMeshShapeTrianglesSubpart embeddedTrianglesSubpart;
    hkVector4 aabbHalfExtents;
    hkVector4 aabbCenter;
    void *materialClass;
    int32 numBitsForSubpartIndex;
    hkArray<struct hkpExtendedMeshShapeTrianglesSubpart> trianglesSubparts;
    hkArray<struct hkpExtendedMeshShapeShapesSubpart> shapesSubparts;
    hkArray<uint16> weldingInfo;
    uint8 weldingType;
    uint32 defaultCollisionFilterInfo;
    int32 cachedNumChildShapes;
    float triangleRadius;
    int32 padding;
};
class hkpStorageExtendedMeshShape : public hkpExtendedMeshShape
{
public:
    virtual ~hkpStorageExtendedMeshShape();
    hkArray<hkpStorageExtendedMeshShapeMeshSubpartStorage *> meshstorage;
    hkArray<hkpStorageExtendedMeshShapeShapeSubpartStorage *> shapestorage;
};
class hkpStorageExtendedMeshShapeMeshSubpartStorage : public hkReferencedObject
{
public:
    virtual ~hkpStorageExtendedMeshShapeMeshSubpartStorage();
    hkArray<hkVector4> vertices;
    hkArray<uint8> indices8;
    hkArray<uint16> indices16;
    hkArray<uint32> indices32;
    hkArray<uint8> materialIndices;
    hkArray<struct hkpStorageExtendedMeshShapeMaterial> materials;
    hkArray<struct hkpNamedMeshMaterial> namedMaterials;
    hkArray<uint16> materialIndices16;
};
class hkpStorageExtendedMeshShapeShapeSubpartStorage : public hkReferencedObject
{
public:
    virtual ~hkpStorageExtendedMeshShapeShapeSubpartStorage();
    hkArray<uint8> materialIndices;
    hkArray<struct hkpStorageExtendedMeshShapeMaterial> materials;
    hkArray<uint16> materialIndices16;
};
class hkpCompressedSampledHeightFieldShape : public hkpSampledHeightFieldShape
{
public:
    virtual ~hkpCompressedSampledHeightFieldShape();
    hkArray<uint16> storage;
    bool triangleFlip;
    float offset;
    float scale;
};
class hkpPlaneShape : public hkpHeightFieldShape
{
public:
    virtual ~hkpPlaneShape();
    hkVector4 plane;
    hkVector4 aabbCenter;
    hkVector4 aabbHalfExtents;
};
class hkpStorageSampledHeightFieldShape : public hkpSampledHeightFieldShape
{
public:
    virtual ~hkpStorageSampledHeightFieldShape();
    hkArray<float> storage;
    bool triangleFlip;
};
class hkpTriSampledHeightFieldBvTreeShape : public hkpBvTreeShape
{
public:
    virtual ~hkpTriSampledHeightFieldBvTreeShape();
    struct hkpSingleShapeContainer childContainer;
    int32 childSize;
    bool wantAabbRejectionTest;
    uint8 padding;
};
class hkpTriSampledHeightFieldCollection : public hkpShapeCollection
{
public:
    virtual ~hkpTriSampledHeightFieldCollection();
    struct hkpSampledHeightFieldShapehkpSampledHeightFieldShape *heightfield;
    int32 childSize;
    float radius;
    hkArray<uint16> weldingInfo;
    hkVector4 triangleExtrusion;
};
class hkpFastMeshShape : public hkpMeshShape
{
public:
    virtual ~hkpFastMeshShape();
};
class hkpMeshShape : public hkpShapeCollection
{
public:
    virtual ~hkpMeshShape();
    hkVector4 scaling;
    int32 numBitsForSubpartIndex;
    hkArray<struct hkpMeshShapeSubpart> subparts;
    hkArray<uint16> weldingInfo;
    uint8 weldingType;
    float radius;
    int32 pad;
};
class hkpStorageMeshShape : public hkpMeshShape
{
public:
    virtual ~hkpStorageMeshShape();
    hkArray<hkpStorageMeshShapeSubpartStorage *> storage;
};
class hkpStorageMeshShapeSubpartStorage : public hkReferencedObject
{
public:
    virtual ~hkpStorageMeshShapeSubpartStorage();
    hkArray<float> vertices;
    hkArray<uint16> indices16;
    hkArray<uint32> indices32;
    hkArray<uint8> materialIndices;
    hkArray<uint32> materials;
    hkArray<uint16> materialIndices16;
};
class hkpSimpleMeshShape : public hkpShapeCollection
{
public:
    virtual ~hkpSimpleMeshShape();
    hkArray<hkVector4> vertices;
    hkArray<struct hkpSimpleMeshShapeTriangle> triangles;
    hkArray<uint8> materialIndices;
    float radius;
    uint8 weldingType;
};
class hkpAabbPhantom : public hkpPhantom
{
public:
    virtual ~hkpAabbPhantom();
    struct hkAabb aabb;
    hkArray<hkpCollidable *> overlappingCollidables;
    bool orderDirty;
};
class hkpBallAndSocketConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpBallAndSocketConstraintData();
    struct hkpBallAndSocketConstraintDataAtoms atoms;
};
class hkpBallSocketChainData : public hkpConstraintChainData
{
public:
    virtual ~hkpBallSocketChainData();
    struct hkpBridgeAtoms atoms;
    hkArray<struct hkpBallSocketChainDataConstraintInfo> infos;
    float tau;
    float damping;
    float cfm;
    float maxErrorDistance;
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
    struct hkpBridgeAtoms atoms;
    struct hkpConstraintDatahkpConstraintData *constraintData;
    uint16 childRuntimeSize;
    uint16 childNumSolverResults;
    float solverResultLimit;
    bool removeWhenBroken;
    bool revertBackVelocityOnBreak;
};
class hkpCachingShapePhantom : public hkpShapePhantom
{
public:
    virtual ~hkpCachingShapePhantom();
    hkArray<void> collisionDetails;
    bool orderDirty;
};
class hkpCallbackConstraintMotor : public hkpLimitedForceConstraintMotor
{
public:
    virtual ~hkpCallbackConstraintMotor();
    void *callbackFunc;
    uint32 callbackType;
    uint64 userData0;
    uint64 userData1;
    uint64 userData2;
};
class hkpCharacterMotion : public hkpMotion
{
public:
    virtual ~hkpCharacterMotion();
};
class hkpCogWheelConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpCogWheelConstraintData();
    struct hkpCogWheelConstraintDataAtoms atoms;
};
class hkpConstraintChainInstance : public hkpConstraintInstance
{
public:
    virtual ~hkpConstraintChainInstance();
    hkArray<hkpEntity *> chainedEntities;
    struct hkpConstraintChainInstanceActionhkpConstraintChainInstanceAction *action;
};
class hkpConstraintChainInstanceAction : public hkpAction
{
public:
    virtual ~hkpConstraintChainInstanceAction();
    struct hkpConstraintChainInstancehkpConstraintChainInstance *constraintInstance;
};
class hkpConstraintCollisionFilter : public hkpPairCollisionFilter
{
public:
    virtual ~hkpConstraintCollisionFilter();
};
class hkpConstraintInstance : public hkReferencedObject
{
public:
    virtual ~hkpConstraintInstance();
    void *owner;
    struct hkpConstraintDatahkpConstraintData *data;
    struct hkpModifierConstraintAtomhkpModifierConstraintAtom *constraintModifiers;
    struct hkpEntityhkpEntity *entities;
    uint8 priority;
    bool wantRuntime;
    uint8 destructionRemapInfo;
    struct hkpConstraintInstanceSmallArraySerializeOverrideType listeners;
    hkStringPtr name;
    uint64 userData;
    void *internal;
    uint32 uid;
};
class hkpDefaultWorldMemoryWatchDog : public hkWorldMemoryAvailableWatchDog
{
public:
    virtual ~hkpDefaultWorldMemoryWatchDog();
    int32 freeHeapMemoryRequested;
};
class hkpEntity : public hkpWorldObject
{
public:
    virtual ~hkpEntity();
    struct hkpMaterial material;
    void *limitContactImpulseUtilAndFlag;
    float damageMultiplier;
    void *breakableBody;
    uint32 solverData;
    uint16 storageIndex;
    uint16 contactPointCallbackDelay;
    struct hkpEntitySmallArraySerializeOverrideType constraintsMaster;
    hkArray<hkpConstraintInstance *> constraintsSlave;
    hkArray<uint8> constraintRuntime;
    void *simulationIsland;
    int8 autoRemoveLevel;
    uint8 numShapeKeysInContactPointProperties;
    uint8 responseModifierFlags;
    uint32 uid;
    struct hkpEntitySpuCollisionCallback spuCollisionCallback;
    struct hkpMaxSizeMotion motion;
    struct hkpEntitySmallArraySerializeOverrideType contactListeners;
    struct hkpEntitySmallArraySerializeOverrideType actions;
    struct hkLocalFramehkLocalFrame *localFrame;
    struct hkpEntityExtendedListenershkpEntityExtendedListeners *extendedListeners;
    uint32 npData;
};
class hkpFixedRigidMotion : public hkpKeyframedRigidMotion
{
public:
    virtual ~hkpFixedRigidMotion();
};
class hkpGenericConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpGenericConstraintData();
    struct hkpBridgeAtoms atoms;
    struct hkpGenericConstraintDataScheme scheme;
};
class hkpHingeConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpHingeConstraintData();
    struct hkpHingeConstraintDataAtoms atoms;
};
class hkpHingeLimitsData : public hkpConstraintData
{
public:
    virtual ~hkpHingeLimitsData();
    struct hkpHingeLimitsDataAtoms atoms;
};
class hkpKeyframedRigidMotion : public hkpMotion
{
public:
    virtual ~hkpKeyframedRigidMotion();
};
class hkpLimitedHingeConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpLimitedHingeConstraintData();
    struct hkpLimitedHingeConstraintDataAtoms atoms;
};
class hkpLinearParametricCurve : public hkpParametricCurve
{
public:
    virtual ~hkpLinearParametricCurve();
    float smoothingFactor;
    bool closedLoop;
    hkVector4 dirNotParallelToTangentAlongWholePath;
    hkArray<hkVector4> points;
    hkArray<float> distance;
};
class hkpMalleableConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpMalleableConstraintData();
    struct hkpConstraintDatahkpConstraintData *constraintData;
    struct hkpBridgeAtoms atoms;
    float strength;
};
class hkpMaxSizeMotion : public hkpKeyframedRigidMotion
{
public:
    virtual ~hkpMaxSizeMotion();
};
class hkpPairCollisionFilter : public hkpCollisionFilter
{
public:
    virtual ~hkpPairCollisionFilter();
    struct hkpPairCollisionFilterMapPairFilterKeyOverrideType disabledPairs;
    struct hkpCollisionFilterhkpCollisionFilter *childFilter;
};
class hkpPhysicsSystem : public hkReferencedObject
{
public:
    virtual ~hkpPhysicsSystem();
    hkArray<hkpRigidBody *> rigidBodies;
    hkArray<hkpConstraintInstance *> constraints;
    hkArray<hkpAction *> actions;
    hkArray<hkpPhantom *> phantoms;
    hkStringPtr name;
    uint64 userData;
    bool active;
};
class hkpPointToPathConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpPointToPathConstraintData();
    struct hkpBridgeAtoms atoms;
    struct hkpParametricCurvehkpParametricCurve *path;
    float maxFrictionForce;
    int8 angularConstrainedDOF;
    hkTransform transform_OS_KS;
};
class hkpPointToPlaneConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpPointToPlaneConstraintData();
    struct hkpPointToPlaneConstraintDataAtoms atoms;
};
class hkpPositionConstraintMotor : public hkpLimitedForceConstraintMotor
{
public:
    virtual ~hkpPositionConstraintMotor();
    float tau;
    float damping;
    float proportionalRecoveryVelocity;
    float constantRecoveryVelocity;
};
class hkpPrismaticConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpPrismaticConstraintData();
    struct hkpPrismaticConstraintDataAtoms atoms;
};
class hkpPulleyConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpPulleyConstraintData();
    struct hkpPulleyConstraintDataAtoms atoms;
};
class hkpRackAndPinionConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpRackAndPinionConstraintData();
    struct hkpRackAndPinionConstraintDataAtoms atoms;
};
class hkpRagdollConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpRagdollConstraintData();
    struct hkpRagdollConstraintDataAtoms atoms;
};
class hkpRagdollLimitsData : public hkpConstraintData
{
public:
    virtual ~hkpRagdollLimitsData();
    struct hkpRagdollLimitsDataAtoms atoms;
};
class hkpRigidBody : public hkpEntity
{
public:
    virtual ~hkpRigidBody();
};
class hkpRotationalConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpRotationalConstraintData();
    struct hkpRotationalConstraintDataAtoms atoms;
};
class hkpSimpleShapePhantom : public hkpShapePhantom
{
public:
    virtual ~hkpSimpleShapePhantom();
    hkArray<struct hkpSimpleShapePhantomCollisionDetail> collisionDetails;
    bool orderDirty;
};
class hkpSimulation : public hkReferencedObject
{
public:
    virtual ~hkpSimulation();
    uint32 determinismCheckFrameCounter;
    struct hkpWorldhkpWorld *world;
    uint8 lastProcessingStep;
    float currentTime;
    float currentPsiTime;
    float physicsDeltaTime;
    float simulateUntilTime;
    float frameMarkerPsiSnap;
    uint32 previousStepResult;
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
    float springConstant;
    float springDamping;
};
class hkpStiffSpringChainData : public hkpConstraintChainData
{
public:
    virtual ~hkpStiffSpringChainData();
    struct hkpBridgeAtoms atoms;
    hkArray<struct hkpStiffSpringChainDataConstraintInfo> infos;
    float tau;
    float damping;
    float cfm;
};
class hkpStiffSpringConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpStiffSpringConstraintData();
    struct hkpStiffSpringConstraintDataAtoms atoms;
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
    float tau;
    float velocityTarget;
    bool useVelocityTargetFromConstraintTargets;
};
class hkpWheelConstraintData : public hkpConstraintData
{
public:
    virtual ~hkpWheelConstraintData();
    struct hkpWheelConstraintDataAtoms atoms;
    hkVector4 initialAxleInB;
    hkVector4 initialSteeringAxisInB;
};

struct hkpConstraintInfo
{
    int32 maxSizeOfSchema;    // 00
    int32 sizeOfSchemas;      // 04
    int32 numSolverResults;   // 08
    int32 numSolverElemTemps; // 0C
};

class hkpConstraintOwner : public hkReferencedObject
{
public:
    hkpConstraintInfo constraintInfo; // 10
};

struct hkpAgentNnSector
{
    uint8 data[960];
};

struct hkpAgentNnTrack
{
public:
    // members
    uint16 bytesUsedInLastSector;                  // 00
    uint8 nnTrackType;                             // 02
    uint8 padding;                                 // 03
    uint32 pad04;                                  // 04
    hkInplaceArray<hkpAgentNnSector *, 1> sectors; // 08
};

class hkpSimulationIsland : public hkpConstraintOwner
{
public:
    hkpWorld *world;              // 20
    int32 numConstraints;         // 28
    uint16 storageIndex;          // 2C
    uint16 dirtyListIndex;        // 2E
    uint8 splitCheckFrameCounter; // 30

    bool splitCheckRequested : 2;         // 31 - 0
    bool isSparse : 2;                    // 31 - 2
    bool actionListCleanupNeeded : 2;     // 31 - 4
    bool allowIslandLocking : 2;          // 31 - 6
    bool isInActiveIslandsArray : 2;      // 32 - 0
    bool activeMark : 2;                  // 32 - 2
    bool tryToIncreaseIslandSizeMark : 2; // 32 - 4
    bool inIntegrateJob : 2;              // 32 - 6

    uint8 pad33;                                 // 33
    mutable hkMultiThreadCheck multiThreadCheck; // 34
    float timeSinceLastHighFrequencyCheck;       // 40
    float timeSinceLastLowFrequencyCheck;        // 44
    hkArray<hkpAction *> actions;                // 48
    float timeOfDeactivation;                    // 58
    uint32 pad5C;                                // 5C
    hkInplaceArray<hkpEntity *, 1> entities;     // 60
    hkpAgentNnTrack midphaseAgentTrack;          // 78
    hkpAgentNnTrack narrowphaseAgentTrack;       // 98
};

class hkpWorld : public hkReferencedObject
{
public:
    virtual ~hkpWorld();
    struct hkpSimulationhkpSimulation *simulation;
    hkVector4 gravity;
    void *fixedIsland;
    struct hkpRigidBodyhkpRigidBody *fixedRigidBody;
    hkArray<hkpSimulationIsland *> activeSimulationIslands;
    hkArray<hkpSimulationIsland *> inactiveSimulationIslands;
    hkArray<hkpSimulationIsland *> dirtySimulationIslands;
    void *maintenanceMgr;
    void *memoryWatchDog;
    bool assertOnRunningOutOfSolverMemory;
    void *broadPhase;
    void *kdTreeManager;
    bool autoUpdateTree;
    void *broadPhaseDispatcher;
    void *phantomBroadPhaseListener;
    void *entityEntityBroadPhaseListener;
    void *broadPhaseBorderListener;
    void *multithreadedSimulationJobData;
    void *collisionInput;
    void *collisionFilter;
    void *collisionDispatcher;
    void *convexListFilter;
    void *pendingOperations;
    int32 pendingOperationsCount;
    int32 pendingBodyOperationsCount;
    int32 criticalOperationsLockCount;
    int32 criticalOperationsLockCountForPhantoms;
    bool blockExecutingPendingOperations;
    bool criticalOperationsAllowed;
    void *pendingOperationQueues;
    int32 pendingOperationQueueCount;
    struct hkMultiThreadCheck multiThreadCheck;
    bool processActionsInSingleThread;
    bool allowIntegrationOfIslandsWithoutConstraintsInASeparateJob;
    uint32 minDesiredIslandSize;
    void *modifyConstraintCriticalSection;
    int32 isLocked;
    void *islandDirtyListCriticalSection;
    void *propertyMasterLock;
    bool wantSimulationIslands;
    bool useHybridBroadphase;
    float snapCollisionToConvexEdgeThreshold;
    float snapCollisionToConcaveEdgeThreshold;
    bool enableToiWeldRejection;
    bool wantDeactivation;
    bool shouldActivateOnRigidBodyTransformChange;
    float deactivationReferenceDistance;
    float toiCollisionResponseRotateNormal;
    int32 maxSectorsPerMidphaseCollideTask;
    int32 maxSectorsPerNarrowphaseCollideTask;
    bool processToisMultithreaded;
    int32 maxEntriesPerToiMidphaseCollideTask;
    int32 maxEntriesPerToiNarrowphaseCollideTask;
    int32 maxNumToiCollisionPairsSinglethreaded;
    int32 simulationType;
    float numToisTillAllowedPenetrationSimplifiedToi;
    float numToisTillAllowedPenetrationToi;
    float numToisTillAllowedPenetrationToiHigher;
    float numToisTillAllowedPenetrationToiForced;
    uint32 lastEntityUid;
    uint32 lastIslandUid;
    uint32 lastConstraintUid;
    hkArray<hkpPhantom *> phantoms;
    hkArray<void *> actionListeners;
    hkArray<void *> entityListeners;
    hkArray<void *> phantomListeners;
    hkArray<void *> constraintListeners;
    hkArray<void *> worldDeletionListeners;
    hkArray<void *> islandActivationListeners;
    hkArray<void *> worldPostSimulationListeners;
    hkArray<void *> worldPostIntegrateListeners;
    hkArray<void *> worldPostCollideListeners;
    hkArray<void *> islandPostIntegrateListeners;
    hkArray<void *> islandPostCollideListeners;
    hkArray<void *> contactListeners;
    hkArray<void *> contactImpulseLimitBreachedListeners;
    hkArray<void *> worldExtensions;
    void *violatedConstraintArray;
    void *broadPhaseBorder;
    void *destructionWorld;
    void *npWorld;
    hkVector4 broadPhaseExtents;
    int32 broadPhaseNumMarkers;
    int32 sizeOfToiEventQueue;
    int32 broadPhaseQuerySize;
    int32 broadPhaseUpdateSize;
    int8 contactPointGeneration;
};
class hkpWorldCinfo : public hkReferencedObject
{
public:
    virtual ~hkpWorldCinfo();
    hkVector4 gravity;
    int32 broadPhaseQuerySize;
    float contactRestingVelocity;
    int8 broadPhaseBorderBehaviour;
    bool mtPostponeAndSortBroadPhaseBorderCallbacks;
    struct hkAabb broadPhaseWorldAabb;
    bool useKdTree;
    bool useMultipleTree;
    int8 treeUpdateType;
    bool autoUpdateKdTree;
    float collisionTolerance;
    struct hkpCollisionFilterhkpCollisionFilter *collisionFilter;
    struct hkpConvexListFilterhkpConvexListFilter *convexListFilter;
    float expectedMaxLinearVelocity;
    int32 sizeOfToiEventQueue;
    float expectedMinPsiDeltaTime;
    struct hkWorldMemoryAvailableWatchDoghkWorldMemoryAvailableWatchDog *memoryWatchDog;
    int32 broadPhaseNumMarkers;
    int8 contactPointGeneration;
    bool allowToSkipConfirmedCallbacks;
    bool useHybridBroadphase;
    float solverTau;
    float solverDamp;
    int32 solverIterations;
    int32 solverMicrosteps;
    float maxConstraintViolation;
    bool forceCoherentConstraintOrderingInSolver;
    float snapCollisionToConvexEdgeThreshold;
    float snapCollisionToConcaveEdgeThreshold;
    bool enableToiWeldRejection;
    bool enableDeprecatedWelding;
    float iterativeLinearCastEarlyOutDistance;
    int32 iterativeLinearCastMaxIterations;
    uint8 deactivationNumInactiveFramesSelectFlag0;
    uint8 deactivationNumInactiveFramesSelectFlag1;
    uint8 deactivationIntegrateCounter;
    bool shouldActivateOnRigidBodyTransformChange;
    float deactivationReferenceDistance;
    float toiCollisionResponseRotateNormal;
    int32 maxSectorsPerMidphaseCollideTask;
    int32 maxSectorsPerNarrowphaseCollideTask;
    bool processToisMultithreaded;
    int32 maxEntriesPerToiMidphaseCollideTask;
    int32 maxEntriesPerToiNarrowphaseCollideTask;
    int32 maxNumToiCollisionPairsSinglethreaded;
    float numToisTillAllowedPenetrationSimplifiedToi;
    float numToisTillAllowedPenetrationToi;
    float numToisTillAllowedPenetrationToiHigher;
    float numToisTillAllowedPenetrationToiForced;
    bool enableDeactivation;
    int8 simulationType;
    bool enableSimulationIslands;
    uint32 minDesiredIslandSize;
    bool processActionsInSingleThread;
    bool allowIntegrationOfIslandsWithoutConstraintsInASeparateJob;
    float frameMarkerPsiSnap;
    bool fireCollisionCallbacks;
};
class hkpPoweredChainData : public hkpConstraintChainData
{
public:
    virtual ~hkpPoweredChainData();
    struct hkpBridgeAtoms atoms;
    hkArray<struct hkpPoweredChainDataConstraintInfo> infos;
    float tau;
    float damping;
    float cfmLinAdd;
    float cfmLinMul;
    float cfmAngAdd;
    float cfmAngMul;
    float maxErrorDistance;
};
class hkpConvexPieceStreamData : public hkReferencedObject
{
public:
    virtual ~hkpConvexPieceStreamData();
    hkArray<uint32> convexPieceStream;
    hkArray<uint32> convexPieceOffsets;
    hkArray<uint32> convexPieceSingleTriangles;
};
class hkpMoppCode : public hkReferencedObject
{
public:
    virtual ~hkpMoppCode();
    struct hkpMoppCodeCodeInfo info;
    hkArray<uint8> data;
    int8 buildType;
};
class hkpAngularDashpotAction : public hkpBinaryAction
{
public:
    virtual ~hkpAngularDashpotAction();
    hkQuaternion rotation;
    float strength;
    float damping;
};
class hkpBallGun : public hkpFirstPersonGun
{
public:
    virtual ~hkpBallGun();
    float bulletRadius;
    float bulletVelocity;
    float bulletMass;
    float damageMultiplier;
    int32 maxBulletsInWorld;
    hkVector4 bulletOffsetFromCenter;
    void *addedBodies;
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
    hkVector4 position;
    hkVector4 velocity;
    float dynamicFriction;
    float staticFriction;
    float keepContactTolerance;
    hkVector4 up;
    float extraUpStaticFriction;
    float extraDownStaticFriction;
    struct hkpShapePhantomhkpShapePhantom *shapePhantom;
    float keepDistance;
    float contactAngleSensitivity;
    uint32 userPlanes;
    float maxCharacterSpeedForSolver;
    float characterStrength;
    float characterMass;
    float maxSlope;
    float penetrationRecoverySpeed;
    int32 maxCastIterations;
    bool refreshManifoldInCheckSupport;
};
class hkpCharacterRigidBodyCinfo : public hkpCharacterControllerCinfo
{
public:
    virtual ~hkpCharacterRigidBodyCinfo();
    uint32 collisionFilterInfo;
    struct hkpShapehkpShape *shape;
    hkVector4 position;
    hkQuaternion rotation;
    float mass;
    float friction;
    float maxLinearVelocity;
    float allowedPenetrationDepth;
    hkVector4 up;
    float maxSlope;
    float maxForce;
    float unweldingHeightOffsetFactor;
    float maxSpeedForSimplexSolver;
    float supportDistance;
    float hardSupportDistance;
    int32 vdbColor;
};
class hkpConstrainedSystemFilter : public hkpCollisionFilter
{
public:
    virtual ~hkpConstrainedSystemFilter();
    struct hkpCollisionFilterhkpCollisionFilter *otherFilter;
};
class hkpDashpotAction : public hkpBinaryAction
{
public:
    virtual ~hkpDashpotAction();
    hkVector4 point;
    float strength;
    float damping;
    hkVector4 impulse;
};
class hkpDisableEntityCollisionFilter : public hkpCollisionFilter
{
public:
    virtual ~hkpDisableEntityCollisionFilter();
    hkArray<hkpEntity *> disabledEntities;
};
class hkpDisplayBindingData : public hkReferencedObject
{
public:
    virtual ~hkpDisplayBindingData();
    hkArray<hkpDisplayBindingDataRigidBody *> rigidBodyBindings;
    hkArray<hkpDisplayBindingDataPhysicsSystem *> physicsSystemBindings;
};
class hkpDisplayBindingDataPhysicsSystem : public hkReferencedObject
{
public:
    virtual ~hkpDisplayBindingDataPhysicsSystem();
    hkArray<hkpDisplayBindingDataRigidBody *> bindings;
    struct hkpPhysicsSystemhkpPhysicsSystem *system;
};
class hkpDisplayBindingDataRigidBody : public hkReferencedObject
{
public:
    virtual ~hkpDisplayBindingDataRigidBody();
    struct hkpRigidBodyhkpRigidBody *rigidBody;
    struct hkReferencedObjecthkReferencedObject *displayObjectPtr;
    hkMatrix4 rigidBodyFromDisplayObjectTransform;
};
class hkpFirstPersonGun : public hkReferencedObject
{
public:
    virtual ~hkpFirstPersonGun();
    uint8 type;
    hkStringPtr name;
    uint8 keyboardKey;
    hkArray<void *> listeners;
};
class hkpGravityGun : public hkpFirstPersonGun
{
public:
    virtual ~hkpGravityGun();
    hkArray<void *> grabbedBodies;
    int32 maxNumObjectsPicked;
    float maxMassOfObjectPicked;
    float maxDistOfObjectPicked;
    float impulseAppliedWhenObjectNotPicked;
    float throwVelocity;
    hkVector4 capturedObjectPosition;
    hkVector4 capturedObjectsOffset;
};
class hkpGroupCollisionFilter : public hkpCollisionFilter
{
public:
    virtual ~hkpGroupCollisionFilter();
    bool noGroupCollisionEnabled;
    uint32 collisionGroups;
};
class hkpMotorAction : public hkpUnaryAction
{
public:
    virtual ~hkpMotorAction();
    hkVector4 axis;
    float spinRate;
    float gain;
    bool active;
};
class hkpMountedBallGun : public hkpBallGun
{
public:
    virtual ~hkpMountedBallGun();
    hkVector4 position;
};
class hkpMouseSpringAction : public hkpUnaryAction
{
public:
    virtual ~hkpMouseSpringAction();
    hkVector4 positionInRbLocal;
    hkVector4 mousePositionInWorld;
    float springDamping;
    float springElasticity;
    float maxRelativeForce;
    float objectDamping;
    uint32 shapeKey;
    hkArray<void *> applyCallbacks;
};
class hkpPhysicsData : public hkReferencedObject
{
public:
    virtual ~hkpPhysicsData();
    struct hkpWorldCinfohkpWorldCinfo *worldCinfo;
    hkArray<hkpPhysicsSystem *> systems;
};
class hkpPhysicsSystemWithContacts : public hkpPhysicsSystem
{
public:
    virtual ~hkpPhysicsSystemWithContacts();
    hkArray<hkpSerializedAgentNnEntry *> contacts;
};
class hkpPoweredChainMapper : public hkReferencedObject
{
public:
    virtual ~hkpPoweredChainMapper();
    hkArray<struct hkpPoweredChainMapperLinkInfo> links;
    hkArray<struct hkpPoweredChainMapperTarget> targets;
    hkArray<hkpConstraintChainInstance *> chains;
};
class hkpProjectileGun : public hkpFirstPersonGun
{
public:
    virtual ~hkpProjectileGun();
    int32 maxProjectiles;
    float reloadTime;
    float reload;
    hkArray<void *> projectiles;
    void *world;
    void *destructionWorld;
};
class hkpReorientAction : public hkpUnaryAction
{
public:
    virtual ~hkpReorientAction();
    hkVector4 rotationAxis;
    hkVector4 upAxis;
    float strength;
    float damping;
};
class hkpSerializedAgentNnEntry : public hkReferencedObject
{
public:
    virtual ~hkpSerializedAgentNnEntry();
    struct hkpEntityhkpEntity *bodyA;
    struct hkpEntityhkpEntity *bodyB;
    uint64 bodyAId;
    uint64 bodyBId;
    bool useEntityIds;
    int8 agentType;
    struct hkpSimpleContactConstraintAtom atom;
    hkArray<uint8> propertiesStream;
    hkArray<struct hkContactPoint> contactPoints;
    hkArray<uint8> cpIdMgr;
    uint8 nnEntryData;
    struct hkpSerializedTrack1nInfo trackInfo;
    uint8 endianCheckBuffer;
    uint32 version;
};
class hkpSerializedDisplayMarker : public hkReferencedObject
{
public:
    virtual ~hkpSerializedDisplayMarker();
    hkTransform transform;
};
class hkpSerializedDisplayMarkerList : public hkReferencedObject
{
public:
    virtual ~hkpSerializedDisplayMarkerList();
    hkArray<hkpSerializedDisplayMarker *> markers;
};
class hkpSerializedDisplayRbTransforms : public hkReferencedObject
{
public:
    virtual ~hkpSerializedDisplayRbTransforms();
    hkArray<struct hkpSerializedDisplayRbTransformsDisplayTransformPair> transforms;
};
class hkpSpringAction : public hkpBinaryAction
{
public:
    virtual ~hkpSpringAction();
    hkVector4 lastForce;
    hkVector4 positionAinA;
    hkVector4 positionBinB;
    float restLength;
    float strength;
    float damping;
    bool onCompression;
    bool onExtension;
};
class hkpTriggerVolume : public hkReferencedObject
{
public:
    virtual ~hkpTriggerVolume();
    hkArray<hkpRigidBody *> overlappingBodies;
    hkArray<struct hkpTriggerVolumeEventInfo> eventQueue;
    struct hkpRigidBodyhkpRigidBody *triggerBody;
    uint32 sequenceNumber;
};
class hkaAnimationBinding : public hkReferencedObject
{
public:
    virtual ~hkaAnimationBinding();
    hkStringPtr originalSkeletonName;
    struct hkaAnimationhkaAnimation *animation;
    hkArray<int16> transformTrackToBoneIndices;
    hkArray<int16> floatTrackToFloatSlotIndices;
    int8 blendHint;
};
class hkaAnimationContainer : public hkReferencedObject
{
public:
    virtual ~hkaAnimationContainer();
    hkArray<hkaSkeleton *> skeletons;
    hkArray<hkaAnimation *> animations;
    hkArray<hkaAnimationBinding *> bindings;
    hkArray<hkaBoneAttachment *> attachments;
    hkArray<hkaMeshBinding *> skins;
};
class hkaAnimationPreviewColorContainer : public hkReferencedObject
{
public:
    virtual ~hkaAnimationPreviewColorContainer();
    hkArray<uint32> previewColor;
};
class hkaBoneAttachment : public hkReferencedObject
{
public:
    virtual ~hkaBoneAttachment();
    hkStringPtr originalSkeletonName;
    hkMatrix4 boneFromAttachment;
    struct hkReferencedObjecthkReferencedObject *attachment;
    hkStringPtr name;
    int16 boneIndex;
};
class hkaDefaultAnimatedReferenceFrame : public hkaAnimatedReferenceFrame
{
public:
    virtual ~hkaDefaultAnimatedReferenceFrame();
    hkVector4 up;
    hkVector4 forward;
    float duration;
    hkArray<hkVector4> referenceFrameSamples;
};
class hkaDeltaCompressedAnimation : public hkaAnimation
{
public:
    virtual ~hkaDeltaCompressedAnimation();
    int32 numberOfPoses;
    int32 blockSize;
    struct hkaDeltaCompressedAnimationQuantizationFormat qFormat;
    uint32 quantizedDataIdx;
    uint32 quantizedDataSize;
    uint32 staticMaskIdx;
    uint32 staticMaskSize;
    uint32 staticDOFsIdx;
    uint32 staticDOFsSize;
    uint32 numStaticTransformDOFs;
    uint32 numDynamicTransformDOFs;
    uint32 totalBlockSize;
    uint32 lastBlockSize;
    hkArray<uint8> dataBuffer;
};
class hkaFootstepAnalysisInfo : public hkReferencedObject
{
public:
    virtual ~hkaFootstepAnalysisInfo();
    hkArray<char> name;
    hkArray<char> nameStrike;
    hkArray<char> nameLift;
    hkArray<char> nameLock;
    hkArray<char> nameUnlock;
    hkArray<float> minPos;
    hkArray<float> maxPos;
    hkArray<float> minVel;
    hkArray<float> maxVel;
    hkArray<float> allBonesDown;
    hkArray<float> anyBonesDown;
    float posTol;
    float velTol;
    float duration;
};
class hkaFootstepAnalysisInfoContainer : public hkReferencedObject
{
public:
    virtual ~hkaFootstepAnalysisInfoContainer();
    hkArray<hkaFootstepAnalysisInfo *> previewInfo;
};
class hkaInterleavedUncompressedAnimation : public hkaAnimation
{
public:
    virtual ~hkaInterleavedUncompressedAnimation();
    hkArray<hkQsTransform> transforms;
    hkArray<float> floats;
};
class hkaMeshBinding : public hkReferencedObject
{
public:
    virtual ~hkaMeshBinding();
    struct hkxMeshhkxMesh *mesh;
    hkStringPtr originalSkeletonName;
    struct hkaSkeletonhkaSkeleton *skeleton;
    hkArray<struct hkaMeshBindingMapping> mappings;
    hkArray<hkTransform> boneFromSkinMeshTransforms;
};
class hkaQuantizedAnimation : public hkaAnimation
{
public:
    virtual ~hkaQuantizedAnimation();
    hkArray<uint8> data;
    uint32 endian;
    void *skeleton;
};
class hkaSkeleton : public hkReferencedObject
{
public:
    virtual ~hkaSkeleton();
    hkStringPtr name;
    hkArray<int16> parentIndices;
    hkArray<struct hkaBone> bones;
    hkArray<hkQsTransform> referencePose;
    hkArray<float> referenceFloats;
    hkArray<hkStringPtr> floatSlots;
    hkArray<struct hkaSkeletonLocalFrameOnBone> localFrames;
};
class hkaSkeletonMapper : public hkReferencedObject
{
public:
    virtual ~hkaSkeletonMapper();
    struct hkaSkeletonMapperData mapping;
};
class hkaSplineCompressedAnimation : public hkaAnimation
{
public:
    virtual ~hkaSplineCompressedAnimation();
    int32 numFrames;
    int32 numBlocks;
    int32 maxFramesPerBlock;
    int32 maskAndQuantizationSize;
    float blockDuration;
    float blockInverseDuration;
    float frameDuration;
    hkArray<uint32> blockOffsets;
    hkArray<uint32> floatBlockOffsets;
    hkArray<uint32> transformOffsets;
    hkArray<uint32> floatOffsets;
    hkArray<uint8> data;
    int32 endian;
};
class hkaWaveletCompressedAnimation : public hkaAnimation
{
public:
    virtual ~hkaWaveletCompressedAnimation();
    int32 numberOfPoses;
    int32 blockSize;
    struct hkaWaveletCompressedAnimationQuantizationFormat qFormat;
    uint32 staticMaskIdx;
    uint32 staticDOFsIdx;
    uint32 numStaticTransformDOFs;
    uint32 numDynamicTransformDOFs;
    uint32 blockIndexIdx;
    uint32 blockIndexSize;
    uint32 quantizedDataIdx;
    uint32 quantizedDataSize;
    hkArray<uint8> dataBuffer;
};
class hkaRagdollInstance : public hkReferencedObject
{
public:
    virtual ~hkaRagdollInstance();
    hkArray<hkpRigidBody *> rigidBodies;
    hkArray<hkpConstraintInstance *> constraints;
    hkArray<int32> boneToRigidBodyMap;
    struct hkaSkeletonhkaSkeleton *skeleton;
};
class hkbAttachmentModifier : public hkbModifier
{
public:
    virtual ~hkbAttachmentModifier();
    struct hkbEventProperty sendToAttacherOnAttach;
    struct hkbEventProperty sendToAttacheeOnAttach;
    struct hkbEventProperty sendToAttacherOnDetach;
    struct hkbEventProperty sendToAttacheeOnDetach;
    struct hkbAttachmentSetuphkbAttachmentSetup *attachmentSetup;
    struct hkbHandlehkbHandle *attacherHandle;
    struct hkbHandlehkbHandle *attacheeHandle;
    int32 attacheeLayer;
    void *attacheeRB;
    uint8 oldMotionType;
    int32 oldFilterInfo;
    void *attachment;
};
class hkbAttachmentSetup : public hkReferencedObject
{
public:
    virtual ~hkbAttachmentSetup();
    float blendInTime;
    float moveAttacherFraction;
    float gain;
    float extrapolationTimeStep;
    float fixUpGain;
    float maxLinearDistance;
    float maxAngularDistance;
    int8 attachmentType;
};
class hkbAttributeModifier : public hkbModifier
{
public:
    virtual ~hkbAttributeModifier();
    hkArray<struct hkbAttributeModifierAssignment> assignments;
};
class hkbBehaviorGraph : public hkbGenerator
{
public:
    virtual ~hkbBehaviorGraph();
    int8 variableMode;
    hkArray<void> uniqueIdPool;
    void *idToStateMachineTemplateMap;
    hkArray<void> mirroredExternalIdMap;
    void *pseudoRandomGenerator;
    struct hkbGeneratorhkbGenerator *rootGenerator;
    struct hkbBehaviorGraphDatahkbBehaviorGraphData *data;
    void *rootGeneratorClone;
    void *activeNodes;
    void *activeNodeTemplateToIndexMap;
    void *activeNodesChildrenIndices;
    void *globalTransitionData;
    void *eventIdMap;
    void *attributeIdMap;
    void *variableIdMap;
    void *characterPropertyIdMap;
    void *variableValueSet;
    void *nodeTemplateToCloneMap;
    void *nodeCloneToTemplateMap;
    void *stateListenerTemplateToCloneMap;
    void *nodePartitionInfo;
    int32 numIntermediateOutputs;
    hkArray<void *> jobs;
    hkArray<void *> allPartitionMemory;
    int16 numStaticNodes;
    int16 nextUniqueId;
    bool isActive;
    bool isLinked;
    bool updateActiveNodes;
    bool stateOrTransitionChanged;
};
class hkbBehaviorGraphData : public hkReferencedObject
{
public:
    virtual ~hkbBehaviorGraphData();
    hkArray<float> attributeDefaults;
    hkArray<struct hkbVariableInfo> variableInfos;
    hkArray<struct hkbVariableInfo> characterPropertyInfos;
    hkArray<struct hkbEventInfo> eventInfos;
    hkArray<struct hkbVariableValue> wordMinVariableValues;
    hkArray<struct hkbVariableValue> wordMaxVariableValues;
    struct hkbVariableValueSethkbVariableValueSet *variableInitialValues;
    struct hkbBehaviorGraphStringDatahkbBehaviorGraphStringData *stringData;
};
class hkbBehaviorGraphInternalState : public hkReferencedObject
{
public:
    virtual ~hkbBehaviorGraphInternalState();
    hkArray<hkbNodeInternalStateInfo *> nodeInternalStateInfos;
    struct hkbVariableValueSethkbVariableValueSet *variableValueSet;
};
class hkbBehaviorGraphStringData : public hkReferencedObject
{
public:
    virtual ~hkbBehaviorGraphStringData();
    hkArray<hkStringPtr> eventNames;
    hkArray<hkStringPtr> attributeNames;
    hkArray<hkStringPtr> variableNames;
    hkArray<hkStringPtr> characterPropertyNames;
};
class hkbBehaviorReferenceGenerator : public hkbGenerator
{
public:
    virtual ~hkbBehaviorReferenceGenerator();
    hkStringPtr behaviorName;
    void *behavior;
};
class hkbBindable : public hkReferencedObject
{
public:
    virtual ~hkbBindable();
    struct hkbVariableBindingSethkbVariableBindingSet *variableBindingSet;
    hkArray<void> cachedBindables;
    bool areBindablesCached;
};
class hkbBlenderGenerator : public hkbGenerator
{
public:
    virtual ~hkbBlenderGenerator();
    float referencePoseWeightThreshold;
    float blendParameter;
    float minCyclicBlendParameter;
    float maxCyclicBlendParameter;
    int16 indexOfSyncMasterChild;
    int16 flags;
    bool subtractLastChild;
    hkArray<hkbBlenderGeneratorChild *> children;
    hkArray<void> childrenInternalStates;
    hkArray<void> sortedChildren;
    float endIntervalWeight;
    int32 numActiveChildren;
    int16 beginIntervalIndex;
    int16 endIntervalIndex;
    bool initSync;
    bool doSubtractiveBlend;
};
class hkbBlenderGeneratorChild : public hkbBindable
{
public:
    virtual ~hkbBlenderGeneratorChild();
    struct hkbGeneratorhkbGenerator *generator;
    struct hkbBoneWeightArrayhkbBoneWeightArray *boneWeights;
    float weight;
    float worldFromModelWeight;
};
class hkbBlenderGeneratorInternalState : public hkReferencedObject
{
public:
    virtual ~hkbBlenderGeneratorInternalState();
    hkArray<struct hkbBlenderGeneratorChildInternalState> childrenInternalStates;
    hkArray<int16> sortedChildren;
    float endIntervalWeight;
    int32 numActiveChildren;
    int16 beginIntervalIndex;
    int16 endIntervalIndex;
    bool initSync;
    bool doSubtractiveBlend;
};
class hkbBlendingTransitionEffect : public hkbTransitionEffect
{
public:
    virtual ~hkbBlendingTransitionEffect();
    float duration;
    float toGeneratorStartTimeFraction;
    hkFlags flags;
    int8 endMode;
    int8 blendCurve;
    void *fromGenerator;
    void *toGenerator;
    hkArray<void> characterPoseAtBeginningOfTransition;
    float timeRemaining;
    float timeInTransition;
    bool applySelfTransition;
    bool initializeCharacterPose;
};
class hkbBlendingTransitionEffectInternalState : public hkReferencedObject
{
public:
    virtual ~hkbBlendingTransitionEffectInternalState();
    hkArray<hkQsTransform> characterPoseAtBeginningOfTransition;
    float timeRemaining;
    float timeInTransition;
    bool applySelfTransition;
    bool initializeCharacterPose;
};
class hkbBoneIndexArray : public hkbBindable
{
public:
    virtual ~hkbBoneIndexArray();
    hkArray<int16> boneIndices;
};
class hkbBoneWeightArray : public hkbBindable
{
public:
    virtual ~hkbBoneWeightArray();
    hkArray<float> boneWeights;
};
class hkbBoolVariableSequencedData : public hkbSequencedData
{
public:
    virtual ~hkbBoolVariableSequencedData();
    hkArray<struct hkbBoolVariableSequencedDataSample> samples;
    int32 variableIndex;
};
class hkbCharacter : public hkReferencedObject
{
public:
    virtual ~hkbCharacter();
    hkArray<hkbCharacter *> nearbyCharacters;
    int16 currentLod;
    int16 numTracksInLod;
    hkStringPtr name;
    void *ragdollDriver;
    void *characterControllerDriver;
    void *footIkDriver;
    void *handIkDriver;
    struct hkbCharacterSetuphkbCharacterSetup *setup;
    struct hkbBehaviorGraphhkbBehaviorGraph *behaviorGraph;
    struct hkbProjectDatahkbProjectData *projectData;
    void *animationBindingSet;
    void *raycastInterface;
    void *world;
    void *eventQueue;
    void *worldFromModel;
    void **poseLocal;
    bool deleteWorldFromModel;
    bool deletePoseLocal;
};
class hkbCharacterControllerModifier : public hkbModifier
{
public:
    virtual ~hkbCharacterControllerModifier();
    struct hkbCharacterControllerControlData controlData;
    hkVector4 initialVelocity;
    int8 initialVelocityCoordinates;
    int8 motionMode;
    bool forceDownwardMomentum;
    bool applyGravity;
    bool setInitialVelocity;
    bool isTouchingGround;
    hkVector4 gravity;
    float timestep;
    bool isInitialVelocityAdded;
};
class hkbCharacterControllerModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbCharacterControllerModifierInternalState();
    hkVector4 gravity;
    float timestep;
    bool isInitialVelocityAdded;
    bool isTouchingGround;
};
class hkbCharacterData : public hkReferencedObject
{
public:
    virtual ~hkbCharacterData();
    struct hkbCharacterDataCharacterControllerInfo characterControllerInfo;
    hkVector4 modelUpMS;
    hkVector4 modelForwardMS;
    hkVector4 modelRightMS;
    hkArray<struct hkbVariableInfo> characterPropertyInfos;
    hkArray<int32> numBonesPerLod;
    struct hkbVariableValueSethkbVariableValueSet *characterPropertyValues;
    struct hkbFootIkDriverInfohkbFootIkDriverInfo *footIkDriverInfo;
    struct hkbHandIkDriverInfohkbHandIkDriverInfo *handIkDriverInfo;
    struct hkbCharacterStringDatahkbCharacterStringData *stringData;
    struct hkbMirroredSkeletonInfohkbMirroredSkeletonInfo *mirroredSkeletonInfo;
    float scale;
    int16 numHands;
    int16 numFloatSlots;
};
class hkbCharacterSetup : public hkReferencedObject
{
public:
    virtual ~hkbCharacterSetup();
    hkArray<hkaSkeletonMapper *> retargetingSkeletonMappers;
    struct hkaSkeletonhkaSkeleton *animationSkeleton;
    struct hkaSkeletonMapperhkaSkeletonMapper *ragdollToAnimationSkeletonMapper;
    struct hkaSkeletonMapperhkaSkeletonMapper *animationToRagdollSkeletonMapper;
    void *animationBindingSet;
    struct hkbCharacterDatahkbCharacterData *data;
    void *mirroredSkeleton;
    void *characterPropertyIdMap;
};
class hkbCharacterStringData : public hkReferencedObject
{
public:
    virtual ~hkbCharacterStringData();
    hkArray<hkStringPtr> deformableSkinNames;
    hkArray<hkStringPtr> rigidSkinNames;
    hkArray<hkStringPtr> animationNames;
    hkArray<hkStringPtr> animationFilenames;
    hkArray<hkStringPtr> characterPropertyNames;
    hkArray<hkStringPtr> retargetingSkeletonMapperFilenames;
    hkArray<hkStringPtr> lodNames;
    hkArray<hkStringPtr> mirroredSyncPointSubstringsA;
    hkArray<hkStringPtr> mirroredSyncPointSubstringsB;
    hkStringPtr name;
    hkStringPtr rigName;
    hkStringPtr ragdollName;
    hkStringPtr behaviorFilename;
};
class hkbClipGenerator : public hkbGenerator
{
public:
    virtual ~hkbClipGenerator();
    hkStringPtr animationName;
    struct hkbClipTriggerArrayhkbClipTriggerArray *triggers;
    float cropStartAmountLocalTime;
    float cropEndAmountLocalTime;
    float startTime;
    float playbackSpeed;
    float enforcedDuration;
    float userControlledTimeFraction;
    int16 animationBindingIndex;
    int8 mode;
    int8 flags;
    hkArray<void> animDatas;
    void *animationControl;
    void *originalTriggers;
    void *mapperData;
    void *binding;
    void *mirroredAnimation;
    hkQsTransform extractedMotion;
    hkArray<void> echos;
    float localTime;
    float time;
    float previousUserControlledTimeFraction;
    int32 bufferSize;
    int32 echoBufferSize;
    bool atEnd;
    bool ignoreStartTime;
    bool pingPongBackward;
};
class hkbClipGeneratorInternalState : public hkReferencedObject
{
public:
    virtual ~hkbClipGeneratorInternalState();
    hkQsTransform extractedMotion;
    hkArray<struct hkbClipGeneratorEcho> echos;
    float localTime;
    float time;
    float previousUserControlledTimeFraction;
    int32 bufferSize;
    int32 echoBufferSize;
    bool atEnd;
    bool ignoreStartTime;
    bool pingPongBackward;
};
class hkbClipTriggerArray : public hkReferencedObject
{
public:
    virtual ~hkbClipTriggerArray();
    hkArray<struct hkbClipTrigger> triggers;
};
class hkbCombineTransformsModifier : public hkbModifier
{
public:
    virtual ~hkbCombineTransformsModifier();
    hkVector4 translationOut;
    hkQuaternion rotationOut;
    hkVector4 leftTranslation;
    hkQuaternion leftRotation;
    hkVector4 rightTranslation;
    hkQuaternion rightRotation;
    bool invertLeftTransform;
    bool invertRightTransform;
    bool invertResult;
};
class hkbCombineTransformsModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbCombineTransformsModifierInternalState();
    hkVector4 translationOut;
    hkQuaternion rotationOut;
};
class hkbCompiledExpressionSet : public hkReferencedObject
{
public:
    virtual ~hkbCompiledExpressionSet();
    hkArray<struct hkbCompiledExpressionSetToken> rpn;
    hkArray<int32> expressionToRpnIndex;
    int8 numExpressions;
};
class hkbComputeDirectionModifier : public hkbModifier
{
public:
    virtual ~hkbComputeDirectionModifier();
    hkVector4 pointIn;
    hkVector4 pointOut;
    float groundAngleOut;
    float upAngleOut;
    float verticalOffset;
    bool reverseGroundAngle;
    bool reverseUpAngle;
    bool projectPoint;
    bool normalizePoint;
    bool computeOnlyOnce;
    bool computedOutput;
};
class hkbComputeDirectionModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbComputeDirectionModifierInternalState();
    hkVector4 pointOut;
    float groundAngleOut;
    float upAngleOut;
    bool computedOutput;
};
class hkbComputeRotationFromAxisAngleModifier : public hkbModifier
{
public:
    virtual ~hkbComputeRotationFromAxisAngleModifier();
    hkQuaternion rotationOut;
    hkVector4 axis;
    float angleDegrees;
};
class hkbComputeRotationFromAxisAngleModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbComputeRotationFromAxisAngleModifierInternalState();
    hkQuaternion rotationOut;
};
class hkbComputeRotationToTargetModifier : public hkbModifier
{
public:
    virtual ~hkbComputeRotationToTargetModifier();
    hkQuaternion rotationOut;
    hkVector4 targetPosition;
    hkVector4 currentPosition;
    hkQuaternion currentRotation;
    hkVector4 localAxisOfRotation;
    hkVector4 localFacingDirection;
    bool resultIsDelta;
};
class hkbComputeRotationToTargetModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbComputeRotationToTargetModifierInternalState();
    hkQuaternion rotationOut;
};
class hkbDampingModifier : public hkbModifier
{
public:
    virtual ~hkbDampingModifier();
    float kP;
    float kI;
    float kD;
    bool enableScalarDamping;
    bool enableVectorDamping;
    float rawValue;
    float dampedValue;
    hkVector4 rawVector;
    hkVector4 dampedVector;
    hkVector4 vecErrorSum;
    hkVector4 vecPreviousError;
    float errorSum;
    float previousError;
};
class hkbDampingModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbDampingModifierInternalState();
    hkVector4 dampedVector;
    hkVector4 vecErrorSum;
    hkVector4 vecPreviousError;
    float dampedValue;
    float errorSum;
    float previousError;
};
class hkbDelayedModifier : public hkbModifierWrapper
{
public:
    virtual ~hkbDelayedModifier();
    float delaySeconds;
    float durationSeconds;
    float secondsElapsed;
    bool isActive;
};
class hkbDelayedModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbDelayedModifierInternalState();
    float secondsElapsed;
    bool isActive;
};
class hkbDetectCloseToGroundModifier : public hkbModifier
{
public:
    virtual ~hkbDetectCloseToGroundModifier();
    struct hkbEventProperty closeToGroundEvent;
    float closeToGroundHeight;
    float raycastDistanceDown;
    uint32 collisionFilterInfo;
    int16 boneIndex;
    int16 animBoneIndex;
    bool isCloseToGround;
};
class hkbDetectCloseToGroundModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbDetectCloseToGroundModifierInternalState();
    bool isCloseToGround;
};
class hkbEvaluateExpressionModifier : public hkbModifier
{
public:
    virtual ~hkbEvaluateExpressionModifier();
    struct hkbExpressionDataArrayhkbExpressionDataArray *expressions;
    void *compiledExpressionSet;
    hkArray<void> internalExpressionsData;
};
class hkbEvaluateExpressionModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbEvaluateExpressionModifierInternalState();
    hkArray<struct hkbEvaluateExpressionModifierInternalExpressionData> internalExpressionsData;
};
class hkbEvaluateHandleModifier : public hkbModifier
{
public:
    virtual ~hkbEvaluateHandleModifier();
    struct hkbHandlehkbHandle *handle;
    hkVector4 handlePositionOut;
    hkQuaternion handleRotationOut;
    bool isValidOut;
    float extrapolationTimeStep;
    float handleChangeSpeed;
    int8 handleChangeMode;
    struct hkbHandle oldHandle;
    hkVector4 oldHandlePosition;
    hkQuaternion oldHandleRotation;
    float timeSinceLastModify;
    bool smoothlyChangingHandles;
};
class hkbEventDrivenModifier : public hkbModifierWrapper
{
public:
    virtual ~hkbEventDrivenModifier();
    int32 activateEventId;
    int32 deactivateEventId;
    bool activeByDefault;
    bool isActive;
};
class hkbEventDrivenModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbEventDrivenModifierInternalState();
    bool isActive;
};
class hkbEventPayload : public hkReferencedObject
{
public:
    virtual ~hkbEventPayload();
};
class hkbEventPayloadList : public hkbEventPayload
{
public:
    virtual ~hkbEventPayloadList();
    hkArray<hkbEventPayload *> payloads;
};
class hkbEventRangeDataArray : public hkReferencedObject
{
public:
    virtual ~hkbEventRangeDataArray();
    hkArray<struct hkbEventRangeData> eventData;
};
class hkbEventSequencedData : public hkbSequencedData
{
public:
    virtual ~hkbEventSequencedData();
    hkArray<struct hkbEventSequencedDataSequencedEvent> events;
};
class hkbEventsFromRangeModifier : public hkbModifier
{
public:
    virtual ~hkbEventsFromRangeModifier();
    float inputValue;
    float lowerBound;
    struct hkbEventRangeDataArrayhkbEventRangeDataArray *eventRanges;
    hkArray<void> wasActiveInPreviousFrame;
};
class hkbEventsFromRangeModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbEventsFromRangeModifierInternalState();
    hkArray<bool> wasActiveInPreviousFrame;
};
class hkbExpressionCondition : public hkbCondition
{
public:
    virtual ~hkbExpressionCondition();
    hkStringPtr expression;
    void *compiledExpressionSet;
};
class hkbExpressionDataArray : public hkReferencedObject
{
public:
    virtual ~hkbExpressionDataArray();
    hkArray<struct hkbExpressionData> expressionsData;
};
class hkbExtractRagdollPoseModifier : public hkbModifier
{
public:
    virtual ~hkbExtractRagdollPoseModifier();
    int16 poseMatchingBone0;
    int16 poseMatchingBone1;
    int16 poseMatchingBone2;
    bool enableComputeWorldFromModel;
};
class hkbExtrapolatingTransitionEffect : public hkbBlendingTransitionEffect
{
public:
    virtual ~hkbExtrapolatingTransitionEffect();
    struct hkbGeneratorSyncInfo fromGeneratorSyncInfo;
    hkQsTransform worldFromModel;
    hkQsTransform motion;
    hkArray<void> pose;
    hkArray<void> additivePose;
    float toGeneratorDuration;
    bool isFromGeneratorActive;
    bool gotPose;
    bool gotAdditivePose;
};
class hkbExtrapolatingTransitionEffectInternalState : public hkReferencedObject
{
public:
    virtual ~hkbExtrapolatingTransitionEffectInternalState();
    struct hkbGeneratorSyncInfo fromGeneratorSyncInfo;
    hkQsTransform worldFromModel;
    hkQsTransform motion;
    hkArray<hkQsTransform> pose;
    hkArray<hkQsTransform> additivePose;
    float toGeneratorDuration;
    bool isFromGeneratorActive;
    bool gotPose;
    bool gotAdditivePose;
};
class hkbFootIkControlsModifier : public hkbModifier
{
public:
    virtual ~hkbFootIkControlsModifier();
    struct hkbFootIkControlData controlData;
    hkArray<struct hkbFootIkControlsModifierLeg> legs;
    hkVector4 errorOutTranslation;
    hkQuaternion alignWithGroundRotation;
};
class hkbFootIkDriverInfo : public hkReferencedObject
{
public:
    virtual ~hkbFootIkDriverInfo();
    hkArray<struct hkbFootIkDriverInfoLeg> legs;
    float raycastDistanceUp;
    float raycastDistanceDown;
    float originalGroundHeightMS;
    float verticalOffset;
    uint32 collisionFilterInfo;
    float forwardAlignFraction;
    float sidewaysAlignFraction;
    float sidewaysSampleWidth;
    bool lockFeetWhenPlanted;
    bool useCharacterUpVector;
    bool isQuadrupedNarrow;
};
class hkbFootIkModifier : public hkbModifier
{
public:
    virtual ~hkbFootIkModifier();
    struct hkbFootIkGains gains;
    hkArray<struct hkbFootIkModifierLeg> legs;
    float raycastDistanceUp;
    float raycastDistanceDown;
    float originalGroundHeightMS;
    float errorOut;
    hkVector4 errorOutTranslation;
    hkQuaternion alignWithGroundRotation;
    float verticalOffset;
    uint32 collisionFilterInfo;
    float forwardAlignFraction;
    float sidewaysAlignFraction;
    float sidewaysSampleWidth;
    bool useTrackData;
    bool lockFeetWhenPlanted;
    bool useCharacterUpVector;
    int8 alignMode;
    hkArray<struct hkbFootIkModifierInternalLegData> internalLegData;
    float prevIsFootIkEnabled;
    bool isSetUp;
    bool isGroundPositionValid;
    float timeStep;
};
class hkbGeneratorTransitionEffect : public hkbTransitionEffect
{
public:
    virtual ~hkbGeneratorTransitionEffect();
    struct hkbGeneratorhkbGenerator *transitionGenerator;
    float blendInDuration;
    float blendOutDuration;
    bool syncToGeneratorStartTime;
    void *fromGenerator;
    void *toGenerator;
    float timeInTransition;
    float duration;
    float effectiveBlendInDuration;
    float effectiveBlendOutDuration;
    int8 toGeneratorState;
    bool echoTransitionGenerator;
    bool echoToGenerator;
    bool justActivated;
    bool updateActiveNodes;
    int8 stage;
};
class hkbGeneratorTransitionEffectInternalState : public hkReferencedObject
{
public:
    virtual ~hkbGeneratorTransitionEffectInternalState();
    float timeInTransition;
    float duration;
    float effectiveBlendInDuration;
    float effectiveBlendOutDuration;
    int8 toGeneratorState;
    bool echoTransitionGenerator;
    bool echoToGenerator;
    bool justActivated;
    bool updateActiveNodes;
    int8 stage;
};
class hkbGetHandleOnBoneModifier : public hkbModifier
{
public:
    virtual ~hkbGetHandleOnBoneModifier();
    struct hkbHandlehkbHandle *handleOut;
    hkStringPtr localFrameName;
    int16 ragdollBoneIndex;
    int16 animationBoneIndex;
};
class hkbGetUpModifier : public hkbModifier
{
public:
    virtual ~hkbGetUpModifier();
    hkVector4 groundNormal;
    float duration;
    float alignWithGroundDuration;
    int16 rootBoneIndex;
    int16 otherBoneIndex;
    int16 anotherBoneIndex;
    float timeSinceBegin;
    float timeStep;
    bool initNextModify;
};
class hkbGetUpModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbGetUpModifierInternalState();
    float timeSinceBegin;
    float timeStep;
    bool initNextModify;
};
class hkbGetWorldFromModelModifier : public hkbModifier
{
public:
    virtual ~hkbGetWorldFromModelModifier();
    hkVector4 translationOut;
    hkQuaternion rotationOut;
};
class hkbGetWorldFromModelModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbGetWorldFromModelModifierInternalState();
    hkVector4 translationOut;
    hkQuaternion rotationOut;
};
class hkbHandIkControlsModifier : public hkbModifier
{
public:
    virtual ~hkbHandIkControlsModifier();
    hkArray<struct hkbHandIkControlsModifierHand> hands;
};
class hkbHandIkDriverInfo : public hkReferencedObject
{
public:
    virtual ~hkbHandIkDriverInfo();
    hkArray<struct hkbHandIkDriverInfoHand> hands;
    int8 fadeInOutCurve;
};
class hkbHandIkModifier : public hkbModifier
{
public:
    virtual ~hkbHandIkModifier();
    hkArray<struct hkbHandIkModifierHand> hands;
    int8 fadeInOutCurve;
    hkArray<void> internalHandData;
};
class hkbHandle : public hkReferencedObject
{
public:
    virtual ~hkbHandle();
    struct hkLocalFramehkLocalFrame *frame;
    struct hkpRigidBodyhkpRigidBody *rigidBody;
    struct hkbCharacterhkbCharacter *character;
    int16 animationBoneIndex;
};
class hkbIntEventPayload : public hkbEventPayload
{
public:
    virtual ~hkbIntEventPayload();
    int32 data;
};
class hkbIntVariableSequencedData : public hkbSequencedData
{
public:
    virtual ~hkbIntVariableSequencedData();
    hkArray<struct hkbIntVariableSequencedDataSample> samples;
    int32 variableIndex;
};
class hkbJigglerGroup : public hkbBindable
{
public:
    virtual ~hkbJigglerGroup();
    struct hkbBoneIndexArrayhkbBoneIndexArray *boneIndices;
    float mass;
    float stiffness;
    float damping;
    float maxElongation;
    float maxCompression;
    bool propagateToChildren;
    bool affectSiblings;
    bool rotateBonesForSkinning;
    bool pad;
};
class hkbJigglerModifier : public hkbModifier
{
public:
    virtual ~hkbJigglerModifier();
    hkArray<hkbJigglerGroup *> jigglerGroups;
    int8 jiggleCoordinates;
    hkArray<void> currentVelocitiesWS;
    hkArray<void> currentPositions;
    float timeStep;
    bool initNextModify;
};
class hkbJigglerModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbJigglerModifierInternalState();
    hkArray<hkVector4> currentVelocitiesWS;
    hkArray<hkVector4> currentPositions;
    float timeStep;
    bool initNextModify;
};
class hkbKeyframeBonesModifier : public hkbModifier
{
public:
    virtual ~hkbKeyframeBonesModifier();
    hkArray<struct hkbKeyframeBonesModifierKeyframeInfo> keyframeInfo;
    struct hkbBoneIndexArrayhkbBoneIndexArray *keyframedBonesList;
};
class hkbLookAtModifier : public hkbModifier
{
public:
    virtual ~hkbLookAtModifier();
    hkVector4 targetWS;
    hkVector4 headForwardLS;
    hkVector4 neckForwardLS;
    hkVector4 neckRightLS;
    hkVector4 eyePositionHS;
    float newTargetGain;
    float onGain;
    float offGain;
    float limitAngleDegrees;
    float limitAngleLeft;
    float limitAngleRight;
    float limitAngleUp;
    float limitAngleDown;
    int16 headIndex;
    int16 neckIndex;
    bool isOn;
    bool individualLimitsOn;
    bool isTargetInsideLimitCone;
    hkVector4 lookAtLastTargetWS;
    float lookAtWeight;
};
class hkbLookAtModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbLookAtModifierInternalState();
    hkVector4 lookAtLastTargetWS;
    float lookAtWeight;
    bool isTargetInsideLimitCone;
};
class hkbManualSelectorGenerator : public hkbGenerator
{
public:
    virtual ~hkbManualSelectorGenerator();
    hkArray<hkbGenerator *> generators;
    int8 selectedGeneratorIndex;
    int8 currentGeneratorIndex;
};
class hkbManualSelectorGeneratorInternalState : public hkReferencedObject
{
public:
    virtual ~hkbManualSelectorGeneratorInternalState();
    int8 currentGeneratorIndex;
};
class hkbMirrorModifier : public hkbModifier
{
public:
    virtual ~hkbMirrorModifier();
    bool isAdditive;
};
class hkbMirroredSkeletonInfo : public hkReferencedObject
{
public:
    virtual ~hkbMirroredSkeletonInfo();
    hkVector4 mirrorAxis;
    hkArray<int16> bonePairMap;
};
class hkbModifier : public hkbNode
{
public:
    virtual ~hkbModifier();
    bool enable;
    bool padModifier;
};
class hkbModifierGenerator : public hkbGenerator
{
public:
    virtual ~hkbModifierGenerator();
    struct hkbModifierhkbModifier *modifier;
    struct hkbGeneratorhkbGenerator *generator;
};
class hkbModifierList : public hkbModifier
{
public:
    virtual ~hkbModifierList();
    hkArray<hkbModifier *> modifiers;
};
class hkbModifierWrapper : public hkbModifier
{
public:
    virtual ~hkbModifierWrapper();
    struct hkbModifierhkbModifier *modifier;
};
class hkbMoveCharacterModifier : public hkbModifier
{
public:
    virtual ~hkbMoveCharacterModifier();
    hkVector4 offsetPerSecondMS;
    float timeSinceLastModify;
};
class hkbMoveCharacterModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbMoveCharacterModifierInternalState();
    float timeSinceLastModify;
};
class hkbNamedEventPayload : public hkbEventPayload
{
public:
    virtual ~hkbNamedEventPayload();
    hkStringPtr name;
};
class hkbNamedIntEventPayload : public hkbNamedEventPayload
{
public:
    virtual ~hkbNamedIntEventPayload();
    int32 data;
};
class hkbNamedRealEventPayload : public hkbNamedEventPayload
{
public:
    virtual ~hkbNamedRealEventPayload();
    float data;
};
class hkbNamedStringEventPayload : public hkbNamedEventPayload
{
public:
    virtual ~hkbNamedStringEventPayload();
    hkStringPtr data;
};
class hkbNode : public hkbBindable
{
public:
    virtual ~hkbNode();
    uint64 userData;
    hkStringPtr name;
    int16 id;
    int8 cloneState;
    bool padNode;
};
class hkbNodeInternalStateInfo : public hkReferencedObject
{
public:
    virtual ~hkbNodeInternalStateInfo();
    struct hkbGeneratorSyncInfo syncInfo;
    hkStringPtr name;
    struct hkReferencedObjecthkReferencedObject *internalState;
    int16 nodeId;
    bool hasActivateBeenCalled;
};
class hkbPoseMatchingGenerator : public hkbBlenderGenerator
{
public:
    virtual ~hkbPoseMatchingGenerator();
    hkQuaternion worldFromModelRotation;
    float blendSpeed;
    float minSpeedToSwitch;
    float minSwitchTimeNoError;
    float minSwitchTimeFullError;
    int32 startPlayingEventId;
    int32 startMatchingEventId;
    int16 rootBoneIndex;
    int16 otherBoneIndex;
    int16 anotherBoneIndex;
    int16 pelvisIndex;
    int8 mode;
    int32 currentMatch;
    int32 bestMatch;
    float timeSinceBetterMatch;
    float error;
    bool resetCurrentMatchLocalTime;
    void *poseMatchingUtility;
};
class hkbPoseMatchingGeneratorInternalState : public hkReferencedObject
{
public:
    virtual ~hkbPoseMatchingGeneratorInternalState();
    int32 currentMatch;
    int32 bestMatch;
    float timeSinceBetterMatch;
    float error;
    bool resetCurrentMatchLocalTime;
};
class hkbPositionRelativeSelectorGenerator : public hkbGenerator
{
public:
    virtual ~hkbPositionRelativeSelectorGenerator();
    hkArray<hkbRegisteredGenerator *> registeredGenerators;
    struct hkbGeneratorhkbGenerator *blendToFixPositionGenerator;
    bool autoComputeEntryPoints;
    float transitionTime;
    bool useCharacterForward;
    hkVector4 characterForward;
    hkVector4 targetPosition;
    hkQuaternion targetRotation;
    float positionTolerance;
    float fixPositionTolerance;
    struct hkbEventProperty fixPositionEvent;
    int32 endFixPositionEventId;
    bool useManualSelection;
    int8 selectedGeneratorIndex;
    hkVector4 entryPosition;
    hkVector4 entryForward;
    hkVector4 lastTargetPos;
    hkQuaternion lastTargetRot;
    hkVector4 targetLinearDisp;
    hkQuaternion targetAngularDisp;
    float clipTime;
    int8 currentGeneratorIndex;
    bool usingBlendToFixPositionGenerator;
    bool doLeadInFixup;
};
class hkbPositionRelativeSelectorGeneratorInternalState : public hkReferencedObject
{
public:
    virtual ~hkbPositionRelativeSelectorGeneratorInternalState();
    hkVector4 entryPosition;
    hkVector4 entryForward;
    hkVector4 lastTargetPos;
    hkQuaternion lastTargetRot;
    hkVector4 targetLinearDisp;
    hkQuaternion targetAngularDisp;
    float clipTime;
    int8 currentGeneratorIndex;
    bool usingBlendToFixPositionGenerator;
    bool doLeadInFixup;
};
class hkbPoweredRagdollControlsModifier : public hkbModifier
{
public:
    virtual ~hkbPoweredRagdollControlsModifier();
    struct hkbPoweredRagdollControlData controlData;
    struct hkbBoneIndexArrayhkbBoneIndexArray *bones;
    struct hkbWorldFromModelModeData worldFromModelModeData;
    struct hkbBoneWeightArrayhkbBoneWeightArray *boneWeights;
};
class hkbProjectData : public hkReferencedObject
{
public:
    virtual ~hkbProjectData();
    hkVector4 worldUpWS;
    struct hkbProjectStringDatahkbProjectStringData *stringData;
    int8 defaultEventMode;
};
class hkbProjectStringData : public hkReferencedObject
{
public:
    virtual ~hkbProjectStringData();
    hkArray<hkStringPtr> animationFilenames;
    hkArray<hkStringPtr> behaviorFilenames;
    hkArray<hkStringPtr> characterFilenames;
    hkArray<hkStringPtr> eventNames;
    hkStringPtr animationPath;
    hkStringPtr behaviorPath;
    hkStringPtr characterPath;
    hkStringPtr fullPathToSource;
    hkStringPtr rootPath;
};
class hkbProxyModifier : public hkbModifier
{
public:
    virtual ~hkbProxyModifier();
    struct hkbProxyModifierProxyInfo proxyInfo;
    hkVector4 linearVelocity;
    float horizontalGain;
    float verticalGain;
    float maxHorizontalSeparation;
    float maxVerticalSeparation;
    float verticalDisplacementError;
    float verticalDisplacementErrorGain;
    float maxVerticalDisplacement;
    float minVerticalDisplacement;
    float capsuleHeight;
    float capsuleRadius;
    float maxSlopeForRotation;
    uint32 collisionFilterInfo;
    int8 phantomType;
    int8 linearVelocityMode;
    bool ignoreIncomingRotation;
    bool ignoreCollisionDuringRotation;
    bool ignoreIncomingTranslation;
    bool includeDownwardMomentum;
    bool followWorldFromModel;
    bool isTouchingGround;
    void *characterProxy;
    void *phantom;
    void *phantomShape;
    hkVector4 horizontalDisplacement;
    float verticalDisplacement;
    float timestep;
    bool previousFrameFollowWorldFromModel;
};
class hkbRealEventPayload : public hkbEventPayload
{
public:
    virtual ~hkbRealEventPayload();
    float data;
};
class hkbRealVariableSequencedData : public hkbSequencedData
{
public:
    virtual ~hkbRealVariableSequencedData();
    hkArray<struct hkbRealVariableSequencedDataSample> samples;
    int32 variableIndex;
};
class hkbReferencePoseGenerator : public hkbGenerator
{
public:
    virtual ~hkbReferencePoseGenerator();
    void *skeleton;
};
class hkbRegisteredGenerator : public hkbBindable
{
public:
    virtual ~hkbRegisteredGenerator();
    struct hkbGeneratorhkbGenerator *generator;
    hkVector4 relativePosition;
    hkVector4 relativeDirection;
};
class hkbRigidBodyRagdollControlsModifier : public hkbModifier
{
public:
    virtual ~hkbRigidBodyRagdollControlsModifier();
    struct hkbRigidBodyRagdollControlData controlData;
    struct hkbBoneIndexArrayhkbBoneIndexArray *bones;
};
class hkbRotateCharacterModifier : public hkbModifier
{
public:
    virtual ~hkbRotateCharacterModifier();
    float degreesPerSecond;
    float speedMultiplier;
    hkVector4 axisOfRotation;
    float angle;
};
class hkbRotateCharacterModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbRotateCharacterModifierInternalState();
    float angle;
};
class hkbSenseHandleModifier : public hkbModifier
{
public:
    virtual ~hkbSenseHandleModifier();
    struct hkbHandle handle;
    hkVector4 sensorLocalOffset;
    hkArray<struct hkbSenseHandleModifierRange> ranges;
    struct hkbHandlehkbHandle *handleOut;
    struct hkbHandlehkbHandle *handleIn;
    hkStringPtr localFrameName;
    hkStringPtr sensorLocalFrameName;
    float minDistance;
    float maxDistance;
    float distanceOut;
    uint32 collisionFilterInfo;
    int16 sensorRagdollBoneIndex;
    int16 sensorAnimationBoneIndex;
    int8 sensingMode;
    bool extrapolateSensorPosition;
    bool keepFirstSensedHandle;
    bool foundHandleOut;
    float timeSinceLastModify;
    int32 rangeIndexForEventToSendNextUpdate;
};
class hkbSequence : public hkbModifier
{
public:
    virtual ~hkbSequence();
    hkArray<hkbEventSequencedData *> eventSequencedData;
    hkArray<hkbRealVariableSequencedData *> realVariableSequencedData;
    hkArray<hkbBoolVariableSequencedData *> boolVariableSequencedData;
    hkArray<hkbIntVariableSequencedData *> intVariableSequencedData;
    int32 enableEventId;
    int32 disableEventId;
    struct hkbSequenceStringDatahkbSequenceStringData *stringData;
    void *variableIdMap;
    void *eventIdMap;
    hkArray<void> nextSampleEvents;
    hkArray<void> nextSampleReals;
    hkArray<void> nextSampleBools;
    hkArray<void> nextSampleInts;
    float time;
    bool isEnabled;
};
class hkbSequenceInternalState : public hkReferencedObject
{
public:
    virtual ~hkbSequenceInternalState();
    hkArray<int32> nextSampleEvents;
    hkArray<int32> nextSampleReals;
    hkArray<int32> nextSampleBools;
    hkArray<int32> nextSampleInts;
    float time;
    bool isEnabled;
};
class hkbSequenceStringData : public hkReferencedObject
{
public:
    virtual ~hkbSequenceStringData();
    hkArray<hkStringPtr> eventNames;
    hkArray<hkStringPtr> variableNames;
};
class hkbSetWorldFromModelModifier : public hkbModifier
{
public:
    virtual ~hkbSetWorldFromModelModifier();
    hkVector4 translation;
    hkQuaternion rotation;
    bool setTranslation;
    bool setRotation;
};
class hkbSplinePathGenerator : public hkbGenerator
{
public:
    virtual ~hkbSplinePathGenerator();
    hkArray<hkbRegisteredGenerator *> registeredGenerators;
    hkVector4 characterForward;
    hkVector4 targetPosition;
    hkVector4 targetDirection;
    struct hkbEventProperty pathEndEvent;
    float leadInGain;
    float leadOutGain;
    bool useProximityTrigger;
    float endEventProximity;
    float endEventTime;
    int8 selectedGeneratorIndex;
    bool useManualSelection;
    bool trackPosition;
    bool usePathEstimation;
    bool useCharacterForward;
    hkVector4 entryPosition;
    hkVector4 entryForward;
    hkVector4 exitPosition;
    hkVector4 exitForward;
    float averageSpeed;
    float pathTime;
    float curTime;
    float pathParam;
    int8 currentGeneratorIndex;
    bool doLeadInFixup;
    bool eventTriggered;
};
class hkbSplinePathGeneratorInternalState : public hkReferencedObject
{
public:
    virtual ~hkbSplinePathGeneratorInternalState();
    hkVector4 entryPosition;
    hkVector4 entryForward;
    hkVector4 exitPosition;
    hkVector4 exitForward;
    float averageSpeed;
    float pathTime;
    float curTime;
    float pathParam;
    int8 currentGeneratorIndex;
    bool doLeadInFixup;
    bool eventTriggered;
};
class hkbStateListener : public hkReferencedObject
{
public:
    virtual ~hkbStateListener();
};
class hkbStateMachine : public hkbGenerator
{
public:
    virtual ~hkbStateMachine();
    struct hkbEvent eventToSendWhenStateOrTransitionChanges;
    struct hkbStateChooserhkbStateChooser *startStateChooser;
    int32 startStateId;
    int32 returnToPreviousStateEventId;
    int32 randomTransitionEventId;
    int32 transitionToNextHigherStateEventId;
    int32 transitionToNextLowerStateEventId;
    int32 syncVariableIndex;
    int32 currentStateId;
    bool wrapAroundStateId;
    int8 maxSimultaneousTransitions;
    int8 startStateMode;
    int8 selfTransitionMode;
    bool isActive;
    hkArray<hkbStateMachineStateInfo *> states;
    struct hkbStateMachineTransitionInfoArrayhkbStateMachineTransitionInfoArray *wildcardTransitions;
    void *stateIdToIndexMap;
    hkArray<void> activeTransitions;
    hkArray<void> transitionFlags;
    hkArray<void> wildcardTransitionFlags;
    hkArray<void> delayedTransitions;
    float timeInState;
    float lastLocalTime;
    int32 previousStateId;
    int32 nextStartStateIndexOverride;
    bool stateOrTransitionChanged;
    bool echoNextUpdate;
    uint16 sCurrentStateIndexAndEntered;
};
class hkbStateMachineEventPropertyArray : public hkReferencedObject
{
public:
    virtual ~hkbStateMachineEventPropertyArray();
    hkArray<struct hkbEventProperty> events;
};
class hkbStateMachineInternalState : public hkReferencedObject
{
public:
    virtual ~hkbStateMachineInternalState();
    hkArray<struct hkbStateMachineActiveTransitionInfo> activeTransitions;
    hkArray<uint8> transitionFlags;
    hkArray<uint8> wildcardTransitionFlags;
    hkArray<struct hkbStateMachineDelayedTransitionInfo> delayedTransitions;
    float timeInState;
    float lastLocalTime;
    int32 currentStateId;
    int32 previousStateId;
    int32 nextStartStateIndexOverride;
    bool stateOrTransitionChanged;
    bool echoNextUpdate;
};
class hkbStateMachineStateInfo : public hkbBindable
{
public:
    virtual ~hkbStateMachineStateInfo();
    hkArray<hkbStateListener *> listeners;
    struct hkbStateMachineEventPropertyArrayhkbStateMachineEventPropertyArray *enterNotifyEvents;
    struct hkbStateMachineEventPropertyArrayhkbStateMachineEventPropertyArray *exitNotifyEvents;
    struct hkbStateMachineTransitionInfoArrayhkbStateMachineTransitionInfoArray *transitions;
    struct hkbGeneratorhkbGenerator *generator;
    hkStringPtr name;
    int32 stateId;
    float probability;
    bool enable;
};
class hkbStateMachineTransitionInfoArray : public hkReferencedObject
{
public:
    virtual ~hkbStateMachineTransitionInfoArray();
    hkArray<struct hkbStateMachineTransitionInfo> transitions;
};
class hkbStringCondition : public hkbCondition
{
public:
    virtual ~hkbStringCondition();
    hkStringPtr conditionString;
};
class hkbStringEventPayload : public hkbEventPayload
{
public:
    virtual ~hkbStringEventPayload();
    hkStringPtr data;
};
class hkbTimerModifier : public hkbModifier
{
public:
    virtual ~hkbTimerModifier();
    float alarmTimeSeconds;
    struct hkbEventProperty alarmEvent;
    float secondsElapsed;
};
class hkbTimerModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbTimerModifierInternalState();
    float secondsElapsed;
};
class hkbTransformVectorModifier : public hkbModifier
{
public:
    virtual ~hkbTransformVectorModifier();
    hkQuaternion rotation;
    hkVector4 translation;
    hkVector4 vectorIn;
    hkVector4 vectorOut;
    bool rotateOnly;
    bool inverse;
    bool computeOnActivate;
    bool computeOnModify;
};
class hkbTransformVectorModifierInternalState : public hkReferencedObject
{
public:
    virtual ~hkbTransformVectorModifierInternalState();
    hkVector4 vectorOut;
};
class hkbTwistModifier : public hkbModifier
{
public:
    virtual ~hkbTwistModifier();
    hkVector4 axisOfRotation;
    float twistAngle;
    int16 startBoneIndex;
    int16 endBoneIndex;
    int8 setAngleMethod;
    int8 rotationAxisCoordinates;
    bool isAdditive;
    hkArray<void> boneChainIndices;
    hkArray<void> parentBoneIndices;
};
class hkbVariableBindingSet : public hkReferencedObject
{
public:
    virtual ~hkbVariableBindingSet();
    hkArray<struct hkbVariableBindingSetBinding> bindings;
    int32 indexOfBindingToEnable;
    bool hasOutputBinding;
};
class hkbVariableValueSet : public hkReferencedObject
{
public:
    virtual ~hkbVariableValueSet();
    hkArray<struct hkbVariableValue> wordVariableValues;
    hkArray<hkVector4> quadVariableValues;
    hkArray<hkReferencedObject *> variantVariableValues;
};
class hkbAlignBoneModifier : public hkbModifier
{
public:
    virtual ~hkbAlignBoneModifier();
    int8 alignMode;
    int8 alignTargetMode;
    bool alignSingleAxis;
    hkVector4 alignAxis;
    hkVector4 alignTargetAxis;
    hkQuaternion frameOfReference;
    float duration;
    int32 alignModeIndex;
    int32 alignTargetModeIndex;
    float time;
    float timeSinceLastModify;
};
class hkbAnimatedSkeletonGenerator : public hkbGenerator
{
public:
    virtual ~hkbAnimatedSkeletonGenerator();
    void *animatedSkeleton;
};
class hkbAuxiliaryNodeInfo : public hkReferencedObject
{
public:
    virtual ~hkbAuxiliaryNodeInfo();
    uint8 type;
    uint8 depth;
    hkStringPtr referenceBehaviorName;
    hkArray<hkStringPtr> selfTransitionNames;
};
class hkbBalanceModifier : public hkbModifier
{
public:
    virtual ~hkbBalanceModifier();
    bool giveUp;
    float comDistThreshold;
    bool passThrough;
    int16 ragdollLeftFootBoneIndex;
    int16 ragdollRightFootBoneIndex;
    float balanceOnAnklesFraction;
    int32 upAxis;
    float fadeInTime;
    float comBiasX;
    hkArray<struct hkbBalanceModifierStepInfo> stepInfo;
    float timeLapsed;
};
class hkbBalanceRadialSelectorGenerator : public hkbRadialSelectorGenerator
{
public:
    virtual ~hkbBalanceRadialSelectorGenerator();
    int32 xAxisMS;
    int32 yAxisMS;
    struct hkbCheckBalanceModifierhkbCheckBalanceModifier *checkBalanceModifier;
};
class hkbBehaviorEventsInfo : public hkReferencedObject
{
public:
    virtual ~hkbBehaviorEventsInfo();
    uint64 characterId;
    hkArray<int16> externalEventIds;
    int32 padding;
};
class hkbBehaviorGraphInternalStateInfo : public hkReferencedObject
{
public:
    virtual ~hkbBehaviorGraphInternalStateInfo();
    uint64 characterId;
    struct hkbBehaviorGraphInternalStatehkbBehaviorGraphInternalState *internalState;
    hkArray<hkbAuxiliaryNodeInfo *> auxiliaryNodeInfo;
    hkArray<int16> activeEventIds;
    hkArray<int16> activeVariableIds;
};
class hkbBehaviorInfo : public hkReferencedObject
{
public:
    virtual ~hkbBehaviorInfo();
    uint64 characterId;
    struct hkbBehaviorGraphDatahkbBehaviorGraphData *data;
    hkArray<struct hkbBehaviorInfoIdToNamePair> idToNamePairs;
};
class hkbCameraShakeEventPayload : public hkbEventPayload
{
public:
    virtual ~hkbCameraShakeEventPayload();
    float amplitude;
    float halfLife;
};
class hkbCatchFallModifier : public hkbModifier
{
public:
    virtual ~hkbCatchFallModifier();
    hkVector4 directionOfFallForwardLS;
    hkVector4 directionOfFallRightLS;
    hkVector4 directionOfFallUpLS;
    struct hkbBoneIndexArrayhkbBoneIndexArray *spineIndices;
    struct hkbCatchFallModifierHand leftHand;
    struct hkbCatchFallModifierHand rightHand;
    struct hkbEventProperty catchFallDoneEvent;
    float spreadHandsMultiplier;
    float radarRange;
    float previousTargetBlendWeight;
    float handsBendDistance;
    float maxReachDistanceForward;
    float maxReachDistanceBackward;
    float fadeInReachGainSpeed;
    float fadeOutReachGainSpeed;
    float fadeOutDuration;
    float fadeInTwistSpeed;
    float fadeOutTwistSpeed;
    int16 raycastLayer;
    int16 velocityRagdollBoneIndex;
    int16 directionOfFallRagdollBoneIndex;
    bool orientHands;
    hkVector4 catchFallPosInBS;
    float currentReachGain;
    float timeSinceLastModify;
    float currentTwistGain;
    int16 currentTwistDirection;
    bool catchFallPosIsValid;
    bool catchFallBegin;
    bool catchFallEnd;
    int8 fadeState;
};
class hkbCharacterAddedInfo : public hkReferencedObject
{
public:
    virtual ~hkbCharacterAddedInfo();
    uint64 characterId;
    hkStringPtr instanceName;
    hkStringPtr templateName;
    hkStringPtr fullPathToProject;
    struct hkaSkeletonhkaSkeleton *skeleton;
    hkQsTransform worldFromModel;
    hkArray<hkQsTransform> poseModelSpace;
};
class hkbCharacterControlCommand : public hkReferencedObject
{
public:
    virtual ~hkbCharacterControlCommand();
    uint64 characterId;
    uint8 command;
    int32 padding;
};
class hkbCharacterInfo : public hkReferencedObject
{
public:
    virtual ~hkbCharacterInfo();
    uint64 characterId;
    uint8 event;
    int32 padding;
};
class hkbCharacterSkinInfo : public hkReferencedObject
{
public:
    virtual ~hkbCharacterSkinInfo();
    uint64 characterId;
    hkArray<uint64> deformableSkins;
    hkArray<uint64> rigidSkins;
};
class hkbCharacterSteppedInfo : public hkReferencedObject
{
public:
    virtual ~hkbCharacterSteppedInfo();
    uint64 characterId;
    float deltaTime;
    hkQsTransform worldFromModel;
    hkArray<hkQsTransform> poseModelSpace;
    hkArray<hkQsTransform> rigidAttachmentTransforms;
};
class hkbCheckBalanceModifier : public hkbModifier
{
public:
    virtual ~hkbCheckBalanceModifier();
    int16 ragdollLeftFootBoneIndex;
    int16 ragdollRightFootBoneIndex;
    float balanceOnAnklesFraction;
    struct hkbEvent eventToSendWhenOffBalance;
    float offBalanceEventThreshold;
    int32 worldUpAxisIndex;
    float comBiasX;
    bool extractRagdollPose;
    float mass;
    hkVector4 comWS;
    hkVector4 desiredComWS;
    float offBalanceDistance;
    hkVector4 errorMS;
};
class hkbCheckRagdollSpeedModifier : public hkbModifier
{
public:
    virtual ~hkbCheckRagdollSpeedModifier();
    struct hkbEventProperty eventToSend;
    float minSpeedThreshold;
    float maxSpeedThreshold;
};
class hkbClientCharacterState : public hkReferencedObject
{
public:
    virtual ~hkbClientCharacterState();
    hkArray<uint64> deformableSkinIds;
    hkArray<uint64> rigidSkinIds;
    hkArray<int16> externalEventIds;
    hkArray<hkbAuxiliaryNodeInfo *> auxiliaryInfo;
    hkArray<int16> activeEventIds;
    hkArray<int16> activeVariableIds;
    uint64 characterId;
    hkStringPtr instanceName;
    hkStringPtr templateName;
    hkStringPtr fullPathToProject;
    struct hkbBehaviorGraphDatahkbBehaviorGraphData *behaviorData;
    struct hkbBehaviorGraphInternalStatehkbBehaviorGraphInternalState *behaviorInternalState;
    void *nodeIdToInternalStateMap;
    bool visible;
    float elapsedSimulationTime;
    struct hkaSkeletonhkaSkeleton *skeleton;
    hkQsTransform worldFromModel;
    hkArray<hkQsTransform> poseModelSpace;
    hkArray<hkQsTransform> rigidAttachmentTransforms;
};
class hkbComputeWorldFromModelModifier : public hkbModifier
{
public:
    virtual ~hkbComputeWorldFromModelModifier();
    int16 poseMatchingRootBoneIndex;
    int16 poseMatchingOtherBoneIndex;
    int16 poseMatchingAnotherBoneIndex;
};
class hkbConstrainRigidBodyModifier : public hkbModifier
{
public:
    virtual ~hkbConstrainRigidBodyModifier();
    struct hkbTargethkbTarget *targetIn;
    float breakThreshold;
    int16 ragdollBoneToConstrain;
    bool breakable;
    int8 pivotPlacement;
    int8 boneToConstrainPlacement;
    int8 constraintType;
    bool clearTargetData;
    bool isConstraintHinge;
    void *constraint;
};
class hkbControlledReachModifier : public hkbModifier
{
public:
    virtual ~hkbControlledReachModifier();
    void *target;
    float time;
    float fadeInStart;
    float fadeInEnd;
    float fadeOutStart;
    float fadeOutEnd;
    float fadeOutDuration;
    float sensorAngle;
    int16 handIndex;
    bool isHandEnabled;
};
class hkbCustomTestGenerator : public hkbReferencePoseGenerator
{
public:
    virtual ~hkbCustomTestGenerator();
    bool hkBool;
    hkStringPtr string;
    int32 hkInt;
    int8 hkInt8;
    int16 hkInt16;
    int32 hkInt32;
    uint8 hkUint8;
    uint16 hkUint16;
    uint32 hkUint32;
    float hkReal;
    hkVector4 hkVector4;
    hkQuaternion hkQuaternion;
    struct hkpRigidBodyhkpRigidBody *hkRigidBody;
    int8 mode_hkInt8;
    int16 mode_hkInt16;
    int32 mode_hkInt32;
    uint8 mode_hkUint8;
    uint16 mode_hkUint16;
    uint32 mode_hkUint32;
    hkFlags flags_hkInt8;
    hkFlags flags_hkInt16;
    hkFlags flags_hkInt32;
    hkFlags flags_hkUint8;
    hkFlags flags_hkUint16;
    hkFlags flags_hkUint32;
    int32 myInt;
    struct hkbGeneratorhkbGenerator *generator1;
    struct hkbGeneratorhkbGenerator *generator2;
    struct hkbModifierhkbModifier *modifier1;
    struct hkbModifierhkbModifier *modifier2;
    hkArray<bool> array_hkBool;
    hkArray<int32> array_int;
    hkArray<int8> array_hkInt8;
    hkArray<int16> array_hkInt16;
    hkArray<int32> array_hkInt32;
    hkArray<int8> array_hkUint8;
    hkArray<int16> array_hkUint16;
    hkArray<int32> array_hkUint32;
    hkArray<float> array_hkReal;
    hkArray<hkbGenerator *> array_hkbGenerator;
    hkArray<hkbModifier *> array_hkbModifier;
    struct hkbCustomTestGeneratorStruck Struck;
    hkArray<struct hkbCustomTestGeneratorStruck> array_Struck;
};
class hkbDemoConfig : public hkReferencedObject
{
public:
    virtual ~hkbDemoConfig();
    hkArray<hkbDemoConfigCharacterInfo *> characterInfo;
    hkArray<struct hkbDemoConfigTerrainInfo> terrainInfo;
    hkArray<int32> skinAttributeIndices;
    int32 buttonPressToEventMap;
    int32 buttonReleaseToEventMap;
    int32 worldUpAxis;
    int32 extraCharacterClones;
    int32 numTracks;
    float proxyHeight;
    float proxyRadius;
    float proxyOffset;
    hkStringPtr rootPath;
    hkStringPtr projectDataFilename;
    bool useAttachments;
    bool useProxy;
    bool useSkyBox;
    bool useTrackingCamera;
    bool accumulateMotion;
    bool testCloning;
    bool useSplineCompression;
    bool forceLoad;
    hkArray<struct hkbDemoConfigStickVariableInfo> stickVariables;
    int32 gamePadToRotateTerrainAboutItsAxisMap;
    int32 gamePadToAddRemoveCharacterMap;
    struct hkpGroupFilterhkpGroupFilter *filter;
};
class hkbDemoConfigCharacterInfo : public hkReferencedObject
{
public:
    virtual ~hkbDemoConfigCharacterInfo();
    hkStringPtr overrideCharacterDataFilename;
    hkStringPtr overrideBehaviorFilename;
    hkVector4 initialPosition;
    hkQuaternion initialRotation;
    int32 modelUpAxis;
    hkArray<int32> ragdollBoneLayers;
};
class hkbEventRaisedInfo : public hkReferencedObject
{
public:
    virtual ~hkbEventRaisedInfo();
    uint64 characterId;
    hkStringPtr eventName;
    bool raisedBySdk;
    int32 senderId;
    int32 padding;
};
class hkbFaceTargetModifier : public hkbModifier
{
public:
    virtual ~hkbFaceTargetModifier();
    struct hkbTargethkbTarget *targetIn;
    float offsetAngle;
    bool onlyOnce;
    bool done;
};
class hkbGravityModifier : public hkbModifier
{
public:
    virtual ~hkbGravityModifier();
    hkVector4 initialVelocityInMS;
    float gravityConstant;
    hkVector4 currentVelocityInMS;
    float secondsElapsed;
};
class hkbHoldFromBlendingTransitionEffect : public hkbBlendingTransitionEffect
{
public:
    virtual ~hkbHoldFromBlendingTransitionEffect();
    void *heldFromPose;
    int32 heldFromPoseSize;
    hkQsTransform heldWorldFromModel;
    void *heldFromSkeleton;
};
class hkbLinkedSymbolInfo : public hkReferencedObject
{
public:
    virtual ~hkbLinkedSymbolInfo();
    hkArray<hkStringPtr> eventNames;
    hkArray<hkStringPtr> variableNames;
};
class hkbMoveBoneTowardTargetModifier : public hkbModifier
{
public:
    virtual ~hkbMoveBoneTowardTargetModifier();
    struct hkbTargethkbTarget *targetIn;
    hkVector4 offsetInBoneSpace;
    hkVector4 alignAxisBS;
    hkVector4 targetAlignAxisTS;
    hkVector4 alignWithCharacterForwardBS;
    hkVector4 currentBonePositionOut;
    hkQuaternion currentBoneRotationOut;
    struct hkbEventProperty eventToSendWhenTargetReached;
    struct hkbGeneratorhkbGenerator *childGenerator;
    float duration;
    int16 ragdollBoneIndex;
    int16 animationBoneIndex;
    int8 targetMode;
    int8 alignMode;
    bool useVelocityPrediction;
    bool affectOrientation;
    bool currentBoneIsValidOut;
    void *targetInternal;
    hkVector4 targetPointTS;
    float time;
    float timeSinceLastModify;
    hkVector4 lastAnimBonePositionMS;
    hkVector4 finalAnimBonePositionMS;
    hkVector4 initialAnimBonePositionMS;
    hkQuaternion finalAnimBoneOrientationMS;
    hkQuaternion animationFromRagdoll;
    hkQsTransform totalMotion;
    hkQsTransform accumulatedMotion;
    bool useAnimationData;
};
class hkbParticleSystemEventPayload : public hkbEventPayload
{
public:
    virtual ~hkbParticleSystemEventPayload();
    uint8 type;
    int16 emitBoneIndex;
    hkVector4 offset;
    hkVector4 direction;
    int32 numParticles;
    float speed;
};
class hkbPoseStoringGeneratorOutputListener : public hkbGeneratorOutputListener
{
public:
    virtual ~hkbPoseStoringGeneratorOutputListener();
    hkArray<hkbPoseStoringGeneratorOutputListenerStoredPose *> storedPoses;
    bool dirty;
    void *nodeToIndexMap;
};
class hkbPoseStoringGeneratorOutputListenerStoredPose : public hkReferencedObject
{
public:
    virtual ~hkbPoseStoringGeneratorOutputListenerStoredPose();
    struct hkbNodehkbNode *node;
    hkArray<hkQsTransform> pose;
    hkQsTransform worldFromModel;
    bool isPoseValid;
};
class hkbRadialSelectorGenerator : public hkbGenerator
{
public:
    virtual ~hkbRadialSelectorGenerator();
    hkArray<struct hkbRadialSelectorGeneratorGeneratorPair> generatorPairs;
    float angle;
    float radius;
    int32 currentGeneratorPairIndex;
    int32 currentEndpointIndex;
    float currentFraction;
    bool hasSetLocalTime;
};
class hkbRaiseEventCommand : public hkReferencedObject
{
public:
    virtual ~hkbRaiseEventCommand();
    uint64 characterId;
    bool global;
    int32 externalId;
};
class hkbReachModifier : public hkbModifier
{
public:
    virtual ~hkbReachModifier();
    hkArray<struct hkbReachModifierHand> hands;
    float newTargetGain;
    float noTargetGain;
    float targetGain;
    float fadeOutDuration;
    int32 raycastLayer;
    uint32 sensingPropertyKey;
    int8 reachMode;
    bool ignoreMySystemGroup;
    float extrapolate;
    hkArray<void> internalHandData;
    float timeLapse;
};
class hkbReachTowardTargetModifier : public hkbModifier
{
public:
    virtual ~hkbReachTowardTargetModifier();
    struct hkbReachTowardTargetModifierHand leftHand;
    struct hkbReachTowardTargetModifierHand rightHand;
    struct hkbTargethkbTarget *targetIn;
    float distanceBetweenHands;
    float reachDistance;
    float fadeInGainSpeed;
    float fadeOutGainSpeed;
    float fadeOutDuration;
    float targetChangeSpeed;
    bool holdTarget;
    bool reachPastTarget;
    bool giveUpIfNoTarget;
    void *targetRB;
    hkVector4 prevTargetInMS;
    float currentGain;
    float timeSinceLastModify;
    int8 fadeState;
    bool haveGivenUp;
    bool isTherePrevTarget;
};
class hkbSetBehaviorCommand : public hkReferencedObject
{
public:
    virtual ~hkbSetBehaviorCommand();
    uint64 characterId;
    struct hkbBehaviorGraphhkbBehaviorGraph *behavior;
    struct hkbGeneratorhkbGenerator *rootGenerator;
    hkArray<hkbBehaviorGraph *> referencedBehaviors;
    int32 startStateIndex;
    bool randomizeSimulation;
    int32 padding;
};
class hkbSetLocalTimeOfClipGeneratorCommand : public hkReferencedObject
{
public:
    virtual ~hkbSetLocalTimeOfClipGeneratorCommand();
    uint64 characterId;
    float localTime;
    int16 nodeId;
};
class hkbSetNodePropertyCommand : public hkReferencedObject
{
public:
    virtual ~hkbSetNodePropertyCommand();
    uint64 characterId;
    hkStringPtr nodeName;
    hkStringPtr propertyName;
    struct hkbVariableValue propertyValue;
    int32 padding;
};
class hkbSetWordVariableCommand : public hkReferencedObject
{
public:
    virtual ~hkbSetWordVariableCommand();
    hkVector4 quadValue;
    uint64 characterId;
    int32 variableId;
    struct hkbVariableValue value;
    uint8 type;
    bool global;
};
class hkbSimulationControlCommand : public hkReferencedObject
{
public:
    virtual ~hkbSimulationControlCommand();
    uint8 command;
};
class hkbSimulationStateInfo : public hkReferencedObject
{
public:
    virtual ~hkbSimulationStateInfo();
    uint8 simulationState;
};
class hkbStateDependentModifier : public hkbModifier
{
public:
    virtual ~hkbStateDependentModifier();
    bool applyModifierDuringTransition;
    hkArray<int32> stateIds;
    struct hkbModifierhkbModifier *modifier;
    bool isActive;
    struct hkbStateMachinehkbStateMachine *stateMachine;
};
class hkbTarget : public hkReferencedObject
{
public:
    virtual ~hkbTarget();
    void *target;
    hkVector4 contactPointTS;
    hkVector4 contactNormalTS;
    hkVector4 shapeCentroidTS;
    float distance;
    int8 targetPriority;
};
class hkbTargetRigidBodyModifier : public hkbModifier
{
public:
    virtual ~hkbTargetRigidBodyModifier();
    struct hkbTargethkbTarget *targetOut;
    int8 targetMode;
    int32 sensingLayer;
    bool targetOnlyOnce;
    bool ignoreMySystemGroup;
    float maxTargetDistance;
    float maxTargetHeightAboveSensor;
    float closeToTargetDistanceThreshold;
    int8 targetAngleMode;
    int8 targetDistanceMode;
    float maxAngleToTarget;
    int16 sensorRagdollBoneIndex;
    int16 sensorAnimationBoneIndex;
    int16 closeToTargetRagdollBoneIndex;
    int16 closeToTargetAnimationBoneIndex;
    hkVector4 sensorOffsetInBoneSpace;
    hkVector4 closeToTargetOffsetInBoneSpace;
    hkVector4 sensorDirectionBS;
    int8 eventMode;
    uint32 sensingPropertyKey;
    bool sensorInWS;
    struct hkbEventProperty eventToSend;
    struct hkbEventProperty eventToSendToTarget;
    struct hkbEventProperty closeToTargetEvent;
    bool useVelocityPrediction;
    bool targetOnlySpheres;
    bool isCloseToTargetOut;
    int8 targetPriority;
    void *targetRb;
    float targetDistance;
    float timeSinceLastModify;
    bool eventHasBeenSent;
    bool closeToTargetEventHasBeenSent;
    bool isActive;
};
class hkbTestStateChooser : public hkbStateChooser
{
public:
    virtual ~hkbTestStateChooser();
    int32 hkInt;
    float real;
    hkStringPtr string;
};
class BGSGamebryoSequenceGenerator : public hkbGenerator
{
public:
    virtual ~BGSGamebryoSequenceGenerator();
    char *pSequence;
    int8 eBlendModeFunction;
    float fPercent;
    hkArray<void> events;
    float fTime;
    bool bDelayedActivate;
    bool bLooping;
};
class BSBoneSwitchGenerator : public hkbGenerator
{
public:
    virtual ~BSBoneSwitchGenerator();
    struct hkbGeneratorhkbGenerator *pDefaultGenerator;
    hkArray<BSBoneSwitchGeneratorBoneData *> ChildrenA;
};
class BSBoneSwitchGeneratorBoneData : public hkbBindable
{
public:
    virtual ~BSBoneSwitchGeneratorBoneData();
    struct hkbGeneratorhkbGenerator *pGenerator;
    struct hkbBoneWeightArrayhkbBoneWeightArray *spBoneWeight;
};
class BSComputeAddBoneAnimModifier : public hkbModifier
{
public:
    virtual ~BSComputeAddBoneAnimModifier();
    int16 boneIndex;
    hkVector4 translationLSOut;
    hkQuaternion rotationLSOut;
    hkVector4 scaleLSOut;
    void *pSkeletonMemory;
};
class BSCyclicBlendTransitionGenerator : public hkbGenerator
{
public:
    virtual ~BSCyclicBlendTransitionGenerator();
    struct hkbGeneratorhkbGenerator *pBlenderGenerator;
    struct hkbEventProperty EventToFreezeBlendValue;
    struct hkbEventProperty EventToCrossBlend;
    float fBlendParameter;
    float fTransitionDuration;
    int8 eBlendCurve;
    void *pTransitionBlenderGenerator;
    void *pTransitionEffect;
    int8 currentMode;
};
class BSDecomposeVectorModifier : public hkbModifier
{
public:
    virtual ~BSDecomposeVectorModifier();
    hkVector4 vector;
    float x;
    float y;
    float z;
    float w;
};
class BSDirectAtModifier : public hkbModifier
{
public:
    virtual ~BSDirectAtModifier();
    bool directAtTarget;
    int16 sourceBoneIndex;
    int16 startBoneIndex;
    int16 endBoneIndex;
    float limitHeadingDegrees;
    float limitPitchDegrees;
    float offsetHeadingDegrees;
    float offsetPitchDegrees;
    float onGain;
    float offGain;
    hkVector4 targetLocation;
    uint32 userInfo;
    bool directAtCamera;
    float directAtCameraX;
    float directAtCameraY;
    float directAtCameraZ;
    bool active;
    float currentHeadingOffset;
    float currentPitchOffset;
    float timeStep;
    void *pSkeletonMemory;
    bool hasTarget;
    hkVector4 directAtTargetLocation;
    hkArray<void> boneChainIndices;
};
class BSDistTriggerModifier : public hkbModifier
{
public:
    virtual ~BSDistTriggerModifier();
    hkVector4 targetPosition;
    float distance;
    float distanceTrigger;
    struct hkbEventProperty triggerEvent;
};
class BSEventEveryNEventsModifier : public hkbModifier
{
public:
    virtual ~BSEventEveryNEventsModifier();
    struct hkbEventProperty eventToCheckFor;
    struct hkbEventProperty eventToSend;
    int8 numberOfEventsBeforeSend;
    int8 minimumNumberOfEventsBeforeSend;
    bool randomizeNumberOfEvents;
    int32 numberOfEventsSeen;
    int8 calculatedNumberOfEventsBeforeSend;
};
class BSEventOnDeactivateModifier : public hkbModifier
{
public:
    virtual ~BSEventOnDeactivateModifier();
    struct hkbEventProperty event;
};
class BSEventOnFalseToTrueModifier : public hkbModifier
{
public:
    virtual ~BSEventOnFalseToTrueModifier();
    bool bEnableEvent1;
    bool bVariableToTest1;
    struct hkbEventProperty EventToSend1;
    bool bEnableEvent2;
    bool bVariableToTest2;
    struct hkbEventProperty EventToSend2;
    bool bEnableEvent3;
    bool bVariableToTest3;
    struct hkbEventProperty EventToSend3;
    bool bSlot1ActivatedLastFrame;
    bool bSlot2ActivatedLastFrame;
    bool bSlot3ActivatedLastFrame;
};
class BSGetTimeStepModifier : public hkbModifier
{
public:
    virtual ~BSGetTimeStepModifier();
    float timeStep;
};
class BSIStateManagerModifier : public hkbModifier
{
public:
    virtual ~BSIStateManagerModifier();
    int32 iStateVar;
    hkArray<struct BSIStateManagerModifierBSiStateData> stateData;
    struct BSIStateManagerModifierBSIStateManagerStateListener myStateListener;
};
class BSIStateManagerModifierBSIStateManagerStateListener : public hkbStateListener
{
public:
    virtual ~BSIStateManagerModifierBSIStateManagerStateListener();
    void *pStateManager;
};
class BSInterpValueModifier : public hkbModifier
{
public:
    virtual ~BSInterpValueModifier();
    float source;
    float target;
    float result;
    float gain;
    float timeStep;
};
class BSIsActiveModifier : public hkbModifier
{
public:
    virtual ~BSIsActiveModifier();
    bool bIsActive0;
    bool bInvertActive0;
    bool bIsActive1;
    bool bInvertActive1;
    bool bIsActive2;
    bool bInvertActive2;
    bool bIsActive3;
    bool bInvertActive3;
    bool bIsActive4;
    bool bInvertActive4;
};
class BSLimbIKModifier : public hkbModifier
{
public:
    virtual ~BSLimbIKModifier();
    float limitAngleDegrees;
    float currentAngle;
    int16 startBoneIndex;
    int16 endBoneIndex;
    float gain;
    float boneRadius;
    float castOffset;
    float timeStep;
    void *pSkeletonMemory;
};
class BSLookAtModifier : public hkbModifier
{
public:
    virtual ~BSLookAtModifier();
    bool lookAtTarget;
    hkArray<struct BSLookAtModifierBoneData> bones;
    hkArray<struct BSLookAtModifierBoneData> eyeBones;
    float limitAngleDegrees;
    float limitAngleThresholdDegrees;
    bool continueLookOutsideOfLimit;
    float onGain;
    float offGain;
    bool useBoneGains;
    hkVector4 targetLocation;
    bool targetOutsideLimits;
    struct hkbEventProperty targetOutOfLimitEvent;
    bool lookAtCamera;
    float lookAtCameraX;
    float lookAtCameraY;
    float lookAtCameraZ;
    float timeStep;
    bool ballBonesValid;
    void *pSkeletonMemory;
};
class BSModifyOnceModifier : public hkbModifier
{
public:
    virtual ~BSModifyOnceModifier();
    struct hkbModifierhkbModifier *pOnActivateModifier;
    struct hkbModifierhkbModifier *pOnDeactivateModifier;
};
class BSOffsetAnimationGenerator : public hkbGenerator
{
public:
    virtual ~BSOffsetAnimationGenerator();
    struct hkbGeneratorhkbGenerator *pDefaultGenerator;
    struct hkbGeneratorhkbGenerator *pOffsetClipGenerator;
    float fOffsetVariable;
    float fOffsetRangeStart;
    float fOffsetRangeEnd;
    hkArray<void> BoneOffsetA;
    hkArray<void> BoneIndexA;
    float fCurrentPercentage;
    uint32 iCurrentFrame;
    bool bZeroOffset;
    bool bOffsetValid;
};
class BSPassByTargetTriggerModifier : public hkbModifier
{
public:
    virtual ~BSPassByTargetTriggerModifier();
    hkVector4 targetPosition;
    float radius;
    hkVector4 movementDirection;
    struct hkbEventProperty triggerEvent;
    bool targetPassed;
};
class BSRagdollContactListenerModifier : public hkbModifier
{
public:
    virtual ~BSRagdollContactListenerModifier();
    struct hkbEventProperty contactEvent;
    struct hkbBoneIndexArrayhkbBoneIndexArray *bones;
    bool throwEvent;
    hkArray<void *> ragdollRigidBodies;
};
class BSSpeedSamplerModifier : public hkbModifier
{
public:
    virtual ~BSSpeedSamplerModifier();
    int32 state;
    float direction;
    float goalSpeed;
    float speedOut;
};
class BSSynchronizedClipGenerator : public hkbGenerator
{
public:
    virtual ~BSSynchronizedClipGenerator();
    struct hkbGeneratorhkbGenerator *pClipGenerator;
    char *SyncAnimPrefix;
    bool bSyncClipIgnoreMarkPlacement;
    float fGetToMarkTime;
    float fMarkErrorThreshold;
    bool bLeadCharacter;
    bool bReorientSupportChar;
    bool bApplyMotionFromRoot;
    void *pSyncScene;
    hkQsTransform StartMarkWS;
    hkQsTransform EndMarkWS;
    hkQsTransform StartMarkMS;
    float fCurrentLerp;
    void *pLocalSyncBinding;
    void *pEventMap;
    int16 sAnimationBindingIndex;
    bool bAtMark;
    bool bAllCharactersInScene;
    bool bAllCharactersAtMarks;
};
class BSTimerModifier : public hkbModifier
{
public:
    virtual ~BSTimerModifier();
    float alarmTimeSeconds;
    struct hkbEventProperty alarmEvent;
    bool resetAlarm;
    float secondsElapsed;
};
class BSTweenerModifier : public hkbModifier
{
public:
    virtual ~BSTweenerModifier();
    bool tweenPosition;
    bool tweenRotation;
    bool useTweenDuration;
    float tweenDuration;
    hkVector4 targetPosition;
    hkQuaternion targetRotation;
    float duration;
    hkQsTransform startTransform;
    float time;
};
class BSiStateTaggingGenerator : public hkbGenerator
{
public:
    virtual ~BSiStateTaggingGenerator();
    struct hkbGeneratorhkbGenerator *pDefaultGenerator;
    int32 iStateToSetAs;
    int32 iPriority;
};
struct hkAabb
{
public:
    hkVector4 min;
    hkVector4 max;
};
struct hkAabbHalf
{
public:
    uint16 data;
    uint16 extras;
};
struct hkAabbUint32
{
public:
    uint32 min;
    uint8 expansionMin;
    uint8 expansionShift;
    uint32 max;
    uint8 expansionMax;
    uint8 shapeKeyByte;
};
struct hkArrayTypeAttribute
{
public:
    int8 type;
};
struct hkBitField
{
public:
    hkArray<uint32> words;
    int32 numBits;
};
struct hkClass
{
public:
    char *name;
    struct hkClasshkClass *parent;
    int32 objectSize;
    int32 numImplementedInterfaces;
    struct hkClassEnum *declaredEnums;
    struct hkClassMember *declaredMembers;
    void *defaults;
    struct hkCustomAttributeshkCustomAttributes *attributes;
    hkFlags flags;
    int32 describedVersion;
};
struct hkClassEnum
{
public:
    char *name;
    struct hkClassEnumItem *items;
    struct hkCustomAttributeshkCustomAttributes *attributes;
    hkFlags flags;
};
struct hkClassEnumItem
{
public:
    int32 value;
    char *name;
};
struct hkClassMember
{
public:
    char *name;
    struct hkClasshkClass *hkClass;
    struct hkClassEnumhkClassEnum *hkClassEnum;
    uint8 type;
    uint8 subtype;
    int16 cArraySize;
    hkFlags flags;
    uint16 offset;
    struct hkCustomAttributeshkCustomAttributes *attributes;
};
struct hkColor
{
public:
};
struct hkContactPoint
{
public:
    hkVector4 position;
    hkVector4 separatingNormal;
};
struct hkContactPointMaterial
{
public:
    uint64 userData;
    uint8 friction;
    uint8 restitution;
    uint8 maxImpulse;
    uint8 flags;
};
struct hkCustomAttributes
{
public:
    struct hkCustomAttributesAttribute *attributes;
};
struct hkCustomAttributesAttribute
{
public:
    char *name;
    hkVariant value;
};
struct hkDataObjectTypeAttribute
{
public:
    char *typeName;
};
struct hkDescriptionAttribute
{
public:
    char *string;
};
struct hkDocumentationAttribute
{
public:
    char *docsSectionTag;
};
struct hkGeometry
{
public:
    hkArray<hkVector4> vertices;
    hkArray<struct hkGeometryTriangle> triangles;
};
struct hkGeometryTriangle
{
public:
    int32 a;
    int32 b;
    int32 c;
    int32 material;
};
struct hkGizmoAttribute
{
public:
    bool visible;
    char *label;
    int8 type;
};
struct hkHalf8
{
public:
    hkHalf quad;
};
struct hkLinkAttribute
{
public:
    int8 type;
};
struct hkMemoryTrackerAttribute
{
public:
};
struct hkModelerNodeTypeAttribute
{
public:
    int8 type;
};
struct hkMotionState
{
public:
    hkTransform transform;
    struct hkSweptTransform sweptTransform;
    hkVector4 deltaAngle;
    float objectRadius;
    hkHalf linearDamping;
    hkHalf angularDamping;
    hkHalf timeFactor;
    uint8 maxLinearVelocity;
    uint8 maxAngularVelocity;
    uint8 deactivationClass;
};
struct hkMultiThreadCheck
{
public:
    uint32 threadId;
    int32 stackTraceId;
    uint16 markCount;
    uint16 markBitStack;
};
struct hkPackedVector3
{
public:
    int16 values;
};
struct hkPostFinishAttribute
{
public:
    void *postFinishFunction;
};
struct hkQTransform
{
public:
    hkQuaternion rotation;
    hkVector4 translation;
};
struct hkRangeInt32Attribute
{
public:
    int32 absmin;
    int32 absmax;
    int32 softmin;
    int32 softmax;
};
struct hkRangeRealAttribute
{
public:
    float absmin;
    float absmax;
    float softmin;
    float softmax;
};
struct hkReflectedFileAttribute
{
public:
    char *value;
};
struct hkSemanticsAttribute
{
public:
    int8 type;
};
struct hkSphere
{
public:
    hkVector4 pos;
};
struct hkSweptTransform
{
public:
    hkVector4 centerOfMass0;
    hkVector4 centerOfMass1;
    hkQuaternion rotation0;
    hkQuaternion rotation1;
    hkVector4 centerOfMassLocal;
};
struct hkTraceStreamTitle
{
public:
    char value;
};
struct hkTrackerSerializableScanSnapshotAllocation
{
public:
    uint64 start;
    uint64 size;
    int32 traceId;
};
struct hkTrackerSerializableScanSnapshotBlock
{
public:
    int32 typeIndex;
    uint64 start;
    uint64 size;
    int32 arraySize;
    int32 startReferenceIndex;
    int32 numReferences;
};
struct hkUiAttribute
{
public:
    bool visible;
    int8 hideInModeler;
    char *label;
    char *group;
    char *hideBaseClassMembers;
    bool endGroup;
    bool endGroup2;
    bool advanced;
};
struct hkMonitorStreamColorTableColorPair
{
public:
    hkStringPtr colorName;
    uint32 color;
};
struct hkMonitorStreamFrameInfo
{
public:
    hkStringPtr heading;
    int32 indexOfTimer0;
    int32 indexOfTimer1;
    uint32 absoluteTimeCounter;
    float timerFactor0;
    float timerFactor1;
    int32 threadId;
    int32 frameStreamStart;
    int32 frameStreamEnd;
};
struct hkMonitorStreamStringMap
{
public:
    hkArray<struct hkMonitorStreamStringMapStringMap> map;
};
struct hkMonitorStreamStringMapStringMap
{
public:
    uint64 id;
    hkStringPtr string;
};
struct hkMemoryResourceHandleExternalLink
{
public:
    hkStringPtr memberName;
    hkStringPtr externalId;
};
struct hkPackfileHeader
{
public:
    int32 magic;
    int32 userTag;
    int32 fileVersion;
    uint8 layoutRules;
    int32 numSections;
    int32 contentsSectionIndex;
    int32 contentsSectionOffset;
    int32 contentsClassNameSectionIndex;
    int32 contentsClassNameSectionOffset;
    char contentsVersion;
    int32 flags;
    int32 pad;
};
struct hkPackfileSectionHeader
{
public:
    char sectionTag;
    char nullByte;
    int32 absoluteDataStart;
    int32 localFixupsOffset;
    int32 globalFixupsOffset;
    int32 virtualFixupsOffset;
    int32 exportsOffset;
    int32 importsOffset;
    int32 endOffset;
};
struct hkRootLevelContainer
{
public:
    hkArray<struct hkRootLevelContainerNamedVariant> namedVariants;
};
struct hkRootLevelContainerNamedVariant
{
public:
    hkStringPtr name;
    hkStringPtr className;
    struct hkReferencedObjecthkReferencedObject *variant;
};
struct hkxAttribute
{
public:
    hkStringPtr name;
    struct hkReferencedObjecthkReferencedObject *value;
};
struct hkxAttributeGroup
{
public:
    hkStringPtr name;
    hkArray<struct hkxAttribute> attributes;
};
struct hkxEnumItem
{
public:
    int32 value;
    hkStringPtr name;
};
struct hkxEnvironmentVariable
{
public:
    hkStringPtr name;
    hkStringPtr value;
};
struct hkxMaterialProperty
{
public:
    uint32 key;
    uint32 value;
};
struct hkxMaterialTextureStage
{
public:
    struct hkReferencedObjecthkReferencedObject *texture;
    int32 usageHint;
    int32 tcoordChannel;
};
struct hkxNodeAnnotationData
{
public:
    float time;
    hkStringPtr description;
};
struct hkxVertexBufferVertexData
{
public:
    hkArray<hkVector4> vectorData;
    hkArray<float> floatData;
    hkArray<uint32> uint32Data;
    hkArray<uint16> uint16Data;
    hkArray<uint8> uint8Data;
    uint32 numVerts;
    uint32 vectorStride;
    uint32 floatStride;
    uint32 uint32Stride;
    uint32 uint16Stride;
    uint32 uint8Stride;
};
struct hkxVertexDescription
{
public:
    hkArray<struct hkxVertexDescriptionElementDecl> decls;
};
struct hkxVertexDescriptionElementDecl
{
public:
    uint32 byteOffset;
    uint16 type;
    uint16 usage;
    uint32 byteStride;
    uint8 numElements;
};
struct hkMeshBoneIndexMapping
{
public:
    hkArray<int16> mapping;
};
struct hkMeshSection
{
public:
    uint8 primitiveType;
    int32 numPrimitives;
    int32 numIndices;
    int32 vertexStartIndex;
    int32 transformIndex;
    uint8 indexType;
    void *indices;
    struct hkMeshVertexBufferhkMeshVertexBuffer *vertexBuffer;
    struct hkMeshMaterialhkMeshMaterial *material;
    int32 sectionIndex;
};
struct hkMeshSectionCinfo
{
public:
    struct hkMeshVertexBufferhkMeshVertexBuffer *vertexBuffer;
    struct hkMeshMaterialhkMeshMaterial *material;
    uint8 primitiveType;
    int32 numPrimitives;
    uint8 indexType;
    void *indices;
    int32 vertexStartIndex;
    int32 transformIndex;
};
struct hkMultipleVertexBufferElementInfo
{
public:
    uint8 vertexBufferIndex;
    uint8 elementIndex;
};
struct hkMultipleVertexBufferLockedElement
{
public:
    uint8 vertexBufferIndex;
    uint8 elementIndex;
    uint8 lockedBufferIndex;
    uint8 vertexFormatIndex;
    uint8 lockFlags;
    uint8 outputBufferIndex;
    int8 emulatedIndex;
};
struct hkMultipleVertexBufferVertexBufferInfo
{
public:
    struct hkMeshVertexBufferhkMeshVertexBuffer *vertexBuffer;
    void *lockedVertices;
    bool isLocked;
};
struct hkVertexFormat
{
public:
    struct hkVertexFormatElement elements;
    int32 numElements;
};
struct hkVertexFormatElement
{
public:
    uint8 dataType;
    uint8 numValues;
    uint8 usage;
    uint8 subUsage;
    hkFlags flags;
    uint8 pad;
};
struct hkpMassProperties
{
public:
    float volume;
    float mass;
    hkVector4 centerOfMass;
    hkMatrix3 inertiaTensor;
};
struct hkpAgent1nSector
{
public:
    uint32 bytesAllocated;
    uint32 pad0;
    uint32 pad1;
    uint32 pad2;
    uint8 data;
};
struct hkpBroadPhaseHandle
{
public:
    uint32 id;
};
struct hkpCdBody
{
public:
    struct hkpShapehkpShape *shape;
    uint32 shapeKey;
    void *motion;
    struct hkpCdBodyhkpCdBody *parent;
};
struct hkpCollidable
{
public:
    int8 ownerOffset;
    uint8 forceCollideOntoPpu;
    uint16 shapeSizeOnSpu;
    struct hkpTypedBroadPhaseHandle broadPhaseHandle;
    struct hkpCollidableBoundingVolumeData boundingVolumeData;
    float allowedPenetrationDepth;
};
struct hkpCollidableBoundingVolumeData
{
public:
    uint32 min;
    uint8 expansionMin;
    uint8 expansionShift;
    uint32 max;
    uint8 expansionMax;
    uint8 padding;
    uint16 numChildShapeAabbs;
    uint16 capacityChildShapeAabbs;
    void *childShapeAabbs;
    void *childShapeKeys;
};
struct hkpLinkedCollidable
{
public:
    hkArray<void> collisionEntries;
};
struct hkpListShapeChildInfo
{
public:
    struct hkpShapehkpShape *shape;
    uint32 collisionFilterInfo;
    int32 shapeSize;
    int32 numChildShapes;
};
struct hkpMeshMaterial
{
public:
    uint32 filterInfo;
};
struct hkpMultiRayShapeRay
{
public:
    hkVector4 start;
    hkVector4 end;
};
struct hkpNamedMeshMaterial
{
public:
    hkStringPtr name;
};
struct hkpTypedBroadPhaseHandle
{
public:
    int8 type;
    int8 ownerOffset;
    int8 objectQualityType;
    uint32 collisionFilterInfo;
};
struct hkpWeldingUtility
{
public:
};
struct hkpCompressedMeshShapeBigTriangle
{
public:
    uint16 a;
    uint16 b;
    uint16 c;
    uint32 material;
    uint16 weldingInfo;
    uint16 transformIndex;
};
struct hkpCompressedMeshShapeChunk
{
public:
    hkVector4 offset;
    hkArray<uint16> vertices;
    hkArray<uint16> indices;
    hkArray<uint16> stripLengths;
    hkArray<uint16> weldingInfo;
    uint32 materialInfo;
    uint16 reference;
    uint16 transformIndex;
};
struct hkpCompressedMeshShapeConvexPiece
{
public:
    hkVector4 offset;
    hkArray<uint16> vertices;
    hkArray<uint8> faceVertices;
    hkArray<uint16> faceOffsets;
    uint16 reference;
    uint16 transformIndex;
};
struct hkpConvexVerticesShapeFourVectors
{
public:
    hkVector4 x;
    hkVector4 y;
    hkVector4 z;
};
struct hkpExtendedMeshShapeShapesSubpart
{
public:
    hkArray<hkpConvexShape *> childShapes;
    hkQuaternion rotation;
    hkVector4 translation;
};
struct hkpExtendedMeshShapeSubpart
{
public:
    int8 type;
    int8 materialIndexStridingType;
    int16 materialStriding;
    void *materialIndexBase;
    uint16 materialIndexStriding;
    uint16 numMaterials;
    void *materialBase;
    uint64 userData;
};
struct hkpExtendedMeshShapeTrianglesSubpart
{
public:
    int32 numTriangleShapes;
    void *vertexBase;
    int32 numVertices;
    void *indexBase;
    uint16 vertexStriding;
    int32 triangleOffset;
    uint16 indexStriding;
    int8 stridingType;
    int8 flipAlternateTriangles;
    hkVector4 extrusion;
    hkQsTransform transform;
};
struct hkpStorageExtendedMeshShapeMaterial
{
public:
    hkHalf restitution;
    hkHalf friction;
    uint64 userData;
};
struct hkpMeshShapeSubpart
{
public:
    void *vertexBase;
    int32 vertexStriding;
    int32 numVertices;
    void *indexBase;
    int8 stridingType;
    int8 materialIndexStridingType;
    int32 indexStriding;
    int32 flipAlternateTriangles;
    int32 numTriangles;
    void *materialIndexBase;
    int32 materialIndexStriding;
    void *materialBase;
    int32 materialStriding;
    int32 numMaterials;
    int32 triangleOffset;
};
struct hkpSimpleMeshShapeTriangle
{
public:
    int32 a;
    int32 b;
    int32 c;
    uint16 weldingInfo;
};
struct hkpBallAndSocketConstraintDataAtoms
{
public:
    struct hkpSetLocalTranslationsConstraintAtom pivots;
    struct hkpSetupStabilizationAtom setupStabilization;
    struct hkpBallSocketConstraintAtom ballSocket;
};
struct hkpBallSocketChainDataConstraintInfo
{
public:
    hkVector4 pivotInA;
    hkVector4 pivotInB;
};
struct hkpCogWheelConstraintDataAtoms
{
public:
    struct hkpSetLocalTransformsConstraintAtom transforms;
    struct hkpCogWheelConstraintAtom cogWheels;
};
struct hkpConstraintInstanceSmallArraySerializeOverrideType
{
public:
    void *data;
    uint16 size;
    uint16 capacityAndFlags;
};
struct hkpEntityExtendedListeners
{
public:
    struct hkpEntitySmallArraySerializeOverrideType activationListeners;
    struct hkpEntitySmallArraySerializeOverrideType entityListeners;
};
struct hkpEntitySmallArraySerializeOverrideType
{
public:
    void *data;
    uint16 size;
    uint16 capacityAndFlags;
};
struct hkpEntitySpuCollisionCallback
{
public:
    void *util;
    uint16 capacity;
    uint8 eventFilter;
    uint8 userFilter;
};
struct hkpGenericConstraintDataScheme
{
public:
    struct hkpGenericConstraintDataSchemeConstraintInfo info;
    hkArray<hkVector4> data;
    hkArray<int32> commands;
    hkArray<void *> modifiers;
    hkArray<hkpConstraintMotor *> motors;
};
struct hkpGenericConstraintDataSchemeConstraintInfo
{
public:
    int32 maxSizeOfSchema;
    int32 sizeOfSchemas;
    int32 numSolverResults;
    int32 numSolverElemTemps;
};
struct hkpHingeConstraintDataAtoms
{
public:
    struct hkpSetLocalTransformsConstraintAtom transforms;
    struct hkpSetupStabilizationAtom setupStabilization;
    struct hkp2dAngConstraintAtom m_2dAng;
    struct hkpBallSocketConstraintAtom ballSocket;
};
struct hkpHingeLimitsDataAtoms
{
public:
    struct hkpSetLocalRotationsConstraintAtom rotations;
    struct hkpAngLimitConstraintAtom angLimit;
    struct hkp2dAngConstraintAtom m_2dAng;
};
struct hkpLimitedHingeConstraintDataAtoms
{
public:
    struct hkpSetLocalTransformsConstraintAtom transforms;
    struct hkpSetupStabilizationAtom setupStabilization;
    struct hkpAngMotorConstraintAtom angMotor;
    struct hkpAngFrictionConstraintAtom angFriction;
    struct hkpAngLimitConstraintAtom angLimit;
    struct hkp2dAngConstraintAtom m_2dAng;
    struct hkpBallSocketConstraintAtom ballSocket;
};
struct hkpMaterial
{
public:
    int8 responseType;
    hkHalf rollingFrictionMultiplier;
    float friction;
    float restitution;
};
struct hkpPairCollisionFilterMapPairFilterKeyOverrideType
{
public:
    void *elem;
    int32 numElems;
    int32 hashMod;
};
struct hkpPointToPlaneConstraintDataAtoms
{
public:
    struct hkpSetLocalTransformsConstraintAtom transforms;
    struct hkpLinConstraintAtom lin;
};
struct hkpPrismaticConstraintDataAtoms
{
public:
    struct hkpSetLocalTransformsConstraintAtom transforms;
    struct hkpLinMotorConstraintAtom motor;
    struct hkpLinFrictionConstraintAtom friction;
    struct hkpAngConstraintAtom ang;
    struct hkpLinConstraintAtom lin0;
    struct hkpLinConstraintAtom lin1;
    struct hkpLinLimitConstraintAtom linLimit;
};
struct hkpProperty
{
public:
    uint32 key;
    uint32 alignmentPadding;
    struct hkpPropertyValue value;
};
struct hkpPropertyValue
{
public:
    uint64 data;
};
struct hkpPulleyConstraintDataAtoms
{
public:
    struct hkpSetLocalTranslationsConstraintAtom translations;
    struct hkpPulleyConstraintAtom pulley;
};
struct hkpRackAndPinionConstraintDataAtoms
{
public:
    struct hkpSetLocalTransformsConstraintAtom transforms;
    struct hkpRackAndPinionConstraintAtom rackAndPinion;
};
struct hkpRagdollConstraintDataAtoms
{
public:
    struct hkpSetLocalTransformsConstraintAtom transforms;
    struct hkpSetupStabilizationAtom setupStabilization;
    struct hkpRagdollMotorConstraintAtom ragdollMotors;
    struct hkpAngFrictionConstraintAtom angFriction;
    struct hkpTwistLimitConstraintAtom twistLimit;
    struct hkpConeLimitConstraintAtom coneLimit;
    struct hkpConeLimitConstraintAtom planesLimit;
    struct hkpBallSocketConstraintAtom ballSocket;
};
struct hkpRagdollLimitsDataAtoms
{
public:
    struct hkpSetLocalRotationsConstraintAtom rotations;
    struct hkpTwistLimitConstraintAtom twistLimit;
    struct hkpConeLimitConstraintAtom coneLimit;
    struct hkpConeLimitConstraintAtom planesLimit;
};
struct hkpRotationalConstraintDataAtoms
{
public:
    struct hkpSetLocalRotationsConstraintAtom rotations;
    struct hkpAngConstraintAtom ang;
};
struct hkpSimpleShapePhantomCollisionDetail
{
public:
    struct hkpCollidablehkpCollidable *collidable;
};
struct hkpStiffSpringChainDataConstraintInfo
{
public:
    hkVector4 pivotInA;
    hkVector4 pivotInB;
    float springLength;
};
struct hkpStiffSpringConstraintDataAtoms
{
public:
    struct hkpSetLocalTranslationsConstraintAtom pivots;
    struct hkpStiffSpringConstraintAtom spring;
};
struct hkpWheelConstraintDataAtoms
{
public:
    struct hkpSetLocalTransformsConstraintAtom suspensionBase;
    struct hkpLinLimitConstraintAtom lin0Limit;
    struct hkpLinSoftConstraintAtom lin0Soft;
    struct hkpLinConstraintAtom lin1;
    struct hkpLinConstraintAtom lin2;
    struct hkpSetLocalRotationsConstraintAtom steeringBase;
    struct hkp2dAngConstraintAtom m_2dAng;
};
struct hkpPoweredChainDataConstraintInfo
{
public:
    hkVector4 pivotInA;
    hkVector4 pivotInB;
    hkQuaternion aTc;
    hkQuaternion bTc;
    struct hkpConstraintMotorhkpConstraintMotor *motors;
    bool switchBodies;
};
struct hkp2dAngConstraintAtom
{
public:
    uint8 freeRotationAxis;
};
struct hkpAngConstraintAtom
{
public:
    uint8 firstConstrainedAxis;
    uint8 numConstrainedAxes;
};
struct hkpAngFrictionConstraintAtom
{
public:
    uint8 isEnabled;
    uint8 firstFrictionAxis;
    uint8 numFrictionAxes;
    float maxFrictionTorque;
};
struct hkpAngLimitConstraintAtom
{
public:
    uint8 isEnabled;
    uint8 limitAxis;
    float minAngle;
    float maxAngle;
    float angularLimitsTauFactor;
};
struct hkpAngMotorConstraintAtom
{
public:
    bool isEnabled;
    uint8 motorAxis;
    int16 initializedOffset;
    int16 previousTargetAngleOffset;
    int16 correspondingAngLimitSolverResultOffset;
    float targetAngle;
    struct hkpConstraintMotorhkpConstraintMotor *motor;
};
struct hkpBallSocketConstraintAtom
{
public:
    uint8 solvingMethod;
    uint8 bodiesToNotify;
    uint8 velocityStabilizationFactor;
    float maxImpulse;
    float inertiaStabilizationFactor;
};
struct hkpBridgeAtoms
{
public:
    struct hkpBridgeConstraintAtom bridgeAtom;
};
struct hkpBridgeConstraintAtom
{
public:
    void *buildJacobianFunc;
    struct hkpConstraintDatahkpConstraintData *constraintData;
};
struct hkpCenterOfMassChangerModifierConstraintAtom
{
public:
    hkVector4 displacementA;
    hkVector4 displacementB;
};
struct hkpCogWheelConstraintAtom
{
public:
    float cogWheelRadiusA;
    float cogWheelRadiusB;
    bool isScrew;
    int8 memOffsetToInitialAngleOffset;
    int8 memOffsetToPrevAngle;
    int8 memOffsetToRevolutionCounter;
};
struct hkpConeLimitConstraintAtom
{
public:
    uint8 isEnabled;
    uint8 twistAxisInA;
    uint8 refAxisInB;
    uint8 angleMeasurementMode;
    uint8 memOffsetToAngleOffset;
    float minAngle;
    float maxAngle;
    float angularLimitsTauFactor;
};
struct hkpConstraintAtom
{
public:
    uint16 type;
};
struct hkpIgnoreModifierConstraintAtom
{
public:
};
struct hkpLinConstraintAtom
{
public:
    uint8 axisIndex;
};
struct hkpLinFrictionConstraintAtom
{
public:
    uint8 isEnabled;
    uint8 frictionAxis;
    float maxFrictionForce;
};
struct hkpLinLimitConstraintAtom
{
public:
    uint8 axisIndex;
    float min;
    float max;
};
struct hkpLinMotorConstraintAtom
{
public:
    bool isEnabled;
    uint8 motorAxis;
    int16 initializedOffset;
    int16 previousTargetPositionOffset;
    float targetPosition;
    struct hkpConstraintMotorhkpConstraintMotor *motor;
};
struct hkpLinSoftConstraintAtom
{
public:
    uint8 axisIndex;
    float tau;
    float damping;
};
struct hkpMassChangerModifierConstraintAtom
{
public:
    hkVector4 factorA;
    hkVector4 factorB;
};
struct hkpModifierConstraintAtom
{
public:
    uint16 modifierAtomSize;
    uint16 childSize;
    struct hkpConstraintAtomhkpConstraintAtom *child;
    uint32 pad;
};
struct hkpMovingSurfaceModifierConstraintAtom
{
public:
    hkVector4 velocity;
};
struct hkpOverwritePivotConstraintAtom
{
public:
    uint8 copyToPivotBFromPivotA;
};
struct hkpPulleyConstraintAtom
{
public:
    hkVector4 fixedPivotAinWorld;
    hkVector4 fixedPivotBinWorld;
    float ropeLength;
    float leverageOnBodyB;
};
struct hkpRackAndPinionConstraintAtom
{
public:
    float pinionRadiusOrScrewPitch;
    bool isScrew;
    int8 memOffsetToInitialAngleOffset;
    int8 memOffsetToPrevAngle;
    int8 memOffsetToRevolutionCounter;
};
struct hkpRagdollMotorConstraintAtom
{
public:
    bool isEnabled;
    int16 initializedOffset;
    int16 previousTargetAnglesOffset;
    hkMatrix3 target_bRca;
    struct hkpConstraintMotorhkpConstraintMotor *motors;
};
struct hkpSetLocalRotationsConstraintAtom
{
public:
    hkRotation rotationA;
    hkRotation rotationB;
};
struct hkpSetLocalTransformsConstraintAtom
{
public:
    hkTransform transformA;
    hkTransform transformB;
};
struct hkpSetLocalTranslationsConstraintAtom
{
public:
    hkVector4 translationA;
    hkVector4 translationB;
};
struct hkpSetupStabilizationAtom
{
public:
    bool enabled;
    float maxAngle;
    uint8 padding;
};
struct hkpSimpleContactConstraintAtom
{
public:
    uint16 sizeOfAllAtoms;
    uint16 numContactPoints;
    uint16 numReservedContactPoints;
    uint8 numUserDatasForBodyA;
    uint8 numUserDatasForBodyB;
    uint8 contactPointPropertiesStriding;
    uint16 maxNumContactPoints;
    struct hkpSimpleContactConstraintDataInfo info;
};
struct hkpSimpleContactConstraintDataInfo
{
public:
    uint16 flags;
    uint16 index;
    float internalData0;
    hkHalf rollingFrictionMultiplier;
    hkHalf internalData1;
    uint32 data;
};
struct hkpSoftContactModifierConstraintAtom
{
public:
    float tau;
    float maxAcceleration;
};
struct hkpStiffSpringConstraintAtom
{
public:
    float length;
};
struct hkpTwistLimitConstraintAtom
{
public:
    uint8 isEnabled;
    uint8 twistAxis;
    uint8 refAxis;
    float minAngle;
    float maxAngle;
    float angularLimitsTauFactor;
};
struct hkpViscousSurfaceModifierConstraintAtom
{
public:
};
struct hkpMoppCodeCodeInfo
{
public:
    hkVector4 offset;
};
struct hkpMoppCodeReindexedTerminal
{
public:
    uint32 origShapeKey;
    uint32 reindexedShapeKey;
};
struct hkpPoweredChainMapperLinkInfo
{
public:
    int32 firstTargetIdx;
    int32 numTargets;
    struct hkpConstraintInstancehkpConstraintInstance *limitConstraint;
};
struct hkpPoweredChainMapperTarget
{
public:
    struct hkpPoweredChainDatahkpPoweredChainData *chain;
    int32 infoIndex;
};
struct hkpSerializedDisplayRbTransformsDisplayTransformPair
{
public:
    struct hkpRigidBodyhkpRigidBody *rb;
    hkTransform localToDisplay;
};
struct hkpSerializedSubTrack1nInfo
{
public:
    int32 sectorIndex;
    int32 offsetInSector;
};
struct hkpSerializedTrack1nInfo
{
public:
    hkArray<hkpAgent1nSector *> sectors;
    hkArray<hkpSerializedSubTrack1nInfo *> subTracks;
};
struct hkpTriggerVolumeEventInfo
{
public:
    uint64 sortValue;
    struct hkpRigidBodyhkpRigidBody *body;
    int32 operation;
};
struct hkaAnnotationTrack
{
public:
    hkStringPtr trackName;
    hkArray<struct hkaAnnotationTrackAnnotation> annotations;
};
struct hkaAnnotationTrackAnnotation
{
public:
    float time;
    hkStringPtr text;
};
struct hkaBone
{
public:
    hkStringPtr name;
    bool lockTranslation;
};
struct hkaDeltaCompressedAnimationQuantizationFormat
{
public:
    uint8 maxBitWidth;
    uint8 preserved;
    uint32 numD;
    uint32 offsetIdx;
    uint32 scaleIdx;
    uint32 bitWidthIdx;
};
struct hkaMeshBindingMapping
{
public:
    hkArray<int16> mapping;
};
struct hkaQuantizedAnimationTrackCompressionParams
{
public:
    float rotationTolerance;
    float translationTolerance;
    float scaleTolerance;
    float floatingTolerance;
};
struct hkaSkeletonLocalFrameOnBone
{
public:
    struct hkLocalFramehkLocalFrame *localFrame;
    int32 boneIndex;
};
struct hkaSkeletonMapperData
{
public:
    struct hkaSkeletonhkaSkeleton *skeletonA;
    struct hkaSkeletonhkaSkeleton *skeletonB;
    hkArray<struct hkaSkeletonMapperDataSimpleMapping> simpleMappings;
    hkArray<struct hkaSkeletonMapperDataChainMapping> chainMappings;
    hkArray<int16> unmappedBones;
    hkQsTransform extractedMotionMapping;
    bool keepUnmappedLocal;
    int32 mappingType;
};
struct hkaSkeletonMapperDataChainMapping
{
public:
    int16 startBoneA;
    int16 endBoneA;
    int16 startBoneB;
    int16 endBoneB;
    hkQsTransform startAFromBTransform;
    hkQsTransform endAFromBTransform;
};
struct hkaSkeletonMapperDataSimpleMapping
{
public:
    int16 boneA;
    int16 boneB;
    hkQsTransform aFromBTransform;
};
struct hkaSplineCompressedAnimationAnimationCompressionParams
{
public:
    uint16 maxFramesPerBlock;
    bool enableSampleSingleTracks;
};
struct hkaSplineCompressedAnimationTrackCompressionParams
{
public:
    float rotationTolerance;
    float translationTolerance;
    float scaleTolerance;
    float floatingTolerance;
    uint16 rotationDegree;
    uint16 translationDegree;
    uint16 scaleDegree;
    uint16 floatingDegree;
    uint8 rotationQuantizationType;
    uint8 translationQuantizationType;
    uint8 scaleQuantizationType;
    uint8 floatQuantizationType;
};
struct hkaWaveletCompressedAnimationCompressionParams
{
public:
    uint16 quantizationBits;
    uint16 blockSize;
    uint16 preserve;
    float truncProp;
    bool useOldStyleTruncation;
    float absolutePositionTolerance;
    float relativePositionTolerance;
    float rotationTolerance;
    float scaleTolerance;
    float absoluteFloatTolerance;
};
struct hkaWaveletCompressedAnimationQuantizationFormat
{
public:
    uint8 maxBitWidth;
    uint8 preserved;
    uint32 numD;
    uint32 offsetIdx;
    uint32 scaleIdx;
    uint32 bitWidthIdx;
};
struct hkaKeyFrameHierarchyUtility
{
public:
};
struct hkaKeyFrameHierarchyUtilityControlData
{
public:
    float hierarchyGain;
    float velocityDamping;
    float accelerationGain;
    float velocityGain;
    float positionGain;
    float positionMaxLinearVelocity;
    float positionMaxAngularVelocity;
    float snapGain;
    float snapMaxLinearVelocity;
    float snapMaxAngularVelocity;
    float snapMaxLinearDistance;
    float snapMaxAngularDistance;
};
struct hkbAttributeModifierAssignment
{
public:
    int32 attributeIndex;
    float attributeValue;
};
struct hkbBlendCurveUtils
{
public:
};
struct hkbBlenderGeneratorChildInternalState
{
public:
    bool isActive;
    bool syncNextFrame;
};
struct hkbBoolVariableSequencedDataSample
{
public:
    float time;
    bool value;
};
struct hkbCharacterControllerControlData
{
public:
    hkVector4 desiredVelocity;
    float verticalGain;
    float horizontalCatchUpGain;
    float maxVerticalSeparation;
    float maxHorizontalSeparation;
};
struct hkbCharacterDataCharacterControllerInfo
{
public:
    float capsuleHeight;
    float capsuleRadius;
    uint32 collisionFilterInfo;
    struct hkpCharacterControllerCinfohkpCharacterControllerCinfo *characterControllerCinfo;
};
struct hkbClipGeneratorEcho
{
public:
    float offsetLocalTime;
    float weight;
    float dwdt;
};
struct hkbClipTrigger
{
public:
    float localTime;
    struct hkbEventProperty event;
    bool relativeToEndOfClip;
    bool acyclic;
    bool isAnnotation;
};
struct hkbCompiledExpressionSetToken
{
public:
    float data;
    int8 type;
    int8 m_operator;
};
struct hkbContext
{
public:
    void *character;
    void *behavior;
    void *nodeToIndexMap;
    void *eventQueue;
    void *sharedEventQueue;
    struct hkbGeneratorOutputListenerhkbGeneratorOutputListener *generatorOutputListener;
    bool eventTriggeredTransition;
    void *world;
    void *attachmentManager;
    void *animationCache;
};
struct hkbDefaultMessageLog
{
public:
};
struct hkbEvaluateExpressionModifierInternalExpressionData
{
public:
    bool raisedEvent;
    bool wasTrueInPreviousFrame;
};
struct hkbEvent
{
public:
    void *sender;
};
struct hkbEventBase
{
public:
    int32 id;
    struct hkbEventPayloadhkbEventPayload *payload;
};
struct hkbEventInfo
{
public:
    hkFlags flags;
};
struct hkbEventProperty
{
public:
};
struct hkbEventRangeData
{
public:
    float upperBound;
    struct hkbEventProperty event;
    int8 eventMode;
};
struct hkbEventSequencedDataSequencedEvent
{
public:
    struct hkbEvent event;
    float time;
};
struct hkbExpressionData
{
public:
    hkStringPtr expression;
    int32 assignmentVariableIndex;
    int32 assignmentEventIndex;
    int8 eventMode;
    bool raisedEvent;
    bool wasTrueInPreviousFrame;
};
struct hkbFootIkControlData
{
public:
    struct hkbFootIkGains gains;
};
struct hkbFootIkControlsModifierLeg
{
public:
    hkVector4 groundPosition;
    struct hkbEventProperty ungroundedEvent;
    float verticalError;
    bool hitSomething;
    bool isPlantedMS;
};
struct hkbFootIkDriverInfoLeg
{
public:
    hkQuaternion prevAnkleRotLS;
    hkVector4 kneeAxisLS;
    hkVector4 footEndLS;
    float footPlantedAnkleHeightMS;
    float footRaisedAnkleHeightMS;
    float maxAnkleHeightMS;
    float minAnkleHeightMS;
    float maxKneeAngleDegrees;
    float minKneeAngleDegrees;
    float maxAnkleAngleDegrees;
    int16 hipIndex;
    int16 kneeIndex;
    int16 ankleIndex;
};
struct hkbFootIkGains
{
public:
    float onOffGain;
    float groundAscendingGain;
    float groundDescendingGain;
    float footPlantedGain;
    float footRaisedGain;
    float footUnlockGain;
    float worldFromModelFeedbackGain;
    float errorUpDownBias;
    float alignWorldFromModelGain;
    float hipOrientationGain;
    float maxKneeAngleDifference;
    float ankleOrientationGain;
};
struct hkbFootIkModifierInternalLegData
{
public:
    hkVector4 groundPosition;
    void *footIkSolver;
};
struct hkbFootIkModifierLeg
{
public:
    hkQsTransform originalAnkleTransformMS;
    hkQuaternion prevAnkleRotLS;
    hkVector4 kneeAxisLS;
    hkVector4 footEndLS;
    struct hkbEventProperty ungroundedEvent;
    float footPlantedAnkleHeightMS;
    float footRaisedAnkleHeightMS;
    float maxAnkleHeightMS;
    float minAnkleHeightMS;
    float maxKneeAngleDegrees;
    float minKneeAngleDegrees;
    float verticalError;
    float maxAnkleAngleDegrees;
    int16 hipIndex;
    int16 kneeIndex;
    int16 ankleIndex;
    bool hitSomething;
    bool isPlantedMS;
    bool isOriginalAnkleTransformMSSet;
};
struct hkbGeneratorSyncInfo
{
public:
    struct hkbGeneratorSyncInfoSyncPoint syncPoints;
    float baseFrequency;
    float localTime;
    float playbackSpeed;
    int8 numSyncPoints;
    bool isCyclic;
    bool isMirrored;
    bool isAdditive;
};
struct hkbGeneratorSyncInfoSyncPoint
{
public:
    int32 id;
    float time;
};
struct hkbHandIkControlData
{
public:
    hkVector4 targetPosition;
    hkQuaternion targetRotation;
    hkVector4 targetNormal;
    struct hkbHandlehkbHandle *targetHandle;
    float transformOnFraction;
    float normalOnFraction;
    float fadeInDuration;
    float fadeOutDuration;
    float extrapolationTimeStep;
    float handleChangeSpeed;
    int8 handleChangeMode;
    bool fixUp;
};
struct hkbHandIkControlsModifierHand
{
public:
    struct hkbHandIkControlData controlData;
    int32 handIndex;
    bool enable;
};
struct hkbHandIkDriverInfoHand
{
public:
    hkVector4 elbowAxisLS;
    hkVector4 backHandNormalLS;
    hkVector4 handOffsetLS;
    hkQuaternion handOrienationOffsetLS;
    float maxElbowAngleDegrees;
    float minElbowAngleDegrees;
    int16 shoulderIndex;
    int16 shoulderSiblingIndex;
    int16 elbowIndex;
    int16 elbowSiblingIndex;
    int16 wristIndex;
    bool enforceEndPosition;
    bool enforceEndRotation;
    hkStringPtr localFrameName;
};
struct hkbHandIkModifierHand
{
public:
    hkVector4 elbowAxisLS;
    hkVector4 backHandNormalLS;
    hkVector4 handOffsetLS;
    hkQuaternion handOrienationOffsetLS;
    float maxElbowAngleDegrees;
    float minElbowAngleDegrees;
    int16 shoulderIndex;
    int16 shoulderSiblingIndex;
    int16 elbowIndex;
    int16 elbowSiblingIndex;
    int16 wristIndex;
    bool enforceEndPosition;
    bool enforceEndRotation;
    hkStringPtr localFrameName;
};
struct hkbIntVariableSequencedDataSample
{
public:
    float time;
    int32 value;
};
struct hkbKeyframeBonesModifierKeyframeInfo
{
public:
    hkVector4 keyframedPosition;
    hkQuaternion keyframedRotation;
    int16 boneIndex;
    bool isValid;
};
struct hkbMessageLog
{
public:
    void *messages;
    int32 maxMessages;
};
struct hkbPoweredRagdollControlData
{
public:
    float maxForce;
    float tau;
    float damping;
    float proportionalRecoveryVelocity;
    float constantRecoveryVelocity;
};
struct hkbProxyModifierProxyInfo
{
public:
    float dynamicFriction;
    float staticFriction;
    float keepContactTolerance;
    hkVector4 up;
    float keepDistance;
    float contactAngleSensitivity;
    uint32 userPlanes;
    float maxCharacterSpeedForSolver;
    float characterStrength;
    float characterMass;
    float maxSlope;
    float penetrationRecoverySpeed;
    int32 maxCastIterations;
    bool refreshManifoldInCheckSupport;
};
struct hkbRealVariableSequencedDataSample
{
public:
    float time;
    float value;
};
struct hkbRigidBodyRagdollControlData
{
public:
    struct hkaKeyFrameHierarchyUtilityControlData keyFrameHierarchyControlData;
    float durationToBlend;
};
struct hkbRoleAttribute
{
public:
    int16 role;
    hkFlags flags;
};
struct hkbSenseHandleModifierRange
{
public:
    struct hkbEventProperty event;
    float minDistance;
    float maxDistance;
    bool ignoreHandle;
};
struct hkbStateMachineActiveTransitionInfo
{
public:
    void *transitionEffect;
    struct hkbNodeInternalStateInfohkbNodeInternalStateInfo *transitionEffectInternalStateInfo;
    struct hkbStateMachineTransitionInfoReference transitionInfoReference;
    struct hkbStateMachineTransitionInfoReference transitionInfoReferenceForTE;
    int32 fromStateId;
    int32 toStateId;
    bool isReturnToPreviousState;
};
struct hkbStateMachineDelayedTransitionInfo
{
public:
    struct hkbStateMachineProspectiveTransitionInfo delayedTransition;
    float timeDelayed;
    bool isDelayedTransitionReturnToPreviousState;
    bool wasInAbutRangeLastFrame;
};
struct hkbStateMachineNestedStateMachineData
{
public:
    void *nestedStateMachine;
    void *eventIdMap;
};
struct hkbStateMachineProspectiveTransitionInfo
{
public:
    struct hkbStateMachineTransitionInfoReference transitionInfoReference;
    struct hkbStateMachineTransitionInfoReference transitionInfoReferenceForTE;
    int32 toStateId;
};
struct hkbStateMachineTimeInterval
{
public:
    int32 enterEventId;
    int32 exitEventId;
    float enterTime;
    float exitTime;
};
struct hkbStateMachineTransitionInfo
{
public:
    struct hkbStateMachineTimeInterval triggerInterval;
    struct hkbStateMachineTimeInterval initiateInterval;
    struct hkbTransitionEffecthkbTransitionEffect *transition;
    struct hkbConditionhkbCondition *condition;
    int32 eventId;
    int32 toStateId;
    int32 fromNestedStateId;
    int32 toNestedStateId;
    int16 priority;
    hkFlags flags;
};
struct hkbStateMachineTransitionInfoReference
{
public:
    int16 fromStateIndex;
    int16 transitionIndex;
    int16 stateMachineId;
};
struct hkbVariableBindingSetBinding
{
public:
    hkStringPtr memberPath;
    void *memberClass;
    int32 offsetInObjectPlusOne;
    int32 offsetInArrayPlusOne;
    int32 rootVariableIndex;
    int32 variableIndex;
    int8 bitIndex;
    int8 bindingType;
    uint8 memberType;
    int8 variableType;
    hkFlags flags;
};
struct hkbVariableInfo
{
public:
    struct hkbRoleAttribute role;
    int8 type;
};
struct hkbVariableValue
{
public:
    int32 value;
};
struct hkbWorldEnums
{
public:
};
struct hkbWorldFromModelModeData
{
public:
    int16 poseMatchingBone0;
    int16 poseMatchingBone1;
    int16 poseMatchingBone2;
    int8 mode;
};
struct hkVariableTweakingHelper
{
public:
    hkArray<struct hkVariableTweakingHelperBoolVariableInfo> boolVariableInfo;
    hkArray<struct hkVariableTweakingHelperIntVariableInfo> intVariableInfo;
    hkArray<struct hkVariableTweakingHelperRealVariableInfo> realVariableInfo;
    hkArray<struct hkVariableTweakingHelperVector4VariableInfo> vector4VariableInfo;
    void *behavior;
    hkArray<void> boolIndices;
    hkArray<void> intIndices;
    hkArray<void> realIndices;
    hkArray<void> vector4Indices;
};
struct hkVariableTweakingHelperBoolVariableInfo
{
public:
    hkStringPtr name;
    bool value;
    bool tweakOn;
};
struct hkVariableTweakingHelperIntVariableInfo
{
public:
    hkStringPtr name;
    int32 value;
    bool tweakOn;
};
struct hkVariableTweakingHelperRealVariableInfo
{
public:
    hkStringPtr name;
    float value;
    bool tweakOn;
};
struct hkVariableTweakingHelperVector4VariableInfo
{
public:
    hkStringPtr name;
    float x;
    float y;
    float z;
    float w;
    bool tweakOn;
};
struct hkbBalanceModifierStepInfo
{
public:
    int16 boneIndex;
    float fractionOfSolution;
};
struct hkbBehaviorInfoIdToNamePair
{
public:
    hkStringPtr behaviorName;
    hkStringPtr nodeName;
    uint8 toolType;
    int16 id;
};
struct hkbCatchFallModifierHand
{
public:
    int16 animShoulderIndex;
    int16 ragdollShoulderIndex;
    int16 ragdollAnkleIndex;
};
struct hkbCustomTestGeneratorStruck
{
public:
    bool hkBool;
    hkStringPtr string;
    int32 hkInt;
    int8 hkInt8;
    int16 hkInt16;
    int32 hkInt32;
    uint8 hkUint8;
    uint16 hkUint16;
    uint32 hkUint32;
    float hkReal;
    int8 mode_hkInt8;
    int16 mode_hkInt16;
    int32 mode_hkInt32;
    uint8 mode_hkUint8;
    uint16 mode_hkUint16;
    uint32 mode_hkUint32;
    hkFlags flags_hkInt8;
    hkFlags flags_hkInt16;
    hkFlags flags_hkInt32;
    hkFlags flags_hkUint8;
    hkFlags flags_hkUint16;
    hkFlags flags_hkUint32;
    struct hkbGeneratorhkbGenerator *generator1;
    struct hkbGeneratorhkbGenerator *generator2;
    struct hkbModifierhkbModifier *modifier1;
    struct hkbModifierhkbModifier *modifier2;
};
struct hkbDemoConfigStickVariableInfo
{
public:
    hkStringPtr variableName;
    float minValue;
    float maxValue;
    float minStickValue;
    float maxStickValue;
    int8 stickAxis;
    int8 stick;
    bool complimentVariableValue;
    bool negateVariableValue;
};
struct hkbDemoConfigTerrainInfo
{
public:
    hkStringPtr filename;
    int32 layer;
    int32 systemGroup;
    bool createDisplayObjects;
    struct hkpRigidBodyhkpRigidBody *terrainRigidBody;
};
struct hkbRadialSelectorGeneratorGeneratorInfo
{
public:
    struct hkbGeneratorhkbGenerator *generator;
    float angle;
    float radialSpeed;
};
struct hkbRadialSelectorGeneratorGeneratorPair
{
public:
    struct hkbRadialSelectorGeneratorGeneratorInfo generators;
    float minAngle;
    float maxAngle;
};
struct hkbReachModifierHand
{
public:
    hkVector4 targetOrSensingPosition;
    hkVector4 targetBackHandNormal;
    float sensingRadius;
    int16 boneIndex;
    int16 handIkTrackIndex;
};
struct hkbReachTowardTargetModifierHand
{
public:
    int16 shoulderIndex;
    bool isEnabled;
};
struct BSIStateManagerModifierBSiStateData
{
public:
    struct hkbGeneratorhkbGenerator *pStateMachine;
    int32 StateID;
    int32 iStateToSetAs;
};
struct BSLookAtModifierBoneData
{
public:
    int16 index;
    hkVector4 fwdAxisLS;
    float limitAngleDegrees;
    float onGain;
    float offGain;
    bool enabled;
    hkVector4 currentFwdAxisLS;
};
struct hkLocalFrame
{
public:
};
struct hkResourceBase
{
public:
};
struct hkResourceContainer
{
public:
};
struct hkResourceHandle
{
public:
};
struct hkMeshBody
{
public:
};
struct hkMeshMaterial
{
public:
};
struct hkMeshShape
{
public:
};
struct hkMeshTexture
{
public:
};
struct hkMeshVertexBuffer
{
public:
};
struct hkMoppBvTreeShapeBase
{
public:
    struct hkpMoppCodehkpMoppCode *code;
    void *moppData;
    uint32 moppDataSize;
    hkVector4 codeInfoCopy;
};
struct hkpBvTreeShape
{
public:
    uint8 bvTreeType;
};
struct hkpCollidableCollidableFilter
{
public:
};
struct hkpCollisionFilter
{
public:
    uint32 prepad;
    uint32 type;
    uint32 postpad;
};
struct hkpConvexListFilter
{
public:
};
struct hkpConvexShape
{
public:
    float radius;
};
struct hkpConvexTransformShapeBase
{
public:
    struct hkpSingleShapeContainer childShape;
    int32 childShapeSize;
};
struct hkpPhantomCallbackShape
{
public:
};
struct hkpRayCollidableFilter
{
public:
};
struct hkpRayShapeCollectionFilter
{
public:
};
struct hkpShape
{
public:
    uint64 userData;
    uint32 type;
};
struct hkpShapeCollection
{
public:
    bool disableWelding;
    uint8 collectionType;
};
struct hkpShapeCollectionFilter
{
public:
};
struct hkpShapeContainer
{
public:
};
struct hkpShapeModifier
{
public:
};
struct hkpSphereRepShape
{
public:
};
struct hkpHeightFieldShape
{
public:
};
struct hkpSampledHeightFieldShape
{
public:
    int32 xRes;
    int32 zRes;
    float heightCenter;
    bool useProjectionBasedHeight;
    uint8 heightfieldType;
    hkVector4 intToFloatScale;
    hkVector4 floatToIntScale;
    hkVector4 floatToIntOffsetFloorCorrected;
    hkVector4 extents;
};
struct hkWorldMemoryAvailableWatchDog
{
public:
};
struct hkpAction
{
public:
    void *world;
    void *island;
    uint64 userData;
    hkStringPtr name;
};
struct hkpArrayAction
{
public:
    hkArray<hkpEntity *> entities;
};
struct hkpBinaryAction
{
public:
    struct hkpEntityhkpEntity *entityA;
    struct hkpEntityhkpEntity *entityB;
};
struct hkpBreakableBody
{
public:
};
struct hkpConstraintChainData
{
public:
};
struct hkpConstraintData
{
public:
    uint64 userData;
};
struct hkpConstraintMotor
{
public:
    int8 type;
};
struct hkpLimitedForceConstraintMotor
{
public:
    float minForce;
    float maxForce;
};
struct hkpMotion
{
public:
    uint8 type;
    uint8 deactivationIntegrateCounter;
    uint16 deactivationNumInactiveFrames;
    struct hkMotionState motionState;
    hkVector4 inertiaAndMassInv;
    hkVector4 linearVelocity;
    hkVector4 angularVelocity;
    hkVector4 deactivationRefPosition;
    uint32 deactivationRefOrientation;
    struct hkpMaxSizeMotionhkpMaxSizeMotion *savedMotion;
    uint16 savedQualityTypeIndex;
    hkHalf gravityFactor;
};
struct hkpParametricCurve
{
public:
};
struct hkpPhantom
{
public:
    hkArray<void *> overlapListeners;
    hkArray<void *> phantomListeners;
};
struct hkpShapePhantom
{
public:
    struct hkMotionState motionState;
};
struct hkpUnaryAction
{
public:
    struct hkpEntityhkpEntity *entity;
};
struct hkpWorldObject
{
public:
    void *world;
    uint64 userData;
    struct hkpLinkedCollidable collidable;
    struct hkMultiThreadCheck multiThreadCheck;
    hkStringPtr name;
    hkArray<struct hkpProperty> properties;
    void *treeData;
};
struct hkaAnimatedReferenceFrame
{
public:
};
struct hkaAnimation
{
public:
    int32 type;
    float duration;
    int32 numberOfTransformTracks;
    int32 numberOfFloatTracks;
    struct hkaAnimatedReferenceFramehkaAnimatedReferenceFrame *extractedMotion;
    hkArray<struct hkaAnnotationTrack> annotationTracks;
};
struct hkbCondition
{
public:
};
struct hkbGenerator
{
public:
};
struct hkbGeneratorOutputListener
{
public:
};
struct hkbSequencedData
{
public:
};
struct hkbStateChooser
{
public:
};
struct hkbTransitionEffect
{
public:
    int8 selfTransitionMode;
    int8 eventMode;
    int8 defaultEventMode;
};
