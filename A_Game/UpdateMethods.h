
bool playerIdleLock = false;
bool rightLock = false;
bool leftLock = false;
bool frontLock = false;
bool backLock = false;
std::vector<ID2D1Bitmap*>maplist;
std::vector<ID2D1Bitmap*>maplist1;
std::wstring printText;
ID2D1SolidColorBrush *Brush;
IDWriteTextFormat *TextFormat;
int Width = 800;
int Height = 600;

ID2D1Bitmap* pBitmap = NULL;


IWICImagingFactory* pIWICFactory = NULL;
IWICBitmapDecoder* pDecoder = NULL;
IWICBitmapFrameDecode* pFrame = NULL;
IWICFormatConverter* pConverter = NULL;
//ID2D1RenderTarget *D2DRenderTarget;
/*----*/
IDXGIKeyedMutex *keyedMutex11;
IDXGIKeyedMutex *keyedMutex10;
ID3D11PixelShader* D2D_PS;

int pos1 = 0;
int pos2 = 0;

ID3D11BlendState* d2dTransparency;
ID3D11Buffer *d2dVertBuffer;
ID3D11Buffer *d2dIndexBuffer;
ID3D11Texture2D *sharedTex11;
ID3D11ShaderResourceView *d2dTexture;
ID3D10Blob* D2D_PS_Buffer;
ID3D11Buffer* cbPerObjectBuffer;
ID3D11SamplerState* CubesTexSamplerState;
ID3D11RasterizerState* CWcullMode;
ID3D11BlendState* Transparency;
Microsoft::WRL::ComPtr<ID3D11Texture2D> SMTexture = nullptr;
ID3D11RasterizerState* CCWcullMode;
ID3D11RasterizerState* RSCullNone;

ID3D11DepthStencilState* DSLessEqual;
ID3D11ShaderResourceView* smrv;
ID3D10Device1 *d3d101Device;
IDWriteFactory *DWriteFactory;
IDXGIAdapter1 *Adapter;

ID2D1Factory*			g_pD2DFactory = NULL;	// Direct2D factory
ID2D1HwndRenderTarget*	g_pRenderTarget = NULL;	// Render target
ID2D1RenderTarget *D2DRenderTarget;
ID2D1SolidColorBrush*	g_pBlackBrush = NULL;	// A black brush, reflect the line color

void CreateD2DResource(HWND hWnd)
{
	if (!g_pRenderTarget)
	{
		HRESULT hr;

		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
		if (FAILED(hr))
		{
			MessageBox(hWnd, L"Create D2D factory failed!", L"Error", 0);
			return;
		}

		// Obtain the size of the drawing area
		RECT rc;
		GetClientRect(hWnd, &rc);

		// Create a Direct2D render target
		hr = g_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				hWnd,
				D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
			),
			&g_pRenderTarget
		);
		if (FAILED(hr))
		{
			MessageBox(hWnd, L"Create render target failed!", L"Error", 0);
			return;
		}

		// Create a brush
		hr = g_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&g_pBlackBrush
		);
		if (FAILED(hr))
		{
			MessageBox(hWnd, L"Create brush failed!", L"Error", 0);
			return;
		}
	}
}
void  drawbitmap()
{
	/*g_pRenderTarget->BeginDraw();

	D2D1_SIZE_F size = pBitmap->GetSize();
	size.height *= 2.5;
	size.width *= 2.5;
	D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(50.f, 50.f);

	//draw bitmap
	g_pRenderTarget->DrawBitmap(
		maplist[pos1],
		D2D1::RectF(
			upperLeftCorner.x,
			upperLeftCorner.y,
			upperLeftCorner.x + size.width / 10,
			upperLeftCorner.y + size.height / 10)
	);


	/*------------------------*/
	/*------------22222----------*/
	/*
	size = pBitmap->GetSize();
	size.height *= 2.5;
	size.width *= 2.5;
	upperLeftCorner = D2D1::Point2F(50.f, 380.f);


	//draw bitmap
	g_pRenderTarget->DrawBitmap(
		maplist1[pos2],
		D2D1::RectF(
			upperLeftCorner.x,
			upperLeftCorner.y,
			upperLeftCorner.x + size.width / 2,
			upperLeftCorner.y + size.height / 2)
	);



	HRESULT hr = g_pRenderTarget->EndDraw();*/

	g_pRenderTarget->BeginDraw();

	// Clear background color to White
	//g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	// Draw Rectangle
	g_pRenderTarget->DrawRectangle(
		D2D1::RectF(100.f, 100.f, 500.f, 500.f),
		g_pBlackBrush
	);

	HRESULT hr = g_pRenderTarget->EndDraw();
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Draw failed!", L"Error", 0);
		return;
	}
}

