#include "d3dUtility.h"
//#include "GameObject.h"
#include "Camera.h"
#include "debugprint.h"
#include <iostream>
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
		
		break;
	}
		
		
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}


//
////**************����Ϊ��ܺ���******************
void Update()
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

	XMVECTOR moveVector = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	float camSpeed = 1.0f / 360;
	if (::GetAsyncKeyState(VK_LEFT) & 0x8000f) //��Ӧ���������
		moveVector += left;
	if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f) //��Ӧ�����ҷ����
		moveVector += right;
	if (::GetAsyncKeyState(VK_UP) & 0x8000f)    //��Ӧ�����Ϸ����
		moveVector += front;
	if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)  //��Ӧ�����·����
		moveVector += back;
	XMMATRIX world;
	XMFLOAT4 deltainDir;
	XMStoreFloat4(&deltainDir, moveVector); //��Bossָ��player������  
	float destX; float destY; float destZ;//BossӦ���ƶ�����λ��
	destX = originPos.x + deltainDir.x*0.0007f;
	destY = originPos.y + deltainDir.y*0.0007f;
	destZ = originPos.z + deltainDir.z*0.0007f;

	world = XMMatrixTranslation(destX, destY, destZ);

	GameObjectList[0]->SetWorldMatrix(world);

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
		static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA = (0,0,0,255)
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
			Update();//��������
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

	GameObject* boss = new GameObject(mainscene.mApp,MD5, L"hellknight.md5mesh");

	GameObject* player = new GameObject(mainscene.mApp, MD5, L"boy.md5mesh");
	//player->mCBuffer.world= XMMatrixTranspose(XMMatrixRotationX(0.5f) * XMMatrixRotationY(0.4f));
	player->SetWorldMatrix(XMMatrixTranslation(3, 0, 0));



	GameObjectList.push_back(player);
	GameObjectList.push_back(boss);
	
	
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
