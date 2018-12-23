#pragma once
#include "d3dUtility.h"
//#include <DirectXMath.h>
//using namespace DirectX;
//����һ������ṹ����������������ͷ�����
struct  Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;

};
//���ǲ��ʵĶԸ��ֹⷴ����
struct Material
{
	XMFLOAT4 ambient;		//���ʻ����ⷴ����
	XMFLOAT4 diffuse;		//��������ⷴ����
	XMFLOAT4 specular;		//���ʾ���ⷴ����
	float    power;			//����ⷴ��ϵ��
};

//��Դ�ṹ������ṹ������3�ֹ�Դ����������
//������ÿ�����Զ����õ������緽���Ͳ����õ�
//��Դλ�ã��Լ�˥�����ӵ�
struct Light
{
	int type;				//��Դ���ͣ�����⣺0�����Դ��1���۹�ƣ�2

	XMFLOAT4 position;		//��Դλ��
	XMFLOAT4 direction;		//��������

	XMFLOAT4 ambient;		//������ǿ��
	XMFLOAT4 diffuse;		//�����ǿ��
	XMFLOAT4 specular;		//�����ǿ��

	float attenuation0;		//����˥������
	float attenuation1;		//һ��˥������
	float attenuation2;		//����˥������

	float alpha;			//�۹����׶�Ƕ�
	float beta;				//�۹����׶�Ƕ�
	float fallOff;			//�۹��˥��ϵ����һ��ȡֵΪ1.0

};
class GameObject
{
public:

	std::vector<GameObject*>* communicateList;
	char tag[20]="";

	void BOSS();
	GameObject(D3DUtility* app);
	~GameObject();
	//void SetPosition();
	void SetWorldMatrix(XMMATRIX mworld);
	void SetViewMatrix(XMMATRIX mview);
	void SetProjMatrix(XMMATRIX mproj);
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
	XMMATRIX world= XMMatrixIdentity();         //��������任�ľ���
	XMMATRIX view= XMMatrixIdentity();      //���ڹ۲�任�ľ���
	XMMATRIX projection= XMMatrixIdentity();    //����ͶӰ�任�ľ���


	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3DX11_TECHNIQUE_DESC techDesc;
	D3DUtility* mapp;

	XMVECTOR GetPos();
	void GetRota();
	void SetPos();
	void SetRota();
private:
	XMVECTOR pos= XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR rota;

	//�������ʺ͹��յ�ȫ�ֶ���
	Material		material;      //����
	Light			light;      //��Դ����
	ID3D11ShaderResourceView* texture = nullptr;      //����
	void buildEffect(ID3D11Device* device);
	void buildInputlayout(ID3D11Device* device);
	void buildVertexBufferandIndicesBuffer(ID3D11Device* device);
	void buildMaterialandLight();
	
public:
	void buildTexture(const wchar_t* filename);
	bool Setup();
};

//this is another test