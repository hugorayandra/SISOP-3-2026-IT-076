# README.md

## Identitas

* Nama  : Muhammad Hugo Rayandra
* NRP   : 5027251076
* Kelas : C

---

# Soal 1 - Present day, Present time

## Identitas

* Nama  : Muhammad Hugo Rayandra
* NRP   : 5027251076
* Kelas : C

---

## Deskripsi Soal

Soal 1 merupakan implementasi sistem komunikasi berbasis client-server menggunakan socket TCP di bahasa C.

Sistem terdiri dari:

* Server: `wired.c`
* Client: `navi.c`
* Header: `protocol.h`

Fitur utama:

* Multi-client chat
* Username unik
* Broadcast pesan
* Command `/exit`
* Mode admin (The Knights)
* RPC admin (list user, uptime, shutdown)
* Logging ke file `history.log`
* Non-blocking I/O menggunakan `select()`

---

## Konsep yang Digunakan

1. Socket Programming (TCP)
   Digunakan untuk komunikasi antara client dan server.

2. Multiplexing (select)
   Server dapat menangani banyak client tanpa thread/fork.

3. Client-Server Architecture
   Server sebagai pusat komunikasi, client sebagai pengguna.

4. Logging System
   Semua aktivitas dicatat dalam file.

---

## Alur Program

### 1. Server Start

Server dijalankan dan mendengarkan koneksi pada IP dan port tertentu.

---

### 2. Client Connect

Client memasukkan username.

Jika username sudah digunakan:

* Client ditolak

---

### 3. Chat System

* Semua pesan dikirim ke server
* Server broadcast ke semua client lain

Format:

```id="f6bqk2"
[username]: pesan
```

---

### 4. Exit

Client mengetik:

```id="2n3zrd"
/exit
```

Client akan disconnect.

---

### 5. Admin Mode

Login sebagai:

```id="7xt8y7"
The Knights
```

Password:

```id="9m9n7m"
protocol7
```

---

### 6. RPC Admin

Command:

```id="zaz3ha"
1 → list user aktif
2 → uptime server
3 → shutdown server
```

---

### 7. Logging

Semua aktivitas disimpan ke:

```id="c7ppw0"
history.log
```

Contoh:

```id="p4h7gf"
[2026-04-29 10:00:00] [User] [john]: hello
```

---

## Struktur Program

```id="jz6a7l"
protocol.h → konfigurasi & logging
wired.c    → server
navi.c     → client
```

---

# PENJELASAN KODE

## 1. protocol.h

Berisi:

* Konstanta
* Fungsi logging
* Timestamp

### Contoh:

```id="j9b6nx"
#define SERVER_PORT 8080
```

---

### Fungsi log_event()

```id="q7qk2q"
void log_event(const char *role, const char *msg)
```

Menulis log ke file `history.log`.

---

## 2. wired.c (SERVER)

### Struktur Client

```id="k6k4mv"
typedef struct {
    int socket;
    char name[32];
    int is_admin;
} Client;
```

---

### Array Client

```id="gkn6cj"
Client clients[MAX_CLIENTS];
```

Digunakan untuk menyimpan semua client aktif.

---

### Fungsi broadcast()

```id="3l8e7p"
send ke semua client kecuali pengirim
```

---

### Fungsi remove_client()

```id="qzuv8j"
close socket dan reset data
```

---

### select()

```id="0zvlj2"
select(max_sd + 1, &readfds, NULL, NULL, NULL);
```

Digunakan untuk:

* menerima koneksi baru
* membaca pesan dari client

---

### Admin Login

```id="2h4l2c"
if (username == ADMIN_NAME)
```

---

### RPC

```id="6dnjlp"
1 → list user
2 → uptime
3 → shutdown
```

---

## 3. navi.c (CLIENT)

### connect()

```id="h5r3fn"
connect(sock, ...)
```

Menghubungkan ke server.

---

### select() di client

Digunakan untuk:

* membaca input user
* menerima pesan dari server

---

### Input user

```id="z2rj19"
fgets(msg, ...)
```

---

### Exit

```id="n4fyhe"
if (strcmp(msg, "/exit") == 0)
```

---

## Cara Menjalankan

### Compile

```id="9t2iqg"
gcc wired.c -o wired
gcc navi.c -o navi
```

---

### Run Server

```id="r8t0g1"
./wired
```

---

### Run Client

```id="8gq4r9"
./navi
```

---

## Cara Membersihkan

```id="xy5dpg"
rm -f wired navi history.log
```

---

# PENJELASAN KODE MENYELURUH – SOAL 1 Present day, Present time

## Gambaran Umum Sistem

Program ini merupakan sistem chat berbasis client-server menggunakan socket TCP. Terdapat tiga komponen utama:

* `protocol.h` → konfigurasi dan utilitas (logging, konstanta)
* `wired.c` → server
* `navi.c` → client

Alur utama:

1. Server aktif dan menunggu koneksi
2. Client terhubung dan mengirim username
3. Server menyimpan client dan memproses pesan
4. Pesan di-broadcast ke client lain
5. Admin memiliki kontrol tambahan (RPC)

---

# 1. PENJELASAN protocol.h

File ini berfungsi sebagai pusat konfigurasi dan utilitas yang digunakan oleh server dan client.

---

## Konstanta

```c
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define NAME_LEN 32
```

Penjelasan:

* `MAX_CLIENTS` → jumlah maksimal client
* `BUFFER_SIZE` → ukuran buffer untuk pesan
* `NAME_LEN` → panjang maksimal username