bool InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter, D3DUtility* mapp)
{
	HRESULT hr;
	//Create our Direc3D 10.1 Device///////////////////////////////////////////////////////////////////////////////////////
	hr = D3D10CreateDevice1(Adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &d3d101Device);

	//Create Shared Texture that Direct3D 10.1 will render on//////////////////////////////////////////////////////////////
	D3D11_TEXTURE2D_DESC sharedTexDesc;

	ZeroMemory(&sharedTexDesc, sizeof(sharedTexDesc));

	sharedTexDesc.Width = Width;
	sharedTexDesc.Height = Height;
	sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedTexDesc.MipLevels = 1;
	sharedTexDesc.ArraySize = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	hr = mapp->device->CreateTexture2D(&sharedTexDesc, NULL, &sharedTex11);

	// Get the keyed mutex for the shared texture (for D3D11)///////////////////////////////////////////////////////////////
	hr = sharedTex11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex11);

	// Get the shared handle needed to open the shared texture in D3D10.1///////////////////////////////////////////////////
	IDXGIResource *sharedResource10;
	HANDLE sharedHandle10;

	hr = sharedTex11->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);

	hr = sharedResource10->GetSharedHandle(&sharedHandle10);

	sharedResource10->Release();

	// Open the surface for the shared texture in D3D10.1///////////////////////////////////////////////////////////////////
	IDXGISurface1 *sharedSurface10;

	hr = d3d101Device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void**)(&sharedSurface10));


	hr = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex10);

	// Create D2D factory///////////////////////////////////////////////////////////////////////////////////////////////////
	ID2D1Factory *D2DFactory;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&D2DFactory);

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;

	ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

	hr = D2DFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProperties, &D2DRenderTarget);

	sharedSurface10->Release();
	D2DFactory->Release();

	// Create a solid color brush to draw something with		
	hr = D2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &Brush);

	//DirectWrite///////////////////////////////////////////////////////////////////////////////////////////////////////////
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&DWriteFactory));

	hr = DWriteFactory->CreateTextFormat(
		L"Script",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24.0f,
		L"en-us",
		&TextFormat
	);

	hr = TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	hr = TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	d3d101Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	return true;
}


void UpdateFist(std::vector<GameObject*> GameObjectList)
{
//	//计算player到Boss的方向
//	GameObject* bo = GameObjectList[1];
//	GameObject* pl = GameObjectList[0];
//	XMVECTOR PlayerPos = pl->GetPos();
//	XMVECTOR BossPos = bo->GetPos();
//
//	XMFLOAT4 originPos;//Boss原本的位置
//	XMStoreFloat4(&originPos, pl->GetPos());
//	XMVECTOR Dir = BossPos - PlayerPos;
//
//	XMVECTOR mY = XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f);
//	XMVECTOR front = XMVector3Normalize(Dir);
//	XMVECTOR back = -front;
//	XMVECTOR right = XMVector3Cross(Dir, mY);
//	XMVECTOR left = -right;
//	//float playerSpeed = 1.0f;
//	XMFLOAT4 deltainDir;
////	XMStoreFloat4(&deltainDir, moveVector); //从Boss指向player的向量  
//	float destX; float destY; float destZ;//Boss应该移动到的位置
//	destX = originPos.x + deltainDir.x*0.0007f;
//	destY = originPos.y + deltainDir.y*0.0007f;
//	destZ = originPos.z + deltainDir.z*0.0007f;
//
//	//rotating
//	XMFLOAT4 plPosf;
//	XMStoreFloat4(&plPosf, PlayerPos);
//	XMFLOAT4 boPosf;
//	XMStoreFloat4(&boPosf, BossPos);
//
//	XMVECTOR x = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//	XMMATRIX world = XMMatrixTranslation(destX, destY, destZ);
//	pl->UpdateMD5Model(pl->NewMD5Model, 0.0005f, 0);
//	XMVECTOR angleVec = XMVector3AngleBetweenVectors(Dir, x);
//	float angleRadians = XMVectorGetX(angleVec);
//	if (plPosf.z <= boPosf.z)
//		world = XMMatrixRotationY(-angleRadians)*XMMatrixTranslation(destX, destY, destZ);
//	else
//		world = XMMatrixRotationY(angleRadians)*XMMatrixTranslation(destX, destY, destZ);
//	pl->SetWorldMatrix(world);
}

