#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int num1, num2;
    int pid1[200], pid2[200],pid[200];
    char comm1[200*16], comm2[200*16],comm[200*16];
    unsigned long long sum_exec_runtime1[200], sum_exec_runtime2[200], sum_exec_runtime[200];
    int isrunning1[200], isrunning2[200], isrunning[200];
    double cpu1[200],cpu2[200], cpu[200];
    int cnt;

    int t_p;
    char t_comm;
    unsigned long long t_runtime;
    char t_comm;
    unsigned long long t_runtime;
    int t_isrunning;
    double t_cpu;
    int temp = 0;

    //char com[20][16];
    

    if(syscall(332, &num1, pid1, comm1, isrunning, sum_exec_runtime1) < 0)
    {
        printf("syscall error\n");
        return -1;
    }
    while(1)
    {
        if(argc == 1)
        {
    
        printf("num = %d",num1);
        sleep(1);
        }
        else 
        {
            sleep(atoi(argv[1]));
        }
        
        syscall(332, &num2, pid2, comm2, isrunning, sum_exec_runtime2);
        cnt = 0;
        int i = 0, j = 0;
        while (i < num1 && j < num2) {
            if (pid2[i] == pid1[j]) {
                pid[cnt] = pid2[i];
                for(int k = 0; k< 16; k++)
                {
                    comm[16*cnt+k] = comm2[16*i+k];
                }
                isrunning[cnt] = isrunning2[i];
                cpu[cnt] = (sum_exec_runtime2[i] - sum_exec_runtime1[i]) / (double)10000000;//百分比
                cnt++;
                i++;
                j++;
            } else if (pid1[i] < pid2[j])
                i++;
            else if (pid1[i] > pid2[j])
                j++;
        int i = 0, j = 0;
        while (i < num1 && j < num2) {
            if (pid2[i] == pid1[j]) {
                pid[cnt] = pid2[i];
                for(int k = 0; k< 16; k++)
                {
                    comm[16*cnt+k] = comm2[16*i+k];
                }
                isrunning[cnt] = isrunning2[i];
                cpu[cnt] = (sum_exec_runtime2[i] - sum_exec_runtime1[i]) / (double)10000000;//百分比
                cnt++;
                i++;
                j++;
            } else if (pid1[i] < pid2[j])
                i++;
            else if (pid1[i] > pid2[j])
                j++;
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
            for(int k = 0; k<16;k++)
            {
                t_comm = comm[16*i+k];
                comm[16*i+k] = comm[16*temp+k];
                comm[16*temp+k] = t_comm;
            }
        }
        //把长串划分为小串
        // for (int i = 0; i < 20; i++)
        // {
        //     for (int j = 0; j < 16; j++)
        //     {
        //         com[i][j] = comm[16*i+j];
        //     }
        // }
        for(int k = 0; k < 20; k++)
        {
            if(isrunning[k] == 0)
            isrunning[k] = 1;
            else isrunning[k] = 0;
        }
        
        system("clear");

        printf("%-5s %-10s %-5s  %-4s  %s\n", "PID",  "ISRUNNING", "CPU","TIME","COMM");
        for (int i = 0; i < 20; i++) {
            printf("%-5d %-10d %-2.3lf%% %-1.3lf ", pid[i],  isrunning[i], cpu[i], (sum_exec_runtime2[i] - sum_exec_runtime1[i])/(double)1000000000);
            for(int k = 0; k<16;k++)
            {
                //if(comm[16*i+k] != '\0')
                printf("%c",comm[16*i+k]);
            }
            printf("\n");
        }

        num1 = num2;
        memcpy(pid1, pid2, sizeof(int) * 200);
        memcpy(comm1, comm2, sizeof(char) * 200 * 16);
        memcpy(isrunning1, isrunning2, sizeof(int) * 200);
        memcpy(sum_exec_runtime1, sum_exec_runtime2, sizeof(unsigned long long/*u64*/) * 200);

    }
}