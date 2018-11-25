#include "SceneManager.h"

SceneManager::SceneManager()
{
	// 화면 크기, 타일 크기 설정
	m_nWin_Width = 960;
	m_nWin_Height = 900;

	m_nTile_Width = 32;
	m_nTile_Height = 32;

	// 내 아이디 초기화
	m_Myid = -1;

	// 채팅 입력 상태 초기화
	m_bInputChat = false;
}

SceneManager::~SceneManager()
{

}

void SceneManager::ImageAlphaSetting(CImage* Img)
{
	unsigned char * pCol = 0;

	long w = Img->GetWidth();
	long h = Img->GetHeight();

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			pCol = (unsigned char *)
				Img->GetPixelAddress(x, y);

			unsigned char alpha = pCol[3];

			if (alpha != 255)
			{
				pCol[0] = (pCol[0] * alpha + 128) >> 8;
				pCol[1] = (pCol[1] * alpha + 128) >> 8;
				pCol[2] = (pCol[2] * alpha + 128) >> 8;
			}
		}
	}
}

void SceneManager::Render(HDC hViewdc)
{
	
}

int SceneManager::GetWinWidth() { return m_nWin_Width; }

int SceneManager::GetWinHeight() { return m_nWin_Height; }

void SceneManager::LoginOkey(sc_login_ok* pBuf)
{
	m_Myid = pBuf->id;
}

void SceneManager::LoginFail(sc_login_fail* pBuf)
{

}

void SceneManager::Chat(sc_chat* pBuf)
{

}

void SceneManager::ProcessKeyDown(wchar_t tChar)
{
	
}

void SceneManager::PressBackspace()
{
	
}