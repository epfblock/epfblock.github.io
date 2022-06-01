#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable : 4996)
#include<iostream>
#include<string.h>
#include<string>
int main(void)
{
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
		std::cout << "错误码:" << a << std::endl;
		WSACleanup();
		system("pause");
		return 0;
	}
	struct sockaddr_in servermsg;
	servermsg.sin_family = AF_INET;
	servermsg.sin_port = htons(12345);
	servermsg.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(socketserver, (const struct sockaddr *)&servermsg, sizeof(servermsg)))
	{
		int a = WSAGetLastError();
		std::cout << "错误码:" << a << std::endl;
		closesocket(socketserver);
		WSACleanup();
		system("pause");
		return 0;
	}
	std::cout << "客户端" << std::endl;
	while(true)
	{
		char a[1500] = { 0 };
		std::cin >> a;
		if (SOCKET_ERROR == send(socketserver, a, sizeof(a), 0))
		{
			int a = WSAGetLastError();
			std::cout << "错误码:" << a << std::endl;
		}
		char buf[1500] = { 0 };
		int res1 = recv(socketserver, buf, 1499, 0);
		if (res1 == 0)
		{
			std::cout << "连接中断" << std::endl;

		}
		else if (SOCKET_ERROR == res1)
		{
			int a = WSAGetLastError();
			std::cout << "错误码:" << a << std::endl;

		}
		else
		{
			std::cout<< " " << buf << std::endl;
		}
	}
	closesocket(socketserver);
	WSACleanup();
	system("pause");
	return 0;
}