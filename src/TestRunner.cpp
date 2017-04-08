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

using namespace std;

int main() {
    AbstractMsgService* service = new SocketMsgService("127.0.0.1", 9091, new MsgManager());
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
