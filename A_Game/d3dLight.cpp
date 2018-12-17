#include "d3dUtility.h"
#include "d3dLight.h"


ID3D11Device* device = NULL;                    //D3D11设备指针
IDXGISwapChain* swapChain = NULL;               //交换链指针
ID3D11DeviceContext* immediateContext = NULL;   //设备上下文指针
ID3D11RenderTargetView* renderTargetView = NULL;//渲染目标视图指针  

//着色器
ID3D11VertexShader* m_VertexShader;
ID3D11PixelShader* m_PixelShader;

//Effect相关全局指针
ID3D11InputLayout* vertexLayout;
ID3DX11Effect* effect;
ID3DX11EffectTechnique* technique;

//声明三个坐标系矩阵
XMMATRIX world;         //用于世界变换的矩阵
XMMATRIX view;          //用于观察变换的矩阵
XMMATRIX projection;    //用于投影变换的矩阵

//声明材质和光照的全局对象
Material		material;      //材质
Light			light[3];      //光源数组
int             lightType = 0;  //光源类型

ID3D11DepthStencilView* depthStencilView;  //深度模板视图
XMVECTOR		eyePositin;                //视点位置

//定义一个顶点结构，这个顶点包含坐标和法向量
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
};

void SetLightEffect(Light light);

