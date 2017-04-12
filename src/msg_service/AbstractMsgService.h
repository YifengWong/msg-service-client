/*
 * AbstractMsgService.h
 *
 *  Created on: 2017年4月8日
 *      Author: wyf
 */

#ifndef MSG_SERVICE_ABSTRACTMSGSERVICE_H_
#define MSG_SERVICE_ABSTRACTMSGSERVICE_H_

#include "Message.h"
#include "MsgManager.h"

/*
 * As an interface for Message Service.
 * */
class AbstractMsgService {
public:
    AbstractMsgService(MsgManager* msgMgr);
    virtual ~AbstractMsgService();

    /*
     * Start the message service
     * */
    virtual void startService() = 0;

    /*
     * Get next request's uuid, to tag a request
     * */
    const std::string* getNextRequestUuid();

    /*
     * Get next message with a image file.
     * */
    Message* getNextMsg(const std::string userUuid);

    /*
     * Send a message to server.
     * */
    void sendMsg(Message* msg);// include result

    /*
     * Send a result to server.
     * */
    void sendResult(std::string userUuid, std::string result);

protected:
    pthread_t recvThreadId;
    pthread_t sendThreadId;

    MsgManager* msgMgr;

};

#endif /* MSG_SERVICE_ABSTRACTMSGSERVICE_H_ */
