#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define MAX_CMDLINE_LENGTH  1024    /* max cmdline length in a line*/
#define MAX_BUF_SIZE        4096    /* max buffer size */
#define MAX_CMD_ARG_NUM     32      /* max number of single command args */
#define WRITE_END 1     // pipe write end
#define READ_END 0      // pipe read end

/* 
 * 需要大家完成的代码已经用注释`DO:`标记
 * 可以编辑器搜索找到
 * 使用支持TODO高亮编辑器（如vscode装TODO highlight插件）的同学可以轻松找到要添加内容的地方。
 */

/*  
    int split_string(char* string, char *sep, char** string_clips);

    基于分隔符sep对于string做分割，并去掉头尾的空格

    arguments:      char* string, 输入, 待分割的字符串 
                    char* sep, 输入, 分割符
                    char** string_clips, 输出, 分割好的字符串数组

    return:   分割的段数 
*/

int split_string(char* string, char *sep, char** string_clips) {
    
    char string_dup[MAX_BUF_SIZE];
    string_clips[0] = strtok(string, sep);
    int clip_num=0;
    
    do {
        char *head, *tail;
        head = string_clips[clip_num];
        tail = head + strlen(string_clips[clip_num]) - 1;
        while(*head == ' ' && head != tail)
            head ++;
        while(*tail == ' ' && tail != head)
            tail --;
        *(tail + 1) = '\0';
        string_clips[clip_num] = head;
        clip_num ++;
    }while(string_clips[clip_num]=strtok(NULL, sep));
    return clip_num;
}

/*
    执行内置命令
    arguments:
        argc: 输入，命令的参数个数
        argv: 输入，依次代表每个参数，注意第一个参数就是要执行的命令，
        若执行"ls a b c"命令，则argc=4, argv={"ls", "a", "b", "c"}
        fd: 输出，命令输入和输出的文件描述符 (Deprecated)
    return:
        int, 若执行成功返回0，否则返回值非零
*/
int exec_builtin(int argc, char**argv, int *fd) {
    if(argc == 0) {
        return 0;
    }
    /* DO: 添加和实现内置指令 */

    if (strcmp(argv[0], "cd") == 0) {
        chdir(argv[1]);//change 文件夹
        return 0;
        
    } else if (strcmp(argv[0], "exit") == 0){
        exit(0);
        return 0;
       
    } else if (strcmp(argv[0], "kill") == 0 && argc == 2){
        kill(atoi(argv[1]), SIGTERM);
        return 0;

    } else if (strcmp(argv[0], "kill") == 0){
        kill(atoi(argv[1]), atoi(argv[2]));
        return 0;

    } else {
        // 不是内置指令时
        return -1;
    }
}

/*
    从argv中删除重定向符和随后的参数，并打开对应的文件，将文件描述符放在fd数组中。
    运行后，fd[0]读端的文件描述符，fd[1]是写端的文件描述符
    arguments:
        argc: 输入，命令的参数个数
        argv: 输入，依次代表每个参数，注意第一个参数就是要执行的命令，
        若执行"ls a b c"命令，则argc=4, argv={"ls", "a", "b", "c"}
        fd: 输出，命令输入和输出使用的文件描述符
    return:
        int, 返回处理过重定向后命令的参数个数
*/

