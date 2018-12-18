#pragma once

#include "d3dUtility.h"
//#include <DirectXMath.h>
//using namespace DirectX;
//����һ������ṹ����������������ͷ�����
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
	void SetWorldMatrix(XMMATRIX world);
	//��ɫ��
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;

	//Effect���ȫ��ָ��
	ID3D11InputLayout* vertexLayout = nullptr;
	ID3DX11Effect* effect = nullptr;
	ID3DX11EffectTechnique* technique = nullptr;
	ID3D11Buffer* vertexBuffer=nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	//������������ϵ����
	XMMATRIX world;         //��������任�ľ���
	XMMATRIX view;          //���ڹ۲�任�ľ���
	XMMATRIX projection;    //����ͶӰ�任�ľ���


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