void UpdatePlayer(std::vector<GameObject*> GameObjectList)
{
	//计算player到Boss的方向
	GameObject* bo = GameObjectList[1];
	GameObject* pl = GameObjectList[0];
	XMVECTOR PlayerPos = pl->GetPos();
	XMVECTOR BossPos = bo->GetPos();

	XMFLOAT4 originPos;//Boss原本的位置
	XMStoreFloat4(&originPos, pl->GetPos());
	XMVECTOR Dir = BossPos - PlayerPos;

	XMVECTOR mY = XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f);
	XMVECTOR front = XMVector3Normalize(Dir);
	XMVECTOR back = -front;
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(Dir, mY));
	XMVECTOR left = -right;
	//float playerSpeed = 1.0f;

	XMVECTOR moveVector = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	float camSpeed = 1.0f / 360;
	if (::GetAsyncKeyState('A') & 0x8000f) //响应键盘左方向键
	{
		moveVector += left; playerIdleLock = true; leftLock = true;
	}
	if (::GetAsyncKeyState('D') & 0x8000f) //响应键盘右方向键
	{
		moveVector += right; playerIdleLock = true; rightLock = true;
	}
	if (::GetAsyncKeyState('W') & 0x8000f)    //响应键盘上方向键
	{
		moveVector += front; playerIdleLock = true;	frontLock = true;
	}
	if (::GetAsyncKeyState('S') & 0x8000f)  //响应键盘下方向键
	{
		moveVector += back; playerIdleLock = true;	backLock = true;
	}


	XMFLOAT4 deltainDir;
	XMStoreFloat4(&deltainDir, moveVector); //从Boss指向player的向量  
	float destX; float destY; float destZ;//Boss应该移动到的位置
	destX = originPos.x + deltainDir.x*0.0020f;
	destY = originPos.y + deltainDir.y*0.0007f;
	destZ = originPos.z + deltainDir.z*0.0020f;

	//rotating
	XMFLOAT4 plPosf;
	XMStoreFloat4(&plPosf, PlayerPos);
	XMFLOAT4 boPosf;
	XMStoreFloat4(&boPosf, BossPos);

	XMVECTOR x = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMMATRIX world = XMMatrixTranslation(destX, destY, destZ);

	if (!playerIdleLock)//没有锁, 开始idle
	{
		pl->UpdateMD5Model(pl->NewMD5Model, 0.0005f, 0);
		XMVECTOR angleVec = XMVector3AngleBetweenVectors(Dir, x);
		float angleRadians = XMVectorGetX(angleVec);
		if (plPosf.z <= boPosf.z)
			world = XMMatrixRotationY(-angleRadians)*XMMatrixTranslation(destX, destY, destZ);
		else
			world = XMMatrixRotationY(angleRadians)*XMMatrixTranslation(destX, destY, destZ);
	}
	else//有锁, 计算旋转角度
	{
		pl->UpdateMD5Model(pl->NewMD5Model, 0.0005f, 1);

		if (frontLock)
		{
			XMVECTOR angleVec = XMVector3AngleBetweenVectors(front, x);
			float angleRadians = XMVectorGetX(angleVec);
			if (plPosf.z <= boPosf.z)
				world = XMMatrixRotationY(-angleRadians)*XMMatrixTranslation(destX, destY, destZ);
			else
				world = XMMatrixRotationY(angleRadians)*XMMatrixTranslation(destX, destY, destZ);

		}
		if (backLock)
		{
			XMVECTOR angleVec = XMVector3AngleBetweenVectors(back, x);
			float angleRadians = XMVectorGetX(angleVec);
			if (plPosf.z <= boPosf.z)
				world = XMMatrixRotationY(angleRadians)*XMMatrixTranslation(destX, destY, destZ);
			else
				world = XMMatrixRotationY(-angleRadians)*XMMatrixTranslation(destX, destY, destZ);
		}
		if (rightLock)
		{
			XMVECTOR angleVec = XMVector3AngleBetweenVectors(right, x);
			float angleRadians = XMVectorGetX(angleVec);
			if (plPosf.y - boPosf.y > -(plPosf.x - boPosf.x))
				world = XMMatrixRotationY(-angleRadians)*XMMatrixTranslation(destX, destY, destZ);
			else
				world = XMMatrixRotationY(angleRadians)*XMMatrixTranslation(destX, destY, destZ);
		}
		if (leftLock)
		{
			XMVECTOR angleVec = XMVector3AngleBetweenVectors(right, x);
			float angleRadians = XMVectorGetX(angleVec);
			if (plPosf.y - boPosf.y > -(plPosf.x - boPosf.x))
				world = XMMatrixRotationY(-angleRadians + XM_PI)*XMMatrixTranslation(destX, destY, destZ);
			else
				world = XMMatrixRotationY(angleRadians + XM_PI)*XMMatrixTranslation(destX, destY, destZ);
		}
	}
	//world = XMMatrixTranslation(destX, destY, destZ);
	pl->SetWorldMatrix(world);
	playerIdleLock = false;
	rightLock = false;
	leftLock = false;
	frontLock = false;
	backLock = false;
}


