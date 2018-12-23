#include "Camera.h"



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

void Camera::SetCamPosition(float deltaX, float deltaZ)
{
	XMVECTOR Eye = XMVectorSet(deltaX, 5.0f, deltaZ, 0.0f);

	XMVECTOR At = XMVectorSet(deltaX-3.0f, 0.0f, deltaZ+5.0f, 0.0f);
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

	//����Player��Boss������
	XMVECTOR playerpos = pl->GetPos();
	XMVECTOR bosspos = bo->GetPos();
	XMVECTOR dir = bosspos-playerpos;
	

	XMVECTOR playerBack = playerpos - XMVector3Normalize(dir)* 10.5f;
	XMFLOAT4 fplayerBack;
	XMStoreFloat4(&fplayerBack, playerBack);
	fplayerBack.y = 7.0f;//��ұ����λ��, Ҳ����camera����Ҫȥ�ĵط�


	//��һ֡camera��λ��Eye = Normalized(currentPos - fplayerBack)*0.1f
	XMVECTOR dir2 = XMLoadFloat4(&fplayerBack) - campos;
	XMVECTOR Eye = XMVector3Normalize(dir2)*0.003f+campos;
	campos = Eye;
	XMVECTOR At = playerpos+dir/2.0f;
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
