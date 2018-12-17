#pragma once
#ifndef __d3dUtilityH__
#define __d3dUtilityH__

#include <Windows.h>
//////////////////////////////////////////////////////
//XNA数学库相关头文件
//////////////////////////////////////////////////////
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;
//////////////////////////////////////////////////////
//DirectX11相关头文件
//////////////////////////////////////////////////////
#include <d3d11.h>
//#include <d3dx11.h>
#include <d3dx11effect.h>

/////////////////////////////////////////////////////
//DirectX11相关库
/////////////////////////////////////////////////////
//#pragma comment(lib, "Effects11.lib")
#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "x64/d3dx11.lib")

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

namespace d3d //定义一个d3d命名空间
{

	//初始化D3D
	bool InitD3D(
		HINSTANCE hInstance,
		int width, int height,
		ID3D11RenderTargetView** renderTargetView,        //目标渲染视图接口
		ID3D11DeviceContext** immediateContext,           //设备上下文接口，设备上下文包含设备的使用环境和设置
		IDXGISwapChain** swapChain,                       //交换链接口，用于描述交换链的特性
		ID3D11Device** device);                           //设备用接口，每个D3D程序至少有一个设备

	//消息循环
	int EnterMsgLoop(bool(*ptr_display)(float timeDelta));

	//回调函数
	LRESULT CALLBACK WndProc(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam);

}

#endif