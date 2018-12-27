#include "GameObject.h"
#include <DDSTextureLoader.h>
void GameObject::BOSS()
{
	int count = communicateList->size();

	for (std::vector<GameObject*>::iterator it = communicateList->begin(); it != communicateList->end(); it++)
	{
		if (strcmp((*it)->tag, "player") == 0)
		{
			
			XMVECTOR PlayerPos = (*it)->GetPos();
			XMVECTOR BossPos = this->GetPos();
			XMVECTOR Dir = XMVector3Normalize(PlayerPos - BossPos);
			XMFLOAT4 deltainDir;
			XMStoreFloat4(&deltainDir, Dir); //��Bossָ��player�����  

			XMFLOAT4 originPos;//Bossԭ����λ��
			XMStoreFloat4(&originPos, BossPos);
			//Bossԭ����λ�� + ��Bossָ��player�����*0.0001f = BossӦ���ƶ�����λ��

			float destX; float destY; float destZ;//BossӦ���ƶ�����λ��
			destX = originPos.x + deltainDir.x*0.0001f;
			destY = originPos.y + deltainDir.y*0.0001f;
			destZ = originPos.z + deltainDir.z*0.0001f;
			world = XMMatrixTranslation(destX, destY, destZ);
			SetWorldMatrix(world);
		}
	}
}


GameObject::GameObject(D3DUtility* app)
{
	mapp = app;
	ID3D11Device* device = app->device.Get();
	buildEffect(device);


	const WCHAR *pwcsName = L"box.dds";
	buildTexture(pwcsName);
	buildInputlayout(device);


	buildVertexBufferandIndicesBuffer(device);
	buildMaterialandLight();
	SetWorldMatrix(XMMatrixIdentity());
	
	
}

GameObject::~GameObject()
{

}

void GameObject::SetWorldMatrix(XMMATRIX mworld)
{
	world = mworld;
	XMVECTOR tmp = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	pos = XMVector4Transform(tmp, mworld);
	Setup();
}

void GameObject::SetViewMatrix(XMMATRIX mview)
{
	view = mview;
	Setup();
}

void GameObject::SetProjMatrix(XMMATRIX mproj)
{
	projection = mproj;
	Setup();
}




XMVECTOR GameObject::GetPos()
{
	return pos;
}

void GameObject::buildEffect(ID3D11Device* device)
{
	//1.Create effect instance from .fx
	HRESULT hr = S_OK;
	ID3DBlob* pTechBLob = NULL;
	ID3DBlob* compilationMsgs = nullptr;

	hr = D3DX11CompileEffectFromFile(
		L"Shader/TextureAndLightShader.fx",
		nullptr, nullptr,
		D3DCOMPILE_ENABLE_STRICTNESS, 0, device, &effect, &compilationMsgs);
	if (FAILED(hr))
	{
		//OutputDebugString((char*)compilationMsgs->GetBufferPointer());
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		::MessageBox(NULL, L"fx load failed!!", L"ERROR", MB_OK);
	}
}

void GameObject::buildInputlayout(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	//��GetTechniqueByName��ȡID3DX11EffectTechnique�Ķ���
//������Ĭ�ϵ�technique��Effect
	technique = effect->GetTechniqueByName("T0");                //Ĭ��Technique

	//D3DX11_PASS_DESC�ṹ��������һ��Effect Pass
	D3DX11_PASS_DESC PassDesc;
	//����GetPassByIndex��ȡEffect Pass
	//������GetDesc��ȡEffect Pass�����������PassDesc������
	technique->GetPassByIndex(0)->GetDesc(&PassDesc);
	
	//�������������벼��
	//�������Ƕ���һ��D3D11_INPUT_ELEMENT_DESC���飬
	//�������Ƕ���Ķ���ṹ���λ�����ͷ������������������������Ԫ��
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numElements = ARRAYSIZE(layout);

	hr=device->CreateInputLayout(
		layout,
		numElements,
		PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize,
		&vertexLayout
	);
	if (FAILED(hr))
	{
		::MessageBox(NULL, L"input layout creation failed!!", L"ERROR", MB_OK);
	}
}

