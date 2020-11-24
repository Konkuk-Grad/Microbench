from pwn import *
from datetime import datetime
from functools import reduce
import os, signal, sys
import inspect, csv, copy

# import logging
# logger = logging.getLogger(__name__)
# logger.setLevel(logging.DEBUG)

# formatter = logging.Formatter('%(asctime)s:%(module)s:%(levelname)s => %(message)s  ', '%Y-%m-%d %H:%M:%S')

# # INFO 레벨 이상의 로그를 콘솔에 출력하는 Handler
# console_handler = logging.StreamHandler()
# console_handler.setLevel(logging.INFO)
# console_handler.setFormatter(formatter)
# logger.addHandler(console_handler)

# # DEBUG 레벨 이상의 로그를 `debug.log`에 출력하는 Handler
# debug_handler = logging.FileHandler('debug.log')
# debug_handler.setLevel(logging.DEBUG)
# debug_handler.setFormatter(formatter)
# logger.addHandler(debug_handler)

# # ERROR 레벨 이상의 로그를 `error.log`에 출력하는 Handler
# error_handler = logging.FileHandler('error.log')
# error_handler.setLevel(logging.ERROR)
# error_handler.setFormatter(formatter)
# logger.addHandler(error_handler)

# graph

import pandas as pd
from pandas import DataFrame
import matplotlib as matplotlib
import matplotlib.pyplot as plt

# Defines
run_file_name = "./mbti.out"

# Global variables
mode_list = {1: "Signal", 2: "IPC", 3: "Lock"}
topology_list = {1: "Ping-pong"}
lock_list = {1: "Semaphore", 2: "Mutex"}
graph_list = {1: "Per cores", 2: "Per processes"}

# File types
MODE_TYPE = {1: "SIG", 2: "IPC", 3: "MUT", 4: "SEM"}
TOPOL_TYPE = {1: "PPT"}
GRAPH_TYPE = {1: "COR", 2: "PRC"}

process_list = []

def select_mode():
    print("Select a testing type\n")
    for key in mode_list:
        print("{0}. {1}".format(key, mode_list[key]))
    print("0. exit\n")

    mode = int(input("type: "))

    if mode == 0:
        print("Terminate program")
        sys.exit(0)
    elif not mode in mode_list:
        print("Not found mode [{0}]\n".format(mode))
        return -1
    else:
        print("\n* Selected Mode: {0}".format(mode_list[mode]))
        return mode

def select_topology():
    print("Select topology\n")
    for key in topology_list:
        print("{0}: {1}".format(key, topology_list[key]))
    print("0. exit\n")

    topology = int(input("Topology: "))

    if topology == 0:
        print("Terminate program")
        sys.exit(0)
    elif not topology in topology_list:
        print("Not found topology [{0}]\n".format(topology))
        return -1
    else:
        return topology
    
def select_lock_mode():
    print("Select Lock mode\n")
    for key in lock_list:
        print("{0}: {1}".format(key, lock_list[key]))
    print("0. exit\n")

    lock = int(input("Lock: "))

    if lock == 0:
        print("Terminate program")
        sys.exit(0)
    elif not lock in lock_list:
        print("Not found lock [{0}]\n".format(lock))
        return -1
    else:
        return lock


def input_value():
    '''
    0. Number of cores
    1. Number of processes or processes' pairs
    2. Number of pattern iterations
    3. Number of tests
    4. Variations of graphs
    5. Gaps
    '''
    flag = 0
    max_cores = os.cpu_count()
    attr_name = {
                    0: "Number of cores", 1: "Number of processes or processes' pairs",
                    2: "Number of pattern iterations", 3: "Number of tests", 4: "Variations of graphs", 5: "Gaps"
                }
    num_of_attr = len(attr_name)
    attr = [0 for i in range(num_of_attr)]
    while flag == 0:
        
        while len(list(filter(lambda x: x <= 0, attr))):
            if attr[0] <= 0:
                attr[0] = int(input(attr_name[0] + " (Max cores: {0}): ".format(max_cores)))
                if attr[0] > max_cores:
                    print("[!] The number entered exceeds max cores {0}".format(max_cores))
                    attr[0] = -1
                    continue
                elif attr[0] <= 0:
                    print("[!] The number entered less equal than 0")
                    attr[0] = -1
                    continue

            if attr[1] <= 0:
                attr[1] = int(input(attr_name[1] + ": "))
                if (attr[1] * 2) > 512:
                    print("[!] The number entered exceeds processes max count (max: 256 pairs, 512 processes/threads) input: {0}".format(attr[1]))
                    attr[1] = -1
                    continue
                elif (attr[1] * 2) <= 0:
                    print("[!] The number entered less equal than 0. input: {0}".format(attr[1]))
                    attr[1] = -1
                    continue

            if attr[2] <= 0:
                attr[2] = int(input(attr_name[2] + ": "))
                if attr[2] > 1000000:
                    print("[!] The number entered exceeds iteration max count (max: 1000000) input: {0}".format(attr[2]))
                    attr[2] = -1
                    continue
                elif attr[2] <= 0:
                    print("[!] The number entered less equal than 0. input: {0}".format(attr[2]))
                    attr[2] = -1
                    continue

            if attr[3] <= 0:
                attr[3] = int(input(attr_name[3] + ": "))
            
            # 4: Variations of graphs
            if attr[4] <= 0:
                attr[4] = int(input(attr_name[4] + " (" + ", ".join(["{0}: {1}".format(key, graph_list[key]) for key in graph_list]) + "): "))
                if not attr[4] in graph_list.keys():
                    attr[4] = -1
            
            if attr[4] == 2 and attr[5] <= 0: # Process들의 개수 증가 추이
                attr[5] = int(input(attr_name[5] + ": "))
            elif attr[4] == 1 and attr[5] <= 0:
                attr[5] = 1 # Core 증가하면서 진행


        print("\n* Test Attribute\n")
        for i in range(num_of_attr):
            print("{0}. {1}: {2}".format(i, attr_name[i], attr[i]))
        print()

        ip = input("Confirm? (Y/N/0: exit): ")
        if 'Y' in ip or 'y' in ip:
            flag = 1
            break
        elif 'N' in ip or 'n' in ip:
            attr = [0 for i in range(num_of_attr)]
            flag = 1
        elif '0' in ip:
            print("Terminate program")
            sys.exit(0)
        
    return attr

