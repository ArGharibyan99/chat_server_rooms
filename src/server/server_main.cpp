#include <iostream>
#include <unistd.h>    // fork, sleep
#include <sys/types.h> // pid_t
#include <cstdlib>     // exit
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "logger.h"
#include "db.h"

static std::atomic<bool> g_stop(false);
static pid_t child_pid;

// Callback called when specific child exits
void db_exit_handler(int sig, siginfo_t *info, void *ucontext) {
	LOG_DEBUG("Child %d exited with status %d", info->si_pid, info->si_status);
	exit(0);
}

// Callback called when interrupted process "CTRL C"
void interrupt_handler(int signo, siginfo_t *info, void *ucontext) {
	if(signo == SIGINT) {
		g_stop.store(true);
	}
}

// ---------------- Main server function ----------------
void run_main_server(pid_t db_pid)
{
    LOG_DEBUG("[MAIN] Main server started. PID = %d", getpid());
	int wait_count = 5;
	int status = 0;

	 const char* socket_path = "/tmp/db.sock";

    /* 1. Create socket */
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        LOG_ERROR("Unable to create socket");
        return;
    }

    /* 2. Connect to DB process */
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

	while(wait_count--) {
		status = connect(sock, (sockaddr*)&addr, sizeof(addr));
		if(status == 0)
			break;

		LOG_DEBUG("[MAIN] Waiting for DB process");
		sleep(1);
	}

	if (status != 0) {
		LOG_ERROR("Unable to connect to DB process");
		kill(child_pid, SIGTERM);
		close(sock);
		return;
	}

    LOG_INFO("Connected to DB process");

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
        LOG_ERROR("Failed to send");
        close(sock);
        return;
    }

    LOG_INFO("Request sent");

    /* 5. Receive response */
    DbResponse resp{};
    ssize_t recvd = recv(sock, &resp, sizeof(resp), MSG_WAITALL);
    if (recvd != sizeof(resp)) {
        LOG_ERROR("Failed to recv");
        close(sock);
        return;
    }

    LOG_INFO("Response received:");
    LOG_INFO("status = %d", resp.status);
	LOG_INFO("payload_len = %d", resp.data_len);
	LOG_INFO("payload = %s", resp.data);

    while (!g_stop.load())
    {
        sleep(3);
    }

	kill(child_pid, SIGTERM);
    /* 6. Cleanup */
    close(sock);

}

int main()
{
	int status;
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = db_exit_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);

    sa.sa_flags = 0;
    sa.sa_sigaction = interrupt_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    pid_t pid = fork();  // create child process

    if (pid < 0)
    {
		LOG_ERROR("Failed to create DB process!\n");
        return 1;
    }

    if (pid == 0)
    {
		child_pid = pid;
        // Child process → DB handling
        run_db_handler();
        return 0;
    }
    else
    {
        // Parent process → main server
        run_main_server(pid);
    }

	waitpid(child_pid, &status, 0);

    return 0;
}
