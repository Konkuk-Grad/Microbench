# num_process $1 is mode, $2 is topology
for ((i=0 ; i<16; i++)); do
    # let "num_process = 1<<$i"
        for ((j=0; j<10; j++)); do
            echo 3 > /proc/sys/vm/drop_caches
            echo "`./mbti.out $1 $2 6 1000000 ${i+1}`" | awk '{print $2}'
    # echo $measure_time
        done
    echo "------------" 
done