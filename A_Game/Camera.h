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

	void SetCamPosition(float deltaX, float deltaZ);
	void Follow();
};