---

```c
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
```

Menentukan alamat server.

---

```c
#define ADMIN_NAME "The Knights"
#define ADMIN_PASS "protocol7"
```

Digunakan untuk login admin.

---

## Fungsi get_timestamp()

```c
time_t now = time(NULL);
struct tm *t = localtime(&now);
```

Mengambil waktu sistem saat ini dan mengubahnya ke format lokal.

```c
strftime(buffer, 32, "[%Y-%m-%d %H:%M:%S]", t);
```

Mengubah waktu menjadi string.

---

## Fungsi log_event()

```c
FILE *fp = fopen("history.log", "a");
```

Membuka file log dalam mode append.

```c
fprintf(fp, "%s [%s] %s\n", timebuf, role, message);
```

Menulis log dengan format:

* timestamp
* role (User/Admin/System)
* isi pesan

---

# 2. PENJELASAN wired.c (SERVER)

Server adalah pusat komunikasi semua client.

---

## Struktur Client

```c
typedef struct {
    int socket;
    char name[NAME_LEN];
    int is_admin;
} Client;
```

Penjelasan:

* `socket` → descriptor koneksi
* `name` → username
* `is_admin` → status admin

---

## Array Clients

```c
Client clients[MAX_CLIENTS];
```

Digunakan untuk menyimpan semua client aktif.

---

## Fungsi name_exists()

Melakukan iterasi untuk mengecek apakah username sudah digunakan.

Tujuan:

* mencegah duplikasi username

---

## Fungsi broadcast()

```c
send(clients[i].socket, msg, strlen(msg), 0);
```

Mengirim pesan ke semua client kecuali pengirim.

---

## Fungsi remove_client()

```c
close(clients[i].socket);
clients[i].socket = 0;
```

Menutup koneksi dan menghapus data client.

---

## Inisialisasi Server

```c
server_fd = socket(AF_INET, SOCK_STREAM, 0);
```

Membuat socket TCP.

```c
bind(server_fd, ...)
listen(server_fd, 10);
```

Mengikat socket ke port dan mulai mendengarkan koneksi.

---

## Mekanisme select()

```c
FD_SET(server_fd, &readfds);
```

Menambahkan server socket ke set.

```c
select(max_sd + 1, &readfds, NULL, NULL, NULL);
```

Fungsi ini:

* memantau banyak socket
* non-blocking

---

## Menerima Koneksi Baru

```c
new_socket = accept(server_fd, NULL, NULL);
```

Server menerima koneksi client.

```c
recv(new_socket, name, NAME_LEN, 0);
```

Menerima username.

---

## Validasi Username

```c
if (name_exists(name))
```

Jika username sudah ada:

* kirim "NAME_EXISTS"
* tutup koneksi

---

## Menyimpan Client

```c
clients[i].socket = new_socket;
strcpy(clients[i].name, name);
```

Menambahkan client ke array.

---

## Menerima Pesan Client

```c
recv(sd, buffer, BUFFER_SIZE, 0);
```

Membaca pesan dari client.

---

## Command /exit

```c
if (strcmp(buffer, "/exit") == 0)
```

Client akan:

* dihapus
* koneksi ditutup

---

## Admin Login

```c
if (strcmp(clients[i].name, ADMIN_NAME) == 0)
```

Jika username admin:

* meminta password
* validasi password

---

## Admin RPC

### 1. List User

```c
for (...)
```

Menampilkan semua user aktif.

---

### 2. Uptime

```c
time(NULL) - start_time
```

Menghitung lama server berjalan.

---

### 3. Shutdown

```c
exit(0);
```

Mematikan server.

---

## Broadcast Pesan

```c
sprintf(msg, "[%s]: %s", name, buffer);
broadcast(msg, sd);
```

Format pesan:

```text
[username]: pesan
```

---

## Logging

Setiap aktivitas:

* connect
* disconnect
* chat
* admin command

Dicatat dengan:

```c
log_event(...)
```

---

# 3. PENJELASAN navi.c (CLIENT)

Client berfungsi sebagai interface pengguna.

---

## Inisialisasi Socket

```c
sock = socket(AF_INET, SOCK_STREAM, 0);
```

Membuat socket.

---

## Koneksi ke Server

```c
connect(sock, ...)
```

Menghubungkan ke server.

---

## Input Username

```c
fgets(name, ...)
```

Mengambil input user.

---

## Pengiriman Username

```c
send(sock, name, NAME_LEN, 0);
```

---

## Loop Utama

Menggunakan `select()` untuk:

* membaca input user
* menerima pesan dari server

---

## Input User

```c
fgets(msg, ...)
```

Mengambil pesan dari user.

---

## Kirim Pesan

```c
send(sock, msg, strlen(msg), 0);
```

---

## Receive Message

```c
recv(sock, msg, BUFFER_SIZE, 0);
```

Menampilkan pesan dari server.

---

## Exit Command

```c
if (strcmp(msg, "/exit") == 0)
```

Client keluar.

---

# HUBUNGAN ANTAR KOMPONEN

```text
CLIENT → SERVER → CLIENT
```

* Client mengirim pesan ke server
* Server memproses dan broadcast
* Client lain menerima

---

# KELEBIHAN DESAIN

* Tidak menggunakan thread di server (efisien)
* Menggunakan select() untuk multi-client
* Logging sistematis
* Mendukung admin control

---



# SOAL 2 - The Battle of Eterion
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
