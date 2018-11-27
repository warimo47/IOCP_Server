// #pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")
#pragma comment(lib,"ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <iostream>
#include <locale.h>

#include "SceneManager.h"

#define WINDOW_CLASS_NAME L"Client"

#define WM_SOCKET 0xFF00
#define WIN_HEIGHT 900
#define WIN_WIDTH 960

// 윈도우 핸들
HWND g_hWnd;
HINSTANCE g_hInst;

// 씬 매니저
SceneManager* g_cSceneManager;

// 소켓
SOCKET g_sock;

// send 버퍼
WSABUF send_wsabuf;
char send_buffer[MAX_PACKET_SIZE];
int sendBytes;

// recv 버퍼
WSABUF recv_wsabuf;
char recv_buffer[MAX_PACKET_SIZE];
char packet_buffer[MAX_PACKET_SIZE];
DWORD in_packet_size;
int saved_packet_size;

// 내 클라이언트 정보
int g_myid;

int Game_Init()
{
	// 한글 사용
	std::wcout.imbue(std::locale("korean"));

	g_cSceneManager = new SceneManager();

	// 윈속 초기화
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) return 1;

	// WSASocket()
	g_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	// connect()
	SOCKADDR_IN sAddr;
	ZeroMemory(&sAddr, sizeof(sAddr));
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(MY_SERVER_PORT);
	sAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	WSAConnect(g_sock, (SOCKADDR*)&sAddr, sizeof(sAddr), NULL, NULL, NULL, NULL);

	// WSAAsyncSelect()
	WSAAsyncSelect(g_sock, g_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = MAX_PACKET_SIZE;

	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = MAX_PACKET_SIZE;
	in_packet_size = 0;
	saved_packet_size = 0;

	cs_login *my_packet = reinterpret_cast<cs_login *>(send_buffer);
	my_packet->size = sizeof(cs_login);
	my_packet->type = CS_LOGIN;
	send_wsabuf.len = sizeof(cs_login);
	DWORD iobyte;

	int ret = WSASend(g_sock, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);

	if (ret)
	{
		int error_code = WSAGetLastError();
		printf("Error while sending packet [%d]", error_code);
	}

	return 0;
}

void Game_Shutdown()
{
	// 윈속 종료
	WSACleanup();
}

void SendChatPacket()
{
	cs_chat my_packet;
	my_packet.size = sizeof(cs_chat);
	my_packet.type = CS_CHAT;
	wcscpy_s(my_packet.chat_str, g_cSceneManager->GetMyCharStr());
	memcpy_s(send_wsabuf.buf, sizeof(cs_chat), &my_packet, sizeof(cs_chat));
	send_wsabuf.len = sizeof(cs_chat);

	DWORD iobyte;
	int ret = WSASend(g_sock, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);

	if (ret)
	{
		int error_code = WSAGetLastError();
		printf("Error while sending packet [%d]", error_code);
	}

	ZeroMemory(g_cSceneManager->GetMyCharStr(), lstrlen(g_cSceneManager->GetMyCharStr()));
}

void ProcessKeyDown(WPARAM wParam)
{
	g_cSceneManager->ProcessKeyDown(wParam);
}

void ProcessKeyUp(WPARAM wParam)
{
	g_cSceneManager->ProcessKeyUp(wParam);
}

void ProcessPacket(char* pBuf)
{
	switch (pBuf[1])
	{
	case SC_LOGIN_OK:
		g_cSceneManager->LoginOkey(reinterpret_cast<sc_login_ok*>(pBuf));
		break;
	case SC_LOGIN_FAIL:
		g_cSceneManager->LoginFail(reinterpret_cast<sc_login_fail*>(pBuf));
		break;
	case SC_CHAT:
		g_cSceneManager->Chat(reinterpret_cast<sc_chat*>(pBuf));
		break;
	default:
		break;
	}
}

void ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;

	int ret = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);

	if (ret)
	{
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (iobyte)
	{
		if (!in_packet_size) in_packet_size = ptr[0];

		int need_packet_size = static_cast<int>(in_packet_size) - saved_packet_size;

		if (static_cast<int>(iobyte) >= need_packet_size)
		{
			memcpy(packet_buffer + saved_packet_size, ptr, need_packet_size);
			ProcessPacket(packet_buffer);
			ptr += need_packet_size;
			iobyte -= need_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else
		{
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}

void SocketProcess(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
		closesocket((SOCKET)wParam);
		exit(-1);
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:
		break;
	case FD_READ:
		ReadPacket((SOCKET)wParam);
		break;
	case FD_WRITE:
		std::cout << "FD_WRITE\n";
		break;
	case FD_OOB:
		std::cout << "FD_OOB\n";
		break;
	case FD_CLOSE:
		closesocket((SOCKET)wParam);
		exit(-1);
		break;
	}
}

// 윈도우 메시지 콜벡 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		MoveWindow(hWnd, 100, 100, WIN_WIDTH, WIN_HEIGHT, TRUE);
		break;
	case WM_GETMINMAXINFO:
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMaxTrackSize.x = WIN_WIDTH;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMaxTrackSize.y = WIN_HEIGHT;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = WIN_WIDTH;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = WIN_HEIGHT;
		break;
	case WM_KEYDOWN:
		ProcessKeyDown(wParam);
		break;
	case WM_KEYUP:
		ProcessKeyUp(wParam);
		break;
	case WM_SOCKET:
		SocketProcess(wParam, lParam);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// Double buffering
		HBITMAP hViewBit = CreateCompatibleBitmap(hdc,
			g_cSceneManager->GetWinWidth(),
			g_cSceneManager->GetWinHeight());

		HDC hViewdc = CreateCompatibleDC(hdc);
		SelectObject(hViewdc, hViewBit);

		// Draw Here
		g_cSceneManager->Render(hViewdc);

		// Double buffering
		BitBlt(hdc, 0, 0,
			g_cSceneManager->GetWinWidth(),
			g_cSceneManager->GetWinHeight(),
			hViewdc, 0, 0, SRCCOPY);

		DeleteDC(hViewdc);
		DeleteObject(hViewBit);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:


		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 윈도우 클래스 등록
	WNDCLASS cwnd;
	cwnd.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	cwnd.lpfnWndProc = WndProc;
	cwnd.cbClsExtra = 0;
	cwnd.cbWndExtra = 0;
	cwnd.hInstance = hInstance;
	cwnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	cwnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	cwnd.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	cwnd.lpszMenuName = NULL;
	cwnd.lpszClassName = WINDOW_CLASS_NAME;
	if (!RegisterClass(&cwnd)) return 1;

	// 윈도우 생성
	g_hWnd = CreateWindow(WINDOW_CLASS_NAME, L"Client",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0,
		WIN_WIDTH + 16, WIN_HEIGHT + 39,
		NULL, NULL, hInstance, NULL);

	if (g_hWnd == NULL) return 1;

	g_hInst = hInstance;

	Game_Init();

	// 메시지 루프
	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// test if this is a quit
			if (msg.message == WM_QUIT) break;

			// translate any accelerator keys
			TranslateMessage(&msg);

			// send the message to the window proc
			DispatchMessage(&msg);
		} // end if

		  // main game processing goes here
		InvalidateRect(g_hWnd, NULL, false);
	}

	Game_Shutdown();

	return static_cast<int>(msg.wParam);
}