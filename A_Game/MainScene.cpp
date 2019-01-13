#include "d3dUtility.h"
//#include "GameObject.h"
#include "Camera.h"
#include "debugprint.h"
#include <iostream>
#include "UpdateMethods.h"

Camera* cam;

std::vector<GameObject*> GameObjectList;
GameObject* scene;
std::shared_ptr<EffectFactory>       m_fxFactory;
std::shared_ptr<CommonStates>        m_states;


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

		static float angle=0.0f;
		//angle += 0.0004f;
		auto iii = XMMatrixRotationY(angle)*XMMatrixScaling(35.22f, 35.22f, 35.22f)*XMMatrixTranslation(0, -20, 0);
		scene->SetWorldMatrix(iii);
		//drawbitmap();
		for (int i = 0; i < count; i++)
		{
			con->OMSetDepthStencilState(m_states->DepthDefault(), 0);
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
			Update(mTimer.DeltaTime());//动画计算
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
	char chsky[20] = "sky";
	
	m_fxFactory.reset(new EffectFactory(mainscene.mApp->device.Get()));
	m_states.reset(new CommonStates(mainscene.mApp->device.Get()));


	GameObject* player = new GameObject(mainscene.mApp, MD5, L"Models/mpplayer.md5mesh", chplayer, m_fxFactory, m_states);
	GameObject* boss = new GameObject(mainscene.mApp, MD5, L"Models/cyberdemon.md5mesh", chboss, m_fxFactory, m_states);
	GameObject* sky = new GameObject(mainscene.mApp, SKY, L"skymap.dds", chsky, m_fxFactory, m_states);
	scene = new GameObject(mainscene.mApp, SDKMESH, L"Teleport.sdkmesh", chsky, m_fxFactory, m_states);

	player->life = 1000000;
	boss->life = 10;


	GameObjectList.push_back(player);
	GameObjectList.push_back(boss);
	GameObjectList.push_back(sky);


	GameObjectList.push_back(scene);
	player->SetWorldMatrix(XMMatrixTranslation(5, 0, -5));
	

	

	boss->communicateList = &GameObjectList;
	cam = new Camera(GameObjectList);

	mciSendString(L"open bossD.mp3 alias bossD wait", NULL, 0, NULL);
	mciSendString(L"open player.mp3 alias player wait", NULL, 0, NULL);
	mciSendString(L"play bgm.mp3", NULL, 0, NULL);
	mainscene.Running();

	return 0;
}

bool MainScene::InitApp()
{
	if (!D3DUtility::InitApp())
		return false;
}
