#ifndef SEARCH_USER_RESPONSE_MESSAGE_H
#define SEARCH_USER_RESPONSE_MESSAGE_H

#include "server_response_message.h"

#include <QList>

class Search_User_Response_Message : public Server_Response_Message
{
private:
    QList<QString> search_Results;
public:
    Search_User_Response_Message(QList<QString> search_Results) {this->search_Results = search_Results;}
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Server_Response_Message::writeToStream(stream);
        stream << Server_Response_Message::Server_Response_Message_Type::Search_User_Response_Message << search_Results;
        return stream;
    }
    static Search_User_Response_Message* readMessageFromStream(QDataStream& stream)
    {
        QList<QString> search_Results;
        stream >> search_Results;
        return new Search_User_Response_Message(search_Results);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client)
    {
        emit chatbox_Client->searchUserResultsReceived(search_Results);
    }
    virtual void print() {qDebug() << "Search_User_Response_Message: " << search_Results;}
};

#endif // SEARCH_USER_RESPONSE_MESSAGE_H
