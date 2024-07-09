[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/J5rciaaA)
<div align=center>

|    NRP     |      Name      |
| :--------: | :------------: |
| 5025221007 | Yehezkiella Felicia Jeis Timbulong |
| 5025221047 | Muhammad Rayyaan Fatikhahur Rakhim |
| 5025221103 | Hilmi Fawwaz Sa'ad |

# Praktikum Modul 4 _(Module 4 Lab Work)_

</div>

### Daftar Soal _(Task List)_

- [Task 1 - TrashOps](/task-1/)

- [Task 2 - Fu53enC0d3D==](/task-2/)

- [Task 3 - MengOS](/task-3/)

### Laporan Resmi Praktikum Modul 4 _(Module 4 Lab Work Report)_

Tulis laporan resmi di sini!

_Write your lab work report here!_

## 1️⃣ Soal 1
### Problem 1a
Kode untuk membuat filesystem diletakkan pada trash.c. Anda harus menggunakan fungsi getattr, readdir, read, unlink, rmdir, rename, mkdir, chown, dan chmod pada struct fuse_operations. Dibebaskan jika ingin menambahkan fungsi lain selain 9 fungsi diatas

**Jawab**

*1. Install FUSE*

Pertama, pastikan bahwa FUSE telah terinstal pada sistem.
```bash
sudo apt-get install fuse libfuse-dev
```
*2. Create the filesystem code*

Kode untuk membuat filesystem diletakkan pada trash.c. Menggunakan fungsi getattr, readdir, read, unlink, rmdir, rename, mkdir, chown, dan chmod pada struct fuse_operations.

```C
#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

static const char *trash_path = "/path/to/trash"; // Set your trash directory path here

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    res = lstat(path, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
    DIR *dp;
    struct dirent *de;
    (void)offset;
    (void)fi;
    (void)flags;
    dp = opendir(path);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0, 0))
            break;
    }
    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
    int fd;
    int res;
    (void)fi;
    fd = open(path, O_RDONLY);
    if (fd == -1)
        return -errno;
    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;
    close(fd);
    return res;
}

static int xmp_unlink(const char *path)
{
    char new_path[1024];
    snprintf(new_path, sizeof(new_path), "%s%s", trash_path, path);
    int res = rename(path, new_path);
    if (res == -1)
        return -errno;
    chmod(new_path, 0000); // Make the file unreadable, unwritable, unexecutable
    return 0;
}

static int xmp_rmdir(const char *path)
{
    char new_path[1024];
    snprintf(new_path, sizeof(new_path), "%s%s", trash_path, path);
    int res = rename(path, new_path);
    if (res == -1)
        return -errno;
    chmod(new_path, 0000); // Make the directory unreadable, unwritable, unexecutable
    return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .unlink = xmp_unlink,
    .rmdir = xmp_rmdir,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &xmp_oper, NULL);
}

```

*3. Compile the filesystem*

Compile the `trash.c` file using `gcc`

```bash
gcc -Wall trash.c `pkg-config fuse --cflags --libs` -o trash_fs
```

*4. Create and Mount the Filesystem*

Create the directories for the filesystem and the trash

```bash
mkdir /path/to/mountpoint
mkdir /path/to/trash
```

Mount the filesystem

```bash
./trash_fs /path/to/mountpoint
```

*5. Test the filesystem*

Test the filesystem by creating files and directories, then deleting them using rm and rmdir.

### Problem 1b
Ketika menggunakan perintah rm atau rmdir untuk file atau direktori yang berada diluar direktori trash, maka file atau direktori tersebut akan dipindahkan ke direktori trash dan menjadi tidak dapat diread, write, dan execute baik oleh pemilik, grup, maupun user lainnya

Barulah ketika perintah rm atau rmdir digunakan untuk file atau direktori yang berada di dalam direktori trash, maka file atau direktori tersebut akan dihapus permanen

**Jawab**

```C
#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>

static const char *trash_path = "/path/to/trash"; // Set your trash directory path here

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    res = lstat(path, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
    DIR *dp;
    struct dirent *de;
    (void)offset;
    (void)fi;
    (void)flags;
    dp = opendir(path);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0, 0))
            break;
    }
    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
    int fd;
    int res;
    (void)fi;
    fd = open(path, O_RDONLY);
    if (fd == -1)
        return -errno;
    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;
    close(fd);
    return res;
}

static int is_in_trash(const char *path)
{
    return strncmp(path, trash_path, strlen(trash_path)) == 0;
}

static int xmp_unlink(const char *path)
{
    int res;

    if (is_in_trash(path)) {
        res = unlink(path);
        if (res == -1)
            return -errno;
    } else {
        char new_path[1024];
        snprintf(new_path, sizeof(new_path), "%s%s", trash_path, path);
        res = rename(path, new_path);
        if (res == -1)
            return -errno;
        chmod(new_path, 0000); // Make the file unreadable, unwritable, unexecutable
    }

    return 0;
}

static int xmp_rmdir(const char *path)
{
    int res;

    if (is_in_trash(path)) {
        res = rmdir(path);
        if (res == -1)
            return -errno;
    } else {
        char new_path[1024];
        snprintf(new_path, sizeof(new_path), "%s%s", trash_path, path);
        res = rename(path, new_path);
        if (res == -1)
            return -errno;
        chmod(new_path, 0000); // Make the directory unreadable, unwritable, unexecutable
    }

    return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .unlink = xmp_unlink,
    .rmdir = xmp_rmdir,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &xmp_oper, NULL);
}

```

*1. Compile the filesystem*

```bash
gcc -Wall trash.c `pkg-config fuse --cflags --libs` -o trash_fs
```
*2. Create and Mount the Filesystem*

```bash
mkdir /path/to/mountpoint
mkdir /path/to/trash
./trash_fs /path/to/mountpoint
```

*3. Test the Filesystem*

Create some test files and directories in the mounted filesystem.
Use rm and rmdir to delete files and directories inside and outside the trash directory.
Verify that files and directories outside the trash directory are moved to the trash and have their permissions changed.
Verify that files and directories inside the trash directory are permanently deleted.

### Problem 1c
File atau direktori yang berada pada direktori trash tidak dapat diubah permission dan kepemilikannya, serta tidak dapat direname

**Jawab**

