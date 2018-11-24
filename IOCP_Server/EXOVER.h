#pragma once

#include <WinSock2.h>

#define MAX_BUFF_SIZE 4000

class EXOVER
{
public:
	WSAOVERLAPPED m_over;
	char m_iobuf[MAX_BUFF_SIZE];
	WSABUF m_wsabuf;
	unsigned char m_packetType;

	EXOVER();
	~EXOVER();
};

