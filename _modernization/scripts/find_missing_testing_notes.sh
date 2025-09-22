#!/bin/bash

echo "Checking memoryX.c for missing Testing Notes..."

grep -n "PARM_" Src/memoryX.c | while IFS=: read line_num rest; do
    func_name=$(echo "$rest" | cut -d' ' -f1)

    # Extract the function section starting from this line
    section=$(sed -n "${line_num},+50p" Src/memoryX.c)

    if ! echo "$section" | grep -q "Testing Notes:"; then
        echo "Missing Testing Notes: $func_name (line $line_num)"
    fi
done