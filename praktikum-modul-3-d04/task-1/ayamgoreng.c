#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#define MAX 300 //remove and hero total
#define MAX_LINE 1000

typedef struct {
    char namaHero[40];
    char role[2][40];  // array untuk menyimpan hero yang memiliki 2 role
    int skillPoints;
} Hero;

void download_extract_datasets(char *path) {
    int status;
    pid_t pid;
    pid = fork();
    
    if (pid == 0) {
        // child process
        execlp("kaggle", "kaggle", "datasets", "download", "-d", "irwansyah10010/mobilelegend", "-p", path, NULL);
        perror("execlp");
        exit(1);
    }
    else if (pid > 0) {
        // parent process
        wait(&status); 
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("Download zip berhasil.\n");
            if (fork() == 0) {
                char zip_path[100]; // atau ukuran yang lebih besar jika diperlukan
                snprintf(zip_path, sizeof(zip_path), "%s/mobilelegend.zip", path);
                execlp("unzip", "unzip", "-q", "mobilelegend.zip", "-d", path, NULL); // mengextract file zip
                perror("execlp");
                exit(1);
            } else {
                wait(NULL); // menunggu child process selesai
                printf("Ekstraksi file zip berhasil.\n");
            }
        } else {
            printf("Download zip gagal.\n");
        }
    }    
    else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}

void remove_zip(char *path) {
    char zip_path[MAX];
    snprintf(zip_path, sizeof(zip_path), "%s/mobilelegend.zip", path);
    
    if (fork() == 0) {
        execlp("rm", "rm", "-f", "mobilelegend.zip", NULL);
        perror("execlp");
        exit(1);
    } else {
        wait(NULL);
        printf("File zip berhasil dihapus.\n");
    }
}

