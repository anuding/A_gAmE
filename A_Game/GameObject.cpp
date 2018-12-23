#include "GameObject.h"
#include <DDSTextureLoader.h>
void GameObject::BOSS()
{
	int count = communicateList->size();

	for (std::vector<GameObject*>::iterator it = communicateList->begin(); it != communicateList->end(); it++)
	{
		if (strcmp((*it)->tag, "player") == 0)
		{
			//找到player对象,获取player的Pos
			XMVECTOR PlayerPos = (*it)->GetPos();
			XMVECTOR BossPos = this->GetPos();
			XMVECTOR Dir = XMVector3Normalize(PlayerPos - BossPos);
			XMFLOAT4 deltainDir;
			XMStoreFloat4(&deltainDir, Dir); //从Boss指向player的向量  

			XMFLOAT4 originPos;//Boss原本的位置
			XMStoreFloat4(&originPos, BossPos);
			//Boss原本的位置 + 从Boss指向player的向量*0.0001f = Boss应该移动到的位置

			float destX; float destY; float destZ;//Boss应该移动到的位置
			destX = originPos.x + deltainDir.x*0.0001f;
			destY = originPos.y + deltainDir.y*0.0001f;
			destZ = originPos.z + deltainDir.z*0.0001f;
			world = XMMatrixTranslation(destX, destY, destZ);
			SetWorldMatrix(world);
		}
	}
}
GameObject::GameObject(D3DUtility* app)
{
	mapp = app;
	ID3D11Device* device = app->device.Get();
	buildEffect(device);

	const WCHAR *pwcsName = L"box.dds";
	buildTexture(pwcsName);
	buildInputlayout(device);
	buildVertexBufferandIndicesBuffer(device);
	buildMaterialandLight();
	SetWorldMatrix(XMMatrixIdentity());
	
	
}

GameObject::~GameObject()
{

}

void GameObject::SetWorldMatrix(XMMATRIX mworld)
{
	world = mworld;
	XMVECTOR tmp = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	pos = XMVector4Transform(tmp, mworld);
	Setup();
}

void GameObject::SetViewMatrix(XMMATRIX mview)
{
	view = mview;
	Setup();
}

void GameObject::SetProjMatrix(XMMATRIX mproj)
{
	projection = mproj;
	Setup();
}




XMVECTOR GameObject::GetPos()
{
	return pos;
}

void GameObject::buildEffect(ID3D11Device* device)
{
	//1.Create effect instance from .fx
	HRESULT hr = S_OK;
	ID3DBlob* pTechBLob = NULL;
	ID3DBlob* compilationMsgs = nullptr;

	hr = D3DX11CompileEffectFromFile(
		L"Shader/TextureAndLightShader.fx",
		nullptr, nullptr,
		D3DCOMPILE_ENABLE_STRICTNESS, 0, device, &effect, &compilationMsgs);
	if (FAILED(hr))
	{
		//OutputDebugString((char*)compilationMsgs->GetBufferPointer());
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		::MessageBox(NULL, L"fx load failed!!", L"ERROR", MB_OK);
	}
}

