#include "GameObject.h"
#include "debugprint.h"
#include <DDSTextureLoader.h>




GameObject::GameObject(D3DUtility * app, TYPE modelType, std::wstring filename,char *tag)
{
	//CUBE 构造函数
	mapp = app;
	dev = app->device.Get();
	con = app->immediateContext.Get();
	type = modelType;
	strcpy_s(this->tag, tag);
	if (modelType == CUBE)
	{
		InitCube(filename);
	}
	if (modelType == FBX)
	{
		InitFBX(filename);
	}
}

GameObject::~GameObject()
{

}

void GameObject::SetWorldMatrix(XMMATRIX world)
{
	this->world = world;
	pos = XMVector4Transform(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), this->world);
}

void GameObject::SetViewProjMatrix(XMMATRIX view, XMMATRIX proj)
{
	cam_view = view;
	cam_projection = proj;
}



XMVECTOR GameObject::GetPos()
{
	return pos;
}


bool GameObject::InitCube(std::wstring filename)
{
	ID3DBlob* compilation_msgs = nullptr;
	HRESULT hr = S_OK;

	D3DCompileFromFile(L"Shader/CubeShader.fx", 0, 0, "VS", "vs_5_0", 0, 0, vs_buffer.GetAddressOf(), &compilation_msgs);
	D3DCompileFromFile(L"Shader/CubeShader.fx", 0, 0, "PS", "ps_5_0", 0, 0, ps_buffer.GetAddressOf(), &compilation_msgs);
	//Create the Shader Objects
	hr = dev->CreateVertexShader(vs_buffer->GetBufferPointer(), vs_buffer->GetBufferSize(), NULL,vertex_shader.GetAddressOf());
	hr = dev->CreatePixelShader(ps_buffer->GetBufferPointer(), ps_buffer->GetBufferSize(), NULL, pixel_shader.GetAddressOf());

	//create input layout
	dev->CreateInputLayout(layout, ARRAYSIZE(layout),
		vs_buffer->GetBufferPointer(), vs_buffer->GetBufferSize(), vertex_layout.GetAddressOf());

	VertexForCube vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f),XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT3(-1.0f, -1.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),XMFLOAT2(0.0f, 1.0f),  XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f)  },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f),  XMFLOAT3(1.0f, 1.0f, -1.0f),  XMFLOAT3(1.0f, 1.0f, -1.0f)   },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),XMFLOAT2(1.0f, 0.0f),  XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f)  },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f)  },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f),  XMFLOAT3(-1.0f, 1.0f, 1.0f),  XMFLOAT3(-1.0f, 1.0f, 1.0f)   },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),XMFLOAT2(1.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f)    },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),XMFLOAT2(1.0f, 0.0f),   XMFLOAT3(1.0f, -1.0f, 1.0f),  XMFLOAT3(1.0f, -1.0f, 1.0f)   }
	};

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(vertices);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA init_data;
	ZeroMemory(&init_data, sizeof(init_data));
	init_data.pSysMem = vertices;
	dev->CreateBuffer(&vbd, &init_data, vertex_buffer.GetAddressOf());

	// ******************
	// 索引数组
	WORD indices[] = {
		// 正面
		0, 1, 2,
		2, 3, 0,
		// 左面
		4, 5, 1,
		1, 0, 4,
		// 顶面
		1, 5, 6,
		6, 2, 1,
		// 背面
		7, 6, 5,
		5, 4, 7,
		// 右面
		3, 2, 6,
		6, 7, 3,
		// 底面
		4, 0, 3,
		3, 7, 4
	};
	// 设置索引缓冲区描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(indices);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	init_data.pSysMem = indices;
	dev->CreateBuffer(&ibd, &init_data, index_buffer.GetAddressOf());

	// ******************
	// 设置常量缓冲区描述
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(cbPerObject);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	// 新建常量缓冲区，不使用初始数据
	dev->CreateBuffer(&cbd, nullptr, perobject_buffer.GetAddressOf());

	//调用CreateDDSTextureFromFile从文件创建纹理
	DirectX::CreateDDSTextureFromFile(dev.Get(),
		filename.c_str(), nullptr, texture.GetAddressOf());

	return true;
}

