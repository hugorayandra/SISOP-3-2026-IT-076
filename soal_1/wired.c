#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

int clients[MAX_CLIENTS];
char names[MAX_CLIENTS][NAME_LEN];
int is_admin[MAX_CLIENTS];
time_t start_time;

// cek username unik
int name_exists(const char *name) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != 0 && strcmp(names[i], name) == 0)
            return 1;
    }
    return 0;
}

int main() {
    int server_fd, new_socket, max_sd;
    struct sockaddr_in address;
    fd_set readfds;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i] = 0;
        is_admin[i] = 0;
    }

    start_time = time(NULL);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(SERVER_IP);
    address.sin_port = htons(SERVER_PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);

    printf("SERVER RUNNING on %s:%d\n", SERVER_IP, SERVER_PORT);
    log_event("System", "SERVER STARTED");

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] > 0)
                FD_SET(clients[i], &readfds);
            if (clients[i] > max_sd)
                max_sd = clients[i];
        }

        select(max_sd + 1, &readfds, NULL, NULL, NULL);

        // ===== NEW CONNECTION =====
        if (FD_ISSET(server_fd, &readfds)) {
            new_socket = accept(server_fd, NULL, NULL);

            char name[NAME_LEN] = {0};
            recv(new_socket, name, NAME_LEN, 0);

            if (name_exists(name)) {
                send(new_socket, "NAME_EXISTS", 11, 0);
                close(new_socket);
                continue;
            }

            int idx = -1;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] == 0) {
                    idx = i;
                    break;
                }
            }

            if (idx == -1) {
                close(new_socket);
                continue;
            }

            clients[idx] = new_socket;
            strcpy(names[idx], name);

            // ===== ADMIN LOGIN =====
            if (strcmp(name, ADMIN_NAME) == 0) {
                send(new_socket, "Enter Password: ", 17, 0);

                char pass[NAME_LEN] = {0};
                recv(new_socket, pass, NAME_LEN, 0);

                if (strcmp(pass, ADMIN_PASS) == 0) {
                    is_admin[idx] = 1;
                    send(new_socket, "AUTH SUCCESS\n", 13, 0);
                    printf("ADMIN CONNECTED\n");
                    log_event("Admin", "LOGIN SUCCESS");
                } else {
                    send(new_socket, "AUTH FAILED\n", 12, 0);
                    close(new_socket);
                    clients[idx] = 0;
                    continue;
                }
            }

            printf("User %s connected\n", name);

            char logbuf[128];
            snprintf(logbuf, sizeof(logbuf), "User '%s' connected", name);
            log_event("System", logbuf);
        }

        // ===== HANDLE CLIENT =====
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = clients[i];
            if (sd <= 0) continue;

            if (FD_ISSET(sd, &readfds)) {
                char buffer[BUFFER_SIZE] = {0};
                int valread = recv(sd, buffer, BUFFER_SIZE - 1, 0);

                if (valread <= 0) {
                    close(sd);
                    clients[i] = 0;
                    continue;
                }

                buffer[valread] = '\0';

                // ===== EXIT =====
                if (strcmp(buffer, "/exit") == 0) {
                    printf("User %s disconnected\n", names[i]);

                    char logbuf[128];
                    snprintf(logbuf, sizeof(logbuf),
                             "User '%s' disconnected", names[i]);
                    log_event("System", logbuf);

                    close(sd);
                    clients[i] = 0;
                    continue;
                }

                // ===== ADMIN COMMAND =====
                if (is_admin[i]) {
                    if (strcmp(buffer, "1") == 0) {
                        char msg[512] = "Active Users:\n";

                        for (int j = 0; j < MAX_CLIENTS; j++) {
                            if (clients[j] != 0 && !is_admin[j]) {
                                strcat(msg, names[j]);
                                strcat(msg, "\n");
                            }
                        }
                        send(sd, msg, strlen(msg), 0);
                    }
                    else if (strcmp(buffer, "2") == 0) {
                        char msg[128];
                        sprintf(msg, "Uptime: %ld sec\n",
                                time(NULL) - start_time);
                        send(sd, msg, strlen(msg), 0);
                    }
                    else if (strcmp(buffer, "3") == 0) {
                        send(sd, "SERVER SHUTDOWN\n", 17, 0);
                        log_event("Admin", "SERVER SHUTDOWN");
                        exit(0);
                    }
                    continue;
                }

                // ===== CHAT =====
                char msg[BUFFER_SIZE];
                snprintf(msg, sizeof(msg),
                         "[%s]: %.900s",
                         names[i], buffer);

                for (int j = 0; j < MAX_CLIENTS; j++) {
                    if (clients[j] != 0 && clients[j] != sd) {
                        send(clients[j], msg, strlen(msg), 0);
                    }
                }

                printf("%s\n", msg);
                log_event("User", msg);
            }
        }
    }
}
