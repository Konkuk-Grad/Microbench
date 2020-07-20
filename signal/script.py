from pwn import *
import os, signal, sys
import inspect

exec_file_name = "./signal_pp.out"

process_list = []

max_cores = 0
def exec_program(iter = 10):
    p = process([exec_file_name] + [str(iter)])
    process_list.append(p)
    p.recvuntil("{")
    g = [int(p.recvuntil(",")[:-1]), int(p.recvuntil("}")[:-1])]
    return g

def init_menu():
    ip = -1
    while ip < 0 or ip >= 3:

        print("Select a testing type")
        print("1. Signal")
        print("2. IPC")
        print("3. Lock")
        print("0. exit")

        ip = int(input())

    if ip == 1: # 1. Signal
        sigtest()
        return 1
    elif ip == 2:
        return 2
    elif ip == 3:
        return 3
    elif ip == 0:
        print("Terminate program")
        sys.exit(0)
    else:
        print("Invalid Input ({0})".format(ip))
        sys.exit(-1)



''' Signal '''
# Initial execution function
def sigtest():
    print("1. Signal")
    context.log_level = 'error'
    
    test_attr = sigtestsetting()
    sigtestiterexec(test_attr)

    context.log_level = 'DEBUG'

    



# Set attribute
def sigtestsetting():
    pcnt, ni, nc, nt, ng = [0,0,0,0,0]
    while pcnt <= 0 or ni <= 0 or nc <= 0 or nt <= 0:
        if pcnt <= 0:
            pcnt = int(input("Number of processes: "))
        if ni <= 0:
            ni = int(input("Number of iterations: "))
        if nc <= 0:
            nc = int(input("Number of cores: "))
            nc = -1 if nc > max_cores else nc
        if nt <= 0:
            nt = int(input("Number of tests:"))
        if ng <= 0:
            ng = int(input("Variations of graphs: "))

    return [pcnt, ni, nc, nt, ng]

    
def sigtestinit(test_attr):
    pcnt, ni, nc, nt, ng = test_attr

    pid_list = []
    affinity_mask = {i for i in range(0, nc)}

    process_list.clear()
    for i in range(pcnt):
        pid_list.append(exec_program(ni))

    for pid in pid_list:
        print(pid)
        os.sched_setaffinity(pid[0], affinity_mask)
        os.sched_setaffinity(pid[1], affinity_mask)

    return pid_list


def sigtestexec(pid_list):
    for pid in pid_list:
        os.kill(pid[0], signal.SIGCONT)

    recv = []
    for p in process_list:
        p.recvuntil("{")
        recv.append(float(p.recvuntil("}")[:-1]))
    avg = sum(recv) / len(pid_list)
    return avg

def sigtestiterexec(test_attr):
    result = {}
    for i in range(test_attr[3]):
        pid_list = sigtestinit(test_attr)
        result[i] = sigtestexec(pid_list)

    for key in result:
        print ("{0}: {1}".format(key, result[key]))

    return result


# Program starts
if __name__ == "__main__":
    max_cores = os.cpu_count()
    init_menu()
    