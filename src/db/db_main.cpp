#include "db.h"

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

    printf("Table: %s\n", table_name);
    // Print header
    for (int j = 0; j < cols; j++) {
        printf("%s\t", PQfname(res, j));
    }
    printf("\n");

    // Print rows
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%s\t", PQgetvalue(res, i, j));
        }
        printf("\n");
    }

    PQclear(res);
}

DbWorker::DbWorker(BlockingQueue<DbRequest>& req,
                   BlockingQueue<DbResponse>& resp,
                   const char* conninfo)
    : request_q_(req), response_q_(resp)
{
    conn_ = PQconnectdb(conninfo);
    if (PQstatus(conn_) != CONNECTION_OK) {
        std::cerr << "DB connection failed: "
                  << PQerrorMessage(conn_) << std::endl;
        exit(1);
    }
}

void DbWorker::operator()() {
    DbRequest req;
    while (request_q_.pop(req)) {
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
		snprintf(resp.data, sizeof(resp.data), "DB received: %s", req.payload);
		resp.data_len = strlen(resp.data);
	}
}

// ---------------- DB process function ----------------
void run_db_handler()
{
    std::cout << "[DB] Database handler process started. PID = "
              << getpid() << std::endl;

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

	// Build the connection string
	snprintf(conninfo, sizeof(conninfo),
			 "host=%s port=%s dbname=%s user=%s password=%s",
			 host, port, dbname, user, password);

	const int WORKERS = WORKERS_COUNT;
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

