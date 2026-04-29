# README.md

## Identitas

* Nama  : Muhammad Hugo Rayandra
* NRP   : 5027251076
* Kelas : C

---

## Deskripsi Soal

Program ini merupakan implementasi sistem game berbasis client-server menggunakan Inter Process Communication (IPC) di Linux. Sistem terdiri dari dua komponen utama:

1. Server (orion.c)
2. Client (eternal.c)

Fitur utama:

* Register dan login user
* Matchmaking (pencarian lawan)
* Battle real-time
* Armory (pembelian senjata)
* Sistem XP, level, dan gold
* Penyimpanan history permainan
* Tampilan terminal berwarna

---

## Konsep yang Digunakan

1. Shared Memory
   Digunakan untuk menyimpan seluruh data player agar bisa diakses oleh semua proses.

2. Message Queue
   Digunakan untuk komunikasi antara client dan server saat matchmaking.

3. Multithreading
   Digunakan untuk menjalankan battle secara real-time.

4. Mutex (Thread Synchronization)
   Digunakan untuk mencegah race condition saat banyak proses mengakses data yang sama.

5. ANSI Escape Code
   Digunakan untuk memberikan warna pada tampilan terminal.

---

## Alur Program

### 1. Register

User membuat akun baru dengan username unik.

### 2. Login

User masuk menggunakan username dan password.

### 3. Menu Utama

User dapat memilih:

* Battle
* Armory
* Logout

### 4. Matchmaking

Client mengirim permintaan ke server melalui message queue.
Server akan mencari lawan yang idle.

Jika tidak ada lawan:

* Player melawan BOT

---

### 5. Battle System

* HP awal: 100
* Attack: tombol spasi (cooldown 1 detik)
* Ultimate: tombol 'u' (damage lebih besar)
* Damage dipengaruhi weapon

---

### 6. Reward System

* Menang: +50 XP, +100 Gold
* Kalah: +10 XP

Jika XP >= 100:

* Level bertambah
* XP di-reset

---

### 7. Armory

Player dapat membeli:

* Sword (+10 damage)
* Axe (+20 damage)
* God Slayer (+50 damage)

---

### 8. History

Setiap hasil battle disimpan di:

```id="h8h8sm"
history.txt
```

---

## Struktur Program

```id="rb6l6r"
arena.h    → Struktur data & konstanta
orion.c    → Server (matchmaking)
eternal.c  → Client (UI & gameplay)
```

---

# PENJELASAN KODE (DETAIL)

## 1. arena.h

File ini adalah pusat struktur data.

### Struct Player

```id="y9wqbp"
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
```

Penjelasan:

* username/password → autentikasi
* gold, level, xp → progression
* active → status login
* in_battle → status sedang bertarung
* hp → health point
* weapon_dmg → tambahan damage

---

### Struct SharedData

```id="9ghk9b"
typedef struct {
    Player players[MAX_USER];
    int player_count;
    pthread_mutex_t lock;
} SharedData;
```

Penjelasan:

* players → array semua user
* player_count → jumlah user
* lock → mutex untuk mencegah race condition

---

### Struct Message

```id="m5w3s9"
typedef struct {
    long type;
    char username[32];
} Message;
```

Digunakan untuk komunikasi client → server.

---

## 2. orion.c (SERVER)

### Inisialisasi IPC

```id="x4z4y0"
shm_id = shmget(SHM_KEY, sizeof(SharedData), IPC_CREAT | 0666);
data = shmat(shm_id, NULL, 0);

msg_id = msgget(MSG_KEY, IPC_CREAT | 0666);
```

Penjelasan:

* shmget → membuat shared memory
* shmat → mengakses shared memory
* msgget → membuat message queue

---

### Fungsi find_idle()

```id="8yx3k1"
int find_idle(int ex) {
    for (...) {
        if (player aktif dan tidak battle)
            return index;
    }
}
```

Digunakan untuk mencari lawan.

---

### Matchmaking

```id="c9u3ka"
msgrcv(msg_id, &msg, ...);

data->players[p1].in_battle = 1;
data->players[p1].hp = BASE_HP;
```

Penjelasan:

* Server menerima request
* Menentukan player yang bertarung
* Set HP awal

---

## 3. eternal.c (CLIENT)

---

### Register

```id="x2we8k"
Player *pl = &data->players[data->player_count++];
```

Penjelasan:

* Menambahkan user baru ke shared memory

---

### Login

```id="y1zk6m"
if (strcmp(password benar))
    active = 1;
```

Penjelasan:

* Validasi user
* Set status aktif

---

### Damage System

```id="n8zk4o"
return BASE_DMG + weapon_dmg;
```

Damage dipengaruhi senjata.

---

### Battle System

```id="v7az0q"
if (c == ' ') {
    if (cooldown terpenuhi)
        hp musuh -= damage;
}
```

Penjelasan:

* Spasi untuk attack
* Ada cooldown 1 detik

---

### Ultimate

```id="0v34rg"
if (c == 'u') {
    hp musuh -= damage * 3;
}
```

Damage lebih besar.

---

### Armory

```id="y21r1p"
if (gold cukup)
    weapon_dmg bertambah;
```

Menambah damage.

---

### Reward

```id="2r83c0"
if (win) xp += 50;
else xp += 10;
```

---

### History

```id="r9dj3m"
fprintf(file, "username WIN/LOSE");
```

Menyimpan hasil battle.

---

## Cara Menjalankan Program

### Compile

```id="yjjbxt"
gcc orion.c -o orion -pthread
gcc eternal.c -o eternal -pthread
```

---

### Run Server

```id="p5yxxt"
./orion
```

---

### Run Client

```id="zkns41"
./eternal
```

---

## Cara Membersihkan

```id="6v3zbf"
rm -f orion eternal history.txt
ipcrm -M 1234
ipcrm -Q 5678
```

---

## Kendala

* Sinkronisasi data antar client
* Race condition
* Input terminal
* IPC cleanup

---

## Solusi

* Menggunakan mutex
* Message queue untuk komunikasi
* BOT fallback
* Cooldown system

---

## Kesimpulan

Program ini berhasil mengimplementasikan sistem client-server berbasis IPC dengan fitur game interaktif. Penggunaan shared memory memungkinkan efisiensi data, sementara message queue mempermudah komunikasi antar proses.

Program ini juga memperkuat pemahaman tentang:

* IPC Linux
* Threading
* Sinkronisasi
* Manajemen memori

---

## Saran

* Tambahkan database per user
* Matchmaking queue
* GUI (ncurses)
* Leaderboard

---
