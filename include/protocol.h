#pragma once
#include <stdint.h>

enum class DbCmd : uint32_t {
    GET_USER = 1,
    INSERT_MESSAGE,
    GET_MESSAGES,
	TEST
};

struct DbRequest {
    uint32_t request_id;
    DbCmd cmd;
    uint32_t payload_len;
    char payload[512];
};

struct DbResponse {
    uint32_t request_id;
    int32_t status;     // 0 = OK, <0 = error
    uint32_t data_len;
    char data[1024];
};

