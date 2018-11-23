#include "MyClient.h"

MyClient::MyClient()
{
	m_isconnected = false;
	Init();
}


MyClient::~MyClient()
{
}

void MyClient::Init()
{
	ZeroMemory(&m_exover.m_over, sizeof(WSAOVERLAPPED));
	m_exover.m_wsabuf.buf = m_exover.m_iobuf;
	m_exover.m_wsabuf.len = sizeof(m_exover.m_iobuf);
	m_exover.m_packetType = PT_RECV;

	m_packet_size = 0;
	m_prev_packet_size = 0;
}