void GameObject::DrawCube()
{
	cbPerObj.WVP = XMMatrixTranspose(world*cam_view*cam_projection);
	con->UpdateSubresource(perobject_buffer.Get(), 0, nullptr, &cbPerObj, 0, 0);

	// 输入装配阶段的索引缓冲区设置
	con->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	con->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	// 设置图元类型，设定输入布局
	con->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	con->IASetInputLayout(vertex_layout.Get());
	// 将着色器绑定到渲染管线
	con->VSSetShader(vertex_shader.Get(), nullptr, 0);
	// 将更新好的常量缓冲区绑定到顶点着色器
	con->VSSetConstantBuffers(0, 1, perobject_buffer.GetAddressOf());
	con->PSSetShader(pixel_shader.Get(), nullptr, 0);
	con->PSSetShaderResources(0, 1, texture.GetAddressOf());

	// 绘制立方体
	con->DrawIndexed(36, 0, 0);
}

bool GameObject::InitFBX(std::wstring filename)
{
	loader = new FBXloader();
	loader->LoadFBX();



	ID3DBlob* compilation_msgs = nullptr;
	HRESULT hr = S_OK;

	D3DCompileFromFile(L"Shader/CubeShader.fx", 0, 0, "VS", "vs_5_0", 0, 0, vs_buffer.GetAddressOf(), &compilation_msgs);
	D3DCompileFromFile(L"Shader/CubeShader.fx", 0, 0, "PS", "ps_5_0", 0, 0, ps_buffer.GetAddressOf(), &compilation_msgs);
	//Create the Shader Objects
	hr = dev->CreateVertexShader(vs_buffer->GetBufferPointer(), vs_buffer->GetBufferSize(), NULL, vertex_shader.GetAddressOf());
	hr = dev->CreatePixelShader(ps_buffer->GetBufferPointer(), ps_buffer->GetBufferSize(), NULL, pixel_shader.GetAddressOf());

	//create input layout
	dev->CreateInputLayout(layout, ARRAYSIZE(layout),
		vs_buffer->GetBufferPointer(), vs_buffer->GetBufferSize(), vertex_layout.GetAddressOf());

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = loader->vertices_size;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA init_data;
	ZeroMemory(&init_data, sizeof(init_data));
	init_data.pSysMem = loader->fbx_vertices;
	dev->CreateBuffer(&vbd, &init_data, vertex_buffer.GetAddressOf());

	// 设置索引缓冲区描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = loader->indices_size;
	num_indices = loader->indices_size / sizeof(int);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	init_data.pSysMem = loader->fbx_indices;
	dev->CreateBuffer(&ibd, &init_data, index_buffer.GetAddressOf());

	// ******************
	// 设置常量缓冲区描述
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(cbPerObject);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	// 新建常量缓冲区，不使用初始数据
	dev->CreateBuffer(&cbd, nullptr, perobject_buffer.GetAddressOf());

	////调用CreateDDSTextureFromFile从文件创建纹理
	DirectX::CreateDDSTextureFromFile(dev.Get(),
		filename.c_str(), nullptr, texture.GetAddressOf());


	 AnimStackNumber = loader->AnimStackNumber;    // 0 - 2
	FrameTime = loader->FrameTime;
	timeCount = loader->timeCount;
	start = loader->start;
	stop = loader->stop;
	 meshNode = loader->pFbxChildNode;
	 mesh = loader->pMesh;
	return true;
}

void GameObject::DrawFbx()
{
	cbPerObj.WVP = XMMatrixTranspose(world*cam_view*cam_projection);
	con->UpdateSubresource(perobject_buffer.Get(), 0, nullptr, &cbPerObj, 0, 0);

	// 输入装配阶段的索引缓冲区设置
	con->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	con->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	// 设置图元类型，设定输入布局
	con->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	con->IASetInputLayout(vertex_layout.Get());
	// 将着色器绑定到渲染管线
	con->VSSetShader(vertex_shader.Get(), nullptr, 0);
	// 将更新好的常量缓冲区绑定到顶点着色器
	con->VSSetConstantBuffers(0, 1, perobject_buffer.GetAddressOf());
	con->PSSetShader(pixel_shader.Get(), nullptr, 0);
	con->PSSetShaderResources(0, 1, texture.GetAddressOf());

	con->DrawIndexed(num_indices, 0, 0);
}


