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
		MessageBox(hwnd, L"��������", L"����¼�", MB_OK);
		
		break;
	}
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
	{
		//����player��Boss�ķ���
		GameObject* bo = GameObjectList[1];
		GameObject* pl = GameObjectList[0];
		XMFLOAT4 originPos;//Bossԭ����λ��
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
		XMStoreFloat4(&deltainDir, moveVector); //��Bossָ��player������  
		float destX; float destY; float destZ;//BossӦ���ƶ�����λ��
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
////**************����Ϊ��ܺ���******************




bool Display(D3DUtility* mApp)
{
	if (mApp->device)
	{
		float camSpeed = 1.0f / 360;
		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f) //��Ӧ���������
			CamdeltaX -= camSpeed;
		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f) //��Ӧ�����ҷ����
			CamdeltaX += camSpeed;
		if (::GetAsyncKeyState(VK_UP) & 0x8000f)    //��Ӧ�����Ϸ����
			CamdeltaZ += camSpeed;
		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)  //��Ӧ�����·����
			CamdeltaZ -= camSpeed;

		cam->SetCamPosition(CamdeltaX, CamdeltaZ);
		//cam->Follow();

		int count = GameObjectList.size();
		for (int i = 0; i < count; i++)
		{
			if (strcmp(GameObjectList[i]->tag, "boss") == 0)
			{
				GameObjectList[i]->BOSS();//ÿ��׷��һ���
			}
		}

		//����һ����������ɫ��Ϣ��4��Ԫ�طֱ��ʾ�죬�̣����Լ�alpha
		float ClearColor[4] = { 0.2f, 0.125f, 0.3f, 1.0f };
		//�����ȾĿ����ͼ
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
			mApp->immediateContext->Draw(36, 0);   //����������
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
			//��������ж����������Ⱦ
			//Update();//��������
			Display(mApp);//��Ⱦ
			
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
