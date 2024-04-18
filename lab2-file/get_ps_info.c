#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(void)
{
    int num1, num2;
    int pid1[50], pid2[50],pid[50];
    char comm1[50*16], comm2[50*16],comm[50*16];
    int sum_exec_runtime1[50], sum_exec_runtime2[50], sum_exec_runtime[50];
    int isrunning1[50], isrunning2[50], isrunning[50];
    double cpu1[50],cpu2[50], cpu[50];
    int cnt;

    int t_p;
    char t_comm;
    unsigned long long t_runtime;
    int t_isrunning;
    double t_cpu;
    int temp = 0;
    

    syscall(322, &num1, pid1, comm1, isrunning, sum_exec_runtime1);
    while(1)
    {

        printf("num = %d",num1);
        sleep(1);
        syscall(322, &num2, pid2, comm2, isrunning, sum_exec_runtime2);
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
        
        system("clear");
        printf("%-5s%-16s%-10s%-5s%-8s\n", "PID", "COMM", "ISRUNNING", "CPU","TIME");
        for (int i = 0; i < 20; i++) {
            printf("%-5d %c %-10d %-2.3lf%% %-2.3lf\n", pid[i], comm[16*i], isrunning[i], cpu[i], sum_exec_runtime[i]/(double)1000000000);
        }

        num1 = num2;
        memcpy(pid1, pid2, sizeof(int) * 50);
        memcpy(comm1, comm2, sizeof(char) * 50 * 16);
        memcpy(isrunning1, isrunning2, sizeof(int) * 50);
        memcpy(sum_exec_runtime1, sum_exec_runtime2, sizeof(unsigned long long/*u64*/) * 50);

    }
}