/*************************************************/
void GameObject::UpdateMatrix()
{
	if (type == CUBE)
	{
		scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		world = scale * world;
	}
	if (type == FBX)
	{
		scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		world = scale * world;
	}

}

void GameObject::AnimeFbx()
{

	// ----- Animation -----
	timeCount += FrameTime;
	if (timeCount > stop) timeCount = start;

	// 移動、回転、拡大のための行列を作成
	FbxMatrix globalPosition = meshNode->EvaluateGlobalTransform(timeCount);
	FbxVector4 t0 = meshNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 r0 = meshNode->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 s0 = meshNode->GetGeometricScaling(FbxNode::eSourcePivot);
	FbxAMatrix geometryOffset = FbxAMatrix(t0, r0, s0);

	// 各頂点に掛けるための最終的な行列の配列
	FbxMatrix *clusterDeformation = new FbxMatrix[loader->vertices_size/sizeof(VertexForCube)];
	memset(clusterDeformation, 0, sizeof(FbxMatrix) * mesh->GetControlPointsCount());

	FbxSkin *skinDeformer = (FbxSkin *)mesh->GetDeformer(0, FbxDeformer::eSkin);
	int clusterCount = skinDeformer->GetClusterCount();
	// 各クラスタから各頂点に影響を与えるための行列作成
	for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++) {
		// クラスタ(ボーン)の取り出し
		FbxCluster *cluster = skinDeformer->GetCluster(clusterIndex);
		FbxMatrix vertexTransformMatrix;
		FbxAMatrix referenceGlobalInitPosition;
		FbxAMatrix clusterGlobalInitPosition;
		FbxMatrix clusterGlobalCurrentPosition;
		FbxMatrix clusterRelativeInitPosition;
		FbxMatrix clusterRelativeCurrentPositionInverse;
		cluster->GetTransformMatrix(referenceGlobalInitPosition);
		referenceGlobalInitPosition *= geometryOffset;
		cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);
		clusterGlobalCurrentPosition = cluster->GetLink()->EvaluateGlobalTransform(timeCount);
		clusterRelativeInitPosition = clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition;
		clusterRelativeCurrentPositionInverse = globalPosition.Inverse() * clusterGlobalCurrentPosition;
		vertexTransformMatrix = clusterRelativeCurrentPositionInverse * clusterRelativeInitPosition;
		// 上で作った行列に各頂点毎の影響度(重み)を掛けてそれぞれに加算
		for (int i = 0; i < cluster->GetControlPointIndicesCount(); i++) {
			int index = cluster->GetControlPointIndices()[i];
			double weight = cluster->GetControlPointWeights()[i];
			FbxMatrix influence = vertexTransformMatrix * weight;
			clusterDeformation[index] += influence;
		}
	}

	// 最終的な頂点座標を計算しVERTEXに変換
	for (int i = 0; i < mesh->GetControlPointsCount(); i++) {
		FbxVector4 outVertex = clusterDeformation[i].MultNormalize(mesh->GetControlPointAt(i));
		loader->fbx_vertices[i].pos.x = (FLOAT)outVertex[0] * 0.6f;
		loader->fbx_vertices[i].pos.y = (FLOAT)outVertex[1] * 0.6f;
		loader->fbx_vertices[i].pos.z = (FLOAT)outVertex[2] * 0.6f;
	}

	delete[] clusterDeformation;
	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = loader->vertices_size;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA init_data;
	ZeroMemory(&init_data, sizeof(init_data));
	init_data.pSysMem = loader->fbx_vertices;
	dev->CreateBuffer(&vbd, &init_data, vertex_buffer.GetAddressOf());
	// ---------------------
}

void GameObject::Draw()
{
	UpdateMatrix();
	if (type == CUBE)
	{
		DrawCube();
	}
	if (type == FBX)
	{
		AnimeFbx();
		DrawFbx();
	}
}

