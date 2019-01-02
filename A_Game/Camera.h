#pragma once


#include "GameObject.h"
class Camera
{
public:

	Camera(std::vector<GameObject*> List);
	~Camera();
	float x = 3.0f;
	float z = -5.0f;
	std::vector<GameObject*> mList;
	XMVECTOR campos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	void SetCamPosition();
	void PlayerDead();
	void Follow();
};

