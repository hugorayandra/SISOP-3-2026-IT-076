#ifndef ARENA_H
#define ARENA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <time.h>

#define MAX_USER 50

#define SHM_KEY 1234
#define MSG_KEY 5678

#define BASE_HP 100
#define BASE_DMG 10

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define CYAN "\033[1;36m"
#define YELLOW "\033[1;33m"
#define RESET "\033[0m"

typedef struct {
    char username[32];
    char password[32];
    int gold;
    int level;
    int xp;
    int active;
    int in_battle;
    int hp;
    int weapon_dmg;
} Player;

typedef struct {
    Player players[MAX_USER];
    int player_count;
    pthread_mutex_t lock;
} SharedData;

typedef struct {
    long type;
    char username[32];
} Message;

#endif
