#include "GameObject.h"
GameObject::GameObject(D3DUtility* app)
{
	mapp = app;
	ID3D11Device* device = app->device.Get();
	buildEffect(device);
	buildInputlayout(device);
	buildVertexBufferandIndicesBuffer(device);
	buildMaterialandLight();
	SetWorldMatrix(XMMatrixIdentity());
	
	
}

GameObject::~GameObject()
{

}

void GameObject::SetWorldMatrix(XMMATRIX world)
{
	//Matrix Stuff
	/*world = XMMatrixIdentity();*/
	//world = XMMatrixRotationZ(0.5f);
	XMVECTOR Eye = XMVectorSet(3.0f, 5.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	view = XMMatrixLookAtLH(Eye, At, Up);
	projection = XMMatrixPerspectiveFovLH(XM_PIDIV2,
		800.0f / 600.0f,
		0.01f,
		100.0f);
	effect->GetVariableByName("World")->AsMatrix()
		->SetMatrix((float*)&world);
	effect->GetVariableByName("View")->AsMatrix()
		->SetMatrix((float*)&view);
	effect->GetVariableByName("Projection")->AsMatrix()
		->SetMatrix((float*)&projection);
	Setup();
}

void GameObject::buildEffect(ID3D11Device* device)
{
	//1.Create effect instance from .fx
	HRESULT hr = S_OK;
	ID3DBlob* pTechBLob = NULL;
	ID3DBlob* compilationMsgs = nullptr;

	hr = D3DX11CompileEffectFromFile(
		L"Shader/SimpleShader.fx",
		nullptr, nullptr,
		D3DCOMPILE_ENABLE_STRICTNESS, 0, device, &effect, &compilationMsgs);
	if (FAILED(hr))
	{
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
	//������GetDesc��ȡEffect Pass��������������PassDesc������
	technique->GetPassByIndex(0)->GetDesc(&PassDesc);
	
	//�������������벼��
	//�������Ƕ���һ��D3D11_INPUT_ELEMENT_DESC���飬
	//�������Ƕ���Ķ���ṹ����λ������ͷ��������������������������Ԫ��
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
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
		//��ʵ��4һ���������������飬����ÿ�������а������������ɫ
	//�������ڶ���ķ�������ͬ�����Լ�ʹ��ͬһ��λ�õ���������Ҳ���뵥������
	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	};
	UINT vertexCount = ARRAYSIZE(vertices);
	D3D11_BUFFER_DESC bd; ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;

	
	hr = device->CreateBuffer(&bd, &InitData, &vertexBuffer);
	if (FAILED(hr))
	{
		::MessageBox(NULL, L"Vertex Buffer creation failed!!", L"ERROR", MB_OK);
		
	}

	WORD indices[] =
	{
		3,1,0,    2,1,3,    6,4,5,    7,4,6,    11,9,8,    10,9,11,
		14,12,13, 15,12,14, 19,17,16, 18,17,19, 22,20,21,  23,20,22
	};
	UINT indexCount = ARRAYSIZE(indices);

	//Create Indices Buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	InitData.pSysMem = indices;
	

	hr = device->CreateBuffer(&bd, &InitData, &indexBuffer);
	if (FAILED(hr))
	{
		::MessageBox(NULL, L"Index Buffer creation failed!!", L"ERROR", MB_OK);
		
	}
}

void GameObject::buildMaterialandLight()
{
	//*************���Ĳ����ò��ʺ͹���***********************
	// ���ò��ʣ�3�й��յķ������Լ�����ⷴ��ϵ��
	//��������ǰ��λ��ʾ��������ķ����ʣ�1��ʾ��ȫ���䣬0��ʾ��ȫ����
	material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); //ǰ��λ�ֱ��ʾ��������ķ�����
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); //ͬ��
	material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);//ͬ��
	material.power = 5.0f;
	// ���ù�Դ
	Light dirLight;
	// �����ֻ��Ҫ���ã�����3�ֹ���ǿ��
	dirLight.type = 0;
	dirLight.direction = XMFLOAT4(-1.0f, 0.0f, 1.0f, 1.0f); //���շ���
	dirLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);   //ǰ��λ�ֱ��ʾ���������ǿ��
	dirLight.diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);   //ͬ��
	dirLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);  //ͬ��
	light = dirLight;
	//������Ϣ�ĳ��������еĲ�����Ϣ���õ�Effect�����-----------------------------
	effect->GetVariableByName("MatAmbient")->AsVector()->SetFloatVector((float*)&(material.ambient));
	effect->GetVariableByName("MatDiffuse")->AsVector()->SetFloatVector((float*)&(material.diffuse));
	effect->GetVariableByName("MatSpecular")->AsVector()->SetFloatVector((float*)&(material.specular));
	effect->GetVariableByName("MatPower")->AsScalar()->SetFloat(material.power);

	//���Ƚ��������ͣ�������ǿ�ȣ������ǿ�ȣ������ǿ�����õ�Effect��
	effect->GetVariableByName("type")->AsScalar()->SetInt(light.type);
	effect->GetVariableByName("LightAmbient")->AsVector()->SetFloatVector((float*)&(light.ambient));
	effect->GetVariableByName("LightDiffuse")->AsVector()->SetFloatVector((float*)&(light.diffuse));
	effect->GetVariableByName("LightSpecular")->AsVector()->SetFloatVector((float*)&(light.specular));

	//�����ֻ��Ҫ������������Լ���
	effect->GetVariableByName("LightDirection")->AsVector()->SetFloatVector((float*)&(light.direction));
	//��������Tectnique���õ�Effect
	technique = effect->GetTechniqueByName("T_DirLight");
}

bool GameObject::Setup()
{
	mapp->immediateContext->IASetInputLayout(vertexLayout);
	mapp->immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	mapp->immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	mapp->immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	technique->GetDesc(&techDesc);
	return true;
}