void UpdateBoss(std::vector<GameObject*> GameObjectList)
{	//计算player到Boss的方向
	GameObject* bo = GameObjectList[1];
	GameObject* pl = GameObjectList[0];

	if (bo->life < 0) {
		bo->UpdateMD5Model(bo->NewMD5Model, 0.0005f, 2);
		return;
	}
	XMVECTOR PlayerPos = pl->GetPos();
	XMVECTOR BossPos = bo->GetPos();
	XMVECTOR Dir = XMVector3Normalize(PlayerPos - BossPos);
	XMFLOAT4 deltainDir;
	XMStoreFloat4(&deltainDir, Dir);

	XMFLOAT4 originPos;
	XMStoreFloat4(&originPos, BossPos);

	float destX; float destY; float destZ;
	destX = originPos.x + deltainDir.x*0.0012f;
	destY = originPos.y + deltainDir.y;
	destZ = originPos.z + deltainDir.z*0.0012f;


	XMFLOAT4 plPosf;
	XMStoreFloat4(&plPosf, PlayerPos);
	XMFLOAT4 boPosf;
	XMStoreFloat4(&boPosf, BossPos);


	XMVECTOR x = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	bo->UpdateMD5Model(bo->NewMD5Model, 0.0005f, 0);
	XMMATRIX world = XMMatrixIdentity();
	if (plPosf.z <= boPosf.z)
	{
		XMVECTOR angleVec = XMVector3AngleBetweenVectors(Dir, x);
		float angleRadians = XMVectorGetX(angleVec);
		world = XMMatrixRotationY(angleRadians)*XMMatrixTranslation(destX, destY, destZ);//
	}
	else
	{
		XMVECTOR angleVec = XMVector3AngleBetweenVectors(-Dir, x);
		float angleRadians = XMVectorGetX(angleVec);
		world = XMMatrixRotationY(angleRadians + XM_PI)*XMMatrixTranslation(destX, destY, destZ);//
	}
	bo->SetWorldMatrix(world);
}



//void UpdateWeapon(std::vector<GameObject*> GameObjectList)
//{
//
void UpdateBoss1(std::vector<GameObject*> GameObjectList)
{	//计算player到Boss的方向
	GameObject* bo = GameObjectList[1];
	GameObject* pl = GameObjectList[0];

	if (bo->life < 0) {
		bo->UpdateMD5Model(bo->NewMD5Model, 0.0005f, 2);
		return;
	}
	XMVECTOR PlayerPos = pl->GetPos();
	XMVECTOR BossPos = bo->GetPos();
	XMVECTOR Dir = XMVector3Normalize(PlayerPos - BossPos);
	XMFLOAT4 deltainDir;
	XMStoreFloat4(&deltainDir, Dir);

	XMFLOAT4 originPos;
	XMStoreFloat4(&originPos, BossPos);

	float destX; float destY; float destZ;
	destX = originPos.x;// +deltainDir.x*0.0002f;
	destY = originPos.y;// +deltainDir.y;
	destZ = originPos.z;// +deltainDir.z*0.0002f;


	XMFLOAT4 plPosf;
	XMStoreFloat4(&plPosf, PlayerPos);
	XMFLOAT4 boPosf;
	XMStoreFloat4(&boPosf, BossPos);


	XMVECTOR x = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	bo->UpdateMD5Model(bo->NewMD5Model, 0.0005f, 1);
	XMMATRIX world = XMMatrixIdentity();
	if (plPosf.z <= boPosf.z)
	{
		XMVECTOR angleVec = XMVector3AngleBetweenVectors(Dir, x);
		float angleRadians = XMVectorGetX(angleVec);
		world = XMMatrixRotationY(angleRadians)*XMMatrixTranslation(destX, destY, destZ);//
	}
	else
	{
		XMVECTOR angleVec = XMVector3AngleBetweenVectors(-Dir, x);
		float angleRadians = XMVectorGetX(angleVec);
		world = XMMatrixRotationY(angleRadians + XM_PI)*XMMatrixTranslation(destX, destY, destZ);//
	}
	bo->SetWorldMatrix(world);
}



