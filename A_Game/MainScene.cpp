#include "d3dUtility.h"
//#include "GameObject.h"
#include "Camera.h"
#include "debugprint.h"
#include <iostream>
#include "UpdateMethods.h"
bool bossdeath = false;
Camera* cam;
GameObject* bullet = nullptr;
std::vector<GameObject*> GameObjectList;
GameObject* scene;
std::shared_ptr<EffectFactory>       m_fxFactory;
std::shared_ptr<CommonStates>        m_states;
float CamdeltaX = 3.0f;
float CamdeltaZ = -5.0f;
float k = 0.1f;
float weaponRange = 200.0f;
int lock = 1;
bool lock2 = 1;
float back = 0.5f;
float x = 0;
GameTimer mTimer1;
XMMATRIX Scale = XMMatrixScaling(0.44f, 0.44f, 0.44f);

bool boss_suo = true;
static int mytime = 0;

void InitD2DScreenTexture(ID3D11Device * d3d11Device)
{
	//Create the vertex buffer
	Vertex v[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f),
		Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f),
	};

	DWORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;
	d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &d2dIndexBuffer);


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	HRESULT hr;
	hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &d2dVertBuffer);

	//Create A shader resource view from the texture D2D will render to,
	//So we can use it to texture a square which overlays our scene
	d3d11Device->CreateShaderResourceView(sharedTex11, NULL, &d2dTexture);
}
void InitD2DScreenBitmap()
{

	CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWICFactory));
	HRESULT hr;
	//create a decoder
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/5.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		1.0f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist.push_back(pBitmap);
	/*2*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/4.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist.push_back(pBitmap);
	/*3*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/3.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist.push_back(pBitmap);
	/*2*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/2.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist.push_back(pBitmap);



	/*1*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/1.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist.push_back(pBitmap);







	/*1*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/11.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist1.push_back(pBitmap);



	/*1*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/12.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist1.push_back(pBitmap);





	/*1*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/13.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist1.push_back(pBitmap);




	/*1*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/14.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist1.push_back(pBitmap);






	/*1*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/15.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist1.push_back(pBitmap);







	/*1*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/16.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist1.push_back(pBitmap);








	/*1*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/17.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist1.push_back(pBitmap);









	/*1*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/18.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist1.push_back(pBitmap);




	/*1*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/19.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist1.push_back(pBitmap);




	/*1*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/20.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist1.push_back(pBitmap);



	/*1*/
	hr = pIWICFactory->CreateDecoderFromFilename(
		TEXT("./xuetiao/21.bmp"),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pFrame);

	// Format convert the frame to 32bppPBGRA
	hr = pIWICFactory->CreateFormatConverter(&pConverter);

	//Initialize Converter
	hr = pConverter->Initialize(
		pFrame,                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
	);

	// create Bitmap
	hr = D2DRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&pBitmap
	);
	maplist1.push_back(pBitmap);

}

void initd2deffect(ID3D11Device * d3d11Device)
{
	ID3DBlob* compilationMsgs = nullptr;
	D3DCompileFromFile(L"Effects.fx", 0, 0, "D2D_PS", "ps_5_0", 0, 0, &D2D_PS_Buffer, &compilationMsgs);
	HRESULT hr;
	hr = d3d11Device->CreatePixelShader(D2D_PS_Buffer->GetBufferPointer(), D2D_PS_Buffer->GetBufferSize(), NULL, &D2D_PS);

	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);


	//Camera information
	
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	d3d11Device->CreateBlendState(&blendDesc, &d2dTransparency);

	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	d3d11Device->CreateBlendState(&blendDesc, &Transparency);


	D3D11_TEXTURE2D_DESC SMTextureDesc;
	SMTexture->GetDesc(&SMTextureDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	SMViewDesc.Format = SMTextureDesc.Format;
	SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SMViewDesc.TextureCube.MipLevels = SMTextureDesc.MipLevels;
	SMViewDesc.TextureCube.MostDetailedMip = 0;
	Microsoft::WRL::ComPtr<ID3D11Resource>res = nullptr;
	hr = d3d11Device->CreateShaderResourceView(res.Get(), &SMViewDesc, &smrv);

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	hr = d3d11Device->CreateSamplerState(&sampDesc, &CubesTexSamplerState);

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	hr = d3d11Device->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;

	hr = d3d11Device->CreateRasterizerState(&cmdesc, &CWcullMode);

	cmdesc.CullMode = D3D11_CULL_NONE;
	//cmdesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = d3d11Device->CreateRasterizerState(&cmdesc, &RSCullNone);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	d3d11Device->CreateDepthStencilState(&dssDesc, &DSLessEqual);








}


