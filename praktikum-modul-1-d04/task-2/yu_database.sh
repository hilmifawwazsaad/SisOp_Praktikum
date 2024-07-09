#!/bin/bash

# Lokasi file CSV
pkm_csv_file="resources/data-pkm.csv"
# Fungsi untuk mengekstrak nama dari file CSV
extract_names() {
    names=()
    IFS=$'\n'
    array=($(awk -F, '{split($2, a, "_"); print a[1]}' "$pkm_csv_file"))
    for word in "${array[@]}"; do
        names+=("$word")
    done
}

# Fungsi untuk mengekstrak password dari file CSV
extract_password() {
    passwords=()
    IFS=$'\n'
    array=($(awk -F, '{split($4, a, " "); split($6, b, " "); gsub(/[()]/, "", b[length(b)]); printf("%s%s\n", a[1], b[length(b)])}' "$pkm_csv_file"))
    for word in "${array[@]}"; do
        passwords+=("$word")
    done
}

# Fungsi untuk menulis nama dan password ke dalam file teks beserta timestamp
write_to_file() {
    save_path="/home/$(whoami)/txt_test"
    output="$save_path/users.txt"
    mkdir -p "$save_path" || { echo "Gagal membuat direktori $save_path"; exit 1; }
    txt_file="$output"
    echo "$(date '+%Y-%m-%d %H:%M:%S'): Data nama dan password yang diperbarui" > "$txt_file"
    for ((i=0; i<${#names[@]}; i++)); do
        echo "$(date '+%Y-%m-%d %H:%M:%S'): ${names[$i]}, ${passwords[$i]}" >> "$txt_file"
    done
    echo "Data nama dan password telah berhasil diperbarui pada: $(date '+%Y-%m-%d %H:%M:%S')" >> "$txt_file"
}


extract_names
extract_password
export names
export passwords
write_to_file