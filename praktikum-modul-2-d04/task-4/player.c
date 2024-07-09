#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#define MAX_CHESSBOARD 3

// variabel untuk papan permainan 3x3
char (*papanPermainan)[MAX_CHESSBOARD];

// struct untuk menyimpan identitas pemain
struct IdentitasPemain
{
    int jenis; // 1: X, 2: O
    int jumlah; // jumlah pemain yang telah bergabung
};

// struct untuk merepresentasikan gerakan pemain
struct Gerakan
{
    long tipePesan; // tipe pesan yang digunakan dalam message queue)
    int pemain; // nomor pemain yang melakukan gerakan (1 atau 2)
    int sel; 
};

// struct untuk menyampaikan validitas gerakan dan status permainan
struct GerakanValid
{
    long tipePesan; // tipe pesan yang (digunakan dalam message queue)
    int status; // kode status validitas gerakan (0: tidak valid, 1: valid, 2: menang, 3: nomor sel tidak valid, 4: seri)
    char simbolPemenang;
};

// memeriksa apakah pemain telah memilih jenis pemain yang valid
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

// menginisialisasi papan permainan dan menampilkan sel-selnya
void inisialisasiPapan()
{
    // jembuat segmen memori bersama untuk papan permainan
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

// menampilkan papan permainan saat ini
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

    while (1)
    {
        int giliran = 0, sel;
        if (pilihanPemain->jumlah == 2)
        {
            printf("\n---- Permainan Dimulai!! ----\n\n");
            printf("Anda bermain sebagai %c\n", simbol);
            inisialisasiPapan();

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
