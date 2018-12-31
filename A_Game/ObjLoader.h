#ifndef OBJLOADER_H_H
#define OBJLOADER_H_H

#include "d3dUtility.h"
#include <string>
#include <vector>
#include <fstream>

/// mtl文件中物体的材质结构定义
class ObjMaterial
{
public:
	char						strName[1024];
	XMFLOAT4					vAmbient;	    //环境光反射率
	XMFLOAT4					vDiffuse;	    //散射光反射率
	XMFLOAT4					vSpecular;	    //镜面光反射率
	float						nShininess;		//镜面反射系数
	ID3D11ShaderResourceView*	pTextureRV;     //纹理对象

public:
	ObjMaterial();
	void Release();
};

/// 顶点数据结构定义
struct ObjVertex				//数据结构顺序，位置，法向量，纹理坐标（和d3dMesh.cpp文件里输入布局要一致）
{
	XMFLOAT3					vPosition;  //位置坐标
	
	XMFLOAT3					vNormal;    //法向量
	XMFLOAT2					vTexCoord;  //纹理坐标
};

/// 子网格结构定义
class ObjSubMesh
{
public:
	// 材质名称
	std::string					strMaterialName;
	// 纹理视图
	ID3D11ShaderResourceView*	pTextureRV;
	// 子集对应的索引开始序号
	unsigned long				lStartIndex;
	// 子集对应的索引数目
	unsigned long				lIndexCount;
public:
	ObjSubMesh();
	void Release();
};

/// OBJ模型载入类
class ObjLoader
{
public:
	// 创建模型的工具所使用的坐标系：左手坐标系或者右手坐标系
	enum
	{
		Left_Hand_System = 1,
		Right_Hand_System
	};
public:
	ObjLoader();
	ObjLoader(ID3DX11Effect* objeffect);
	// 从文件载入模型
	// resPath：模型文件路径
	// objFilename：obj文件名，不包含路径信息
	// nHandSystem: 模型记录的是左手坐标系下的数据还是右手坐标系下的数据
	bool Load(ID3D11Device* device, char* resPath, char* objFilename, int nHandSystem = Left_Hand_System);
	// 释放资源
	void Release();
	// 绘制模型
	void RenderEffect(ID3D11DeviceContext* deviceContext, ID3DX11EffectTechnique* pTechnique);

	ID3DX11Effect* GetRenderEffect() { return effect; }
	std::vector<ObjVertex> getVertexInfo();

	void setMaterialAlpha(float alpha);
private:
	// 提取顶点信息（位置、纹理、法线）
	bool ParseVertices(ID3D11Device* device, char* fileName, int nHandSystem);
	// 解析材质文件
	bool ParseMaterialFile(ID3D11Device* device, char* fileName);
	// 解析子网格（submesh）结构
	bool ParseSubset(char* fileName, int nHandSystem);
	// 创建顶点缓存和索引缓存
	bool CreateBuffers(ID3D11Device* device);

	

private:

	// 文件顶点数据信息
	std::vector<XMFLOAT3>		m_vertices; //坐标
	std::vector<XMFLOAT2>		m_textures; //纹理
	std::vector<XMFLOAT3>		m_normals;  //法向量
	// 网格顶点信息
	std::vector<ObjVertex>		m_meshVertices;	
	// 网格信息
	std::vector<ObjSubMesh>		m_Mesh;
	// 材质信息
	std::vector<ObjMaterial>	m_Material;
	// OBJ资源路径
	std::string m_strResPath;
	
	// 网格顶点缓存
	ID3D11Buffer				*m_meshVertexBuffer; 
	// 网格索引缓存
	ID3D11Buffer				*m_meshIndexBuffer;
	// Effect对象
	ID3DX11Effect* effect;

};

#endif