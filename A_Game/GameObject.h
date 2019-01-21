#pragma once
#include "d3dUtility.h"
#include "FBXloader.h"
#include "GeometryStructure.h"


const D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

//Create effects constant buffer's structure//
struct cbPerObject
{
	XMMATRIX  WVP;
	XMMATRIX World;

	XMFLOAT4 difColor;
	BOOL hasTexture;
	BOOL hasNormMap;
};


enum TYPE
{
	CUBE = 1,
	FBX=2
};


/*************************************************************************************************/

class GameObject
{
	FBXloader* loader;
	int AnimStackNumber ;   // 0 - 2
	FbxTime FrameTime  ;
	FbxTime timeCount  ;
	FbxTime start ;
	FbxTime stop;
	FbxNode* meshNode  ;
	FbxMesh* mesh;
	
public:
	GameObject(D3DUtility* app, TYPE modelType, std::wstring filename, char *tag);
	~GameObject();
	void SetWorldMatrix(XMMATRIX mworld);
	void SetViewProjMatrix(XMMATRIX mview, XMMATRIX mproj);
	XMVECTOR GetPos();
	void Draw();
public:
	std::vector<GameObject*>* communicate_list;
	char tag[20]="";
	TYPE type=CUBE;
	
	D3DUtility* mapp;
	Microsoft::WRL::ComPtr<ID3D11Device> dev;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> con;

	cbPerObject cbPerObj;//Matrix staff
	XMMATRIX world = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	XMMATRIX scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX cam_view = XMMatrixLookAtLH(
		XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	);
	XMMATRIX cam_projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 800.0f / 600.0f, 1.0f, 1000.0f);
	XMVECTOR pos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);




private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> vertex_layout;
	//Buffers
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;			// 顶点缓冲区
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;			// 索引缓冲区
	Microsoft::WRL::ComPtr<ID3D11Buffer> perobject_buffer;		//常量缓冲
	//ID3D11Buffer* perobject_buffer;
	//着色器
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;	// 顶点着色器
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;		// 像素着色器
	Microsoft::WRL::ComPtr<ID3D10Blob> vs_buffer;
	Microsoft::WRL::ComPtr<ID3D10Blob> ps_buffer;
	//texture
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	UINT stride = sizeof(VertexForCube);	// 跨越字节数
	UINT offset = 0;						// 起始偏移量
	int num_indices = 0;
private:
	bool InitCube(std::wstring filename);
	bool InitFBX(std::wstring filename);

	void UpdateMatrix();
	void DrawCube();
	void DrawFbx();
	void AnimeFbx();
};