//**************以下为框架函数******************
bool Setup()
{
	//这里主要包含3个主要步骤
	//第一步从.fx文件创建ID3DEffect对象
	//第二步创建顶点缓存以及顶点索引缓存
	//第三步设置变换坐标系
	//第四步设置材质和光照
	//*************第一步从.fx文件创建ID3DEffect对象****************************
	HRESULT hr = S_OK;              //声明HRESULT的对象用于记录函数调用是否成功
	ID3DBlob* pTechBlob = NULL;     //声明ID3DBlob的对象用于存放从文件读取的信息
	ID3DBlob* compilationMsgs = nullptr;
	//从我们之前建立的.fx文件读取着色器相关信息
	//hr = D3DX11CompileFromFile(L"lightShader.fx", NULL, NULL, NULL, "fx_5_0",
	//	D3DCOMPILE_ENABLE_STRICTNESS, 0, NULL, &pTechBlob, NULL, NULL);
	//if (FAILED(hr))
	//{
	//	::MessageBox(NULL, L"fx文件载入失败", L"Error", MB_OK); //如果读取失败，弹出错误信息
	//	return hr;
	//}
	////调用D3DX11CreateEffectFromMemory创建ID3DEffect对象
	//hr = D3DX11CreateEffectFromMemory(pTechBlob->GetBufferPointer(),
	//	pTechBlob->GetBufferSize(), 0, device, &effect);
	hr = D3DX11CompileEffectFromFile(
		L"Shader/LightShader.fx",
		nullptr, nullptr,
		D3DCOMPILE_ENABLE_STRICTNESS, 0, device, &effect, &compilationMsgs);
	if (FAILED(hr))
	{
		::MessageBox(NULL, L"创建Effect失败", L"Error", MB_OK);  //创建失败，弹出错误信息
		return hr;
	}
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
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	//layout元素个数
	UINT numElements = ARRAYSIZE(layout);
	//调用CreateInputLayout创建输入布局
	hr = device->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &vertexLayout);
	//设置生成的输入布局到设备上下文中
	immediateContext->IASetInputLayout(vertexLayout);
	if (FAILED(hr))
	{
		::MessageBox(NULL, L"创建Input Layout失败", L"Error", MB_OK);
		return hr;
	}
	//*************第一步从.fx文件创建ID3DEffect对象****************************

	//*************第二步创建顶点缓存以及顶点索引缓存***************************
	//和实验4一样，创建顶点数组，由于每个顶点有包含了坐标和颜色
	//但是由于顶点的法向量不同，所以即使是同一个位置的三个顶点也必须单独定义
	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	};
	UINT vertexCount = ARRAYSIZE(vertices);
	//创建顶点缓存，方法同实验4一样
	//首先声明一个D3D11_BUFFER_DESC的对象bd
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * 24;      //注意：由于这里定义了24个顶点所以要乘以24
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;  //注意：这里表示创建的是顶点缓存
	bd.CPUAccessFlags = 0;

	//声明一个D3D11_SUBRESOURCE_DATA数据用于初始化子资源
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;         //设置需要初始化的数据，这里的数据就是顶点数组

	//声明一个ID3D11Buffer对象作为顶点缓存
	ID3D11Buffer* vertexBuffer;
	//调用CreateBuffer创建顶点缓存
	hr = device->CreateBuffer(&bd, &InitData, &vertexBuffer);
	if (FAILED(hr))
	{
		::MessageBox(NULL, L"创建VertexBuffer失败", L"Error", MB_OK);
		return hr;
	}
	//设置索引数组
	//注意：数组里的每一个数字表示顶点数组的对应下标的顶点。
	//      由于立方体由12个三角形组成，所以共需要36个顶点
	//      这里每三个数字构成一个三角形
	WORD indices[] =
	{
		3,1,0,    2,1,3,    6,4,5,    7,4,6,    11,9,8,    10,9,11,
		14,12,13, 15,12,14, 19,17,16, 18,17,19, 22,20,21,  23,20,22
	};
	UINT indexCount = ARRAYSIZE(indices);

	//创建索引缓存
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 36;        // 由于有36个顶点所以这里要乘以36
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;    // 注意：这里表示创建的是索引缓存
	bd.CPUAccessFlags = 0;

	InitData.pSysMem = indices;                //设置需要初始化的数据，这里的数据就是索引数组
	ID3D11Buffer* indexBuffer;                 //声明一个ID3D11Buffer对象作为索引缓存
	//调用CreateBuffer创建索引缓存
	hr = device->CreateBuffer(&bd, &InitData, &indexBuffer);
	if (FAILED(hr))
	{
		::MessageBox(NULL, L"创建IndexBuffer失败", L"Error", MB_OK);
		return hr;
	}
	UINT stride = sizeof(Vertex);                 //获取Vertex的大小作为跨度
	UINT offset = 0;                              //设置偏移量为0
	//设置顶点缓存，参数的解释见实验4
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//设置索引缓存
	immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	//指定图元类型，D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST表示图元为三角形
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//*************第二步创建顶点缓存以及顶点索引缓存****************************

	//*************第三步设置变换坐标系***********************
	//初始化世界矩阵
	world = XMMatrixIdentity();

	//初始化观察矩阵
	XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);//相机位置
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);  //目标位置
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);  //up
	view = XMMatrixLookAtLH(Eye, At, Up);   //设置观察坐标系

	//设置投影矩阵
	projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 800.0f / 600.0f, 0.01f, 100.0f);
	//*************第三步设置变换坐标系***********************

	//*************第四步设置材质和光照***********************
	// 设置材质：3中光照的反射率以及镜面光反射系数
	//反射率中前三位表示红绿蓝光的反射率，1表示完全反射，0表示完全吸收
	material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); //前三位分别表示红绿蓝光的反射率
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); //同上
	material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);//同上
	material.power = 5.0f;

	// 设置光源
	Light dirLight, pointLight, spotLight;
	// 方向光只需要设置：方向、3种光照强度
	dirLight.type = 0;
	dirLight.direction = XMFLOAT4(-1.0f, 0.0f, 1.0f, 1.0f); //光照方向
	dirLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);   //前三位分别表示红绿蓝光的强度
	dirLight.diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);   //同上
	dirLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);  //同上

	// 点光源需要设置：位置、3中光照强度、3个衰减因子
	pointLight.type = 1;
	pointLight.position = XMFLOAT4(0.0f, 2.0f, -5.0f, 1.0f); //光源位置
	pointLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);   //前三位分别表示红绿蓝光的强度
	pointLight.diffuse = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);   //同上
	pointLight.specular = XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f);  //同上
	pointLight.attenuation0 = 0;      //常量衰减因子
	pointLight.attenuation1 = 0.1f;   //一次衰减因子
	pointLight.attenuation2 = 0;      //二次衰减因子

	// 聚光灯需要设置Light结构中所有的成员
	spotLight.type = 2;
	spotLight.position = XMFLOAT4(0.0f, 0.0f, -3.0f, 1.0f); //光源位置
	spotLight.direction = XMFLOAT4(0.4f, 0.0f, 1.0f, 1.0f); //光照方向
	spotLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);   //前三位分别表示红绿蓝光的强度
	spotLight.diffuse = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);   //同上
	spotLight.specular = XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f);  //同上
	spotLight.attenuation0 = 0;    //常量衰减因子
	spotLight.attenuation1 = 0.1f; //一次衰减因子
	spotLight.attenuation2 = 0;    //二次衰减因子
	spotLight.alpha = XM_PI / 6;   //内锥角度
	spotLight.beta = XM_PI / 3;    //外锥角度
	spotLight.fallOff = 1.0f;      //衰减系数，一般为1.0

	light[0] = dirLight;
	light[1] = pointLight;
	light[2] = spotLight;
	//*************第四步设置材质和光照***********************
	return true;
}