void GameObject::buildVertexBufferandIndicesBuffer(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	//2. Create Vertex Buffer and Indices Buffer
		//��ʵ��4һ�����������飬����ÿ����а���������ɫ
	//�������ڶ���ķ������ͬ�����Լ�ʹ��ͬһ��λ�õ�����Ҳ���뵥�����
	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)  ,XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)  ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) ,XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)   , XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)  , XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)  , XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)  ,XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) ,XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)  , XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) , XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)   , XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)  ,XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)  ,XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) ,XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)  ,XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)    ,XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)    ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)   ,XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)     ,XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)    ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)    ,XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)  ,XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)  ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) ,XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)   ,XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)  ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)  ,XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)     ,XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)   ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)    ,XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)    ,XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)   ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)     ,XMFLOAT2(1.0f, 1.0f) },
	};
	UINT vertexCount = ARRAYSIZE(vertices);
	D3D11_BUFFER_DESC bd = {}; 
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;

	
	hr = device->CreateBuffer(&bd, &InitData, &vertexBuffer);
	if (FAILED(hr))
	{
		::MessageBox(NULL, L"Vertex Buffer creation failed!!", L"ERROR", MB_OK);
		
	}
}

void GameObject::buildMaterialandLight()
{
	//*************���Ĳ����ò��ʺ͹���***********************
	// ���ò��ʣ�3�й��յķ������Լ�����ⷴ��ϵ��
	//��������ǰ��λ��ʾ�������ķ����ʣ�1��ʾ��ȫ���䣬0��ʾ��ȫ����
	material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); //ǰ��λ�ֱ��ʾ�������ķ�����
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); //ͬ��
	material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);//ͬ��
	material.power = 5.0f;
	// ���ù�Դ
	Light dirLight;
	// �����ֻ��Ҫ���ã�����3�ֹ���ǿ��
	dirLight.direction = XMFLOAT4(-1.0f, 0.0f, 1.0f, 1.0f); //���շ���
	dirLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);   //ǰ��λ�ֱ��ʾ��������ǿ��
	dirLight.diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);   //ͬ��
	dirLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);  //ͬ��
	light = dirLight;
	//������Ϣ�ĳ�������еĲ�����Ϣ���õ�Effect�����-----------------------------
	effect->GetVariableByName("MatAmbient")->AsVector()->SetFloatVector((float*)&(material.ambient));
	effect->GetVariableByName("MatDiffuse")->AsVector()->SetFloatVector((float*)&(material.diffuse));
	effect->GetVariableByName("MatSpecular")->AsVector()->SetFloatVector((float*)&(material.specular));
	effect->GetVariableByName("MatPower")->AsScalar()->SetFloat(material.power);

	//���Ƚ��������ͣ�������ǿ�ȣ������ǿ�ȣ������ǿ�����õ�Effect��
	effect->GetVariableByName("LightAmbient")->AsVector()->SetFloatVector((float*)&(light.ambient));
	effect->GetVariableByName("LightDiffuse")->AsVector()->SetFloatVector((float*)&(light.diffuse));
	effect->GetVariableByName("LightSpecular")->AsVector()->SetFloatVector((float*)&(light.specular));

	//�����ֻ��Ҫ������������Լ���
	effect->GetVariableByName("LightDirection")->AsVector()->SetFloatVector((float*)&(light.direction));
	//��������Tectnique���õ�Effect
	technique = effect->GetTechniqueByName("T_DirLight");
}

void GameObject::buildTexture(const wchar_t* filename)
{
	DirectX::CreateDDSTextureFromFile(mapp->device.Get(),
		filename, nullptr, &texture);

	//���������õ���Texture��
	effect->GetVariableByName("Texture")->AsShaderResource()->SetResource(texture);
	technique = effect->GetTechniqueByName("T_DirLight");                //Ĭ��Technique

}

bool GameObject::Setup()
{
	effect->GetVariableByName("World")->AsMatrix()
		->SetMatrix((float*)&world);
	effect->GetVariableByName("View")->AsMatrix()
		->SetMatrix((float*)&view);
	effect->GetVariableByName("Projection")->AsMatrix()
		->SetMatrix((float*)&projection);
	mapp->immediateContext->IASetInputLayout(vertexLayout);
	mapp->immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	mapp->immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	mapp->immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	technique->GetDesc(&techDesc);
	return true;
}
