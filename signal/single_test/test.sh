#!/bin/bash

echo "Start 256 100000"
for (( i = 0; i < 10; i++));do
	print $i
	python3 script.py 256 100000 >> result_256.txt
done

echo "Complete 256 100000"
echo "Start 512 100000"
for (( i = 0; i < 10; i++));do
	print $i
	python3 script.py 512 100000 >> result_512.txt
done

echo "Complete 512 100000"
echo "Start 1024 100000"
for (( i = 0; i < 10; i++));do
	print $i
	python3 script.py 1024 100000 >> result_1024.txt
done

echo "Complete 1024 100000"
