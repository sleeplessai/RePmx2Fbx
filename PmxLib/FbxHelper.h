#ifndef __FbxHelper_h__
#define __FbxHelper_h__

#include <map>
#include <memory>
#include <fbxsdk.h>
#include "MathVector.h"

class FbxHelper
{
public:
	class Shape
	{
		friend class FbxHelper;
	public:
		Shape(FbxHelper * owner, const char * strName);
		~Shape();

		void InitPositionSize(int count);
		void SetPositionAt(const Vector3 & pos, int idx);
		void AddNormal(const Vector3 & normal);
		void AddUV(const Vector2 & uv);

		void BeginFace(int matID);
		void AddIndex(int vertID);
		void EndFace();

		void AddMaterial(FbxSurfaceMaterial * pMaterial);

		void SetBindPose(FbxNode * pNode, const FbxAMatrix & mat);

		// Morph/BlendShape support
		void InitBlendShape(int morphCount);
		void AddMorphTarget(const char* morphName, int vertexIndex, const Vector3& offset);
		void AddUVMorphTarget(const char* morphName, int vertexIndex, const Vector4& offset);

	private:
		FbxHelper					*m_pOwner;
		FbxNode						*m_pNode;
		FbxMesh						*m_pMesh;
		FbxGeometryElementNormal	*m_pNormalElement;
		FbxGeometryElementUV		*m_pUVElement;

		FbxGeometryElementMaterial	*m_pMaterialElement;

		FbxSkin						*m_pSkin;
		FbxBlendShape				*m_pBlendShape;
		FbxGeometryElementUV		*m_pUVMorphElement;

		std::map<FbxNode*, FbxAMatrix>	m_BindPoses;
	};

	struct BoneInfo
	{
		FbxNode		*m_pNode = nullptr;
		FbxSkeleton	*m_pSkeleton = nullptr;
		FbxCluster	*m_pCluster = nullptr;
	};

public:
	FbxHelper();
	~FbxHelper();

	// Delete copy constructor and assignment operator for RAII safety
	FbxHelper(const FbxHelper &) = delete;
	FbxHelper & operator=(const FbxHelper &) = delete;

	void SetInfo();
	bool SaveScene(const char * pFileName, bool pEmbedMedia = false, int pFileFormat = -1);

	void AddNodeToRoot(FbxNode * pNode);

	std::unique_ptr<Shape> BeginShape(const char * strName);
	// EndShape is no longer needed as a public method due to unique_ptr RAII

	FbxSurfacePhong * NewPhong(const char * strMatName);
	FbxFileTexture * NewTexture(const char * strName, const char * strFileName);
	void NewBoneNode(const char * strName, const Shape * pShape, BoneInfo & boneInfo);

public:
	void StoreBindPose(FbxNode * pNode, std::map<FbxNode*, FbxAMatrix> & poseMap);

private:
	FbxManager	*m_pSdkMgr;
	FbxScene	*m_pScene;
};

#endif
