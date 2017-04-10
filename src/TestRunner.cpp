/*
 * TestRunner.cpp
 *
 *  Created on: 2017年4月8日
 *      Author: wyf
 */
#include <iostream>
#include <string>
#include <fstream>

#include "msg_service/Message.h"
#include "msg_service/MsgManager.h"
#include "msg_service/SocketMsgService.h"
#include "msg_service/AbstractMsgService.h"
#include "msg_service/RedisMsgService.h"

#include <stdio.h>
#include <hiredis/hiredis.h>

using namespace std;

redisReply * aRedisCommand(redisContext* conn, char* command, int comLen,
                           char* key, int keyLen, char* bytes, int byteLen) {
    int argc = 3;
    const char *argv[] = {command, key, bytes};
    const size_t argvlen[] = {comLen, keyLen, byteLen};

    return (redisReply *)redisCommandArgv(conn, argc, argv, argvlen);
}

redisReply* lpopFrom(redisContext* conn, char* key, int keyLen) {
    int argc = 2;
    const char *argv[] = {"LPOP", key};
    const size_t argvlen[] = {4, keyLen};

    return (redisReply *)redisCommandArgv(conn, argc, argv, argvlen);
}


int main() {
//    redisContext* conn = redisConnect("127.0.0.1",6379);
//        if(conn->err){
//            printf("connection error:%s\n",conn->errstr);
//        }
//
////      redisReply* reply = (redisReply*)redisCommand(conn,"set foo 1234");
////      freeReplyObject(reply);
////
////      reply = (redisReply*)redisCommand(conn,"get foo");
////      printf("%s\n",reply->str);
////      freeReplyObject(reply);
////
////      redisFree(conn);
//
//      float* params = new float[21];
//      for (int i = 0; i < 21; i++) params[i] = 10;
//      Message* msg = new Message(Message::FLAG_IMG,
//                "QWERTYUIOPASDFGHJKLZXCVBNM123456", 3, 4, "zxcv", params);
//
////      redisReply *r = aRedisCommand(conn, "LPOP", 5, "client", 6,
////                                    msg->getMsgBytes(), Message::HEAD_LENGTH+msg->getFileLength());
//
//      redisReply *r = lpopFrom(conn, "client", 6);
//      if (!r) {
//          printf("empty reply\n");
//      }
//
//      printf("reply: %s %d\n", r->str, r->len);

    AbstractMsgService* service = new RedisMsgService("127.0.0.1", 6379, new MsgManager());
    service->startService();
    cout << "start" << endl;

    float* params = new float[21];
    for (int i = 0; i < 21; i++) params[i] = 10;

//  Message* msg = new Message(Message::FLAG_IMG,
//          "QWERTYUIOPASDFGHJKLZXCVBNM123456", 3, 4, "zxcv", params);
    Message* msg = new Message(Message::FLAG_RESULT,
                "QWERTYUIOPASDFGHJKLZXCVBNM123456", 3, 4, "zxcv", NULL);

    while (1) {
        string input;
        cin >> input;
        if (input.compare("send") == 0) {

            service->sendMsg(new Message(msg));
            cout << "send finish" << endl;

        } else if (input.compare("recv") == 0) {
            Message* img = service->getNextMsg("QWERTYUIOPASDFGHJKLZXCVBNM123456");

            if (img == NULL) {
                cout << "nothing." << endl;
                continue;
            } else {
                cout << img->getFileNum();
            }
            cout << endl;
            delete img;
        } else if (input.compare("exit") == 0) {
            cout << "exit 1" << endl;
            break;
        } else if (input.compare("uuid") == 0) {
            const string* id = service->getNextRequestUuid();
            if (id == NULL) {
                cout << "nothing." << endl;
                continue;
            }
            cout << *id << endl;
        } else if (input.compare("result") == 0) {
            service->sendMsg(new Message(msg));
            cout << "send finish" << endl;
        }

    }
    cout << "exit 1" << endl;

    return 0;
}
