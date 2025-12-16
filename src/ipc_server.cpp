#include "ipc_server.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

IpcServer::IpcServer(const char* path,
                     BlockingQueue<DbRequest>& req,
                     BlockingQueue<DbResponse>& resp)
    : request_q_(req), response_q_(resp)
{
    server_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    unlink(path);
    bind(server_fd_, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd_, 1);
	std::cout << "IPC Server started" << std::endl;
}

void IpcServer::run() {
    client_fd_ = accept(server_fd_, nullptr, nullptr);
	std::cout << "Client "<< client_fd_ << " connected"  << std::endl;
    while (true) {
        DbRequest req{};
        ssize_t n = recv(client_fd_, &req, sizeof(req), 0);
        if (n <= 0)
            break;

		std::cout << "Request received" << std::endl;
		std::cout << "Id: " << req.request_id << std::endl;
		std::cout << "Payload Len: " << req.payload_len << std::endl;
		std::cout << "Payload: " << req.payload << std::endl;
        request_q_.push(req);

        DbResponse resp{};
        response_q_.pop(resp);
        send(client_fd_, &resp, sizeof(resp), 0);
		std::cout << "Response sent" << std::endl;
    }
}

IpcServer::~IpcServer() {
    if (client_fd_ >= 0) {
        close(client_fd_);
    }

    if (server_fd_ >= 0) {
        close(server_fd_);
    }
}
