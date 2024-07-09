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

// menyimpan pemilihan pemain
struct PemilihanPemain
{
    int pemain; // nomor pemain yang bergabung
    int jumlah; // jumlah pemain yang sudah bergabung
};

// representasi gerakan pemain
struct GerakanPemain
{
    long tipePesan; // tipe pesan yang digunakan dalam message queue)
    int nomorPemain; // nomor pemain yang melakukan gerakan (1 atau 2)
    int selPilihan; 
};

// Struktur untuk menyampaikan validitas gerakan dan status permainan
struct GerakanValid
{
    long tipePesan; // Tipe pesan (digunakan dalam message queue)
    int kodeStatus; // Kode status validitas gerakan (0: tidak valid, 1: valid, 2: menang, 3: sel tidak valid, 4: seri)
    char simbolMenang; // Simbol pemain yang menang (jika ada)
};

// Fungsi untuk menginisialisasi papan permainan
void siapkanPapanPermainan()
{
    // Membuat segmen memori bersama untuk papan permainan
    int idMemoriBerbagi = shmget((key_t)1414, sizeof(char[MAX_CHESSBOARD][MAX_CHESSBOARD]), 0666 | IPC_CREAT);
    // Menautkan segmen memori bersama ke variabel papanPermainan
    papanPermainan = shmat(idMemoriBerbagi, NULL, 0);

    // Inisialisasi semua sel papan permainan dengan kosong
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            papanPermainan[i][j] = ' ';
        }
    }
}

// Fungsi untuk memperbarui papan permainan dengan gerakan pemain
int terapkanGerakan(int selPilihan, char simbolPemain)
{
    // Memeriksa apakah nomor sel valid (antara 1 dan 9)
    if (selPilihan < 1 || selPilihan > 9)
    {
        printf("-- Tidak valid: Nomor sel harus antara 1 dan 9! --\n");
        return -1;
    }

    // Menghitung baris dan kolom berdasarkan nomor sel
    int baris = (selPilihan - 1) / MAX_CHESSBOARD;
    int kolom = (selPilihan - 1) % MAX_CHESSBOARD;
    int isValid = 1;

    // Periksa apakah sel yang dipilih sudah terisi
    if (papanPermainan[baris][kolom] != ' ')
    {
        printf("-- Tidak valid: Sel sudah terisi! --\n");
        isValid = 0;
    }
    else
    {
        papanPermainan[baris][kolom] = simbolPemain; // Menambahkan simbol pemain ke sel yang dipilih
    }

    return isValid;
}

// Fungsi untuk memeriksa apakah pemain telah menang
int periksaMenang(char simbol)
{
    // Memeriksa baris, kolom, dan diagonal untuk mencari tanda-tanda kemenangan
    if (papanPermainan[0][0] == simbol && papanPermainan[0][1] == simbol && papanPermainan[0][2] == simbol ||
        papanPermainan[1][0] == simbol && papanPermainan[1][1] == simbol && papanPermainan[1][2] == simbol ||
        papanPermainan[2][0] == simbol && papanPermainan[2][1] == simbol && papanPermainan[2][2] == simbol ||
        papanPermainan[0][0] == simbol && papanPermainan[1][0] == simbol && papanPermainan[2][0] == simbol ||
        papanPermainan[0][1] == simbol && papanPermainan[1][1] == simbol && papanPermainan[2][1] == simbol ||
        papanPermainan[0][2] == simbol && papanPermainan[1][2] == simbol && papanPermainan[2][2] == simbol ||
        papanPermainan[0][0] == simbol && papanPermainan[1][1] == simbol && papanPermainan[2][2] == simbol ||
        papanPermainan[0][2] == simbol && papanPermainan[1][1] == simbol && papanPermainan[2][0] == simbol)
    {
        return 1; // Kondisi menang terpenuhi
    }
    return 0; // Belum ada pemenang
}

// Fungsi untuk memeriksa apakah permainan berakhir imbang
int periksaImbang()
{
    int selTerisi = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (papanPermainan[i][j] != ' ')
            {
                selTerisi++; // Menghitung jumlah sel yang terisi
            }
        }
    }
    if (selTerisi == 9)
    {
        return 1; // Semua sel sudah terisi, menunjukkan imbang
    }
    return 0; // Permainan belum imbang
}

