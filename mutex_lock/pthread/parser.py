import subprocess
import time
import seaborn as sns
import matplotlib


import pandas as pd
import numpy as np
# import matplotlib.pyplot as plt

from parse import compile
iter = 10;
inst = 'sudo ./pthread_general '

tryCount = 100000
# Thread   0 is Ended with  85.739479 status : 44938 total response time :  58.272597 realrunning time :  27.466882 critical Entry ratio :  67.964720
p = compile('Thread {} is Ended with {art} status : {tid} aver response time : {atrt} realrunning time : {arrt} critical Entry ratio :  {acer}\n')
pid = []
aver_running_time = [[] for i in range(iter)]
aver_total_response_time = [[] for i in range(iter)]
aver_real_running_time = [[] for i in range(iter)]
aver_critical_Entry_ratio = [[] for i in range(iter)]
threadNum_list = range(100)

for i in range(iter) : 
    for threadNum in range(100) :
        cmd = subprocess.Popen(inst+" "+str(threadNum+1)+" "+str(tryCount), shell=True, stdout=subprocess.PIPE)
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

        aver_running_time[i].append(total_runing_time/(threadNum+1))
        aver_total_response_time[i].append(total_total_response_time/(threadNum+1))
        aver_real_running_time[i].append(total_real_running_time/(threadNum+1))
        aver_critical_Entry_ratio[i].append(total_critical_entry_ratio/(threadNum+1))
# sns.set(style="whitegrid")
# df = pd.DataFrame(dict(response_time= aver_total_response_time),threadNum_list)
df_res = pd.DataFrame(aver_total_response_time, columns = range(100))
df_total = pd.DataFrame(aver_running_time, columns = range(100))
# print(df)
df_total.to_csv('/home/ep/Microbench/mutex_lock/pthread/res.csv',
sep=',',
na_rep = 'NaN')
df_res.to_csv('/home/ep/Microbench/mutex_lock/pthread/res.csv',
sep=',',
na_rep = 'NaN')

# # current_palette = sns.color_palette()
# # sns.palplot(current_palette)
# g = sns.lineplot(data=df,linewidth=2.5, palette ="tab10")
# # g.fig.autofmt_xdate()
# matplotlib.pyplot.show()

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