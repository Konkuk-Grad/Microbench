import subprocess
import time
import seaborn as sns
import matplotlib


import pandas as pd
import numpy as np
# import matplotlib.pyplot as plt

from parse import compile

inst = 'sudo ./pthread_general '

tryCount = 1000000
# Thread   0 is Ended with  85.739479 status : 44938 total response time :  58.272597 realrunning time :  27.466882 critical Entry ratio :  67.964720
p = compile('Thread {} is Ended with {art} status : {tid} aver response time : {atrt} realrunning time : {arrt} critical Entry ratio :  {acer}\n')
pid = []
aver_running_time = []
aver_total_response_time = []
aver_real_running_time = []
aver_critical_Entry_ratio = []
threadNum_list = []


for threadNum in range(100) :
    cmd = subprocess.Popen(inst+" "+str(threadNum+1)+" "+str(tryCount), shell=True, stdout=subprocess.PIPE)
    threadNum_list.append(threadNum+1)
    total_runing_time = 0
    total_total_response_time = 0
    total_real_running_time = 0
    total_critical_entry_ratio = 0 
    # time.sleep(1)
    for line in cmd.stdout:
        r = p.parse(line.decode('ascii'))
        print(line.decode('ascii'))
        print(r)
        total_runing_time += float(r['art'])
        total_total_response_time += float(r['atrt'])
        total_real_running_time += float(r['arrt'])
        total_critical_entry_ratio += float(r['acer'])

    aver_running_time.append(total_runing_time/(threadNum+1))
    aver_total_response_time.append(total_total_response_time/(threadNum+1))
    aver_real_running_time.append(total_real_running_time/(threadNum+1))
    aver_critical_Entry_ratio.append(total_critical_entry_ratio/(threadNum+1))
    
# fig = plt.figure()
# fig.suptitle("pthread Count up")
# fig, ax_lst = plt.subplots(2, 2, figsize=(8,5))
# ax_lst[0][0].plot(threadNum_list, aver_running_time,'bo:')
# ax_lst[0][1].plot(threadNum_list, aver_total_response_time,'bo:')
# ax_lst[1][0].plot(threadNum_list, aver_real_running_time,"bo:")
# ax_lst[1][1].plot(threadNum_list, aver_critical_Entry_ratio,"bo:")
# plt.show()
sns.set(style="whitegrid")
df = pd.DataFrame(dict(total_time = aver_running_time),threadNum_list)
# current_palette = sns.color_palette()
# sns.palplot(current_palette)
g = sns.lineplot(data=df,linewidth=2.5, palette ="tab10")
# g.fig.autofmt_xdate()
matplotlib.pyplot.show()

# for item in aver :
    # print(item)

# p = compile("It's {}, I love it!")
# print(p)
# r=p.parse("It's spam, I love it!"
# print(r)


# cmd = subprocess.Popen(inst+" "+str(threadNum)+" "+str(tryCount), shell=True, stdout=subprocess.PIPE)
# for line in cmd.stdout:
#     if "thread" in line:
#         print(line)

# Thread  83 is Ended with 194.502948 status : 0