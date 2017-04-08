#include "Message.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <cstdlib>


Message::Message(byte * headBytes) {

    memcpy(&this->flag, headBytes, 1);
    this->userUuid.append(headBytes + 1, headBytes + 33);
    memcpy(&this->fileNum, headBytes + 34, 4);
    memcpy(&this->fileLength, headBytes + 38, 4);
    fileBytes = NULL;
    fileBytesCount = 0;
    params = NULL;

    if (this->flag > FLAG_HIGH) {
        // construct fail
    }
}

Message::Message(const byte flag, const std::string userUuid, const int fileNum,
                 const int fileLength, const byte * fileBytes, const float * params) {
    this->flag = flag;
    this->userUuid = userUuid;
    this->fileNum = fileNum;
    this->fileLength = fileLength;

    if (flag == FLAG_IMG) {
	    this->params = new float[Message::PARAM_NUM * Message::PARAM_NUM_EACH];
		for (int i = 0; i < Message::PARAM_NUM * Message::PARAM_NUM_EACH; i++) {
			this->params[i] = params[i];
		}
    }



	this->fileBytes = new byte[fileLength];
	memcpy(this->fileBytes, fileBytes, fileLength);
    this->fileBytesCount = this->fileLength;
}

Message::Message(Message* msg) {
	this->flag = msg->flag;
	this->userUuid = msg->userUuid;
	this->fileNum = msg->fileNum;
	this->fileLength = msg->fileLength;
	this->fileBytesCount = msg->fileBytesCount;
	this->params = NULL;
	this->fileBytes = NULL;

	if (msg->params != NULL) {
		this->params = new float[PARAM_NUM * PARAM_NUM_EACH];
		for (int i = 0; i < PARAM_NUM * PARAM_NUM_EACH; i++)
			this->params[i] = msg->params[i];
	}

	if (msg->fileBytes != NULL) {
		this->fileBytes = new byte[fileLength];
		for (int i = 0; i < fileLength; i++)
			this->fileBytes[i] = msg->fileBytes[i];
	}
}


Message::~Message() {
    if (fileBytes != NULL) {
        delete fileBytes;
        fileBytes = NULL;
    }
    if (params != NULL) {
    	delete params;
    	params = NULL;
    }
}

byte Message::getFlag() { return flag; }

std::string Message::getUserUuid() { return userUuid; }

int Message::getFileNum() { return fileNum; }

int Message::getFileLength() { return fileLength; }

byte * Message::getFileBytes() { return fileBytes; }

float* Message::getParams() { return params; }


byte * Message::getMsgBytes() {
	int allLength = HEAD_LENGTH + fileLength;
	if (flag == FLAG_IMG) allLength += PARAM_ALL_LENGTH;

    byte* allBytes = new byte[allLength];

    memcpy(allBytes, &flag, 1);
    memcpy(allBytes + 1, userUuid.data(), 33);
    memcpy(allBytes + 34, &fileNum, 4);
    memcpy(allBytes + 38, &fileLength, 4);

    int count = 42;
    if (flag == FLAG_IMG) {
    	for (int i = 0; i < PARAM_NUM * PARAM_NUM_EACH; i++, count+=4) {
    		memcpy(allBytes + count, &params[i], 4);
    	}
    }

    memcpy(allBytes + count, fileBytes, fileLength);

    return allBytes;
}


void Message::writeParamsBytes(const byte* bytes, int byteLength) {
	if (params == NULL) params = new float[PARAM_NUM * PARAM_NUM_EACH];
	if (byteLength == PARAM_ALL_LENGTH) {
		int count = 0;
		for (int i = 0; i < PARAM_NUM * PARAM_NUM_EACH; i++, count +=4) {
			memcpy(&params[i], bytes + count, 4);
		}
	}
}

void Message::writeFileBytes(const byte * bytes, int byteLength) {
    if (fileBytes == NULL) fileBytes = new byte[this->fileLength];
    if (byteLength <= this->fileLength - fileBytesCount) {
        for (int i = 0; i < byteLength; ) {
            fileBytes[fileBytesCount++] = bytes[i++];
        }
    }
}