int process_redirect(int argc, char** argv, int *fd) {
    /* 默认输入输出到命令行，即输入STDIN_FILENO，输出STDOUT_FILENO */
    fd[READ_END] = STDIN_FILENO;
    fd[WRITE_END] = STDOUT_FILENO;
    int i = 0, j = 0;
    while(i < argc) {
        int tfd;
        if(strcmp(argv[i], ">") == 0) {
            //DO: 打开输出文件从头写入
            tfd = open(argv[i+1], O_CREAT | O_WRONLY | O_TRUNC, 775);//没有创建，有只写，有内容清空
            if(tfd < 0) {
                printf("open '%s' error: %s\n", argv[i+1], strerror(errno));
            } else {
                //DO: 输出重定向
                fd[WRITE_END] = tfd;//写到tfd
            }
            i += 2;
        } else if(strcmp(argv[i], ">>") == 0) {
            //DO: 打开输出文件追加写入
            tfd = open(argv[i+1], O_CREAT | O_WRONLY | O_ACCMODE, 775);
            if(tfd < 0) {
                printf("open '%s' error: %s\n", argv[i+1], strerror(errno));
            } else {
                //DO:输出重定向
                fd[WRITE_END] = tfd;
            }
            i += 2;
        } else if(strcmp(argv[i], "<") == 0) {
            //DO: 读输入文件
            tfd = open(argv[i+1], O_RDONLY);
            if(tfd < 0) {
                printf("open '%s' error: %s\n", argv[i+1], strerror(errno));
            } else {
                //DO:输出重定向
                fd[READ_END] = tfd;
            }
            i += 2;
        } else {
            argv[j++] = argv[i++];
        }
    }
    argv[j] = NULL;
    return j;   // 新的argc
}



/*
    在本进程中执行，且执行完毕后结束进程。
    arguments:
        argc: 命令的参数个数
        argv: 依次代表每个参数，注意第一个参数就是要执行的命令，
        若执行"ls a b c"命令，则argc=4, argv={"ls", "a", "b", "c"}
    return:
        int, 若执行成功则不会返回（进程直接结束），否则返回非零
*/
int execute(int argc, char** argv) {
    int fd[2];
    // 默认输入输出到命令行，即输入STDIN_FILENO，输出STDOUT_FILENO 
    fd[READ_END] = STDIN_FILENO;
    fd[WRITE_END] = STDOUT_FILENO;
    // 处理重定向符，如果不做本部分内容，请注释掉process_redirect的调用
    argc = process_redirect(argc, argv, fd);
    if(exec_builtin(argc, argv, fd) == 0) {
        exit(0);
    }
    // 将标准输入输出STDIN_FILENO和STDOUT_FILENO修改为fd对应的文件
    dup2(fd[READ_END], STDIN_FILENO);
    dup2(fd[WRITE_END], STDOUT_FILENO);
    /* DO:运行命令与结束 */
    if(execvp(argv[0], argv) == -1) {//v means argv and p means path
        return -1;
    }
    exit(0);
}

