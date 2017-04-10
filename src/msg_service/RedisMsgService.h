/*
* RedisMsgService.h
 *
 *  Created on: 2017年4月10日
 *      Author: wyf
 */

#ifndef MSG_SERVICE_REDISMSGSERVICE_H_
#define MSG_SERVICE_REDISMSGSERVICE_H_

#include "AbstractMsgService.h"
#include <hiredis/hiredis.h>

class RedisMsgService: public AbstractMsgService {
public:
    const std::string CLIENT = "client";
    const std::string SERVER = "server";

    RedisMsgService(std::string redisServerIp, int redisServerPort, MsgManager* msgMgr);
    virtual ~RedisMsgService();
    void startService();

private:
    redisContext* conn;
    std::string redisServerIp;
    int redisServerPort;

    static redisReply* rpushTo(redisContext* conn, const char* key, int keyLen, const char* value, int valueLen);
    static redisReply* lpopFrom(redisContext* conn, const char* key, int keyLen);

    static void* recvThreadRunner(void *service_ptr);
    static void* sendThreadRunner(void *service_ptr);
};

#endif /* MSG_SERVICE_REDISMSGSERVICE_H_ */
