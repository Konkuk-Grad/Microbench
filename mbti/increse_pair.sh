# num_process
for ((i=0 ; i<9; i++)); do
    let "num_process = 1<<$i"
    # echo $num_process
        for ((j=0; j<10; j++)); do
            echo 3 > /proc/sys/vm/drop_caches
            echo "`./mbti.out $1 $2 ${num_process} 1000000 16`"
    # echo $measure_time
        done
    echo "------------" 
done