void Cleanup()
{
	//释放全局指针
	if (renderTargetView) renderTargetView->Release();
	if (immediateContext) immediateContext->Release();
	if (swapChain) swapChain->Release();
	if (device) device->Release();
	if (m_VertexShader) m_VertexShader->Release();
	if (m_PixelShader) m_PixelShader->Release();

	if (vertexLayout) vertexLayout->Release();
	if (effect) effect->Release();
	if (depthStencilView) depthStencilView->Release();
}

bool Display(float timeDelta)
{
	if (device)
	{
		//声明一个数组存放颜色信息，4个元素分别表示红，绿，蓝以及alpha
		float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
		immediateContext->ClearRenderTargetView(renderTargetView, ClearColor);
		//清除当前绑定的深度模板视图
		immediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		// 每隔一段时间更新一次场景，以实现立方体的旋转.
		static float angle = 0.0f;   //声明一个静态变量用于记录角度
		angle += timeDelta;          //将当前角度加上一个时间差
		if (angle >= 6.28f)           //如果当前角度大于2PI，则归零
			angle = 0.0f;
		world = XMMatrixRotationY(angle);   //根据当前角度绕Y轴旋转，设置世界坐标系

		//将坐标变换矩阵的常量缓存中的矩阵和坐标设置到Effect框架中---------------------
		//注意：这里的"World"，"View"，"Projection"，"EyePosition"是在.fx文件中定义的
		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float*)&world);  //设置世界坐标系
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float*)&view);    //设置观察坐标系
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float*)&projection); //设置投影坐标系
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float*)&eyePositin); //设置视点

		//材质信息的常量缓存中的材质信息设置到Effect框架中-----------------------------
		effect->GetVariableByName("MatAmbient")->AsVector()->SetFloatVector((float*)&(material.ambient));
		effect->GetVariableByName("MatDiffuse")->AsVector()->SetFloatVector((float*)&(material.diffuse));
		effect->GetVariableByName("MatSpecular")->AsVector()->SetFloatVector((float*)&(material.specular));
		effect->GetVariableByName("MatPower")->AsScalar()->SetFloat(material.power);

		//光源的常量缓存中的光源信息设置到Effect框架中---------------------------------
		SetLightEffect(light[lightType]);

		//定义一个D3DX11_TECHNIQUE_DESC对象来描述technique
		D3DX11_TECHNIQUE_DESC techDesc;
		technique->GetDesc(&techDesc);    //获取technique的描述
		//获取通道（PASS）把它设置到设备上下文中。
		//这里由于只有一个通道所以其索引为0
		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(36, 0, 0);   //绘制立方体

		swapChain->Present(0, 0);
	}
	return true;
}
//**************框架函数******************


