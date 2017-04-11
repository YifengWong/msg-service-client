/*
 * RedisMsgService.cpp
 *
 *  Created on: 2017年4月10日
 *      Author: wyf
 */

#include "RedisMsgService.h"
#include <unistd.h>

RedisMsgService::RedisMsgService(std::string redisServerIp, int redisServerPort, MsgManager* msgMgr):AbstractMsgService(msgMgr) {
    this->redisServerIp = redisServerIp;
    this->redisServerPort = redisServerPort;
    this->conn = NULL;
}

RedisMsgService::~RedisMsgService() {
    redisFree(conn);
}

void RedisMsgService::startService() {
    this->conn = redisConnect(redisServerIp.c_str(), redisServerPort);
    if(this->conn->err){
        printf("connection error:%s\n",conn->errstr);
    }

    int recvThreadRe = pthread_create(&recvThreadId, NULL, &recvThreadRunner, this);
    int sendThreadRe = pthread_create(&sendThreadId, NULL, &sendThreadRunner, this);

    if (recvThreadRe || sendThreadRe) {
        printf("Create thread error!\n");
        return;
    }
}

redisReply* RedisMsgService::rpushTo(redisContext* conn, const char* key, int keyLen, const char* value, int valueLen) {
    int argc = 3;
    const char *argv[] = {"RPUSH", key, value};
    const size_t argvlen[] = {5, keyLen, valueLen};

    return (redisReply *)redisCommandArgv(conn, argc, argv, argvlen);
}

redisReply* RedisMsgService::lpopFrom(redisContext* conn, const char* key, int keyLen) {
    int argc = 2;
    const char *argv[] = {"LPOP", key};
    const size_t argvlen[] = {4, keyLen};

    return (redisReply *)redisCommandArgv(conn, argc, argv, argvlen);
}

void* RedisMsgService::recvThreadRunner(void *service_ptr)  {
    pthread_detach(pthread_self());
    RedisMsgService *client = (RedisMsgService *)service_ptr;
    while (1) {
        redisReply *r = RedisMsgService::lpopFrom(client->conn, client->CLIENT.c_str(), 6);
        if (!r || r->len == 0) {
            usleep(5);
            freeReplyObject(r);
            continue;
        }

        // recv a head
        int nowCount = 0;

        Message* msg = new Message(r->str);
        nowCount += Message::HEAD_LENGTH;
        // TODO read error

        // recv the params
        if (msg->getFlag() == Message::FLAG_IMG) {
            msg->writeParamsBytes(r->str+nowCount, Message::PARAM_ALL_LENGTH);
            nowCount += Message::PARAM_ALL_LENGTH;
        }

        // recv the file
        msg->writeFileBytes(r->str+nowCount, msg->getFileLength());
        client->msgMgr->pushRecv(msg);

        struct timeval tv;
        gettimeofday(&tv,NULL);
        long time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        printf("%d,%ld\n", msg->getFileNum(), time);
    }

    return 0;
}

void* RedisMsgService::sendThreadRunner(void *service_ptr) {
    pthread_detach(pthread_self());
    RedisMsgService *client = (RedisMsgService *)service_ptr;

    while (1) {
        Message* msg = client->msgMgr->popSend();
        if (msg == NULL) {
            usleep(5);
            continue;
        }

        redisReply *r = RedisMsgService::rpushTo(client->conn, client->SERVER.c_str(), 6,
                                                 msg->getMsgBytes(), Message::HEAD_LENGTH+msg->getFileLength());
        delete msg;
    }
    return 0;
}