int read_identitas_hero(const char *csvname, Hero heroes[]) {
    FILE *file = fopen(csvname, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int count = 0;
    char line[MAX_LINE];
    bool firstLine = true;
    while (fgets(line, MAX_LINE, file) != NULL && count < MAX) {
        if (firstLine) {
        firstLine = false;
        continue;
        }

        char temp_name[40], temp_role[40];
        sscanf(line, "%[^;];%[^;];%*[^;];%*s", temp_name, temp_role);
        char *token = strtok(temp_role, "/");
        int role_count = 0;
        while (token != NULL && role_count < 2) {
            strcpy(heroes[count].namaHero, temp_name);
            strncpy(heroes[count].role[role_count], token, sizeof(heroes[count].role[role_count]) - 1);
            heroes[count].role[role_count][sizeof(heroes[count].role[role_count]) - 1] = '\0'; // memastikan akhiran string
            count++;
            token = strtok(NULL, "/");
        }
    }
    fclose(file);
    return count;
}

void calculate_point(const char *csvskill, int data_hero, Hero heroes[]) {
    FILE *skill = fopen(csvskill, "r");
    if (skill == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    int total_point = 0;
    while(fgets(line, MAX_LINE, skill) != NULL) {
        char namaHero[40];
        float hp, physical_attack, attack_speed, mana, mana_regen, attack_speed_total;

        sscanf(line, "%[^;];%f;%*f;%f;%*f;%f;%*f;%f;%f;%*f;%*f;%*f;%*f;%*f;%*f;%*f;%*f;%*f;%*f;%*f;%*f;%*f;%*f;%*f;%*f;%*f;%f", 
            namaHero, &hp, &physical_attack, &attack_speed, &mana, &mana_regen, &attack_speed_total);

        for (int i = 0; i < data_hero; i++) {
            if (strcmp(heroes[i].namaHero, namaHero) == 0) {
            for (int j = 0; j < 2 && heroes[i].role[j][0] != '\0'; j++) {
                if (strcmp(heroes[i].role[j], "TANK") == 0) {
                total_point += hp;
                } else if (strcmp(heroes[i].role[j], "ASSASSIN") == 0) {
                total_point += physical_attack;
                } else if (strcmp(heroes[i].role[j], "FIGHTER") == 0) {
                total_point += hp + physical_attack;
                } else if (strcmp(heroes[i].role[j], "MM") == 0) {
                total_point += physical_attack * attack_speed;
                } else if (strcmp(heroes[i].role[j], "MAGE") == 0) {
                total_point += mana / mana_regen;
                } else if (strcmp(heroes[i].role[j], "SUPPORT") == 0) {
                total_point += mana_regen + hp;
                }
            }
            heroes[i].skillPoints = total_point;
            }
            total_point = 0;
        }
    }
    fclose(skill);

    // untuk mencetak semua poin hero
    // for (int i = 0; i < data_hero; i++) {
    //     printf("Hero %s dengan role %s memiliki point %d\n", heroes[i].namaHero, heroes[i].role[0], heroes[i].skillPoints);
    // }
}

void best_hero(int data_hero, Hero heroes[], Hero best_heroes[6]) {
    int max_points[6] = {0}; // inisialisasi array untuk menyimpan poin maksimum untuk setiap role
    for (int i = 0; i < data_hero; i++) {
        for (int j = 0; j < 2 && heroes[i].role[j][0] != '\0'; j++) {
            if (strcmp(heroes[i].role[j], "TANK") == 0 && heroes[i].skillPoints > max_points[0]) {
                max_points[0] = heroes[i].skillPoints;
                strcpy(best_heroes[0].namaHero, heroes[i].namaHero);
                strcpy(best_heroes[0].role[0], heroes[i].role[0]);
                best_heroes[0].skillPoints = heroes[i].skillPoints;
            } else if (strcmp(heroes[i].role[j], "ASSASSIN") == 0 && heroes[i].skillPoints > max_points[1]) {
                max_points[1] = heroes[i].skillPoints;
                strcpy(best_heroes[1].namaHero, heroes[i].namaHero);
                strcpy(best_heroes[1].role[0], heroes[i].role[0]);
                best_heroes[1].skillPoints = heroes[i].skillPoints;
            } else if (strcmp(heroes[i].role[j], "FIGHTER") == 0 && heroes[i].skillPoints > max_points[2]) {
                max_points[2] = heroes[i].skillPoints;
                strcpy(best_heroes[2].namaHero, heroes[i].namaHero);
                strcpy(best_heroes[2].role[0], heroes[i].role[0]);
                best_heroes[2].skillPoints = heroes[i].skillPoints;
            } else if (strcmp(heroes[i].role[j], "MM") == 0 && heroes[i].skillPoints > max_points[3]) {
                max_points[3] = heroes[i].skillPoints;
                strcpy(best_heroes[3].namaHero, heroes[i].namaHero);
                strcpy(best_heroes[3].role[0], heroes[i].role[0]);
                best_heroes[3].skillPoints = heroes[i].skillPoints;
            } else if (strcmp(heroes[i].role[j], "MAGE") == 0 && heroes[i].skillPoints > max_points[4]) {
                max_points[4] = heroes[i].skillPoints;
                strcpy(best_heroes[4].namaHero, heroes[i].namaHero);
                strcpy(best_heroes[4].role[0], heroes[i].role[0]);
                best_heroes[4].skillPoints = heroes[i].skillPoints;
            } else if (strcmp(heroes[i].role[j], "SUPPORT") == 0 && heroes[i].skillPoints > max_points[5]) {
                max_points[5] = heroes[i].skillPoints;
                strcpy(best_heroes[5].namaHero, heroes[i].namaHero);
                strcpy(best_heroes[5].role[0], heroes[i].role[0]);
                best_heroes[5].skillPoints = heroes[i].skillPoints;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    /*NOTE
    1. sebelum menjalankan prorgam ini, pastikan sudah menginstall kaggle CLI pada terminal atau sistem Anda
    2. gunakan perintah "pip install kaggle" untuk mendownload
    3. jika sudah, silahkan jalankan program di bawah
    4. jika ingin lebih mudah dan efisien, bisa menggunakan system() untuk mendownload dan ekstrak zip  
    */

    char *ori_path = ".";
    Hero heroes[MAX];
    Hero best_heroes[6]; // inisialisasi array untuk menyimpan pahlawan terbaik untuk setiap peran

    download_extract_datasets(ori_path); // mendownload dan ekstrak zip
    remove_zip(ori_path); // menghapus zip
    
    char *csvhero = "./Data Mobile Legend/identitas.csv";
    char *csvskill = "./Data Mobile Legend/Atribut/atribut-hero.csv";
    
    int data_hero = read_identitas_hero(csvhero, heroes); // membaca dan menyimpan data hero
    calculate_point(csvskill, data_hero, heroes); // menghitung poin skill setiap hero
    best_hero(data_hero, heroes, best_heroes); // mencari hero dengan poin skill tertinggi untuk setiap role

    // periksa apakah argumen role ada
    if (argc > 1) {
        char *role = argv[1];
        printf("\nHero terbaik untuk role:\n");
        printf("-- %s --\n", role);
        for (int i = 0; i < 6; i++) {
            if (strcmp(best_heroes[i].role[0], role) == 0) {
                printf("Hero terbaik adalah %s dengan skillpoint %d\n", best_heroes[i].namaHero, best_heroes[i].skillPoints);
            }
        }
    }
    else {
        // rinr hero terbaik untuk setiap role
        printf("\nHero terbaik setiap role:\n");
        for (int i = 0; i < 6; i++) {
            printf("-- %s --\n", best_heroes[i].role[0]);
            printf("Hero terbaik adalah %s dengan skillpoint %d\n", best_heroes[i].namaHero, best_heroes[i].skillPoints);
        }
    }

    return 0;
}
