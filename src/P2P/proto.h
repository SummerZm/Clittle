/* P2P ������Э��
 * 
 * ���ڣ�2004-5-21
 *
 * ���ߣ�shootingstars(zhouhuis22@sina.com)
 *
 */

#pragma once
#include <list>

// ����iMessageType��ֵ
#define LOGIN 1
#define LOGOUT 2
#define P2PTRANS 3
#define GETALLUSER  4

// �������˿�
#define SERVER_PORT 2280

// Client��¼ʱ����������͵���Ϣ
struct stLoginMessage
{
	char userName[10];
	char password[10];
};

// Clientע��ʱ���͵���Ϣ
struct stLogoutMessage
{
	char userName[10];
};

// Client���������������һ��Client(userName)���Լ�������UDP����Ϣ
struct stP2PTranslate
{
	char userName[10];
};

// Client����������͵���Ϣ��ʽ
struct stMessage
{
	int iMessageType;
	union _message
	{
		stLoginMessage loginmember;
		stLogoutMessage logoutmember;
		stP2PTranslate translatemessage;
	}message;
};

// �ͻ��ڵ���Ϣ
struct stUserListNode
{
	char userName[10];
	unsigned int ip;
	unsigned short port;
};

// Server��Client���͵���Ϣ
struct stServerToClient
{
	int iMessageType;
	union _message
	{
		stUserListNode user;
	}message;

};

//======================================
// �����Э�����ڿͻ���֮���ͨ��
//======================================
#define P2PMESSAGE 100               // ������Ϣ
#define P2PMESSAGEACK 101            // �յ���Ϣ��Ӧ��
#define P2PSOMEONEWANTTOCALLYOU 102  // ��������ͻ��˷��͵���Ϣ
                                     // ϣ���˿ͻ��˷���һ��UDP�򶴰�
#define P2PTRASH        103          // �ͻ��˷��͵Ĵ򶴰������ն�Ӧ�ú��Դ���Ϣ

// �ͻ���֮�䷢����Ϣ��ʽ
struct stP2PMessage
{
	int iMessageType;
	int iStringLen;         // or IP address
	unsigned short Port; 
};

using namespace std;
typedef list<stUserListNode *> UserList;

