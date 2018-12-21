#pragma once

#include "d3dUtility.h"
//#include <DirectXMath.h>
//using namespace DirectX;
//定义一个顶点结构，这个顶点包含坐标和法向量
struct  Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;

	XMFLOAT2 Tex;

};
//这是材质的对各种光反射率
struct Material
{
	XMFLOAT4 ambient;		//材质环境光反射率
	XMFLOAT4 diffuse;		//材质漫射光反射率
	XMFLOAT4 specular;		//材质镜面光反射率
	float    power;			//镜面光反射系数
};

//光源结构，这个结构包括了3种光源的所有属性
//但不是每种属性都会用到，比如方向光就不会用到
//光源位置，以及衰减因子等
struct Light
{
	int type;				//光源类型，方向光：0，点光源：1，聚光灯：2

	XMFLOAT4 position;		//光源位置
	XMFLOAT4 direction;		//方向向量

	XMFLOAT4 ambient;		//环境光强度
	XMFLOAT4 diffuse;		//漫射光强度
	XMFLOAT4 specular;		//镜面光强度

	float attenuation0;		//常量衰减因子
	float attenuation1;		//一次衰减因子
	float attenuation2;		//二次衰减因子

	float alpha;			//聚光灯内锥角度
	float beta;				//聚光灯外锥角度
	float fallOff;			//聚光灯衰减系数，一般取值为1.0

};
class GameObject
{
public:
	GameObject(D3DUtility* app);
	~GameObject();
	//void SetPosition();
	void SetWorldMatrix(XMMATRIX world);
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
	//声明材质和光照的全局对象
	Material		material;      //材质
	Light			light;      //光源数组
	ID3D11ShaderResourceView* texture = nullptr;      //纹理
	void buildEffect(ID3D11Device* device);
	void buildInputlayout(ID3D11Device* device);
	void buildVertexBufferandIndicesBuffer(ID3D11Device* device);
	void buildMaterialandLight();
	void buildTexture();
	bool Setup();
};

//this is another test