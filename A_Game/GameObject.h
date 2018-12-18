#pragma once

#include "d3dUtility.h"
//#include <DirectXMath.h>
//using namespace DirectX;
//定义一个顶点结构，这个顶点包含坐标和法向量
struct  Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};
class GameObject
{
public:
	GameObject(D3DUtility* app);
	~GameObject();
	//void SetPosition();
	void SetWorldMatrix(XMMATRIX world = XMMatrixIdentity());
	//着色器
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;

	//Effect相关全局指针
	ID3D11InputLayout* vertexLayout = nullptr;
	ID3DX11Effect* effect = nullptr;
	ID3DX11EffectTechnique* technique = nullptr;
	ID3D11Buffer* vertexBuffer=nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	//声明三个坐标系矩阵
	XMMATRIX world;         //用于世界变换的矩阵
	XMMATRIX view;          //用于观察变换的矩阵
	XMMATRIX projection;    //用于投影变换的矩阵


	 UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3DX11_TECHNIQUE_DESC techDesc;
	D3DUtility* mapp;
private:
	
	void buildEffect(ID3D11Device* device);
	void buildInputlayout(ID3D11Device* device);
	void buildVertexBufferandIndicesBuffer(ID3D11Device* device);
	bool Setup();
};

//this is another test