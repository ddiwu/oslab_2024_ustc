#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(void)
{
    int num1, num2;
    int pid1[400], pid2[400],pid[400];
    char comm1[400][16], comm2[400][16],comm[400][16];
    int sum_exec_runtime1[400], sum_exec_runtime2[400], sum_exec_runtime[400];
    int isrunning1[400], isrunning2[400], isrunning[400];
    double cpu1[400],cpu2[400], cpu[400];
    int cnt;

    int t_p;
    char t_comm[16];
    int t_runtime;
    int t_isrunning;
    double t_cpu;
    int temp = 0;
    

    syscall(322, &num1, pid1, comm1, isrunning, sum_exec_runtime1);
    while(1)
    {
        sleep(1);
        syscall(322, &num2, pid2, comm2, isrunning, sum_exec_runtime2);
        cnt = 0;
        for(int i = 0; i< num2; i++)
        {
            for(int j = 0; j < num1; j++)
            {
                if(strcmp(comm1[j], comm2[i]) == 0)
                {
                    pid[i] = pid2[i];
                    strcpy(comm[i], comm2[i]);
                    isrunning[i] = isrunning2[i];
                    sum_exec_runtime[i] = sum_exec_runtime2[i];
                    cpu[i] = (double)(sum_exec_runtime2[i] - sum_exec_runtime1[j]) / 10000000;//百分比
                    cnt++;
                    break;
                }
            }
        }
        
        //将这些组数据按照cpu值从大到小排序
        for (int i = 0; i < cnt;  i++)
        {
            for (int j =i; j< cnt; j++)
            {
                if (cpu[j] > cpu[temp])
                {
                    temp = j;
                }
            }
            t_p = pid[i];
            pid[i] = pid[temp];
            pid[temp] = t_p;
            t_runtime = sum_exec_runtime[i];
            sum_exec_runtime[i] = sum_exec_runtime[temp];
            sum_exec_runtime[temp] = t_runtime;
            t_isrunning = isrunning[i];
            isrunning[i] = isrunning[temp];
            isrunning[temp] = t_isrunning;  
            t_cpu = cpu[i];
            cpu[i] = cpu[temp];
            cpu[temp] = t_cpu;
            strcpy(t_comm, comm[i]);
            strcpy(comm[i], comm[temp]);
            strcpy(comm[temp], t_comm);
        }
        
        system("clear");
        printf("%-5s%-16s%-10s%-5s%-8s\n", "PID", "COMM", "ISRUNNING", "CPU","TIME");
        for (int i = 0; i < 20; i++) {
            printf("%-5d%-16s%-10d%-2.3lf%%%-2.3lf\n", pid[i], comm[i], isrunning[i], cpu[i], (double)sum_exec_runtime[i]/1000000000);
        }

        num1 = num2;
        memcpy(pid1, pid2, sizeof(int) * 400);
        memcpy(comm1, comm2, sizeof(char) * 400 * 16);
        memcpy(isrunning1, isrunning2, sizeof(int) * 400);
        memcpy(sum_exec_runtime1, sum_exec_runtime2, sizeof(unsigned long long/*u64*/) * 400);

    }
}