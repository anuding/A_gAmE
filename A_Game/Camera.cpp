#include "Camera.h"
#include "math.h"


Camera::Camera(std::vector<GameObject*> list)
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

void Camera::MoveCamPosition(float pos_x, float pos_y, float pos_z)
{
	XMVECTOR eye = pos + XMVectorSet(pos_x, pos_y, pos_z, 0.0f);
	XMVECTOR at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(eye, at, up);
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 800.0f / 600.0f, 0.01f, 100.0f);

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
