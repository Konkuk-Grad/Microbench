from pwn import *
import os, signal, sys
import inspect

exec_file_name = "./signal_pp.out"

process_list = []

def exec_program(iter = 10):
    p = process([exec_file_name] + [str(iter)])
    process_list.append(p)
    p.recvuntil("{")
    return int(p.recvuntil(",")[:-1])

if __name__ == "__main__":
    # INPUT FROM USER
    pcnt = 1
    noi = 10
    try :
        pcnt = int(sys.argv[1])
        noi = int(sys.argv[2])
    except:
        print("[Error] sys.argv[1] or sys.argv[2] is not a positive integer. (Input: {0}, {1})".format(sys.argv[1], sys.argv[2]))
    
    context.log_level = 'error'
    pid_list = []
    for i in range(pcnt):
        # [Iteration count, Message Queue]
        pid_list.append(exec_program(noi))

    for pid in pid_list:
        os.kill(pid, signal.SIGCONT)
    
    # WAIT UNTIL BEING RESULTED
    recv = []
    for i in range(pcnt):
        # print ("Wait: {0}".format(i))
        # process_list[i].recvuntil("[Average]")
        process_list[i].recvuntil("{")
        recv.append(float(process_list[i].recvuntil("}")[:-1]))

    avg = 0
    for i in range(pcnt):
        # print("[{0}]: {1} ms".format(i, recv[i]))
        avg += recv[i]
    context.log_level = 'DEBUG'
    # print("NoP: {0} / NoI: {1} / ART: {2} ms".format(pcnt, noi, avg / pcnt))
    print(avg / pcnt)