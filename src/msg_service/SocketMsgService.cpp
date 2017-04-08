/*
 * SocketMsgService.cpp
 *
 *  Created on: 2017年4月8日
 *      Author: wyf
 */

#include "SocketMsgService.h"

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>

#include <unistd.h>
#include <pthread.h>

#include <sys/time.h>

SocketMsgService::SocketMsgService(std::string ip, int port, MsgManager* msgMgr):AbstractMsgService(msgMgr) {

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverIp = ip;
    serverPort = port;
    serverAddr = new struct sockaddr_in();

    memset(serverAddr, 0, sizeof(struct sockaddr_in));
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_port = htons(serverPort);
    serverAddr->sin_addr.s_addr = inet_addr(serverIp.data());
}

SocketMsgService::~SocketMsgService() {
    close(clientSocket);
}

 void SocketMsgService::startService() {
     if (connect(clientSocket, (struct sockaddr *)serverAddr, sizeof(struct sockaddr)) < 0) {
         std::cout << serverAddr->sin_port << std::endl;
         perror("connect");
         exit(1);
     }

     int recvThreadRe = pthread_create(&recvThreadId, NULL, &recvThreadRunner, this);
     int sendThreadRe = pthread_create(&sendThreadId, NULL, &sendThreadRunner, this);

     if (recvThreadRe || sendThreadRe) {
         printf("Create thread error!\n");
         return;
     }
 }

void* SocketMsgService::recvThreadRunner(void *service_ptr) {
    pthread_detach(pthread_self());
    SocketMsgService *client = (SocketMsgService *)service_ptr;
    while (1) {
        // recv a head
        int headCount = 0;
        byte* headBytes = new byte[Message::HEAD_LENGTH];
        while (headCount != Message::HEAD_LENGTH) {
            headCount += recv(client->clientSocket, headBytes + headCount,
                    Message::HEAD_LENGTH - headCount, 0);
        }
        Message* msg = new Message(headBytes);
        delete headBytes;
        // TODO read error

        // recv the params
        if (msg->getFlag() == Message::FLAG_IMG) {
            int paramCount = 0;
            byte* paramBytes = new byte[Message::PARAM_ALL_LENGTH];
            while (paramCount != Message::PARAM_ALL_LENGTH) {
                paramCount += recv(client->clientSocket, paramBytes + paramCount,
                        Message::PARAM_ALL_LENGTH - paramCount, 0);
            }
            msg->writeParamsBytes(paramBytes, paramCount);
            delete paramBytes;
        }

        // recv the file
        byte* fileBytes = new byte[msg->getFileLength()];
        int fileCount = 0;
        while (fileCount != msg->getFileLength()) {
            fileCount += recv(client->clientSocket, fileBytes + fileCount,
                    msg->getFileLength() - fileCount, 0);
        }
        msg->writeFileBytes(fileBytes, fileCount);
        delete fileBytes;

        client->msgMgr->pushRecv(msg);

        struct timeval tv;
        gettimeofday(&tv,NULL);
        long time = tv.tv_sec * 1000 + tv.tv_usec / 1000;

        printf("%d,%ld\n", msg->getFileNum(), time);
    }

    return 0;
}


void* SocketMsgService::sendThreadRunner(void *service_ptr) {
    pthread_detach(pthread_self());
    SocketMsgService *client = (SocketMsgService *)service_ptr;

    while (1) {
        Message* msg = client->msgMgr->popSend();
        if (msg == NULL) {
            sleep(1);
            continue;
        }
        // TODO send error
        int count = 0;
        int allLength = msg->getFileLength() + msg->HEAD_LENGTH;
        if (msg->getFlag() == Message::FLAG_IMG) allLength += Message::PARAM_ALL_LENGTH;
        byte* allBytes = msg->getMsgBytes();

        while (count != allLength) {
            count += send(client->clientSocket, allBytes + count, allLength - count, 0);
        }

        delete msg;
        delete allBytes;
    }
    return 0;
}
