#include "db.h"

// ---------------- DB process function ----------------
void run_db_handler()
{
    std::cout << "[DB] Database handler process started. PID = "
              << getpid() << std::endl;

    while (true)
    {
        // Simulate handling DB read/write operations
        std::cout << "[DB] Handling DB tasks..." << std::endl;
        sleep(2);
    }
}