```C
#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>

static const char *trash_path = "/path/to/trash"; // Set your trash directory path here

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    res = lstat(path, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
    DIR *dp;
    struct dirent *de;
    (void)offset;
    (void)fi;
    (void)flags;
    dp = opendir(path);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0, 0))
            break;
    }
    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
    int fd;
    int res;
    (void)fi;
    fd = open(path, O_RDONLY);
    if (fd == -1)
        return -errno;
    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;
    close(fd);
    return res;
}

static int is_in_trash(const char *path)
{
    return strncmp(path, trash_path, strlen(trash_path)) == 0;
}

static int xmp_unlink(const char *path)
{
    int res;

    if (is_in_trash(path)) {
        res = unlink(path);
        if (res == -1)
            return -errno;
    } else {
        char new_path[1024];
        snprintf(new_path, sizeof(new_path), "%s%s", trash_path, path);
        res = rename(path, new_path);
        if (res == -1)
            return -errno;
        chmod(new_path, 0000); // Make the file unreadable, unwritable, unexecutable
    }

    return 0;
}

static int xmp_rmdir(const char *path)
{
    int res;

    if (is_in_trash(path)) {
        res = rmdir(path);
        if (res == -1)
            return -errno;
    } else {
        char new_path[1024];
        snprintf(new_path, sizeof(new_path), "%s%s", trash_path, path);
        res = rename(path, new_path);
        if (res == -1)
            return -errno;
        chmod(new_path, 0000); // Make the directory unreadable, unwritable, unexecutable
    }

    return 0;
}

static int xmp_rename(const char *from, const char *to, unsigned int flags)
{
    if (is_in_trash(from))
        return -EPERM; // Operation not permitted
    return rename(from, to);
}

static int xmp_chmod(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    (void)fi;
    if (is_in_trash(path))
        return -EPERM; // Operation not permitted
    int res = chmod(path, mode);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid, struct fuse_file_info *fi)
{
    (void)fi;
    if (is_in_trash(path))
        return -EPERM; // Operation not permitted
    int res = lchown(path, uid, gid);
    if (res == -1)
        return -errno;
    return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .unlink = xmp_unlink,
    .rmdir = xmp_rmdir,
    .rename = xmp_rename,
    .chmod = xmp_chmod,
    .chown = xmp_chown,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
```

*1. Compile the filesystem*

```bash
gcc -Wall trash.c `pkg-config fuse --cflags --libs` -o trash_fs
```
*2. Create and Mount the Filesystem*

```bash
mkdir /path/to/mountpoint
mkdir /path/to/trash
./trash_fs /path/to/mountpoint
```

*3. Test the Filesystem*

Create some test files and directories in the mounted filesystem.
Use rm and rmdir to delete files and directories inside and outside the trash directory.
Try renaming, changing permissions, and changing ownership of files and directories within the trash directory to ensure these operations are not permitted.
Verify that files and directories outside the trash directory are moved to the trash and have their permissions changed.
Verify that files and directories inside the trash directory are permanently deleted.

### Problem 1d
Untuk memulihkan file atau direktori dari direktori trash, anda harus menggunakan perintah mv dengan format mv [path_file_dalam_trash] [arg]. Opsi pertama untuk arg adalah dengan path biasa sehingga file atau direktori akan dipindahkan dari direktori trash ke path tersebut. Opsi kedua untuk arg adalah restore sehingga file atau direktori akan kembali ke path asal sebelum ia dipindah ke trash. Permission untuk file atau direktori yang dipulihkan harus kembali seperti sebelum dimasukkan ke trash. Khusus untuk arg restore anda harus membuat path yang sesuai apabila path asal tidak ada (terhapus/dipindah)

**Jawab**

*1. Define a structure to store metadata*

```C
#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include <stdlib.h>
#include <dirent.h>
```

*2. Utility function to write metadata*

```C
static int write_metadata(const char *path, const char *original_path, mode_t mode, uid_t uid, gid_t gid) {
    char metadata_path[1024];
    snprintf(metadata_path, sizeof(metadata_path), "%s.metadata", path);

    FILE *fp = fopen(metadata_path, "w");
    if (!fp)
        return -errno;

    fprintf(fp, "path=%s\nmode=%o\nuid=%d\ngid=%d\n", original_path, mode, uid, gid);
    fclose(fp);
    return 0;
}
```
*3. Utility function to read metadata*

```C
static int read_metadata(const char *path, char *original_path, mode_t *mode, uid_t *uid, gid_t *gid) {
    char metadata_path[1024];
    snprintf(metadata_path, sizeof(metadata_path), "%s.metadata", path);

    FILE *fp = fopen(metadata_path, "r");
    if (!fp)
        return -errno;

    fscanf(fp, "path=%s\nmode=%o\nuid=%d\ngid=%d\n", original_path, mode, uid, gid);
    fclose(fp);
    return 0;
}
```

*4. Update unlink and rmdir operations to save metadata*

```C
static int xmp_unlink(const char *path) {
    int res;

    if (is_in_trash(path)) {
        res = unlink(path);
        if (res == -1)
            return -errno;
    } else {
        char new_path[1024];
        snprintf(new_path, sizeof(new_path), "%s%s", trash_path, path);

        struct stat st;
        if (lstat(path, &st) == -1)
            return -errno;

        res = rename(path, new_path);
        if (res == -1)
            return -errno;

        chmod(new_path, 0000); // Make the file unreadable, unwritable, unexecutable

        write_metadata(new_path, path, st.st_mode, st.st_uid, st.st_gid);
    }

    return 0;
}

static int xmp_rmdir(const char *path) {
    int res;

    if (is_in_trash(path)) {
        res = rmdir(path);
        if (res == -1)
            return -errno;
    } else {
        char new_path[1024];
        snprintf(new_path, sizeof(new_path), "%s%s", trash_path, path);

        struct stat st;
        if (lstat(path, &st) == -1)
            return -errno;

        res = rename(path, new_path);
        if (res == -1)
            return -errno;

        chmod(new_path, 0000); // Make the directory unreadable, unwritable, unexecutable

        write_metadata(new_path, path, st.st_mode, st.st_uid, st.st_gid);
    }

    return 0;
}
```

*5. Implement restore functionality*

