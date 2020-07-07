from pwn import *
import os, signal
import inspect

ping_file_name = "./signal_ping.out"
pong_file_name = "./signal_pong.out"

ping_plist = []
pong_plist = []

pcnt = 100
def exec_program(iter = 10, arg = []):
    arg = [str(i) for i in arg]
    print(arg)
    p1, p2 = process([ping_file_name] + arg), process([pong_file_name] + arg) 
    # print(inspect.getmembers(p1))
    ping_plist.append(p1)
    # p1.recvuntil("[Ping] Ping PID")
    # print(p1.recvline())
    pong_plist.append(p2)
    # p2.recvuntil("[Pong] Ping PID")
    # print(p2.recvline())

for i in range(pcnt):
    # [Iteration count, Message Queue]
    exec_program(10, [2000 + i, 1000])

# ping_plist[0].interactive()
recv = []
for i in range(pcnt):
    print ("Wait: {0}".format(i))
    recv.append(ping_plist[i].recvuntil("ms")[-11:-3])
for i in range(pcnt):
    print("[{0}]: {1} ms".format(i, recv[i]))

# INPUT FROM USER
# TRIGGER PROGRAMS
# WAIT UNTIL BEING RESULTED
# PRINT RESULT