#pragma once

#include "EXOVER.h"

#define MAX_PACKET_SIZE 255
#define PT_RECV 0
#define PT_SEND 1

class MyClient
{
public:
	// ���ϰ� ���� ����
	SOCKET m_s;
	bool m_isconnected;

	EXOVER m_exover; // Ŭ���̾�Ʈ Ȯ�� ����ü
	int m_packet_size;  // ���� �����ϰ� �ִ� ��Ŷ�� ũ��
	int	m_prev_packet_size; // ������ recv���� �ϼ����� �ʾƼ� ������ ���� ��Ŷ�� �պκ��� ũ��
	char m_packet[MAX_PACKET_SIZE]; // Ŭ���̾�Ʈ ��Ŷ ����


	MyClient();
	~MyClient();

	void Init();
};

