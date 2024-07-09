#!/bin/bash

#mendapatkan waktu saat ini
current_hour=$(date +'%Y%m%d%H')

#mendapatkan list file log dari satu jam terakhir
log_files=$(ls /home/$(whoami)/metrics/metrics_${current_hour}*.log)

#nisialisasi nilai minimum dan maksimum
min_mem_total=999999
max_mem_total=0
min_mem_used=999999
max_mem_used=0
min_mem_free=999999
max_mem_free=0
min_mem_shared=999999
max_mem_shared=0
min_mem_buff=999999
max_mem_buff=0
min_mem_available=999999
max_mem_available=0
min_swap_total=999999
max_swap_total=0
min_swap_used=999999
max_swap_used=0
min_swap_free=999999
max_swap_free=0
min_path_size=999999
max_path_size=0
path_size_mb=0
total_records=0

#fungsi untuk mengonversi ukuran path ke MB
function convert_to_mb() {
    local size_str=$1
    local size_unit=${size_str: -1} #mendptkan karakter terakhir dari string
    local size_value=${size_str:0:-1} #mendapatkan nilai dari string tanpa karakter non-numerik
    local size_mb=0

    case $size_unit in
        "G")
            size_mb=$(( size_value * 1024 ))
            ;;
        "M")
            size_mb=$size_value
            ;;
        *)
        echo "Invalid unit: $size_unit"
            ;;
    esac

    echo $size_mb
}

#memproses setiap file log
for file in $log_files; do
    #membaca data dari file log
    ((total_records++))
    while IFS=, read -r mem_total mem_used mem_free mem_shared mem_buff mem_available swap_total swap_used swap_free path path_size; do
        
        #memperbarui nilai minimum dan maksimum
        if [[ $mem_total -lt $min_mem_total ]]; then
            min_mem_total=$mem_total
        fi
        if [[ $mem_total -gt $max_mem_total ]]; then
            max_mem_total=$mem_total
        fi
        if [[ $mem_used -lt $min_mem_used ]]; then
            min_mem_used=$mem_used
        fi
        if [[ $mem_used -gt $max_mem_used ]]; then
            max_mem_used=$mem_used
        fi
        if [[ $mem_free -lt $min_mem_free ]]; then
            min_mem_free=$mem_free
        fi
        if [[ $mem_free -gt $max_mem_free ]]; then
            max_mem_free=$mem_free
        fi
        if [[ $mem_shared -lt $min_mem_shared ]]; then
            min_mem_shared=$mem_shared
        fi
        if [[ $mem_shared -gt $max_mem_shared ]]; then
            max_mem_shared=$mem_shared
        fi
        if [[ $mem_buff -lt $min_mem_buff ]]; then
            min_mem_buff=$mem_buff
        fi
        if [[ $mem_buff -gt $max_mem_buff ]]; then
            max_mem_buff=$mem_buff
        fi
        if [[ $mem_available -lt $min_mem_available ]]; then
            min_mem_available=$mem_available
        fi
        if [[ $mem_available -gt $max_mem_available ]]; then
            max_mem_available=$mem_available
        fi
        if [[ $swap_total -lt $min_swap_total ]]; then
            min_swap_total=$swap_total
        fi
        if [[ $swap_total -gt $max_swap_total ]]; then
            max_swap_total=$swap_total
        fi
        if [[ $swap_used -lt $min_swap_used ]]; then
            min_swap_used=$swap_used
        fi
        if [[ $swap_used -gt $max_swap_used ]]; then
            max_swap_used=$swap_used
        fi
        if [[ $swap_free -lt $min_swap_free ]]; then
            min_swap_free=$swap_free
        fi
        if [[ $swap_free -gt $max_swap_free ]]; then
            max_swap_free=$swap_free
        fi

        #asumsi path_size berisi ukuran path dalam format "xxM" atau "xxG"
        path_size=$(echo "$path_size" | sed 's/[^0-9M]//g')

        #cek apakah path_size valid
        if [[ $path_size =~ ^[0-9]+[GM]$ ]]; then
            path_size_mb=$(convert_to_mb $path_size)

            #update min_path_size jika diperlukan
            if (( path_size_mb < min_path_size )); then
                min_path_size=$path_size_mb
            fi

            #update max_path_size jika diperlukan
            if (( path_size_mb > max_path_size )); then
                max_path_size=$path_size_mb
            fi
        fi
    done < $file
done

#menghitung rata-rata
avg_mem_total=$(( (min_mem_total + max_mem_total) / 2 ))
avg_mem_used=$(( (min_mem_used + max_mem_used) / 2 ))
avg_mem_free=$(( (min_mem_free + max_mem_free) / 2 ))
avg_mem_shared=$(( (min_mem_shared + max_mem_shared) / 2 ))
avg_mem_buff=$(( (min_mem_buff + max_mem_buff) / 2 ))
avg_mem_available=$(( (min_mem_available + max_mem_available) / 2 ))
avg_swap_total=$(( (min_swap_total + max_swap_total) / 2 ))
avg_swap_used=$(( (min_swap_used + max_swap_used) / 2 ))
avg_swap_free=$(( (min_swap_free + max_swap_free) / 2 ))
avg_path_size=$(( (min_path_size + max_path_size) / 2 ))

#menyimpan hasil aggregasi ke dalam file
echo $total_records
echo "type,mem_total,mem_used,mem_free,mem_shared,mem_buff,mem_available,swap_total,swap_used,swap_free,path,path_size" > /home/$(whoami)/metrics/metrics_agg_${current_hour}.log
echo "minimum,$min_mem_total,$min_mem_used,$min_mem_free,$min_mem_shared,$min_mem_buff,$min_mem_available,$min_swap_total,$min_swap_used,$min_swap_free,/home/$(whoami)/,${min_path_size}M" >> /home/$(whoami)/metrics/metrics_agg_${current_hour}.log
echo "maximum,$max_mem_total,$max_mem_used,$max_mem_free,$max_mem_shared,$max_mem_buff,$max_mem_available,$max_swap_total,$max_swap_used,$max_swap_free,/home/$(whoami)/,${max_path_size}M" >> /home/$(whoami)/metrics/metrics_agg_${current_hour}.log
echo "average,$avg_mem_total,$avg_mem_used,$avg_mem_free,$avg_mem_shared,$avg_mem_buff,$avg_mem_available,$avg_swap_total,$avg_swap_used,$avg_swap_free,/home/$(whoami)/,${avg_path_size}M" >> /home/$(whoami)/metrics/metrics_agg_${current_hour}.log

#memastikan file log hanya dapat dibaca oleh pemiliknya
chmod 400 /home/$(whoami)/metrics/metrics_agg_${current_hour}.log