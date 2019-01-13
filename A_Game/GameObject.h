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
struct BoundingBoxMD5
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
	std::vector<BoundingBoxMD5> frameBounds;
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

struct SurfaceMaterial
{
	std::wstring matName;
	XMFLOAT4 difColor;
	int texArrayIndex;
	int normMapTexArrayIndex;
	bool hasNormMap;
	bool hasTexture;
	bool transparent;
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
	OBJ=3,//don't use
	SDKMESH=4,
	SKY=5
};


/*************************************************************************************************/

class GameObject
{
public:
	std::vector<GameObject*>* communicateList;
	char tag[20]="";
	TYPE mType=CUBE;

	ID3D11InputLayout* vertexLayout;

	// 网格顶点缓存
	ID3D11Buffer				*m_meshVertexBuffer;
	// 网格索引缓存
	ID3D11Buffer				*m_meshIndexBuffer;
	// 文件顶点数据信息
	std::vector<XMFLOAT3>		m_vertices; //坐标
	std::vector<XMFLOAT2>		m_textures; //纹理
	std::vector<XMFLOAT3>		m_normals;  //法向量


	std::vector<int> meshSubsetIndexStart;
	std::vector<int> meshSubsetTexture;
	XMMATRIX meshWorld;

	GameObject(D3DUtility* app);
	void CreateSphere(int LatLines, int LongLines);
	GameObject(D3DUtility* app, TYPE modelType, std::wstring filename, char *tag,
		std::shared_ptr<EffectFactory> fa, std::shared_ptr<CommonStates> state);
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
	std::vector<SurfaceMaterial> material;
	cbPerObject cbPerObj;//Matrix staff
	D3DUtility* mapp;

	XMVECTOR GetPos();


public:
	ID3D11SamplerState* CubesTexSamplerState;
	ID3D11BlendState* Transparency;
	ID3D11RasterizerState* RSCullNone;
	ID3D11RasterizerState* CCWcullMode;
	ID3D11RasterizerState* CWcullMode;

	int life;

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

	/*------*/


	XMVECTOR pos= XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	bool InitCube();
	bool InitMd5();
	bool InitSdkmesh(std::wstring filename, std::shared_ptr<EffectFactory> fa, std::shared_ptr<CommonStates> state);
	bool InitSky();

	void UpdateMatrix();
	void DrawMd5();
	void DrawCube();
	void DrawSdkmesh();

	int NumSphereVertices;
	int NumSphereFaces;
	ID3D11VertexShader* SKYMAP_VS;
	ID3D11PixelShader* SKYMAP_PS;
	ID3D10Blob* SKYMAP_VS_Buffer;
	ID3D10Blob* SKYMAP_PS_Buffer;
	XMMATRIX Rotationx;
	XMMATRIX Rotationz;
	XMMATRIX Rotationy;
	ID3D11DepthStencilState* DSLessEqual;
	ID3D11ShaderResourceView* smrv;
	ID3D11Buffer* sphereIndexBuffer;
	ID3D11Buffer* sphereVertBuffer;
	XMMATRIX sphereWorld;
	/*----*/
public:
	std::unique_ptr<Model>                m_model;
	std::shared_ptr<CommonStates>        m_states;

	bool LoadMD5Model(std::wstring filename,
		Model3D& MD5Model,
		std::vector<ID3D11ShaderResourceView*>& shaderResourceViewArray,
		std::vector<std::wstring> texFileNameArray);
	
	bool LoadMD5Anim(std::wstring filename, Model3D& MD5Model);
	void UpdateMD5Model(Model3D& MD5Model, float deltaTime, int animation);
	void Draw();
	void DrawSky();
};

//this is another test
