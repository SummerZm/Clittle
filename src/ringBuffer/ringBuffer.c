/*************************************************************************
 > File Name: ./ringBuffer.c
 > Author: Leafxu
 > Created Time: 2020年01月03日 星期五 19时35分53秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX_MSG_ITEMS       3
#define MSG_JSON_FORMAT     "{\"num\":\"%d\",\"timeStamp\":\"%ul\", \"msg\":\"%s\"}\r\n"

#define BOOL	int
#define FALSE	0
#define TRUE	1

struct MsgItem
{
    unsigned long long timeStamp;   
    char* msg;
    int len;
};

// Ring Buffer - if roll back add 1000.
struct MsgEntity;
typedef BOOL (*MsgAdd)(struct MsgEntity*, const char*);
typedef int (*MsgShow)(struct MsgEntity*, char* buff, int len);
typedef void (*MsgDel)(struct MsgEntity*);

struct MsgEntity
{
    int cursor;   
    struct MsgItem items[MAX_MSG_ITEMS];
    MsgAdd add;
    MsgDel clear;
    MsgShow toString;
};

struct MsgManage
{
    struct MsgEntity actionInRepo;
    struct MsgEntity actionOutRepo;
};

// MsgManager->actionInLog->items[pos]
static struct MsgManage* msgManager;

// MsgManager->actionInLog->add( , str)
BOOL _msg_add(struct MsgEntity* entity, const char* data, int len)
{
    struct timespec time;
    struct MsgEntity* pEntity = entity;
    int pos = pEntity->cursor;
    //printf("total: %d\n", pos);
    char* str = (char*)malloc(len);
    memset(str, 0, len);
    if (NULL==str) return FALSE;
    while (pos>=MAX_MSG_ITEMS) {
        pos %= MAX_MSG_ITEMS;
    }

    //printf("Pos1: %d\n", pos);
    strncpy(str, data, len);
    if (pEntity->items[pos].msg) free(pEntity->items[pos].msg);
    printf("Pos3: %d\n", pos);
    pEntity->items[pos].msg = str;

    clock_gettime(CLOCK_REALTIME, &time);
    pEntity->items[pos].timeStamp = time.tv_sec;
    pEntity->items[pos].len = len;
    pEntity->cursor++;
    printf("msg_add ok: %d\n", pEntity->cursor);
    return TRUE;
}

BOOL msg_add(struct MsgEntity* entity, const char* data)
{
    if (NULL==entity || NULL==data) return FALSE;
    _msg_add(entity, data, strlen(data)+1);
    return TRUE;
}

/*
 * @ Note: Malloc inside. you should free the memory youself.
 * @ Comment: Translate msg to format string. 
 * @ Return: The length of msg string.
 */
int _msg_to_string(struct MsgEntity* entity, char** msgBuff)
{
    int index = 0;
    int msgTotalLen = 0;
    struct MsgEntity* pEntity = entity;
    int msgHeadPos = pEntity->cursor;
    const char* format = MSG_JSON_FORMAT;
    //printf("_msg_to_string\n");
    // calculate head pos.
    if (msgHeadPos<MAX_MSG_ITEMS) {
        msgHeadPos = 0;
    }
    else 
    {
        while (msgHeadPos>=MAX_MSG_ITEMS) 
        {
            msgHeadPos %= MAX_MSG_ITEMS;
        }
    }
    //printf("HEAD Pos:%d\n", msgHeadPos);
    // calculate msg tatol len.
    index = msgHeadPos;
    do
    {
        if (index >= pEntity->cursor) break;
        msgTotalLen += pEntity->items[index].len + sizeof(MSG_JSON_FORMAT);
        index = ++index % MAX_MSG_ITEMS;
    } while(index!=msgHeadPos);
    //printf("msgTotalLen :%d\n", msgTotalLen);
    // copy msg items
    if (msgTotalLen==0) return 0;
    *msgBuff = (char*)malloc(msgTotalLen+1);
    memset(*msgBuff, 0, msgTotalLen+1);
    index = msgHeadPos;
    do
    {
        if (index >= pEntity->cursor) break;
        sprintf(*msgBuff+strlen(*msgBuff), format, index, pEntity->items[index].timeStamp, pEntity->items[index].msg);
        index = ++index % MAX_MSG_ITEMS;
    } while(index!=msgHeadPos);
    msgTotalLen = strlen(*msgBuff);
    return msgTotalLen;
}

 /*
  * @ Comment: Translate msg to format string. 
  * @ Return: The length of msg string.
  */
int msg_to_string(struct MsgEntity* entity, char* buff, int len)
{
    int ret = -1;
    int msgLen = 0;
    char* msgStr = NULL;
    if (NULL==entity || NULL==buff || len<=0) return ret;
    msgLen = _msg_to_string(entity, &msgStr);
    memset(buff, 0, len);
    if (msgLen!=0 && msgLen<=len) 
    {
        strncpy(buff, msgStr, msgLen);
        ret = msgLen;
    }
    if (msgStr) free(msgStr);
    return ret;
}

void msg_clear(struct MsgEntity* entity)
{
    int cursor = 0;
    if (NULL==entity) return;
    for (cursor; cursor<MAX_MSG_ITEMS; cursor++) 
    {
        if (entity->items[cursor].msg) 
        {
            free(entity->items[cursor].msg);
            entity->items[cursor].msg = NULL;
            entity->items[cursor].len = 0;
        }
    }
    entity->cursor = 0;
}

/* Hook operation func*/
void _init_msg_entity(struct MsgEntity* entity)
{
    entity->cursor = 0;
    entity->add = msg_add;
    entity->clear = msg_clear;
    entity->toString = msg_to_string;
}

/* single mode*/
struct MsgManage* getActionMsgInstance() 
{ 
    if (NULL==msgManager) 
    {
        msgManager = (struct MsgManage*)malloc(sizeof(struct MsgManage));
        memset(msgManager, 0, sizeof(struct MsgManage));
        _init_msg_entity(&msgManager->actionInRepo);
        _init_msg_entity(&msgManager->actionOutRepo);
    }
    return msgManager; 
} 

/* single mode*/
struct MsgEntity* getActionOutMsgEntity()
{
    struct MsgManage* msgManager = getActionMsgInstance();
    return &msgManager->actionInRepo;
}

int main(int argc, char** argv)
{
    int msgLen = 0;
    char msgStr[1024]={0};
    struct MsgEntity* actionOutRepo = getActionOutMsgEntity();
    actionOutRepo->add(actionOutRepo, "aaaa");
    sleep(1);
    actionOutRepo->add(actionOutRepo, "bbbb");
    sleep(1);
    actionOutRepo->add(actionOutRepo, "cccc");
    msgLen = actionOutRepo->toString(actionOutRepo, msgStr, sizeof(msgStr));
    printf("String:%s\n", msgStr);
    actionOutRepo->clear(actionOutRepo);
    msgLen = actionOutRepo->toString(actionOutRepo, msgStr, sizeof(msgStr));
    printf("String:%s\n", msgStr);
	return 0;
}

