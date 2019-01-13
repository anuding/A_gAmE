#pragma once
#ifndef __D3DUtilityH__
#define __D3DUtilityH__


#include <string>
#include <vector>
#include <Windows.h>
#include <wrl.h>
//数学库相关头文件
#include <DirectXMath.h>
using namespace DirectX;

//DirectX11相关头文件
#include <d3d11.h>
#include <d3dcompiler.h>
#include "GameTimer.h"
//文件流
#include <fstream>
#include <istream>
#include <sstream>
#include <dwrite.h>
#include <dinput.h>
//DirectX11相关库
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#pragma comment (lib, "D3D10_1.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "D2D1.lib")
#pragma comment (lib, "dwrite.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
//#include <d3dx11effect.h>
#include <Effects.h>
#include <Model.h>
#include <CommonStates.h>
#include <d3dcompiler.h>
#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include <D3D10_1.h>
#include <DXGI.h>
#include <D2D1.h>
#include <sstream>
#include <dwrite.h>
#include <dinput.h>
#include <vector>
#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include <Windows.h>
#include <wrl.h>
#include<wincodec.h>

class D3DUtility
{
public:
	ID3D11Buffer* cbPerObjectBuffer;

	D3DUtility(HINSTANCE hInstance);
	HINSTANCE mhAppInst = nullptr;
	HWND  mhMainWnd = nullptr;
	WNDCLASS wc = { };
	GameTimer mTimer;

//protected:
	virtual bool InitApp();
	static D3DUtility* mApp;
//private:
	int mClientWidth = 800;
	int mClientHeight = 600;
	//初始化窗口
	HRESULT InitWindow();
	static D3DUtility* GetApp();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)=0;

	ID3D11DepthStencilView* depthStencilView;  //深度模板视图
	ID3D11Texture2D* depthStencilBuffer;       //深度缓存


	Microsoft::WRL::ComPtr<ID3D11Device> device = nullptr;                    //D3D11设备指针
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = nullptr;               //交换链指针
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext = nullptr;   //设备上下文指针
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView = nullptr;//渲染目标视图指针  

	HRESULT InitD3D(
		HINSTANCE hInstance,
		int width, int height,
		ID3D11RenderTargetView** renderTargetView,        //目标渲染视图接口
		ID3D11DeviceContext** immediateContext,           //设备上下文接口，设备上下文包含设备的使用环境和设置
		IDXGISwapChain** swapChain,                       //交换链接口，用于描述交换链的特性
		ID3D11Device** device);                           //设备用接口，每个D3D程序至少有一个设备

};
#endif