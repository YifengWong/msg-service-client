#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>
typedef char byte;

/*
 * The message use for communication.
 * */
class Message {
public:
    static const byte FLAG_FAIL = 0;
    static const byte FLAG_IMG = 1;
    static const byte FLAG_NEED = 2;
    static const byte FLAG_RESULT = 3;
    static const byte FLAG_HIGH = 3;

    static const int HEAD_LENGTH = 42;

    static const int PARAM_NUM = 7;
    static const int PARAM_NUM_EACH = 3;
	// All params length in byte.
    static const int PARAM_ALL_LENGTH = 4 * PARAM_NUM_EACH * PARAM_NUM;

    // The postion of each param in the params array.
    static const int LINEAR_ACCELERATION = 0;
    static const int ACCELEROMETER = 3;
    static const int GRAVITY = 6;
    static const int ORIENTATION = 9;
    static const int GYROSCOPE = 12;
    static const int MAGNETIC_FIELD = 15;
    static const int PRESSURE = 18;

    /*
     * Create a message with its head bytes in a fixed length HEAD_LENGTH
     * */
    Message(byte* headBytes);

    /*
     * Create a message with its attributes
     * */
    Message(const byte flag, const std::string userUuid, const int fileNum,
            const int fileLength, const byte * fileBytes, const float * params);

    /*
     * Copy and create a new message with an existed message
     * */
    Message(Message* msg);
    ~Message();

    /*
     * Get the flag in the message.
     * */
    byte getFlag();

    /*
     * Get the request uuid in the message.
     * */
    std::string getUserUuid();

    /*
     * Get the file num, to check the message order.
     * */
    int getFileNum();

    /*
     * Get the file length, to make it binary-safe.
     * */
    int getFileLength();

    /*
     * Get the file bytes.
     * */
    byte* getFileBytes();

    /*
     * Get the message all bytes.
     * */
    byte* getMsgBytes();

    /*
     * Get the message bytes all length, to make it binary-safe.
     * */
    int getMsgLength();


    /*
     * return the params array.
     * use such as: linear_acceleration_x
     *              - getParams[LINEAR_ACCELERATION+0]
     *              gravity_y
     *              - getParams[GRAVITY+1]
     * */
    float* getParams();

    /*
     * Write the bytes into the message file.
     * */
    void writeFileBytes(const byte* bytes, int byteLength);

    /*
     * Write the bytes into the message params.
     * */
    void writeParamsBytes(const byte* bytes, int byteLength);


private:
    byte flag;
    std::string userUuid;
    int fileNum;
    int fileLength;

    float* params;

    int fileBytesCount;
    byte* fileBytes;
};

#endif /* MESSAGE_H_ */
