#pragma once

#define MAX_BUFF_SIZE 4000
#define MAX_PACKET_SIZE 256

#define MY_SERVER_PORT 4000

#define CS_LOGIN 1
#define CS_LOGOUT 2
#define CS_CHAT 3

#define SC_LOGIN_OK 1
#define SC_LOGIN_FAIL 2
#define SC_CHAT 3

#define ID_STR_LENGTH 50

#define MAX_STR_SIZE 50

#pragma pack (push, 1)

/* Client -> Server */

struct cs_login
{
	unsigned char size;
	unsigned char type;
};

struct cs_logout
{
	unsigned char size;
	unsigned char type;
};

struct cs_chat
{
	unsigned char size;
	unsigned char type;
	wchar_t chat_str[MAX_STR_SIZE];
};

/* Server -> Client */

struct sc_login_ok
{
	unsigned char size;
	unsigned char type;
	unsigned short id;
	unsigned short x;
	unsigned short y;
	unsigned short hp;
	unsigned char level;
	unsigned long exp;
};

struct sc_login_fail
{
	unsigned char size;
	unsigned char type;
};

struct sc_chat
{
	unsigned char size;
	unsigned char type;
	unsigned short id;
	wchar_t chat_str[MAX_STR_SIZE];
};

#pragma pack (pop)