def exec_single_test(attr):
    exec_attr = [attr[6], attr[7], attr[1], attr[2], attr[0]]
    exec_time = []
    for i in range(attr[3]):
        print("[{5}] mode: {0}, topology: {1}, NoP: {2}, NoI: {3}, NoC:{4}".format(exec_attr[0], exec_attr[1], exec_attr[2], exec_attr[3], exec_attr[4], i))
        p = process([run_file_name] + [str(i) for i in exec_attr])
        p.recvuntil("{")
        received_time = float(p.recvuntil("}")[:-1])
        # print("[*] Receive single time: " + str(received_time))
        exec_time.append(received_time)
        sleep(0.5)
    
    return exec_time

def exec_whole_tests(attr):
    '''
    0. Number of cores
    1. Number of processes or processes' pairs
    2. Number of pattern iterations
    3. Number of tests
    4. Variations of graphs
    5. Gaps
    6. mode
    7. lock
    8. topology
    '''
    test_attr = copy.deepcopy(attr)
    result_list = []
    result_avg_list = []
    
    y_axis = 0
    if(attr[4] == 1): # Per cores
        y_axis = 0
    elif (attr[4] == 2): # Per processes
        y_axis = 1
    else:
        print("Invalid Graphs: ", attr[4])
    
    x_axis=[i for i in range(1, attr[y_axis] + 1, attr[5])]

    for i in range(1, attr[y_axis] + 1, attr[5]):
        test_attr[y_axis] = i
        temp_list = exec_single_test(test_attr)
        # print("[*] Receive Time: " + str(temp_list))
        result_list.append(temp_list)
        
    for i in range(len(result_list)):
        # print("[#] Average Time: {0}".format(sum(result_list[i]) / attr[3]))
        result_avg_list.append(sum(result_list[i]) / attr[3])
    filetype = [MODE_TYPE[attr[6]], TOPOL_TYPE[attr[7]], GRAPH_TYPE[attr[4]]]

    filename = make_csv(x_axis, result_avg_list, filetype)
    return filename
    
    
def make_csv(x_axis, y_axis, filetype):
    now = datetime.now()
    filename = "{0}{1:02d}{2:02d}{3:02d}{4:02d}{5:02d}_{6}_{7}_{8}.csv".format(
                now.year, now.month, now.day, now.hour, now.minute, now.second,
                filetype[0], filetype[1], filetype[2]
                )
    with open(filename, 'w', newline='\n') as csvfile:
        csvfile.write("index,time\n")
        for i in range(len(x_axis)):
            csvfile.write("{0},{1}\n".format(x_axis[i], y_axis[i]))
    
    print("\n* CSV File is saved (file name: {0})".format(filename))
    return filename

def print_graph(filename):
    df = pd.read_csv(filename, index_col='index')
    plot = df.plot(marker='o')

    x_axis = list(df.index)
    y_axis = list(df["time"])

    x_label = graph_list[[k for k, v in GRAPH_TYPE.items() if v == filename[-7:-4]][0]]
    # plot.bar(x, y)
    for i, v in enumerate(x_axis):
        plot.text(v, y_axis[i], "{0:.4f}".format(y_axis[i]),
                fontsize = 9,
                color='blue',
                horizontalalignment='center',
                verticalalignment='bottom')

    plot.set_xticks(x_axis)
    
    plot.set_xlabel(x_label)
    plot.set_ylabel('time (ms)')

    plt.grid(True)
    # plt.minorticks_on()

    # Save graph image file
    plt.savefig("./" + filename[:-3])
    return filename[:-3] + ".png"

# Menu
def init_menu():
    mode = -1
    lock = 0
    topology = -1
    while mode <= 0 or lock <= 0 or topology <= 0:
        mode = select_mode() if mode <= 0 else mode    
        if mode <= 0:
            continue
        elif mode == 3: # Lock
            lock = select_lock_mode()
            if lock <= 0:
                continue
            else:
                mode += lock - 1 if lock > 0 else lock
        elif mode <= 2:
            lock = 1
        topology = select_topology() if topology <= 0 else topology

    attr = input_value() + [mode, topology]
    return exec_whole_tests(attr)


# Program starts
if __name__ == "__main__":
    filename = init_menu()
    ip = input("Save as png file? (Y/N): ")

    if 'Y' in ip or 'y' in ip:
        print_graph(filename)

    # filename = "20209281473.csv"
    # print_graph(filename)