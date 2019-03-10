#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")


#define WINDOW_CLASS "UGPDX"
#define WINDOW_TITLE "Demo Window"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
LPDIRECT3D9 g_D3D = NULL;
LPDIRECT3DDEVICE9 g_D3DDevice = NULL;

D3DXMATRIX g_projetion;			//透视矩阵
D3DXMATRIX g_worldMatrix;		//世界矩阵
D3DXMATRIX g_translation;		//平移矩阵
D3DXMATRIX g_rotaltion;			//旋转矩阵

float g_angle = 0.0f;

LPDIRECT3DVERTEXBUFFER9 g_VertexBuffer = NULL;
//初始化D3D窗口
bool InitializeD3D(HWND hWnd);
bool InitializeObjects();
void RenderScene();
void Shutdown();


struct stD3DVertex
{
	//x,y,z坐标  d3d左手坐标系		opengl 右手坐标系
	float x, y, z, rhw;
	unsigned long color;


};

#define D3DFVF_VERTEX	(D3DFVF_XYZ|D3DFVF_DIFFUSE)


//消息处理函数
//参数1:窗口句柄
//参数2:消息
//参数3:消息参数
//参数4:消息参数
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	//处理你想要处理的消息
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return  0;
		break;


	case WM_KEYUP:
		if (wParam = VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		break;
	default:
		break;
	}
	//其他消息 可以转给window去处理
	return DefWindowProc(hWnd,msg,wParam,lParam);
}

//HINSTANCE : 应用程序句柄
//HWND		: 窗口句柄


//WinMain函数时系统提供给用户Windows应用程序入口点
//参数1(:HINSTANCE) :	是当前应用长须实例的Handle
//参数2(HINSTANCE)  :	是应用程序上一个实例的Handle.(MSDN:如果你想要知道程序是否有另一个实例，
//					建议使用Mutex来实现，用Mutex可以实现只运行一个实例)
//参数3(LPTSTR)		:	字符串，是命令行参数
//参数4(int)		:	int型，指明windows应该怎么实现，windows定义了一系列的宏来帮助记忆,以SW开头,
//					如:SW_SHOW(返回值是一个int型)
int WINAPI WinMain(HINSTANCE hInst,
	HINSTANCE prevhInst,
	LPTSTR cmdLine,
	int show)
{
	//属于一个窗台类
	WNDCLASSEX wc = 
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		MsgProc,
		0,
		0,
		hInst,
		NULL,
		NULL,
		NULL,
		NULL,
		WINDOW_CLASS,
		NULL
	};

	//注册窗口
	RegisterClassEx(&wc);

	//创建普通窗口
	HWND hWnd = CreateWindow(WINDOW_CLASS, WINDOW_TITLE,WS_OVERLAPPEDWINDOW,
		100,100,WINDOW_WIDTH,WINDOW_HEIGHT,GetDesktopWindow(),NULL,hInst,NULL);

	if (InitializeD3D(hWnd))
	{
		//该功能设置指定窗口的显示状态
		ShowWindow(hWnd, SW_SHOWDEFAULT);

		//立即刷新窗口
		UpdateWindow(hWnd);

		//Windows程序中，消息是由MSG结构体来表示的
		MSG msg;
		//对结构体对象进行初始化或清零
		ZeroMemory(&msg, sizeof(msg));

		//循环访问消息并处理
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

			}
			else
			{
				//没有消息要处理，就可以做其他的一些工作（游戏引擎开发之类的可以用）
				//这里就是d3d进行处理
				RenderScene();
			}
		}
	}

	//自定义释放函数
	Shutdown();


	//注销一个窗口类
	//参数1:它指定窗口类的名称。
	//参数2:句柄的模块的创建类的实例
	UnregisterClass(WINDOW_CLASS, hInst);
}
		

bool InitializeD3D(HWND hWnd)
{
	D3DDISPLAYMODE displayMode;

	g_D3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(g_D3D == NULL)return false;

	if (FAILED(g_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode)))
	{
		return false;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.BackBufferFormat = displayMode.Format;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	if (FAILED(g_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_D3DDevice)))
	{
		return false;
	}

	//调用
	if (!InitializeObjects())
		return false;
	return true;


}

void Shutdown()
{
	if (g_D3DDevice != NULL) g_D3DDevice->Release();
	if (g_D3D != NULL)g_D3D->Release();
	if (g_VertexBuffer != NULL)g_VertexBuffer->Release();

	g_D3DDevice = NULL;
	g_D3D = NULL;
	g_VertexBuffer = NULL;
}

void RenderScene()
{
	g_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0),1.0f,0);
	g_D3DDevice->BeginScene();

	D3DXMatrixTranslation(&g_translation, 0.0f, 0.0f, 3.0f);
	D3DXMatrixRotationY(&g_rotaltion, g_angle);
	g_worldMatrix = g_rotaltion * g_translation;

	g_angle += 0.01f;
	if (g_angle >= 360) g_angle = 0.0f;

	g_D3DDevice->SetTransform(D3DTS_WORLD, &g_worldMatrix);
	//输出显示3d图形
	g_D3DDevice->SetStreamSource(0,g_VertexBuffer,0,sizeof(stD3DVertex));
	g_D3DDevice->SetFVF(D3DFVF_VERTEX);
	//g_D3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, 4);	//绘制4个点
	//g_D3DDevice->DrawPrimitive(D3DPT_LINELIST,0,2);		//绘制两条线
	g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);	//绘制一个三角形

	//g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);	//绘制一个四边形(由两个三角形组成)


	g_D3DDevice->EndScene();

	g_D3DDevice->Present(NULL,NULL,NULL,NULL);
}



bool InitializeObjects()
{
	

	//透视矩阵
	D3DXMatrixPerspectiveFovLH(&g_projetion,45.0f,WINDOW_WIDTH/WINDOW_HEIGHT,0.1f,1000.0f);

	////正交投影变换
	//g_D3DDevice->SetTransform(D3DTS_PROJECTION, &g_ortho);
		//透视投影变换
	g_D3DDevice->SetTransform(D3DTS_PROJECTION, &g_projetion);
	//关闭光照
	g_D3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	//取消 背面消隐
	g_D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	//透视数据
	stD3DVertex objData[] =
	{
		{-0.3f,-0.3f,1.0F, D3DCOLOR_XRGB(255, 255,0)},
		{0.3f, -0.3f,1.0f, D3DCOLOR_XRGB(255, 0, 0)},
		{0.0f, 0.3f, 1.0f, D3DCOLOR_XRGB(0, 0,255)},
	};

	//创建一个顶点缓存
	if(FAILED(g_D3DDevice->CreateVertexBuffer(sizeof(objData), 0,D3DFVF_VERTEX, D3DPOOL_DEFAULT,&g_VertexBuffer,NULL)))
		return false;

	void *ptr;

	if (g_VertexBuffer->Lock(0, sizeof(objData), (void **)&ptr, 0))
		return false;

	//数据中的数据拷贝到顶点缓存中
	memcpy(ptr, objData,sizeof(objData));

	//解锁
	g_VertexBuffer->Unlock();

	return true;


}