#pragma once


#include "GameObject.h"


class Camera
{
private:
	const XMVECTOR DIRLEFT = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DIRRIGHT = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DIRFORWARD = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DIRBACK = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DIRUP = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DIRDOWN = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
private:
	std::vector<GameObject*> list;
	XMVECTOR pos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	//std::unique_ptr<DirectX::Mouse> mMouse;
	//DirectX::Mouse::ButtonStateTracker mMouseTracker;			// Êó±ê×´Ì¬×·×ÙÆ÷

public:
	Camera(std::vector<GameObject*> list, HWND mhMainWnd);
	~Camera();
	void SetCamPosition(float pos_x, float pos_y, float pos_z);
	void MoveCamPosition(Mouse::State mouseState,
		DirectX::Mouse::ButtonStateTracker mMouseTracker);
	void Follow();
};

