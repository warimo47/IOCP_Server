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
	// ��� Ÿ��
	CImage m_iTile;

	// �÷��̾�, �ٸ� ����
	CImage m_iPlayer;
	CImage m_iUser;

	// ����
	CImage m_iMonster1;
	CImage m_iMonster2;
	CImage m_iMonster3;
	CImage m_iMonster4;

	// ê�� Ȱ��ȭ ������
	CImage m_iChatIcon;

	// ȭ�� ũ��, Ÿ�� ũ��
	int m_nWin_Width, m_nWin_Height;
	int m_nTile_Width, m_nTile_Height;

	// �� ID
	int m_Myid;

	// �� ����
	short m_map[300][300];

public:
	// ä�� �Է� ����
	bool m_bInputChat;

	// �� ID ���ڿ�
	wchar_t m_id_str[ID_STR_LENGTH];

	// �� ê�� ���� �� ���ڿ�
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