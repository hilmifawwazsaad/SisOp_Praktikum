#!/bin/bash

# set variabel untuk memuat resource data pkm
pkm_tsv_file="resources/DataPKM.tsv"

# baca file tsv, karena pakai tab separated value, pakai '\t'
awk -F'\t' '
BEGIN {
    # panjang judul maks
    maksimum_judul = 20
    print "No. Nama Pengusul"
}
{
    # ganti underscore menjadi spasi pada kolom kedua
    gsub(/_/, " ", $2)
    
    split($5, judul, " ")

    # Check if the title length is greater than the maximum
    if (length(judul) > maksimum_judul) {
        #print output
        printf "%s\t%s\t%s\n", $1, $2, $3
    }
}
' "$pkm_tsv_file"