int main()
{
    int i, idAntrianPesan;
    struct PemilihanPemain *pemilihanPemain = (struct PemilihanPemain *)malloc(sizeof(struct PemilihanPemain));
    int idMemoriBerbagi;

    idMemoriBerbagi = shmget((key_t)2121, 1024, 0666 | IPC_CREAT);
    pemilihanPemain = shmat(idMemoriBerbagi, NULL, 0);
    pemilihanPemain->pemain = 0;
    pemilihanPemain->jumlah = 0;

    printf("Menunggu pemain untuk bergabung dalam permainan...\n");

    int giliranSaatIni = 0;
    while (1)
    {
        if (pemilihanPemain->jumlah == 2)
        {
            siapkanPapanPermainan();
            idAntrianPesan = msgget((key_t)3030, IPC_CREAT | 0644);

            printf("---- Permainan Dimulai! ----\n");
            while (1)
            {
                int giliranPemainSekarang = giliranSaatIni % 2 + 1;
                int selPilihan;

                struct GerakanPemain gerakanPemain;
                struct GerakanValid validitas, validitas2;

                // Menerima gerakan pemain dari antrian pesan
                msgrcv(idAntrianPesan, &gerakanPemain, sizeof(gerakanPemain), 1, 0);

                int pemainSekarang = gerakanPemain.nomorPemain;
                selPilihan = gerakanPemain.selPilihan;
                char simbol;
                printf("Pesan Baru\n");
                if (pemainSekarang == 1)
                {
                    printf("Pengirim: Pemain X\nPesan: %d\n", selPilihan);
                    simbol = 'X';
                }
                else
                {
                    printf("Pengirim: Pemain O\nPesan: %d\n", selPilihan);
                    simbol = 'O';
                }

                // Memvalidasi dan menerapkan gerakan pemain ke papan permainan
                if (terapkanGerakan(selPilihan, simbol) == 1)
                {
                    if (periksaMenang(simbol))
                    {
                        // Jika pemain menang, kirim pesan ke antrian pesan
                        validitas.kodeStatus = 2; // menang
                        validitas2.kodeStatus = 2;
                        validitas.tipePesan = 2;
                        validitas2.tipePesan = 3;
                        validitas.simbolMenang = simbol;
                        validitas2.simbolMenang = simbol;
                        msgsnd(idAntrianPesan, &validitas, sizeof(validitas), 0);
                        msgsnd(idAntrianPesan, &validitas2, sizeof(validitas2), 0);
                        printf("%c Pemain menang!!\n", simbol);
                        printf("---- Permainan Selesai! ----\n");
                        break;
                    }
                    else if (periksaImbang() == 1)
                    {
                        // Jika permainan berakhir imbang, kirim pesan ke antrian pesan
                        validitas.kodeStatus = 4;
                        validitas2.kodeStatus = 4;
                        validitas.tipePesan = 2;
                        msgsnd(idAntrianPesan, &validitas, sizeof(validitas), 0);
                        validitas2.tipePesan = 3;
                        msgsnd(idAntrianPesan, &validitas2, sizeof(validitas2), 0);
                        printf("Permainan berakhir seri\n");
                        printf("---- Permainan Selesai! ----\n");
                        break;
                    }
                    else
                    {
                        // Jika gerakan valid, lanjutkan dengan giliran pemain berikutnya
                        validitas.kodeStatus = 1;
                        validitas2.kodeStatus = 1;
                        validitas.tipePesan = 2;
                        msgsnd(idAntrianPesan, &validitas, sizeof(validitas), 0);
                        validitas2.tipePesan = 3;
                        msgsnd(idAntrianPesan, &validitas2, sizeof(validitas2), 0);
                        giliranSaatIni++; // Pindah ke giliran pemain berikutnya
                    }
                }
                else if (terapkanGerakan(selPilihan, simbol) == -1)
                { // <1 || >9
                    // Jika nomor sel tidak valid, kirim pesan ke antrian pesan
                    validitas.tipePesan = 2;
                    validitas.kodeStatus = 3;
                    msgsnd(idAntrianPesan, &validitas, sizeof(validitas), 0);
                    validitas2.tipePesan = 3;
                    validitas2.kodeStatus = 3;
                    msgsnd(idAntrianPesan, &validitas2, sizeof(validitas2), 0);
                }
                else
                {
                    // Jika gerakan tidak valid, kirim pesan ke antrian pesan
                    validitas.tipePesan = 2;
                    validitas.kodeStatus = 0;
                    msgsnd(idAntrianPesan, &validitas, sizeof(validitas), 0);
                    validitas2.tipePesan = 3;
                    validitas2.kodeStatus = 0;
                    msgsnd(idAntrianPesan, &validitas2, sizeof(validitas2), 0);
                }
            }
            break;
        }
    }
}
