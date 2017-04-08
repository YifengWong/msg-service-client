/*
 * QueueManager.h
 *
 *  Created on: 2017年3月13日
 *      Author: wyf
 */

#ifndef QUEUEMANAGER_H_
#define QUEUEMANAGER_H_

#include <queue>
#include <map>
#include <string>
#include <set>

#include "Message.h"

class MsgManager {
public:
    MsgManager();
	virtual ~MsgManager();

	void pushSend(Message* msg);
	Message* popSend();

	void pushRecv(Message* msg);
	Message* popNextMessage(const std::string userUuid);
	std::string* popNextRequestUuid();
	void setFinished(const std::string userUuid);

private:
	// 待发送
	pthread_mutex_t mtxSendQueue;
	std::queue<Message*>* sendQueue;
	void cleanSendQueue();

	// 已接收
	pthread_mutex_t mtxRecvQueueMap;
	pthread_mutex_t mtxUuidRequestQueue;
	pthread_mutex_t mtxUuidFinishedQueue;
	std::map<std::string, std::queue<Message*>*>* recvQueueMap;
	std::queue<std::string>* uuidRequestQueue;
	std::set<std::string>* uuidFinishedSet;
	void cleanRecvQueueMap();
	void cleanRequestQueue();
	void cleanFinishedSet();

	bool checkFinished(const std::string userUuid);
};

#endif /* QUEUEMANAGER_H_ */
