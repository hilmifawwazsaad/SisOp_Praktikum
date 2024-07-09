#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dirent.h>
#include <stdlib.h>

#define TARGET_DIR "/usr/operating-system/praktikum-modul-4-d04/task-2/target"
static const char base64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Function to get the username based on UID
char *get_user_name(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        return pw->pw_name;
    } else {
        return "unknown"; // Handle cases where getpwuid fails
    }
}

// Function to check if the user has access to the target directory
int has_access(const char *username, const char *path) {
    char target_dir[PATH_MAX];
    snprintf(target_dir, PATH_MAX, "%s/%s", TARGET_DIR, username);
    return strstr(path, target_dir) == path;
}

// Base64 encoding function
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

// Base64 decoding function
unsigned char *base64_decode(const char *data, size_t input_length, size_t *output_length) {
    if (input_length % 4 != 0) return NULL;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : base64_table[strchr(base64_table, data[i++]) - base64_table];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : base64_table[strchr(base64_table, data[i++]) - base64_table];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : base64_table[strchr(base64_table, data[i++]) - base64_table];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : base64_table[strchr(base64_table, data[i++]) - base64_table];

        uint32_t triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return decoded_data;
}

// Function to construct the full path by appending the target directory
void construct_full_path(const char *path, char *full_path) {
    snprintf(full_path, PATH_MAX, "%s%s", TARGET_DIR, path);
}

// FUSE callback to read file attributes
static int fuse_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    (void)fi;
    int res = 0;
    memset(stbuf, 0, sizeof(struct stat));

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    res = lstat(full_path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

// FUSE callback to read directory contents
static int fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    (void) offset;
    (void) fi;
    (void) flags;

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    DIR *dp = opendir(full_path);
    if (dp == NULL)
        return -errno;

    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        if (de->d_name[0] == '.')
            continue;

        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        filler(buf, de->d_name, &st, 0, 0);
    }

    closedir(dp);
    return 0;
}

// FUSE callback to create a directory
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

// FUSE callback to remove a directory
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

// FUSE callback to create a file
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

// FUSE callback to remove a file
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

// FUSE callback to rename a file or directory
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

// FUSE callback to open a file
static int fuse_open(const char *path, struct fuse_file_info *fi) {
    struct fuse_context *ctx = fuse_get_context();
    char *username = get_user_name(ctx->uid);

    char full_path[PATH_MAX];
    construct_full_path(path, full_path);

    if (!has_access(username, full_path))
        return -EACCES;

    int fd = open(full_path, fi->flags);
    if (fd == -1)
        return -errno;

    fi->fh = fd;
    return 0;
}

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

// FUSE callback to release (close) a file
static int fuse_release(const char *path, struct fuse_file_info *fi) {
    close(fi->fh);
    return 0;
}

// FUSE callback to write data to a file
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

// FUSE callback to truncate (resize) a file
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

static struct fuse_operations fuse_oper = {
    .getattr = fuse_getattr,
    .readdir = fuse_readdir,
    .open = fuse_open,
    .read = fuse_read,
    .release = fuse_release,
    .write = fuse_write,
    .truncate = fuse_truncate,
    .create = fuse_create,
    .mkdir = fuse_mkdir,
    .rmdir = fuse_rmdir,
    .unlink = fuse_unlink,
    .rename = fuse_rename,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &fuse_oper, NULL);
}
