#include "Camera.h"
#include "math.h"
#include "debugprint.h"
Camera::Camera(std::vector<GameObject*> list, HWND mhMainWnd)
{
	this->list = list;

}


Camera::~Camera()
{
}

void Camera::SetCamPosition(float pos_x, float pos_y, float pos_z)
{
	XMVECTOR eye = XMVectorSet(pos_x, pos_y, pos_z, 0.0f);
	pos = eye;
	XMVECTOR at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(eye, at, up);
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 800.0f / 600.0f, 0.01f, 100.0f);

	for (int i = 0; i < list.size(); i++)
	{
		list[i]->SetViewProjMatrix(view, projection);
	}
}

void Camera::MoveCamPosition(Mouse::State mouseState,
	DirectX::Mouse::ButtonStateTracker mMouseTracker)
{

	static float cubePhi = 0.0f, cubeTheta = 0.0f;
	// 获取鼠标状态
	//Mouse::State mouseState = mMouse->GetState();
	Mouse::State lastMouseState = mMouseTracker.GetLastState();
	// 更新鼠标按钮状态跟踪器，仅当鼠标按住的情况下才进行移动
	mMouseTracker.Update(mouseState);
	if (mouseState.leftButton == true)//mMouseTracker.leftButton == mMouseTracker.HELD)//mouseState.leftButton == true &&
	{
		cubeTheta -= (mouseState.x - lastMouseState.x) * 0.0001f;
		cubePhi -= (mouseState.y - lastMouseState.y) * 0.0001f;
	}
	DP1("Theta: %d\n", cubeTheta);
	DP1("Phi: %d\n", cubePhi);
	/*static float angle = 0.0f;
	angle += 0.0009f;*/
	/*auto rota = XMMatrixRotationY(angle)*XMMatrixTranslation(3.0f, 0.0f, 0.0f);*/
	auto rota0 = XMMatrixRotationY(cubeTheta) * XMMatrixRotationX(cubePhi);
	/*GameObjectList[1]->SetWorldMatrix(rota);*/
	list[0]->SetWorldMatrix(rota0*XMMatrixTranslation(5.0f, 0.0f, 0.0f));


	XMVECTOR move_vector= XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	if (::GetAsyncKeyState('A') & 0x8000f) //响应键盘左方向键
	{
		move_vector += DIRLEFT;
	}
	if (::GetAsyncKeyState('D') & 0x8000f) //响应键盘左方向键
	{
		move_vector += DIRRIGHT;
	}
	if (::GetAsyncKeyState('W') & 0x8000f) //响应键盘左方向键
	{
		move_vector += DIRFORWARD;
	}
	if (::GetAsyncKeyState('S') & 0x8000f) //响应键盘左方向键
	{
		move_vector += DIRBACK;
	}
	XMVECTOR eye = pos + move_vector;
	pos = eye; 
	/*auto dwd=XMVector4Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
		XMMatrixRotationY(cubeTheta) * XMMatrixRotationX(cubePhi));*/
	XMVECTOR at = pos + XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(eye, at, up); 
	//60 1.0471975511965977461542144610932 1.570796327f
	XMMATRIX projection = XMMatrixPerspectiveFovLH(1.047197551f, 800.0f / 600.0f, 0.01f, 100.0f);

	
	for (int i = 0; i < list.size(); ++i)
	{
		list[i]->SetViewProjMatrix(view, projection);
	}
}

void Camera::Follow()
{
	//float step = 1.0f;
	//GameObject* bo = mList[1];
	//GameObject* pl = mList[0];

	////计算Player到Boss的向量
	//XMVECTOR playerpos = pl->GetPos();
	//XMVECTOR bosspos = bo->GetPos();
	//XMVECTOR dir = bosspos - playerpos;


	//XMVECTOR playerBack = playerpos - XMVector3Normalize(dir)* 10.5f;
	//XMFLOAT4 fplayerBack;
	//XMStoreFloat4(&fplayerBack, playerBack);
	//fplayerBack.y = 7.0f;//玩家背后的位置, 也就是camera最终要去的地方


	//					 //下一帧camera的位置Eye = Normalized(currentPos - fplayerBack)*0.1f
	//XMVECTOR dir2 = XMLoadFloat4(&fplayerBack) - campos;
	//XMVECTOR Eye = XMVector3Normalize(dir2)*0.003f + campos;
	//campos = Eye;
	//XMVECTOR At = playerpos + dir / 2.0f;
	//XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//XMMATRIX view = XMMatrixLookAtLH(Eye, At, Up);//eye= currentPos
	//XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 800.0f / 600.0f, 0.01f, 100.0f);

	//int count = mList.size();
	//for (int i = 0; i < count; i++)
	//{
	//	mList[i]->SetViewProjMatrix(view, projection);
	//}
}
