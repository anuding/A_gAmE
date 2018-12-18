#include "d3dUtility.h"
#include "GameObject.h"

class  MainScene : public D3DUtility
{
public:
	virtual bool InitApp() override;
	MainScene(HINSTANCE hInstance);
	int Running();
};
MainScene::MainScene(HINSTANCE hInstance) : D3DUtility(hInstance)
{

}

int MainScene::Running()
{
	// Run the message loop.
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		//分发消息,消息可以改变动画渲染中的某些参数,比如人物运动,
		//相机视角等等...,消息的处理将放在下面消息处理(Procedure)来说
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{
			//声明一个数组存放颜色信息，4个元素分别表示红，绿，蓝以及alpha
			float ClearColor[4] = { 0.2f, 0.125f, 0.3f, 1.0f };
			//清除渲染目标视图
			immediateContext->ClearRenderTargetView(renderTargetView, ClearColor);
			swapChain->Present(0, 0);
			//OutputDebugString(L"!!!ALIVE!!\r\n");
			//在这里进行动画计算和渲染
		}
	}

	return 0;
}





//
////**************以下为框架函数******************
//bool Setup(GameObject* obj)
//{
//	
//	immediateContext->IASetInputLayout(obj->vertexLayout);
//	immediateContext->IASetVertexBuffers(0, 1, &(obj->vertexBuffer), &obj->stride, &obj->offset);
//	immediateContext->IASetIndexBuffer(obj->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
//	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	return true;
//}
//
//void Cleanup()
//{
//	//释放全局指针
//	if (renderTargetView) renderTargetView->Release();
//	if (immediateContext) immediateContext->Release();
//	if (swapChain) swapChain->Release();
//	if (device) device->Release();
//	/*if (m_VertexShader) m_VertexShader->Release();
//	if (m_PixelShader) m_PixelShader->Release();
//
//	if (vertexLayout) vertexLayout->Release();
//	if (effect) effect->Release();*/
//	if (depthStencilView) depthStencilView->Release();
//}
//
//bool Display(float timeDelta)
//{
//	if (device)
//	{
//
//		//清除渲染目标视图
//		immediateContext->ClearRenderTargetView(renderTargetView, ClearColor);
//		
//			
//		gtechnique->GetPassByIndex(0)->Apply(0, immediateContext);
//
//		immediateContext->DrawIndexed(36, 0, 0);
//		swapChain->Present(0, 0);
//	}
//	return true;
//}
//**************框架函数******************

//
////
//// 回调函数
////
//LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	switch (msg)
//	{
//	case WM_DESTROY:
//		::PostQuitMessage(0);
//		break;
//
//	case WM_KEYDOWN:
//		if (wParam == VK_ESCAPE)
//			::DestroyWindow(hwnd);
//		break;
//	}
//	return ::DefWindowProc(hwnd, msg, wParam, lParam);
//}

//
// 主函数WinMain
//
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{

	MainScene mainscene(hInstance);
	//TestApp *theApp = new TestApp(hInstance);
	mainscene.InitApp();
	mainscene.Running();


	return 0;
}

bool MainScene::InitApp()
{
	if (!D3DUtility::InitApp())
		return false;
}
