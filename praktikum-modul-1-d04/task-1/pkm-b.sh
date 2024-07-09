#!/bin/bash

# set variabel untuk memuat resource data pkm
pkm_tsv_file="resources/DataPKM.tsv"

# baca file tsv, karena pakai tab separated value, pakai '\t'
awk -F'\t' '

{
    # hitung berapa banyak skema yang terhitung sama pada kolom ke 7
    if ($7 in schema) {
        schema[$7]++
    } else {
        schema[$7] = 1
    }
}

END {
    # cari skema yang paling diminiati
    pkm_paling_diminati = 0
    for (i in schema) {
        if (schema[i] > pkm_paling_diminati) {
            pkm_paling_diminati = schema[i]
            skema_pkm = i
        }
    }
    printf "Skema dengan peminat terbanyak adalah skema %s\n", skema_pkm
}
' "$pkm_tsv_file"