int main() {
    /* 输入的命令行 */
    char cmdline[MAX_CMDLINE_LENGTH];

    char *commands[128];
    char *many_commands[128];
    
    char path[256];

    int many_cmd_count;
    int cmd_count;
    int i;
    while (1) {
        /* DO: 增加打印当前目录，格式类似"shell:/home/oslab ->"，你需要改下面的printf */
        getcwd(path, 256);//256为path空间BYTE数
        printf("shell: %s ->",path);
        fflush(stdout);

        fgets(cmdline, 256, stdin);
        strtok(cmdline, "\n");

        /*  基于";"的多命令执行，请自行选择位置添加 */
        many_cmd_count = split_string(cmdline, ";", many_commands); 
        
        /* 由管道操作符'|'分割的命令行各个部分，每个部分是一条命令 -> 多条命令*/
        /* 拆解命令行 */
        for(i = 0; i < many_cmd_count; i++) 
        {    
        cmd_count = split_string(many_commands[i], "|", commands);

        if(cmd_count == 0) {
            continue;
        } else if(cmd_count == 1) {     // 没有管道的单一命令
            char *argv[MAX_CMD_ARG_NUM];
            int argc;
            int fd[2];
            // 处理参数，分出命令名和参数
            argc = split_string(commands[0], " ", argv);
            /* 在没有管道时，内建命令直接在主进程中完成，外部命令通过创建子进程完成 */
            if(exec_builtin(argc, argv, fd) == 0) {
                continue;
            }
            // 创建子进程，运行命令，等待命令运行结束
            else {
                int pid = fork();
                if(pid == 0)//子进程才执行
                {
                    execute(argc,argv);
                }
            }
            wait(NULL);
        } 
        else if(cmd_count == 2) {     // 两个命令间的管道
            int pipefd[2];
            int ret = pipe(pipefd);
            if(ret < 0) {
                printf("pipe error!\n");
                continue;
            }
            // 子进程1
            int pid = fork();
            if(pid == 0) {  
                /*子进程1 将标准输出重定向到管道，注意这里数组的下标被挖空了要补全*/
                close(pipefd[READ_END]);
                dup2(pipefd[WRITE_END], STDOUT_FILENO);
                close(pipefd[WRITE_END]);
                /* 
                    在使用管道时，为了可以并发运行，所以内建命令也在子进程中运行
                    因此我们用了一个封装好的execute函数
                 */
                char *argv[MAX_CMD_ARG_NUM];

                int argc = split_string(commands[0], " ", argv);
                execute(argc, argv);
                exit(255);
                
            }
            // 因为在shell的设计中，管道是并发执行的，所以我们不在每个子进程结束后才运行下一个
            // 而是直接创建下一个子进程
            // 子进程2
            pid = fork();
            if(pid == 0) {  
                /* 子进程2 将标准输入重定向到管道，注意这里数组的下标被挖空了要补全 */
                close(pipefd[WRITE_END]);
                dup2(pipefd[READ_END], STDIN_FILENO);
                close(pipefd[READ_END]);

                char *argv[MAX_CMD_ARG_NUM];
                /* 处理参数，分出命令名和参数，并使用execute运行
                 * 在使用管道时，为了可以并发运行，所以内建命令也在子进程中运行
                 * 因此我们用了一个封装好的execute函数
                 */
                int argc = split_string(commands[1], " ", argv);
                execute(argc, argv);
                exit(255);
            }
            close(pipefd[WRITE_END]);
            close(pipefd[READ_END]);
            
            
            while (wait(NULL) > 0);
        } 
        else {    // 选做：三个以上的命令
            int read_fd;    // 上一个管道的读端口（出口）
            for(int i=0; i<cmd_count; i++) {
                int pipefd[2];
                // 创建管道，n条命令只需要n-1个管道，所以有一次循环中是不用创建管道的
                if(i != cmd_count -1)
                {
                    int ret = pipe(pipefd);
                    if(ret < 0) {
                    printf("pipe error!\n");
                    continue;
                    }
                }

                int pid = fork();
                if(pid == 0) {
                    // 除了最后一条命令外，都将标准输出重定向到当前管道入口
                    if(i != cmd_count - 1)
                    {
                        close(pipefd[READ_END]);
                        dup2(pipefd[WRITE_END], STDOUT_FILENO);
                        close(pipefd[WRITE_END]);
                    }

                    // DO:除了第一条命令外，都将标准输入重定向到上一个管道出口
                    if(i != 0) {
                        dup2(read_fd, STDIN_FILENO);
                        close(read_fd);
                    }

                    /* DO:处理参数，分出命令名和参数，并使用execute运行
                     * 在使用管道时，为了可以并发运行，所以内建命令也在子进程中运行
                     * 因此我们用了一个封装好的execute函数
                     */

                    char *argv[MAX_CMD_ARG_NUM];
                    int argc = split_string(commands[i], " ", argv);
                    execute(argc, argv);
                    exit(255);
                }
                /* 父进程除了第一条命令，都需要关闭当前命令用完的上一个管道读端口 
                 * 父进程除了最后一条命令，都需要保存当前命令的管道读端口 
                 * 记得关闭父进程没用的管道写端口
                 */
                if(i != 0) {
                    close(read_fd);
                }

                if(i != cmd_count - 1) {//父进程进行连接的操作
                    read_fd = pipefd[READ_END];
                }

                close(pipefd[WRITE_END]);

                // if(i == cmd_count - 1) {
                //     close(pipefd[READ_END]);
                // }

                // 因为在shell的设计中，管道是并发执行的，所以我们不在每个子进程结束后才运行下一个
                // 而是直接创建下一个子进程
            }
            // 等待所有子进程结束
            while (wait(NULL) > 0);
        }
        }
    }
}
