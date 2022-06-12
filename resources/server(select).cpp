#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable : 4996)
#include<iostream>
#include<string>
fd_set allsockets;
BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		for (u_int i = 0; i < allsockets.fd_count; i++)
		{
			closesocket(allsockets.fd_array[i]);
		}
		WSACleanup();
	}
	return true;
}
int main(void)
{
	SetConsoleCtrlHandler(fun,true);
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
	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(12345);
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(socketserver, (const struct sockaddr *)&si, sizeof(si)))
	{
		int a = WSAGetLastError();
		std::cout << "错误码:" << a << std::endl;
		closesocket(socketserver);
		WSACleanup();
		system("pause");
		return 0;
	}
	if (SOCKET_ERROR == listen(socketserver, SOMAXCONN))
	{
		int a = WSAGetLastError();
		std::cout << "错误码:" << a << std::endl;
		closesocket(socketserver);
		WSACleanup();
		system("pause");
		return 0;
	}

	FD_ZERO(&allsockets);
	FD_SET(socketserver,&allsockets);
	while (true)
	{
		fd_set readsockets = allsockets;
		fd_set writesockets = allsockets;
		timeval st;
		st.tv_sec = 3;
		st.tv_usec = 0;
		int nres = select(0,&readsockets,&writesockets, NULL,&st);
		if (0 == nres)
		{
			continue;
		}
		else if(nres>0)
		{
			for (u_int i = 0; i < readsockets.fd_count; i++)
			{
				if (readsockets.fd_array[i]==socketserver)
				{
					SOCKET socketclient = accept(socketserver,NULL,NULL);
					if (SOCKET_ERROR==socketclient)
					{
						continue;
					}
 					FD_SET(socketclient,&allsockets);
					if (SOCKET_ERROR == send(socketclient, "信息发送成功", sizeof("信息发送成功"), 0))
					{
						int a = WSAGetLastError();
						std::cout << "错误码:" << a << std::endl;
					} 
				}
				else
				{
					char strbuff[1500] = { 0 };
					int nrecv = recv(readsockets.fd_array[i],strbuff,1500,0);
						if (0 == nrecv)
						{
							SOCKET socketTemp = readsockets.fd_array[i];
							FD_CLR(readsockets.fd_array[i],&allsockets);
							closesocket(socketTemp);
						}
						else if(0<nrecv)
						{
							std::cout << strbuff << std::endl;
						}
						else
						{
							int a=WSAGetLastError();
							std::cout << "错误码:" << a << std::endl;
							if (a==10054)
							{
								SOCKET socketTemp = readsockets.fd_array[i];
								FD_CLR(readsockets.fd_array[i], &allsockets);
								closesocket(socketTemp);
							}
						}
					
				}
			}
		}
		else
		{
			break;
		}
	}
	//FD_CLR(socketserver, &clientsockets);
	//closesocket(socketserver);
	//FD_ISSET(socketserver, &clientsockets);
	for (u_int i=0;i<allsockets.fd_count;i++)
	{
		closesocket(allsockets.fd_array[i]);
	}
	closesocket(socketserver);
	WSACleanup();
	system("pause");
	return 0;
}