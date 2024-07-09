#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#define MAX 1000

// POIN A DAN B
char message[MAX][MAX]; // array untuk menyimpan kata-kata
int message_length = 0; // panjang pesan

// POIN C DAN D
// struktur untuk data yang dibutuhkan setiap thread
typedef struct {
    char *word;  // Kata untuk ditulis
    FILE *fp;    // File pointer untuk file output
    int position; // Posisi kata dalam argumen
} ThreadData;

// fungsi yang akan dijalankan oleh setiap thread untuk menulis kata
void *write_word(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    fprintf(data->fp, "%s ", data->word);  // tulis kata ke file
    return NULL;
}

// fungsi yang akan dijalankan oleh thread untuk menulis pesan ke dalam file (POIN A DAN B)
void *write_msg(void *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Gagal membuka file untuk menulis");
        pthread_exit(NULL);
    }
    // menulis setiap kata ke dalam file
    for (int i = 0; i < message_length; i++) {
        fprintf(file, "%s ", message[i]);
    }

    fclose(file);
    printf("Pesan berhasil ditulis ke dalam file '%s'.\n", (char *)filename);
    pthread_exit(NULL);
}

// fungsi untuk menulis pesan ke dalam file dengan menggunakan multiple threads
void write_message(char **words, int count, char *filename) {
    pthread_t threads[count];
    ThreadData thread_data[count];
    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < count; i++) {
        thread_data[i].word = words[i];
        thread_data[i].fp = fp;
        thread_data[i].position = i;
        pthread_create(&threads[i], NULL, write_word, &thread_data[i]);
        sleep(1);  // tunggu 1 detik sebelum membuat thread baru
    }

    // tunggu semua thread selesai
    for (int i = 0; i < count; i++) {
        pthread_join(threads[i], NULL);
    }

    fclose(fp);
}

// fungsi yang akan dijalankan oleh thread untuk membaca pesan dari file (POIN A DAN B)
void *read_msg(void *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Gagal membuka file untuk dibaca");
        pthread_exit(NULL);
    }

    char word[MAX];
    printf("Pesan yang ditulis:\n");
    while (fscanf(file, "%s", word) != EOF) {
        printf("%s ", word); // membaca kata-kata dari file
    }
    printf("\n");

    fclose(file);
    pthread_exit(NULL);
}

// fungsi untuk membaca pesan dari file
void read_message(char *filename) {
    char buffer[1024];
    FILE *fp = fopen(filename, "r");
    
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    pthread_t read_thread, write_thread;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <write/read> [<message>] [<target_container>]\n", argv[0]);
        return 1;
    }
    FILE *fp;

    if (strcmp(argv[1], "write") == 0) {
        // //  POIN A DAN B
        // for (int i = 2; i < argc - 1; i++) {
        //     strcpy(message[message_length], argv[i]);
        //     message_length++;
        // }
        // if (pthread_create(&write_thread, NULL, write_msg, (void *)argv[argc - 1])) {
        //     perror("Gagal membuat thread");
        //     return 1;
        // }
        // pthread_join(write_thread, NULL); // menunggu thread selesai

        // POIN C DAN D    
        if (argc < 4) {
            fprintf(stderr, "Usage: %s write <message> <target_container>\n", argv[0]);
            return 1;
        }
        char *words[argc - 3];
        int word_count = argc - 3;
        for (int i = 0; i < word_count; i++) {
            words[i] = argv[i + 2];
        }

        // Membuat nama file dari nama kontainer tujuan
        char filename[100];
        snprintf(filename, sizeof(filename), "%s", argv[argc - 1]);

        write_message(words, word_count, filename);

        fp = fopen(filename, "a");
        if (fp == NULL) {
            perror("Error opening file");
            return 1;
        }
        
        fprintf(fp, "from %s\n", getenv("SENDER")); 

        printf("Pesan berhasil ditulis ke %s\n", argv[argc - 1]);
        printf("nama file : %s\n", filename);
    } else if (strcmp(argv[1], "read") == 0) {
        char filename[100];
        snprintf(filename, sizeof(filename), "%s", getenv("SENDER"));
        read_message(filename);
    }
    // } else if (strcmp(argv[1], "read") == 0) { // POIN A DAN B
    //     if (pthread_create(&read_thread, NULL, read_msg, (void *)argv[2])) {
    //         perror("Gagal membuat thread");
    //         return 1;
    //     }
    //     pthread_join(read_thread, NULL); // menunggu thread selesai
    // }
    else {
        fprintf(stderr, "Invalid command\n");
        return 1;
    }

    return 0;
}