void GameObject::buildInputlayout(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	//用GetTechniqueByName获取ID3DX11EffectTechnique的对象
//先设置默认的technique到Effect
	technique = effect->GetTechniqueByName("T0");                //默认Technique

	//D3DX11_PASS_DESC结构用于描述一个Effect Pass
	D3DX11_PASS_DESC PassDesc;
	//利用GetPassByIndex获取Effect Pass
	//再利用GetDesc获取Effect Pass的描述，并存如PassDesc对象中
	technique->GetPassByIndex(0)->GetDesc(&PassDesc);
	
	//创建并设置输入布局
	//这里我们定义一个D3D11_INPUT_ELEMENT_DESC数组，
	//由于我们定义的顶点结构包括位置坐标和法向量，所以这个数组里有两个元素
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
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
		//和实验4一样，创建顶点数组，由于每个顶点有包含了坐标和颜色
	//但是由于顶点的法向量不同，所以即使是同一个位置的三个顶点也必须单独定义
	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)  ,XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)  ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) ,XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)   , XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)  , XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)  , XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)  ,XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) ,XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)  , XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) , XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)   , XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)  ,XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)  ,XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) ,XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)  ,XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)    ,XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)    ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)   ,XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)     ,XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)    ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)    ,XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)  ,XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)  ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) ,XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)   ,XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)  ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)  ,XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)     ,XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)   ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)    ,XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)    ,XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)   ,XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)     ,XMFLOAT2(1.0f, 1.0f) },
	};
	UINT vertexCount = ARRAYSIZE(vertices);
	D3D11_BUFFER_DESC bd = {}; 
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;

	
	hr = device->CreateBuffer(&bd, &InitData, &vertexBuffer);
	if (FAILED(hr))
	{
		::MessageBox(NULL, L"Vertex Buffer creation failed!!", L"ERROR", MB_OK);
		
	}
}

void GameObject::buildMaterialandLight()
{
	//*************第四步设置材质和光照***********************
	// 设置材质：3中光照的反射率以及镜面光反射系数
	//反射率中前三位表示红绿蓝光的反射率，1表示完全反射，0表示完全吸收
	material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); //前三位分别表示红绿蓝光的反射率
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); //同上
	material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);//同上
	material.power = 5.0f;
	// 设置光源
	Light dirLight;
	// 方向光只需要设置：方向、3种光照强度
	dirLight.direction = XMFLOAT4(-1.0f, 0.0f, 1.0f, 1.0f); //光照方向
	dirLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);   //前三位分别表示红绿蓝光的强度
	dirLight.diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);   //同上
	dirLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);  //同上
	light = dirLight;
	//材质信息的常量缓存中的材质信息设置到Effect框架中-----------------------------
	effect->GetVariableByName("MatAmbient")->AsVector()->SetFloatVector((float*)&(material.ambient));
	effect->GetVariableByName("MatDiffuse")->AsVector()->SetFloatVector((float*)&(material.diffuse));
	effect->GetVariableByName("MatSpecular")->AsVector()->SetFloatVector((float*)&(material.specular));
	effect->GetVariableByName("MatPower")->AsScalar()->SetFloat(material.power);

	//首先将光照类型，环境光强度，漫射光强度，镜面光强度设置到Effect中
	effect->GetVariableByName("LightAmbient")->AsVector()->SetFloatVector((float*)&(light.ambient));
	effect->GetVariableByName("LightDiffuse")->AsVector()->SetFloatVector((float*)&(light.diffuse));
	effect->GetVariableByName("LightSpecular")->AsVector()->SetFloatVector((float*)&(light.specular));

	//方向光只需要“方向”这个属性即可
	effect->GetVariableByName("LightDirection")->AsVector()->SetFloatVector((float*)&(light.direction));
	//将方向光的Tectnique设置到Effect
	technique = effect->GetTechniqueByName("T_DirLight");
}

void GameObject::buildTexture(const wchar_t* filename)
{
	DirectX::CreateDDSTextureFromFile(mapp->device.Get(),
		filename, nullptr, &texture);

	//将纹理设置到“Texture”
	effect->GetVariableByName("Texture")->AsShaderResource()->SetResource(texture);
	technique = effect->GetTechniqueByName("T_DirLight");                //默认Technique

}

bool GameObject::Setup()
{
	effect->GetVariableByName("World")->AsMatrix()
		->SetMatrix((float*)&world);
	effect->GetVariableByName("View")->AsMatrix()
		->SetMatrix((float*)&view);
	effect->GetVariableByName("Projection")->AsMatrix()
		->SetMatrix((float*)&projection);
	mapp->immediateContext->IASetInputLayout(vertexLayout);
	mapp->immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	mapp->immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	mapp->immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	technique->GetDesc(&techDesc);
	return true;
}
