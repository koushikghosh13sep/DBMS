#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: ./TestAll [Executable-Name]"
    exit 1;
fi

for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20
do
	./$1 $i 
done
	
