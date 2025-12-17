#pragma once
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <atomic>
#include "db_queue.h"
#include "protocol.h"

class IpcServer {
public:
    IpcServer(const char* path,
              BlockingQueue<DbRequest>& req,
              BlockingQueue<DbResponse>& resp);

    ~IpcServer();   // <-- REQUIRED

    void run();

private:
    int server_fd_;
    int client_fd_;

    BlockingQueue<DbRequest>& request_q_;
    BlockingQueue<DbResponse>& response_q_;
};

