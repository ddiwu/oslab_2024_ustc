#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int num1, num2;
    int pid1[55], pid2[55],pid[55];
    char comm1[55*16], comm2[55*16],comm[55*16];
    unsigned long long sum_exec_runtime1[55], sum_exec_runtime2[55], sum_exec_runtime[55];
    int isrunning1[55], isrunning2[55], isrunning[55];
    double cpu1[55],cpu2[55], cpu[55];
    int cnt;

    int t_p;
    char t_comm;
    unsigned long long t_runtime;
    int t_isrunning;
    double t_cpu;
    int temp = 0;

    int d;

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
            sleep(1);
            d = 1;
        }
        else 
        {
            sleep(atoi(argv[1]));
            d = atoi(argv[1]);
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
                cpu[cnt] = (sum_exec_runtime2[i] - sum_exec_runtime1[i]) / ((double)1000000 * d);//百分比
                sum_exec_runtime[cnt] = sum_exec_runtime2[i];
                cnt++;
                i++;
                j++;
            } else if (pid1[i] < pid2[j])
                i++;
            else if (pid1[i] > pid2[j])
                j++;
        }
        
        //将这些组数据按照cpu值从大到小排序
        for (int k = 0; k < cnt;  k++)
        {
            for (int m =k; m< cnt; m++)
            {
                if (cpu[m] > cpu[temp])
                {
                    temp = m;
                }
            }
            t_p = pid[k];
            pid[k] = pid[temp];
            pid[temp] = t_p;
            t_runtime = sum_exec_runtime[k];
            sum_exec_runtime[k] = sum_exec_runtime[temp];
            sum_exec_runtime[temp] = t_runtime;
            t_isrunning = isrunning[k];
            isrunning[k] = isrunning[temp];
            isrunning[temp] = t_isrunning;  
            t_cpu = cpu[k];
            cpu[k] = cpu[temp];
            cpu[temp] = t_cpu;
            for(int n = 0; n<16;n++)
            {
                t_comm = comm[16*k+n];
                comm[16*k+n] = comm[16*temp+n];
                comm[16*temp+n] = t_comm;
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
        // for(int k = 0; k < 20; k++)
        // {
        //     if(cpu[k] > 0.000)
        //     isrunning[k] = 1;
        //     else isrunning[k] = 0;
        // }
        
        system("clear");

        // for(int l = 0; l < cnt; l++)
        // {
        //     printf("%d",isrunning[l]);
        // }
        // printf("\n");

        printf("%-5s %-10s %-5s  %-5s  %s\n", "PID",  "ISRUNNING", "CPU","TIME","COMM");
        for (int p = 0; p < 20; p++) {
            printf("%-5d %-10d %-2.3lf%% %-2.3lf ", pid[p],  isrunning[p], cpu[p], sum_exec_runtime[p]/10000000000.0);
            for(int d = 0; d<16;d++)
            {
                //if(comm[16*i+k] != '\0')
                printf("%c",comm[16*p+d]);
            }
            printf("\n");
        }

        num1 = num2;
        memcpy(pid1, pid2, sizeof(int) * 55);
        memcpy(comm1, comm2, sizeof(char) * 55 * 16);
        memcpy(isrunning1, isrunning2, sizeof(int) * 55);
        memcpy(sum_exec_runtime1, sum_exec_runtime2, sizeof(unsigned long long/*u64*/) * 55);

    }
}