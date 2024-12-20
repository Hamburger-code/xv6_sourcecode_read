//lab1中的系统调用pingpong
/*
疑问1：子进程和父进程使用相同的变量名会不会出错，共用recv会不会出错？这个内存共用问题还是表示疑惑
疑问2：所有read等都要进行错误判断吗？
这份代码中根本不管发送了什么信息，只是简单的发送与接收正常，先不管内容公用方面的东西，后边回来再看。

下次待提高：所有的函数都要写报错处理
*/



#include "kernel/types.h"
#include "user/user.h"
int main (int argc, char *argv[]) {
	if (argc > 1) {
		fprintf(2, "Usage: pingpong \n");
		exit(1);//异常退出
	}	
	char recv[25];
	int p1[2],p2[2];//分别对应两个管道，下标0对应的文件描述符为可读的，下标为1是可写的
	pipe(p1);//父进程发送给子进程管道1
	pipe(p2);//子进程发送给父进程管道2
	int *status_c = 0; //C语言指针赋值为0表示空指针


	int pid = fork();//创建子进程，从代码中就可以简单理解，这句代码的上方是子进程与父进程公用的内存，下方用fork的返回值将父进程与子进程隔开


	if (pid == 0) {
		//这里是子进程
		close(p1[1]);//关闭1号管道的写
		int num = read(p1[0],recv,sizeof(recv));
		if (num <= 0) {
			fprintf(2, "p->c read error\n");
			exit(1);
		}
		close(p1[0]);
		recv[num] = '\0';
		int cur_pid = getpid();
		printf("%d: received ping and message is %s\n", cur_pid, recv);
		close(p2[0]);
		if (write(p2[1], "C->P:father", sizeof("C->P:father")) != sizeof("C->P:father")) {
			fprintf(2, "c->p write error\n");
			exit(1);
		}
		close(p2[1]);
		exit(0);
	}
	else if (pid > 0) {
		//这里是父进程
		close(p1[0]);//关闭1号管道的读
		if (write(p1[1], "P->C:son", sizeof("P->C:son")) != sizeof("P->C:son")) {
			fprintf(2, "p->c write error\n");
			exit(1);
		}
		close(p1[1]);
		close(p2[1]);//关闭2号管道的写
		int num = read(p2[0], recv, sizeof(recv));
		if (num <= 0) {
			fprintf(2, "c->p read error\n");
			exit(1);
		}
		close(p2[0]);
		recv[num] = '\0';
		int cur_pid = getpid();
		printf("%d: received pong and message is %s\n", cur_pid,recv);
		//等待子进程结束
		int endcpid = wait(status_c);//status_c返回的是一个状态码,子进程使用exit(num),如果顺利,那么，返回的就是num
		if (endcpid < 0) {
			fprintf(2, "wait error\n");
			exit(1);
		}
		printf("end_subprocess_pid: %d, status: 0x%x\n", endcpid, *status_c);
		exit(0);
	}
	else {
		//这里是报错处理
		fprintf(2,"fork_fault \n");
		exit(1);
	}
	exit(0);//正常退出,这句命令可有可无
}

