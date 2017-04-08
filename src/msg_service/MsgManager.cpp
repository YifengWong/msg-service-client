/*
 * MsgManager.cpp
 *
 *  Created on: 2017年3月13日
 *      Author: wyf
 */

#include "MsgManager.h"

#include <time.h>

MsgManager::MsgManager() {
	sendQueue = new std::queue<Message*>();
	recvQueueMap = new std::map<std::string, std::queue<Message*>*>();
	uuidRequestQueue = new std::queue<std::string>();
	uuidFinishedSet = new std::set<std::string>();
}

MsgManager::~MsgManager() {
	delete sendQueue;
	delete recvQueueMap;
	delete uuidRequestQueue;
	delete uuidFinishedSet;
}

void MsgManager::pushSend(Message* msg) {
	if (msg->getFlag() == Message::FLAG_RESULT) setFinished(msg->getUserUuid());

	pthread_mutex_lock(&mtxSendQueue);
	sendQueue->push(msg);
	pthread_mutex_unlock(&mtxSendQueue);

	if (msg->getFlag() == Message::FLAG_RESULT) {
		setFinished(msg->getUserUuid());
	}
}

Message* MsgManager::popSend() {
	Message* msg = NULL;
	pthread_mutex_lock(&mtxSendQueue);
	if (!sendQueue->empty()) {
		msg = sendQueue->front();
		sendQueue->pop();
	}

	pthread_mutex_unlock(&mtxSendQueue);
	return msg;
}

// TODO 解析
// 解析Msg
// 结束时如何检查/删除?
void MsgManager::pushRecv(Message* msg) {
	if (checkFinished(msg->getUserUuid())) return;

	pthread_mutex_lock(&mtxRecvQueueMap);

	// push
	std::map<std::string, std::queue<Message*>*>::iterator it =
			recvQueueMap->find(msg->getUserUuid());
	if (it == recvQueueMap->end()) {

		pthread_mutex_lock(&mtxUuidRequestQueue);
		uuidRequestQueue->push(msg->getUserUuid());
		pthread_mutex_unlock(&mtxUuidRequestQueue);

		recvQueueMap->insert(std::make_pair(msg->getUserUuid(), new std::queue<Message*>()));
		it = recvQueueMap->find(msg->getUserUuid());
	}
	it->second->push(msg);

	pthread_mutex_unlock(&mtxRecvQueueMap);

}

Message* MsgManager::popNextMessage(const std::string userUuid) {
	Message* re = NULL;
	pthread_mutex_lock(&mtxRecvQueueMap);

	std::map<std::string, std::queue<Message*>*>::iterator it =
			recvQueueMap->find(userUuid);
	if (it != recvQueueMap->end()) {
	    if (!it->second->empty()) {
            re = it->second->front();
            it->second->pop();
        }
	}


	pthread_mutex_unlock(&mtxRecvQueueMap);
	return re;
}

std::string* MsgManager::popNextRequestUuid() {
	std::string* s = NULL;

	pthread_mutex_lock(&mtxUuidRequestQueue);

	if (!uuidRequestQueue->empty()) {
		s = &(uuidRequestQueue->front());
		uuidRequestQueue->pop();
	}

	pthread_mutex_unlock(&mtxUuidRequestQueue);

	return s;
}

void MsgManager::setFinished(const std::string userUuid) {
	pthread_mutex_lock(&mtxUuidFinishedQueue);
	uuidFinishedSet->insert(userUuid);
	pthread_mutex_unlock(&mtxUuidFinishedQueue);

	pthread_mutex_lock(&mtxRecvQueueMap);

	std::map<std::string, std::queue<Message*>*>::iterator it =
			recvQueueMap->find(userUuid);
	if (it != recvQueueMap->end()) {
		while (!it->second->empty()) {
			Message* msg = it->second->front();
			delete msg;
			it->second->pop();
		}
		recvQueueMap->erase(it);
	}

	pthread_mutex_unlock(&mtxRecvQueueMap);

}

bool MsgManager::checkFinished(const std::string userUuid) {
	bool re = false;
	pthread_mutex_lock(&mtxUuidFinishedQueue);
	std::set<std::string>::iterator it = uuidFinishedSet->find(userUuid);
	if (it != uuidFinishedSet->end()) re = true;
	pthread_mutex_unlock(&mtxUuidFinishedQueue);
	return re;
}

void MsgManager::cleanSendQueue() {
	while (!sendQueue->empty()) {
		Message* msg = sendQueue->front();
		delete msg;
		sendQueue->pop();
	}
}

void MsgManager::cleanRecvQueueMap() {
	std::map<std::string, std::queue<Message*>*>::iterator it;
	for (it = recvQueueMap->begin(); it != recvQueueMap->end(); it++) {
		std::queue<Message*>* q = it->second;
		while (!q->empty()) {
			Message* msg = q->front();
			delete msg;
			q->pop();
		}
		delete q;
	}
	recvQueueMap->clear();
}

void MsgManager::cleanRequestQueue() {
	while (!uuidRequestQueue->empty()) {
		uuidRequestQueue->pop();
	}
}

void MsgManager::cleanFinishedSet() {
	uuidFinishedSet->clear();

}
