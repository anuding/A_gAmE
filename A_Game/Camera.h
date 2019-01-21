#pragma once


#include "GameObject.h"
class Camera
{
private:
	std::vector<GameObject*> list;
	XMVECTOR pos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
public:
	Camera(std::vector<GameObject*> list);
	~Camera();
	void SetCamPosition(float pos_x, float pos_y, float pos_z);
	void MoveCamPosition(float pos_x, float pos_y, float pos_z);
	void Follow();
};

