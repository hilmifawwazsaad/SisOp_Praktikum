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