void RenderText(ID3D11DeviceContext *d3d11DevCon,std::wstring text, int inInt)
{

	d3d11DevCon->PSSetShader(D2D_PS, 0, 0);

	//Release the D3D 11 Device
	keyedMutex11->ReleaseSync(0);
	//Use D3D10.1 device
	keyedMutex10->AcquireSync(0, 5);
	//Draw D2D content		
	D2DRenderTarget->BeginDraw();

	//Clear D2D Background
	D2DRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	/*------------------------*/
	//clear screen
	//D2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));


	//get bimap size
	D2D1_SIZE_F size = pBitmap->GetSize();
	size.height *= 2.5;
	size.width *= 2.5;
	D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(50.f, 380.f);


	//draw bitmap
	D2DRenderTarget->DrawBitmap(
		maplist[pos1],
		D2D1::RectF(
			upperLeftCorner.x,
			upperLeftCorner.y,
			upperLeftCorner.x + size.width / 2,
			upperLeftCorner.y + size.height / 2)
	);


	/*------------------------*/
	/*------------22222----------*/

	size = pBitmap->GetSize();
	size.height *= 2.5;
	size.width *= 2.5;
	upperLeftCorner = D2D1::Point2F(50.f, 380.f);


	//draw bitmap
	D2DRenderTarget->DrawBitmap(
		maplist1[pos2],
		D2D1::RectF(
			upperLeftCorner.x,
			upperLeftCorner.y,
			upperLeftCorner.x + size.width / 2,
			upperLeftCorner.y + size.height / 2)
	);





	/*--------------*/
	//Create our string
	std::wostringstream printString;
	printString << text << inInt;
	printText = printString.str();

	//Set the Font Color
	D2D1_COLOR_F FontColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);

	//Set the brush color D2D will use to draw with
	Brush->SetColor(FontColor);

	//Create the D2D Render Area
	D2D1_RECT_F layoutRect = D2D1::RectF(50, 50, Width, Height);//left/top/right/bottom

	//Draw the Text
	D2DRenderTarget->DrawText(
		printText.c_str(),
		wcslen(printText.c_str()),
		TextFormat,
		layoutRect,
		Brush
	);
	D2D1_RECT_F layoutRect1 = D2D1::RectF(0, 0, Width, Height);//left/top/right/bottom
	D2D1_RECT_F layoutRect2 = D2D1::RectF(0, 0, Width, Height);//left/top/right/bottom

	//D2DRenderTarget->DrawBitmap(mybitmap,layoutRect1,1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
	//	layoutRect2);

	D2DRenderTarget->EndDraw();

	//Release the D3D10.1 Device
	keyedMutex10->ReleaseSync(1);

	//Use the D3D11 Device
	keyedMutex11->AcquireSync(1, 5);

	//Use the shader resource representing the direct2d render target
	//to texture a square which is rendered in screen space so it
	//overlays on top of our entire scene. We use alpha blending so
	//that the entire background of the D2D render target is "invisible",
	//And only the stuff we draw with D2D will be visible (the text)

	//Set the blend state for D2D render target texture objects
	d3d11DevCon->OMSetBlendState(d2dTransparency, NULL, 0xffffffff);

	//Set the d2d Index buffer
	d3d11DevCon->IASetIndexBuffer(d2dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the d2d vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	d3d11DevCon->IASetVertexBuffers(0, 1, &d2dVertBuffer, &stride, &offset);

	XMMATRIX WVP;
	cbPerObject cbPerObj;
	WVP = XMMatrixIdentity();
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	d3d11DevCon->PSSetShaderResources(0, 1, &d2dTexture);
	d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

	d3d11DevCon->RSSetState(CWcullMode);
	d3d11DevCon->DrawIndexed(6, 0, 0);

}