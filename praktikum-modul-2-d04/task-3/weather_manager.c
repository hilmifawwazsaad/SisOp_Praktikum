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
