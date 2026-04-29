#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdio.h>
#include <time.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define NAME_LEN 32

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

#define ADMIN_NAME "The Knights"
#define ADMIN_PASS "protocol7"

// ===== LOGGING =====
static inline void log_event(const char *role, const char *msg) {
    FILE *fp = fopen("history.log", "a");
    if (!fp) return;

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] %s\n",
        tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
        tm->tm_hour, tm->tm_min, tm->tm_sec,
        role, msg);

    fclose(fp);
}

#endif
