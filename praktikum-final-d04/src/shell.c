#include "shell.h"
#include "kernel.h"
#include "std_lib.h"
#include "filesystem.h"

void shell() {
  char buf[64];
  char cmd[64];
  char arg[2][64];

  byte cwd = FS_NODE_P_ROOT;

  while (true) {
    printString("MengOS:");
    printCWD(cwd);
    printString("$ ");
    readString(buf);
    parseCommand(buf, cmd, arg);

    if (strcmp(cmd, "cd")) cd(&cwd, arg[0]);
    else if (strcmp(cmd, "ls")) ls(cwd, arg[0]);
    else if (strcmp(cmd, "mv")) mv(cwd, arg[0], arg[1]);
    else if (strcmp(cmd, "cp")) cp(cwd, arg[0], arg[1]);
    else if (strcmp(cmd, "cat")) cat(cwd, arg[0]);
    else if (strcmp(cmd, "mkdir")) mkdir(cwd, arg[0]);
    else if (strcmp(cmd, "clear")) clearScreen();
    else printString("Invalid command\n");
  }
}

// TODO: 4. Implement printCWD function
void printCWD(byte cwd) {
  struct node_fs node_fs_buf;
  char path[FS_MAX_NODE][MAX_FILENAME];
  int depth = 0;
  byte temp;
  int i;

  // Membaca sektor yang berisi informasi node dari filesystem ke buffer
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  // Jika cwd adalah root, langsung print "/"
  if (cwd == FS_NODE_P_ROOT)
  {
    printString("/");
    return;
  }

  // Mencari path dari root ke cwd
  while (cwd != FS_NODE_P_ROOT)
  {
    temp = cwd;
    strcpy(path[depth], node_fs_buf.nodes[temp].node_name);
    cwd = node_fs_buf.nodes[temp].parent_index;
    depth++;
  }

  printString("/");

  // Print path dari cwd ke root
  for (i = depth - 1; i >= 0; i--)
  {
    printString(path[i]);
    if (i == 0)
    {
      printString("/");
    }
  }
}

// TODO: 5. Implement parseCommand function
void parseCommand(char *buf, char *cmd, char arg[2][64]) {
  int i = 0, j = 0, k = 0;

  // Inisialisasi cmd, arg[0], dan arg[1] dengan nol
  for (i = 0; i < 64; i++)
  {
    cmd[i] = arg[0][i] = arg[1][i] = 0;
  }

  i = 0; // Reset i untuk penggunaan selanjutnya

  // Salin perintah ke cmd sampai menemukan spasi atau akhir string
  while (buf[i] != ' ' && buf[i] != '\0')
  {
    cmd[j++] = buf[i++];
  }

  // Jika ada argumen pertama, salin ke arg[0]
  if (buf[i] == ' ')
  {
    i++; // Lewati spasi
    j = 0;
    while (buf[i] != ' ' && buf[i] != '\0')
    {
      arg[0][j++] = buf[i++];
    }
  }

  // Jika ada argumen kedua, salin ke arg[1]
  if (buf[i] == ' ')
  {
    i++; // Lewati spasi
    j = 0;
    while (buf[i] != ' ' && buf[i] != '\0')
    {
      arg[1][j++] = buf[i++];
    }
  }
}

// TODO: 6. Implement cd function
void cd(byte *cwd, char *dirname)
{
  struct node_fs node_fs_buf;
  int i;

  // Membaca sektor yang berisi informasi node dari filesystem ke buffer
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  // Jika argumen dirname "..", kembali ke parent directory
  if (strcmp(dirname, "..") == 1)
  {
    if (*cwd != FS_NODE_P_ROOT)
    {
      *cwd = node_fs_buf.nodes[*cwd].parent_index;
    }
    return;
  }

  // Jika argumen dirname "/", kembali ke root directory
  if (strcmp(dirname, "/") == 1)
  {
    *cwd = FS_NODE_P_ROOT;
    return;
  }

  // Cari direktori yang sesuai dengan dirname
  for (i = 0; i < FS_MAX_NODE; i++)
  {
    if (node_fs_buf.nodes[i].parent_index == *cwd && strcmp(node_fs_buf.nodes[i].node_name, dirname))
    {
      if (node_fs_buf.nodes[i].data_index == FS_NODE_D_DIR)
      {
        *cwd = i;
        return;
      }
    }
  }

  // Jika tidak ditemukan, print pesan error
  printString("Directory not found\n");
}