/*
int main (int argc, char *argv[]) {
	if (argc > 1) {
		fprintf(2, "Usage: pingpong \n");
		exit(1);//异常退出
	}	
	int exist_status = 0;
	char buf = 'D';
	int p1[2],p2[2];//分别对应两个管道，下标0对应的文件描述符为可读的，下标为1是可写的
	pipe(p1);//父进程发送给子进程管道1
	pipe(p2);//子进程发送给父进程管道2
	int pid = fork();//创建子进程
	if (pid == 0) {
		//这里是子进程
		close(p1[1]);//关闭1号管道的写
		close(p2[0]);
		if (read(p1[0], &buf, sizeof(char))!= sizeof(char)) {
			fprintf(2,"p->c read error \n");
			exist_status = 1;
		}
		else {
			fprintf(1, "%d: received ping\n", getpid());
		}
		if (write(p2[1], &buf, sizeof(char)) != sizeof(char)) {
			fprintf(2, "c->p write error \n");
			exist_status = 1;
		}
		close(p1[0]);
		close(p2[1]);
		exit(exist_status);
	}
	else if (pid > 0) {
		//这里是父进程
		close(p1[0]);//关闭1号管道的读
		close(p2[1]);//关闭2号管道的写
		if (write(p1[1], &buf, sizeof(char)) != sizeof(char)) {
			fprintf(2,"p->c write error! \n");
			exist_status = 1;
		}
		if (read(p2[0], &buf, sizeof(char)) != sizeof(char)){
			fprintf(2, "c->p read error \n");
			exist_status = 1;
		}
		fprintf(1, "%d: received pong\n", getpid());
		close(p2[0]);
		close(p1[1]);
		exit(exist_status);
	}
	else {
		//这里是报错处理
		fprintf(2,"fork_fault \n");
		close(p1[0]);
		close(p2[1]);
		close(p1[1]);
		close(p2[0]);
		exit(1);
	}
	//exit(0);//正常退出
}
*/
/*
int main (int argc, char *argv[]) {
	if (argc > 1) {
		fprintf(2, "Usage: pingpong \n");
		exit(1);//异常退出
	}	
	char recv[10];
	int p1[2],p2[2];//分别对应两个管道，下标0对应的文件描述符为可读的，下标为1是可写的
	pipe(p1);//父进程发送给子进程管道1
	pipe(p2);//子进程发送给父进程管道2
	int pid = fork();//创建子进程
	if (pid == 0) {
		//这里是子进程
		close(p1[1]);//关闭1号管道的写
		int num = read(p1[0],recv,sizeof(recv));
		close(p1[0]);
		recv[num] = '\0';
		int cur_pid = getpid();
		printf("%d: received ping \n", cur_pid);
		close(p2[0]);
		write(p2[1], "1", 1);
		close(p2[1]);
		exit(0);
	}
	else if (pid > 0) {
		//这里是父进程
		close(p1[0]);//关闭1号管道的读
		write(p1[1], "1", 1);//写入一个字节
		close(p1[1]);
		close(p2[1]);//关闭2号管道的写
		int num = read(p2[0], recv, sizeof(recv));
		close(p2[0]);
		recv[num] = '\0';
		int cur_pid = getpid();
		printf("%d: received pong \n", cur_pid);
		wait(0);
		exit(0);
	}
	else {
		//这里是报错处理
		fprintf(2,"fork_fault \n");
		close(p1[0]);
		close(p1[1]);
		close(p2[0]);
		close(p2[1]);
		exit(1);
	}
	exit(0);//正常退出
}
*/
/*
#include "kernel/types.h"
#include "user/user.h"
	
#define RD 0 //pipe的read端
#define WR 1 //pipe的write端
	
	int main(int argc, char const *argv[]) {
		char buf = 'P'; //用于传送的字节
	
		int fd_c2p[2]; //子进程->父进程
		int fd_p2c[2]; //父进程->子进程
		pipe(fd_c2p);
		pipe(fd_p2c);
	
		int pid = fork();
		int exit_status = 0;
	
		if (pid < 0) {
			fprintf(2, "fork() error!\n");
			close(fd_c2p[RD]);
			close(fd_c2p[WR]);
			close(fd_p2c[RD]);
			close(fd_p2c[WR]);
			exit(1);
		} else if (pid == 0) { //子进程
			close(fd_p2c[WR]);
			close(fd_c2p[RD]);
	
			if (read(fd_p2c[RD], &buf, sizeof(char)) != sizeof(char)) {
				fprintf(2, "child read() error!\n");
				exit_status = 1; //标记出错
			} else {
				fprintf(1, "%d: received ping\n", getpid());
			}
	
			if (write(fd_c2p[WR], &buf, sizeof(char)) != sizeof(char)) {
				fprintf(2, "child write() error!\n");
				exit_status = 1;
			}
	
			close(fd_p2c[RD]);
			close(fd_c2p[WR]);
	
			exit(exit_status);
		} else { //父进程
			close(fd_p2c[RD]);
			close(fd_c2p[WR]);
	
			if (write(fd_p2c[WR], &buf, sizeof(char)) != sizeof(char)) {
				fprintf(2, "parent write() error!\n");
				exit_status = 1;
			}
	
			if (read(fd_c2p[RD], &buf, sizeof(char)) != sizeof(char)) {
				fprintf(2, "parent read() error!\n");
				exit_status = 1; //标记出错
			} else {
				fprintf(1, "%d: received pong\n", getpid());
			}
	
			close(fd_p2c[WR]);
			close(fd_c2p[RD]);
	
			exit(exit_status);
		}
	}
*/

