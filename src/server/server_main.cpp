#include <iostream>
#include <unistd.h>    // fork, sleep
#include <sys/types.h> // pid_t
#include <csignal>     // kill
#include <cstdlib>     // exit
#include <sys/socket.h>
#include <sys/un.h>
#include "db.h"

// Callback called when specific child exits
void db_exit_handler(int sig, siginfo_t *info, void *ucontext) {
	printf("Child %d exited with status %d\n", info->si_pid, info->si_status);
	exit(0);
	// Your callback logic here
}

// ---------------- Main server function ----------------
void run_main_server(pid_t db_pid)
{
    std::cout << "[MAIN] Main server started. PID = "
              << getpid() << std::endl;

	 const char* socket_path = "/tmp/db.sock";

    /* 1. Create socket */
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Unable to create socket");
        return;
    }

    /* 2. Connect to DB process */
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return;
    }

    std::cout << "Connected to DB process\n";

    /* 3. Prepare request */
    DbRequest req{};
    req.request_id = 1;
    req.cmd = DbCmd::TEST;

    const char* msg = "Hello DB process";
    strncpy(req.payload, msg, sizeof(req.payload) - 1);
    req.payload_len = strlen(req.payload);

    /* 4. Send request */
    ssize_t sent = send(sock, &req, sizeof(req), 0);
    if (sent != sizeof(req)) {
        perror("send");
        close(sock);
        return;
    }

    std::cout << "Request sent\n";

    /* 5. Receive response */
    DbResponse resp{};
    ssize_t recvd = recv(sock, &resp, sizeof(resp), MSG_WAITALL);
    if (recvd != sizeof(resp)) {
        perror("recv");
        close(sock);
        return;
    }

    std::cout << "Response received:\n";
    std::cout << "status=" << resp.status << "\n";
    std::cout << "data=" << resp.data << "\n";

    while (true)
    {
        sleep(3);
    }
    /* 6. Cleanup */
    close(sock);

}

int main()
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = db_exit_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);

    pid_t pid = fork();  // create child process

    if (pid < 0)
    {
		std::cerr << "ERROR: Failed to create DB process!" << std::endl;
        return 1;
    }

    if (pid == 0)
    {
        // Child process → DB handling
        run_db_handler();
        return 0;
    }
    else
    {
        // Parent process → main server
        run_main_server(pid);
    }

    return 0;
}
