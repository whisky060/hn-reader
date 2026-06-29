#ifndef HNCLIENT_H
#define HMCLIENT_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <curl/websockets.h>
#include <string>

class HNClient {
    public:
        HNClient();
        ~HNClient();

        void connect();
        void close();
        
    private:
        int socket = -1;
        SSL* ssl = nullptr;    
};



#endif