#include <iostream>
#include <sys/types.h> // pid_t
#include <unistd.h>    // fork, sleep
#include <cstring>
#include <thread>
#include "db_queue.h"
#include "ipc_server.h"

#define WORKERS_COUNT 1

extern "C" {
	#include <libpq-fe.h>
}

void run_db_handler();

class DbWorker {
public:
	DbWorker(BlockingQueue<DbRequest>& req,
             BlockingQueue<DbResponse>& resp,
             const char* conninfo);

	void operator()();

private:
	void handle_request(const DbRequest& req, DbResponse& resp);
	PGconn *conn_;

    BlockingQueue<DbRequest>& request_q_;
    BlockingQueue<DbResponse>& response_q_;	
};
