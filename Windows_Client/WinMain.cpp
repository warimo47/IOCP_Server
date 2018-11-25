#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")
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

// 영어/한글 키보드 입력 상태
bool englishKoreanState = true;
bool englishLittleBig = true;

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

	std::cout << "아이디를 입력하세요 : ";

	std::wcin >> g_cSceneManager->m_id_str;

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
	wcscpy_s(my_packet->id_str, g_cSceneManager->m_id_str);
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
	wcscpy_s(my_packet.chat_str, g_cSceneManager->m_myChatStr);
	memcpy_s(send_wsabuf.buf, sizeof(cs_chat), &my_packet, sizeof(cs_chat));
	send_wsabuf.len = sizeof(cs_chat);

	DWORD iobyte;
	int ret = WSASend(g_sock, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);

	if (ret)
	{
		int error_code = WSAGetLastError();
		printf("Error while sending packet [%d]", error_code);
	}

	g_cSceneManager->m_bInputChat = !(g_cSceneManager->m_bInputChat);
	ZeroMemory(g_cSceneManager->m_myChatStr, lstrlen(g_cSceneManager->m_myChatStr));
}

void ProcessKeyDown(WPARAM wParam)
{
	// 채팅 중 일때
	if (g_cSceneManager->m_bInputChat)
	{
		wchar_t character;
		bool canInputChar = false;

		switch (wParam)
		{
		case VK_RETURN:
		{
			if (lstrlen(g_cSceneManager->m_myChatStr) == 0)
			{
				g_cSceneManager->m_bInputChat = !(g_cSceneManager->m_bInputChat);
				return;
			}

			SendChatPacket();
		}
		break;
		case VK_BACK:
			g_cSceneManager->PressBackspace();
			break;
		case VK_CAPITAL:
			englishLittleBig = !englishLittleBig;
			break;
		case VK_SHIFT:
			englishLittleBig = !englishLittleBig;
			break;
		case 'A': character = L'A'; canInputChar = true; break;
		case 'B': character = L'B'; canInputChar = true; break;
		case 'C': character = L'C'; canInputChar = true; break;
		case 'D': character = L'D'; canInputChar = true; break;
		case 'E': character = L'E'; canInputChar = true; break;
		case 'F': character = L'F'; canInputChar = true; break;
		case 'G': character = L'G'; canInputChar = true; break;
		case 'H': character = L'H'; canInputChar = true; break;
		case 'I': character = L'I'; canInputChar = true; break;
		case 'J': character = L'J'; canInputChar = true; break;
		case 'K': character = L'K'; canInputChar = true; break;
		case 'L': character = L'L'; canInputChar = true; break;
		case 'M': character = L'M'; canInputChar = true; break;
		case 'N': character = L'N'; canInputChar = true; break;
		case 'O': character = L'O'; canInputChar = true; break;
		case 'P': character = L'P'; canInputChar = true; break;
		case 'Q': character = L'Q'; canInputChar = true; break;
		case 'R': character = L'R'; canInputChar = true; break;
		case 'S': character = L'S'; canInputChar = true; break;
		case 'T': character = L'T'; canInputChar = true; break;
		case 'U': character = L'U'; canInputChar = true; break;
		case 'V': character = L'V'; canInputChar = true; break;
		case 'W': character = L'W'; canInputChar = true; break;
		case 'X': character = L'X'; canInputChar = true; break;
		case 'Y': character = L'Y'; canInputChar = true; break;
		case 'Z': character = L'Z'; canInputChar = true; break;
		case VK_NUMPAD0: case '0': character = L'0'; canInputChar = true; break;
		case VK_NUMPAD1: case '1': character = L'1'; canInputChar = true; break;
		case VK_NUMPAD2: case '2': character = L'2'; canInputChar = true; break;
		case VK_NUMPAD3: case '3': character = L'3'; canInputChar = true; break;
		case VK_NUMPAD4: case '4': character = L'4'; canInputChar = true; break;
		case VK_NUMPAD5: case '5': character = L'5'; canInputChar = true; break;
		case VK_NUMPAD6: case '6': character = L'6'; canInputChar = true; break;
		case VK_NUMPAD7: case '7': character = L'7'; canInputChar = true; break;
		case VK_NUMPAD8: case '8': character = L'8'; canInputChar = true; break;
		case VK_NUMPAD9: case '9': character = L'9'; canInputChar = true; break;
		default:
			break;
		}

		if (canInputChar)
		{
			if (englishLittleBig && (L'A' <= character && character <= L'Z')) character += 32;
			g_cSceneManager->ProcessKeyDown(character);
		}

		if (lstrlen(g_cSceneManager->m_myChatStr) == MAX_STR_SIZE - 1)
		{
			SendChatPacket();
		}

	}
	// 채팅 중이 아닐때
	else
	{
		int dir = -1;
		bool canSendPacket = true;
		unsigned char sendPacketType;

		switch (wParam)
		{
		case VK_RETURN:
			g_cSceneManager->m_bInputChat = !(g_cSceneManager->m_bInputChat);
			canSendPacket = false;
			break;
		}
	}
}

void ProcessKeyUp(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_SHIFT:
		englishLittleBig = !englishLittleBig;
		break;
	}
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