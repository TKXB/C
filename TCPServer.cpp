#include <stdio.h>
#include <WinSock2.h>
#pragma comment (lib,"ws2_32")

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
	SOCKET sLisent = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.S_un.S_addr = inet_addr("10.211.55.3");
	ServerAddr.sin_port = htons(1234);
	bind(sLisent, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr));

	listen(sLisent, SOMAXCONN);
	sockaddr_in ClientAddr;
	int nSize = sizeof(ClientAddr);
	SOCKET sClient = accept(sLisent, (SOCKADDR *)&ClientAddr, &nSize);
	printf("ClientIP=%s:%d\r\n",inet_ntoa(ClientAddr.sin_addr),ntohs(ClientAddr.sin_port));
	char szMsg[MAXBYTE] = { 0 };
	lstrcpy(szMsg, "hello Client!\r\n");
	send(sClient, szMsg, strlen(szMsg) + sizeof(char), 0);
	while (true){
	char cMsg[MAXBYTE] = {0};
	recv(sClient, cMsg, MAXBYTE, 0);
	printf("Client Msg : %s \r\n", cMsg);
	}
	WSACleanup();
	return 0;
}
