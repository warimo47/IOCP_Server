#pragma comment(lib, "ws2_32.lib")

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <iostream>

#include "MyClient.h"
#include "MyTime.h"
#include "Error.h"

#define MY_SERVER_PORT 6000

////////// IOCP ////////////////////////////////////////////////

HANDLE gh_iocp;

////// 클라이언트 배열 //////////////////////////////////////////

std::vector<MyClient> g_clients;
// std::array<MyClient, NUM_OF_USER> g_clients;

std::chrono::steady_clock::time_point timerStartTime;
std::priority_queue <MyTime, std::vector<MyTime>, MyTime> timer_queue;
std::mutex timeLock;

void initialize()
{
	timerStartTime = std::chrono::high_resolution_clock::now();

	srand(static_cast<unsigned int>(time(0)));

	gh_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	std::wcout.imbue(std::locale("korean"));

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
}

void StartRecv(int id)
{
	unsigned long r_flag = 0;

	ZeroMemory(&g_clients[id].m_exover.m_over, sizeof(WSAOVERLAPPED));

	int ret = WSARecv(g_clients[id].m_s, &g_clients[id].m_exover.m_wsabuf, 1,
		NULL, &r_flag, &g_clients[id].m_exover.m_over, NULL);

	if (0 != ret)
	{
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) error_display("Recv Error ", err_no);
	}
}

void SendPacket(int id, void *ptr)
{
	char *packet = reinterpret_cast<char *>(ptr);
	EXOVER *s_over = new EXOVER;
	s_over->m_packetType = PT_SEND;
	memcpy(s_over->m_iobuf, packet, packet[0]);
	s_over->m_wsabuf.buf = s_over->m_iobuf;
	s_over->m_wsabuf.len = s_over->m_iobuf[0];
	ZeroMemory(&s_over->m_over, sizeof(WSAOVERLAPPED));

	int res = WSASend(g_clients[id].m_s, &s_over->m_wsabuf,
		1, NULL, 0, &s_over->m_over, NULL);

	if (0 != res)
	{
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) error_display("Send Error! ", err_no);
	}
}

void ProcessPacket(int id, char *packet)
{
	switch (packet[1])
	{
	default:
		std::cout << "Unkown Packet Type from Client [" << id << "]\n\n";
		return;
	}
}

void DisconnectPlayer(int id)
{
	closesocket(g_clients[id].m_s);
	g_clients[id].m_isconnected = false;

	std::cout << "Logout from Client [" << id << "]\n\n";
}

void timer_thread()
{
	std::cout << "Timer thread Start!\n\n";

	while (true)
	{
		while (true)
		{
			auto nownanoTime = std::chrono::high_resolution_clock::now() - timerStartTime;
			long long nowTime = std::chrono::duration_cast<std::chrono::milliseconds>(nownanoTime).count();

			timeLock.lock();

			if (timer_queue.empty())
			{
				timeLock.unlock();
				break;
			}

			if (timer_queue.top().t > nowTime)
			{
				timeLock.unlock();
				break;
			}

			int key = timer_queue.top().id;
			unsigned char et = timer_queue.top().eventType;

			timer_queue.pop();

			timeLock.unlock();

			EXOVER* postOver = new EXOVER;

			postOver->m_over.hEvent = 0;
			postOver->m_over.Internal = 0;
			postOver->m_over.InternalHigh = 0;
			postOver->m_over.Offset = 0;
			postOver->m_over.OffsetHigh = 0;
			postOver->m_over.Pointer = 0;
			postOver->m_packetType = et;
			postOver->m_wsabuf.buf = postOver->m_iobuf;
			postOver->m_wsabuf.len = 0;

			PostQueuedCompletionStatus(gh_iocp, 1, key, &postOver->m_over);
		}

		Sleep(1000);
	}
}

