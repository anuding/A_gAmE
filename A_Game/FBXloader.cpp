#include "FBXloader.h"
#include "debugprint.h"

FBXloader::FBXloader()
{
}


FBXloader::~FBXloader()
{
}


HRESULT FBXloader::LoadFBX()
{
	if (g_pFbxSdkManager == nullptr)
	{
		g_pFbxSdkManager = FbxManager::Create();

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(g_pFbxSdkManager, IOSROOT);
		g_pFbxSdkManager->SetIOSettings(pIOsettings);
	}

	FbxImporter* pImporter = FbxImporter::Create(g_pFbxSdkManager, "");
	FbxScene* pFbxScene = FbxScene::Create(g_pFbxSdkManager, "");

	bool bSuccess = pImporter->Initialize("E:\\Model_Proj\\gallina\\gallina_tri.fbx", -1, g_pFbxSdkManager->GetIOSettings());
	if (!bSuccess) return E_FAIL;

	bSuccess = pImporter->Import(pFbxScene);
	if (!bSuccess) return E_FAIL;

	pImporter->Destroy();

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();




	FbxArray<FbxString*> AnimStackNameArray;
	pFbxScene->FillAnimStackNameArray(AnimStackNameArray);
	FbxAnimStack *AnimationStack = pFbxScene->FindMember<FbxAnimStack>(AnimStackNameArray[AnimStackNumber]->Buffer());
	pFbxScene->SetCurrentAnimationStack(AnimationStack);

	FbxTakeInfo *takeInfo = pFbxScene->GetTakeInfo(*(AnimStackNameArray[AnimStackNumber]));
	start = takeInfo->mLocalTimeSpan.GetStart();
	stop = takeInfo->mLocalTimeSpan.GetStop();
	FrameTime.SetTime(0, 0, 0, 1, 0, pFbxScene->GetGlobalSettings().GetTimeMode());
	timeCount = start;

	int tr = 0;
	if (pFbxRootNode)
	{
		//GetChildCount()获取场景中的物体数量, 一般为一个
		for (int j = 0; j < pFbxRootNode->GetChildCount(); j++)
		{
			pFbxChildNode = pFbxRootNode->GetChild(j);

			if (pFbxChildNode->GetNodeAttribute() == NULL)
				continue;
			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();
			if (AttributeType != FbxNodeAttribute::eMesh)
				continue;



			pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();
			int numSubmesh = pFbxChildNode->GetMaterialCount();

			FbxVector4* pVertices = pMesh->GetControlPoints();
		
			int cnt = pMesh->GetControlPointsCount();
			//GetControlPointsCount()获得d3d中顶点的数量  pMesh->GetPolygonCount()
			vertices_size = sizeof(VertexForCube)*pMesh->GetControlPointsCount();
			fbx_vertices = new VertexForCube[pMesh->GetControlPointsCount()];
			for (int i = 0; i < pMesh->GetControlPointsCount(); i++) {
				fbx_vertices[i].pos.x = (FLOAT)pMesh->GetControlPointAt(i)[0]*0.01f;
				fbx_vertices[i].pos.y = (FLOAT)pMesh->GetControlPointAt(i)[1]*0.01f;
				fbx_vertices[i].pos.z = (FLOAT)pMesh->GetControlPointAt(i)[2]*0.01f;
				fbx_vertices[i].texCoord.x = 0.0f;
				fbx_vertices[i].texCoord.y = 1.0f; 
				fbx_vertices[i].tangent.x = 0.0f;
				fbx_vertices[i].tangent.y = 0.0f;
				fbx_vertices[i].tangent.z = 0.0f;
				fbx_vertices[i].normal.x = 0.0f;
				fbx_vertices[i].normal.y = 0.0f;
				fbx_vertices[i].normal.z = 0.0f;
			}

			indices_size= sizeof(int)*pMesh->GetPolygonVertexCount();
			int indices_cnt =pMesh->GetPolygonVertexCount();

			fbx_indices = pMesh->GetPolygonVertices();
		

			
		}

	}
	return S_OK;
}