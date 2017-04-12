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

/*
 * A message manager, manage all messages.
 * */
class MsgManager {
public:
    MsgManager();
	virtual ~MsgManager();

    /*
     * Push the message into the send queue and it will be sent.
     * */
	void pushSend(Message* msg);

    /*
     * Get the next message about the request user-uuid.
     * */
	Message* popNextMessage(const std::string userUuid);

    /*
     * Get the next request user-uuid.
     * */
	std::string* popNextRequestUuid();

    /*
     * Set the finished status about the user-uuid, it will wait for server's check.
     * */
	void setFinished(const std::string userUuid);

private:
    /*
     * Pop the message which is waiting for being sent.
     * */
	Message* popSend();

    /*
     * Push a received message to the queue and it will be dealt.
     * */
	void pushRecv(Message* msg);

	// The message wait for sending.
	pthread_mutex_t mtxSendQueue;
	std::queue<Message*>* sendQueue;
	void cleanSendQueue();

	// The message had been received.
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
