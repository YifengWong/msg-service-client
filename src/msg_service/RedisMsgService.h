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

/*
 * A message service work with Redis.
 * */
class RedisMsgService: public AbstractMsgService {
public:
    const std::string CLIENT = "client";
    const std::string SERVER = "server";

    RedisMsgService(std::string redisServerIp, int redisServerPort, MsgManager* msgMgr);
    virtual ~RedisMsgService();

    /*
     * Start service, it will connect to the redis server and begin all threads.
     * */
    void startService();

private:
    redisContext* conn;
    std::string redisServerIp;
    int redisServerPort;

    static redisReply* rpushTo(redisContext* conn, const char* key, size_t keyLen, const char* value, size_t valueLen);
    static redisReply* lpopFrom(redisContext* conn, const char* key, size_t keyLen);

    static void* recvThreadRunner(void *service_ptr);
    static void* sendThreadRunner(void *service_ptr);
};

#endif /* MSG_SERVICE_REDISMSGSERVICE_H_ */