void worker_thread()
{
	while (true)
	{
		unsigned long io_size;
		unsigned long long iocp_key; // 64 비트 integer , 우리가 64비트로 컴파일해서 64비트
		WSAOVERLAPPED *over;
		BOOL ret = GetQueuedCompletionStatus(gh_iocp, &io_size, &iocp_key, &over, INFINITE);
		int key = static_cast<int>(iocp_key);
		// cout << "WT: Network I/O with Client [" << key << "]\n\n";

		if (FALSE == ret)
		{
			std::cout << "Error in GQCS\n";
			DisconnectPlayer(key);
			continue;
		}

		if (0 == io_size)
		{
			DisconnectPlayer(key);
			continue;
		}

		EXOVER *p_over = reinterpret_cast<EXOVER *>(over);

		// cout << "WT: Is Recv ? [" << p_over->is_recv << "]\n\n";

		if (p_over->m_packetType == PT_RECV)
		{
			// cout << "WT: Packet From Client [" << key << "]\n\n";
			int work_size = io_size;
			char *wptr = p_over->m_iobuf;
			while (work_size)
			{
				int p_size;
				if (g_clients[key].m_packet_size)
					p_size = g_clients[key].m_packet_size;
				else
				{
					p_size = wptr[0];
					g_clients[key].m_packet_size = p_size;
				}

				int need_size = p_size - g_clients[key].m_prev_packet_size;

				if (need_size <= work_size)
				{
					memcpy(g_clients[key].m_packet
						+ g_clients[key].m_prev_packet_size, wptr, need_size);
					ProcessPacket(key, g_clients[key].m_packet);
					g_clients[key].m_prev_packet_size = 0;
					g_clients[key].m_packet_size = 0;
					work_size -= need_size;
					wptr += need_size;
				}
				else
				{
					memcpy(g_clients[key].m_packet + g_clients[key].m_prev_packet_size, wptr, work_size);
					g_clients[key].m_prev_packet_size += work_size;
					work_size = 0;
				}
			}
			StartRecv(key);
		}
		else if (p_over->m_packetType == PT_SEND) // Send 후처리
		{
			// cout << "WT: A packet was sent to Client[" << key << "]\n\n";
			delete p_over;
		}
		else
		{
			std::cout << "Packet Type Error!\n";
		}
	}
}

void accept_thread()	// 새로 접속해 오는 클라이언트를 IOCP로 넘기는 역할
{
	SOCKET s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN bind_addr;
	ZeroMemory(&bind_addr, sizeof(bind_addr));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons(MY_SERVER_PORT);
	bind_addr.sin_addr.s_addr = INADDR_ANY;	// 0.0.0.0  아무대서나 오는 것을 다 받겠다.

	::bind(s, reinterpret_cast<sockaddr *>(&bind_addr), sizeof(bind_addr));
	listen(s, 1000);

	std::cout << "Server Ready!\n\n";

	while (true)
	{
		SOCKADDR_IN c_addr;
		ZeroMemory(&c_addr, sizeof(c_addr));
		c_addr.sin_family = AF_INET;
		c_addr.sin_port = htons(MY_SERVER_PORT);
		c_addr.sin_addr.s_addr = INADDR_ANY;	// 0.0.0.0  아무대서나 오는 것을 다 받겠다.
		int addr_size = sizeof(sockaddr);

		SOCKET cs = WSAAccept(s, reinterpret_cast<sockaddr *>(&c_addr), &addr_size, NULL, NULL);

		if (cs == INVALID_SOCKET)
		{
			ErrorDisplay("In Accept Thread: WSAAccept()");
			continue;
		}

		// cout << "New Client Connected!\n";

		int id = -1;

		for (int i = 0; i < g_clients.size(); ++i)
			if (!g_clients[i].m_isconnected)
			{
				id = i;
				break;
			}

		if (-1 == id)
		{
			std::cout << "MAX USER Exceeded\n";
			continue;
		}

		// cout << "Accept [" << id << "] Client \n\n";

		g_clients[id].m_s = cs;
		g_clients[id].Init();

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(cs), gh_iocp, id, 0);
		g_clients[id].m_isconnected = true;
		StartRecv(id);
	}
}

int main()
{
	initialize();

	// CreateWorkerThreads
	std::vector <std::thread> w_threads;
	for (int i = 0; i < 4; ++i) // 4인 이유는 쿼드코어 CPU 라서
		w_threads.push_back(std::thread{ worker_thread });

	// CreateAcceptThreads
	std::thread a_thread{ accept_thread };

	// CreateTimerThreads
	std::thread t_thread{ timer_thread };

	// Threads join
	for (auto& th : w_threads) th.join();
	a_thread.join();
	t_thread.join();
}