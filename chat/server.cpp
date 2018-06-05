#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <sstream>
#define defaultport "27015"

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

void main()
{
	//initialising winstock
	WSAData data;
	int result = WSAStartup(MAKEWORD(2, 2), &data);
	if (result != 0)
	{
		cout<<"WSA startup failed!"<< endl;
		return;
	}
	else
	{
		cout << "WSA startup complete." << endl;
	}

	//creating socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cout << "Cannot create socket!" << endl;
		return;
	}
	else
	{
		cout << "Listening socket created." << endl;
	}

	//bind ip and port to socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = ADDR_ANY;

	bind(listening, (sockaddr*) &hint, sizeof(hint));

	//setup winsock socket for listening
	listen(listening, SOMAXCONN);

	fd_set master;

	FD_ZERO(&master);

	FD_SET(listening, &master);

	while (true)
	{
		fd_set copy = master;

		int socketcount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketcount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			if (sock == listening)
			{
				//accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				//add a new conn to the list
				FD_SET(client, &master);
				cout << client<< endl;
				//send weolcome msg
				string welcomemsg = "sup.\r\n";
				send(client, welcomemsg.c_str(), welcomemsg.size(), 0);
			}
			else
			{
				char buff[4096];
				ZeroMemory(buff, 4096);

				//receive a msg
				int bytesin = recv(sock, buff, 4096, 0);

				if (bytesin <= 0)
				{
					//drop
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{ 
					stringstream ss;
					ss << "sock" << buff;
					string out = ss.str();
					//send msg to other clients
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET sockout = master.fd_array[i];

						if (sockout != listening && sockout != sock)
						{
							send(sockout, buff, 4096, 0);
						}
					}
				}
			}
		}
	}

	//shutdown winsoc
	WSACleanup();

	system("pause");
}
	
	/*wait for connection
	sockaddr_in client;
	int clientsize = sizeof(client);

	SOCKET clientsocket = accept(listening, (sockaddr*) &client, &clientsize);

	char host[NI_MAXHOST];     //client remote name
	char service[NI_MAXSERV];  // server port

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*) &client, clientsize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl << endl;
	}

	//close listening socket
	closesocket(listening);

	//while loop receive and echo back msg
	char buff[4096];

	while (true)
	{
		ZeroMemory(buff, 4096);

		//wait for client msg
		int bytesreceived = recv(clientsocket, buff, 4096, 0);
		if (bytesreceived == SOCKET_ERROR)
		{
			cout << "error in receive" << endl;
			break;
		}

		if (bytesreceived == 0)
		{
			cout << host << " Disconnected." << endl;
			break;
		}

		//echo msg
		cout << host <<"> " << string(buff, 0, bytesreceived)<< " <server sending back>"<< endl;
		send(clientsocket, buff, bytesreceived+1, 0);
	}*/