```C
static int xmp_rename(const char *from, const char *to, unsigned int flags) {
    if (is_in_trash(from)) {
        if (strcmp(to, "restore") == 0) {
            char original_path[1024];
            mode_t mode;
            uid_t uid;
            gid_t gid;

            if (read_metadata(from, original_path, &mode, &uid, &gid) == -1)
                return -errno;

            if (rename(from, original_path) == -1)
                return -errno;

            if (chmod(original_path, mode) == -1)
                return -errno;

            if (chown(original_path, uid, gid) == -1)
                return -errno;

            char metadata_path[1024];
            snprintf(metadata_path, sizeof(metadata_path), "%s.metadata", from);
            unlink(metadata_path); // Remove metadata file

            return 0;
        }
    }

    return rename(from, to);
}
```

*6. Mount the filesystem with FUSE operations*

```C
static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .unlink = xmp_unlink,
    .rmdir = xmp_rmdir,
    .rename = xmp_rename,
    .chmod = xmp_chmod,
    .chown = xmp_chown,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
```

*1. Compile the filesystem*

```bash
gcc -Wall trash.c `pkg-config fuse --cflags --libs` -o trash_fs
```
*2. Create and Mount the Filesystem*

```bash
mkdir /path/to/mountpoint
mkdir /path/to/trash
./trash_fs /path/to/mountpoint
```

*3. Test the Filesystem*

Create some test files and directories in the mounted filesystem.
Use rm and rmdir to delete files and directories inside and outside the trash directory.
Use the mv command to test restoring files and directories from the trash directory.
Verify that files and directories are restored to their original paths and permissions.

### Problem 1e
Direktori trash tidak dapat dihapus, dipindah, direname. Anda juga tidak dapat membuat direktori dengan nama trash atau restore

**Jawab**

*1. Prevent the trash directory from being deleted, moved, or renamed*

Fungsi `xmp_unlink`, `xmp_rmdir`, dan `xmp_rename` memeriksa apakah path yang dimaksud adalah `trash_path`.
Jika ya, menulis log bahwa operasi tersebut gagal dan mengembalikan error `-EPERM` (Operation not permitted).

```C
static int xmp_unlink(const char *path) {
    if (strcmp(path, trash_path) == 0) {
        write_log("FAILED TO REMOVE TRASH DIRECTORY");
        return -EPERM;
    }
    // Existing unlink code...
}

static int xmp_rmdir(const char *path) {
    if (strcmp(path, trash_path) == 0) {
        write_log("FAILED TO REMOVE TRASH DIRECTORY");
        return -EPERM;
    }
    // Existing rmdir code...
}

static int xmp_rename(const char *from, const char *to, unsigned int flags) {
    if (strcmp(from, trash_path) == 0 || strcmp(to, trash_path) == 0) {
        write_log("FAILED TO RENAME TRASH DIRECTORY");
        return -EPERM;
    }
    // Existing rename code...
}
```

*2. Prevent creation of directories named "trash" or "restore"*

Fungsi `xmp_mkdir` memeriksa apakah path yang dimaksud mengandung nama `trash` atau `restore`.
Jika ya, menulis log bahwa operasi tersebut gagal dan mengembalikan error `-EPERM`.

```C
static int xmp_mkdir(const char *path, mode_t mode) {
    if (strstr(path, "/trash") != NULL || strstr(path, "/restore") != NULL) {
        write_log("FAILED TO CREATE TRASH/RESTORE DIRECTORY");
        return -EPERM;
    }
    return mkdir(path, mode);
}
```

*3. Mount the filesystem with FUSE operations*

Mendefinisikan struktur `fuse_operations` dengan fungsi-fungsi yang telah kita definisikan sebelumnya.
Memanggil `fuse_main` untuk menjalankan filesystem dengan operasi yang telah didefinisikan.

```C
static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .unlink = xmp_unlink,
    .rmdir = xmp_rmdir,
    .rename = xmp_rename,
    .chmod = xmp_chmod,
    .chown = xmp_chown,
    .mkdir = xmp_mkdir,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
```

*1. Compile the filesystem*

```bash
gcc -Wall trash.c `pkg-config fuse --cflags --libs` -o trash_fs
```
*2. Create and Mount the Filesystem*

```bash
mkdir /path/to/mountpoint
mkdir /path/to/trash
touch /path/to/trash.log
./trash_fs /path/to/mountpoint
```

*3. Test the Filesystem*

Create some test files and directories in the mounted filesystem.
Use rm and rmdir to delete files and directories inside and outside the trash directory.
Use the mv command to test restoring files and directories from the trash directory.
Verify that logs are correctly written to trash.log for all actions.

### Problem 1f
Catatlah log pada file trash.log. Format untuk tiap baris log adalah YYMMDD-HH:MM:SS KETERANGAN. Format keterangan akan berdasarkan aksi sesuai soal 1f

**Jawab**

*1. Include necessary libraries and define paths*

Mengatur versi FUSE yang digunakan (FUSE_USE_VERSION 30).
Mengimpor pustaka yang dibutuhkan seperti fuse.h, stdio.h, string.h, dll.
Mendefinisikan trash_path dan log_path sebagai lokasi direktori trash dan file log.

```C
#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>

static const char *trash_path = "/path/to/trash";
static const char *log_path = "/path/to/trash.log";
```

*2. Utility function to write logs*

Fungsi write_log menulis pesan log ke file trash.log.
Membuka file trash.log dalam mode append.
Mendapatkan waktu saat ini dan memformatnya.
Menulis waktu dan deskripsi log ke file.
Menutup file log.

```C
static void write_log(const char *description) {
    FILE *log_fp = fopen(log_path, "a");
    if (log_fp == NULL) {
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%y%m%d-%H:%M:%S", t);

    fprintf(log_fp, "%s %s\n", time_str, description);
    fclose(log_fp);
}
```

*3. Update logging in existing functions*

Menambahkan logging di dalam fungsi `xmp_unlink`, `xmp_rmdir`, dan `xmp_rename` untuk mencatat aksi yang dilakukan, seperti memindahkan ke trash, merestorasi dari trash, atau gagal melakukan operasi.

