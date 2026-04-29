#include "arena.h"

SharedData *data;
int shm_id, msg_id;

int me = -1;
int enemy = -1;
time_t last_attack = 0;

// ===== FIND USER =====
int find_user(char *u) {
    for (int i = 0; i < data->player_count; i++) {
        if (strcmp(data->players[i].username, u) == 0)
            return i;
    }
    return -1;
}

// ===== REGISTER =====
void reg() {
    char u[32], p[32];
    printf("Username: "); scanf("%s", u);

    if (find_user(u) != -1) {
        printf("User exists!\n");
        return;
    }

    printf("Password: "); scanf("%s", p);

    Player *pl = &data->players[data->player_count++];

    strcpy(pl->username, u);
    strcpy(pl->password, p);
    pl->gold = 150;
    pl->level = 1;
    pl->xp = 0;
}

// ===== LOGIN =====
int login() {
    char u[32], p[32];
    printf("Username: "); scanf("%s", u);
    printf("Password: "); scanf("%s", p);

    int i = find_user(u);

    if (i == -1 || strcmp(data->players[i].password, p) != 0) {
        printf("Login gagal\n");
        return 0;
    }

    me = i;
    data->players[i].active = 1;
    return 1;
}

// ===== DAMAGE =====
int dmg() {
    return BASE_DMG + data->players[me].weapon_dmg;
}

// ===== ARMORY =====
void armory() {
    printf("1. Sword(100)\n2. Axe(300)\n3. God(1000)\n");
    int c; scanf("%d", &c);

    if (c==1 && data->players[me].gold>=100) {
        data->players[me].weapon_dmg=10;
        data->players[me].gold-=100;
    }
}

// ===== REWARD =====
void reward(int win) {
    if (win) {
        data->players[me].xp+=50;
        data->players[me].gold+=100;
    } else {
        data->players[me].xp+=10;
    }
}

// ===== BATTLE =====
void battle() {
    Message msg;
    msg.type=1;
    strcpy(msg.username,data->players[me].username);
    msgsnd(msg_id,&msg,sizeof(msg)-sizeof(long),0);

    sleep(2);

    for(int i=0;i<data->player_count;i++){
        if(i!=me && data->players[i].in_battle)
            enemy=i;
    }

    if(enemy==-1){
        printf("VS BOT\n");
        data->players[me].hp=BASE_HP;
        while(data->players[me].hp>0){
            data->players[me].hp-=5;
            printf("\rHP %d",data->players[me].hp);
            fflush(stdout);
            sleep(1);
        }
        reward(0);
        return;
    }

    printf(GREEN "BATTLE START\n" RESET);

    while(1){
        printf("\rHP:%d ENEMY:%d",
            data->players[me].hp,
            data->players[enemy].hp);

        char c;
        scanf(" %c",&c);

        if(c==' '){
            if(time(NULL)-last_attack>=1){
                last_attack=time(NULL);
                data->players[enemy].hp-=dmg();
            }
        }

        if(c=='u'){
            data->players[enemy].hp-=dmg()*3;
        }

        if(data->players[enemy].hp<=0){
            printf(GREEN "\nWIN\n" RESET);
            reward(1);
            break;
        }

        if(data->players[me].hp<=0){
            printf(RED "\nLOSE\n" RESET);
            reward(0);
            break;
        }
    }

    data->players[me].in_battle=0;
}

// ===== MENU =====
void menu(){
    while(1){
        printf("\n1.Battle\n2.Armory\n3.Logout\n");
        int c; scanf("%d",&c);

        if(c==1) battle();
        if(c==2) armory();
        if(c==3) break;
    }
}

// ===== MAIN =====
int main(){
    shm_id=shmget(SHM_KEY,sizeof(SharedData),IPC_CREAT|0666);
    data=shmat(shm_id,NULL,0);

    msg_id=msgget(MSG_KEY,IPC_CREAT|0666);

    while(1){
        printf("\n1.Register\n2.Login\n3.Exit\n");
        int c; scanf("%d",&c);

        if(c==1) reg();
        else if(c==2 && login()) menu();
        else break;
    }
}