class  MainScene : public D3DUtility
{
public:
	virtual bool InitApp() override;
	MainScene(HINSTANCE hInstance);
	int Running();
	//virtual void Update(const GameTimer& gt)override;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
public:

};
MainScene::MainScene(HINSTANCE hInstance) : D3DUtility(hInstance)
{

}

LRESULT MainScene::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

	case WM_LBUTTONDOWN:
	{
		//MessageBox(hwnd, L"你点了左键", L"点击事件", MB_OK);

		break;
	}
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);
		if (wParam == VK_SPACE)  //响应键盘空格键
		{
			GameObjectList.push_back(bullet);
			GameObject* pl = GameObjectList[0];
			XMVECTOR dir = pl->GetPos();
			XMFLOAT4 originPos;//Bullet原本的位置
			XMStoreFloat4(&originPos, dir);
			//bullet->SetWorldMatrix();
			bullet->SetWorldMatrix(XMMatrixTranslation(originPos.x, originPos.y, originPos.z));
			cam->mList = GameObjectList;
			mciSendString(L"play player from 0 ", NULL, 0, NULL);

		}
		if (wParam == 'J')  //响应键盘空格键
		{
			GameObjectList.push_back(bullet);
			GameObject* pl = GameObjectList[0];
			XMVECTOR dir = pl->GetPos();
			XMFLOAT4 originPos;//Bullet原本的位置
			XMStoreFloat4(&originPos, dir);
			//bullet->SetWorldMatrix();
			bullet->SetWorldMatrix(XMMatrixTranslation(originPos.x, originPos.y, originPos.z));
			pl->UpdateMD5Model(pl->NewMD5Model, 0.0005f, 0);
			cam->mList = GameObjectList;

		}
		break;
	}


	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}



//
////**************以下为框架函数******************
void Update(float dt)
{
	UpdatePlayer(GameObjectList);
	if (boss_suo)
		UpdateBoss(GameObjectList);
	else
		UpdateBoss1(GameObjectList);
	//UpdateWeapon(GameObjectList);
	cam->Follow();
}

