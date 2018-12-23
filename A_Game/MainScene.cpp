#include "d3dUtility.h"
#include "GameObject.h"
#include "Camera.h"
//#include "debugprint.h"
#include <iostream>
Camera* cam;
std::vector<GameObject*> GameObjectList;
//float fMoveX = 0.0f;
//float fMoveY = 0.0f;
//float fMoveZ = 0.0f;
//__m128 MoveX = _mm_set1_ps(fMoveX);
//__m128 MoveY = _mm_set1_ps(fMoveY);
//__m128 MoveZ = _mm_set1_ps(fMoveZ);

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
		//计算player到Boss的方向
		GameObject* bo = GameObjectList[1];
		GameObject* pl = GameObjectList[0];
		XMFLOAT4 originPos;//Boss原本的位置
		XMStoreFloat4(&originPos, pl->GetPos());
		XMVECTOR dir = bo->GetPos() - pl->GetPos();
		
		XMVECTOR tmp = XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f);
		XMVECTOR front = XMVector3Normalize(dir);
		XMVECTOR back = -front;
		XMVECTOR right = XMVector3Cross(dir, tmp);
		XMVECTOR left = -right;
		//float playerSpeed = 1.0f;

		XMVECTOR moveVector=XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);
		if (wParam == 'W')//W 0x57
		{
			moveVector +=  front;
		}
		if (wParam == 'S')//W 0x57
		{
			moveVector +=  back;
		}
		if (wParam == 'A')//W 0x57
		{
			moveVector +=  left;
		}
		if (wParam == 'D')//W 0x57
		{
			moveVector +=  right;
		}
		XMMATRIX world;
		XMFLOAT4 deltainDir;
		XMStoreFloat4(&deltainDir, moveVector); //从Boss指向player的向量  
		float destX; float destY; float destZ;//Boss应该移动到的位置
		destX = originPos.x + deltainDir.x*0.1f;
		destY = originPos.y + deltainDir.y*0.1f;
		destZ = originPos.z + deltainDir.z*0.1f;

		world = XMMatrixTranslation(destX, destY, destZ);
		
		GameObjectList[0]->SetWorldMatrix(world);
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
		float camSpeed = 1.0f / 360;
		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f) //响应键盘左方向键
			CamdeltaX -= camSpeed;
		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f) //响应键盘右方向键
			CamdeltaX += camSpeed;
		if (::GetAsyncKeyState(VK_UP) & 0x8000f)    //响应键盘上方向键
			CamdeltaZ += camSpeed;
		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)  //响应键盘下方向键
			CamdeltaZ -= camSpeed;

		cam->SetCamPosition(CamdeltaX, CamdeltaZ);
		//cam->Follow();

		int count = GameObjectList.size();
		for (int i = 0; i < count; i++)
		{
			if (strcmp(GameObjectList[i]->tag, "boss") == 0)
			{
				GameObjectList[i]->BOSS();//每次追踪一点点
			}
		}

		//声明一个数组存放颜色信息，4个元素分别表示红，绿，蓝以及alpha
		float ClearColor[4] = { 0.2f, 0.125f, 0.3f, 1.0f };
		//清除渲染目标视图
		mApp->immediateContext->ClearRenderTargetView(mApp->renderTargetView.Get(), ClearColor);
		mApp->immediateContext->ClearDepthStencilView(mApp->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		//for (auto iter = GameObjectList.cbegin(); iter != GameObjectList.cend(); iter++)
		//{
		//	&iter->;
		//}
		//int count = GameObjectList.size();
		for (int i = 0; i < count; i++)
		{
			GameObjectList[i]->technique->GetPassByIndex(0)->Apply(0, mApp->immediateContext.Get());
			//mApp->immediateContext->DrawIndexed(36, 0, 0);
			mApp->immediateContext->Draw(36, 0);   //绘制立方体
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
	GameObject* boss = new GameObject(mainscene.mApp);

	const WCHAR *pwcsName = L"player.dds";
	player->buildTexture(pwcsName);
	strcpy_s(player->tag,"player");


	pwcsName=L"boss.dds";
	boss->buildTexture(pwcsName);
	strcpy_s(boss->tag, "boss");


	

	GameObjectList.push_back(player);
	GameObjectList.push_back(boss);

	boss->communicateList =&GameObjectList;

	player->SetWorldMatrix(XMMatrixTranslation(5, 0, 0));
	cam = new Camera(GameObjectList);


	mainscene.Running();


	return 0;
}

bool MainScene::InitApp()
{
	if (!D3DUtility::InitApp())
		return false;
}
