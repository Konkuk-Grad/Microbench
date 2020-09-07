from pwn import *
from datetime import datetime
from functools import reduce
import os, signal, sys
import inspect, csv, copy

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
graph_list = {1: "Per cores", 2: "Per processes"}
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
    attr = []
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
            if attr[1] <= 0:
                attr[1] = int(input(attr_name[1] + ": "))

            if attr[2] <= 0:
                attr[2] = int(input(attr_name[2] + ": "))

            if attr[3] <= 0:
                attr[3] = int(input(attr_name[3] + ": "))
            
            if attr[4] <= 0:
                attr[4] = int(input(attr_name[4] + ": "))
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
        p = process([run_file_name] + [str(i) for i in exec_attr])
        p.recvuntil("{")
        exec_time.append(float(p.recvuntil("}")[:-1]))
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
    7. topology
    '''
    test_attr = copy.deepcopy(attr)
    result_list = []
    result_avg_list = []
    x_axis=[i for i in range(1, attr[1] + 1, attr[5])]
    for i in range(1, attr[1] + 1, attr[5]):
        test_attr[1] = i
        result_list.append(exec_single_test(test_attr))
        
    for i in range(len(result_list)):
        result_avg_list.append(sum(result_list[i]) / attr[3])
    
    filename = make_csv(x_axis, result_avg_list)
    return filename
    
    
def make_csv(x_axis, y_axis):
    now = datetime.now()
    filename = "{0}{1}{2}{3}{4}{5}.csv".format(now.year, now.month, now.day, now.hour, now.minute, now.second)
    with open(filename, 'w', newline='\n') as csvfile:
        csvfile.write("index,time\n")
        for i in range(len(x_axis)):
            csvfile.write("{0},{1}\n".format(x_axis[i], y_axis[i]))
    
    print("\n* CSV File is saved (file name: {0})".format(filename))
    return filename

def print_graph(filename):
    df = pd.read_csv(filename, index_col='index')
    plot = df.plot(marker='o')

    plot.set_xlabel('index')
    plot.set_ylabel('time (ms)')

    plt.grid(True)
    plt.minorticks_on()

    # Save graph image file
    plt.savefig("./" + filename[:-3])
    return filename[:-3] + ".png"

# Menu
def init_menu():
    mode = -1
    topology = -1
    while mode <= 0 or topology <= 0:
        mode = select_mode() if mode <= 0 else mode    
        if mode <= 0:
            continue

        topology = select_topology() if topology <= 0 else topology
    
    attr = input_value() + [mode, topology]
    return exec_whole_tests(attr)


# Program starts
if __name__ == "__main__":
    filename = init_menu()
    ip = input("Save as png file? (Y/N): ")
    if 'Y' in ip or 'y' in ip:
        print_graph(filename)

    # filename = "202097152945.csv"
    # print_graph(filename)

    
    