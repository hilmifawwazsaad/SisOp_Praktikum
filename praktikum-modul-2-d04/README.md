[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/o3jj1gXA)
<div align=center>

|    NRP     |      Name      |
| :--------: | :------------: |
| 5025221007 | Yehezkiella Felicia Jeis Timbulong|
| 5025221047 | Muhammad Rayyaan Fatikhahur Rakhim |
| 5025221103 | Hilmi Fawwaz Sa'ad |

# Praktikum Modul 2 _(Module 2 Lab Work)_

</div>

### Daftar Soal _(Task List)_

- [Task 1 - HORSECHAMP](/task-1/)

- [Task 2 - Tugas Akhir Yuan dan Bubu _(Yuan's and Bubu's Final Project)_](/task-2/)

- [Task 3 - Ramalan Cuaca Rama _(Rama's Weather Forecast)_](/task-3/)

- [Task 4 - Tic Tac Toe](/task-4/)

### Laporan Resmi Praktikum Modul 2 _(Module 2 Lab Work Report)_

Tulis laporan resmi di sini!

_Write your lab work report here!_
## 1️⃣ Soal 1
DJumanto adalah seorang pengembang hebat yang telah menciptakan banyak kode keren. Saat ini, ia ingin membuat sebuah proyek baru yang disebut "horsechamp", sebuah program untuk lomba balapan kuda bernama horsechamp.c. Program ini memiliki 4 fungsi utama, yaitu HorseFetcher, HorseRaceHooray, HorseChampionLeaderboard, dan Main.
```C
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_HORSES 120
#define MAX_PRIME 4000
#define RACE_NUMBER 1

void HorseFetcher() {
    int result = system("wget 'https://docs.google.com/uc?export=download&id=1gJkoTbiHZcJ1M36vqZPTrj3qTQkRMElQ' -O horses.zip");
    if (result != 0) {
        fprintf(stderr, "Download failed.\n");
        return;
    }

    result = system("unzip -o horses.zip -d horse_files");
    if (result != 0) {
        fprintf(stderr, "Unzip failed.\n");
        return;
    }
}

typedef struct {
    char horse_name[20];
    char owner[20];
    int prime_number;
} HorseInfo;

bool is_prime(int n) {
    if (n <= 1)
        return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0)
            return false;
    }
    return true;
}

void randomize_array(HorseInfo *arr, int count) {
    srand(time(NULL));
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        HorseInfo temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void *race_thread(void *arg) {
    HorseInfo *horse = (HorseInfo *)arg;
    int prime_value = 0;
    while (prime_value == 0) {
        int rand_num = rand() % MAX_PRIME + 1;
        if (is_prime(rand_num)) {
            prime_value = rand_num;
        }
    }
    usleep(1000); // Simulate delay
    horse->prime_number = prime_value;
    return NULL;
}

void DisplayHorseFiles() {
    printf("-----------------------------------------------------------------------\n");
    printf("Available Horse Data Files:\n");

    DIR *dir = opendir("horse_files");
    if (dir == NULL) {
        perror("Failed to open 'horse_files'");
        return;
    }

    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, "horse_") != NULL) {
            count++;
            printf("%d. %s\n", count, entry->d_name);
        }
    }

    closedir(dir);
}

void HorseRaceHooray() {
    DisplayHorseFiles();
    int choice;
    printf("Choose a number: ");
    if (scanf("%d", &choice) != 1 || choice < 1) {
        printf("Invalid choice. Try again.\n");
        return;
    }

    char selected_horse[50];
    snprintf(selected_horse, sizeof(selected_horse), "horse_%d", choice);

    char horse_path[100];
    snprintf(horse_path, sizeof(horse_path), "horse_files/%s.txt", selected_horse);

    FILE *file = fopen(horse_path, "r");
    if (file == NULL) {
        perror("File open error");
        return;
    }

    HorseInfo horses[MAX_HORSES];
    int horse_count = 0;
    while (horse_count < MAX_HORSES && fscanf(file, "%[^:]:%s\n", horses[horse_count].horse_name, horses[horse_count].owner) == 2) {
        horse_count++;
    }
    fclose(file);

    randomize_array(horses, horse_count);

    pthread_t threads[horse_count];
    for (int i = 0; i < horse_count; i++) {
        if (pthread_create(&threads[i], NULL, race_thread, (void *)&horses[i]) != 0) {
            perror("Thread creation error");
            return;
        }
    }

    for (int i = 0; i < horse_count; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Thread join error");
            return;
        }
    }

    char result_filename[200];
    int race_no = RACE_NUMBER;

    while (true) {
        snprintf(result_filename, sizeof(result_filename), "HorseRace_%d.txt", race_no);
        FILE *check_file = fopen(result_filename, "r");
        if (check_file == NULL) {
            break;
        }
        fclose(check_file);
        race_no++;
    }

    FILE *output_file = fopen(result_filename, "w");
    if (output_file == NULL) {
        perror("Output file error");
        return;
    }

    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);

    fprintf(output_file, "----Horse Race %d------\n", race_no);
    fprintf(output_file, "Date: %02d/%02d/%04d\n", local_time->tm_mday, local_time->tm_mon + 1, local_time->tm_year + 1900);
    fprintf(output_file, "Time: %02d:%02d:%02d\n\n", local_time->tm_hour, local_time->tm_min, local_time->tm_sec);

    for (int i = 0; i < horse_count; i++) {
        fprintf(output_file, "%d. %s %s %d\n", i + 1, horses[i].horse_name, horses[i].owner, horses[i].prime_number);
    }

    fclose(output_file);
    printf("Horse race completed, results stored in: %s\n", result_filename);
}

void HorseChampionLeaderboard(const char *directory) {
    int choice;
    printf("Select Horse Race History:\n");

    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror("Failed to open directory.\n");
        return;
    }

    struct dirent *entry;
    int history_count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, "HorseRace_") != NULL) {
            history_count++;
            printf("%d. %s\n", history_count, entry->d_name);
        }
    }

    if (history_count == 0) {
        printf("No history found.\n");
        closedir(dir);
        return;
    }

    printf("Enter number: ");
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > history_count) {
        printf("Invalid choice.\n");
        closedir(dir);
        return;
    }

    closedir(dir);

    char file_path[100];
    snprintf(file_path, sizeof(file_path), "%s/HorseRace_%d.txt", directory, choice);

    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return;
    }

    printf("-----------------------------------------------------------------------\n");
    printf("\n----HORSE CHAMPIONSHIP (%d)----\n", choice);

    bool skip_first = true;
    char line[500];
    while (fgets(line, sizeof(line), file)) {
        if (skip_first) {
            skip_first = false;
            continue;
        }
        printf("%s", line);
    }

    fclose(file);
}

int main() {
    int choice;
    do {
        printf("-----------------------------------------------------------------------\n");
        printf("Main Menu:\n");
        printf("1. Download Horse Data\n");
        printf("2. Begin Horse Race\n");
        printf("3. View Winners\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                HorseFetcher();
                if (access("horses.zip", F_OK) != -1) {
                    printf("Horse data downloaded successfully.\n");
                } else {
                    printf("Failed to download horse data.\n");
                }
                break;
            case 2:
                HorseRaceHooray();
                break;
            case 3:
                HorseChampionLeaderboard(".");
                break;
            case 4:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 4);

    return 0;
}
```
_dokumentasi_
![alt text](/resource/1a-1.png)
![alt text](/resource/1a-2.png)
![alt text](/resource/1a-3.png)
![alt text](/resource/1a-4.png)
![alt text](/resource/1a-5.png)
![alt text](/resource/1a-6.png)
![alt text](/resource/1a-7.png)

### Problem 1a
Buatlah fungsi HorseFetcher, yang bertugas mengambil data kuda dari internet dan menyimpannya dalam file dengan nama horse_1.txt, horse_2.txt, horse_3.txt, dan seterusnya. Kuda dan pemilik kuda dipisahkan dengan “:”, sebagai contoh “Morioh:DJumanto”, DJumanto adalah pemilik dari kuda Morioh.

**Jawab**
```C
void HorseFetcher() {
    int result = system("wget 'https://docs.google.com/uc?export=download&id=1gJkoTbiHZcJ1M36vqZPTrj3qTQkRMElQ' -O horses.zip");
    if (result != 0) {
        fprintf(stderr, "Download failed.\n");
        return;
    }

    result = system("unzip -o horses.zip -d horse_files");
    if (result != 0) {
        fprintf(stderr, "Unzip failed.\n");
        return;
    }
}
```
HorseFetcher adalah fungsi yang mengunduh dan mengekstrak data kuda dari Google Drive menggunakan wget dan unzip. Jika ada kesalahan saat mengunduh atau mengekstrak, pesan kesalahan akan ditampilkan.

- Perintah Pengunduhan
```C
int result = system("wget 'https://docs.google.com/uc?export=download&id=1gJkoTbiHZcJ1M36vqZPTrj3qTQkRMElQ' -O horses.zip");
```
system menjalankan perintah shell di dalam kode C. Di sini, fungsi wget digunakan untuk mengunduh file dari Google Drive. Opsi -O horses.zip mengarahkan output ke file bernama horses.zip.
Hasil pengunduhan disimpan dalam variabel result. Jika result bernilai selain 0, itu menandakan bahwa perintah gagal.

- Pemeriksaan Kesalahan Pengunduhan
```C
if (result != 0) {
    fprintf(stderr, "Download failed.\n");
    return;
}
```
Pemeriksaan apakah perintah wget berhasil. Jika result bukan 0, berarti terjadi kesalahan saat mengunduh file. Dalam hal ini, fungsi menulis pesan kesalahan ke stderr dan keluar dari fungsi (return).

- Bagian Perintah Ekstraksi
```C
result = system("unzip -o horses.zip -d horse_files");
```
Setelah unduhan berhasil, perintah unzip digunakan untuk mengekstrak file horses.zip ke direktori horse_files. Opsi -o digunakan untuk mengekstrak dan menimpa file yang ada jika diperlukan.

- Bagian Perintah Ekstraksi
```C
if (result != 0) {
    fprintf(stderr, "Unzip failed.\n");
    return;
}
```
Pemeriksaan apakah perintah unzip berhasil. Jika result bukan 0, berarti ada kesalahan saat mengekstrak file. Dalam hal ini, fungsi menulis pesan kesalahan ke stderr dan mengakhiri eksekusi dengan return.

### Problem 1b
Buatlah fungsi HorseRaceHooray, yang bertugas melakschildan balapan kuda. Parameter yang diguakan adalah salah satu file kuda yang telah kalian fetch sebelumnya. Gunakan thread untuk menjalankan perlombaan tersebut. Setiap kuda akan melakukan perhitungan bilangan prima dengan angka acak antara 1 sampai 4000. Kuda yang menyelesaikan perhitungan lebih awal akan menempati posisi lebih tinggi dari kuda lainnya. Setelah perlombaan selesai, simpan hasil lomba dalam format HorseRace_(nomor lomba).txt.

**Jawab**

- Fungsi HorseRaceHooray
```C
void HorseRaceHooray() {
    DisplayHorseFiles();
    int choice;
    printf("Choose a number: ");
    if (scanf("%d", &choice) != 1 || choice < 1) {
        printf("Invalid choice. Try again.\n");
        return;
    }

    char selected_horse[50];
    snprintf(selected_horse, sizeof(selected_horse), "horse_%d", choice);

    char horse_path[100];
    snprintf(horse_path, sizeof(horse_path), "horse_files/%s.txt", selected_horse);

    FILE *file = fopen(horse_path, "r");
    if (file == NULL) {
        perror("File open error");
        return;
    }

    HorseInfo horses[MAX_HORSES];
    int horse_count = 0;
    while (horse_count < MAX_HORSES && fscanf(file, "%[^:]:%s\n", horses[horse_count].horse_name, horses[horse_count].owner) == 2) {
        horse_count++;
    }
    fclose(file);

    randomize_array(horses, horse_count);

    pthread_t threads[horse_count];
    for (int i = 0; i < horse_count; i++) {
        if (pthread_create(&threads[i], NULL, race_thread, (void *)&horses[i]) != 0) {
            perror("Thread creation error");
            return;
        }
    }

    for (int i = 0; i < horse_count; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Thread join error");
            return;
        }
    }

    char result_filename[200];
    int race_no = RACE_NUMBER;

    while (true) {
        snprintf(result_filename, sizeof(result_filename), "HorseRace_%d.txt", race_no);
        FILE *check_file = fopen(result_filename, "r");
        if (check_file == NULL) {
            break;
        }
        fclose(check_file);
        race_no++;
    }

    FILE *output_file = fopen(result_filename, "w");
    if (output_file == NULL) {
        perror("Output file error");
        return;
    }

    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);

    fprintf(output_file, "----Horse Race %d------\n", race_no);
    fprintf(output_file, "Date: %02d/%02d/%04d\n", local_time->tm_mday, local_time->tm_mon + 1, local_time->tm_year + 1900);
    fprintf(output_file, "Time: %02d:%02d:%02d\n\n", local_time->tm_hour, local_time's minute, local_time's second);

    for (int i = 0; i < horse_count; i++) {
        fprintf(output_file, "%d. %s %s %d\n", i + 1, horses[i].horse_name, horses[i].owner, horses[i].prime_number);
    }

    fclose(output_file);
    printf("Horse race completed, results stored in: %s\n", result_filename);
}
```
HorseRaceHooray adalah fungsi untuk memulai perlombaan kuda. Pertama, ia menampilkan file data kuda yang tersedia dan meminta pengguna untuk memilih salah satunya. Jika tidak valid, ia menampilkan pesan kesalahan. Setelah valid, ia membuka file dan membaca informasi kuda ke dalam array HorseInfo. Setelah itu, ia mengacak array dan membuat thread untuk setiap kuda, menjalankan fungsi race_thread. Setelah semua thread selesai, ia menyimpan hasil perlombaan ke dalam file dan menampilkan pesan bahwa perlombaan selesai.

- Bagian Meminta user memilih kuda
```C
    DisplayHorseFiles();  // Menampilkan daftar file kuda yang tersedia
    int choice;
    printf("Choose a number: ");  // Meminta pengguna memilih file data kuda
    if (scanf("%d", &choice) != 1 || choice < 1) {
        printf("Invalid choice. Try again.\n");  // Validasi input pengguna
        return;  // Keluar dari fungsi jika input tidak valid
    }
```
Kode ini menampilkan file kuda yang tersedia menggunakan DisplayHorseFiles. Kemudian, meminta pengguna memilih nomor file kuda dengan scanf. Jika input tidak valid (mis. bukan angka atau kurang dari 1), program memberikan pesan kesalahan dan keluar dari fungsi.

- Membuka File Data Kuda dan Membaca Isinya
```C
    char selected_horse[50];
    snprintf(selected_horse, sizeof(selected_horse), "horse_%d", choice);  // Menentukan nama file berdasarkan pilihan pengguna

    char horse_path[100];
    snprintf(horse_path, sizeof(horse_path), "horse_files/%s.txt", selected_horse);  // Membuat path file data kuda

    FILE *file = fopen(horse_path, "r");  // Membuka file data kuda
    if (file == NULL) {
        perror("File open error");  // Jika file tidak ditemukan atau tidak dapat dibuka
        return;
    }

    HorseInfo horses[MAX_HORSES];
    int horse_count = 0;  // Menghitung jumlah kuda yang ada dalam file
    while (horse_count < MAX_HORSES && fscanf(file, "%[^:]:%s\n", horses[horse_count].horse_name, horses[horse_count].owner) == 2) {
        horse_count++;  // Membaca informasi kuda dari file
    }
    fclose(file);  // Menutup file setelah selesai membaca

```
Bagian ini menentukan nama file yang dipilih pengguna dan membuka file tersebut. Jika file tidak dapat dibuka, fungsi akan keluar. Jika file terbuka, program membaca informasi kuda ke dalam array HorseInfo hingga jumlah maksimal kuda atau akhir file.

- Mengacak Data Kuda dan Memulai Thread Perlombaan
```C
    randomize_array(horses, horse_count);  // Mengacak data kuda agar urutan tidak selalu sama

    pthread_t threads[horse_count];  // Membuat array untuk menyimpan thread kuda
    for (int i = 0; i < horse_count; i++) {
        if (pthread_create(&threads[i], NULL, race_thread, (void *)&horses[i]) != 0) {  // Membuat thread untuk setiap kuda
            perror("Thread creation error");
            return;  // Jika terjadi kesalahan saat membuat thread
        }
    }

    for (int i = 0; i < horse_count; i++) {
        if (pthread_join(threads[i], NULL) != 0) {  // Menunggu semua thread selesai
            perror("Thread join error");
            return;  // Jika terjadi kesalahan saat menggabungkan thread
        }
    }

```
Bagian ini mengacak data kuda menggunakan randomize_array. Kemudian, membuat thread untuk setiap kuda yang menjalankan fungsi race_thread. Jika terjadi kesalahan saat membuat atau menggabungkan thread, pesan kesalahan ditampilkan dan fungsi keluar.

- Menyimpan Hasil Perlombaan
```C
    char result_filename[200];
    int race_no = RACE_NUMBER;

    while (true) {  // Menentukan nama file hasil perlombaan
        snprintf(result_filename, sizeof(result_filename), "HorseRace_%d.txt", race_no);  // Nama file berdasarkan nomor perlombaan
        FILE *check_file = fopen(result_filename, "r");  // Memeriksa apakah file sudah ada
        if (check_file == NULL) {  // Jika file belum ada, keluar dari loop
            break;
        }
        fclose(check_file);  // Jika file sudah ada, tutup file dan tingkatkan nomor perlombaan
        race_no++;
    }

    FILE *output_file = fopen(result_filename, "w");  // Membuka file hasil perlombaan untuk ditulis
    if (output_file == NULL) {  // Jika file tidak dapat dibuka
        perror("Output file error");
        return;
    }

    time_t current_time = time(NULL);  // Mengambil waktu saat ini
    struct tm *local_time = localtime(&current_time);  // Mengonversi waktu ke format lokal

    fprintf(output_file, "----Horse Race %d------\n", race_no);  // Menulis informasi dasar perlombaan
    fprintf(output_file, "Date: %02d/%02d/%04d\n", local_time->tm_mday, local_time->tm_mon + 1, local_time->tm_year + 1900);  // Menulis tanggal
    fprintf(output_file, "Time: %02d:%02d:%02d\n\n", local_time->tm_hour, local_time->tm_minute, local_time->tm_second);  // Menulis waktu

    for (int i = 0; i < horse_count; i++) {
        fprintf(output_file, "%d. %s %s %d\n", i + 1, horses[i].horse_name, horses[i].owner, horses[i].prime_number);  // Menulis hasil kuda
    }

    fclose(output_file);  // Menutup file setelah selesai menulis
    printf("Horse race completed, results stored in: %s\n", result_filename);  // Menampilkan pesan bahwa perlombaan selesai

```
Bagian ini menyimpan hasil perlombaan ke dalam file. Program membuat nama file hasil perlombaan berdasarkan nomor perlombaan. Jika nama sudah ada, nomor ditingkatkan dan memeriksa lagi. Setelah menentukan nama file, hasil ditulis ke file, termasuk informasi dasar, tanggal, waktu, dan hasil setiap kuda. Jika terjadi kesalahan saat membuka file, program keluar dengan pesan kesalahan. Setelah selesai, program menutup file dan menampilkan pesan bahwa perlombaan telah selesai.

### Problem 1c
Buatlah fungsi HorseChampionLeaderboard, yang bertugas menampilkan pemenang dari pertandingan-pertandingan sebelumnya. Berikan opsi kepada pengguna untuk memilih lomba mana yang ingin dilihat hasilnya.

**Jawab**
- Fungsi HorseChampionLeaderboard
```C
void HorseChampionLeaderboard(const char *directory) {
    int choice;
    printf("Select Horse Race History:\n");

    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror("Failed to open directory.\n");
        return;
    }

    struct dirent *entry;
    int history_count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, "HorseRace_") != NULL) {
            history_count++;
            printf("%d. %s\n", history_count, entry->d_name);
        }
    }

    if (history_count == 0) {
        printf("No history found.\n");
        closedir(dir);
        return;
    }

    printf("Enter number: ");
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > history_count) {
        printf("Invalid choice.\n");
        closedir(dir);
        return;
    }

    closedir(dir);

    char file_path[100];
    snprintf(file_path, sizeof(file_path), "%s/HorseRace_%d.txt", directory, choice);

    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return;
    }

    printf("-----------------------------------------------------------------------\n");
    printf("\n----HORSE CHAMPIONSHIP (%d)----\n", choice);

    bool skip_first = true;
    char line[500];
    while (fgets(line, sizeof(line), file)) {
        if (skip_first) {
            skip_first = false;
            continue;
        }
        printf("%s", line);
    }

    fclose(file);
}
```
Penjelasan mendetail dari kode fungsi HorseChampionLeaderboard ini akan dilakukan per bagian untuk memberikan konteks dan pemahaman yang lebih jelas.
- Pendahuluan
```C
void HorseChampionLeaderboard(const char *directory) {
    int choice;
    printf("Select Horse Race History:\n");
```
Fungsi HorseChampionLeaderboard bertugas untuk menampilkan riwayat perlombaan kuda dari direktori yang diberikan. Parameter directory adalah nama direktori di mana file riwayat disimpan.
int choice; mendefinisikan variabel untuk menampung pilihan pengguna.
printf("Select Horse Race History:\n"); menampilkan prompt untuk meminta pengguna memilih riwayat perlombaan.

 - Membuka direktori dan menghitung file
```C
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror("Failed to open directory.\n");
        return;
    }

    struct dirent *entry;
    int history_count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, "HorseRace_") != NULL) {
            history_count++;
            printf("%d. %s\n", history_count, entry->d_name);
        }
    }
```
DIR *dir = opendir(directory); mencoba membuka direktori yang diberikan.
if (dir == NULL) { ... } memeriksa apakah direktori berhasil dibuka; jika gagal, fungsi menampilkan pesan kesalahan dan keluar.
struct dirent *entry; mendefinisikan variabel untuk membaca entri direktori.
int history_count = 0; menginisialisasi penghitung file riwayat.
while ((entry = readdir(dir)) != NULL) { ... } membaca setiap entri dalam direktori.
if (strstr(entry->d_name, "HorseRace_") != NULL) { ... } memeriksa apakah nama file berisi "HorseRace_"; jika iya, menambah history_count dan mencetak nama file beserta indeksnya.

- Bagian Validasi Pilihan Pengguna
```C
    if (history_count == 0) {
        printf("No history found.\n");
        closedir(dir);
        return;
    }

    printf("Enter number: ");
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > history_count) {
        printf("Invalid choice.\n");
        closedir(dir);
        return;
    }

    closedir(dir);
}
```
if (history_count == 0) { ... } memeriksa apakah ada file riwayat yang ditemukan. Jika tidak ada, fungsi menampilkan pesan bahwa tidak ada riwayat dan menutup direktori.
printf("Enter number: "); meminta pengguna memilih nomor file riwayat.
if (scanf("%d", &choice) != 1 || choice < 1 || choice > history_count) { ... } memeriksa apakah input pengguna valid. Jika tidak, menampilkan pesan kesalahan dan keluar.
closedir(dir); menutup direktori setelah selesai.

- Bagian Membuka File Riwayat dan Menampilkan Konten
```C
    char file_path[100];
    snprintf(file_path, sizeof(file_path), "%s/HorseRace_%d.txt", directory, choice);

    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return;
    }

    printf("-----------------------------------------------------------------------\n");
    printf("\n----HORSE CHAMPIONSHIP (%d)----\n", choice);

    bool skip_first = true;
    char line[500];
    while (fgets(line, sizeof(line), file)) {
        if (skip_first) {
            skip_first = false;
            continue;
        }
        printf("%s", line);
    }

    fclose(file);
}
```

### Kendala

Waktu melihat hasil pemenang, teks tidak dapat terbaca, hanya menghasilkan simbol-simbol yang tidak dapat terbaca, kemudian sudah di fix sehingga hasilnya benar

## 2️⃣ Soal 2
Yuan dan Bubu ditugaskan untuk mengunduh gambar oleh dosennya, namun dengan beberapa tantangan. Mereka diberikan satu folder yang berisikan file random. Dimana pada folder tersebut, terdapat beberapa file yang berisi petunjuk terkait gambar yang harus diunduh oleh Yuan dan Bubu.
### Problem 2a
Pertama-tama, mereka harus mengunduh sebuah file zip dari google drive, kemudian mengekstraknya dan menghapus file zip tersebut. Setelah itu, mereka harus membuat folder baru dengan nama “task” dan memindahkan file yang berkaitan dengan petunjuk ke dalam folder “task”. File yang berkaitan memiliki penamaan dengan awalan "task_{id}" dan berekstensi txt. Lalu hapus folder hasil zip sebelumya.

**Jawab**

_1. Dalam Main Function, kita buat dahulu variabel dan pemanggilan fungsi yang dibutuhkan untuk menyelesaikan problem 2a_
```C
int main()
{
    // SOAL POIN A
    char *url = "https://drive.google.com/uc?export=download&id=1CdWKLXVAc6P26sF-5oSrlVAsSExA1PQh";
    char *output_filename = "task_sisop.zip";
    char *path = "task";
    pthread_t yuan_thread, bubu_thread; // thread identifier

    // fungsi download file
    char *downloaded_file = download_file(url, output_filename);

    // fungsi create directory
    create_directory(path);

    // fungs unzip file
    unzip_file(downloaded_file, path);

    // hapus file tidak berguna yang sudah di download
    if (remove(downloaded_file) != 0)
    {
        perror("Gagal menghapus file yang sudah di download");
    }
    else
    {
        printf("Menghapus file: %s\n", downloaded_file);
    }

    // membebaskan memory
    free(downloaded_file);

    // mencari file .txt dan memindahkannya ke folder task
    search_task(path);
 
    ...
```
- `pthread_t yuan_thread, bubu_thread;` untuk menampung identitas thread yang akan dibuat. Digunakan pada problem 2b dan 2c
- Setelah proses ekstrak/unzip selesai, file .zip akan dihapus
- `free(downloaded_file)` untuk membebaskan memori yang dialokasikan untuk `downloaded_file` setelah file tidak digunakan

_2. Membuat fungsi untuk mendowload file .zip dari google drive_
```C
char *download_file(char *url, char *output_name)
{
    // membuat array argumen untuk wget
    char *argz[] = {"wget", "--no-check-certificate", "-O", output_name, url, NULL};

    pid_t pid;
    pid = fork();

    if (pid == 0)
    {
        // child process: mengksekusi wget command
        execv("/usr/bin/wget", argz);
        perror("wget failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // parent process: menunggu child process selesai
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            printf("Download zip dari link berhasil\n");
        }
        else
        {
            printf("Download zip dari link gagal\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // gagal membuat fork
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    // mengembalikan nama file yang sudah di download
    return strdup(output_name);
}
```
- Menggunakan `fork()` untuk mendownload file .zip tersebut
- Array `*argz[]` digunakan untuk menyimpan argumen dalam perintah `wget`
- `--no-check-certificate` untuk mengabaikan sertifikat SSL
- `-O` untuk menentukan nama file output dari parameter `output_name`
- Jika `pid == 0` maka akan dijalankan perintah `execv` untuk menjalankan perintah `wget` dengan argumen yang telah disiapkan sebelumnya
- Apabila child berjalan dengan normal, maka `pid > 0` menampilkan output bahwa zip telah berhasil didownload.  Hasil ini merupakan proses parent yang menunggu proses child selesai dengan `waitpid`
- `strdup(output_name)` untuk mengembalikan nama file yang sudah diunduh untuk mengalokasikan memori baru dan menyalin string `output_name` ke memori tersebut

_3. Menyiapkan fungsi untuk membuat directory sebagai tempat file hasil ekstrak .zip_
```C
void create_directory(const char *path)
{
    // cek apakah directory sudah ada
    struct stat st;
    if (stat(path, &st) == -1)
    {
        // jika directory belum ada, maka buat directory
        if (mkdir(path, 0777) == -1)
        {
            perror("Gagal membuat directory");
            exit(EXIT_FAILURE);
        }
        printf("Membuat direktori: %s\n", path);
    }
}
```
- Fungsi `stat` digunakan untuk mengecek apakah direktori sudah ada. Hasilnya akan disimpan dalam struktur `st`
- Jika `stat` mengembalikan nilai -1, maka fungsi `mkdir` dipanggil untuk membuat direktori sesuai path yang diberikan dengan permission `0777` 

_4. Membuat fungsi untuk ekstrak/unzip file .zip yang sudah didownload sebelumnya. Hasil zip langsung dipindahkan dalam direktori "task"_
```C
void unzip_file(char *filename, char *path)
{
    // membuat array argumen untuk unzip
    char *arguments[] = {"unzip", "-o", "-q", filename, "-d", path, NULL};

    pid_t pid;
    pid = fork();

    if (pid == 0)
    {
        // child process: mengksekusi unzip command
        execv("/usr/bin/unzip", arguments);
        perror("unzip failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // parent process: menunggu child process selesai
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            printf("Unzip berhasil\n");
        }
        else
        {
            printf("Unzip gagal\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // gagal membuat fork
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
}
```
- Menggunakan `fork()` untuk ekstrak file .zip tersebut
- Array `*arguments[]` digunakan untuk menyimpan argumen dalam perintah `unzip`
- `-o` untuk melewati konfirmasi overwrite
- `-q` untuk mode quiet agar tidak menampilkan output yang berlebihan
- `-d` untuk menentukan direktori output
- Jika `pid == 0` maka akan dijalankan perintah `execv` untuk menjalankan perintah `unzip` dengan argumen yang telah disiapkan sebelumnya
- Apabila child berjalan dengan normal, maka `pid > 0` menampilkan output bahwa unzip telah berhasil. Hasil ini merupakan proses parent yang menunggu proses child selesai dengan `waitpid`

_5. Setelah zip diekstrak dan hasilnya dipindahkan dalam direktori "task", dilanjutkan untuk mencari file berekstensi .txt. Dalam hal ini saya tidak perlu mengecek awal penamaan apakah task_id karena sudah dipastikan bahwa file dengan ekstensi .txt pasti memiliki format penamaan tersebut_
```C
void search_task(const char *path)
{
    DIR *dir;           // pointer ke directory
    struct dirent *ent; // pointer ke file/directory
    const char *ori_path = "/home/hilmifawwaz/sisop/praktikum-modul-2-d04/task-2/task";

    // membuka directory
    if ((dir = opendir(path)) != NULL)
    {
        // membaca semua file/directory dalam directory
        while ((ent = readdir(dir)) != NULL)
        {
            // cek apakah nama file bukan "." dan "..", jika bukan maka lanjutkan
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
            {
                char full_path[PATH_MAX];
                snprintf(full_path, sizeof(full_path), "%s/%s", path, ent->d_name);

                // cek apakah file adalah directory, jika iya maka rekursif panggil fungsi search_task
                if (ent->d_type == DT_DIR)
                {
                    search_task(full_path);
                    // menghapus directory jika sudah selesai
                    if (rmdir(full_path) != 0)
                    {
                        perror("Gagal menghapus directory");
                    }
                    else
                    {
                        printf("Menghapus directory: %s\n", full_path);
                    }
                }
                else
                {
                    // cek apakah file adalah .txt, jika iya maka pindahkan file ke folder task
                    if (strstr(ent->d_name, ".txt") != NULL)
                    {
                        // membuat array argumen untuk mv
                        char new_path[PATH_MAX];
                        snprintf(new_path, sizeof(new_path), "%s/%s", ori_path, ent->d_name); // path tujuan file
                        if (rename(full_path, new_path) != 0)
                        {
                            perror("Gagal memindahkan file");
                        }
                        else
                        {
                            printf("Memindahkan file: %s\n", new_path);
                        }
                    }
                    else
                    {
                        // jika bukan file .txt, maka hapus file
                        if (remove(full_path) != 0)
                        {
                            perror("Gagal menghapus file");
                        }
                        else
                        {
                            printf("Menghapus file: %s\n", full_path);
                        }
                    }
                }
            }
        }
        closedir(dir);
    }
    else
    {
        perror("Gagal membuka directory");
        exit(EXIT_FAILURE);
    }
}
```
- Program tersebut digunakan mencari file yang sesuai (.txt) dengan cara mengecek secara rekursif setiap direktori yang ada
- Jika proses rekursif selesai, direktori akan dihapus menggunakan `rmdir`
- Selama proses rekursif, apabila menemukan entri bukan sebuah direktori, maka dilakukan pengecekan. Jika hasilnya adalah sebuah file berekstensi `.txt` maka dipindahkan ke dalam direktori tujuan menggunakan `snprintf(new_path, sizeof(new_path), "%s/%s", ori_path, ent->d_name);`
- Jika bukan file berkestensi `.txt`, maka file akan dihapus menggunakan fungsi `remove()`

_6. Dokumentasi. Dokumentasi ini saya lakukan satu per satu fungsi agar terlihat hasilnya. Tetapi, aslinya dalam program ini seharusnya hanya dijalankan sekali saja untuk semua proses_
- Download file .zip dari google drive
![alt text](/resource/2a-1.png)
- Membuat direktori "task" untuk menyimpan hasil ekstrak file .zip
![alt text](/resource/2a-2.png)
- Proses ekstrak/unzip file .zip sekaligus outputnya dipindahkan ke dalam direktori "task"
![alt text](/resource/2a-3.png)
- Menghapus file .zip yang di download
![alt text](/resource/2a-4.png)
- Mencari file .txt yang sesuai
![alt text](/resource/2a-5.png)

### Problem 2b
Yuan dan Bubu membagi tugas secara efisien dengan mengerjakannya secara bersamaan (overlapping) dan membaginya sama banyak. Yuan akan mengerjakan task dari awal, sementara Bubu dari akhir. Misalnya, jika ada 20 task, Yuan akan memulai dari task0-task1-dst dan Bubu akan memulai dari task19-task18-dst. Lalu buatlah file “recap.txt” yang menyimpan log setiap kali mereka selesai melakukan task (kriteria setiap task akan dijelaskan di poin c).

**Jawab**

_1. Mempersiapkan thread pada Main Function_
```C
int main()
{
    // SOAL POIN A
    char *url = "https://drive.google.com/uc?export=download&id=1CdWKLXVAc6P26sF-5oSrlVAsSExA1PQh";
    char *output_filename = "task_sisop.zip";
    char *path = "task";
    pthread_t yuan_thread, bubu_thread; // thread identifier
    
    .....
    
    // SOAL POIN B, C
    // membuat thread
    if (pthread_create(&yuan_thread, NULL, yuan, NULL) != 0)
    {
        fprintf(stderr, "Error creating thread\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&bubu_thread, NULL, bubu, NULL) != 0)
    {
        fprintf(stderr, "Error creating thread\n");
        exit(EXIT_FAILURE);
    }

    // menunggu thread selesai
    pthread_join(yuan_thread, NULL);
    pthread_join(bubu_thread, NULL);

    .....
```
- `pthread_t yuan_thread, bubu_thread;` untuk menampung identitas thread yang akan dibuat
- `pthread_create(&yuan_thread, NULL, yuan, NULL)` untuk membuat thread baru untuk memanggil fungsi `yuan` sebagai fungsi yang akan dieksekusi oleh thread tersebut
- `pthread_create(&bubu_thread, NULL, bubu, NULL)` untuk membuat thread baru yang memanggil fungsi `bubu` sebagai fungsi yang akan dieksekusi oleh thread tersebut
- `pthread_join` digunakan untuk menggabungkan thread lain. Fungsi ini akan memblokir eksekusi program hingga thread tersebut selesai. Jadi, program dapat menjalankan dua atau lebih tugas secara bersamaan (paralel)

_2. Membuat fungsi yuan untuk menjalankan tugasnya_
```C
void *yuan(void *arg)
{
    // membuat directory Yuan
    char *yuan_path = "/home/hilmifawwaz/sisop/praktikum-modul-2-d04/task-2/task/Yuan";

    // cek apakah directory sudah ada
    if (access(yuan_path, F_OK) == -1)
    {
        create_directory(yuan_path);
        printf("Folder Yuan dibuat\n");
    }
    else
    {
        printf("Folder 'Yuan' sudah ada!!!\n");
    }

    // menjalankan task untuk Yuan
    work_task_yuan(yuan_path);

    printf("Tugas Yuan Selesai\n");

    pthread_exit(0); // mengakhiri dan keluar dari thread
}
```
- `char *yuan_path` menyimpan path dari direktori yang akan dibuat untuk tugas yang berkaitan dengan Yuan
- `access(yuan_path, F_OK)` digunakan untuk memeriksa apakah direktori dengan path yang ditentukan sudah ada. Jika belum ada, fungsi `create_directory` yang saya jelaskan pada 2a dipanggil untuk membuat direktori tersebut
- Fungsi `work_task_yuan(yuan_path)` dipanggil menjalankan tugas yang berkaitan dengan Yuan
- `pthread_exit(0);` digunakan untuk mengakhiri thread saat ini dan keluar dari thread

_3. Membuat fungsi untuk download gambar sesuai tugas Yuan. Fungsi ini dipanggil dari fungsi yuan pada nomor 2_
```C
void work_task_yuan(const char *yuan_path)
{
    DIR *dir;                                                                           // pointer ke directory
    struct dirent *ent;                                                                 // pointer ke file/directory
    const char *ori_path = "/home/hilmifawwaz/sisop/praktikum-modul-2-d04/task-2/task"; // path asal
    int num_tasks = 0;                                                                  // counter task
    int num_files = 0;                                                                  // counter file
    char file_names[100][PATH_MAX];                                                     // array untuk menyimpan nama file

    // membuka directory
    if ((dir = opendir(ori_path)) != NULL)
    {
        // membaca semua file/directory dalam directory
        while ((ent = readdir(dir)) != NULL)
        {
            // cek apakah nama file dimulai dengan "task_", jika iya maka lanjutkan
            if (strncmp(ent->d_name, "task_", 5) == 0)
            {
                // mengambil nomor task dari nama file 
                int current_task_number = atoi(ent->d_name + 5);

                // cek apakah nomor task berada di range 0-9, jika iya maka lanjutkan
                if (current_task_number >= 0 && current_task_number <= 9)
                {
                    // menyimpan nama file ke dalam array
                    snprintf(file_names[num_files], sizeof(file_names[num_files]), "%s", ent->d_name);
                    num_files++;
                }
            }
        }
        closedir(dir);

        // menampilkan jumlah file
        printf("Jumlah file yang ditemukan: %d\n", num_files);

        // mengurutkan nama file secara ascending menggunakan bubble sort
        for (int i = 0; i < num_files - 1; i++)
        {
            for (int j = 0; j < num_files - i - 1; j++)
            {
                // mengambil nomor task dari nama file
                int task_number1 = atoi(file_names[j] + 5);
                int task_number2 = atoi(file_names[j + 1] + 5);
                // membandingkan nomor task, jika task_number1 > task_number2 maka tukar posisi
                if (task_number1 > task_number2)
                {
                    char temp[PATH_MAX];
                    strcpy(temp, file_names[j]);
                    strcpy(file_names[j], file_names[j + 1]);
                    strcpy(file_names[j + 1], temp);
                }
            }
        }

        // // menampilkan nama file yang sudah diurutkan
        // for (int i = 0; i < num_files; i++)
        // {
        //     printf("File %d: %s\n", i+1, file_names[i]);
        // }

        // mengakses nama file yang sudah diurutkan
        for (int i = 0; i < num_files; i++)
        {
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(file_names[i]), "%s/%s", ori_path, file_names[i]); // membangun full path file dengan menggabungkan path asal dan nama file
            printf("Membaca file: %s\n", full_path);

            // membuka file
            FILE *fp = fopen(full_path, "r");
            if (fp == NULL)
            {
                perror("Gagal membuka file");
                exit(EXIT_FAILURE);
            }
            else
            {
                // membaca dan memproses isi file
                int count;           // counter jumlah gambar yang akan didownload
                char resolution[20]; // resolusi gambar
                char categorie[100]; // kategori gambar

                fscanf(fp, "%d %s %[^\n]", &count, resolution, categorie); // membaca isi file

                printf("Count: %d\n", count);
                printf("Resolution: %s\n", resolution);
                printf("Categorie: %s\n", categorie);

                // membuat directory sesuai dengan nama file
                char dir_name[100]; // array untuk menyimpan nama directory

                // proses penaaman directory dari nama file
                char *file_name = strtok(file_names[i], "."); // memisahkan nama file dengan ekstensi
                char *underscore = strchr(file_name, '_');    // mencari underscore pertama pada nama file
                if (underscore != NULL)
                {
                    char *second_underscore = strchr(underscore + 1, '_'); // mencari underscore kedua pada nama file
                    if (second_underscore != NULL)
                    {
                        *second_underscore = '\0';
                    }
                }
                if (underscore != NULL)
                {
                    memmove(underscore, underscore + 1, strlen(underscore)); // menghapus underscore pertama
                }
                snprintf(dir_name, sizeof(dir_name), "%s", file_name); // menyimpan nama directory
                char dir_path[PATH_MAX];
                snprintf(dir_path, sizeof(dir_path), "%s/%s", yuan_path, dir_name); // membangun full path directory dengan menggabungkan path Yuan dan nama directory
                create_directory(dir_path);                                         // membuat directory

                pid_t pid;
                pid = fork();

                if (pid == 0)
                {
                    // child process: mengksekusi wget command
                    for (int i = 1; i <= count; i++)
                    {
                        char link[200];
                        snprintf(link, sizeof(link), "https://source.unsplash.com/random/%s/?%s", resolution, categorie); // membuat link download gambar

                        char filename[5000];                                                                 // array untuk menyimpan nama file gambar
                        snprintf(filename, sizeof(filename), "%s/gambar%d.png", dir_path, i);                // membangun full path file gambar
                        char *argv[] = {"wget", "-q", "--no-check-certificate", link, "-O", filename, NULL}; // array argumen untuk wget

                        pid_t pid;
                        pid = fork();

                        if (pid == 0)
                        {
                            // child process: mengksekusi wget command
                            execv("/usr/bin/wget", argv);
                            perror("wget failed");
                            exit(EXIT_FAILURE);
                        }
                        else if (pid > 0)
                        {
                            // parent process: menunggu child process selesai
                            int status;
                            waitpid(pid, &status, 0);
                            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                            {
                                printf("Download sukses: %s\n", filename);
                            }
                            else
                            {
                                printf("Download gagal: %s\n", filename);
                                exit(EXIT_FAILURE);
                            }
                        }
                        else
                        {
                            // gagal membuat fork
                            perror("Fork failed");
                            exit(EXIT_FAILURE);
                        }
                    }
                    exit(EXIT_SUCCESS); // keluar dari child process setelah semua file selesai didownload
                }
                else if (pid > 0)
                {
                    // parent process: menunggu child process selesai
                    int status;
                    waitpid(pid, &status, 0);

                    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                    {
                        printf("Horee! Download sukses\n");
                    }
                    else
                    {
                        printf("Yahh! Download gagal\n");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    // gagal membuat fork
                    perror("Fork failed");
                    exit(EXIT_FAILURE);
                }

                // memindahkan file ke directory yang ditentukan
                char new_file_path[PATH_MAX];
                snprintf(new_file_path, sizeof(new_file_path), "%s/%s", yuan_path, file_names[i]); // membangun full path file tujuan

                // cek apakah file berhasil dipindahkan
                if (rename(full_path, new_file_path) != 0)
                {
                    // jika file adalah directory, maka gunakan command "mv" untuk memindahkan
                    // EISDIR adalah error code untuk menandakan bahwa file adalah directory
                    if (errno == EISDIR)
                    {
                        char *arguments[] = {"mv", full_path, new_file_path, NULL}; // array argumen untuk mv

                        pid_t pid;
                        pid = fork();

                        if (pid == 0)
                        {
                            // child process: mengksekusi mv command
                            execv("/bin/mv", arguments);
                            perror("mv failed");
                            exit(EXIT_FAILURE);
                        }
                        else if (pid > 0)
                        {
                            // parent process: menunggu child process selesai
                            int status;
                            waitpid(pid, &status, 0);
                            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                            {
                                printf("Pemindahan file sukses\n");
                            }
                            else
                            {
                                printf("Pemindahan file gagal\n");
                                exit(EXIT_FAILURE);
                            }
                        }
                        else
                        {
                            // gagal membuat fork
                            perror("Fork failed");
                            exit(EXIT_FAILURE);
                        }
                    }
                    else
                    {
                        perror("Gagal memindahkan file");
                        exit(EXIT_FAILURE);
                    }
                }

                fclose(fp);  // menutup file
                num_tasks++; // increment counter task

                // merekap data ke file recap.txt
                recap_data("Yuan", file_name, count, categorie, resolution);
            }
        }
    }
}
```
- Sebenarnya, program ini gabungan dari problem 2c. Namun, karena saya langsung membuatnya dalam satu fungsi yang juga berjalan pada problem 2b, maka jawaban problem 2c akan menggunakan ini
- Penjelasan program secara singkat sudah terlihat pada comment di atas
- Karena pada proses thread task_{id} yang didownload diambil secara random, saya menggunakan array `char file_names[100][PATH_MAX]` untuk menyimpan nama file yang akan diurutkan secara ascending dari task_0 sampai task_9
- Setiap file yang namanya diawali `task_` akan diproses dan disimpan dalam array `file_names`. Pemrosesan akan dilakukan menggunakan bubble sort berdasarkan {id} pada nama file
- Setiap file yang sudah diurutkan akan diproses. Direktori baru akan dibuat berdasarkan nama file
- Setiap file akan dibuka dan isinya akan diproses
Untuk setiap file, akan dilakukan fork untuk melakukan download gambar menggunakan perintah `wget` dalam child process
- Setelah download selesai, file akan dipindahkan ke direktori yang sesuai. Recap data juga dilakukan ke dalam file "recap.txt" melalui fungsi `recap_data`

_4. Pada poin ini, saya hanya mengcopykan program saya karena memang program bubu mirip dengan program yuan. Yang membedakan hanya pada sorting pada fungsi `work_task_bubu` yang dilakukan secara descending dari task_19 sampai task_10_
```C
void *bubu(void *arg)
{
    // membuat directory Bubu
    char *bubu_path = "/home/hilmifawwaz/sisop/praktikum-modul-2-d04/task-2/task/Bubu";

    // cek apakah directory sudah ada
    if (access(bubu_path, F_OK) == -1)
    {
        create_directory(bubu_path);
        printf("Folder Bubu dibuat\n");
    }
    else
    {
        printf("Folder 'Bubu' sudah ada!!!\n");
    }

    // menjalankan task untuk Bubu
    work_task_bubu(bubu_path);

    printf("Tugas Bubu Selesai\n");

    pthread_exit(0); // mengakhiri dan keluar dari thread
}
```
```C
void work_task_bubu(const char *bubu_path)
{
    DIR *dir;                                                                           // pointer ke directory
    struct dirent *ent;                                                                 // pointer ke file/directory
    const char *ori_path = "/home/hilmifawwaz/sisop/praktikum-modul-2-d04/task-2/task"; // path asal
    int num_tasks = 0;                                                                  // counter task
    int num_files = 0;                                                                  // counter file
    char file_names[100][PATH_MAX];                                                     // array untuk menyimpan nama file

    // membuka directory
    if ((dir = opendir(ori_path)) != NULL)
    {
        // membaca semua file/directory dalam directory
        while ((ent = readdir(dir)) != NULL)
        {
            // mengambil nama file yang dimulai dengan "task_"
            int current_task_number = atoi(ent->d_name + 5);

            // cek apakah nomor task berada di range 10-19, jika iya maka lanjutkan
            if (current_task_number >= 10 && current_task_number <= 19)
            {
                // menyimpan nama file ke dalam array
                snprintf(file_names[num_files], sizeof(file_names[num_files]), "%s", ent->d_name);
                num_files++;
            }
        }
        closedir(dir);

        // menampilkan jumlah file
        printf("Jumlah file yang ditemukan: %d\n", num_files);

        // Mengurutkan nama file secara descending menggunakan bubble sort
        for (int i = 0; i < num_files - 1; i++)
        {
            for (int j = 0; j < num_files - i - 1; j++)
            {
                // mengambil nomor task dari nama file
                int task_number1 = atoi(file_names[j] + 5);
                int task_number2 = atoi(file_names[j + 1] + 5);
                // membandingkan nomor task, jika task_number1 < task_number2 maka tukar posisi
                if (task_number1 < task_number2)
                {
                    char temp[PATH_MAX];
                    strcpy(temp, file_names[j]);
                    strcpy(file_names[j], file_names[j + 1]);
                    strcpy(file_names[j + 1], temp);
                }
            }
        }

        // // menampilkan nama file yang sudah diurutkan
        // for (int i = 0; i < num_files; i++)
        // {
        //     printf("File %d: %s\n", i+1, file_names[i]);
        // }

        // mengakses nama file yang sudah diurutkan
        for (int i = 0; i < num_files; i++)
        {
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(file_names[i]), "%s/%s", ori_path, file_names[i]); // membangun full path file dengan menggabungkan path asal dan nama file
            printf("Membaca file: %s\n", full_path);

            // membuka file
            FILE *fp = fopen(full_path, "r");
            if (fp == NULL)
            {
                perror("Gagal membuka file");
                exit(EXIT_FAILURE);
            }
            else
            {
                // membaca dan memproses isi file
                int count;           // counter jumlah gambar yang akan didownload
                char resolution[20]; // resolusi gambar
                char categorie[100]; // kategori gambar

                fscanf(fp, "%d %s %[^\n]", &count, resolution, categorie); // membaca isi file

                printf("Count: %d\n", count);
                printf("Resolution: %s\n", resolution);
                printf("Categorie: %s\n", categorie);

                // membuat directory sesuai dengan nama file
                char dir_name[100]; // array untuk menyimpan nama directory

                // proses penaaman directory dari nama file
                char *file_name = strtok(file_names[i], "."); // memisahkan nama file dengan ekstensi
                char *underscore = strchr(file_name, '_');    // mencari underscore pertama pada nama file
                if (underscore != NULL)
                {
                    char *second_underscore = strchr(underscore + 1, '_'); // mencari underscore kedua pada nama file
                    if (second_underscore != NULL)
                    {
                        *second_underscore = '\0';
                    }
                }
                if (underscore != NULL)
                {
                    memmove(underscore, underscore + 1, strlen(underscore)); // menghapus underscore pertama
                }
                
                snprintf(dir_name, sizeof(dir_name), "%s", file_name); // menyimpan nama directory
                char dir_path[PATH_MAX];
                snprintf(dir_path, sizeof(dir_path), "%s/%s", bubu_path, dir_name); // membangun full path directory dengan menggabungkan path Bubu dan nama directory
                create_directory(dir_path);                                         // membuat directory

                pid_t pid;
                pid = fork();

                if (pid == 0)
                {
                    // child process: mengksekusi wget command
                    for (int i = 1; i <= count; i++)
                    {
                        char link[200];
                        snprintf(link, sizeof(link), "https://source.unsplash.com/random/%s/?%s", resolution, categorie); // membuat link download gambar

                        char filename[5000];                                                                 // array untuk menyimpan nama file gambar
                        snprintf(filename, sizeof(filename), "%s/gambar%d.png", dir_path, i);                // membangun full path file gambar
                        char *argv[] = {"wget", "-q", "--no-check-certificate", link, "-O", filename, NULL}; // array argumen untuk wget

                        pid_t pid;
                        pid = fork();

                        if (pid == 0)
                        {
                            // child process: mengksekusi wget command
                            execv("/usr/bin/wget", argv);
                            perror("wget failed");
                            exit(EXIT_FAILURE);
                        }
                        else if (pid > 0)
                        {
                            // parent process: menunggu child process selesai
                            int status;
                            waitpid(pid, &status, 0);
                            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                            {
                                printf("Download sukses: %s\n", filename);
                            }
                            else
                            {
                                printf("Download gagal: %s\n", filename);
                                exit(EXIT_FAILURE);
                            }
                        }
                        else
                        {
                            // gagal membuat fork
                            perror("Fork failed");
                            exit(EXIT_FAILURE);
                        }
                    }
                    exit(EXIT_SUCCESS); // keluar dari child process setelah semua file selesai didownload
                }
                else if (pid > 0)
                {
                    // parent process: menunggu child process selesai
                    int status;
                    waitpid(pid, &status, 0);

                    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                    {
                        printf("Horee! Download sukses\n");
                    }
                    else
                    {
                        printf("Yahh! Download failed\n");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    // gagal membuat fork
                    perror("Fork failed");
                    exit(EXIT_FAILURE);
                }

                // memindahkan file ke directory yang ditentukan
                char new_file_path[PATH_MAX];
                snprintf(new_file_path, sizeof(new_file_path), "%s/%s", bubu_path, file_names[i]); // membangun full path file tujuan

                // cek apakah file berhasil dipindahkan
                if (rename(full_path, new_file_path) != 0)
                {
                    // jika file adalah directory, maka gunakan command "mv" untuk memindahkan
                    // EISDIR adalah error code untuk menandakan bahwa file adalah directory
                    if (errno == EISDIR)
                    {
                        char *arguments[] = {"mv", full_path, new_file_path, NULL}; // array argumen untuk mv

                        pid_t pid;
                        pid = fork();

                        if (pid == 0)
                        {
                            // child process: mengksekusi mv command
                            execv("/bin/mv", arguments);
                            perror("mv failed");
                            exit(EXIT_FAILURE);
                        }
                        else if (pid > 0)
                        {
                            // parent process: menunggu child process selesai
                            int status;
                            waitpid(pid, &status, 0);
                            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                            {
                                printf("Pemindahan file sukses\n");
                            }
                            else
                            {
                                printf("Pemindahan file gagal\n");
                                exit(EXIT_FAILURE);
                            }
                        }
                        else
                        {
                            // gaga membuat fork
                            perror("Fork failed");
                            exit(EXIT_FAILURE);
                        }
                    }

                    else
                    {

                        perror("Failed to move file");
                        exit(EXIT_FAILURE);
                    }
                }

                fclose(fp);  // menutup file
                num_tasks++; // increment counter task

                // merekap data ke file recap.txt
                recap_data("Bubu", file_name, count, categorie, resolution);
            }
        }
    }
}
```

_5. Membuat fungsi recap_data untuk menyimpan log setiap kali mereka selesai mengerjakan task_
```C
void recap_data(const char *name, const char *task, int count, const char *categorie, const char *resolution)
{
    // waktu sekarang
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    // mengkonversi nama task
    char task_name[100];
    strcpy(task_name, task);
    char *capital = strstr(task_name, "task"); // mencari kata "task" pada task_name
    if (capital != NULL)
    {
        *capital = 'T'; // mengganti huruf pertama dari "task" menjadi "T"
    }
    task_name[0] = toupper(task_name[0]); // mengubah huruf pertama menjadi huruf besar

    // memisahkan angka dari kata "task"
    char *number = task_name + 4; // mengambil angka setelah kata "task"
    int num = atoi(number); // mengubah string angka menjadi integer
    sprintf(task_name + 4, " %d", num); // menambahkan spasi dan angka setelah kata "Task"

    // menambahkan data ke file recap.txt
    FILE *file = fopen("recap.txt", "a");
    if (file == NULL)
    {
        perror("Gagal membuka file recap.txt");
        exit(EXIT_FAILURE);
    }

    // menambahkan data ke file recap.txt
    fprintf(file, "[%s]-[%s] %s completed, download %d %s images with %s resolution\n", timestamp, name, task_name, count, categorie, resolution);

    fclose(file);
}
```
- Fungsi `time()` digunakan untuk mendapatkan waktu saat ini dalam bentuk detik sejak epoch (00:00:00 UTC, 1 Januari 1970)
- Fungsi `localtime()` digunakan untuk mengonversi waktu yang diperoleh menjadi struktur tm yang menyimpan informasi waktu dalam format lokal
- Fungsi `strftime()` digunakan untuk memformat waktu ke dalam string dengan format tertentu, dalam hal ini digunakan format "YYYY-MM-DD HH:MM:SS"
- Proses konversi nama task yang awalnya "task1" menjadi "Task 1" untuk digunakan dalam recap data
- `fopen("recap.txt", "a")` untuk membuka file "recap.txt" dalam mode "a". Menggunakan `fprintf()` untuk menambahkan data baru ke akhir file recap.txt

_6. Dokumentasi_
- Hasil pemrosesan thread
![alt text](/resource/2b-1.png)
- Isi direktori yuan
![alt text](/resource/2b-2.png)
- Isi direktori bubu
![alt text](/resource/2b-3.png)
- Contoh isi direktori task8
![alt text](/resource/2b-4.png)
- Isi file recap.txt
```txt
[2024-04-21 22:57:00]-[Yuan] Task 0 completed, download 1 music images with 300x300 resolution
[2024-04-21 22:57:01]-[Yuan] Task 1 completed, download 1 nature images with 200x150 resolution
[2024-04-21 22:57:04]-[Bubu] Task 19 completed, download 4 city images with 250x300 resolution
[2024-04-21 22:57:06]-[Bubu] Task 18 completed, download 1 nature images with 250x150 resolution
[2024-04-21 22:57:07]-[Bubu] Task 17 completed, download 1 minimalist images with 150x250 resolution
[2024-04-21 22:57:08]-[Yuan] Task 2 completed, download 4 night images with 200x150 resolution
[2024-04-21 22:57:10]-[Yuan] Task 3 completed, download 1 fruit images with 200x150 resolution
[2024-04-21 22:57:13]-[Bubu] Task 16 completed, download 3 face images with 250x150 resolution
[2024-04-21 22:57:14]-[Yuan] Task 4 completed, download 3 night images with 300x250 resolution
[2024-04-21 22:57:25]-[Yuan] Task 5 completed, download 5 music images with 250x150 resolution
[2024-04-21 22:57:28]-[Bubu] Task 15 completed, download 4 city images with 200x200 resolution
[2024-04-21 22:57:36]-[Bubu] Task 14 completed, download 4 summer images with 250x200 resolution
[2024-04-21 22:57:39]-[Bubu] Task 13 completed, download 2 face images with 150x200 resolution
[2024-04-21 22:57:44]-[Bubu] Task 12 completed, download 3 minimalist images with 100x250 resolution
[2024-04-21 22:57:47]-[Yuan] Task 6 completed, download 5 minimalist images with 300x100 resolution
[2024-04-21 22:57:54]-[Yuan] Task 7 completed, download 5 fruit images with 200x250 resolution
[2024-04-21 22:58:01]-[Yuan] Task 8 completed, download 4 fruit images with 200x300 resolution
[2024-04-21 22:58:02]-[Bubu] Task 11 completed, download 2 fruit images with 150x250 resolution
[2024-04-21 22:58:08]-[Bubu] Task 10 completed, download 3 nature images with 300x200 resolution
[2024-04-21 22:58:29]-[Yuan] Task 9 completed, download 4 summer images with 200x250 resolution
```

### Problem 2c
Yuan dan Bubu menemukan petunjuk dalam setiap file task txt dengan format "{jumlah} {resolusi} {kategori}". Mereka harus mengunduh gambar sesuai dengan jumlah, resolusi, dan kategori yang disebutkan. Contohnya, jika task0 berisi "5 250x150 music", mereka mengunduh 5 gambar berukuran 250x150 dalam kategori music. Gambar diunduh dari unsplash.com dan disimpan bersama file txt setiap task berdasarkan orang yang mengerjakan task.

**Jawab**

Jawaban no 2c sudah include pada jawaban 2b. Dimulai pada poin 3 dan seterusnya.

### Problem 2d
Setelah semua tugas selesai, mereka menambahkan jumlah gambar dari setiap kategori ke dalam "recap.txt". dan pada baris terakhir tambahkan jumlah seluruh gambar yg di unduh.

**Jawab**

_1. Membuat fungsi total_recap_data yang digunakan untuk menjumlahkan semua gambar yang diunduh dan yang sesuai kategorinya. Tetapi sebelum itu, pada Main Function jangan lupa untuk memanggil fungsi total_recap_data_
```C
int main()
{
    .....

    // SOAL POIN D
    total_recap_data();

    .....
    return 0;
}
```
```C
void total_recap_data()
{
    FILE *file = fopen("recap.txt", "r");
    if (file == NULL)
    {
        perror("Gagal untuk membuka recap.txt");
        exit(EXIT_FAILURE);
    }

    // inisialisasi variabel
    int nature = 0;
    int night = 0;
    int fruit = 0;
    int music = 0;
    int city = 0;
    int summer = 0;
    int minimalist = 0;
    int face = 0;
    int total_images = 0;

    // membaca setiap baris file recap.txt
    char line[100];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        int count;

        // mencari kata kunci pada baris
        if (strstr(line, "nature images"))
        {
            sscanf(line, "[%*[^]]]-[%*[^]]] Task %*d completed, download %d nature images", &count);
            nature += count;
        }
        else if (strstr(line, "night images"))
        {
            sscanf(line, "[%*[^]]]-[%*[^]]] Task %*d completed, download %d night images", &count);
            night += count;
        }
        else if (strstr(line, "fruit images"))
        {
            sscanf(line, "[%*[^]]]-[%*[^]]] Task %*d completed, download %d fruit images", &count);
            fruit += count;
        }
        else if (strstr(line, "music images"))
        {
            sscanf(line, "[%*[^]]]-[%*[^]]] Task %*d completed, download %d music images", &count);
            music += count;
        }
        else if (strstr(line, "city images"))
        {
            sscanf(line, "[%*[^]]]-[%*[^]]] Task %*d completed, download %d city images", &count);
            city += count;
        }
        else if (strstr(line, "summer images"))
        {
            sscanf(line, "[%*[^]]]-[%*[^]]] Task %*d completed, download %d summer images", &count);
            summer += count;
        }
        else if (strstr(line, "minimalist images"))
        {
            sscanf(line, "[%*[^]]]-[%*[^]]] Task %*d completed, download %d minimalist images", &count);
            minimalist += count;
        }
        else if (strstr(line, "face images"))
        {
            sscanf(line, "[%*[^]]]-[%*[^]]] Task %*d completed, download %d face images", &count);
            face += count;
        }
    }

    // menghitung total images
    total_images = nature + night + fruit + music + city + summer + minimalist + face;

    fclose(file);

    // append data ke file recap.txt
    FILE *append_file = fopen("recap.txt", "a");
    if (append_file == NULL)
    {
        perror("Gagal membuka file recap.txt untuk menambahkan data");
        exit(EXIT_FAILURE);
    }

    // menambahkan data ke file recap.txt
    fprintf(append_file, "\nnature: %d images\n", nature);
    fprintf(append_file, "night: %d images\n", night);
    fprintf(append_file, "fruit: %d images\n", fruit);
    fprintf(append_file, "music: %d images\n", music);
    fprintf(append_file, "city: %d images\n", city);
    fprintf(append_file, "summer: %d images\n", summer);
    fprintf(append_file, "minimalist: %d images\n", minimalist);
    fprintf(append_file, "face: %d images\n", face);
    fprintf(append_file, "total images: %d images", total_images);

    fclose(append_file);
}
```
- Membuka file "recap.txt" dalam mode baca ("r") menggunakan fungsi `fopen("recap.txt", "r")`
- Karena kita sudah tau apa saja kategori yang didownload, kita dapat langsung melakukan inisialisasi variabel untuk merekam jumlah gambar yang terunduh untuk setiap kategori dan total keseluruhan
- `fgets` untuk membaca setiap baris dari file "recap.txt". Fungsi ini akan mencari keyword yang menandakan enis gambar yang terunduh dan mengambil jumlah gambar yang terunduh untuk setiap kategori tersebut
- Membuka kembali file "recap.txt" dalam mode append ("a") untuk menambahkan data baru berupa jumlah gambar untuk setiap kategori dan jumlah gambar keseluruhan

_2. Dokumentasi. Dokumentasi ini terekam dalam file "recap.txt" pada bagian bawah_
```txt
nature: 5 images
night: 7 images
fruit: 12 images
music: 6 images
city: 8 images
summer: 8 images
minimalist: 9 images
face: 5 images
total images: 60 images
```

### Problem 2e
Karena tugas sudah siap dikumpulkan, zip folder Yuan dan Bubu menjadi satu zip bernamakan submission.zip dan JANGAN HAPUS folder aslinya.

**Jawab**

_1. Membuat fungsi untuk mengcopy file "recap.txt" dan zip direktori "task" Tetapi sebelum itu, pada Main Function jangan lupa untuk memanggil fungsi finally()_
```C
int main()
{
    .....
    
    // SOAL POIN E
    finally();

    return 0;
}
```
```C
void finally()
{
    // copy file recap.txt ke folder task
    char *copy_command[] = {"cp", "recap.txt", "task/", NULL};

    pid_t pid;
    pid = fork();

    if (pid == 0)
    {
        // child process: mengksekusi cp command
        execv("/bin/cp", copy_command);
        perror("Copy failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // parent process: menunggu child process selesai
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            printf("Copy sukses\n");
        }
        else
        {
            printf("Copy gagal\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // gagal membuat fork
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    // zip folder task
    char *argz[] = {"zip", "-r", "task/submission.zip", "task/", NULL};

    pid = fork();

    if (pid == 0)
    {
        // child process: mengksekusi zip command
        execv("/usr/bin/zip", argz);
        perror("zip failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // parent process: menunggu child process selesai
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            printf("Zip sukses\n");
        }
        else
        {
            printf("Zip gagal\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // gagal membuat fork
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
}
```
- Menggunakan `fork()` untuk copy file dan zip direktori
- Karena pada contoh file "recap.txt" juga ikut di zip, maka saya mengcopy file tersebut ke dalam direktori "task" menggunakan command-line utility `cp`
- Setelah file di copy, maka dilanjutkan proses zip direktori
- Array `argz` dibuat yang berisi argumen untuk command `zip`
- Di dalam child process, fungsi `execv` digunakan untuk mengeksekusi command `zip` dengan argumen yang telah disiapkan sebelumnya

_2. Dokumentasi_
- Isi direktori "task"
![alt text](/resource/2e-1.png)


### Kendala

Kendala yang dialami ketika mengerjakan nomor 2 adalah kegagalan dalam mengurutkan tugas yuan dan bubu. Sebelum saya menggunakan array untuk mengurutkan proses pengerjaan task, thread mengerjakan task secara random/acak dan tidak sesuai dengan apa yang soal inginkan. Akhirnya, saya mengakali dengan menggunakan array untuk mengurutkan proses pendownloadan gambar. Yuan menggunakan ascending, sedangkan bubu menggunakan descending dan berhasil.

## 3️⃣ Soal 3
Rama adalah seorang mahasiswa yang mengambil mata kuliah Sistem Operasi. Ia diberikan proyek oleh dosennya untuk mengelola data cuaca dari berbagai kecamatan di Indonesia. Data tersebut tersedia dalam file zip yang bisa didownload disini. Karena suka tantangan, Rama melakukan analisis dengan menggunakan process di linux. Bantulah Rama untuk menyelesaikan tugas ini dengan satu program C bernama weather_manager.c.

**Jawab**
```C
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <string.h>

#define LOG_FILE "logbook.txt"
#define ZIP_FILE "weather_data.zip"
#define WEATHER_FOLDER "weather"
#define LOG_FORMAT "[%s] %s\n"

void logbook(const char *message) {
    time_t current_time;
    struct tm *timeinfo;
    char timestamp[80];

    time(&current_time);
    timeinfo = localtime(&current_time);

    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", timeinfo);

    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }
    fprintf(log_file, LOG_FORMAT, timestamp, message);
    fclose(log_file);
}

void preprocessing() {
    FILE *input_file = fopen("weather/kecamatanforecast.csv", "r");
    FILE *temp_file = fopen("weather/kecamatanforecast_temp.csv", "w");
    if (input_file == NULL || temp_file == NULL) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    char line[512]; // Assuming maximum line length is 512 characters
    fgets(line, sizeof(line), input_file); // Read and ignore header line
    fputs(line, temp_file);

    while (fgets(line, sizeof(line), input_file)) {
        char temp_line[512];
        strcpy(temp_line, line);  // Create a temporary copy of the line for printing

        char *token = strtok(line, ",");
        int column = 1;
        char suhu_udara[10];
        char parameter_udara[10];
        char value_suhu_udara[10];
        while (token != NULL) {
            // Trim newline characters from the token
            char *newline = strchr(token, '\n');
            if (newline != NULL) {
                *newline = '\0';
            }

            if (column == 4) { // Adjust column index to capture "Suhu Udara"
                strcpy(suhu_udara, token);
                strcpy(value_suhu_udara, suhu_udara);
            }
            if (column == 1) { // Adjust column index to capture parameter
                strcpy(parameter_udara, token);
            }
            column++;
            token = strtok(NULL, ",");
        }
        float value_suhu = atof(suhu_udara);
        if (value_suhu <= 35) {
            // Print the line only if suhu <= 35
            // printf("%s", temp_line);

            // Write the line to the temporary file
            fputs(temp_line, temp_file);
        } else {
            char log_message[100];
            sprintf(log_message, "Successfully drop row with parameter %s value %s", parameter_udara, value_suhu_udara);
            logbook(log_message);
            // Skip writing the line to the temporary file
        }
    }

    fclose(input_file);
    fclose(temp_file);

    remove("weather/kecamatanforecast.csv");
    rename("weather/kecamatanforecast_temp.csv", "weather/kecamatanforecast.csv");

    logbook("Data preprocessing completed.");
}



void grouping() {
    // Create folder "city_group" if it doesn't exist
    mkdir("city_group", 0777);

    // Open the preprocessed dataset
    FILE *input_file = fopen("weather/kecamatanforecast.csv", "r");
    if (input_file == NULL) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    char line[512]; // Assuming maximum line length is 512 characters
    fgets(line, sizeof(line), input_file); // Read and ignore header line

    FILE *file_map[10000] = {NULL}; // Array to store file pointers based on loc_id index

    while (fgets(line, sizeof(line), input_file)) {
        if (strlen(line) > 0) {
            char temp_line[sizeof(line)];
            strcpy(temp_line, line);

            char *loc_id = strtok(temp_line, ",");
            if (loc_id == NULL) {
                continue;
            }

            int index = atoi(loc_id) % 10000;

            if (!file_map[index]) {
                char grouped_file_name[256];
                snprintf(grouped_file_name, sizeof(grouped_file_name), "city_group/city_%s.csv", loc_id);
                file_map[index] = fopen(grouped_file_name, "a");
                if (!file_map[index]) {
                    perror("Error opening grouped file");
                    continue;
                }
                // Write the header line for the new city file
                fputs("0,1,6,7,8,9,10\n", file_map[index]); // Assuming the header line format
            }
            // Write the entire line to the corresponding city file
            fputs(line, file_map[index]);
        }
    }

    // Close all opened city files
    for (int i = 0; i < 10000; i++) {
        if (file_map[i]) {
            fclose(file_map[i]);
        }
    }

    fclose(input_file);

    // Log the successful grouping of data
    logbook("Successfully grouped data.\n");
}

int main() {
    pid_t pid;
    int status;

    pid = fork();
    if (pid < 0) {
        perror("Error in fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execlp("wget", "wget", "-O", ZIP_FILE, "https://drive.google.com/uc?export=download&id=1U1-9v9pJeX5MoofwoBHCcD9nA7YWy_np", NULL);
        perror("Error in execlp");
        exit(EXIT_FAILURE);
    } else {
        wait(&status);
        if (WIFEXITED(status) && !WEXITSTATUS(status)) {
            logbook("Successfully download file..");
            pid = fork();
            if (pid < 0) {
                perror("Error in fork");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                execlp("unzip", "unzip", ZIP_FILE, "-d", WEATHER_FOLDER, NULL);
                perror("Error in execlp");
                exit(EXIT_FAILURE);
            } else {
                wait(&status);
                if (WIFEXITED(status) && !WEXITSTATUS(status)) {
                    remove(ZIP_FILE);
                    logbook("Successfully unzip file..");

                    // Perform data preprocessing after extracting the zip file
                    preprocessing();
                    grouping();
                }
            }
        }
    }

    return 0;
}
```

### Problem 3a
Rama harus mendownload dan menyimpan file tersebut ke dalam penyimpanan lokal dan mengekstrak data cuaca tersebut pada folder yang bernama “weather”. Untuk menghemat memori Rama menghapus file zip tersebut.

**Jawab**

```C

int main() {
    pid_t pid;  // Tipe data untuk menyimpan ID proses
    int status;  // Status untuk memantau hasil fork dan proses child

    pid = fork();  // Buat proses child
    if (pid < 0) {  // Jika fork gagal
        perror("Error in fork");  // Tampilkan pesan kesalahan
        exit(EXIT_FAILURE);  // Keluar jika gagal
    } else if (pid == 0) {  // Proses child
        execlp("wget", "wget", "-O", ZIP_FILE, "https://drive.google.com/uc?export=download&id=1U1-9v9pJeX5MoofwoBHCcD9nA7YWy_np", NULL);  // Unduh file zip
        perror("Error in execlp");  // Tampilkan pesan kesalahan jika gagal
        exit(EXIT_FAILURE);  // Keluar jika gagal
    } else {  // Proses parent
        wait(&status);  // Tunggu proses child selesai
        if (WIFEXITED(status) && !WEXITSTATUS(status)) {  // Jika proses child sukses
            logbook("Successfully download file.");  // Catat log berhasil mengunduh
            pid = fork();  // Buat proses child baru
            if (pid < 0) {
                perror("Error in fork");  // Tampilkan pesan kesalahan jika gagal
                exit(EXIT_FAILURE);  // Keluar jika gagal
            } else if (pid == 0) {  // Proses child
                execlp("unzip", "unzip", ZIP_FILE, "-d", WEATHER_FOLDER, NULL);  // Ekstrak file zip
                perror("Error in execlp");  // Tampilkan pesan kesalahan jika gagal
                exit(EXIT_FAILURE);  // Keluar jika gagal
            } else {  // Proses parent
                wait(&status);  // Tunggu proses child selesai
                if (WIFEXITED(status) && !WEXITSTATUS(status)) {  // Jika proses child sukses
                    remove(ZIP_FILE);  // Hapus file zip setelah diekstrak
                    logbook("Successfully unzip file.");  // Catat log berhasil mengekstrak
                    preprocessing();  // Lakukan pemrosesan data
                    grouping();  // Lakukan pengelompokan data
                }
            }
        }
    }

    return 0;  // Kembali ke sistem operasi
}
```

- Inisialisasi Variabel dan Fork Pertama

```C
pid_t pid;  // Tipe data untuk menyimpan ID proses
int status;  // Variabel untuk memantau status proses child

pid = fork();  // Buat proses child
if (pid < 0) {  // Jika fork gagal
    perror("Error in fork");  // Tampilkan pesan kesalahan
    exit(EXIT_FAILURE);  // Keluar jika gagal
} 
```
Di bagian ini, program membuat proses baru (child) menggunakan fungsi fork(). Jika fork() gagal, akan ditampilkan pesan kesalahan dan program keluar dengan EXIT_FAILURE. Jika berhasil, variabel pid akan mengandung ID proses child.

 - Proses child pertama
 ```C
    else if (pid == 0) {  // Jika ini proses child
    execlp("wget", "wget", "-O", ZIP_FILE, "https://drive.google.com/uc?export=download&id=1U1-9v9pJeX5MoofwoBHCcD9nA7YWy_np", NULL);  // Unduh file zip
    perror("Error in execlp");  // Tampilkan pesan kesalahan jika gagal
    exit(EXIT_FAILURE);  // Keluar jika gagal
}
 ``` 
 Jika pid adalah 0, berarti ini adalah proses child. Dalam kasus ini, program akan menjalankan execlp untuk mengunduh file dari URL yang diberikan menggunakan wget. Jika execlp gagal, akan ditampilkan pesan kesalahan, dan proses child keluar dengan EXIT_FAILURE.

- Proses Parent Menunggu Child Pertama
```C 
    else {  // Jika ini adalah proses parent
    wait(&status);  // Tunggu proses child selesai
    if (WIFEXITED(status) && !WEXITSTATUS(status)) {  // Jika child berhasil
        logbook("Successfully download file.");  // Catat log berhasil mengunduh
```
Jika pid bukan 0, berarti ini adalah proses parent. Proses parent menggunakan wait(&status) untuk menunggu proses child selesai. Jika child berhasil (diperiksa dengan WIFEXITED(status) dan !WEXITSTATUS(status)), akan dicatat log bahwa pengunduhan berhasil.

- Fork Kedua dan Proses Child Kedua
```C 
pid = fork();  // Buat proses child baru
if (pid < 0) {  // Jika fork gagal
    perror("Error in fork");  // Tampilkan pesan kesalahan jika gagal
    exit(EXIT_FAILURE);  // Keluar jika gagal
} else if (pid == 0) {  // Jika ini proses child
    execlp("unzip", "unzip", ZIP_FILE, "-d", WEATHER_FOLDER, NULL);  // Ekstrak file zip
    perror("Error in execlp");  // Tampilkan pesan kesalahan jika gagal
    exit(EXIT_FAILURE);  // Keluar jika gagal
}

```
Setelah pengunduhan berhasil, parent membuat child baru untuk mengekstrak file zip. Seperti sebelumnya, jika fork() gagal, program keluar dengan pesan kesalahan. Jika berhasil dan ini adalah proses child, execlp digunakan untuk menjalankan perintah unzip. Jika gagal, akan ada pesan kesalahan, dan proses child keluar.

- Parent Menunggu Child Kedua dan Tindakan Setelah Ekstraksi
```C 
else {  // Jika ini adalah proses parent
    wait(&status);  // Tunggu proses child selesai
    if (WIFEXITED(status) && !WEXITSTATUS(status)) {  // Jika child berhasil
        remove(ZIP_FILE);  // Hapus file zip setelah diekstrak
        logbook("Successfully unzip file.");  // Catat log berhasil mengekstrak
        preprocessing();  // Lakukan pemrosesan data
        grouping();  // Lakukan pengelompokan data
    }
}
```
Jika proses parent, ini akan menunggu proses child kedua selesai. Jika proses child kedua berhasil, file zip yang diekstrak akan dihapus dengan remove(ZIP_FILE). Log juga dicatat untuk menunjukkan keberhasilan ekstraksi. Setelah itu, fungsi preprocessing() dijalankan untuk melakukan pemrosesan data dan grouping() untuk pengelompokan data.

_Dokumentasi_

![alt text](/resource/3a-1.png)

### Problem 3b
Setelah berhasil, Rama melakukan data preprocessing dengan cara mencari data yang tidak masuk akal pada kolom “Suhu Udara” dan melakukan drop pada row tersebut. Lalu update dataset “kecamatanforecast.csv”.

**Jawab**

```C
    void preprocessing() {
    FILE *input_file = fopen("weather/kecamatanforecast.csv", "r");  // Buka file input
    FILE *temp_file = fopen("weather/kecamatanforecast_temp.csv", "w");  // Buka file sementara
    if (input_file == NULL || temp_file == NULL) {
        perror("Error opening input file");  // Tampilkan pesan kesalahan jika gagal
        exit(EXIT_FAILURE);  // Keluar jika gagal
    }

    char line[512];  // Buffer untuk membaca baris
    fgets(line, sizeof(line), input_file);  // Baca dan abaikan baris header
    fputs(line, temp_file);  // Tulis header ke file sementara

    while (fgets(line, sizeof(line), input_file)) {  // Baca setiap baris dari file input
        char temp_line[512];  // Buffer untuk mencatat baris sementara
        strcpy(temp_line, line);  // Buat salinan sementara dari baris

        char *token = strtok(line, ",");  // Tokenisasi dengan delimiter ","
        int column = 1;  // Variabel untuk melacak kolom
        char suhu_udara[10];
        char parameter_udara[10];
        char value_suhu_udara[10];
        
        while (token != NULL) {  // Iterasi melalui token
            char *newline = strchr(token, '\n');  // Temukan karakter newline dan hapus
            if (newline != NULL) {
                *newline = '\0';
            }

            if (column == 4) {  // Kolom suhu udara
                strcpy(suhu_udara, token);  // Simpan suhu udara
                strcpy(value_suhu_udara, suhu_udara);
            }
            if (column == 1) {  // Kolom parameter
                strcpy(parameter_udara, token);
            }
            column++;  // Naikkan indeks kolom
            token = strtok(NULL, ",");  // Ambil token berikutnya
        }

        float value_suhu = atof(suhu_udara);  // Konversi suhu udara menjadi float
        if (value_suhu <= 35) {  // Hanya tulis jika suhu kurang dari atau sama dengan 35
            fputs(temp_line, temp_file);  // Tulis baris ke file sementara
        } else {
            char log_message[100];  // Buffer untuk pesan log
            sprintf(log_message, "Successfully drop row with parameter %s value %s", parameter_udara, value_suhu_udara);  // Buat pesan log
            logbook(log_message);  // Catat pesan log
        }
    }

    fclose(input_file);  // Tutup file input
    fclose(temp_file);  // Tutup file sementara

    remove("weather/kecamatanforecast.csv");  // Hapus file lama
    rename("weather/kecamatanforecast_temp.csv", "weather/kecamatanforecast.csv");  // Ganti nama file sementara ke nama file asli

    logbook("Data preprocessing completed.");  // Catat log selesai pemrosesan
}
```

 - Bagian pembuka file
```C 
FILE *input_file = fopen("weather/kecamatanforecast.csv", "r");  // Buka file input
FILE *temp_file = fopen("weather/kecamatanforecast_temp.csv", "w");  // Buka file sementara
if (input_file == NULL atau temp_file == NULL) {
    perror("Error opening input file");  // Pesan kesalahan jika file tidak bisa dibuka
    exit(EXIT_FAILURE);  // Keluar jika gagal membuka file
}
```
Bagian ini membuka file CSV yang ingin diproses dan file sementara untuk menyimpan hasil pemrosesan. Jika salah satu file tidak dapat dibuka, program akan menampilkan pesan kesalahan dan berhenti.

 - Bagian Membaca dan Menulis Header
```C 
char line[512];  // Buffer untuk membaca baris
fgets(line, sizeof(line), input_file);  // Baca baris pertama (header)
fputs(line, temp_file);  // Tulis baris header ke file sementara
```
Bagian ini membaca baris pertama dari file CSV yang biasanya adalah header, dan menulisnya ke file sementara. Buffer berukuran 512 karakter digunakan untuk menyimpan data yang dibaca dari file.


 - Bagian pemrosesan baris
```C 
while (fgets(line, sizeof(line), input_file)) {  // Baca setiap baris dari file input
    char temp_line[512];  // Buffer untuk menyimpan baris sementara
    strcpy(temp_line, line);  // Buat salinan sementara dari baris

    char *token = strtok(line, ",");  // Tokenisasi baris menggunakan koma sebagai delimiter
    int column = 1;  // Variabel untuk melacak kolom
    char suhu_udara[10];  // Variabel untuk menyimpan suhu udara
    char parameter_udara[10];  // Variabel untuk menyimpan parameter udara
    char value_suhu_udara[10];  // Variabel untuk menyimpan nilai suhu udara


```
Bagian ini memulai loop untuk membaca setiap baris dari file input. Baris pertama dilewati karena sudah ditulis ke file sementara. Salinan baris dibuat untuk digunakan jika perlu menulis ulang ke file sementara. Kemudian, setiap baris dipecah (tokenisasi) berdasarkan koma, yang merupakan pemisah antar-kolom.


 - Bagian ekstraksi dan penyaringan data
```C 
    while (token != NULL) {  // Iterasi melalui setiap token
        char *newline = strchr(token, '\n');  // Hapus karakter newline jika ada
        if (newline != NULL) {
            *newline = '\0';
        }

        if (column == 4) {  // Jika ini adalah kolom ke-4 (suhu udara)
            strcpy(suhu_udara, token);  // Simpan suhu udara
            strcpy(value_suhu_udara, suhu_udara);
        }

        if (column == 1) {  // Jika ini adalah kolom pertama (parameter udara)
            strcpy(parameter_udara, token);  // Simpan parameter udara
        }

        column++;  // Naikkan indeks kolom
        token = strtok(NULL, ",");  // Ambil token berikutnya
    }

    float value_suhu = atof(suhu_udara);  // Ubah suhu udara menjadi tipe float
    if (value_suhu <= 35) {  // Jika suhu kurang dari atau sama dengan 35
        fputs(temp_line, temp_file);  // Tulis baris ke file sementara
    } else {  // Jika suhu lebih dari 35
        char log_message[100];  // Buat pesan log
        sprintf(log_message, "Successfully drop row with parameter %s value %s", parameter_udara, value_suhu_udara);  // Tulis pesan log
        logbook(log_message);  // Catat pesan log
    }
}
```
Bagian ini mengambil setiap baris yang dibaca dan mengekstrak nilai kolom ke-4 (suhu udara) dan kolom pertama (parameter udara). Jika suhu udara lebih dari 35 derajat, baris tersebut dihapus, dan pesan log dicatat untuk mencatat penghapusan ini. Jika suhu udara kurang dari atau sama dengan 35 derajat, baris tersebut ditulis ke file sementara.

- Bagian penyelesaian pemrosesan
```C 
fclose(input_file);  // Tutup file input
fclose(temp_file);  // Tutup file sementara

remove("weather/kecamatanforecast.csv");  // Hapus file lama
rename("weather/kecamatanforecast_temp.csv", "weather/kecamatanforecast.csv");  // Ganti nama file sementara menjadi file asli

logbook("Data preprocessing completed.");  // Catat log selesai pemrosesan

```
Bagian ini menutup file input dan file sementara. Setelah pemrosesan selesai, file input lama dihapus dan file sementara diganti namanya menjadi file asli. Setelah semua langkah selesai, pesan log dicatat untuk mencatat bahwa pemrosesan data telah selesai.

_Dokumentasi_

![alt text](/resource/3b-1.png)

### Problem 3c
Kemudian dari dataset tersebut, Rama mengelompokkannya berdasarkan ID Lokasi dan memisahkannya menjadi dataset baru dengan format nama “city_{ID Lokasi}” contoh “city_5012725.csv”. Semua dataset baru tersebut dimasukan ke dalam folder bernama “city_group”.

**Jawab**

```C
void grouping() {
    mkdir("city_group", 0777);  // Buat folder "city_group" jika belum ada

    FILE *input_file = fopen("weather/kecamatanforecast.csv", "r");  // Buka file input
    if (input_file == NULL) {
        perror("Error opening input file");  // Tampilkan pesan kesalahan jika gagal
        exit(EXIT_FAILURE);  // Keluar jika gagal
    }

    char line[512];  // Buffer untuk membaca baris
    fgets(line, sizeof(line), input_file);  // Baca dan abaikan baris header

    FILE *file_map[10000] = {NULL};  // Array untuk menyimpan penunjuk file berdasarkan indeks loc_id

    while (fgets(line, sizeof(line), input_file)) {  // Baca setiap baris dari file input
        if (strlen(line) > 0) {  // Jika baris tidak kosong
            char temp_line[sizeof(line)];
            strcpy(temp_line, line);

            char *loc_id = strtok(temp_line, ",");  // Dapatkan loc_id
            if (loc_id == NULL) {
                continue;  // Jika tidak ada loc_id, lanjutkan
            }

            int index = atoi(loc_id) % 10000;  // Hitung indeks untuk file_map

            if (!file_map[index]) {  // Jika file belum dibuka
                char grouped_file_name[256];  // Buffer untuk nama file yang dikelompokkan
                snprintf(grouped_file_name, sizeof(grouped_file_name), "city_group/city_%s.csv", loc_id);  // Buat nama file berdasarkan loc_id
                file_map[index] = fopen(grouped_file_name, "a");  // Buka file
                if (!file_map[index]) {
                    perror("Error opening grouped file");  // Tampilkan pesan kesalahan jika gagal
                    continue;
                }
                fputs("0,1,6,7,8,9,10\n", file_map[index]);  // Tulis header ke file baru
            }
            fputs(line, file_map[index]);  // Tulis baris ke file yang sesuai
        }
    }

    // Tutup semua file yang dibuka
    for (int i = 0; i < 10000; i++) {
        if (file_map[i]) {
            fclose(file_map[i]);  // Tutup file
        }
    }

    fclose(input_file);  // Tutup file input

    logbook("Successfully grouped data.\n");  // Catat log selesai pengelompokan
}
```

- Pembuatan Folder city_group
```C 
mkdir("city_group", 0777);  // Buat folder "city_group" jika belum ada
```
Pada bagian ini, folder bernama city_group dibuat dengan izin 0777, yang berarti folder tersebut dapat diakses dan dimodifikasi oleh semua pengguna. Jika folder sudah ada, mkdir tidak akan membuat yang baru.

- Membuka File Input
```C 
FILE *input_file = fopen("weather/kecamatanforecast.csv", "r");  // Buka file input
if (input_file == NULL) {
    perror("Error opening input file");  // Tampilkan pesan kesalahan jika gagal
    exit(EXIT_FAILURE);  // Keluar jika gagal
}
```
Bagian ini membuka file CSV yang berisi data cuaca. Jika file tidak ditemukan atau tidak dapat dibuka, fungsi perror akan menampilkan pesan kesalahan dan program akan keluar dengan kode kesalahan.


- Membaca Header File CSV
```C 
char line[512];  // Buffer untuk membaca baris
fgets(line, sizeof(line), input_file);  // Baca dan abaikan baris header
```
Bagian ini membaca baris pertama (header) dari file CSV dan mengabaikannya. Buffer line digunakan untuk menyimpan baris yang dibaca.

- Inisialisasi Array Penunjuk File
```C 
FILE *file_map[10000] = {NULL};  // Array untuk menyimpan penunjuk file berdasarkan indeks loc_id
```
Bagian ini menginisialisasi array yang akan menyimpan penunjuk file berdasarkan indeks loc_id. Indeks didasarkan pada nilai loc_id yang akan dihitung kemudian.

- Pengelompokan Data Berdasarkan loc_id
```C 
while (fgets(line, sizeof(line), input_file)) {  // Baca setiap baris dari file input
    if (strlen(line) > 0) {  // Jika baris tidak kosong
        char temp_line[sizeof(line)];
        strcpy(temp_line, line);

        char *loc_id = strtok(temp_line, ",");  // Dapatkan loc_id
        if (loc_id == NULL) {
            continue;  // Jika tidak ada loc_id, lanjutkan
        }

        int index = atoi(loc_id) % 10000;  // Hitung indeks untuk file_map

        if (!file_map[index]) {  // Jika file belum dibuka
            char grouped_file_name[256];  // Buffer untuk nama file yang dikelompokkan
            snprintf(grouped_file_name, sizeof(grouped_file_name), "city_group/city_%s.csv", loc_id);  // Buat nama file berdasarkan loc_id
            file_map[index] = fopen(grouped_file_name, "a");  // Buka file
            if (!file_map[index]) {
                perror("Error opening grouped file");  // Tampilkan pesan kesalahan jika gagal
                continue;
            }
            fputs("0,1,6,7,8,9,10\n", file_map[index]);  // Tulis header ke file baru
        }
        fputs(line, file_map[index]);  // Tulis baris ke file yang sesuai
    }
}
```
Bagian ini membaca setiap baris dari file CSV. Jika baris tidak kosong, ia menyalin baris ke temp_line dan memecahnya dengan delimiter , untuk mendapatkan loc_id. Jika loc_id ada, indeks dihitung dengan operasi modulo 10.000 (atoi(loc_id) % 10000), sehingga nilai indeks berada dalam rentang 0-9.999.

Jika file untuk indeks tersebut belum ada, maka file baru dibuat di dalam folder city_group dengan nama berdasarkan loc_id. Jika file gagal dibuka, kesalahan ditampilkan dan iterasi dilanjutkan ke baris berikutnya. Jika file berhasil dibuka, header standar ditulis ke file tersebut. Baris kemudian ditulis ke file yang sesuai berdasarkan indeks.

- Penutupan File dan Logging
```C 
// Tutup semua file yang dibuka
for (int i = 0; i < 10000; i++) {
    if (file_map[i]) {
        fclose(file_map[i]);  // Tutup file
    }
}

fclose(input_file);  // Tutup file input

logbook("Successfully grouped data.\n");  // Catat log selesai pengelompokan

```
Bagian ini menutup semua file yang telah dibuka untuk menghindari kebocoran memori dan file yang terbuka. Penutupan dilakukan dengan iterasi melalui seluruh array file_map dan menutup file jika tidak NULL. Setelah itu, file input juga ditutup. Terakhir, pesan log dicatat untuk menandai bahwa pengelompokan telah berhasil dilakukan.

_Dokumentasi_

![alt text](/resource/3c-1.png)

### Problem 3d
Rama harus membuat logbook untuk laporan kepada dosen, sehingga semua activities diatas harus disimpan dalam “logbook.txt” dengan format { [YYYY-MM-dd HH:mm:ss] [message]}.

**Jawab**

```C
void logbook(const char *message) {
    time_t current_time;
    struct tm *timeinfo;
    char timestamp[80];  // Array untuk menyimpan stempel waktu

    time(&current_time);  // Dapatkan waktu saat ini
    timeinfo = localtime(&current_time);  // Konversi ke waktu lokal

    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", timeinfo);  // Format stempel waktu

    FILE *log_file = fopen(LOG_FILE, "a");  // Buka file log untuk penambahan
    if (log_file == NULL) {
        perror("Error opening log file");  // Tampilkan pesan kesalahan jika gagal
        exit(EXIT_FAILURE);  // Keluar dari program jika gagal
    }
    fprintf(log_file, LOG_FORMAT, timestamp, message);  // Tulis pesan log dengan stempel waktu
    fclose(log_file);  // Tutup file log
}
```

- Menginisiasi Variabel dan Mendapatkan Waktu Saat Ini
```C 
time_t current_time;  // Menyimpan waktu saat ini dalam format time_t
struct tm *timeinfo;  // Menyimpan informasi waktu yang terstruktur
char timestamp[80];  // Buffer untuk menyimpan stempel waktu

time(&current_time);  // Mendapatkan waktu saat ini dalam format time_t
timeinfo = localtime(&current_time);  // Mengkonversi waktu ke waktu lokal yang terstruktur
```
Pada bagian ini, variabel yang diperlukan untuk mencatat waktu dan stempel waktu diinisiasi. time(&current_time) digunakan untuk mendapatkan waktu saat ini dalam format time_t, dan localtime(&current_time) mengkonversi waktu tersebut ke struktur waktu lokal yang lebih mudah dimanipulasi.

- Penutupan File dan Logging
```C 
strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", timeinfo);  // Format stempel waktu
```
Stempel waktu dibuat menggunakan strftime. Ini mengubah waktu yang terstruktur menjadi string yang diformat sesuai dengan pola yang diberikan. Pola yang digunakan di sini adalah [tahun-bulan-hari jam:menit:detik], sehingga memberikan waktu yang dapat dibaca.

- Membuka File Log untuk Penambahan
```C 
FILE *log_file = fopen(LOG_FILE, "a");  // Buka file log dalam mode penambahan
if (log_file == NULL) {
    perror("Error opening log file");  // Tampilkan pesan kesalahan jika file gagal dibuka
    exit(EXIT_FAILURE);  // Keluar dari program jika gagal
}
```
Di bagian ini, file log dibuka dalam mode penambahan dengan fopen(LOG_FILE, "a"). Mode penambahan berarti data baru akan ditambahkan di akhir file tanpa menghapus data yang ada. Jika file tidak dapat dibuka, perror menampilkan pesan kesalahan, dan exit(EXIT_FAILURE) keluar dari program untuk mencegah kerusakan lebih lanjut.

- Menulis Pesan Log dan Menutup File

```C 
fprintf(log_file, LOG_FORMAT, timestamp, message);  // Tulis pesan log dengan stempel waktu
fclose(log_file);  // Tutup file log
```
Bagian ini menulis pesan log ke dalam file dengan format yang sudah ditentukan, termasuk stempel waktu dan pesan yang diterima sebagai argumen. fprintf digunakan untuk menulis ke file dengan format tertentu. Setelah pesan ditulis, file ditutup menggunakan fclose untuk memastikan semua data disimpan dengan benar dan untuk mencegah kebocoran sumber daya.

_Dokumentasi_

![alt text](/resource/3d-1.png)

### Kendala

Tidak ada ("-")

## 4️⃣ Soal 4
Sebagai mahasiswa baru informatika, satria memiliki ide untuk membuat game tic-tac-toe yang berjalan di terminal. Tic-tac-toe merupakan permainan dengan 2 pemain yang secara bergantian menempatkan tanda 'X' atau 'O' hingga membentuk garis horizontal, vertikal, atau diagonal. Untuk membuat game ini, satria membutuhkan 2 program c, yaitu game.c dan player.c
### Problem 4a
Pengguna dapat memilih tipe pemain (‘X’ atau ‘O’) berdasarkan perintah dari program ‘player.c’. Jika tipe telah dipilih oleh pengguna sebelumnya, beri pesan " has already been taken, choose other player types". Informasi terkait apakah tipe pemain telah dipilih atau tidak, dapat diakses melalui shared memory.

**Jawab**

_1. Sebelum menjalankan apa yang diminta soal, kita siapkan dahulu papan tic tac toe pada kedua program, yaitu player.c dan game.c_
```C
#define MAX_CHESSBOARD 3

// variabel untuk papan permainan 3x3
char (*papanPermainan)[MAX_CHESSBOARD];
```
- Mendefinisikan konstanta bernama `MAX_CHESSBOARD` dengan nilai 3 pada header. Konstanta ini akan digunakan untuk menentukan papan permainan tic tac toe berukuran 3x3
- `char (*papanPermainan)[MAX_CHESSBOARD]` merupakan pointer ke array 2 dimensi dengan ukuran 3 baris dan 3 kolom

_2. Membuat struct untuk menyimpan data pemain. Walaupun penamaan struct pada player.c dan game.c terlihat berbeda, semuanya memiliki kegunaan yang sama. Program pertama pada player.c dan program kedua pada game.c_
```C
// struct untuk menyimpan identitas pemain
struct IdentitasPemain
{
    int jenis; // 1: X, 2: O
    int jumlah; // jumlah pemain yang telah bergabung
};
```
```C
// menyimpan pemilihan pemain
struct PemilihanPemain
{
    int pemain; // nomor pemain yang bergabung
    int jumlah; // jumlah pemain yang sudah bergabung
};
```
_3. Membuat fungsi untuk memeriksa pemilihan tipe/jenis pemain pada program player.c_
```C
int periksaPemain(int jenis, int memoriBersama)
{
    if (jenis == memoriBersama)
    {
        if (jenis == 1)
        {
            printf("Tipe pemain 'X' sudah dipilih, pilih jenis pemain lainnya\n");
            return 1;
        }
        else if (jenis == 2)
        {
            printf("Tipe pemain 'O' sudah dipilih, pilih jenis pemain lainnya\n");
            return 1;
        }
        else
        {
            printf("Masukkan tipe pemain yang valid!\n");
            return 1;
        }
    }
    else
    {
        if (jenis == 1 || jenis == 2)
        {
            return 0;
        }
        else
        {
            printf("Masukkan tipe pemain yang valid!\n");
            return 1;
        }
    }
    return 0;
}
```
- Fungsi ini menerima 2 parameter, yaitu `jenis` yang merupakan jenis pemain yang diperiksa dan `memoriBersama` yang merupakan jenis pemain yang telah dipilih sebelumnya
- Fungsi akan memeriksa apakah `jenis` sama dengan `memoriBersama`, jika iya makan akan mencetak pesan bahwa tipe pemain telah dipilih dan mengembalikan nilai 1.
- Jika `jenis` adalah 1 atau 2, yang merupakan jenis pemain yang valid, fungsi akan mengembalikan nilai 0

_4. Setelah membuat fungsi untuk mengecek tipe/jenis pemain, kita beralih pada Main Function program player.c_
```C
int main()
{
    int tipePemain;
    char simbol;
    int idMemori;
    struct IdentitasPemain *pilihanPemain = (struct IdentitasPemain *)malloc(sizeof(struct IdentitasPemain)); // alokasi memori untuk menyimpan jenis pemain
    idMemori = shmget((key_t)2121, 1024, 0666); // mendapatkan id memori bersama, memori 2121, 1024 byte, 0666 untuk permission (rw)
    pilihanPemain = shmat(idMemori, NULL, 0); // menautkan memori bersama ke variabel pilihanPemain

    do
    {
        printf("Jenis pemain yang tersedia:\n");
        printf("1. X\n2. O\n");
        printf("Pilih jenis Anda (1 atau 2): ");
        scanf("%d", &tipePemain);
    } while (periksaPemain(tipePemain, pilihanPemain->jenis));

    pilihanPemain->jenis = tipePemain;
    pilihanPemain->jumlah++;

    printf("Menunggu pemain lain memilih jenis pemain...\n");

    int idAntrian;
    idAntrian = msgget((key_t)3030, IPC_CREAT | 0644); // mendapatkan id antrian pesan, key 3030, 0644 untuk permission

    if (tipePemain == 1)
    {
        simbol = 'X';
    }
    else
    {
        simbol = 'O';
    }

    .....
```
- `tipePemain` untuk menyimpan jenis pemain yang dipilih oleh pengguna
- `simbol` untuk menyimpan simbol pemain yang terkait dengan jenis pemain yang dipilih
- `idMemori`untuk menyimpan ID memori bersama
- `pilihanPemain` yang merupakan pointer ke struktur `IdentitasPemain` yang digunakan untuk menyimpan pilihan pemain (jenis dan jumlah). Melalui `malloc`, dilakukan alokasi memori untuk menyimpan informasi pemain yang berbagi (jenis pemain yang dipilih dan jumlah pemain yang telah bergabung)
- `shmget` untuk mendapatkan ID memori bersama melalui parameter kunci akses, ukuran memori, dan hak akses/permission memori bersama
- `shmat()` digunakan untuk menautkan memori bersama yang telah dialokasikan ke variabel `pilihanPemain`
- Program akan looping secara terus menerus sampai pemilihan pemain valid, yaitu ketika `periksaPemain()` mengembalikan nilai 0. Jika valid, maka informasi akan diperbarui di dalam variabel `pilihanPemain`
- `msgget()` untuk mendapatkan ID antrian pesan dengan parameter kunci dan flag untuk membuat antrian pesan baru jika belum ada (IPC_CREAT) serta hak akses untuk antrian pesan
- Variabel `simbol` ditentukan berdasarkan jenis pemain yang dipilih ('X' jika jenis pemain adalah 1 dan 'O' jika jenis pemain adalah 2)

_5. Main Function program game.c. Dikarenakan ketika pemain dan game belum dimulai, maka pada terminal game.c tidak menampilkan apa-apa, Namun, saya memilih untuk menampilkan informasi menunggu pemain untuk bergabung dalam permainan_
```C
int main()
{
    int i, idAntrianPesan;
    struct PemilihanPemain *pemilihanPemain = (struct PemilihanPemain *)malloc(sizeof(struct PemilihanPemain));
    int idMemoriBerbagi;

    idMemoriBerbagi = shmget((key_t)2121, 1024, 0666 | IPC_CREAT);
    pemilihanPemain = shmat(idMemoriBerbagi, NULL, 0);
    pemilihanPemain->pemain = 0;
    pemilihanPemain->jumlah = 0;

    printf("Menunggu pemain untuk bergabung dalam permainan...\n");
    .....
```
- `i` untuk iterasi
- `idAntrianPesan` untuk menyimpan ID antrian pesan
- `pemilihanPemain` pointer ke struktur PemilihanPemain, yang digunakan untuk menyimpan informasi pemilihan pemain
- `idMemoriBerbagi` untuk menyimpan ID memori bersama
- `shmget` dan `shmat` untuk pengambilan dan penautan segmen memori bersama ke variabel `pemilihanPemain`. Nilai awal `pemain` dan `jumlah` di setting ke 0 menandakan bahwa belum ada pemain yang bergabung

_6. Dokumentasi_
- Terminal player.c
![alt text](/resource/4a-1.png)
- Kondisi ketika salah satu player sudah memilih tipe/jenis pemain
![alt text](/resource/4a-2.png)
- Kondisi ketika player memilih tipe/jenis pemain yang sudah dipilih oleh pemain sebelumnya
![alt text](/resource/4a-3.png)
- Terminal game.c
![alt text](/resource/4a-4.png)

### Problem 4b
Karena berjalan di terminal, program ini menggunakan nomor kotak (1-9) yang berurutan dari kiri atas ke kanan bawah untuk memudahkan pemilihan kotak. Program 'player.c' akan menampilkan kotak tic tac toe saat ini dan meminta input pengguna berupa nomor kotak sesuai gilirannya (player 'X' memulai terlebih dahulu). Input yang diberikan pengguna kemudian dikirimkan ke 'game.c' menggunakan message queue.

**Jawab**

_1. Membuat struct untuk menyimpan pergerakan setiap pemain. Pada kedua program memiliki struct ini untuk kebutuhan representasi gerakan pemain. Program ini pertama pada player.c dan kedua pada game.c_
```C
// struct untuk merepresentasikan gerakan pemain
struct Gerakan
{
    long tipePesan; // tipe pesan yang digunakan dalam message queue)
    int pemain; // nomor pemain yang melakukan gerakan (1 atau 2)
    int sel; 
};
```
```C
// representasi gerakan pemain
struct GerakanPemain
{
    long tipePesan; // tipe pesan yang digunakan dalam message queue)
    int nomorPemain; // nomor pemain yang melakukan gerakan (1 atau 2)
    int selPilihan; 
};
```

_2. Kita berfokus dahulu pada program player.c. Pada program player.c kita harus membuat fungsi untuk menampilkan papan tic tac toe_
```C
// menginisialisasi papan permainan dan menampilkan sel-selnya
void inisialisasiPapan()
{
    // membuat segmen memori bersama untuk papan permainan
    int idMemori = shmget((key_t)1414, sizeof(char[MAX_CHESSBOARD][MAX_CHESSBOARD]), 0666 | IPC_CREAT);
    // menautkan segmen memori bersama ke variabel papanPermainan
    papanPermainan = shmat(idMemori, NULL, 0);

    int nomorSel = 1;
    printf("\n  ");
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            printf("%d", nomorSel++);
            if (j < 2)
            {
                printf(" | ");
            }
        }
        if (i < 2)
            printf("\n -----------\n  ");
    }
    printf("\n\nPilih nomor sel yang sesuai untuk memilih sebuah sel.\n");
    printf("\n");
}
```
- `shmget()` untuk membuat segmen memori bersama (ukuran sesuai papan permainan) yang akan digunakan untuk menyimpan papan permainan. Segmen memori ini diberi kunci `(key_t)1414` untuk mengaksesnya di tempat lain dalam program. Hak akses `0666` untuk rw semua pengguna
- `shmat` untuk menautkan ke variabel `papanPermainan`. Sehingga, `papanPermainan` akan menunjuk ke segmen memori yang telah dialokasikan untuk menyimpan papan permainan
- Iterasi untuk menampilkan nomor sel dari 1 sampai 9 secara berurutan ke terminal. Setelah nomor sel ditampilkan, ditambahkan garis pemisah horizontal dan vertikal untuk membentuk tampilan papan permainan yang terdiri dari 3 baris dan 3 kolom

_3. Main Function program player.c_
```C
while (1)
    {
        int giliran = 0, sel;
        if (pilihanPemain->jumlah == 2)
        {
            printf("\n---- Permainan Dimulai!! ----\n\n");
            printf("Anda bermain sebagai %c\n", simbol);
            inisialisasiPapan();

     .....
```
- `giliran` untuk menentukan giliran pemain
- `sel`  untuk menyimpan nomor sel yang dipilih oleh pemain
- jika jumlah pemain sudah 2, maka program akan dijalankan dan papan tic tac toe akan ditampilkan ke terminal player.c melalui pemanggilan fungsi `inisialisasiPapan()`

_4. Membuat fungsi pada game.c untuk menyiapkan papan permainan dengan menginisialisasi segmen memori bersama yang akan digunakan untuk menyimpan status setiap sel pada papan permainan_
```C
void siapkanPapanPermainan()
{
    // Membuat segmen memori bersama untuk papan permainan
    int idMemoriBerbagi = shmget((key_t)1414, sizeof(char[MAX_CHESSBOARD][MAX_CHESSBOARD]), 0666 | IPC_CREAT);
    // Menautkan segmen memori bersama ke variabel papanPermainan
    papanPermainan = shmat(idMemoriBerbagi, NULL, 0);

    // Inisialisasi semua sel papan permainan dengan kosong
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            papanPermainan[i][j] = ' ';
        }
    }
}
```
- `shmget()` untuk membuat segmen memori bersama (ukuran sesuai papan permainan) yang akan digunakan untuk menyimpan papan permainan. Segmen memori ini diberi kunci `(key_t)1414` untuk mengaksesnya di tempat lain dalam program. Hak akses `0666` untuk rw semua pengguna
- `shmat()` untuk menautkan ke variabel `papanPermainan`. Dengan demikian, variabel `papanPermainan` akan menunjuk ke segmen memori yang telah dialokasikan untuk menyimpan papan permainan
- Iterasi untuk menginisialisasi setiap sel pada papan permainan dengan karakter kosong (`' '`)

_5. Main Function game.c. Program ini untuk menerima gerakan pemain dari antrian pesan, mengekstraksi informasi tentang gerakan tersebut, dan menampilkan informasi tersebut ke layar_
```C
int main()
{
    .....

    int giliranSaatIni = 0;
    while (1)
    {
        if (pemilihanPemain->jumlah == 2)
        {
            siapkanPapanPermainan();
            idAntrianPesan = msgget((key_t)3030, IPC_CREAT | 0644);

            printf("---- Permainan Dimulai! ----\n");
            while (1)
            {
                int giliranPemainSekarang = giliranSaatIni % 2 + 1;
                int selPilihan;

                struct GerakanPemain gerakanPemain;
                struct GerakanValid validitas, validitas2;

                // Menerima gerakan pemain dari antrian pesan
                msgrcv(idAntrianPesan, &gerakanPemain, sizeof(gerakanPemain), 1, 0);

                int pemainSekarang = gerakanPemain.nomorPemain;
                selPilihan = gerakanPemain.selPilihan;
                char simbol;
                printf("Pesan Baru\n");
                if (pemainSekarang == 1)
                {
                    printf("Pengirim: Pemain X\nPesan: %d\n", selPilihan);
                    simbol = 'X';
                }
                else
                {
                    printf("Pengirim: Pemain O\nPesan: %d\n", selPilihan);
                    simbol = 'O';
                }
     .....
```
- Struct `GerakanValid` belum saya jelaskan disini, nanti akan saya jelaskan pada problem 4c
- Jika jumlah pemain sudah 2, maka permainan dapat dimulai dengan memanggil fungsi `siapkanPapanPermainan()` dan mendapatkan ID antrian pesan melalui `msgget()`
- Dengan `msgrcv()`, program menerima pesan yang dikirimkan ke antrian pesan dengan ID `idAntrianPesan`.
Pesan yang diterima disimpan dalam variabel `gerakanPemain`
- Variabel `pemainSekarang` diisi dengan nomor pemain yang melakukan gerakan, yang diambil dari `gerakanPemain.nomorPemain`
- Variabel `selPilihan` diisi dengan nomor sel yang dipilih oleh pemain tersebut, yang diambil dari `gerakanPemain.selPilihan`

_6. Dokumentasi_
- Terminal player.c dengan jenis pemain 'X'
![alt text](/resource/4b-1.png)
- Terminal player.c dengan jenis pemain 'O'
![alt text](/resource/4b-2.png)
- Terminal game.c ketika pemain sudah memilih sel
![alt text](/resource/4b-3.png)

### Problem 4c
Selanjutnya, ‘game.c’ harus mengecek apakah input user sesuai atau tidak berdasarkan nilai kotak permainan saat ini. Kirimkan pesan error kepada pengguna jika nomor kotak yang dipilih tidak kosong atau diluar angka 1-9. Pesan dapat dikirimkan melalui message queue dengan ‘mesg_type’ yang berbeda.

**Jawab**

_1. Menindaklanjuti dari 4b, maka kita akan membuat struct pada game.c dan player.c. Struct ini digunakan untuk mengecek gerakan atau input dari pemain itu valid. Program berikut, pertama pada game.c dan kedua pada player.c_
```C
struct GerakanValid
{
    long tipePesan; // Tipe pesan (digunakan dalam message queue)
    int kodeStatus; // Kode status validitas gerakan (0: tidak valid, 1: valid, 2: menang, 3: sel tidak valid, 4: seri)
    char simbolMenang; // Simbol pemain yang menang (jika ada)
};
```
```C
struct GerakanValid
{
    long tipePesan; // tipe pesan yang (digunakan dalam message queue)
    int status; // kode status validitas gerakan (0: tidak valid, 1: valid, 2: menang, 3: nomor sel tidak valid, 4: seri)
    char simbolPemenang;
};
```
- Kedua struct memiliki kegunaan yang sama di masing-masing program, yaitu untuk mengecek valid atau tidaknya gerakan pemain di tic tac toe

_2. Mungkin pada penjelasan poin 2 berikut sampai selesai akan mewakili juga jawaban nomor 4d karena problem 4c dan 4d saling berhubungan. Selanjutnya, kita beralih pada program player.c terlebih dahulu. Di sini, kita membuat fungsi untuk menampilkan papan secara real time_
```C
void tampilkanPapan()
{
    printf("\n  ");
    for (int i = 0; i < MAX_CHESSBOARD; i++)
    {
        for (int j = 0; j < MAX_CHESSBOARD; j++)
        {
            printf("%c", papanPermainan[i][j]);
            if (j < 2)
            {
                printf(" | ");
            }
        }
        if (i < 2)
            printf("\n -----------\n  ");
    }
    printf("\n\n\n");
}
```
- Pada setiap sel, karakter yang sesuai dengan status sel pada papan permainan ('X', 'O', atau ' ') dicetak ke terminal
- Program di atas, intinya bertanggung jawab untuk menampilkan tampilan visual dari papan permainan ke layar, sehingga pemain dapat melihat status terkini dari papan permainan saat mereka bermain

_3. Main Function player.c. Program ini merupakan kelanjutan dari problem 4b poin 3. Program ini mengatur jalannya permainan tic-tac-toe antara dua pemain_
```C
int main()
{
    ......
            while (1)
            {
                struct Gerakan gerakanPemain;
                struct GerakanValid validitas;
                if (tipePemain == (giliran % 2) + 1)
                {
                    printf("Giliran Anda!\n");
                    printf("Masukkan nomor sel: ");
                    scanf("%d", &sel);

                    gerakanPemain.tipePesan = 1; // tipe pesan 1 untuk gerakan pemain
                    gerakanPemain.pemain = tipePemain;
                    gerakanPemain.sel = sel;

                    msgsnd(idAntrian, &gerakanPemain, sizeof(gerakanPemain), 0); // mengirim gerakan pemain ke antrian pesan
                }
                else
                {
                    // menampilkan pesan untuk pemain yang tidak sedang gilirannya
                    if (simbol == 'X')
                    {
                        printf("Giliran pemain O...\n");
                    }
                    else
                    {
                        printf("Giliran pemain X...\n");
                    }
                }

                msgrcv(idAntrian, &validitas, sizeof(validitas), tipePemain + 1, 0); // menerima validitasgerakan dari antrian pesan

                if (validitas.status == 1)
                {
                    // jika gerakan valid, tampilkan papan permainan
                    giliran++;
                    tampilkanPapan();
                }
                else if (validitas.status == 0 && tipePemain == (giliran % 2) + 1)
                {
                    // jika gerakan tidak valid, tampilkan pesan kesalahan
                    printf("Sel tersebut sudah terisi !!\n");
                }
                else if (validitas.status == 2)
                {
                    // jika permainan selesai dan pemain menang
                    tampilkanPapan();
                    if (simbol == validitas.simbolPemenang)
                    {
                        printf("---- Permainan Selesai! ----\n");
                        printf("Selamat, Anda menang! :)\n");
                    }
                    else
                    {
                        printf("---- Permainan Selesai! ----\n");
                        printf("Maaf, Anda kalah! :(\n");
                    }
                    break;
                }
                else if (validitas.status == 3 && tipePemain == (giliran % 2) + 1)
                {
                    // jika nomor sel tidak valid, tampilkan pesan kesalahan
                    printf("Nomor sel harus antara 1 - 9 !!\n");
                }
                else if (validitas.status == 4)
                {
                    // jika permainan berakhir seri, tampilkan pesan seri
                    tampilkanPapan();
                    printf("---- Permainan Selesai! ----\n");
                    printf("Permainan berakhir seri\n");
                    break;
                }
            }
            break;
        }
    }
}
```
- Variabel `giliran` digunakan untuk menentukan giliran pemain saat ini dengan cara melakukan operasi modulus terhadap variabel tersebut. Jika giliran saat ini sama dengan jenis pemain yang sedang aktif (variabel `tipePemain`), program meminta pemain tersebut untuk memilih nomor sel yang ingin dipilih
- Setelah pemain memasukkan nomor sel, gerakan pemain tersebut dikemas ke dalam struct `Gerakan` yang memiliki informasi tentang tipe pesan, jenis pemain, dan nomor sel. Kemudian gerakan tersebut dikirimkan ke antrian pesan menggunakan fungsi `msgsnd()`
- Setelah pemain melakukan gerakan, program menerima pesan yang berisi validitas gerakan tersebut dari antrian pesan menggunakan fungsi `msgrcv()`. Pesan ini berisi informasi tentang apakah gerakan tersebut valid, tidak valid, atau berbagai kondisi lainnya
- Dalam case ini, kita menggunakan 5 kode validitas, 0: tidak valid, 1: valid, 2: menang, 3: sel tidak valid, 4: seri. Percabangan `if` dan `else if` digunakan di dalam loop untuk memeriksa kondisi dari setiap langkah permainan. Percabangan ini memungkinkan program untuk mengambil tindakan yang berbeda tergantung pada status permainan pada saat itu (penjelasan ada di bawah ini)
- Jika gerakan tidak valid (bernilai '0'), pesan kesalahan ditampilkan
- Jika gerakan pemain valid (bernilai '1'), papan permainan diperbarui dan giliran pemain berganti
- Jika nomor sel tidak valid (bernilai '3'), pesan kesalahan ditampilkan
- Jika permainan selesai dengan pemenang (bernilai '2'), pesan kemenangan atau kekalahan ditampilkan sesuai kondisi pemain
- Jika permainan berakhir seri (bernilai '4'), pesan seri ditampilkan

_4. Penjelasan pada program player.c telah selesai, selanjutnya berganti pada program game.c untuk membuat fungsi untuk memperbarui papan permainan_
```C
int terapkanGerakan(int selPilihan, char simbolPemain)
{
    // Memeriksa apakah nomor sel valid (antara 1 dan 9)
    if (selPilihan < 1 || selPilihan > 9)
    {
        printf("-- Tidak valid: Nomor sel harus antara 1 dan 9! --\n");
        return -1;
    }

    // Menghitung baris dan kolom berdasarkan nomor sel
    int baris = (selPilihan - 1) / MAX_CHESSBOARD;
    int kolom = (selPilihan - 1) % MAX_CHESSBOARD;
    int isValid = 1;

    // Periksa apakah sel yang dipilih sudah terisi
    if (papanPermainan[baris][kolom] != ' ')
    {
        printf("-- Tidak valid: Sel sudah terisi! --\n");
        isValid = 0;
    }
    else
    {
        papanPermainan[baris][kolom] = simbolPemain; // Menambahkan simbol pemain ke sel yang dipilih
    }

    return isValid;
}
```
- Pertama, fungs akan memeriksa apakah nomor sel yang dipilih berada dalam rentang valid antara 1 dan 9. Jika nomor sel diluar rentang tersebut, fungsi akan mencetak pesan kesalahan dan mengembalikan nilai `-1` untuk menandakan bahwa gerakan tidak valid
- Jika valid, fungsi akan menghitung baris dan kolom yang sesuai dengan nomor sel tersebut menggunakan operasi sederhana. Terlihat bahwa `selPilihan - 1`, ini dikarenakan indeks array dimulai dari 0, sedangkan nomor sel dimulai dari 1. Sehingga diperlukan pengurangan 1
- `papanPermainan[baris][kolom] != ' '` untuk mengecek apakah sel sudah terisi, jika sudah maka gerakan tidak valid. Jika belum terisi, `simbolPemain` akan ditambahkan ke sel
- Mengembalikan nilai `isValid`. Jika bernilai 0 berarti tidak valid, jika bernilai 1 maka valid

_5. Membuat fungsi untuk mengecek kondisi pemain jika menang pada game.c_
```C
int periksaMenang(char simbol)
{
    // Memeriksa baris, kolom, dan diagonal untuk mencari tanda-tanda kemenangan
    if (papanPermainan[0][0] == simbol && papanPermainan[0][1] == simbol && papanPermainan[0][2] == simbol ||
        papanPermainan[1][0] == simbol && papanPermainan[1][1] == simbol && papanPermainan[1][2] == simbol ||
        papanPermainan[2][0] == simbol && papanPermainan[2][1] == simbol && papanPermainan[2][2] == simbol ||
        papanPermainan[0][0] == simbol && papanPermainan[1][0] == simbol && papanPermainan[2][0] == simbol ||
        papanPermainan[0][1] == simbol && papanPermainan[1][1] == simbol && papanPermainan[2][1] == simbol ||
        papanPermainan[0][2] == simbol && papanPermainan[1][2] == simbol && papanPermainan[2][2] == simbol ||
        papanPermainan[0][0] == simbol && papanPermainan[1][1] == simbol && papanPermainan[2][2] == simbol ||
        papanPermainan[0][2] == simbol && papanPermainan[1][1] == simbol && papanPermainan[2][0] == simbol)
    {
        return 1; // Kondisi menang terpenuhi
    }
    return 0; // Belum ada pemenang
}
```
- Intinya, fungsi tersebut digunakan untuk mengecek simbol yang sama secara horizontal (baris), vertikal (kolom), ataupun diagonal pada papan. Jika salah satu kondisi di atas terpenuhi, fungsi akan mengembalikan nilai 1. Jika tidak terpenuhi, fungsi akan mengembalikan nilai 0

_6. Setelah membuat fungsi pada poin 5, dilanjutkan membuat fungsi untuk mengecek kondisi pemain jika seri/seimbang/draw_
```C
int periksaImbang()
{
    int selTerisi = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (papanPermainan[i][j] != ' ')
            {
                selTerisi++; // Menghitung jumlah sel yang terisi
            }
        }
    }
    if (selTerisi == 9)
    {
        return 1; // Semua sel sudah terisi, menunjukkan imbang
    }
    return 0; // Permainan belum imbang
}
```
- Pertama, fungsi menginsialisasi variabel `selTerisi` dengan nilai 0 yang nantinya digunakan untuk menghitung jumlah sel yang sudah terisi pada papan
- `papanPermainan[i][j] != ' '` untuk mengecek isi dari sel (selain spasi). Jika terpenuhi, variabel `selTerisi` akan bertambah 1
- `selTerisi == 9` untuk mengecek bahwa jumlah sel yang terisi sama dengan jumlah total sel. Sehingga, fungsi akan mengembalikan nilai 1 yang menunjukkan bahwa permainan berkahir dengan seri/imbang/draw

_7. Kita lanjutkan pada Main Function pada program game.c_
```C
int main()
{
    .....
            while (1)
            {
                int giliranPemainSekarang = giliranSaatIni % 2 + 1;
                int selPilihan;

                struct GerakanPemain gerakanPemain;
                struct GerakanValid validitas, validitas2;

                // Menerima gerakan pemain dari antrian pesan
                msgrcv(idAntrianPesan, &gerakanPemain, sizeof(gerakanPemain), 1, 0);

                int pemainSekarang = gerakanPemain.nomorPemain;
                selPilihan = gerakanPemain.selPilihan;
                char simbol;
                printf("Pesan Baru\n");
                if (pemainSekarang == 1)
                {
                    printf("Pengirim: Pemain X\nPesan: %d\n", selPilihan);
                    simbol = 'X';
                }
                else
                {
                    printf("Pengirim: Pemain O\nPesan: %d\n", selPilihan);
                    simbol = 'O';
                }

                // Memvalidasi dan menerapkan gerakan pemain ke papan permainan
                if (terapkanGerakan(selPilihan, simbol) == 1)
                {
                    if (periksaMenang(simbol))
                    {
                        // Jika pemain menang, kirim pesan ke antrian pesan
                        validitas.kodeStatus = 2; // menang
                        validitas2.kodeStatus = 2;
                        validitas.tipePesan = 2;
                        validitas2.tipePesan = 3;
                        validitas.simbolMenang = simbol;
                        validitas2.simbolMenang = simbol;
                        msgsnd(idAntrianPesan, &validitas, sizeof(validitas), 0);
                        msgsnd(idAntrianPesan, &validitas2, sizeof(validitas2), 0);
                        printf("%c Pemain menang!!\n", simbol);
                        printf("---- Permainan Selesai! ----\n");
                        break;
                    }
                    else if (periksaImbang() == 1)
                    {
                        // Jika permainan berakhir imbang, kirim pesan ke antrian pesan
                        validitas.kodeStatus = 4;
                        validitas2.kodeStatus = 4;
                        validitas.tipePesan = 2;
                        msgsnd(idAntrianPesan, &validitas, sizeof(validitas), 0);
                        validitas2.tipePesan = 3;
                        msgsnd(idAntrianPesan, &validitas2, sizeof(validitas2), 0);
                        printf("Permainan berakhir seri\n");
                        printf("---- Permainan Selesai! ----\n");
                        break;
                    }
                    else
                    {
                        // Jika gerakan valid, lanjutkan dengan giliran pemain berikutnya
                        validitas.kodeStatus = 1;
                        validitas2.kodeStatus = 1;
                        validitas.tipePesan = 2;
                        msgsnd(idAntrianPesan, &validitas, sizeof(validitas), 0);
                        validitas2.tipePesan = 3;
                        msgsnd(idAntrianPesan, &validitas2, sizeof(validitas2), 0);
                        giliranSaatIni++; // Pindah ke giliran pemain berikutnya
                    }
                }
                else if (terapkanGerakan(selPilihan, simbol) == -1)
                { // <1 || >9
                    // Jika nomor sel tidak valid, kirim pesan ke antrian pesan
                    validitas.tipePesan = 2;
                    validitas.kodeStatus = 3;
                    msgsnd(idAntrianPesan, &validitas, sizeof(validitas), 0);
                    validitas2.tipePesan = 3;
                    validitas2.kodeStatus = 3;
                    msgsnd(idAntrianPesan, &validitas2, sizeof(validitas2), 0);
                }
                else
                {
                    // Jika gerakan tidak valid, kirim pesan ke antrian pesan
                    validitas.tipePesan = 2;
                    validitas.kodeStatus = 0;
                    msgsnd(idAntrianPesan, &validitas, sizeof(validitas), 0);
                    validitas2.tipePesan = 3;
                    validitas2.kodeStatus = 0;
                    msgsnd(idAntrianPesan, &validitas2, sizeof(validitas2), 0);
                }
            }
            break;
        }
    }
}
```
- Kita langsung loncat pada percabangan `if else` `terapkanGerakan`. Semua gerakan pemain diterapkan ke papan dengan menggunakan fungsi `terapkanGerakan`
- Jika gerakan valid (bernilai '1'), maka program akan mengecek semua kondisi dengan memanggil fungsi `periksaMenang(simbol)` untuk mengecek apakah gerakan tersebut sudah menang dan `periksaImbang() == 1` untuk mengecek gerakan tersebut akan menghasilkan seri. Jika kedua kondisi tersebut tidak terpenuhi, maka program melanjutkan ke giliran pemain berikutnya dengan meningkatkan `giliranSaatIni` 
- Jika nomor sel tidak valid (`terapkanGerakan` mengembalikan nilai -1), program mengirimkan pesan kesalahan nomor sel ke antrian pesan
- Jika gerakan tidak valid (sel telah terisi sebelumnya), program mengirimkan pesan kesalahan gerakan ke antrian pesan
- `validitas.kodeStatus` dan `validitas2.kodeStatus` untuk memberikan informasi tentang status pesan yang dikirim. 0 : tidak valid, 1 : valid, 2 : menang, 3 : sel tidak valid, 4 : seri
- `validitas.tipePesan` dan `validitas2.tipePesan` ntuk menunjukkan jenis pesan yang dikirim. 1 : berisi gerakan pemain, 2 : pesan berisi validitas gerakan untuk pemain yang sedang giliran, 3 : pesan berisi validitas gerakan untuk pemain yang tidak sedang giliran

_8. Dokumentasi_
- Ketika pemain sudah memilih beberapa sel
![alt text](/resource/4c-1.png)
- Ketika pemain memilih sel yang sudah diisi/dipilih oleh pemain lain
![alt text](/resource/4c-2.png)
- ketika pemain memilih sel diluar 1 sampai 9
![alt text](/resource/4c-3.png)

### Problem 4d
Jika input valid, ‘game.c’ perlu mengubah nilai pada kotak tic-tac-toe dan memutuskan apakah permainan telah selesai atau masih berlanjut. Permainan telah selesai jika kotak telah terisi semua (seri) atau terdapat simbol (‘X’ atau ‘O’) yang membentuk garis vertikal, horizontal, maupun diagonal. Kotak tic tac toe yang telah diperbarui dan hasil akhir permainan (jika sudah berakhir) dikirimkan ke 'player.c' untuk kemudian ditampilkan kepada pengguna.

**Jawab**

Dikarenakan penjelasan 4d ini sudah dijelaskan pada poin 4c, maka saya hanya melampirkan dokumentasinya saja

- Kondisi ketika menang (POV Pemenang)
![alt text](/resource/4d-1.png)
- Kondisi ketika menang (POV Kalah)
![alt text](/resource/4d-2.png)
- Kondisi ketika seri/imbang/draw
![alt text](/resource/4d-3.png)
- Terminal game.c
![alt text](/resource/4d-4.png)

### Kendala

Ketika praktikum, proses IPC antara program satu dengan program yang lain masih belum bisa (error). Akhirnya ketika revisi, problem IPC tersebut bisa terselesaikan dan task-4 solved

<div align=center>

# ------- SELESAI -------
</div>
