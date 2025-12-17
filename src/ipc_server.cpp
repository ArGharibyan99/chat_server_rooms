#include "ipc_server.h"
#include "logger.h"

extern std::atomic<bool> db_shutdown;

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
	LOG_INFO("IPC Server started");
}

void IpcServer::run() {
    client_fd_ = accept(server_fd_, nullptr, nullptr);
	LOG_INFO("Client %d connected", client_fd_);
    while (true && !db_shutdown.load()) {
        DbRequest req{};
        ssize_t n = recv(client_fd_, &req, sizeof(req), 0);
        if (n <= 0)
            break;

		LOG_INFO("Request received");
		LOG_INFO("Id: %d", req.request_id);
		LOG_INFO("Payload Len: %d", req.payload_len);
		LOG_INFO("Payload: %s", req.payload);
        request_q_.push(req);

        DbResponse resp{};
        response_q_.pop(resp);
        send(client_fd_, &resp, sizeof(resp), 0);
		LOG_INFO("Response sent");
    }
}

IpcServer::~IpcServer() {
	LOG_INFO("IPC Server stopped");
    if (client_fd_ >= 0) {
        close(client_fd_);
    }
	LOG_INFO("Client closed");

    if (server_fd_ >= 0) {
        close(server_fd_);
    }
	LOG_INFO("Server closed");
}
