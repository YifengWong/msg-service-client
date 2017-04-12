/*
 * AbstractMsgService.cpp
 *
 *  Created on: 2017年4月8日
 *      Author: wyf
 */

#include "AbstractMsgService.h"

AbstractMsgService::AbstractMsgService(MsgManager* msgMgr) {
    this->msgMgr = msgMgr;
    recvThreadId = -1;
    sendThreadId = -1;
}

AbstractMsgService::~AbstractMsgService() {
    delete msgMgr;
}


const std::string* AbstractMsgService::getNextRequestUuid() {
    return msgMgr->popNextRequestUuid();
}

Message* AbstractMsgService::getNextMsg(const std::string userUuid) {
    return msgMgr->popNextMessage(userUuid);
}

void AbstractMsgService::sendMsg(Message* msg) {
    msgMgr->pushSend(msg);
}

void AbstractMsgService::sendResult(std::string userUuid, std::string result) {
    Message* msg = new Message(Message::FLAG_RESULT, userUuid, 0,
            result.length(), result.data(), NULL);
    sendMsg(msg);
}



