#include "d3dUtility.h"
//#include "GameObject.h"
#include "Camera.h"
#include "debugprint.h"
#include <iostream>
//Camera* cam;
std::vector<GameObject*> GameObjectList;


float CamdeltaX = 3.0f;
float CamdeltaZ = -5.0f;
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
		MessageBox(hwnd, L"你点了左键", L"点击事件", MB_OK);
		
		break;
	}
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);
		
		break;
	}
		
		
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}


//
////**************以下为框架函数******************


bool Display(D3DUtility* mApp)
{
	if (mApp->device)
	{
		int count = GameObjectList.size();

		ID3D11Device* dev;
		ID3D11DeviceContext* con;
		dev = mApp->device.Get();
		con = mApp->immediateContext.Get();
		static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA = (0,0,0,255)
		con->ClearRenderTargetView(mApp->renderTargetView.Get(), reinterpret_cast<const float*>(&black));
		con->ClearDepthStencilView(mApp->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		
		/*static float phi = 0.0f, theta = 0.0f;
		phi += 0.00003f, theta += 0.00005f;
		GameObjectList[1]->mCBuffer.world = XMMatrixTranspose(XMMatrixTranslation(phi, 0.0f, 0.0f)*XMMatrixRotationX(0.4) * XMMatrixRotationY(0.4f));
		*/
		for (int i = 0; i < count; i++)
		{

			//GameObjectList[i]->DrawMyself();
			GameObjectList[i]->UpdateMd5();
			GameObjectList[i]->DrawMd5();
			
		}
		mApp->swapChain->Present(0, 0);
	}
	return true;
}
//**************框架函数******************

int MainScene::Running()
{
	// Run the message loop.
	MSG msg = { 0 };

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
			//在这里进行动画计算和渲染
			//Update();//动画计算
			Display(mApp);//渲染
			
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

	GameObject* player = new GameObject(mainscene.mApp);
	//player->mCBuffer.world= XMMatrixTranspose(XMMatrixRotationX(0.2f) * XMMatrixRotationY(0.2f));

	//GameObject* boss = new GameObject(mainscene.mApp);
	//boss->mCBuffer.world = XMMatrixTranspose(XMMatrixTranslation(3.0f, 0.0f, 0.0f)*XMMatrixRotationX(0.4) * XMMatrixRotationY(0.4f));


	GameObjectList.push_back(player);
	//GameObjectList.push_back(boss);
	mainscene.Running();



	return 0;
}

bool MainScene::InitApp()
{
	if (!D3DUtility::InitApp())
		return false;
}