```C
static int xmp_unlink(const char *path) {
    // Existing code...
    char description[1024];
    snprintf(description, sizeof(description), "MOVED %s TO TRASH", path);
    write_log(description);
    // Remaining code...
}

static int xmp_rmdir(const char *path) {
    // Existing code...
    char description[1024];
    snprintf(description, sizeof(description), "MOVED %s TO TRASH", path);
    write_log(description);
    // Remaining code...
}

static int xmp_rename(const char *from, const char *to, unsigned int flags) {
    // Existing code...
    char description[1024];
    if (is_in_trash(from)) {
        if (strcmp(to, "restore") == 0) {
            char original_path[1024];
            mode_t mode;
            uid_t uid;
            gid_t gid;

            read_metadata(from, original_path, &mode, &uid, &gid);
            snprintf(description, sizeof(description), "RESTORED %s FROM TRASH TO %s", from, original_path);
            write_log(description);
            // Remaining code...
        } else {
            snprintf(description, sizeof(description), "FAILED TO RENAME %s", from);
            write_log(description);
            return -EPERM;
        }
    } else {
        snprintf(description, sizeof(description), "MOVED %s TO %s", from, to);
        write_log(description);
    }
    // Remaining code...
}
```

*4. Log permission and ownership changes failures*

Fungsi `xmp_chmod` dan `xmp_chown` memeriksa apakah file atau direktori berada di dalam trash.
Jika ya, menulis log bahwa operasi tersebut gagal dan mengembalikan error `-EPERM`.

```C
static int xmp_chmod(const char *path, mode_t mode) {
    if (is_in_trash(path)) {
        char description[1024];
        snprintf(description, sizeof(description), "FAILED TO CHMOD %s", path);
        write_log(description);
        return -EPERM;
    }
    return chmod(path, mode);
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid) {
    if (is_in_trash(path)) {
        char description[1024];
        snprintf(description, sizeof(description), "FAILED TO CHOWN %s", path);
        write_log(description);
        return -EPERM;
    }
    return chown(path, uid, gid);
}
```

*1. Compile the filesystem*

```bash
gcc -Wall trash.c `pkg-config fuse --cflags --libs` -o trash_fs
```
*2. Create and Mount the Filesystem*

```bash
mkdir /path/to/mountpoint
mkdir /path/to/trash
touch /path/to/trash.log
./trash_fs /path/to/mountpoint
```

*3. Test the Filesystem*

Create some test files and directories in the mounted filesystem.
Use rm and rmdir to delete files and directories inside and outside the trash directory.
Use the mv command to test restoring files and directories from the trash directory.
Verify that logs are correctly written to trash.log for all actions.

### Kendala

Lumayan susah dan tidak bisa berjalan dengan benar

## 2️⃣ Soal 2
### Problem 2a
Clone direktori target dan buatlah 4 user pada linux dengan username sesuai isi folder pada direktori target (password dibebaskan)
**Jawab**

