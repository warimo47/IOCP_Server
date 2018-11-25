#pragma once

#include <atlimage.h>
#include <iostream>
#include <fstream>
#include "..\IOCP_Server\protocol.h"

struct Client
{
	unsigned char objectType;
};

class SceneManager
{
	// 배경 타일
	CImage m_iTile;

	// 플레이어, 다른 유저
	CImage m_iPlayer;
	CImage m_iUser;

	// 몬스터
	CImage m_iMonster1;
	CImage m_iMonster2;
	CImage m_iMonster3;
	CImage m_iMonster4;

	// 챗팅 활성화 아이콘
	CImage m_iChatIcon;

	// 화면 크기, 타일 크기
	int m_nWin_Width, m_nWin_Height;
	int m_nTile_Width, m_nTile_Height;

	// 내 ID
	int m_Myid;

	// 맵 정보
	short m_map[300][300];

public:
	// 채팅 입력 상태
	bool m_bInputChat;

	// 내 ID 문자열
	wchar_t m_id_str[ID_STR_LENGTH];

	// 내 챗팅 저장 할 문자열
	wchar_t m_myChatStr[MAX_STR_SIZE];

public:
	SceneManager();
	~SceneManager();

	void ImageAlphaSetting(CImage*);

	void Render(HDC);

	int GetWinWidth();
	int GetWinHeight();

	void LoginOkey(sc_login_ok*);
	void LoginFail(sc_login_fail*);

	void Chat(sc_chat*);

	void ProcessKeyDown(wchar_t tChar);
	void PressBackspace();
};