/*
 * SocketMsgService.h
 *
 *  Created on: 2017年4月8日
 *      Author: wyf
 */

#ifndef MSG_SERVICE_SOCKETMSGSERVICE_H_
#define MSG_SERVICE_SOCKETMSGSERVICE_H_

#include "AbstractMsgService.h"

class SocketMsgService: public AbstractMsgService {
public:
    SocketMsgService(std::string ip, int port, MsgManager* msgMgr);
    virtual ~SocketMsgService();

    void startService();

private:
    std::string serverIp;
    int serverPort;
    struct sockaddr_in *serverAddr;
    int clientSocket;

    static void* recvThreadRunner(void *service_ptr);
    static void* sendThreadRunner(void *service_ptr);
};

#endif /* MSG_SERVICE_SOCKETMSGSERVICE_H_ */
