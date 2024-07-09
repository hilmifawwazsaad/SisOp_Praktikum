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

static int xmp_mkdir(const char *path, mode_t mode) {
    if (strstr(path, "/trash") != NULL || strstr(path, "/restore") != NULL) {
        write_log("FAILED TO CREATE TRASH/RESTORE DIRECTORY");
        return -EPERM;
    }
    return mkdir(path, mode);
}

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
