#pragma once
#include "d3dUtility.h"

//定义一个顶点结构，这个顶点包含坐标和法向量
struct  VertexForCube
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};


const D3D11_INPUT_ELEMENT_DESC inputLayout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


struct Vertex	//Overloaded Vertex Structure
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz,
		float tx, float ty, float tz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz),
		tangent(tx, ty, tz) {}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 biTangent;

	// Will not be sent to shader
	int StartWeight;
	int WeightCount;
};


struct Joint
{
	std::wstring name;
	int parentID;

	XMFLOAT3 pos;
	XMFLOAT4 orientation;
};
struct BoundingBox
{
	XMFLOAT3 min;
	XMFLOAT3 max;
};

struct FrameData
{
	int frameID;
	std::vector<float> frameData;
};
struct AnimJointInfo
{
	std::wstring name;
	int parentID;

	int flags;
	int startIndex;
};

struct ModelAnimation
{
	int numFrames;
	int numJoints;
	int frameRate;
	int numAnimatedComponents;

	float frameTime;
	float totalAnimTime;
	float currAnimTime;

	std::vector<AnimJointInfo> jointInfo;
	std::vector<BoundingBox> frameBounds;
	std::vector<Joint>	baseFrameJoints;
	std::vector<FrameData>	frameData;
	std::vector<std::vector<Joint>> frameSkeleton;
};
///////////////**************new**************////////////////////

struct Weight
{
	int jointID;
	float bias;
	XMFLOAT3 pos;
	///////////////**************new**************////////////////////
	XMFLOAT3 normal;
	///////////////**************new**************////////////////////
};

struct ModelSubset
{
	int texArrayIndex;
	int numTriangles;

	std::vector<Vertex> vertices;
	std::vector<XMFLOAT3> jointSpaceNormals;
	std::vector<DWORD> indices;
	std::vector<Weight> weights;

	std::vector<XMFLOAT3> positions;

	ID3D11Buffer* vertBuff;
	ID3D11Buffer* indexBuff;
};

struct Model3D
{
	int numSubsets;
	int numJoints;

	std::vector<Joint> joints;
	std::vector<ModelSubset> subsets;

	///////////////**************new**************////////////////////
	std::vector<ModelAnimation> animations;
	///////////////**************new**************////////////////////
};
struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT3 pos;
	float range;
	XMFLOAT3 dir;
	float cone;
	XMFLOAT3 att;
	float pad2;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};


//Create effects constant buffer's structure//
struct cbPerObject
{
	XMMATRIX  WVP;
	XMMATRIX World;
	//These will be used for the pixel shader
	XMFLOAT4 difColor;
	BOOL hasTexture;
	//Because of HLSL structure packing, we will use windows BOOL
	//instead of bool because HLSL packs things into 4 bytes, and
	//bool is only one byte, where BOOL is 4 bytes
	BOOL hasNormMap;
};
struct cbPerFrame
{
	Light  light;
};

enum TYPE
{
	CUBE = 1,
	MD5=2,
	OBJ=3
};

class GameObject
{
public:
	std::vector<GameObject*>* communicateList;
	char tag[20]="";
	TYPE mType=CUBE;

	void BOSS();
	GameObject(D3DUtility* app);
	GameObject(D3DUtility* app, TYPE modelType, std::wstring filename);
	~GameObject();

	void SetWorldMatrix(XMMATRIX mworld);
	void SetViewMatrix(XMMATRIX mview);
	void SetProjMatrix(XMMATRIX mproj);
	//着色器
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;	// 顶点着色器
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;		// 像素着色器

	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;//MD5专用
	
	ID3D10Blob* VS_Buffer = nullptr;
	ID3D10Blob* PS_Buffer = nullptr;
	//输入布局
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mVertexLayout;	// 顶点输入布局
	ID3D11InputLayout* vertLayout;//MD5专用

	//资源
	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;			// 顶点缓冲区
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;			// 索引缓冲区
	
	ID3D11Buffer* cbPerObjectBuffer;//常量缓冲区 for Matrix
	ID3D11Buffer* cbPerFrameBuffer;//MD5专用个屁

	cbPerObject cbPerObj;//Matrix staff
	D3DUtility* mapp;

	XMVECTOR GetPos();
	void GetRota();
	void SetPos();
	void SetRota();


public:

	XMMATRIX camView = XMMatrixLookAtLH(
		XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	);
	XMMATRIX camProjection=XMMatrixPerspectiveFovLH(XM_PIDIV2, 800.0f / 600.0f, 1.0f, 1000.0f);
	XMMATRIX Scale;
	XMMATRIX World=XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	XMMATRIX smilesWorld;

	ID3D11Device* dev;
	ID3D11DeviceContext* con;
	Model3D NewMD5Model;
	Light light;

	cbPerFrame constbuffPerFrame;

	std::vector<ID3D11ShaderResourceView*> meshSRV;
	std::vector<std::wstring> textureNameArray;

	XMVECTOR pos= XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	bool InitCube();
	bool InitMd5();



	
	void UpdateMatrix();
	void DrawMd5();
	void DrawCube();
public:
	bool Setup();
	//LoadMD5Model() function prototype
	bool LoadMD5Model(std::wstring filename,
		Model3D& MD5Model,
		std::vector<ID3D11ShaderResourceView*>& shaderResourceViewArray,
		std::vector<std::wstring> texFileNameArray);
	bool LoadMD5Anim(std::wstring filename, Model3D& MD5Model);
	void UpdateMD5Model(Model3D& MD5Model, float deltaTime, int animation);
	void Draw();
};

//this is another test