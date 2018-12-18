#include "d3dUtility.h"
#include "GameObject.h"
#include <vector>

std::vector<GameObject*> GameObjectList;
float fMoveX = 0.0f;
float fMoveY = 0.0f;
float fMoveZ = 0.0f;
__m128 MoveX = _mm_set1_ps(fMoveX);
__m128 MoveY = _mm_set1_ps(fMoveY);
__m128 MoveZ = _mm_set1_ps(fMoveZ);
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
		MessageBox(hwnd, L"��������", L"����¼�", MB_OK);
		
		break;
	}
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);
		if (wParam == 'W')//W 0x57
		{
			fMoveZ += 0.1f;
		}
		if (wParam == 'S')//W 0x57
		{
			fMoveZ -= 0.1f;
		}
		if (wParam == 'A')//W 0x57
		{
			fMoveX -= 0.1f;
		}
		if (wParam == 'D')//W 0x57
		{
			fMoveX += 0.1f;
		}
		XMMATRIX world;
		//world = XMMatrixIdentity();
		world = XMMatrixTranslation(fMoveX, fMoveY, fMoveZ);

		//__m128 MoveX = _mm_set1_ps(fMoveX);
		GameObjectList[0]->SetWorldMatrix(world);
		break;
	}
		
		
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}







//
////**************����Ϊ��ܺ���******************




bool Display(D3DUtility* mApp)
{
	if (mApp->device)
	{
		//����һ����������ɫ��Ϣ��4��Ԫ�طֱ��ʾ�죬�̣����Լ�alpha
		float ClearColor[4] = { 0.2f, 0.125f, 0.3f, 1.0f };
		//�����ȾĿ����ͼ
		mApp->immediateContext->ClearRenderTargetView(mApp->renderTargetView.Get(), ClearColor);

		//for (auto iter = GameObjectList.cbegin(); iter != GameObjectList.cend(); iter++)
		//{
		//	&iter->;
		//}
		int count = GameObjectList.size();
		for (int i = 0; i < count; i++)
		{
			GameObjectList[i]->technique->GetPassByIndex(0)->Apply(0, mApp->immediateContext.Get());
			mApp->immediateContext->DrawIndexed(36, 0, 0);
		}
	
		mApp->swapChain->Present(0, 0);
	}
	return true;
}
//**************��ܺ���******************

int MainScene::Running()
{
	// Run the message loop.
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		//�ַ���Ϣ,��Ϣ���Ըı䶯����Ⱦ�е�ĳЩ����,���������˶�,
		//����ӽǵȵ�...,��Ϣ�Ĵ�������������Ϣ����(Procedure)��˵
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{
			Display(mApp);
			//OutputDebugString(L"!!!ALIVE!!\r\n");
			//��������ж����������Ⱦ
		}
	}

	return 0;
}


//
// ������WinMain
//
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{

	MainScene mainscene(hInstance);
	mainscene.InitApp();

	GameObject* cube = new GameObject(mainscene.mApp);
	GameObject* boss = new GameObject(mainscene.mApp);
	
	GameObjectList.push_back(cube);
	GameObjectList.push_back(boss);
	//XMMATRIX world;
	////world = XMMatrixIdentity();
	//world = XMMatrixTranslation(fMoveX, fMoveY, fMoveZ);

	////__m128 MoveX = _mm_set1_ps(fMoveX);
	//GameObjectList[0]->SetWorldMatrix(world);

	mainscene.Running();


	return 0;
}

bool MainScene::InitApp()
{
	if (!D3DUtility::InitApp())
		return false;
}
