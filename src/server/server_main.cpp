#include <iostream>
#include <unistd.h>    // fork, sleep
#include <sys/types.h> // pid_t
#include <csignal>     // kill
#include <cstdlib>     // exit
#include "db.h"

// ---------------- Main server function ----------------
void run_main_server(pid_t db_pid)
{
    std::cout << "[MAIN] Main server started. PID = "
              << getpid() << std::endl;

    while (true)
    {
        std::cout << "[MAIN] Server running..." << std::endl;
        sleep(3);
    }
}

int main()
{
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
