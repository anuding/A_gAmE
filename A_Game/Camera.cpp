#include "Camera.h"
#include "math.h"


Camera::Camera(std::vector<GameObject*> List)
{
	mList = List;
	XMVECTOR Eye = XMVectorSet(3.0f, 5.0f, -5.0f, 0.0f);
	campos = Eye;
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(Eye, At, Up);
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 800.0f / 600.0f, 0.01f, 100.0f);

	int count = List.size();
	for (int i = 0; i < count; i++)
	{
		mList[i]->SetViewMatrix(view);
		mList[i]->SetProjMatrix(projection);
	}
}


Camera::~Camera()
{
}

void Camera::SetCamPosition()
{
	float step = 1.0f;
	GameObject* bo = mList[1];
	GameObject* pl = mList[0];

	//计算Player到Boss的向量
	XMVECTOR playerpos = pl->GetPos();
	XMVECTOR bossPos = bo->GetPos();
	XMVECTOR dir = bossPos - playerpos;
	XMFLOAT4 bosspos;
	XMFLOAT4 dirpos;
	XMFLOAT4 cmpos;

	XMStoreFloat4(&bosspos, bossPos);
	XMStoreFloat4(&cmpos, campos);
	XMStoreFloat4(&dirpos, dir);

	cmpos.x = cmpos.x + dirpos.x / abs(dirpos.x)*0.0004f;

	campos = XMVectorSet(cmpos.x, cmpos.y, cmpos.z, 0.0f);

	XMVECTOR Eye = campos;
	campos = Eye;


	XMVECTOR At = XMVectorSet(bosspos.x, bosspos.y, bosspos.z, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(Eye, At, Up);
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 800.0f / 600.0f, 0.01f, 100.0f);

	int count = mList.size();
	for (int i = 0; i < count; i++)
	{
		mList[i]->SetViewMatrix(view);
		mList[i]->SetProjMatrix(projection);
	}
}


void Camera::PlayerDead()
{
	float step = 1.0f;
	GameObject* bo = mList[1];
	GameObject* pl = mList[0];

	//计算Player到Boss的向量
	XMVECTOR playerpos = pl->GetPos();
	XMVECTOR bossPos = bo->GetPos();
	XMVECTOR dir = bossPos - playerpos;
	XMFLOAT4 bosspos;
	XMFLOAT4 dirpos;
	XMFLOAT4 cmpos;

	XMStoreFloat4(&bosspos, bossPos);
	XMStoreFloat4(&cmpos, campos);
	XMStoreFloat4(&dirpos, dir);

	cmpos.x = cmpos.x - dirpos.x / abs(dirpos.x)*0.0004f;

	campos = XMVectorSet(cmpos.x, cmpos.y, cmpos.z, 0.0f);

	XMVECTOR Eye = campos;
	campos = Eye;
	bosspos.x = bosspos.x - dirpos.x / abs(dirpos.x)*0.0004f;
	bosspos.z = bosspos.z - dirpos.z / abs(dirpos.z)*0.0004f;
	XMVECTOR At = XMVectorSet(bosspos.x, bosspos.y, bosspos.z, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(Eye, At, Up);
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 800.0f / 600.0f, 0.01f, 100.0f);

	int count = mList.size();
	for (int i = 0; i < count; i++)
	{
		mList[i]->SetViewMatrix(view);
		mList[i]->SetProjMatrix(projection);
	}
}

void Camera::Follow()
{
	float step = 1.0f;
	GameObject* bo = mList[1];
	GameObject* pl = mList[0];

	//计算Player到Boss的向量
	XMVECTOR playerpos = pl->GetPos();
	XMVECTOR bosspos = bo->GetPos();
	XMVECTOR dir = bosspos - playerpos;


	XMVECTOR playerBack = playerpos - XMVector3Normalize(dir)* 10.5f;
	XMFLOAT4 fplayerBack;
	XMStoreFloat4(&fplayerBack, playerBack);
	fplayerBack.y = 7.0f;//玩家背后的位置, 也就是camera最终要去的地方


						 //下一帧camera的位置Eye = Normalized(currentPos - fplayerBack)*0.1f
	XMVECTOR dir2 = XMLoadFloat4(&fplayerBack) - campos;
	XMVECTOR Eye = XMVector3Normalize(dir2)*0.003f + campos;
	campos = Eye;
	XMVECTOR At = playerpos + dir / 2.0f;
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(Eye, At, Up);//eye= currentPos
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 800.0f / 600.0f, 0.01f, 100.0f);

	int count = mList.size();
	for (int i = 0; i < count; i++)
	{
		mList[i]->SetViewMatrix(view);
		mList[i]->SetProjMatrix(projection);
	}
}