// TODO: 7. Implement ls function
void ls(byte cwd, char *dirname) {
  struct node_fs node_fs_buf;
  int i;
  byte target_dir = cwd;

  // Membaca sektor yang berisi informasi node
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  // Jika argumen dirname diberikan dan tidak kosong
  if (dirname[0] != '\0')
  {
    bool found = false;

    // Jika argumen adalah ".", tetap di direktori saat ini
    if (strcmp(dirname, ".") == 1)
    {
      target_dir = cwd;
      found = true;
    }
    else
    {
      // Cari direktori yang sesuai dengan dirname
      for (i = 0; i < FS_MAX_NODE; i++)
      {
        if (node_fs_buf.nodes[i].parent_index == cwd && strcmp(node_fs_buf.nodes[i].node_name, dirname) == 1)
        {
          if (node_fs_buf.nodes[i].data_index == FS_NODE_D_DIR)
          {
            target_dir = i;
            found = true;
            break;
          }
          else
          {
            printString("ls: not a directory\n");
            return;
          }
        }
      }
    }

    if (!found)
    {
      printString("ls: no such directory\n");
      return;
    }
  }

  // Tampilkan isi direktori target
  for (i = 0; i < FS_MAX_NODE; i++)
  {
    if (node_fs_buf.nodes[i].parent_index == target_dir && strcmp(node_fs_buf.nodes[i].node_name, dirname) == 0)
    {
      printString(node_fs_buf.nodes[i].node_name);
      printString("\n");
    }
  }
}

// TODO: 8. Implement mv function
void mv(byte cwd, char *src, char *dst) {
  struct node_fs node_fs_buf;
  int i, src_index = -1, dst_parent_index = cwd;
  char *outputname;

  // Membaca sektor yang berisi informasi node dari filesystem ke buffer
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  // Mencari indeks file yang akan dipindahkan
  for (i = 0; i < FS_MAX_NODE; i++)
  {
    if (node_fs_buf.nodes[i].parent_index == cwd && strcmp(node_fs_buf.nodes[i].node_name, src) == 1)
    {
      if (node_fs_buf.nodes[i].data_index != FS_NODE_D_DIR)
      {
        src_index = i;
        break;
      }
      else
      {
        printString("mv: cannot move a directory\n");
        return;
      }
    }
  }

  if (src_index == -1)
  {
    printString("mv: file not found\n");
    return;
  }

  // Memproses tujuan
  if (dst[0] == '/')
  {
    dst_parent_index = FS_NODE_P_ROOT;
    outputname = dst + 1;
  }
  else if (dst[0] == '.' && dst[1] == '.')
  {
    dst_parent_index = node_fs_buf.nodes[cwd].parent_index;
    outputname = dst + 3;
  }
  else
  {
    outputname = findChar(dst, '/');
    if (outputname == 0)
    {
      outputname = dst;
    }
    else
    {
      *outputname = '\0';
      outputname++;
      for (i = 0; i < FS_MAX_NODE; i++)
      {
        if (node_fs_buf.nodes[i].parent_index == cwd && strcmp(node_fs_buf.nodes[i].node_name, dst) == 1)
        {
          if (node_fs_buf.nodes[i].data_index == FS_NODE_D_DIR)
          {
            dst_parent_index = i;
          }
          else
          {
            printString("mv: not a directory\n");
            return;
          }
          break;
        }
      }
    }
  }

  // Mengubah parent_index dan nama file
  node_fs_buf.nodes[src_index].parent_index = dst_parent_index;
  strcpy(node_fs_buf.nodes[src_index].node_name, outputname);

  // Menyimpan perubahan ke filesystem
  writeSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  writeSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
}

