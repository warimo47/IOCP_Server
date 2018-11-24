#pragma once

#include "EXOVER.h"

#define MAX_PACKET_SIZE 255
#define PT_RECV 0
#define PT_SEND 1

class MyClient
{
public:
	// 소켓과 연결 여부
	SOCKET m_s;
	bool m_isconnected;

	EXOVER m_exover; // 클라이언트 확장 구조체
	int m_packet_size;  // 지금 조립하고 있는 패킷의 크기
	int	m_prev_packet_size; // 지난번 recv에서 완성되지 않아서 저장해 놓은 패킷의 앞부분의 크기
	char m_packet[MAX_PACKET_SIZE]; // 클라이언트 패킷 버퍼


	MyClient();
	~MyClient();

	void Init();
};

