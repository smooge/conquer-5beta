#!/bin/bash

if [[ -d Src ]]; then
    (for file in Src/*.c;  do
	echo "$(basename "$file"): $(grep -c 'Testing Notes:' "$file") Testing Notes, $(grep -c 'PARM_' "$file") functions"
     done ) | sort -k2,2 -k 5,5 -nr > /tmp/test_notes.txt
else
    echo "Wrong directory. Find the base directory and use it."
fi