#pragma once
#ifndef __D3DUtilityH__
#define __D3DUtilityH__


#include <string>
#include <vector>
#include <Windows.h>
#include <wrl.h>
//��ѧ�����ͷ�ļ�
#include <DirectXMath.h>
using namespace DirectX;

//DirectX11���ͷ�ļ�
#include <d3d11.h>
#include <d3dcompiler.h>
#include "GameTimer.h"
//�ļ���
#include <fstream>
#include <istream>
#include <sstream>
#include <dwrite.h>
#include <dinput.h>
//DirectX11��ؿ�
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

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
	//��ʼ������
	HRESULT InitWindow();
	static D3DUtility* GetApp();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)=0;

	ID3D11DepthStencilView* depthStencilView;  //���ģ����ͼ
	ID3D11Texture2D* depthStencilBuffer;       //��Ȼ���


	Microsoft::WRL::ComPtr<ID3D11Device> device = nullptr;                    //D3D11�豸ָ��
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = nullptr;               //������ָ��
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext = nullptr;   //�豸������ָ��
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView = nullptr;//��ȾĿ����ͼָ��  


	//ID3D11Device* device = nullptr;                    //D3D11�豸ָ��
	//IDXGISwapChain* swapChain = nullptr;               //������ָ��
	//ID3D11DeviceContext* immediateContext = nullptr;   //�豸������ָ��
	//ID3D11RenderTargetView* renderTargetView = nullptr;//��ȾĿ����ͼָ��  

	HRESULT InitD3D(
		HINSTANCE hInstance,
		int width, int height,
		ID3D11RenderTargetView** renderTargetView,        //Ŀ����Ⱦ��ͼ�ӿ�
		ID3D11DeviceContext** immediateContext,           //�豸�����Ľӿڣ��豸�����İ����豸��ʹ�û���������
		IDXGISwapChain** swapChain,                       //�������ӿڣ���������������������
		ID3D11Device** device);                           //�豸�ýӿڣ�ÿ��D3D����������һ���豸



	
};
#endif