#!/bin/bash

#target path yang akan dimonitoring
target_path="/home/$(whoami)/"

#path penyimpanan log
save_path="/home/$(whoami)/metrics"

#buat direktori untuk menyimpan log
mkdir -p "$save_path" || { echo "Gagal membuat direktori $save_path"; exit 1; }

#pola nama file log
log_file="$save_path/metrics_$(date +"%Y%m%d%H%M%S").log"

#fungsi untuk mencatat metrik
log_metrics() {
  #metrik penggunaan RAM
  ram_metrics=$(free -m | awk 'NR==2 {printf "%d,%d,%d,%d,%d,%d", $2, $3, $4, $5, $6, $7}')

  #metrik memori swap
  swap_metrics=$(free -m | awk 'NR==3 {printf ",%d,%d,%d", $2, $3, $4}')

  #metrik penggunaan disk
  disk_metrics=$(du -sh "$target_path" | awk -v target_path="$target_path" '{printf ",%s,%s\n", target_path, $1 }')

  #menulis metrik ke file log
  echo "mem_total,mem_used,mem_free,mem_shared,mem_buff,mem_available,swap_total,swap_used,swap_free,path,path_size" >> "$log_file"
  echo "$ram_metrics$swap_metrics$disk_metrics" >> "$log_file"
}

#cetak metrik
log_metrics
