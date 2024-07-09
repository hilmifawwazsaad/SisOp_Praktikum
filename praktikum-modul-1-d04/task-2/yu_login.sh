#!/bin/bash

pkm_csv_file="resources/data-pkm.csv"

# Read user input
echo "Please enter a first name: "
read name

echo "Please enter a password: "
read password

time_input=$(date '+%m/%d/%Y %H:%M:%S')

# Use awk to check if the name and password match the user input
login_attempt=$(awk -F, -v name="$name" -v password="$password" 'BEGIN { found=0 } {
    split($2, a, "_");
    split($4, b, " "); split($6, c, " "); gsub(/[()]/, "", c[length(c)]);
    combined_string = b[1] c[length(c)];
    if (a[1] == name && combined_string == password) {
        print "LOGIN: SUCCESS " name " is logged in";
        found=1;
        exit; # Exit from awk after finding the first match
    }
} END {
    if (found == 0) {
        print "LOGIN: ERROR Failed login attempt on " nama;
    }
}' "$pkm_csv_file")


# Write awk output and current date/time to the output file
echo "$time_input" "$login_attempt" >> task-2/output/log.txt
