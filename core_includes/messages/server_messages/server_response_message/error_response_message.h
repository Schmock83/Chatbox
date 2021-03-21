#ifndef ERROR_RESPONSE_MESSAGE_H
#define ERROR_RESPONSE_MESSAGE_H

#include "server_response_message.h"

class Error_Response_Message : public Server_Response_Message
{
private:
    QString error_Message;
public:
    Error_Response_Message(QString error_Message) {this->error_Message = error_Message;}
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Server_Response_Message::writeToStream(stream);
        stream << Server_Response_Message::Server_Response_Message_Type::Error_Response_Message << error_Message;
        return stream;
    }
    static Error_Response_Message* readMessageFromStream(QDataStream& stream)
    {
        QString error_Message;
        stream >> error_Message;
        return new Error_Response_Message(error_Message);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client)
    {
        emit chatbox_Client->showMainWindowError(error_Message);
    }
    virtual void print() {qDebug() << "Error_Response_Message: " << error_Message;}
};

#endif // ERROR_RESPONSE_MESSAGE_H
