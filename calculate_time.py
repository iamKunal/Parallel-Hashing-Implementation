#!/usr/bin/env python

from pwn import *

context.log_level='error'

N=10**4
step=10**4
N_MAX=2*10**5+1
table_size=192

total_times=[]

for i in xrange(N,N_MAX,step):

    r=process(['./parallel.out',str(i),str(table_size)])
    print "N =",i
    output=r.recvall().split('\n')
    times=[]
    for k in output:
        for line in output:
            if("Time" in line):
                times.append(float(line.split()[-1]))
    #print list(set(times))
    p=sum(times)/float(len(times))
#    print "Parallel Time : ", p
    r.kill()
    
    r=process(['./sequential.out', str(i),str(table_size)])

    output=r.recvall().split('\n')
    times=[]
    for k in output:
        for line in output:
            if("Time" in line):
                times.append(float(line.split()[-1]))
    #print list(set(times))
    s=sum(times)/float(len(times))
#    print "Sequential : ", s
    r.kill()
    total_times.append([i,p,s])
    for t in total_times:
        print t
with open('observations/seq_vs_parallel_%d.csv' %(N/10000), 'w') as f:
    f.write('N\tParallel\tSequential\n')
    for t in total_times:
        i,p,s=t
        f.write("%d\t%f\t%f\n" %(i/10000,p,s))
