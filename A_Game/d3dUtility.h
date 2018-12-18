#pragma once
#ifndef __d3dUtilityH__
#define __d3dUtilityH__

#include <Windows.h>
//////////////////////////////////////////////////////
//XNA��ѧ�����ͷ�ļ�
//////////////////////////////////////////////////////
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;
//////////////////////////////////////////////////////
//DirectX11���ͷ�ļ�
//////////////////////////////////////////////////////
#include <d3d11.h>
//#include <d3dx11.h>
#include <d3dx11effect.h>

/////////////////////////////////////////////////////
//DirectX11��ؿ�
/////////////////////////////////////////////////////
//#pragma comment(lib, "Effects11.lib")
#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "x64/d3dx11.lib")

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

namespace d3d //����һ��d3d�����ռ�
{

	//��ʼ��D3D
	bool InitD3D(
		HINSTANCE hInstance,
		int width, int height,
		ID3D11RenderTargetView** renderTargetView,        //Ŀ����Ⱦ��ͼ�ӿ�
		ID3D11DeviceContext** immediateContext,           //�豸�����Ľӿڣ��豸�����İ����豸��ʹ�û���������
		IDXGISwapChain** swapChain,                       //�������ӿڣ���������������������
		ID3D11Device** device);                           //�豸�ýӿڣ�ÿ��D3D����������һ���豸

	//��Ϣѭ��
	int EnterMsgLoop(bool(*ptr_display)(float timeDelta));

	//�ص�����
	LRESULT CALLBACK WndProc(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam);

}

#endif