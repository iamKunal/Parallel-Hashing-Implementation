#!/usr/bin/env python

from pwn import *

context.log_level='error'

table_size=190
step_size=10
N=10**5

table_max=600+1

total_col=[]
for i in xrange(table_size,table_max,step_size):
    
    r=process(['./parallel.out',str(N),str(i)])
    print "Table Size =",i
    output=r.recvall().split('\n')
    cols=[]
    p_done=False
    for k in output:
        for line in output:
            if("left out" in line):
                cols.append(float(line.split()[-1]))
            if("Success" in line):
                p_done=True
                cols.append(0)
    p=sum(cols)//len(cols)
    r.kill()
    
#    r=process(['./sequential.out', str(N),str(i)])

#    output=r.recvall().split('\n')
#    cols=[]
#    s_done=False
#    for k in output:
#        for line in output:
#            if("left out" in line):
#                cols.append(float(line.split()[-1]))
#            if("Success" in line):
#                s_done=True
#    s=sum(cols)/float(len(cols))
#    r.kill()
#    total_times.append([i,p,s,p_done,s_done])
    total_col.append([i,p,p_done])
    for t in total_col:
        print t
with open('observations/collisions_%d.csv' %(step_size), 'w') as f:
    f.write('Table Size\tCollisions\tDone\n')
    for t in total_col:
        i,p,p_done=t
        f.write("%d\t%d\t%d\n" %(i,p,p_done))
