#!/bin/bash

# Ask for input
echo "Enter data, separated by commas (No.,Nama_Pengusul,Departemen,Fakultas,Judul,Pendamping,Skema):"
read -a input_data

# Check if user already exists in the CSV file
user_data=$(echo "${input_text}\n" | sed 's/ //g; s/,/ /g')
  # Convert input data to space-separated string
if grep -qi ${input_data[1]} resources/data-pkm.csv; then
    echo "$(date '+%m/%d/%Y %H:%M:%S') REGISTER: ERROR ${input_data[1]} already existed" >> task-2/output/log.txt
    exit 1
fi
echo "${input_data[*]}" >> resources/data-pkm.csv

# Print success message
echo "$(date '+%m/%d/%Y %H:%M:%S') REGISTER: SUCCESS ${input_data[1]} is registered. Proposal ${input_data[0]} is added" >> task-2/output/log.txt


