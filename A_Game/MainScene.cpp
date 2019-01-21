#include "d3dUtility.h"
//#include "GameObject.h"
#include "Camera.h"
#include "debugprint.h"
#include <iostream>
#include "UpdateMethods.h"

Camera* cam;

std::vector<GameObject*> GameObjectList;
GameObject* scene;
//std::shared_ptr<EffectFactory>       m_fxFactory;
//std::shared_ptr<CommonStates>        m_states;


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

	//UpdatePlayer(GameObjectList);
	//UpdateBoss(GameObjectList);
	cam->SetCamPosition(3.0f,2.0f,-8.0f);
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

		static float angle = 0.0f;
		angle += 0.0009f;
		/*auto rota = XMMatrixRotationY(angle)*XMMatrixTranslation(3.0f, 0.0f, 0.0f);*/
		auto rota0 = XMMatrixRotationY(angle);
		/*GameObjectList[1]->SetWorldMatrix(rota);*/
		GameObjectList[0]->SetWorldMatrix(rota0*XMMatrixTranslation(5.0f, 0.0f, 0.0f));
		for (int i = 0; i < count; i++)
		{
			//con->OMSetDepthStencilState(m_states->DepthDefault(), 0);
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
	//m_fxFactory.reset(new EffectFactory(mainscene.mApp->device.Get()));
	//m_states.reset(new CommonStates(mainscene.mApp->device.Get()));


	char chplayer[20] = "player";
	/*********************** Create GameObject **************************/
	GameObject* cube = new GameObject(mainscene.mApp,CUBE,L"green_tex.dds",chplayer);
	cube->SetWorldMatrix(XMMatrixTranslation(5.0f, 0.0f, 0.0f));
	GameObjectList.push_back(cube);
	GameObject* cubefbx = new GameObject(mainscene.mApp, FBX, L"green_tex.dds", chplayer);
	GameObjectList.push_back(cubefbx);


	cam = new Camera(GameObjectList);
	mainscene.Running();

	return 0;
}

bool MainScene::InitApp()
{
	if (!D3DUtility::InitApp())
		return false;
}
