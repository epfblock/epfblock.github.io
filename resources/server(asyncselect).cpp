#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable : 4996)
#include<iostream>
#include<string>
#include<wchar.h>
SOCKET all_socket[1024];
int socket_count = 0;
#define UM_ASYNCSELECTMSG WM_USER+1
int y = 0;
LRESULT CALLBACK WinBackProc(HWND hWnd, UINT msgid, WPARAM wparam, LPARAM lparam)
{
	HDC hdc = GetDC(hWnd);
	switch (msgid)
	{
	case UM_ASYNCSELECTMSG:
	{
		//MessageBox(NULL, TEXT("成功"), TEXT("提示"), MB_OK);
		SOCKET sock = (SOCKET)wparam;
		if (HIWORD(lparam)!=0)
		{
			if (WSAECONNABORTED== HIWORD(lparam))
			{
				TextOut(hdc, 0, y, L"socket_close", strlen("socket_close"));
				y += 15;
				WSAAsyncSelect(sock, hWnd, 0, 0);
				closesocket(sock);
				for (int i = 0; i < socket_count; i++)
				{
					if (sock == all_socket[i])
					{
						all_socket[i] = all_socket[socket_count - 1];
						socket_count--;
						break;
					}

				}
			}
			break;
		}
		switch (LOWORD(lparam))
		{
		case FD_ACCEPT:
		{
			TextOut(hdc,0,y,L"accept",strlen("accept"));
			y += 15;
			SOCKET socketclient = accept(sock, NULL, NULL);
			if (SOCKET_ERROR == socketclient)
			{
				int a = WSAGetLastError();
				break;
			}
			if (SOCKET_ERROR == WSAAsyncSelect(socketclient, hWnd, UM_ASYNCSELECTMSG, FD_READ|FD_WRITE|FD_CLOSE))
			{
				int a = WSAGetLastError();
				closesocket(socketclient);
				break;
			}
			all_socket[socket_count] = socketclient;
			socket_count++;
		}
			
			break;
		case FD_READ:
		{
			char str[1024] = { 0 };			
			if (SOCKET_ERROR==recv(sock, str, 1023, 0))
			{
				break;
			}
			int bufSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
			wchar_t *str1 = new wchar_t[bufSize];
			MultiByteToWideChar(CP_ACP, 0, str, -1, str1, bufSize);
			TextOut(hdc, 0, y,str1,bufSize-1);
			y += 15;
		}

			break;
		case FD_WRITE:
			break;
		case FD_CLOSE:
			TextOut(hdc, 0, y, L"socket_close", strlen("socket_close"));
			y += 15;
			WSAAsyncSelect(sock,hWnd,0,0);
			closesocket(sock);
			for (int i = 0; i < socket_count; i++)
			{
				if (sock==all_socket[i])
				{
					all_socket[i] = all_socket[socket_count - 1];
					socket_count--;
					break;
				}

			}
			break;
		}
		
	}
	break;
	case WM_CREATE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	}
	ReleaseDC(hWnd,hdc);
	return DefWindowProc(hWnd, msgid, wparam, lparam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrInstance,LPSTR lpCmdline,int nShowCmd)
{
	WNDCLASSEX wc;
	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = 0;
	wc.hbrBackground = NULL;
	wc.hCursor = NULL;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WinBackProc;
	wc.lpszClassName = TEXT("MyWindow");
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW|CS_VREDRAW;

	
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, TEXT("注册失败"), TEXT("提示"), MB_OK);
		return 0;
	}

	HWND hWnd1=CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, wc.lpszClassName, TEXT("MyWindow"),WS_OVERLAPPEDWINDOW,200,200,600,400,NULL, NULL, hInstance, NULL);

	if (hWnd1==NULL)
	{
		MessageBox(NULL, TEXT("创建窗口失败"),TEXT("提示"),MB_OK);
		return 0;
	}
	ShowWindow(hWnd1,nShowCmd);
	UpdateWindow(hWnd1);

	WORD wdVersion = MAKEWORD(2, 2);
	WSAData wdSockMsg;
	int res = WSAStartup(wdVersion, &wdSockMsg);
	if (0 != res)
	{
		switch (res)
		{
		case WSASYSNOTREADY:
			std::cout << "请检查网络库" << std::endl;
			break;
		case WSAVERNOTSUPPORTED:
			std::cout << "请更新网络库" << std::endl;
			break;
		case WSAEINPROGRESS:
			std::cout << "请重新启动软件" << std::endl;
			break;
		case WSAEPROCLIM:
			std::cout << "请关闭其他程序，以提供本软件网络资源" << std::endl;
			break;
		default:
			break;
		}
		return 0;
	}
	if (HIBYTE(wdSockMsg.wVersion) != 2 || LOBYTE(wdSockMsg.wVersion) != 2)
	{
		std::cout << "版本错误" << std::endl;
		WSACleanup();
		return 0;
	}
	SOCKET socketserver = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socketserver)
	{
		int a = WSAGetLastError();
		WSACleanup();
		return 0;
	}
	all_socket[socket_count] =socketserver;
	socket_count++;
	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(12345);
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(socketserver, (const struct sockaddr *)&si, sizeof(si)))
	{
		int a = WSAGetLastError();
		closesocket(socketserver);
		WSACleanup();
		return 0;
	}
	if (SOCKET_ERROR == listen(socketserver, SOMAXCONN))
	{
		int a = WSAGetLastError();
		closesocket(socketserver);
		WSACleanup();
		return 0;
	}
	if (SOCKET_ERROR == WSAAsyncSelect(socketserver, hWnd1, UM_ASYNCSELECTMSG, FD_ACCEPT))
	{
		int a = WSAGetLastError();
		closesocket(socketserver);
		WSACleanup();
		return 0;
	}

	MSG msg;
	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	for (int i = 0; i < socket_count; i++)
	{
		closesocket(all_socket[i]);
	}
	closesocket(socketserver);
	WSACleanup();
	return 0;
}
