//usage: ./tail target dest.
//tail target文件并将结果保存至desk

#include <stdio.h>
#include <string.h>
#include <ntsid.h>           //linux下没有ntsid.h，删除后编译
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

FILE * file;

// 守护进程初始化函数
void init_daemon()
{
    pid_t pid;
    int i = 0;

    if ((pid = fork()) == -1) {
        printf("Fork error !\n");
        exit(1);
    }
    if (pid != 0) {
        exit(0);        // 父进程退出
    }

    setsid();           // 子进程开启新会话，并成为会话首进程和组长进程
    if ((pid = fork()) == -1) {
        printf("Fork error !\n");
        exit(-1);
    }
    if (pid != 0) {
        exit(0);        // 结束第一子进程，第二子进程不再是会话首进程
    }
    chdir("/tmp");      // 改变工作目录
    umask(0);           // 重设文件掩码
    for (; i < getdtablesize(); ++i) {
        close(i);        // 关闭打开的文件描述符
    }

    return;
}


void executeCMD(const char *cmd, char* result)
{
    char buf_ps[1024];
    char ps[1024]={0};
    FILE *ptr;
    strcpy(ps, cmd);
    file = fopen(result,"a");

    if((ptr=popen(ps, "r"))!=NULL)              //执行系统命令并获取结果
    {
        while(fgets(buf_ps, 1024, ptr)!=NULL)
        {

            fwrite(buf_ps,1, strlen(buf_ps),file);
            fflush(file);
        }
        pclose(ptr);
        ptr = NULL;
    }
    else
    {
        printf("popen %s error\n", ps);
    }
}

void main(int argc, char* argv[]){
    init_daemon();

    char cmd[256] = "tail -f ";
    char * dest = strcat(cmd,argv[1]);
    executeCMD(dest, argv[2]);

}
