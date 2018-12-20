#include "GameObject.h"

//123123
//123123123123
//4567899
//asdbasd
GameObject::GameObject(D3DUtility* app)
{
	mapp = app;
	ID3D11Device* device = app->device.Get();
	buildEffect(device);
	buildInputlayout(device);	
	buildVertexBufferandIndicesBuffer(device);
	SetWorldMatrix();
	
	
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
	technique = effect->GetTechniqueByIndex(0);
	D3DX11_PASS_DESC PassDesc;

	technique->GetPassByIndex(0)->GetDesc(&PassDesc);
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,
			D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
			D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
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
	Vertex vertices[] = {
		{XMFLOAT3(-1.0f,1.0f,-1.0f), XMFLOAT4(0.0f,0.0f,1.0f,1.0f)},
		{XMFLOAT3(1.0f,1.0f,-1.0f), XMFLOAT4(0.0f,1.0f,0.0f,1.0f)},
		{XMFLOAT3(1.0f,1.0f, 1.0f), XMFLOAT4(0.0f,1.0f,1.0f,1.0f)},
		{XMFLOAT3(-1.0f,1.0f, 1.0f), XMFLOAT4(1.0f,0.0f,0.0f,1.0f)},

		{XMFLOAT3(-1.0f,-1.0f,-1.0f),XMFLOAT4(1.0f,0.0f,1.0f,1.0f)},
		{XMFLOAT3(1.0f,-1.0f,-1.0f),XMFLOAT4(1.0f,1.0f,0.0f,1.0f)},
		{XMFLOAT3(1.0f,-1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
		{XMFLOAT3(-1.0f,-1.0f, 1.0f),XMFLOAT4(0.0f,0.0f,0.0f,1.0f)},
	};
	UINT vertexCount = ARRAYSIZE(vertices);
	D3D11_BUFFER_DESC bd; ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * 8;
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
		3,1,0, 2,1,3, 0,5,4, 1,5,0, 3,4,7, 0,4,3,
		1,6,5, 2,6,1, 2,7,6, 3,7,2, 6,4,5, 7,4,6,
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

bool GameObject::Setup()
{
	mapp->immediateContext->IASetInputLayout(vertexLayout);
	mapp->immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	mapp->immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	mapp->immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	technique->GetDesc(&techDesc);
	return true;
}
