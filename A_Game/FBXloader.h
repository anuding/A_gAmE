#pragma once
#include <vector>
#include <fbxsdk.h>
#include <Windows.h>
#include "GeometryStructure.h"

class FBXloader
{
public:
	FBXloader();
	~FBXloader();

	VertexForCube *fbx_vertices;
	int *fbx_indices;
	int vertices_size = 0;
	int indices_size = 0;
	int AnimStackNumber = 2;    // 0 - 2
	FbxTime FrameTime, timeCount, start, stop;
	FbxNode* pFbxChildNode;
	FbxMesh* pMesh;

	std::vector<VertexForCube> pOutVertexVector;
	HRESULT LoadFBX();
	FbxManager* g_pFbxSdkManager = nullptr;
};

