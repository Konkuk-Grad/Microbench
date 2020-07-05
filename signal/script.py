from pwn import *
import os, signal, sys
import inspect

exec_file_name = "./signal_pp.out"

process_list = []

def exec_program(iter = 10):
    p = process([exec_file_name] + [str(iter)])
    process_list.append(p)

if __name__ == "__main__":
    # INPUT FROM USER
    pcnt = 1
    try :
        pcnt = int(sys.argv[1])
    except:
        print("[Error] sys.argv[1] is not a positive integer. (Input: {0})".format(sys.argv[1]))
    
    for i in range(pcnt):
        # [Iteration count, Message Queue]
        exec_program(10)

    # WAIT UNTIL BEING RESULTED
    recv = []
    for i in range(pcnt):
        print ("Wait: {0}".format(i))
        recv.append(process_list[i].recvuntil("ms")[:-3])

    for i in range(pcnt):
        print("[{0}]: {1} ms".format(i, recv[i]))