#include <d3d9.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")


#define WINDOW_CLASS "UGPDX"
#define WINDOW_TITLE "Demo Window"

LPDIRECT3D9 g_D3D = NULL;
LPDIRECT3DDEVICE9 g_D3DDevice = NULL;

LPDIRECT3DVERTEXBUFFER9 g_VertexBuffer = NULL;
//��ʼ��D3D����
bool InitializeD3D(HWND hWnd);
bool InitializeObjects();
void RenderScene();
void Shutdown();


struct stD3DVertex
{
	//x,y,z����  d3d��������ϵ		opengl ��������ϵ
	float x, y, z, rhw;
	unsigned long color;


};

#define D3DFVF_VERTEX	(D3DFVF_XYZRHW|D3DFVF_DIFFUSE)


//��Ϣ������
//����1:���ھ��
//����2:��Ϣ
//����3:��Ϣ����
//����4:��Ϣ����
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	//��������Ҫ�������Ϣ
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
	//������Ϣ ����ת��windowȥ����
	return DefWindowProc(hWnd,msg,wParam,lParam);
}

//HINSTANCE : Ӧ�ó�����
//HWND		: ���ھ��


//WinMain����ʱϵͳ�ṩ���û�WindowsӦ�ó�����ڵ�
//����1(:HINSTANCE) :	�ǵ�ǰӦ�ó���ʵ����Handle
//����2(HINSTANCE)  :	��Ӧ�ó�����һ��ʵ����Handle.(MSDN:�������Ҫ֪�������Ƿ�����һ��ʵ����
//					����ʹ��Mutex��ʵ�֣���Mutex����ʵ��ֻ����һ��ʵ��)
//����3(LPTSTR)		:	�ַ������������в���
//����4(int)		:	int�ͣ�ָ��windowsӦ����ôʵ�֣�windows������һϵ�еĺ�����������,��SW��ͷ,
//					��:SW_SHOW(����ֵ��һ��int��)
int WINAPI WinMain(HINSTANCE hInst,
	HINSTANCE prevhInst,
	LPTSTR cmdLine,
	int show)
{
	//����һ����̨��
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

	//ע�ᴰ��
	RegisterClassEx(&wc);

	//������ͨ����
	HWND hWnd = CreateWindow(WINDOW_CLASS, WINDOW_TITLE,WS_OVERLAPPEDWINDOW,
		100,100,640,480,GetDesktopWindow(),NULL,hInst,NULL);

	if (InitializeD3D(hWnd))
	{
		//�ù�������ָ�����ڵ���ʾ״̬
		ShowWindow(hWnd, SW_SHOWDEFAULT);

		//����ˢ�´���
		UpdateWindow(hWnd);

		//Windows�����У���Ϣ����MSG�ṹ������ʾ��
		MSG msg;
		//�Խṹ�������г�ʼ��������
		ZeroMemory(&msg, sizeof(msg));

		//ѭ��������Ϣ������
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

			}
			else
			{
				//û����ϢҪ�����Ϳ�����������һЩ��������Ϸ���濪��֮��Ŀ����ã�
				//�������d3d���д���
				RenderScene();
			}
		}
	}

	//�Զ����ͷź���
	Shutdown();


	//ע��һ��������
	//����1:��ָ������������ơ�
	//����2:�����ģ��Ĵ������ʵ��
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

	//����
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

	//�����ʾ3dͼ��
	g_D3DDevice->SetStreamSource(0,g_VertexBuffer,0,sizeof(stD3DVertex));
	g_D3DDevice->SetFVF(D3DFVF_VERTEX);
	g_D3DDevice->DrawPrimitive(D3DPT_LINELIST,0,2);

	g_D3DDevice->EndScene();

	g_D3DDevice->Present(NULL,NULL,NULL,NULL);
}



bool InitializeObjects()
{
	unsigned long col = D3DCOLOR_XRGB(255, 255, 255);
	
	//����һЩ����
	stD3DVertex objData[] = 
	{
		{420.0f,150.0f,0.5,1.0f,col,},
		{420.0f,350.0f,0.5,1.0f,col,},
		{220.0f,150.0f,0.5,1.0f,col,},
		{220.0f,350.0f,0.5,1.0f,col,},
	};

	//����һ�����㻺��
	if(FAILED(g_D3DDevice->CreateVertexBuffer(sizeof(objData), 0,D3DFVF_VERTEX, D3DPOOL_DEFAULT,&g_VertexBuffer,NULL)))
		return false;

	void *ptr;

	if (g_VertexBuffer->Lock(0, sizeof(objData), (void **)&ptr, 0))
		return false;

	//�����е����ݿ��������㻺����
	memcpy(ptr, objData,sizeof(objData));

	//����
	g_VertexBuffer->Unlock();

	return true;


}