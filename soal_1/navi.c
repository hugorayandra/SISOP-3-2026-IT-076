#include "protocol.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

int sock;

// THREAD RECEIVE
void *receive_msg(void *arg) {
    char msg[BUFFER_SIZE];

    while (1) {
        int valread = recv(sock, msg, BUFFER_SIZE, 0);
        if (valread > 0) {
            msg[valread] = '\0';
            printf("\n%s\n", msg);
            fflush(stdout);
        }
    }
}

// THREAD SEND
void *send_msg(void *arg) {
    char msg[BUFFER_SIZE];

    while (1) {
        fgets(msg, BUFFER_SIZE, stdin);
        msg[strcspn(msg, "\n")] = 0;

        send(sock, msg, strlen(msg), 0);

        if (strcmp(msg, "/exit") == 0) {
            printf("Disconnecting...\n");
            exit(0);
        }
    }
}

int main() {
    struct sockaddr_in serv_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("CONNECT ERROR");
        return 1;
    }

    printf("CONNECTED TO SERVER\n");

    char name[NAME_LEN];
    printf("Enter your name: ");
    fgets(name, NAME_LEN, stdin);
    name[strcspn(name, "\n")] = 0;

    send(sock, name, NAME_LEN, 0);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, receive_msg, NULL);
    pthread_create(&t2, NULL, send_msg, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    close(sock);
}
