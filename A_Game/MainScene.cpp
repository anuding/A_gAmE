#include "d3dUtility.h"
//#include "GameObject.h"
#include "Camera.h"
#include "debugprint.h"
#include <iostream>
#include "UpdateMethods.h"

Camera* cam;

std::vector<GameObject*> GameObjectList;
GameObject* pFbxScene;
//std::shared_ptr<EffectFactory>       m_fxFactory;
//std::shared_ptr<CommonStates>        m_states;


class  MainScene : public D3DUtility
{

	XMVECTOR pos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	//const XMVECTOR DIRLEFT = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	//const XMVECTOR DIRRIGHT = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	//const XMVECTOR DIRFORWARD = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	//const XMVECTOR DIRBACK = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	//const XMVECTOR DIRUP = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//const XMVECTOR DIRDOWN = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
public:
	virtual bool InitApp() override;
	MainScene(HINSTANCE hInstance);
	int Running();
	void Update(float dt);
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
	case WM_INPUT:

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_XBUTTONDOWN:

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP:

	case WM_MOUSEWHEEL:
	case WM_MOUSEHOVER:
	case WM_MOUSEMOVE:
		mMouse->ProcessMessage(msg, wParam, lParam);
		return 0;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		return 0;

	case WM_ACTIVATEAPP:
		mMouse->ProcessMessage(msg, wParam, lParam);
		return 0;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}



//
////**************以下为框架函数******************
void MainScene::Update(float dt)
{
	static float cubePhi = 0.0f, cubeTheta = 0.0f;
	// 获取鼠标状态
	Mouse::State mouseState = mMouse->GetState();
	Mouse::State lastMouseState = mMouseTracker.GetLastState();
	// 更新鼠标按钮状态跟踪器，仅当鼠标按住的情况下才进行移动
	mMouseTracker.Update(mouseState);
	if (mouseState.leftButton == true)//mMouseTracker.leftButton == mMouseTracker.HELD)//mouseState.leftButton == true &&
	{
		cubeTheta -= (mouseState.x - lastMouseState.x) * 0.001f;
		cubePhi += (mouseState.y - lastMouseState.y) * 0.001f;
	}
	DP1("Theta: %d\n", cubeTheta);
	DP1("Phi: %d\n", cubePhi);

	auto rota0 = XMMatrixRotationY(cubeTheta) * XMMatrixRotationX(cubePhi);
	auto dir = XMVector4Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
		XMMatrixRotationY(cubeTheta) * XMMatrixRotationX(cubePhi));
	XMVECTOR mY = XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f);
	XMVECTOR front = XMVector3Normalize(dir);
	XMVECTOR back = -front;
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(dir, mY));
	XMVECTOR left = -right;
	XMVECTOR move_vector = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	if (::GetAsyncKeyState('A') & 0x8000f) //响应键盘左方向键
	{
		move_vector += left;
	}
	if (::GetAsyncKeyState('D') & 0x8000f) //响应键盘左方向键
	{
		move_vector += right;
	}
	if (::GetAsyncKeyState('W') & 0x8000f) //响应键盘左方向键
	{
		move_vector += front;
	}
	if (::GetAsyncKeyState('S') & 0x8000f) //响应键盘左方向键
	{
		move_vector += back;
	}
	if (::GetAsyncKeyState('C') & 0x8000f) //响应键盘左方向键
	{
		move_vector += XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	}
	if (::GetAsyncKeyState('V') & 0x8000f) //响应键盘左方向键
	{
		move_vector += XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	}
	XMVECTOR eye = pos + move_vector;
	pos = eye;
	XMVECTOR at = pos + dir;
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(eye, at, up);
	//60 1.0471975511965977461542144610932 1.570796327f
	XMMATRIX projection = XMMatrixPerspectiveFovLH(1.047197551f, 800.0f / 600.0f, 0.01f, 100.0f);


	for (int i = 0; i < GameObjectList.size(); ++i)
	{
		GameObjectList[i]->SetViewProjMatrix(view, projection);
	}
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
	GameObject* cubefbx = new GameObject(mainscene.mApp, FBX, L"archer.dds", chplayer);
	GameObjectList.push_back(cubefbx);


	cam = new Camera(GameObjectList,mainscene.mhMainWnd);
	mainscene.Running();

	return 0;
}

bool MainScene::InitApp()
{
	if (!D3DUtility::InitApp())
		return false;

	mMouse->SetWindow(mhMainWnd);
	mMouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
}