//
// 回调函数
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);

		if (wParam == VK_F1)  //按F1键将光源类型改为方向光
			lightType = 0;
		if (wParam == VK_F2)  //按F2键将光源类型改为点光源
			lightType = 1;
		if (wParam == VK_F3)  //按F3键将光源类型改为聚光灯光源
			lightType = 2;

		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// 主函数WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{

	//初始化
	//**注意**:最上面声明的IDirect3DDevice9指针，在这里作为参数传给InitD3D函数
	if (!d3d::InitD3D(hinstance,
		800,
		600,
		&renderTargetView,
		&immediateContext,
		&swapChain,
		&device))// [out]The created device.
	{
		::MessageBox(0, L"InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if (!Setup())
	{
		::MessageBox(0, L"Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop(Display);

	Cleanup();

	return 0;
}

//光源的常量缓存设置到Effect框架中
//由于光照设置比较复杂，所以以一个函数来进行设置
void SetLightEffect(Light light)
{
	//首先将光照类型，环境光强度，漫射光强度，镜面光强度设置到Effect中
	effect->GetVariableByName("type")->AsScalar()->SetInt(light.type);
	effect->GetVariableByName("LightAmbient")->AsVector()->SetFloatVector((float*)&(light.ambient));
	effect->GetVariableByName("LightDiffuse")->AsVector()->SetFloatVector((float*)&(light.diffuse));
	effect->GetVariableByName("LightSpecular")->AsVector()->SetFloatVector((float*)&(light.specular));

	//下面根据光照类型的不同设置不同的属性
	if (light.type == 0)  //方向光
	{
		//方向光只需要“方向”这个属性即可
		effect->GetVariableByName("LightDirection")->AsVector()->SetFloatVector((float*)&(light.direction));
		//将方向光的Tectnique设置到Effect
		technique = effect->GetTechniqueByName("T_DirLight");
	}
	else if (light.type == 1)  //点光源
	{
		//点光源需要“位置”，“常量衰变因子”，“一次衰变因子”，“二次衰变因子”
		effect->GetVariableByName("LightPosition")->AsVector()->SetFloatVector((float*)&(light.position));
		effect->GetVariableByName("LightAtt0")->AsScalar()->SetFloat(light.attenuation0);
		effect->GetVariableByName("LightAtt1")->AsScalar()->SetFloat(light.attenuation1);
		effect->GetVariableByName("LightAtt2")->AsScalar()->SetFloat(light.attenuation2);

		//将点光源的Tectnique设置到Effect
		technique = effect->GetTechniqueByName("T_PointLight");
	}
	else if (light.type == 2) //聚光灯光源
	{
		//点光源需要“方向”，“方向”，“常量衰变因子”，“一次衰变因子”，“二次衰变因子”
		//“内锥角度”，“外锥角度”，“聚光灯衰减系数”
		effect->GetVariableByName("LightPosition")->AsVector()->SetFloatVector((float*)&(light.position));
		effect->GetVariableByName("LightDirection")->AsVector()->SetFloatVector((float*)&(light.direction));

		effect->GetVariableByName("LightAtt0")->AsScalar()->SetFloat(light.attenuation0);
		effect->GetVariableByName("LightAtt1")->AsScalar()->SetFloat(light.attenuation1);
		effect->GetVariableByName("LightAtt2")->AsScalar()->SetFloat(light.attenuation2);

		effect->GetVariableByName("LightAlpha")->AsScalar()->SetFloat(light.alpha);
		effect->GetVariableByName("LightBeta")->AsScalar()->SetFloat(light.beta);
		effect->GetVariableByName("LightFallOff")->AsScalar()->SetFloat(light.fallOff);

		//将聚光灯光源的Tectnique设置到Effect
		technique = effect->GetTechniqueByName("T_SpotLight");
	}
}