// TODO: 9. Implement cp function
void cp(byte cwd, char *src, char *dst) {
  struct node_fs node_fs_buf;
  int i, src_index = -1, dst_parent_index = cwd;
  char *outputname;
  int free_node_index = -1;

  // Membaca sektor yang berisi informasi node dari filesystem ke buffer
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  // Mencari indeks file yang akan disalin
  for (i = 0; i < FS_MAX_NODE; i++)
  {
    if (node_fs_buf.nodes[i].parent_index == cwd && strcmp(node_fs_buf.nodes[i].node_name, src) == 1)
    {
      if (node_fs_buf.nodes[i].data_index != FS_NODE_D_DIR)
      {
        src_index = i;
        break;
      }
      else
      {
        printString("cp: cannot copy a directory\n");
        return;
      }
    }
  }

  if (src_index == -1)
  {
    printString("cp: file not found\n");
    return;
  }

  // Mencari node bebas untuk file baru
  for (i = 0; i < FS_MAX_NODE; i++)
  {
    if (node_fs_buf.nodes[i].parent_index == 0x00 && node_fs_buf.nodes[i].node_name[0] == '\0')
    {
      free_node_index = i;
      break;
    }
  }

  if (free_node_index == -1)
  {
    printString("cp: no free node available\n");
    return;
  }

  // Memproses tujuan
  if (dst[0] == '/')
  {
    dst_parent_index = FS_NODE_P_ROOT;
    outputname = dst + 1;
  }
  else if (dst[0] == '.' && dst[1] == '.')
  {
    dst_parent_index = node_fs_buf.nodes[cwd].parent_index;
    outputname = dst + 3;
  }
  else
  {
    outputname = findChar(dst, '/');
    if (outputname == 0)
    {
      outputname = dst;
    }
    else
    {
      *outputname = '\0';
      outputname++;
      for (i = 0; i < FS_MAX_NODE; i++)
      {
        if (node_fs_buf.nodes[i].parent_index == cwd && strcmp(node_fs_buf.nodes[i].node_name, dst) == 1)
        {
          if (node_fs_buf.nodes[i].data_index == FS_NODE_D_DIR)
          {
            dst_parent_index = i;
          }
          else
          {
            printString("cp: not a directory\n");
            return;
          }
          break;
        }
      }
    }
  }

  // Menyalin metadata file dari src_index ke free_node_index
  node_fs_buf.nodes[free_node_index].parent_index = dst_parent_index;
  strcpy(node_fs_buf.nodes[free_node_index].node_name, outputname);
  node_fs_buf.nodes[free_node_index].data_index = node_fs_buf.nodes[src_index].data_index;

  // Menyimpan perubahan ke filesystem
  writeSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  writeSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
}

// TODO: 10. Implement cat function
void cat(byte cwd, char *filename) {
  struct node_fs node_fs_buf;
  struct file_metadata file_meta;
  enum fs_return status;
  int i, file_index = -1;

  // Membaca sektor yang berisi informasi node dari filesystem ke buffer
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  // Mencari indeks file yang akan dibaca
  for (i = 0; i < FS_MAX_NODE; i++)
  {
    if (node_fs_buf.nodes[i].parent_index == cwd && strcmp(node_fs_buf.nodes[i].node_name, filename) == 1)
    {
      if (node_fs_buf.nodes[i].data_index != FS_NODE_D_DIR)
      {
        file_index = i;
        break;
      }
      else
      {
        printString("cat: cannot read a directory\n");
        return;
      }
    }
  }

  if (file_index == -1)
  {
    printString("cat: file not found\n");
    return;
  }

  // Membaca metadata file
  file_meta.parent_index = cwd;
  strcpy(file_meta.node_name, filename);
  fsRead(&file_meta, &status);

  if (status != FS_SUCCESS)
  {
    printString("cat: failed to read file\n");
    return;
  }

  // Menampilkan isi file
  printString(file_meta.buffer);
  printString("\n");
}

// TODO: 11. Implement mkdir function
void mkdir(byte cwd, char *dirname) {
  struct node_fs node_fs_buf;
  int i, new_index = -1;

  // Membaca sektor yang berisi informasi node dari filesystem ke buffer
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  // Mencari indeks kosong untuk direktori baru, pastikan tidak menggunakan indeks 0
  for (i = 1; i < FS_MAX_NODE; i++)
  { // Mulai dari 1, bukan 0
    // Pastikan hanya menggunakan node yang benar-benar kosong (0x00)
    if (node_fs_buf.nodes[i].parent_index == 0x00 && node_fs_buf.nodes[i].node_name[0] == '\0')
    {
      new_index = i;
      break;
    }
    else if (node_fs_buf.nodes[i].parent_index == cwd && strcmp(node_fs_buf.nodes[i].node_name, dirname) == 1)
    {
      printString("mkdir: file already exists\n");
      return;
    }
  }

  if (new_index == -1)
  {
    printString("mkdir: no space left to create directory\n");
    return;
  }

  // Membuat direktori baru
  node_fs_buf.nodes[new_index].parent_index = cwd;
  strcpy(node_fs_buf.nodes[new_index].node_name, dirname);
  node_fs_buf.nodes[new_index].data_index = FS_NODE_D_DIR;

  // Menyimpan perubahan ke filesystem
  writeSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  writeSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
}