#include "db.h"
#include "logger.h"

std::atomic<bool> db_shutdown(false);

void print_db_table(PGconn *conn, const char *table_name) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM %s", table_name);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed for %s: %s\n", table_name, PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    int cols = PQnfields(res);

    LOG_DEBUG("Table: %s\n", table_name);
    // Print header
    for (int j = 0; j < cols; j++) {
        LOG_DEBUG("%s\t", PQfname(res, j));
    }
    LOG_DEBUG("\n");

    // Print rows
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            LOG_DEBUG("%s\t", PQgetvalue(res, i, j));
        }
        LOG_DEBUG("\n");
    }

    PQclear(res);
}

void sigterm_handler(int signo)
{
    if (signo == SIGTERM) {
        LOG_DEBUG("Received SIGTERM signal in DB process");
        db_shutdown.store(true);
    }
}

void register_signals()
{
    struct sigaction sa;
    sa.sa_handler = sigterm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;  // or SA_RESTART

    sigaction(SIGTERM, &sa, nullptr);
    LOG_DEBUG("Registered SIGTERM handler in DB process");
    sigaction(SIGINT,  &sa, nullptr); // optional Ctrl+C
    LOG_DEBUG("Registered SIGINT handler in DB process");
}

DbWorker::DbWorker(BlockingQueue<DbRequest>& req,
                   BlockingQueue<DbResponse>& resp,
                   const char* conninfo)
    : request_q_(req), response_q_(resp)
{
    conn_ = PQconnectdb(conninfo);
    if (PQstatus(conn_) != CONNECTION_OK) {
        LOG_ERROR("DB connection failed: %s", PQerrorMessage(conn_));
        exit(1);
    }
}

void DbWorker::operator()() {
    DbRequest req;
    while (request_q_.pop(req) && !db_shutdown.load()) {
        DbResponse resp{};
        resp.request_id = req.request_id;
        handle_request(req, resp);
        response_q_.push(resp);
    }
    PQfinish(conn_);
}

void DbWorker::handle_request(const DbRequest& req, DbResponse& resp) {
    if (req.cmd == DbCmd::GET_USER) {
        PGresult* r = PQexec(conn_,
            "SELECT username FROM users LIMIT 1");

        if (PQresultStatus(r) != PGRES_TUPLES_OK) {
            resp.status = -1;
            snprintf(resp.data, sizeof(resp.data),
                     "%s", PQerrorMessage(conn_));
        } else {
            resp.status = 0;
            snprintf(resp.data, sizeof(resp.data),
                     "%s", PQgetvalue(r, 0, 0));
            resp.data_len = strlen(resp.data);
        }
        PQclear(r);
    } else if (req.cmd == DbCmd::TEST) {
		resp.status = 0;
		snprintf(resp.data, sizeof(resp.data), "%s", "DB Reply");
		resp.data_len = strlen(resp.data);
	}
}

// ---------------- DB process function ----------------
void run_db_handler()
{
    LOG_DEBUG("[DB] Database handler process started. PID = %d", getpid());

    BlockingQueue<DbRequest> request_q;
    BlockingQueue<DbResponse> response_q;

    const char *host = "postgres";
    const char *port = "5432";
    const char *dbname = "chat_db";
    const char *user = "user";
    const char *password = "password";

    PGconn *conn;
    PGresult *tables; 

    char conninfo[256]; // Make sure it’s large enough

    register_signals();

    // Build the connection string
    snprintf(conninfo, sizeof(conninfo),
	"host=%s port=%s dbname=%s user=%s password=%s",
 	 host, port, dbname, user, password);

    std::vector<std::thread> workers;
    for (int i = 0; i < WORKERS_COUNT; i++) {
        workers.emplace_back(
            DbWorker(request_q, response_q, conninfo)
        );
    }

    IpcServer ipc("/tmp/db.sock", request_q, response_q);
    ipc.run();

    request_q.stop();
    for (auto& t : workers)
        t.join();
}

