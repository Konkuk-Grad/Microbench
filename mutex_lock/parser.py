import subprocess
import time
import matplotlib


import pandas as pandas
import numpy as numpy
import matplotlib.pyplot as plt

from parse import compile

inst = './pthread_general '

tryCount = 100000

p = compile('Thread {} is Ended with {t} status : 0\n')
aver = []
threadNum_list = []


for threadNum in range(10) :
    cmd = subprocess.Popen(inst+" "+str(threadNum+1)+" "+str(tryCount), shell=True, stdout=subprocess.PIPE)
    threadNum_list.append(threadNum+1)
    tmp = 0
    time.sleep(1)
    for line in cmd.stdout:
        r = p.parse(line.decode('ascii'))
        print(line.decode('ascii'))
        print(r)
        tmp += float(r['t'])
    aver.append(tmp/(threadNum+1))
    

for item in aver :
    print(item)

# p = compile("It's {}, I love it!")
# print(p)
# r=p.parse("It's spam, I love it!")
# print(r)


# cmd = subprocess.Popen(inst+" "+str(threadNum)+" "+str(tryCount), shell=True, stdout=subprocess.PIPE)
# for line in cmd.stdout:
#     if "thread" in line:
#         print(line)

# Thread  83 is Ended with 194.502948 status : 0