clone target yang berisikan user
![image](https://github.com/sisop-its-s24/praktikum-modul-4-d04/assets/112644900/3e1a9959-6501-4169-8943-bf846e081929)
lalu tambahkan user menggunakan
![image](https://github.com/sisop-its-s24/praktikum-modul-4-d04/assets/112644900/188a0c04-0b4d-45d1-9d36-fa6d04c88aa1)
sehingga user akan tertambahkan pada bagian Ubuntu/home
![image](https://github.com/sisop-its-s24/praktikum-modul-4-d04/assets/112644900/177cd3fd-724c-45c4-be35-fb9c23a2d4a8)

### Problem 2b
Ketika folder mount point dari fuse yang kalian buat diakses, akan langsung menuju ke dalam target folder yang telah di clone (tidak dimulai dari root)

**Jawab**
agar folder mount point dari fuse langsung menuju ke dalam target folder yang telah diclone maka kita perlu membuat
```C
#define TARGET_DIR "/usr/operating-system/praktikum-modul-4-d04/task-2/target"
```
sehingga target directory akan langsung menuju pada target
![image](https://github.com/sisop-its-s24/praktikum-modul-4-d04/assets/112644900/dd40b7e0-0c53-43e6-b34a-fbe3d9b9864c)
dan didalam mountpoint akan beriiskan directory target yang sudah diclone sebelumnya
### Problem 2c
Buatlah agar tiap user tidak dapat menambah, mengubah, maupun menghapus folder maupun file dalam folder yang bukan miliknya

**Jawab**

agar tiap user tidak dapat menambah, mengubah, maupun menghapus folder maupun file dalam folder yang bukan miliknya, maka kita perlu membuat handler yang membatasi user ketika mengakses directory dari user lain untuk tidak dapat mengutak atik dengan rincian yang telah disebutkan

- membuat directory
```C
static int fuse_mkdir(const char *path, mode_t mode) {
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    if (!has_access(username, full_path))
        return -EACCES;

    int res = mkdir(full_path, mode);
    if (res == -1)
        return -errno;

    return 0;
}

```
- menghapus directory
```C
static int fuse_rmdir(const char *path) {
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    if (!has_access(username, full_path))
        return -EACCES;

    int res = rmdir(full_path);
    if (res == -1)
        return -errno;

    return 0;
}


```

- membuat file
```C
static int fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    if (!has_access(username, full_path))
        return -EACCES;

    int fd = open(full_path, fi->flags, mode);
    if (fd == -1)
        return -errno;

    fi->fh = fd;
    return 0;
}
```

- menghapus file
```C
static int fuse_unlink(const char *path) {
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    if (!has_access(username, full_path))
        return -EACCES;

    int res = unlink(full_path);
    if (res == -1)
        return -errno;

    return 0;
}
```

- Mengganti nama file atau direktori
```C
static int fuse_rename(const char *oldpath, const char *newpath, unsigned int flags) {
    (void) flags;
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_oldpath[PATH_MAX];
    char full_newpath[PATH_MAX];
    construct_full_path(oldpath, full_oldpath);
    construct_full_path(newpath, full_newpath);

    if (!has_access(username, full_oldpath) || !has_access(username, full_newpath))
        return -EACCES;

    int res = rename(full_oldpath, full_newpath);
    if (res == -1)
        return -errno;

    return 0;
}

```

- Menulis ke file
```C
static int fuse_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int fd = fi->fh;
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    if (!has_access(username, full_path))
        return -EACCES;

    ssize_t res = pwrite(fd, buf, size, offset);
    if (res == -1)
        return -errno;

    return res;
}
```

- Memotong ukuran file
```C
static int fuse_truncate(const char *path, off_t size, struct fuse_file_info *fi) {
    (void)fi;
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    if (!has_access(username, full_path))
        return -EACCES;

    int res = truncate(full_path, size);
    if (res == -1)
        return -errno;

    return 0;
}

```
Pada setiap function tersebut selalu terdapat
```C

    if (!has_access(username, full_path))
        return -EACCES;
```
yang mana akan membatasi user yang tidak memiliki akses agar tidak bisa melakukan operasi seperti yang disebutkan tadi.
![image](https://github.com/sisop-its-s24/praktikum-modul-4-d04/assets/112644900/3dd01e77-d25f-4d3f-a70f-14438889e47e)
Contoh apabila user budi mencoba membuat directory pada user cony maka akan muncul pesan bahwa tidak bisa 


### Problem 2d
Buatlah agar user dapat menambah, mengubah, maupun menghapus folder maupun file dalam folder miliknya

**Jawab**

```C
int has_access(const char *username, const char *path) {
    char target_dir[PATH_MAX];
    snprintf(target_dir, PATH_MAX, "%s/%s", TARGET_DIR, username);
    return strstr(path, target_dir) == path;
}
```

- membuat directory
```C
static int fuse_mkdir(const char *path, mode_t mode) {
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    if (!has_access(username, full_path))
        return -EACCES;

    int res = mkdir(full_path, mode);
    if (res == -1)
        return -errno;

    return 0;
}

```
- menghapus directory
```C
static int fuse_rmdir(const char *path) {
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    if (!has_access(username, full_path))
        return -EACCES;

    int res = rmdir(full_path);
    if (res == -1)
        return -errno;

    return 0;
}


```

- membuat file
```C
static int fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    if (!has_access(username, full_path))
        return -EACCES;

    int fd = open(full_path, fi->flags, mode);
    if (fd == -1)
        return -errno;

    fi->fh = fd;
    return 0;
}
```

- menghapus file
```C
static int fuse_unlink(const char *path) {
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    if (!has_access(username, full_path))
        return -EACCES;

    int res = unlink(full_path);
    if (res == -1)
        return -errno;

    return 0;
}
```

- Mengganti nama file atau direktori
```C
static int fuse_rename(const char *oldpath, const char *newpath, unsigned int flags) {
    (void) flags;
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_oldpath[PATH_MAX];
    char full_newpath[PATH_MAX];
    construct_full_path(oldpath, full_oldpath);
    construct_full_path(newpath, full_newpath);

    if (!has_access(username, full_oldpath) || !has_access(username, full_newpath))
        return -EACCES;

    int res = rename(full_oldpath, full_newpath);
    if (res == -1)
        return -errno;

    return 0;
}

```

- Menulis ke file
```C
static int fuse_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int fd = fi->fh;
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    if (!has_access(username, full_path))
        return -EACCES;

    ssize_t res = pwrite(fd, buf, size, offset);
    if (res == -1)
        return -errno;

    return res;
}
```

- Memotong ukuran file
```C
static int fuse_truncate(const char *path, off_t size, struct fuse_file_info *fi) {
    (void)fi;
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    if (!has_access(username, full_path))
        return -EACCES;

    int res = truncate(full_path, size);
    if (res == -1)
        return -errno;

    return 0;
}

```
Melalui function
```C
int has_access(const char *username, const char *path) {
    char target_dir[PATH_MAX];
    snprintf(target_dir, PATH_MAX, "%s/%s", TARGET_DIR, username);
    return strstr(path, target_dir) == path;
}
```
dan diimplementasikan pada function lainnya akan mengecek status user apakah memiliki akses atau tidak pada directory tersebut.
![image](https://github.com/sisop-its-s24/praktikum-modul-4-d04/assets/112644900/00b9eb69-3cd1-4dac-b55f-80a816a38db6)


### Problem 2e
Semua isi file akan ter-encode jika diakses oleh selain user pemilik folder tersebut (menggunakan encoding Base64)

**Jawab**
```C
static const char base64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
```
kita dapat membuat encode 64 dengan inisiasi char base64
```C
char *base64_encode(const unsigned char *data, size_t input_length) {
    char *encoded_data;
    size_t output_length = 4 * ((input_length + 2) / 3);
    encoded_data = malloc(output_length + 1);
    if (encoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = base64_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = base64_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = base64_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = base64_table[(triple >> 0 * 6) & 0x3F];
    }

    // Pad the output with '=' characters if necessary
    for (size_t i = 0; i < (3 - input_length % 3) % 3; i++) {
        encoded_data[output_length - 1 - i] = '=';
    }
    encoded_data[output_length] = '\0';

    return encoded_data;
}
```
lalu melalui base64 encode akan diubah data file yang sebelumnya agar ter encode

```C
// FUSE callback to read data from a file
static int fuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int fd = fi->fh;
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    ssize_t res = pread(fd, buf, size, offset);
    if (res == -1)
        return -errno;

    // If the user is not the owner, encode the data with Base64
    char full_path[PATH_MAX];
    construct_full_path(path, full_path);
    if (!has_access(username, full_path)) {
        char *encoded_data = base64_encode((unsigned char *)buf, res);
        if (encoded_data) {
            memcpy(buf, encoded_data, strlen(encoded_data));
            free(encoded_data);
            res = strlen(encoded_data); // Update the result size to encoded data size
        }
    }

    return res;
}
```
- Setelah membaca data dari file menggunakan pread, hasilnya disimpan di buffer buf.
- Jika user yang mengakses file bukanlah pemilik folder (!has_access(username, full_path)), maka data tersebut akan di-encode menggunakan fungsi base64_encode.
- Data yang sudah di-encode kemudian ditulis kembali ke buffer buf, dan ukuran hasil pembacaan (res) diperbarui dengan ukuran data yang sudah di-encode.
Namun disini masih belum bisa ketika user lain mengakses file lain atau membaca justru muncul pesan `permission denied`
![image](https://github.com/sisop-its-s24/praktikum-modul-4-d04/assets/112644900/f66971aa-fa37-4815-b757-296748053bf4)

### Problem 2f
Sebaliknya, file akan ter-decode ketika diakses oleh user pemilik folder tersebut

**Jawab**
```C
unsigned char *base64_decode(const char *data, size_t input_length, size_t *output_length) {
    // Jika panjang input bukan kelipatan dari 4, maka data tidak valid untuk Base64, sehingga fungsi mengembalikan NULL.
    if (input_length % 4 != 0) return NULL;

    // Menghitung panjang output yang dihasilkan dari dekode Base64.
    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--; // Mengurangi panjang jika ada padding '=' di akhir.
    if (data[input_length - 2] == '=') (*output_length)--;

    // Mengalokasikan memori untuk menyimpan data yang telah di-decode.
    unsigned char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    // Mengubah setiap 4 karakter Base64 menjadi 3 byte asli.
    for (size_t i = 0, j = 0; i < input_length;) {
        // Mengambil nilai setiap sextet (6-bit grup) dari Base64 karakter.
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : base64_table[strchr(base64_table, data[i++]) - base64_table];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : base64_table[strchr(base64_table, data[i++]) - base64_table];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : base64_table[strchr(base64_table, data[i++]) - base64_table];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : base64_table[strchr(base64_table, data[i++]) - base64_table];

        // Menggabungkan sextet menjadi triple (3 byte asli).
        uint32_t triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

        // Memecah triple kembali menjadi byte dan menyimpannya dalam data yang di-decode.
        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    // Mengembalikan pointer ke data yang telah di-decode.
    return decoded_data;
}
```
code ini dapat mengubah data dalam file agar ter decode ketika user tersebut yang mengakses file yang ada dalam akses miliknya
![image](https://github.com/sisop-its-s24/praktikum-modul-4-d04/assets/112644900/076ea383-f3d2-4c12-a5d8-04bfda9e1f09)


### Kendala
- ketika mengerjakan nomer 2e masih belum bisa melakukan encode padahal dari codenya sudah ada, karena ketika dicoba yang muncul justru `permission denied` meskipun hanya melakukan `cat cagedBird.txt` kemungkinan masalah ada pada permission yang membatasi konteks open sehingga user lain tidak dapat membuka file dari user lain

## 3️⃣ Soal 3
### Problem 3a
Implementasikan fungsi `printString` pada `kernel.c` yang akan menampilkan string ke layar.

Implementasi dapat menggunakan fungsi `interrupt` dengan service `int 10h` dengan parameter `AH = 0x0E` untuk teletype output. Karakter yang ditampilkan dapat dimasukkan pada register `AL`. Fungsi ini akan menampilkan string karakter ASCII (null-terminated) ke layar

**Jawab**

*1. Persiapan environment untuk bochs di WSL*

Sebelumnya, hapus paket-paket perangkat lunak `bochs`, `bximage`, dan `bochsbios` dari sistem Linux.
```bash
sudo apt remove bochs bximage bochsbios
```
Setelah itu, install `alien` (untuk mengkonversi paket-paket perangkat lunak antara format paket yang berbeda) dan `rpm`
```bash
sudo apt install alien rpm
```
Setelah menginstall `alien` dan `rpm`, selanjutnya melakukan download bochs dengan ekstensi file `.rpm`. Bochs bisa didownload pada link https://sourceforge.net/projects/bochs/files/bochs/2.8/. Pastikan Anda mendownload bochs yang bernama `bochs-2.8-1.x86_64.rpm`

Setelah download file selesai, pindahkan file tersebut ke tempat yang bisa diakses oleh terminal Linux/WSL/Ubuntu Anda. Anda bisa membuat directory `bochs` untuk menyimpan file `bochs-2.8-1.x86_64.rpm` tersebut. Jika sudah, install paket `.rpm` tersebut pada sistem Linux.
```bash
sudo alien -i bochs-2.8-1.x86_64.rpm
```
*2. Install `NASM`, `BCC`, `ld86`, dan `make`*
```bash
sudo apt install nasm
```
```bash
sudo apt install bcc
```
```bash
sudo apt install bin86
```
```bash
sudo apt install make
```
*3. Install library `SDL2`. `SDL2` adalah library multimedia yang banyak digunakan dalam pengembangan aplikasi yang memerlukan akses ke hardware tingkat rendah. Fungsinya untuk menyediakan serangkaian fungsi yang memungkinkan pengembang untuk mengakses perangkat keras seperti grafis, suara, input, dan lainnya dengan cara yang mudah dan platform-independen*
```bash
sudo apt install libsdl2-2.0-0
```

*4. Install library `libsamplerate`. Fungsi dari library tersebut untuk menyediakan alat yang efisien dan berkualitas tinggi untuk mengubah laju sampel (sampling rate) dari sinyal audio*
```bash
sudo apt install libsamplerate0
```

*5. Selanjutnya, membuat program fungsi `printString` pada `kernel.c`*
```C
void printString(char* str) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == '\n') {
            // Jika karakter newline ditemukan, tambahkan carriage return dan newline
            interrupt(0x10, 0x0E << 8 | '\r', 0, 0, 0);
            interrupt(0x10, 0x0E << 8 | '\n', 0, 0, 0);
        } else {
            // Menampilkan karakter yang dibaca
            interrupt(0x10, 0x0E << 8 | str[i], 0, 0, 0);
        }
        i++;
    }
}
```
- Fungsi tersebut menerima satu parameter yaitu pointer `char` yang menunjuk ke string yang akan ditampilkan
- `int i = 0` digunakan sebagai penghitung indeks untuk mengakses setiap karakter dalam string
- Loop `while` akan terus berjalan  selama karakter yang diakses pada indeks `i` bukan merupakan null terminator (`\0`). Null terminator menandakan akhir dari string C
- `if (str[i] == '\n')` digunakan untuk mengecek apakah karakter yang dibaca adalah newline (`\n`). Jika iya, dua interrupt BIOS dipanggil untuk menambahkan carriage return (`\r`) dan newline (`\n`) ke layar yang berguna untuk memastikan bahwa posisi kursor pindah ke awal baris berikutnya. Jika tidak, karakter tersebut akan ditampilkan pada layar
- `0x10` merupakan interrupt BIOS yang digunakan untuk layanan video/display
- `0x0E` digunakan untuk menampilkan karakter pada mode teks dengan menampilkan karakter pada posisi kursor saat ini dan menggerakkan kursor ke posisi berikutnya
- `<< 8` digunakan untuk  menggeser nilai `0x0E` ke kiri sebanyak 8 bit, sehingga nilai tersebut ditempatkan pada byte yang lebih tinggi dalam register yang digunakan untuk parameter interrupt BIOS

### Problem 3b
Implementasikan fungsi `readString` pada `kernel.c` yang akan membaca string dari keyboard.

Implementasi dapat menggunakan fungsi `interrupt` dengan service `int 16h` dengan parameter `AH = 0x00` untuk keyboard input. Fungsi ini akan membaca karakter ASCII (non-control) yang dimasukkan oleh pengguna dan menyimpannya pada buffer hingga menekan tombol `Enter`. Handle tombol `Backspace` dibebaskan kepada praktikan

**Jawab**

```C
void readString(char* buf) {
    int i = 0;
    char input;
    char capsLock = 0; // Variabel untuk menandai status Caps Lock

    while (1) {
        input = interrupt(0x16, 0, 0, 0, 0); // Baca karakter dari keyboard

        // Periksa apakah Caps Lock ditekan
        if (input == 0x3A) {
            capsLock = !capsLock; // Toggle status Caps Lock
            continue; // Lanjutkan ke iterasi berikutnya
        }

        // Jika Caps Lock aktif, ubah huruf menjadi huruf kapital
        if (capsLock && input >= 'a' && input <= 'z') {
            input -= 32; // Ubah ke huruf kapital
        }

        if (input == '\r') { // Jika tombol Enter ditekan
            buf[i] = '\0'; // Menambahkan null-terminator pada akhir string
            // Tambahkan carriage return dan newline
            interrupt(0x10, 0x0E << 8 | '\r', 0, 0, 0);
            interrupt(0x10, 0x0E << 8 | '\n', 0, 0, 0);
            break; // Keluar dari loop
        } else if (input == '\b') { // Jika tombol Backspace ditekan
            if (i > 0) { // Pastikan ada karakter untuk dihapus
                i--; // Pindah ke indeks sebelumnya
                // Hapus karakter dari layar dengan mencetak spasi dan kembali
                interrupt(0x10, 0x0E << 8 | '\b', 0, 0, 0);
                interrupt(0x10, 0x0E << 8 | ' ', 0, 0, 0);
                interrupt(0x10, 0x0E << 8 | '\b', 0, 0, 0);
            }
        } else if (input >= ' ' && input <= '~') { // Karakter ASCII yang valid
            buf[i] = input; // Menyimpan karakter pada buffer
            interrupt(0x10, 0x0E << 8 | input, 0, 0, 0); // Menampilkan karakter yang dibaca
            i++; // Pindah ke indeks berikutnya
        }
    }
}
```
- Fungsi tersebut menerima satu parameter yaitu pointer `char` yang menunjuk ke buffer tempat string akan disimpan
- `int i = 0` digunakan sebagai penghitung indeks untuk mengakses setiap posisi dalam buffer
- `char input` digunakan untuk menyimpan karakter yang dibaca dari keyboard
- `char capsLock = 0` digunakan untuk menandai status Caps Lock (aktif atau tidak)
- Loop `while (1)` akan selalu berjalan sampai pengguna menekan tombol enter
- `input = interrupt(0x16, 0, 0, 0, 0)` merupakan pemanggilan interrupt BIOS untuk membaca karakter dari keyboard
- `if (input == 0x3A)` digunakan untuk mengecek apakah Caps Lock ditekan. Jika iya, status Caps Lock di-toggle (diubah dari aktif menjadi tidak aktif atau sebaliknya)
- `if (capsLock && input >= 'a' && input <= 'z')` digunakan untuk mengecek apakah kondisi `capsLock` aktif dan karakter yang dibaca adalah huruf kecil. Jika iya,  karakter tersebut diubah menjadi huruf kapital dengan mengurangi nilai ASCII sebesar 32 (`input -= 32`)
- `if (input == '\r')` digunakan untuk mengecek apakah tombol Enter ditekan. Jika iya, null-terminator (`\0`) ditambahkan pada akhir string dalam buffer serta carriage return (`\r`) dan newline (`\n`) ditambahkan ke layar menggunakan interrupt BIOS untuk mengakhiri baris input
- `else if (input == '\b')`digunakan untuk mengecek apakah tombol Backspace ditekan. Jika iya, karakter akan dihapus dari layar dengan mencetak spasi di posisi kursor saat ini dan menggerakkan kursor kembali ke posisi sebelumnya
- `else if (input >= ' ' && input <= '~')` digunakan untuk mengecek apakah karakter yang dibaca valid. Jika iya, karakter tersebut disimpan dalam buffer dan ditampilkan pada layar dan indek `i` increment untuk menyimpan karakter berikutnya

### Problem 3c
Implementasikan fungsi `clearScreen` pada `kernel.c` yang akan membersihkan layar.

Ukuran layar adalah `80x25` karakter. Setelah layar dibersihkan, kursor akan kembali ke posisi awal yaitu `(0, 0)` dan buffer video untuk warna karakter diubah ke warna putih. Implementasi dapat menggunakan fungsi `interrupt` dengan service `int 10h` atau menggunakan fungsi `putInMemory` untuk mengisi memori video

**Jawab**

```C
void clearScreen() {
    int i;

    // Membersihkan layar dengan mengisi setiap karakter dengan spasi
    for (i = 0; i < 80 * 25; i++) {
        putInMemory(0xB800, i * 2, ' '); // Set karakter ke spasi
        putInMemory(0xB800, i * 2 + 1, 0x07); // Atur warna karakter ke putih
    }

    // Set kursor ke posisi awal (0, 0)
    interrupt(0x10, 0x02 << 8, 0, 0, 0);
}
```
- `int i` merupakan variabel `i` yang digunakan sebagai penghitung untuk iterasi melalui setiap karakter pada layar
- Dalam loop `for (i = 0; i < 80 * 25; i++)`, setiap karakter pada layar diisi dengan spasi
- `putInMemory` digunakan untuk menempatkan karakter spasi ke lokasi memori video VGA yang sesuai
- `0xB800` merupakan alamat memori VGA
- `i * 2` merupakan offset untuk karakter. Karakter ditempatkan pada setiap offset genap karena setiap karakter dalam mode teks diikuti oleh byte atribut warna di lokasi offset ganjil
- `i * 2 + 1` merupakan offset untuk karakter ganjil. Dalam kasus ini digunakan untuk mengatur atribut warna menjadi putih
- `0x07` digunakan untuk mewakili warna putih pada layar VGA dalam mode teks
- `0x02 << 8` digunakan untuk menunjukkan fungsi interrupt BIOS untuk mengatur posisi kursor

### Problem 3d
Lengkapi implementasi fungsi yang terdapat pada `std_lib.h` pada `std_lib.c`.

Fungsi-fungsi di atas dapat digunakan untuk mempermudah implementasi fungsi `printString`, `readString`, dan `clearScreen`

**Jawab**

```C
#include "std_lib.h"

int div(int a, int b) {
    // div untuk OS yang tidak memiliki fungsi pembagian
    int quotient = 0;
    while (a >= b) {
        a -= b;
        quotient++;
    }
    return quotient;
}

int mod(int a, int b) {
    // mod untuk OS yang tidak memiliki fungsi modulus
    while (a >= b) {
        a -= b;
    }
    return a;
}

void memcpy(byte* src, byte* dst, unsigned int size) {
    unsigned int i;
    for (i = 0; i < size; i++) {
        dst[i] = src[i];
    }
}

unsigned int strlen(char* str) {
    unsigned int length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

bool strcmp(char* str1, char* str2) {
    unsigned int i = 0;
    while (str1[i] != '\0' || str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return false;
        }
        i++;
    }
    return true;
}

void strcpy(char* src, char* dst) {
    unsigned int i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

void clear(byte* buf, unsigned int size) {
    unsigned int i;
    for (i = 0; i < size; i++) {
        buf[i] = 0;
    }
}
```
- `int div(int a, int b)` digunakan untuk melakukan pembagian dua bilangan bulat
- `int mod(int a, int b)` digunakan untuk mendapatkan sisa dari pembagian dua bilangan bulat
- `void memcpy(byte* src, byte* dst, unsigned int size)` digunakan untuk menyalin data dari area memori yang ditunjuk oleh `src` ke area memori yang ditunjuk oleh `dst`. Parameter `size` menunjukkan jumlah byte yang akan disalin
- `unsigned int strlen(char* str)` digunakan untuk menghitung panjang dari string `str` dengan mengiterasi melalui karakter-karakternya hingga menemui karakter null terminator (`\0`) dan mengembalikannya (abaikan null terminator)
- `bool strcmp(char* str1, char* str2)` digunakan untuk membandingkan dua string `str1` dan `str2` karakter per karakter hingga menemui karakter null terminator pada salah satu dari kedua string tersebut. Jika kedua string identik, fungsi ini mengembalikan `true`, jika tidak, fungsi ini mengembalikan `false`
- `void strcpy(char* src, char* dst)` digunakan untuk menyalin string dari `src` ke `dst` termasuk null terminator
- `void clear(byte* buf, unsigned int size)` digunakan untuk mengisi area memori yang ditunjuk oleh `buf` dengan nilai 0. Parameter `size` menunjukkan jumlah byte yang akan diisi dengan nilai 0

### Problem 3e
Buat automasi untuk melakukan kompilasi dengan melengkapi file `makefile`.

Untuk melakukan kompilasi program, akan digunakan perintah `make build`. Semua hasil kompilasi program akan disimpan pada direktori `bin/`. Untuk menjalankan program, akan digunakan perintah `make run`.

**Jawab**

*1. Persiapan `makefile`*
```makefile
prepare:
	mkdir -p bin
	dd if=/dev/zero of=bin/floppy.img bs=512 count=2880

bootloader: 
	nasm -f bin src/bootloader.asm -o bin/bootloader.bin
	dd if=bin/bootloader.bin of=bin/floppy.img bs=512 count=1 conv=notrunc

stdlib:
	bcc -ansi -c -I./include src/std_lib.c -o bin/std_lib.o

kernel: 
	nasm -f as86 src/kernel.asm -o bin/kernel_asm.o
	bcc -ansi -c -I./include src/kernel.c -o bin/kernel.o

link:
	ld86 -o bin/kernel -d bin/kernel.o bin/kernel_asm.o bin/std_lib.o
	dd if=bin/kernel of=bin/floppy.img bs=512 seek=1 conv=notrunc

build: prepare bootloader stdlib kernel link

run:
	bochs -f bochsrc.txt 

clean:
	rm -f bin/*.o bin/*.bin bin/*.img

.PHONY: prepare bootloader stdlib kernel link build run clean
```
- `prepare` digunakan untuk membuat direktori `bin` jika belum ada, dan membuat file image disk kosong (`floppy.img`) dengan ukuran 2880 blok, masing-masing berukuran 512 byte
- `boatloader` digunakan untuk mengkompilasi bootloader dari file assembler `bootloader.asm` menjadi `bootloader.bin`. Kemudian, menggunakan `dd`, `bootloader.bin` disalin ke `floppy.img` pada blok pertama (blok boot) tanpa menghapus isi file image sebelumnya
- `stdlib` digunakan untuk mengkompilasi file C `std_lib.c` menjadi objek `std_lib.o` menggunakan compiler `bcc`. Argumen `-ansi` digunakan untuk mengompilasi sesuai dengan standar ANSI, dan `-I./include` digunakan untuk menyertakan direktori include yang diperlukan
- `kernel` digunakan untuk engkompilasi kernel dari file assembler `kernel.asm` dan file C `kernel.c` menjadi objek `kernel_asm.o` dan `kernel.o` menggunakan assembler `as86` dan compiler `bcc`
- `link` digunakan untuk menggabungkan objek `kernel_asm.o`, `kernel.o`, dan `std_lib.o` menjadi sebuah binary kernel bernama `kernel` menggunakan linker `ld86`. Binary kernel kemudian disalin ke `floppy.img` pada blok kedua menggunakan `dd`
- `build` merupakan target utama yang memanggil secara berurutan target-target `prepare`, `bootloader`, `stdlib`, `kernel`, dan `link` untuk membangun seluruh proyek kernel
- `run` digunakan untuk menjalankan emulator Bochs dengan menggunakan konfigurasi yang disimpan dalam file `bochsrc.txt`
- `clean` digunakan untuk menghapus semua file objek, file binary, dan file image yang dibuat selama proses kompilas
- `.PHONY` digunakan untuk membuat semua target yang terdaftar di atas dianggap sebagai target phony, yang berarti target tersebut tidak mewakili file yang sebenarnya. Ini penting agar `make` tidak memeriksa apakah ada file dengan nama yang sama dengan target sebelum menjalankan perintah

*2. Dokumentasi*
- Make build

![alt text](/resource/3e-1.png)

- Make run

![alt text](/resource/3e-2.png)

- Tampilan GUI, clearScreen, readString, dan printString

![alt text](/resource/3e-3.png)

- Before Delete

![alt text](/resource/3e-4.png)

- After Delete

![alt text](/resource/3e-5.png)

### Kendala

Sejauh ini tidak menemukan kendala untuk nomor 3

<div align=center>

# ------- SELESAI -------

</div>
