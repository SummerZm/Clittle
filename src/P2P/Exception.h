/* �쳣��
 *
 * �ļ�����Exception.h
 *
 * ���ڣ�2004.5.5
 *
 * ���ߣ�shootingstars(zhouhuis22@sina.com)
 */

#ifndef __HZH_Exception__
#define __HZH_Exception__

#define EXCEPTION_MESSAGE_MAXLEN 256
#include "string.h"

class Exception
{
private:
	char m_ExceptionMessage[EXCEPTION_MESSAGE_MAXLEN];
public:
	Exception(char *msg)
	{
		strncpy(m_ExceptionMessage, msg, EXCEPTION_MESSAGE_MAXLEN);
	}

	char *GetMessage()
	{
		return m_ExceptionMessage;
	}
};



#endif