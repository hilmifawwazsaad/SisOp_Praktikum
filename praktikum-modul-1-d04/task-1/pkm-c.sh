#!/bin/bash

# set variabel untuk memuat resource data pkm
pkm_tsv_file="resources/DataPKM.tsv"

# baca file tsv, karena pakai tab separated value, pakai '\t'
awk -F'\t' '

# buat array kosong untuk nama mhs dan dosbing
BEGIN {
    mahasiswa = ""
    dosbing = ""
}

# Proses setiap baris dari file TSV
{
    # Hilangkan garis bawah dari nama_mhs
    nama_mhs = $2
    nama_dosbing = $6
    gsub(/_/, " ", nama_mhs)

    # Periksa apakah nama_mhs dan dosbing ada dalam array
    if (mahasiswa == "") {
        mahasiswa = nama_mhs
        dosbing = nama_dosbing
       }

    # hitung nama mhs yg sama dan ambil nilai dari dosen pembimbing
    count[nama_mhs]++
    dosen_pembimbing[nama_mhs] = dosen_pembimbing[nama_mhs] nama_dosbing
}

END {
    # Loop melalui array dan print nama mahasiswa dengan lebih dari satu proposal
    for (nama_mhs in count) {
        if (count[nama_mhs] > 1) {
                printf "nama yang lebih dari 1 adalah %s dengan dosbing %s, silahkan dihubungi komandan\n", nama_mhs, dosen_pembimbing[nama_mhs]
        }
    }
}
' "$pkm_tsv_file"
