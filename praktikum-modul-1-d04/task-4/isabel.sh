#!/bin/bash

#absolute path dari direktori penyimpanan folder dan zip
PATH_ABS="/home/hilmifawwaz/sisop/praktikum-modul-1-d04/task-4"

#inisialisasi variabel foldercount
foldercount=0
#membuat nama folder yang unik
folder_name="folder_$((++foldercount))"
while [ -d "$PATH_ABS/$folder_name" ]; do
    folder_name="folder_$((++folder_count))"
done

#fungsi untuk mendownload foto (4a)
download_photos() {
    local count=$1
    local folder=$2

    mkdir -p "$PATH_ABS/$folder"
    for ((i=1; i<=$count; i++)); do
        wget --show-progress --progress=bar -q -O "$PATH_ABS/$folder/foto_$i.jpg" "https://phinemo.com/wp-content/uploads/2017/02/hidup-bebas.jpg"
        echo "Download foto_$i.jpg ke $folder"
    done
}

#fungsi untuk mengecek apakah waktu adalah genap (4a)
is_even() {
    local time=$1
    ((time % 2 == 0))
}

#fungsi untuk mengecek apakah waktu adalah tengah malam-00.00 (4a)
is_midnight() {
    local hour=$1
    local minute=$2
    [[ $hour -eq 0 && $minute -eq 0 ]]
}

#inisialisasi variabel last_execution_time
last_execution_time=0
#fungsi untuk mengupdate waktu eksekusi terakhir 
update_last_execution_time() {
    last_execution_time=$(date +%s)
    echo "$last_execution_time" > ~/.last_execution_time
    echo "Update waktu eksekusi terakhir: $(date)"
}

#fungsi untuk zip folder (4b)
folder_zip() {
    # Loop melalui setiap folder di dalam PATH_ABS yang belum di-zip
    for folder in "$PATH_ABS"/folder_*; do
        # Pastikan itu adalah sebuah direktori dan bukan file zip
        if [ -d "$folder" ] && [ ! -e "$folder.zip" ]; then
            # Buat nama file zip baru berdasarkan nomor folder
            zip_name="$PATH_ABS/ayang_${folder##*_}.zip"

            # Zip folder tanpa memeriksa apakah kosong atau tidak
            zip -r "$zip_name" "$folder"
        fi
    done
}

#fungsi untuk menghapus folder dan zip (4c)
delete_folders_and_zips() {
    local directory="$1"

    #inisialisasi variabel untuk mengecek apakah ada folder di direktori
    local folder_ditemukan=false
    for folder in "$directory"/folder_*; do
        if [ -d "$folder" ]; then
            folder_ditemukan=true
            rm -rf "$folder" #hapus folder
            echo "Folder $folder telah dihapus"
        fi
    done

    #cek jika tidak ada folder di direktori
    if ! $folder_ditemukan; then
        echo "Tidak ada folder di direktori $directory"
    fi

    #inisialisasi variabel untuk mengecek apakah ada zip di direktori
    local zip_ditemukan=false
    for zip_file in "$directory"/ayang_*.zip; do
        if [ -f "$zip_file" ]; then
            zip_ditemukan=true
            rm -f "$zip_file" #hapus file zip
            echo "File zip $zip_file telah dihapus"
        fi
    done

    #cek jika tidak ada zip di direktori
    if ! $zip_ditemukan; then
        echo "Tidak ada zip di direktori $directory"
    fi
}

#fungsi untuk mendownload foto Levi atau Eren(4d)
download_foto() {
    local character=$1
    local tanggal=$(date +"%Y%m%d")

    if [[ "$character" == "levi" ]]; then
        wget -O "$PATH_ABS/levi_$tanggal.jpg" "https://i.pinimg.com/564x/0d/a7/a1/0da7a17b89ea6ee5b62a7eee2f7af31b.jpg"
        echo "Download foto Levi berhasil"
    elif [[ "$character" == "eren" ]]; then
        wget -O "$PATH_ABS/eren_$tanggal.jpg" "https://i.pinimg.com/564x/66/96/dc/6696dc04a235984e905a88c2607c7ffe.jpg"
        echo "Download foto Eren berhasil"
    else
        echo "Karakter tidak valid, silahkan tentukan 'levi' atau 'eren'."
    fi
}

# Main function
main() {
    local current_hour=$(date +"%H")
    local current_minute=$(date +"%M")

    #panggil fungsi sesuai argumen
    if [ "$1" = "4a" ]; then
        echo "Jam sekarang: $current_hour, Menit sekarang: $current_minute"
        
        #cek apa sudah ada file last_execution_time
        if [[ ! -f ~/.last_execution_time ]]; then
            echo $(date +%s) > ~/.last_execution_time
        fi

        last_execution_time=$(cat ~/.last_execution_time)
        time_diff=$(( ( $(date +%s) - last_execution_time) / 3600 )) #menghitung perbedaan waktu dalam jam

        #jika jam 00.00, download 10 foto
        if is_midnight "$current_hour" "$current_minute"; then
            download_photos 10 "$folder_name"
            echo "Download 10 foto pada tengah malam (00.00)."
        else
            #jika perbedaan waktu adalah 5 jam, download 5 foto jika jamnya genap, 3 foto jika jamnya ganjil
            if (( time_diff == 5 )); then
                if is_even "$current_hour"; then
                    download_photos 5 "$folder_name"
                    echo "Download 5 foto pada jam genap."
                else
                    download_photos 3 "$folder_name"
                    echo "Download 3 foto pada jam ganjil."
                fi
                #menginisialisasi time_diff menjadi 0 setelah pengunduhan dilakukan
                time_diff=0
                #update waktu eksekusi terakhir setelah pengunduhan dilakukan
                update_last_execution_time
            else
                echo "Tidak ada foto yang di download. Terakhir dijalankan $time_diff jam yang lalu."
            fi
        fi
    elif [ "$1" = "4b" ]; then
        folder_zip
    elif [ "$1" = "4c" ]; then
        delete_folders_and_zips "$PATH_ABS" #letak directory peyimpanan folder dan zip
    elif [ "$1" = "4d" ]; then
        if [[ ! -f ~/.last_downloaded ]]; then
            touch ~/.last_downloaded
            echo "File .last_downloaded telah dibuat."
        fi

        if [[ -f ~/.last_downloaded ]]; then
            last_downloaded=$(cat ~/.last_downloaded)
        fi

        if [[ "$last_downloaded" == "levi" ]]; then
            download_foto "eren"
            echo "eren" > ~/.last_downloaded
        else
            download_foto "levi"
            echo "levi" > ~/.last_downloaded
        fi
    fi
}

main "$@"
