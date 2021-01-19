/******************************************************************
* Copyright(c) 2020-2028 ZHENGZHOU Tiamaes LTD.
* All right reserved. See COPYRIGHT for detailed Information.
*
* @fileName: udpserver.h
* @brief: UDP通信服务
* @author: dinglf
* @date: 2021-01-15
* @history:
*******************************************************************/
#ifndef _UDP_SERVER_H_
#define _UDP_SERVER_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <netinet/in.h>
//#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>

#include "thread_pool.h"
#include "taskprocess.h"
#include "pub.h"

using namespace std;

#define THREAD_NUM 5
class UdpServer
{
private:
	static UdpServer*  m_pUdpServer; 
	bool is_stop;   //是否停止select的标志
	int threadnum;   //线程数目
	int sockfd;     //监听的fd
	fd_set rest, west;
	unsigned short m_port; //端口
	threadpool<BaseTask> *pool;   //线程池的指针
	struct sockaddr_in ServerAddr;   //绑定的sockaddr
	class CGarbo //在析构函数中删除单例
    {
        public:
            ~CGarbo()
            {
                if(UdpServer::m_pUdpServer != NULL)
                {
					delete UdpServer::m_pUdpServer;
					UdpServer::m_pUdpServer = NULL;
				}
            }
     };
     static CGarbo Garbo; //定义一个静态成员，程序结束时，系统会自动调用它的析构函数				
public://构造函数
	UdpServer()
	{}
	UdpServer(int thread) : is_stop(false) , threadnum(thread) , sockfd(-1) , pool(NULL)
	{
	}
	~UdpServer()  //析构
	{
		is_stop = true;
		if(pool != NULL)
		{
			pool->stop();
			delete pool;
			pool = NULL;
		}
	}
	static UdpServer* GetInstance()
	{
		if (NULL == m_pUdpServer) 
		{
			m_pUdpServer = new UdpServer(THREAD_NUM);
		}
		return m_pUdpServer;		
	}
	static void Destroy()
	{
		if(m_pUdpServer != NULL)
		{
			delete m_pUdpServer;
			m_pUdpServer = NULL;
		}
	}	
	bool init(unsigned short svrport,pudpFun callback);
	void selectloop();
    bool startpool();
	void stoppool();
	int  senddata(const char* ip,unsigned short port,const char *data,int datalen);

	static int setnonblocking(int fd)  //将fd设置称非阻塞
	{
		int old_option = fcntl(fd, F_GETFL);
		int new_option = old_option | O_NONBLOCK;
		fcntl(fd, F_SETFL, new_option);
		return old_option;
	}
 
};
#endif