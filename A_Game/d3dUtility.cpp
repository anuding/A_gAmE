//���������Լ�������"D3DUtility.h"ͷ�ļ�
#include "D3DUtility.h"

D3DUtility* D3DUtility::mApp = nullptr;
LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return D3DUtility::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}



D3DUtility::D3DUtility(HINSTANCE hInstance)
{
	mApp = this;
}
D3DUtility* D3DUtility::GetApp()
{
	return mApp;
}


bool D3DUtility::InitApp()
{
	InitWindow();
	InitD3D(
		mhAppInst,
		mClientWidth,
		mClientHeight, 
		renderTargetView.GetAddressOf(),
		immediateContext.GetAddressOf(),
		swapChain.GetAddressOf(),
		device.GetAddressOf());
	return true;
}

HRESULT D3DUtility::InitWindow()
{
	wc.lpfnWndProc = MainWndProc;
	wc.hInstance = mhAppInst;
	wc.lpszClassName = L"Sample Window Class";
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	RegisterClass(&wc);
	mhMainWnd = CreateWindowEx(
		0,                              // Optional window styles.
		L"Sample Window Class",                     // Window class
		L"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, mClientWidth, mClientHeight,

		NULL,       // Parent window    
		NULL,       // Menu
		mhAppInst,  // Instance handle
		NULL        // Additional application data
	);

	if (mhMainWnd == NULL)
	{
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	return S_OK;
}


HRESULT D3DUtility::InitD3D(HINSTANCE hInstance, int width, int height, ID3D11RenderTargetView ** renderTargetView, ID3D11DeviceContext ** immediateContext, IDXGISwapChain ** swapChain, ID3D11Device ** device)
{
	//***********�ڶ����֣���ʼ��D3D��ʼ***************
	//��ʼ��D3D�豸��ҪΪ���²���
	//1. �����������������DXGI_SWAP_CHAIN_DESC�ṹ
	//2. ʹ��D3D11CreateDeviceAndSwapChain����D3D�豸��ID3D11Device��
	//   �豸�����Ľӿڣ�ID3D11DeviceContext�����������ӿڣ�IDXGISwapChain��
	//3. ����Ŀ����Ⱦ��ͼ��ID3D11RenderTargetView��
	//4. �����ӿڣ�View Port��   


	//��һ���������������������DXGI_SWAP_CHAIN_DESC�ṹ
	DXGI_SWAP_CHAIN_DESC sd;                           //��������һ��DXGI_SWAP_CHAIN_DESC�Ķ���sd
	ZeroMemory(&sd, sizeof(sd));                   //��ZeroMemory��sd���г�ʼ����ZeroMemory���÷���ʵ��һ�Ĳ���֪ʶ
	sd.BufferCount = 1;                                //�������к�̨����������ͨ��Ϊ1
	sd.BufferDesc.Width = width;                       //�������еĴ��ڿ�
	sd.BufferDesc.Height = height;                     //�������еĴ��ڸ�
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //ָ��32λ���ظ�ʽ����ʾ������Alpha��8λ��������ʽ����P50
	sd.BufferDesc.RefreshRate.Numerator = 60;          //ˢ��Ƶ�ʵķ���Ϊ60
	sd.BufferDesc.RefreshRate.Denominator = 1;         //ˢ��Ƶ�ʵķ�ĸΪ1������ˢ��Ƶ��Ϊÿ��6��
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  //����������̨������÷�����CPU�Ժ�̨����ķ��� 
	sd.OutputWindow = mhMainWnd;                            //ָ����ȾĿ�괰�ڵľ��
	sd.SampleDesc.Count = 1;                           //���ز��������ԣ������в����ö��ز�������
	sd.SampleDesc.Quality = 0;                         //����Count=1��Quality=0����ϸ����P54
	sd.Windowed = TRUE;                                //TRUEΪ����ģʽ��FALSEΪȫ��ģʽ

	//�ڶ����������豸���������Լ�����ִ��������
	//����һ������ȷ�����Դ���Featurelevel��˳��
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0, //D3D11 ��֧�ֵ�����������shader model 5
		D3D_FEATURE_LEVEL_10_1, //D3D10 ��֧�ֵ�����������shader model 4.
		D3D_FEATURE_LEVEL_10_0,
	};

	//��ȡD3D_FEATURE_LEVEL�����Ԫ�ظ���
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	//����D3D11CreateDeviceAndSwapChain�������������豸�����豸������
	//�ֱ����swapChain��device��immediateContext
	if (FAILED(D3D11CreateDeviceAndSwapChain(
		NULL,                       //ȷ����ʾ��������NULL��ʾĬ����ʾ������
		D3D_DRIVER_TYPE_HARDWARE,   //ѡ���������ͣ������ʾʹ����άӲ������
		NULL,                       //ֻ����һ����������D3D_DRIVER_TYPE_SOFTWAREʱ����ʹ���������
		0,                          //Ҳ��������ΪD3D11_CREATE_DEVICE_DEBUG��������ģʽ
		featureLevels,              //ǰ�涨���D3D_FEATURE_LEVEL����
		numFeatureLevels,           //D3D_FEATURE_LEVEL��Ԫ�ظ���
		D3D11_SDK_VERSION,          //SDK�İ汾������ΪD3D11
		&sd,                        //ǰ�涨���DXGI_SWAP_CHAIN_DESC����
		swapChain,                  //���ش����õĽ�����ָ�룬InitD3D�������ݵ�ʵ��
		device,                     //���ش����õ��豸��ָ�룬InitD3D�������ݵ�ʵ��
		NULL,                       //���ص�ǰ�豸֧�ֵ�featureLevels�����еĵ�һ������һ������ΪNULL
		immediateContext)))      //���ش����õ��豸������ָ�룬InitD3D�������ݵ�ʵ��
	{
		::MessageBox(0, L"CreateDevice - FAILED", 0, 0);  //�������ʧ�ܣ�������Ϣ��
		return false;
	}

	//��������������������ȾĿ����ͼ
	HRESULT hr = 0;         //COMҪ�����еķ������᷵��һ��HRESULT���͵Ĵ����
	ID3D11Texture2D* pBackBuffer = NULL;      //ID3D11Texture2D���͵ģ���̨����ָ��
	//����GetBuffer()�����õ���̨������󣬲�����&pBackBuffer��
	hr = (*swapChain)->GetBuffer(0,                        //����������һ������Ϊ0
		__uuidof(ID3D11Texture2D), //��������
		(LPVOID*)&pBackBuffer);   //����ָ��
//�ж�GetBuffer�Ƿ���óɹ�
	if (FAILED(hr))
	{
		::MessageBox(0, L"GetBuffer - FAILED", 0, 0); //�������ʧ�ܣ�������Ϣ��
		return false;
	}

	//����CreateRenderTargetView��������ȾĿ����ͼ�����������renderTargetView��
	hr = (*device)->CreateRenderTargetView(pBackBuffer,            //���洴���õĺ�̨����
		NULL,                   //����ΪNULL�õ�Ĭ�ϵ���ȾĿ����ͼ
		renderTargetView);     //���ش����õ���ȾĿ����ͼ��InitD3D�������ݵ�ʵ��
	pBackBuffer->Release();   //�ͷź�̨����
	//�ж�CreateRenderTargetView�Ƿ���óɹ�
	if (FAILED(hr))
	{
		::MessageBox(0, L"CreateRender - FAILED", 0, 0);  //�������ʧ�ܣ�������Ϣ��
		return false;
	}

	//����ȾĿ����ͼ�󶨵���Ⱦ����  
	(*immediateContext)->OMSetRenderTargets(1,                   //�󶨵�Ŀ����ͼ�ĸ���
		renderTargetView,    //��ȾĿ����ͼ��InitD3D�������ݵ�ʵ�� 
		NULL);              //����ΪNULL��ʾ�������ģ��

//���Ĳ��������ӿڴ�С��D3D11Ĭ�ϲ��������ӿڣ��˲�������ֶ�����  
	D3D11_VIEWPORT vp;    //����һ���ӿڵĶ���
	vp.Width = width;     //�ӿڵĿ�
	vp.Height = height;   //�ӿڵĸ�
	vp.MinDepth = 0.0f;   //���ֵ�����ޣ�**�������ֵ��[0, 1]��������ֵ��0
	vp.MaxDepth = 1.0f;   //���ֵ�����ޣ�����ֵ��1
	vp.TopLeftX = 0;      //�ӿ����Ͻǵĺ�����
	vp.TopLeftY = 0;      //�ӿ����Ͻǵ�������

	//�����ӿ�
	(*immediateContext)->RSSetViewports(1,     //�ӿڵĸ���
		&vp); //���洴�����ӿڶ���

	return true;
	//***********�ڶ����֣���ʼ��D3D����***************
	return E_NOTIMPL;
}



