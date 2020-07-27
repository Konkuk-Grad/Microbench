from pwn import *
from datetime import datetime
import os, signal, sys
import inspect, csv, copy

# Defines
run_file_name = "./signal_pp.out"

# Global variables
topology_list = {1: "Ping-pong"}
graph_list = {1: "Per cores", 2: "Per processes"}
process_list = []
max_cores = 0

def run_program(iter):
    # context.log_level = 'error'
    p = process([run_file_name] + [str(iter)])
    process_list.append(p)
    p.recvuntil("{")
    pids = [int(p.recvuntil(",")[:-1]), int(p.recvuntil("}")[:-1])]
    # context.log_level = 'INFO'
    return pids

# Menu
def init_menu():
    menu = {1: "Signal", 2: "IPC", 3: "Lock"}
    while True:
        mode = -1
        topology = -1
        while mode < 0 or mode > len(menu):

# Select Mode
            print("Select a testing type\n")
            print("1. Signal")
            print("2. IPC")
            print("3. Lock")
            print("0. exit\n")

            mode = int(input("type: "))

        if mode == 0:
            print("Terminate program")
            sys.exit(0)
        else :
            print("\n* Selected Mode: {0}".format(menu[mode]))
# Select Topology
        while topology < 0 or topology > len(topology_list):
            print("Select topology\n")
            for key in topology_list:
                print("{0}: {1}".format(key, topology_list[key]))
            print("0. exit\n")

            topology = int(input("Topology: "))

        if topology == 0:
            print("Terminate program")
            sys.exit(0)
        else :
            print("\n* Selected Mode: {0}, Topology: {1}\n".format(menu[mode], topology_list[topology]))

        if mode == 1: # 1. Signal
            sig_test(topology)
            return 1
        elif mode == 2:
            return 2
        elif mode == 3:
            return 3
        else:
            print("Invalid Input ({0})".format(mode))
            sys.exit(-1)



''' Signal '''
# Initial runution function
def sig_test(topology):
    print("#########\n1. Signal\n#########\n")
    
    test_attr = sig_test_setattr()    
    result = sig_test_all_run(test_attr)

    # Create a file named by datetime
    now = datetime.now()
    filename = "{0}{1}{2}{3}{4}{5}.csv".format(now.year, now.month, now.day, now.hour, now.minute, now.second)
    with open(filename, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile, delimiter=' ',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        csvwriter.writerow(result.values())    
    
    print("\n* Save successed (file name: {0})".format(filename))
    

# Set attribute
def sig_test_setattr():
    '''
    0: Number of processes or Number of processes' pairs
    1: Number of iterations
    2: Number of cores
    3: Number of tests
    4: Graph type
    5: Number of gap
    '''
    flag = 0
    num_of_attr = 6
    attr = []
    attr_name = {
                    0: "Number of processes", 1: "Number of iterations",
                    2: "Number of cores", 3: "Number of tests",
                    4: "Graph type", 5: "Number of gap"
                }
    while flag == 0:
        attr = [0 for i in range(num_of_attr)]

        while len(list(filter(lambda x: x <= 0, attr))):
            if attr[0] <= 0:
                attr[0] = int(input("Number of processes' pairs: "))
            if attr[1] <= 0:
                attr[1] = int(input("Number of iterations: "))
            if attr[2] <= 0:
                attr[2] = int(input("Number of cores (Max cores: {0}): ".format(max_cores)))
                attr[2] = -1 if attr[2] > max_cores else attr[2]
            if attr[3] <= 0:
                attr[3] = int(input("Number of tests: "))
            if attr[4] <= 0:
                attr[4] = int(input("Variations of graphs: "))
            if attr[4] == 2 and attr[5] <= 0:
                attr[5] = int(input("Gaps: "))

        if attr[4] != 2:
            attr[5] = 1

        print("\n* Test Attribute\n")
        for i in range(num_of_attr):
            print("{0}. {1}: {2}".format(i, attr_name[i], attr[i]))
        print()

        flag = 1
        # while flag == 0:
        #     flag = input("Confirm? (Y/N): ")
        #     print (flag, type(flag))
        #     if flag == "Y":
        #         print ("hello")
        #         flag = 1
        #         break
        #     elif flag == "N":
        #         print("wtf")
        #         flag = 0
        #         break
        #     else:
        #         print("???????")
        #         flag = 0



    return attr

    
def sig_test_init(test_attr):
    pcnt, ni, nc, nt, ng, ngap = test_attr

    pid_list = []
    affinity_mask = {i for i in range(0, nc)}

    process_list.clear()
    # context.log_level = 'error'
    for i in range(pcnt):
        pid_list.append(run_program(ni))

    for pid in pid_list:
        os.sched_setaffinity(pid[0], affinity_mask)
        os.sched_setaffinity(pid[1], affinity_mask)

    return pid_list


def sig_test_run(pid_list):
    for pid in pid_list:
        os.kill(pid[0], signal.SIGCONT)

    recv = []
    for p in process_list:
        p.recvuntil("{")
        recv.append(float(p.recvuntil("}")[:-1]))
    avg = sum(recv) / len(pid_list)
    # context.log_level = 'DEBUG'
    return avg

def sig_test_all_run(attr):
    result = {}
    
    # Increase number of processes' pairs from 1 
    plog = log.progress("Testing ", level = logging.CRITICAL)
    # context.log_level = 'ERROR'
    
    for i in range(1, attr[0] + 1, attr[5]): # attr[0]: Number of processes', attr[5] = gap
        # Number of tests
        for j in range(attr[3]):
            # context.log_level = 'INFO'
            plog.status("Total pairs: {0}, Current pairs: {1}, Current iterations: {2}".format(attr[0], i, j + 1))
            # context.log_level = 'ERROR'
            # Single runution
            test_attr = copy.deepcopy(attr)
            test_attr[0] = i
            
            pid_list = sig_test_init(test_attr)
            single_result = sig_test_run(pid_list)
            
            try:
                result[i] += single_result
            except:
                result[i] = single_result

    # context.log_level = 'INFO'
    
    plog.success("Hello world!")
        
    for key in result:
        result[key] /= attr[3]
    for key in result:
        print ("{0}: {1}".format(key, result[key]))

    return result


# Program starts
if __name__ == "__main__":
    max_cores = os.cpu_count()
    init_menu()
    