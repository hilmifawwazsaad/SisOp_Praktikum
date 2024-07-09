#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

// prototipe fungsi
char *download_file(char *url, char *output_name);                                                               // fungsi untuk download file
void create_directory(const char *path);                                                                         // fungsi untuk membuat directory
void unzip_file(char *filename, char *path);                                                                     // fungsi untuk unzip file
void search_task(const char *path);                                                                              // fungsi untuk mencari task
void *yuan(void *arg);                                                                                           // fungsi untuk thread yuan
void *bubu(void *arg);                                                                                           // fungsi untuk thread bubu
void work_task_yuan(const char *yuan_path);                                                                      // fungsi untuk task yuan
void work_task_bubu(const char *bubu_path);                                                                      // fungsi untuk task bubu
void recap_data(const char *name, const char *task, int count, const char *categorie, const char *resolution);   // fungsi untuk merekap data
void total_recap_data();                                                                                         // fungsi untuk merekap total data
void finally();                                                                                                  // fungsi untuk menyelesaikan semua proses

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

    // SOAL POIN D
    total_recap_data();

    // SOAL POIN E
    finally();

    printf("Alhamdulillah Selesai Ya Allah\n");
    return 0;
}