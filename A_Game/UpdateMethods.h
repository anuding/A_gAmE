//
//bool playerIdleLock = false;
//bool rightLock = false;
//bool leftLock = false;
//bool frontLock = false;
//bool backLock = false;
//
//
//
//
//
//void UpdatePlayer(std::vector<GameObject*> GameObjectList)
//{
//	//计算player到Boss的方向
//	GameObject* bo = GameObjectList[1];
//	GameObject* pl = GameObjectList[0];
//	XMVECTOR PlayerPos = pl->GetPos();
//	XMVECTOR BossPos = bo->GetPos();
//
//	XMFLOAT4 originPos;//Boss原本的位置
//	XMStoreFloat4(&originPos, pl->GetPos());
//	XMVECTOR Dir = BossPos - PlayerPos;
//
//	XMVECTOR mY = XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f);
//	XMVECTOR front = XMVector3Normalize(Dir);
//	XMVECTOR back = -front;
//	XMVECTOR right = XMVector3Normalize(XMVector3Cross(Dir, mY));
//	XMVECTOR left = -right;
//	//float playerSpeed = 1.0f;
//
//	XMVECTOR moveVector = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//	float camSpeed = 1.0f / 360;
//	if (::GetAsyncKeyState('A') & 0x8000f) //响应键盘左方向键
//	{
//		moveVector += left; playerIdleLock = true; leftLock = true;
//	}
//	if (::GetAsyncKeyState('D') & 0x8000f) //响应键盘右方向键
//	{
//		moveVector += right; playerIdleLock = true; rightLock = true;
//	}
//	if (::GetAsyncKeyState('W') & 0x8000f)    //响应键盘上方向键
//	{
//		moveVector += front; playerIdleLock = true;	frontLock = true;
//	}
//	if (::GetAsyncKeyState('S') & 0x8000f)  //响应键盘下方向键
//	{
//		moveVector += back; playerIdleLock = true;	backLock = true;
//	}
//
//
//	XMFLOAT4 deltainDir;
//	XMStoreFloat4(&deltainDir, moveVector); //从Boss指向player的向量  
//	float destX; float destY; float destZ;//Boss应该移动到的位置
//	destX = originPos.x + deltainDir.x*0.0020f;
//	destY = originPos.y + deltainDir.y*0.0007f;
//	destZ = originPos.z + deltainDir.z*0.0020f;
//
//	//rotating
//	XMFLOAT4 plPosf;
//	XMStoreFloat4(&plPosf, PlayerPos);
//	XMFLOAT4 boPosf;
//	XMStoreFloat4(&boPosf, BossPos);
//
//	XMVECTOR x = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//	XMMATRIX world = XMMatrixTranslation(destX, destY, destZ);
//
//	if (!playerIdleLock)//没有锁, 开始idle
//	{
//		pl->UpdateMD5Model(pl->NewMD5Model, 0.0005f, 0);
//		XMVECTOR angleVec = XMVector3AngleBetweenVectors(Dir, x);
//		float angleRadians = XMVectorGetX(angleVec);
//		if (plPosf.z <= boPosf.z)
//			world = XMMatrixRotationY(-angleRadians)*XMMatrixTranslation(destX, destY, destZ);
//		else
//			world = XMMatrixRotationY(angleRadians)*XMMatrixTranslation(destX, destY, destZ);
//	}
//	else//有锁, 计算旋转角度
//	{
//		pl->UpdateMD5Model(pl->NewMD5Model, 0.0005f, 1);
//
//		if (frontLock)
//		{
//			XMVECTOR angleVec = XMVector3AngleBetweenVectors(front, x);
//			float angleRadians = XMVectorGetX(angleVec);
//			if (plPosf.z <= boPosf.z)
//				world = XMMatrixRotationY(-angleRadians)*XMMatrixTranslation(destX, destY, destZ);
//			else
//				world = XMMatrixRotationY(angleRadians)*XMMatrixTranslation(destX, destY, destZ);
//
//		}
//		if (backLock)
//		{
//			XMVECTOR angleVec = XMVector3AngleBetweenVectors(back, x);
//			float angleRadians = XMVectorGetX(angleVec);
//			if (plPosf.z <= boPosf.z)
//				world = XMMatrixRotationY(angleRadians)*XMMatrixTranslation(destX, destY, destZ);
//			else
//				world = XMMatrixRotationY(-angleRadians)*XMMatrixTranslation(destX, destY, destZ);
//		}
//		if (rightLock)
//		{
//			XMVECTOR angleVec = XMVector3AngleBetweenVectors(right, x);
//			float angleRadians = XMVectorGetX(angleVec);
//			if (plPosf.y - boPosf.y > -(plPosf.x - boPosf.x))
//				world = XMMatrixRotationY(-angleRadians)*XMMatrixTranslation(destX, destY, destZ);
//			else
//				world = XMMatrixRotationY(angleRadians)*XMMatrixTranslation(destX, destY, destZ);
//		}
//		if (leftLock)
//		{
//			XMVECTOR angleVec = XMVector3AngleBetweenVectors(right, x);
//			float angleRadians = XMVectorGetX(angleVec);
//			if (plPosf.y - boPosf.y > -(plPosf.x - boPosf.x))
//				world = XMMatrixRotationY(-angleRadians + XM_PI)*XMMatrixTranslation(destX, destY, destZ);
//			else
//				world = XMMatrixRotationY(angleRadians + XM_PI)*XMMatrixTranslation(destX, destY, destZ);
//		}
//	}
//	//world = XMMatrixTranslation(destX, destY, destZ);
//	pl->SetWorldMatrix(world);
//	playerIdleLock = false;
//	rightLock = false;
//	leftLock = false;
//	frontLock = false;
//	backLock = false;
//}
//
//
//void UpdateBoss(std::vector<GameObject*> GameObjectList)
//{	//计算player到Boss的方向
//	GameObject* bo = GameObjectList[1];
//	GameObject* pl = GameObjectList[0];
//
//	if (bo->life < 0) {
//		bo->UpdateMD5Model(bo->NewMD5Model, 0.0005f, 2);
//		return;
//	}
//	XMVECTOR PlayerPos = pl->GetPos();
//	XMVECTOR BossPos = bo->GetPos();
//	XMVECTOR Dir = XMVector3Normalize(PlayerPos - BossPos);
//	XMFLOAT4 deltainDir;
//	XMStoreFloat4(&deltainDir, Dir);
//
//	XMFLOAT4 originPos;
//	XMStoreFloat4(&originPos, BossPos);
//
//	float destX; float destY; float destZ;
//	destX = originPos.x + deltainDir.x*0.0012f;
//	destY = originPos.y + deltainDir.y;
//	destZ = originPos.z + deltainDir.z*0.0012f;
//
//
//	XMFLOAT4 plPosf;
//	XMStoreFloat4(&plPosf, PlayerPos);
//	XMFLOAT4 boPosf;
//	XMStoreFloat4(&boPosf, BossPos);
//
//
//	XMVECTOR x = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//	bo->UpdateMD5Model(bo->NewMD5Model, 0.0005f, 0);
//	XMMATRIX world = XMMatrixIdentity();
//	if (plPosf.z <= boPosf.z)
//	{
//		XMVECTOR angleVec = XMVector3AngleBetweenVectors(Dir, x);
//		float angleRadians = XMVectorGetX(angleVec);
//		world = XMMatrixRotationY(angleRadians)*XMMatrixTranslation(destX, destY, destZ);//
//	}
//	else
//	{
//		XMVECTOR angleVec = XMVector3AngleBetweenVectors(-Dir, x);
//		float angleRadians = XMVectorGetX(angleVec);
//		world = XMMatrixRotationY(angleRadians + XM_PI)*XMMatrixTranslation(destX, destY, destZ);//
//	}
//	bo->SetWorldMatrix(world);
//}
//
//
//
//
