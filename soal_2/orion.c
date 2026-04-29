
#include "arena.h"

SharedData *data;
int shm_id, msg_id;

int find_idle(int ex) {
    for (int i = 0; i < data->player_count; i++) {
        if (i != ex &&
            data->players[i].active &&
            !data->players[i].in_battle) {
            return i;
        }
    }
    return -1;
}

int main() {
    shm_id = shmget(SHM_KEY, sizeof(SharedData), IPC_CREAT | 0666);
    data = shmat(shm_id, NULL, 0);

    msg_id = msgget(MSG_KEY, IPC_CREAT | 0666);

    pthread_mutex_init(&data->lock, NULL);

    printf("Orion ready...\n");

    Message msg;

    while (1) {
        msgrcv(msg_id, &msg, sizeof(msg)-sizeof(long), 1, 0);

        pthread_mutex_lock(&data->lock);

        int p1 = -1;
        for (int i = 0; i < data->player_count; i++) {
            if (strcmp(data->players[i].username, msg.username) == 0)
                p1 = i;
        }

        int p2 = find_idle(p1);

        data->players[p1].in_battle = 1;
        data->players[p1].hp = BASE_HP;

        if (p2 != -1) {
            data->players[p2].in_battle = 1;
            data->players[p2].hp = BASE_HP;

            printf("%s vs %s\n",
                data->players[p1].username,
                data->players[p2].username);
        } else {
            printf("%s vs BOT\n", data->players[p1].username);
        }

        pthread_mutex_unlock(&data->lock);
    }
}
