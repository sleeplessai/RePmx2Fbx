#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <windows.h>
#include "PmxReader.h"
#include "FbxHelper.h"

namespace fs = std::filesystem;

static void SavePmxToFbx(PmxReader & pmx, const char * strFileName)
{
	FbxHelper fbx;

	auto shp = fbx.BeginShape(pmx.ModelName.c_str());
	{
		shp->InitPositionSize(static_cast<int>(pmx.VertexList.size()));

		int count = 0;
		for (const auto & item : pmx.VertexList)
		{
			shp->SetPositionAt(item.Position, count);
			shp->AddNormal(item.Normal);
			shp->AddUV(item.UV);
			++count;
		}

		int currFace = 0;
		for (size_t matID = 0, szMat = pmx.MaterialList.size(); matID < szMat; ++matID)
		{
			const auto & currMat = pmx.MaterialList[matID];
			auto pFbxMat = fbx.NewPhong(currMat.Name.c_str());
			pFbxMat->Diffuse.Set(FbxDouble3(currMat.Diffuse.X, currMat.Diffuse.Y, currMat.Diffuse.Z));
			pFbxMat->Ambient.Set(FbxDouble3(currMat.Ambient.X, currMat.Ambient.Y, currMat.Ambient.Z));
			pFbxMat->Specular.Set(FbxDouble3(currMat.Specular.X, currMat.Specular.Y, currMat.Specular.Z));
			pFbxMat->SpecularFactor = currMat.Power;

			auto pFbxTex = fbx.NewTexture("Diffuse", currMat.Tex.c_str());
			pFbxMat->Diffuse.ConnectSrcObject(pFbxTex);

			auto pFbxOpacityTex = fbx.NewTexture("Opacity", currMat.Tex.c_str());
			pFbxOpacityTex->SetAlphaSource(FbxTexture::eBlack);
			pFbxMat->TransparentColor.ConnectSrcObject(pFbxOpacityTex);
			shp->AddMaterial(pFbxMat);

			count = 0;
			for (int endFace = currFace + currMat.FaceCount; currFace < endFace; ++currFace)
			{
				if (count == 0)
				{
					shp->BeginFace(static_cast<int>(matID));
				}

				shp->AddIndex(pmx.FaceList[currFace]);

				if (count == 2)
				{
					shp->EndFace();
					count = 0;
				}
				else
				{
					++count;
				}
			}
		}

		std::vector<FbxHelper::BoneInfo> fbxBoneList(pmx.BoneList.size());
		count = 0;
		for (const auto & currBone : pmx.BoneList)
		{
			fbx.NewBoneNode(currBone.Name.c_str(), shp.get(), fbxBoneList[count]);
			++count;
		}

		size_t boneID = 0;
		for (const auto & currBone : pmx.BoneList)
		{
			if (currBone.Parent < 0)
			{
				fbxBoneList[boneID].m_pSkeleton->SetSkeletonType(FbxSkeleton::eRoot);
				fbx.AddNodeToRoot(fbxBoneList[boneID].m_pNode);
			}
			else
			{
				fbxBoneList[boneID].m_pSkeleton->SetSkeletonType(FbxSkeleton::eLimbNode);
				fbxBoneList[currBone.Parent].m_pNode->AddChild(fbxBoneList[boneID].m_pNode);
			}
			++boneID;
		}

		count = 0;
		for (auto & item : fbxBoneList)
		{
			const auto & currBone = pmx.BoneList[count];
			++count;

			FbxAMatrix matLocal;
			matLocal.SetRow(0, FbxVector4(currBone.LocalMatrix._11, currBone.LocalMatrix._12, currBone.LocalMatrix._13, currBone.LocalMatrix._14));
			matLocal.SetRow(1, FbxVector4(currBone.LocalMatrix._21, currBone.LocalMatrix._22, currBone.LocalMatrix._23, currBone.LocalMatrix._24));
			matLocal.SetRow(2, FbxVector4(currBone.LocalMatrix._31, currBone.LocalMatrix._32, currBone.LocalMatrix._33, currBone.LocalMatrix._34));
			matLocal.SetRow(3, FbxVector4(currBone.LocalMatrix._41, currBone.LocalMatrix._42, currBone.LocalMatrix._43, currBone.LocalMatrix._44));

			item.m_pNode->LclTranslation.Set(FbxDouble3(matLocal.GetT()));
			item.m_pNode->LclRotation.Set(FbxDouble3(matLocal.GetR()));

			FbxAMatrix matWorld;
			matWorld.SetRow(0, FbxVector4(currBone.WorldMatrix._11, currBone.WorldMatrix._12, currBone.WorldMatrix._13, currBone.WorldMatrix._14));
			matWorld.SetRow(1, FbxVector4(currBone.WorldMatrix._21, currBone.WorldMatrix._22, currBone.WorldMatrix._23, currBone.WorldMatrix._24));
			matWorld.SetRow(2, FbxVector4(currBone.WorldMatrix._31, currBone.WorldMatrix._32, currBone.WorldMatrix._33, currBone.WorldMatrix._34));
			matWorld.SetRow(3, FbxVector4(currBone.WorldMatrix._41, currBone.WorldMatrix._42, currBone.WorldMatrix._43, currBone.WorldMatrix._44));

			item.m_pCluster->SetTransformLinkMatrix(matWorld);
		}

		count = 0;
		for (const auto & item : pmx.VertexList)
		{
			for (int i = 0; i < 4; ++i)
			{
				const auto & currWeight = item.Weight[i];
				if (currWeight.IsValid())
				{
					fbxBoneList[currWeight.Bone].m_pCluster->AddControlPointIndex(count, currWeight.Value);
				}
			}
			++count;
		}
	}

	fbx.SaveScene(strFileName);
}

static std::vector<char> ReadFile(const wchar_t* strFileName)
{
	std::ifstream file(strFileName, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		return {};
	}

	const auto size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(static_cast<size_t>(size));
	if (file.read(buffer.data(), size))
	{
		return buffer;
	}

	return {};
}

static std::wstring Utf8ToWstring(const std::string& str)
{
	if (str.empty()) return {};
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &wstr[0], size_needed);
	return wstr;
}

static std::string WstringToUtf8(const std::wstring& wstr)
{
	if (wstr.empty()) return {};
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &str[0], size_needed, nullptr, nullptr);
	return str;
}

int wmain(int argc, const wchar_t** argv)
{
	if (argc == 2)
	{
		fs::path inputPath(argv[1]);
		if (fs::exists(inputPath))
		{
			if (inputPath.has_parent_path())
			{
				fs::current_path(inputPath.parent_path());
			}

			fs::path fileNameOnly = inputPath.filename();
			auto buffer = ReadFile(fileNameOnly.c_str());

			if (!buffer.empty())
			{
				PmxReader reader(buffer.data(), buffer.size());

				std::wstring outputPath = fileNameOnly.wstring() + L".fbx";
				SavePmxToFbx(reader, WstringToUtf8(outputPath).c_str());
			}
		}
	}
	return 0;
}
