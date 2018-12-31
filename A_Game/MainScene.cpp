#include "d3dUtility.h"
//#include "GameObject.h"
#include "Camera.h"
#include "debugprint.h"
#include <iostream>
#include "UpdateMethods.h"
Camera* cam;
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
void Update(float dt)
{
	UpdatePlayer(GameObjectList);
	UpdateBoss(GameObjectList);
	cam->Follow();
}

bool Display(D3DUtility* mApp)
{
	if (mApp->device)
	{
		int count = GameObjectList.size();

		ID3D11Device* dev;
		ID3D11DeviceContext* con;
		dev = mApp->device.Get();
		con = mApp->immediateContext.Get();
		static float black[4] = { 0.7f, 0.7f, 0.7f, 1.0f };	// RGBA = (0,0,0,255)
		con->ClearRenderTargetView(mApp->renderTargetView.Get(), reinterpret_cast<const float*>(&black));
		con->ClearDepthStencilView(mApp->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		for (int i = 0; i < count; i++)
		{

			GameObjectList[i]->Draw();

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
			Update(mTimer.DeltaTime());//动画计算
			//cam->SetCamPosition(0.0f,0.0f);
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
	char chplayer[20] = "player";
	char chboss[20] = "boss";
	GameObject* player = new GameObject(mainscene.mApp, MD5, L"Models/mpplayer.md5mesh", chplayer);
	GameObject* boss = new GameObject(mainscene.mApp, MD5, L"Models/cyberdemon.md5mesh", chboss);

	GameObject* cubeOri = new GameObject(mainscene.mApp);


	player->SetWorldMatrix(XMMatrixTranslation(3, 0, 0));

	GameObject* grass = new GameObject(mainscene.mApp, OBJ, L"Models/cyberdemon.md5mesh", chboss);
	grass->SetWorldMatrix(XMMatrixRotationX(XM_PI));
	
	GameObjectList.push_back(player);
	GameObjectList.push_back(boss);
	GameObjectList.push_back(cubeOri);
GameObjectList.push_back(grass);

	boss->communicateList = &GameObjectList;
	cam = new Camera(GameObjectList);
	
	
	mainscene.Running();



	return 0;
}

bool MainScene::InitApp()
{
	if (!D3DUtility::InitApp())
		return false;
}