bool Display(D3DUtility* mApp)
{
	if (mApp->device)
	{
		int count = GameObjectList.size();
		for (int j = 0; j < count; j++)
		{
			if (strcmp(GameObjectList[j]->tag, "weapon1") == 0)
			{

				XMFLOAT4 weaponPos;
				XMFLOAT4 bossPos;
				XMFLOAT4 playerPos;
				GameObject* we = GameObjectList[j];
				GameObject* bo = GameObjectList[1];
				GameObject* pl = GameObjectList[0];
				XMStoreFloat4(&weaponPos, we->GetPos());
				XMStoreFloat4(&bossPos, bo->GetPos());
				XMStoreFloat4(&playerPos, pl->GetPos());
				if ((weaponPos.x - playerPos.x)*(weaponPos.x - playerPos.x) + (weaponPos.z - playerPos.z)*(weaponPos.z - playerPos.z) < k) {

					XMMATRIX world;
					XMFLOAT4 dirpos;
					world = XMMatrixTranslation(bossPos.x, bossPos.y, bossPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
					pl->life = pl->life - 1;

					XMVECTOR playerpos = pl->GetPos();
					XMVECTOR bossPos = bo->GetPos();
					XMVECTOR dir = bossPos - playerpos;
					XMStoreFloat4(&dirpos, dir);
					playerPos.x = playerPos.x + playerPos.x / abs(dirpos.x)*back;
					//playerPos.z = playerPos.z + playerPos.z / abs(dirpos.z)*0.5f;
					XMMATRIX BackPosition;
					BackPosition = XMMatrixTranslation(playerPos.x, playerPos.y, playerPos.z);
					pl->SetWorldMatrix(BackPosition);
				}
				XMStoreFloat4(&weaponPos, we->GetPos());



				if ((weaponPos.x - bossPos.x)*(weaponPos.x - bossPos.x) + (weaponPos.z - bossPos.z)*(weaponPos.z - bossPos.z) < weaponRange) {
					float weX; float weY; float weZ;//怪兽的武器
					weX = weaponPos.x + 0.005f;
					XMMATRIX world1;
					world1 = XMMatrixTranslation(weX, weaponPos.y, weaponPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world1);
				}
				else {
					GameObject* bo = GameObjectList[1];
					XMFLOAT4 checkPos;
					XMStoreFloat4(&checkPos, bo->GetPos());
					XMMATRIX world;
					world = XMMatrixTranslation(checkPos.x, checkPos.y, checkPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
				}
			}
		}

		for (int j = 0; j < count; j++)
		{
			if (strcmp(GameObjectList[j]->tag, "weapon2") == 0)
			{

				XMFLOAT4 weaponPos;
				XMFLOAT4 bossPos;
				XMFLOAT4 playerPos;
				GameObject* we = GameObjectList[j];
				GameObject* bo = GameObjectList[1];
				GameObject* pl = GameObjectList[0];
				XMStoreFloat4(&weaponPos, we->GetPos());
				XMStoreFloat4(&bossPos, bo->GetPos());
				XMStoreFloat4(&playerPos, pl->GetPos());
				if ((weaponPos.x - playerPos.x)*(weaponPos.x - playerPos.x) + (weaponPos.z - playerPos.z)*(weaponPos.z - playerPos.z) < k) {

					XMMATRIX world;
					world = XMMatrixTranslation(bossPos.x, bossPos.y, bossPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
					pl->life = pl->life - 1;



					XMFLOAT4 dirpos;
					XMVECTOR playerpos = pl->GetPos();
					XMVECTOR bossPos = bo->GetPos();
					XMVECTOR dir = bossPos - playerpos;
					XMStoreFloat4(&dirpos, dir);
					playerPos.x = playerPos.x + playerPos.x / abs(dirpos.x)*back;
					//playerPos.z = playerPos.z + playerPos.z / abs(dirpos.z)*0.5f;
					XMMATRIX BackPosition;
					BackPosition = XMMatrixTranslation(playerPos.x, playerPos.y, playerPos.z);
					pl->SetWorldMatrix(BackPosition);
				}
				XMStoreFloat4(&weaponPos, we->GetPos());

				if ((weaponPos.x)*(weaponPos.x) + (weaponPos.z)*(weaponPos.z) < weaponRange) {
					float weX; float weY; float weZ;//怪兽的武器
					weX = weaponPos.x - 0.005f;
					XMMATRIX world1;
					world1 = XMMatrixTranslation(weX, weaponPos.y, weaponPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world1);
				}
				else {
					GameObject* bo = GameObjectList[1];
					XMFLOAT4 checkPos;
					XMStoreFloat4(&checkPos, bo->GetPos());
					XMMATRIX world;
					world = XMMatrixTranslation(checkPos.x, checkPos.y, checkPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
				}
			}
		}

		for (int j = 0; j < count; j++)
		{
			if (strcmp(GameObjectList[j]->tag, "weapon3") == 0)
			{
				XMFLOAT4 weaponPos;
				XMFLOAT4 bossPos;
				XMFLOAT4 playerPos;
				GameObject* we = GameObjectList[j];
				GameObject* bo = GameObjectList[1];
				GameObject* pl = GameObjectList[0];
				XMStoreFloat4(&weaponPos, we->GetPos());
				XMStoreFloat4(&bossPos, bo->GetPos());
				XMStoreFloat4(&playerPos, pl->GetPos());
				if ((weaponPos.x - playerPos.x)*(weaponPos.x - playerPos.x) + (weaponPos.z - playerPos.z)*(weaponPos.z - playerPos.z) < k) {

					XMMATRIX world;
					world = XMMatrixTranslation(bossPos.x, bossPos.y, bossPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
					pl->life = pl->life - 1;

					XMFLOAT4 dirpos;
					XMVECTOR playerpos = pl->GetPos();
					XMVECTOR bossPos = bo->GetPos();
					XMVECTOR dir = bossPos - playerpos;
					XMStoreFloat4(&dirpos, dir);
					playerPos.z = playerPos.z + playerPos.z / abs(dirpos.z)*back;
					XMMATRIX BackPosition;
					BackPosition = XMMatrixTranslation(playerPos.x, playerPos.y, playerPos.z);
					pl->SetWorldMatrix(BackPosition);
				}
				XMStoreFloat4(&weaponPos, we->GetPos());

				if ((weaponPos.x)*(weaponPos.x) + (weaponPos.z)*(weaponPos.z) < weaponRange) {
					float weX; float weY; float weZ;//怪兽的武器
					weZ = weaponPos.z + 0.005f;//  +
					XMMATRIX world1;
					world1 = XMMatrixTranslation(weaponPos.x, weaponPos.y, weZ);
					GameObjectList[j]->SetWorldMatrix(Scale*world1);
				}
				else {
					GameObject* bo = GameObjectList[1];
					XMFLOAT4 checkPos;
					XMStoreFloat4(&checkPos, bo->GetPos());
					XMMATRIX world;
					world = XMMatrixTranslation(checkPos.x, checkPos.y, checkPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
				}
			}
		}

		for (int j = 0; j < count; j++)
		{
			if (strcmp(GameObjectList[j]->tag, "weapon4") == 0)
			{
				XMFLOAT4 weaponPos;
				XMFLOAT4 bossPos;
				XMFLOAT4 playerPos;
				GameObject* we = GameObjectList[j];
				GameObject* bo = GameObjectList[1];
				GameObject* pl = GameObjectList[0];
				XMStoreFloat4(&weaponPos, we->GetPos());
				XMStoreFloat4(&bossPos, bo->GetPos());
				XMStoreFloat4(&playerPos, pl->GetPos());
				if ((weaponPos.x - playerPos.x)*(weaponPos.x - playerPos.x) + (weaponPos.z - playerPos.z)*(weaponPos.z - playerPos.z) < k) {

					XMMATRIX world;
					world = XMMatrixTranslation(bossPos.x, bossPos.y, bossPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
					pl->life = pl->life - 1;

					XMFLOAT4 dirpos;
					XMVECTOR playerpos = pl->GetPos();
					XMVECTOR bossPos = bo->GetPos();
					XMVECTOR dir = bossPos - playerpos;
					XMStoreFloat4(&dirpos, dir);
					playerPos.z = playerPos.z + playerPos.z / abs(dirpos.z)*back;
					XMMATRIX BackPosition;
					BackPosition = XMMatrixTranslation(playerPos.x, playerPos.y, playerPos.z);
					pl->SetWorldMatrix(BackPosition);
				}
				XMStoreFloat4(&weaponPos, we->GetPos());

				if ((weaponPos.x)*(weaponPos.x) + (weaponPos.z)*(weaponPos.z) < weaponRange) {
					float weX; float weY; float weZ;//怪兽的武器
					weZ = weaponPos.z - 0.005f;
					XMMATRIX world1;
					world1 = XMMatrixTranslation(weaponPos.x, weaponPos.y, weZ);
					GameObjectList[j]->SetWorldMatrix(Scale*world1);
				}
				else {
					GameObject* bo = GameObjectList[1];
					XMFLOAT4 checkPos;
					XMStoreFloat4(&checkPos, bo->GetPos());
					XMMATRIX world;
					world = XMMatrixTranslation(checkPos.x, checkPos.y, checkPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
				}
			}
		}

		for (int j = 0; j < count; j++)
		{
			if (strcmp(GameObjectList[j]->tag, "weapon5") == 0)
			{


				XMFLOAT4 weaponPos;
				XMFLOAT4 bossPos;
				XMFLOAT4 playerPos;
				GameObject* we = GameObjectList[j];
				GameObject* bo = GameObjectList[1];
				GameObject* pl = GameObjectList[0];
				XMStoreFloat4(&weaponPos, we->GetPos());
				XMStoreFloat4(&bossPos, bo->GetPos());
				XMStoreFloat4(&playerPos, pl->GetPos());
				if ((weaponPos.x - playerPos.x)*(weaponPos.x - playerPos.x) + (weaponPos.z - playerPos.z)*(weaponPos.z - playerPos.z) < k) {

					XMMATRIX world;
					world = XMMatrixTranslation(bossPos.x, bossPos.y, bossPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
					pl->life = pl->life - 1;

					XMFLOAT4 dirpos;
					XMVECTOR playerpos = pl->GetPos();
					XMVECTOR bossPos = bo->GetPos();
					XMVECTOR dir = bossPos - playerpos;
					XMStoreFloat4(&dirpos, dir);
					playerPos.x = playerPos.x + playerPos.x / abs(dirpos.x)*back;
					playerPos.z = playerPos.z + playerPos.z / abs(dirpos.z)*back;
					XMMATRIX BackPosition;
					BackPosition = XMMatrixTranslation(playerPos.x, playerPos.y, playerPos.z);
					pl->SetWorldMatrix(BackPosition);

				}
				XMStoreFloat4(&weaponPos, we->GetPos());

				if ((weaponPos.x)*(weaponPos.x) + (weaponPos.z)*(weaponPos.z) < weaponRange) {
					float weX; float weY; float weZ;//怪兽的武器
					weX = weaponPos.x + 0.00353553390f;
					weZ = weaponPos.z + 0.00353553390f;
					XMMATRIX world1;
					world1 = XMMatrixTranslation(weX, weaponPos.y, weZ);
					GameObjectList[j]->SetWorldMatrix(Scale*world1);
				}
				else {
					GameObject* bo = GameObjectList[1];
					XMFLOAT4 checkPos;
					XMStoreFloat4(&checkPos, bo->GetPos());
					XMMATRIX world;
					world = XMMatrixTranslation(checkPos.x, checkPos.y, checkPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
				}
			}
		}

		for (int j = 0; j < count; j++)
		{
			if (strcmp(GameObjectList[j]->tag, "weapon8") == 0)
			{

				XMFLOAT4 weaponPos;
				XMFLOAT4 bossPos;
				XMFLOAT4 playerPos;
				GameObject* we = GameObjectList[j];
				GameObject* bo = GameObjectList[1];
				GameObject* pl = GameObjectList[0];
				XMStoreFloat4(&weaponPos, we->GetPos());
				XMStoreFloat4(&bossPos, bo->GetPos());
				XMStoreFloat4(&playerPos, pl->GetPos());
				if ((weaponPos.x - playerPos.x)*(weaponPos.x - playerPos.x) + (weaponPos.z - playerPos.z)*(weaponPos.z - playerPos.z) < k) {

					XMMATRIX world;
					world = XMMatrixTranslation(bossPos.x, bossPos.y, bossPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
					pl->life = pl->life - 1;

					XMFLOAT4 dirpos;
					XMVECTOR playerpos = pl->GetPos();
					XMVECTOR bossPos = bo->GetPos();
					XMVECTOR dir = bossPos - playerpos;
					XMStoreFloat4(&dirpos, dir);
					playerPos.x = playerPos.x + playerPos.x / abs(dirpos.x)*back;
					playerPos.z = playerPos.z + playerPos.z / abs(dirpos.z)*back;
					XMMATRIX BackPosition;
					BackPosition = XMMatrixTranslation(playerPos.x, playerPos.y, playerPos.z);
					pl->SetWorldMatrix(BackPosition);
				}
				XMStoreFloat4(&weaponPos, we->GetPos());

				if ((weaponPos.x)*(weaponPos.x) + (weaponPos.z)*(weaponPos.z) < weaponRange) {
					float weX; float weY; float weZ;//怪兽的武器
					weX = weaponPos.x - 0.0035f;
					weZ = weaponPos.z + 0.0035f;
					XMMATRIX world1;
					world1 = XMMatrixTranslation(weX, weaponPos.y, weZ);
					GameObjectList[j]->SetWorldMatrix(Scale*world1);
				}
				else {
					GameObject* bo = GameObjectList[1];
					XMFLOAT4 checkPos;
					XMStoreFloat4(&checkPos, bo->GetPos());
					XMMATRIX world;
					world = XMMatrixTranslation(checkPos.x, checkPos.y, checkPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
				}
			}
		}
		for (int j = 0; j < count; j++)
		{
			if (strcmp(GameObjectList[j]->tag, "weapon6") == 0)
			{

				XMFLOAT4 weaponPos;
				XMFLOAT4 bossPos;
				XMFLOAT4 playerPos;
				GameObject* we = GameObjectList[j];
				GameObject* bo = GameObjectList[1];
				GameObject* pl = GameObjectList[0];
				XMStoreFloat4(&weaponPos, we->GetPos());
				XMStoreFloat4(&bossPos, bo->GetPos());
				XMStoreFloat4(&playerPos, pl->GetPos());
				if ((weaponPos.x - playerPos.x)*(weaponPos.x - playerPos.x) + (weaponPos.z - playerPos.z)*(weaponPos.z - playerPos.z) < k) {

					XMMATRIX world;
					world = XMMatrixTranslation(bossPos.x, bossPos.y, bossPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
					pl->life = pl->life - 1;

					XMFLOAT4 dirpos;
					XMVECTOR playerpos = pl->GetPos();
					XMVECTOR bossPos = bo->GetPos();
					XMVECTOR dir = bossPos - playerpos;
					XMStoreFloat4(&dirpos, dir);
					playerPos.x = playerPos.x + playerPos.x / abs(dirpos.x)*back;
					playerPos.z = playerPos.z + playerPos.z / abs(dirpos.z)*back;
					XMMATRIX BackPosition;
					BackPosition = XMMatrixTranslation(playerPos.x, playerPos.y, playerPos.z);
					pl->SetWorldMatrix(BackPosition);
				}
				XMStoreFloat4(&weaponPos, we->GetPos());

				if ((weaponPos.x)*(weaponPos.x) + (weaponPos.z)*(weaponPos.z) < weaponRange) {
					float weX; float weY; float weZ;//怪兽的武器
					weX = weaponPos.x - 0.0035f;
					weZ = weaponPos.z - 0.0035f;
					XMMATRIX world1;
					world1 = XMMatrixTranslation(weX, weaponPos.y, weZ);
					GameObjectList[j]->SetWorldMatrix(Scale*world1);
				}
				else {
					GameObject* bo = GameObjectList[1];
					XMFLOAT4 checkPos;
					XMStoreFloat4(&checkPos, bo->GetPos());
					XMMATRIX world;
					world = XMMatrixTranslation(checkPos.x, checkPos.y, checkPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
				}
			}
		}
		for (int j = 0; j < count; j++)
		{
			if (strcmp(GameObjectList[j]->tag, "weapon7") == 0)
			{
				XMFLOAT4 weaponPos;
				XMFLOAT4 bossPos;
				XMFLOAT4 playerPos;
				GameObject* we = GameObjectList[j];
				GameObject* bo = GameObjectList[1];
				GameObject* pl = GameObjectList[0];
				XMStoreFloat4(&weaponPos, we->GetPos());
				XMStoreFloat4(&bossPos, bo->GetPos());
				XMStoreFloat4(&playerPos, pl->GetPos());
				if ((weaponPos.x - playerPos.x)*(weaponPos.x - playerPos.x) + (weaponPos.z - playerPos.z)*(weaponPos.z - playerPos.z) < k) {

					XMMATRIX world;
					world = XMMatrixTranslation(bossPos.x, bossPos.y, bossPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
					pl->life = pl->life - 1;

					XMFLOAT4 dirpos;
					XMVECTOR playerpos = pl->GetPos();
					XMVECTOR bossPos = bo->GetPos();
					XMVECTOR dir = bossPos - playerpos;
					XMStoreFloat4(&dirpos, dir);
					playerPos.x = playerPos.x + playerPos.x / abs(dirpos.x)*back;
					playerPos.z = playerPos.z + playerPos.z / abs(dirpos.z)*back;
					XMMATRIX BackPosition;
					BackPosition = XMMatrixTranslation(playerPos.x, playerPos.y, playerPos.z);
					pl->SetWorldMatrix(BackPosition);
				}
				XMStoreFloat4(&weaponPos, we->GetPos());

				if ((weaponPos.x)*(weaponPos.x) + (weaponPos.z)*(weaponPos.z) < weaponRange) {
					float weX; float weY; float weZ;//怪兽的武器
					weX = weaponPos.x + 0.0035f;
					weZ = weaponPos.z - 0.0035f;
					XMMATRIX world1;
					world1 = XMMatrixTranslation(weX, weaponPos.y, weZ);
					GameObjectList[j]->SetWorldMatrix(Scale*world1);
				}
				else {
					GameObject* bo = GameObjectList[1];
					XMFLOAT4 checkPos;
					XMStoreFloat4(&checkPos, bo->GetPos());
					XMMATRIX world;
					world = XMMatrixTranslation(checkPos.x, checkPos.y, checkPos.z);
					GameObjectList[j]->SetWorldMatrix(Scale*world);
				}


			}
		}



		for (int i = 0; i < count; i++)
		{
			if (strcmp(GameObjectList[i]->tag, "bullet") == 0)
			{

				GameObject* bo = GameObjectList[1];
				GameObject* pl = GameObjectList[0];
				GameObject* bu = GameObjectList[i];
				XMVECTOR dir = bo->GetPos() - pl->GetPos();
				XMFLOAT4 originPos;//Bullet原本的位置
				XMFLOAT4 checkPos;
				XMFLOAT4 plPos;
				XMStoreFloat4(&originPos, bu->GetPos());
				XMStoreFloat4(&checkPos, bo->GetPos());
				XMStoreFloat4(&plPos, pl->GetPos());
				//&& (originPos.x != checkPos.x) && (originPos.y != checkPos.y)
				if (abs(originPos.x - checkPos.x) > 0.1f || abs(originPos.z - checkPos.z) > 0.1f)
				{
					XMVECTOR front = XMVector3Normalize(dir);
					XMFLOAT4 deltainDir;
					XMStoreFloat4(&deltainDir, front);
					float destX; float destY; float destZ;//Boss应该移动到的位置
					destX = originPos.x + deltainDir.x*0.006f;
					destY = originPos.y + deltainDir.y*0.006f;
					destZ = originPos.z + deltainDir.z*0.006f;
					XMMATRIX world;

					world = XMMatrixTranslation(destX, destY, destZ);
					GameObjectList[i]->SetWorldMatrix(XMMatrixScaling(0.2f, 0.2f, 0.2f)*world);
					
				}
				else {
					GameObjectList.pop_back();
					bo->life = bo->life - 1;

				}
			}
		}

		


		for (int i = 0; i < count; i++)
		{
			if (strcmp(GameObjectList[i]->tag, "player") == 0)
			{

				GameObject* pl = GameObjectList[i];
				if (pl->life < 0) {

					XMMATRIX world1;
					XMFLOAT4 checkPos;
					XMStoreFloat4(&checkPos, pl->GetPos());

					x = x - 0.0010f;
					world1 = XMMatrixTranslation(checkPos.x, x, checkPos.z);
					GameObjectList[i]->SetWorldMatrix(world1);
					lock = 2;
					cam->SetCamPosition();
					std::vector<GameObject* >::iterator itor2;
					for (std::vector<GameObject* >::iterator iter = GameObjectList.begin(); iter != GameObjectList.end(); )
					{
						GameObject* k = *iter;
						if (strcmp(k->tag, "weapon1") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}
						if (strcmp(k->tag, "weapon2") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}
						if (strcmp(k->tag, "weapon3") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}
						if (strcmp(k->tag, "weapon4") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}
						if (strcmp(k->tag, "weapon5") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}

						if (strcmp(k->tag, "weapon6") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}
						if (strcmp(k->tag, "weapon7") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}
						if (strcmp(k->tag, "weapon8") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}
						else
							iter++;
					}
				}
			}
		}

		for (int i = 0; i < count; i++)
		{
			if (strcmp(GameObjectList[i]->tag, "boss") == 0)
			{

				GameObject* bo = GameObjectList[i];
				GameObject* pl = GameObjectList[0];
				XMFLOAT4 plPos;

				XMStoreFloat4(&plPos, pl->GetPos());
				if (bo->life < 0) {
					if (!bossdeath)
					{
						bossdeath = true;
						mciSendString(L"play bossD from 0 to 3000", NULL, 0, NULL);
					}
					
					XMMATRIX world1;
					XMFLOAT4 checkPos;
					XMStoreFloat4(&checkPos, bo->GetPos());

					x = x + 0.0010f;
					world1 = XMMatrixTranslation(checkPos.x, x, checkPos.z);
					GameObjectList[i]->SetWorldMatrix(world1);
					lock = 2;
					cam->SetCamPosition();
					//bo->PopWeapon(GameObjectList);
					std::vector<GameObject* >::iterator itor2;
					for (std::vector<GameObject* >::iterator iter = GameObjectList.begin(); iter != GameObjectList.end(); )
					{
						GameObject* k = *iter;
						if (strcmp(k->tag, "weapon1") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}
						if (strcmp(k->tag, "weapon2") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}
						if (strcmp(k->tag, "weapon3") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}
						if (strcmp(k->tag, "weapon4") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}
						if (strcmp(k->tag, "weapon5") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}

						if (strcmp(k->tag, "weapon6") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}
						if (strcmp(k->tag, "weapon7") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}
						if (strcmp(k->tag, "weapon8") == 0)

						{

							itor2 = iter;
							GameObjectList.erase(itor2);
						}
						else
							iter++;
					}

				}
			}
		}
		ID3D11Device* dev;
		ID3D11DeviceContext* con;
		dev = mApp->device.Get();
		con = mApp->immediateContext.Get();
		static float black[4] = { 0.7f, 0.7f, 0.7f, 1.0f };	// RGBA = (0,0,0,255)
		con->ClearRenderTargetView(mApp->renderTargetView.Get(), reinterpret_cast<const float*>(&black));
		
		con->ClearDepthStencilView(mApp->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		//static float dis = 0.0f;
		//dis += 0.001f;
		//GameObjectList[2]->SetWorldMatrix(XMMatrixTranslation(0, 0, dis));
		static float angle=0.0f;
		//angle += 0.0004f;
		auto iii = XMMatrixRotationY(angle)*XMMatrixScaling(35.22f, 35.22f, 35.22f)*XMMatrixTranslation(0, -20, 0);
		scene->SetWorldMatrix(iii);
		//drawbitmap();
		for (int i = 0; i < count; i++)
		{
			con->OMSetDepthStencilState(m_states->DepthDefault(), 0);
			GameObjectList[i]->Draw();
			//con->OMSetDepthStencilState(m_states->DepthDefault(), 0);
			
		
		}
		
		//drawbitmap();
		//con->OMSetDepthStencilState(m_states->DepthDefault(), 0);
		
		mApp->swapChain->Present(0, 0);
	}
	return true;
}
//**************框架函数******************

int MainScene::Running()
{
	// Run the message loop.
	MSG msg = { 0 };
	mTimer.Reset();
	while (msg.message != WM_QUIT)
	{
		//分发消息,消息可以改变动画渲染中的某些参数,比如人物运动,
		//相机视角等等...,消息的处理将放在下面消息处理(Procedure)来说
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{
			mTimer.Tick();
			//在这里进行动画计算和渲染
			//mTimer1.Reset();
			//int t1 = mTimer1.TotalTime();
			mytime++;
			if (mytime % 4999 == 0)
			{
				boss_suo = false;
			}
			else if(mytime%9997==0)
			{
				boss_suo = true;
				mytime = 0;
			}
			Update(mTimer.DeltaTime());//动画计算
			//cam->SetCamPosition(0.0f,0.0f);
			Display(mApp);//渲染
			//RenderText((mApp->immediateContext.Get()), L"FPS", 0);
			
		}
	}

	return 0;
}


//
// 主函数WinMain
//
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{

	MainScene mainscene(hInstance);
	mainscene.InitApp();
	//CreateD2DResource(mainscene.mhMainWnd);
	
	/*---*/
	//InitD2D_D3D101_DWrite(Adapter, mainscene.mApp);

	//
	//InitD2DScreenBitmap();

	char chplayer[20] = "player";
	char chboss[20] = "boss";
	char chsky[20] = "sky";
	mTimer1.Start();
	mTimer1.Reset();
	m_fxFactory.reset(new EffectFactory(mainscene.mApp->device.Get()));
	m_states.reset(new CommonStates(mainscene.mApp->device.Get()));


	GameObject* player = new GameObject(mainscene.mApp, MD5, L"Models/mpplayer.md5mesh", chplayer, m_fxFactory, m_states);
	GameObject* boss = new GameObject(mainscene.mApp, MD5, L"Models/cyberdemon.md5mesh", chboss, m_fxFactory, m_states);
	GameObject* sky = new GameObject(mainscene.mApp, SKY, L"skymap.dds", chsky, m_fxFactory, m_states);
	scene = new GameObject(mainscene.mApp, SDKMESH, L"Teleport.sdkmesh", chsky, m_fxFactory, m_states);

	//GameObject* cubeOri = new GameObject(mainscene.mApp);
	//GameObject* tele = new GameObject(mainscene.mApp,SDKMESH,L"fireball.sdkmesh", chboss, m_fxFactory, m_states);

	player->life = 1000000;
	boss->life = 10;
	bullet = new GameObject(mainscene.mApp);


	//bullet = new GameObject(mainscene.mApp);
	strcpy_s(bullet->tag, "bullet");

	GameObject* weapon1 = new GameObject(mainscene.mApp, SDKMESH, L"fireball.sdkmesh", chboss, m_fxFactory, m_states);
	//pwcsName = L"box.dds";
	//weapon1->buildTexture(pwcsName);
	strcpy_s(weapon1->tag, "weapon1");

	GameObject* weapon2 = new GameObject(mainscene.mApp, SDKMESH, L"fireball.sdkmesh", chboss, m_fxFactory, m_states);
	//pwcsName = L"box.dds";
	//weapon2->buildTexture(pwcsName);
	strcpy_s(weapon2->tag, "weapon2");

	GameObject* weapon3 = new GameObject(mainscene.mApp, SDKMESH, L"fireball.sdkmesh", chboss, m_fxFactory, m_states);
	//pwcsName = L"box.dds";
	//weapon3->buildTexture(pwcsName);
	strcpy_s(weapon3->tag, "weapon3");

	GameObject* weapon4 = new GameObject(mainscene.mApp, SDKMESH, L"fireball.sdkmesh", chboss, m_fxFactory, m_states);
	//pwcsName = L"box.dds";
	//weapon4->buildTexture(pwcsName);
	strcpy_s(weapon4->tag, "weapon4");

	GameObject* weapon5 = new GameObject(mainscene.mApp, SDKMESH, L"fireball.sdkmesh", chboss, m_fxFactory, m_states);
	//pwcsName = L"box.dds";
	//weapon5->buildTexture(pwcsName);
	strcpy_s(weapon5->tag, "weapon5");

	GameObject* weapon6 = new GameObject(mainscene.mApp, SDKMESH, L"fireball.sdkmesh", chboss, m_fxFactory, m_states);
	//pwcsName = L"box.dds";
	//weapon6->buildTexture(pwcsName);
	strcpy_s(weapon6->tag, "weapon6");

	GameObject* weapon7 = new GameObject(mainscene.mApp, SDKMESH, L"fireball.sdkmesh", chboss, m_fxFactory, m_states);
	//pwcsName = L"box.dds";
	//weapon7->buildTexture(pwcsName);
	strcpy_s(weapon7->tag, "weapon7");

	GameObject* weapon8 = new GameObject(mainscene.mApp, SDKMESH, L"fireball.sdkmesh", chboss, m_fxFactory, m_states);
	//pwcsName = L"box.dds";
	//weapon8->buildTexture(pwcsName);

	strcpy_s(weapon8->tag, "weapon8");
	//InitD2DScreenBitmap();
	GameObjectList.push_back(player);
	GameObjectList.push_back(boss);
	GameObjectList.push_back(sky);


	GameObjectList.push_back(weapon1);
	GameObjectList.push_back(weapon2);
	GameObjectList.push_back(weapon3);
	GameObjectList.push_back(weapon4);
	GameObjectList.push_back(weapon5);
	GameObjectList.push_back(weapon6);
	GameObjectList.push_back(weapon7);
	GameObjectList.push_back(weapon8);
	GameObjectList.push_back(scene);
	player->SetWorldMatrix(XMMatrixTranslation(5, 0, -5));
	

	//GameObjectList.push_back(cubeOri);

	mciSendString(L"open bossD.mp3 alias bossD wait", NULL, 0, NULL);

	mciSendString(L"open player.mp3 alias player wait", NULL, 0, NULL);
	mciSendString(L"play bgm.mp3", NULL, 0, NULL);

	boss->communicateList = &GameObjectList;
	cam = new Camera(GameObjectList);

	mTimer1.Start();
	mainscene.Running();

	return 0;
}

bool MainScene::InitApp()
{
	if (!D3DUtility::InitApp())
		return false;
}
