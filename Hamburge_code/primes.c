//素数
/*
1.对于管道的理解更加深刻
2.学会了创建多个子进程的方法，父进程主要是用来控制中间变量的，实际的任务都是子进程来执行的
3.对于进程之间的执行逻辑有了更深的理解。

*/
#include "kernel/types.h"
#include "user/user.h"

#define RD 0  //管道的读端
#define WR 1  //管道的写端
#define dataNum 34
int main(int argc, char *argv[]) {
	if (argc > 1) {
		fprintf(2, "Usage: primes\n");
		exit(1);
	}
	//先把上一道题做收尾，每天两道题，不要超过，科研还要不要做了
	//2024.12.22,争取把lab1三道题搞定
	int buf; //总共34个数
	int p1[2], p2[2]; //管道1读写文件描述符
	int cprocessnum = 1;//记录进程
	//父进程创建两个管道
	pipe(p1);


	//第一个进程的行为：向第一个管道中传输2-35数值
	int pid1 = fork();
	if (pid1 < 0) {
		fprintf(2, "fork1 create error!\n");
		exit(1);//强制退出
	}
	else if (pid1 == 0) {
		close(p1[RD]);
		for (int i = 2; i < dataNum+2; i++) {
			if (write(p1[WR],(char*)&i,sizeof(int)) != sizeof(int)) {
				fprintf(2, "c1 write error\n");
				exit(1);//强制退出
			}//从pi管道写入数据	
		}
		close(p1[WR]);
		exit(0);
	}
	//对于下一个子进程，只需要前一个进程的读取端，不需要写入端，只要父进程关掉这个写入端就可以了
	//同时下一个进程还需要一个新管道
	close(p1[WR]);

	for (;;) {
		//缺少一个终止条件判断
		if (read(p1[RD], (char*)&buf, sizeof(int))== 0) break;
		cprocessnum++;
		printf("prime %d\n", buf);
		//父进程创建一个新管道
		pipe(p2);


		//创建一个新进程,新进程包含了新的管道的读写端，和上一条管道的读端
		int pid = fork();
		if (pid < 0) {
			fprintf(2, "c2 write error\n");
			exit(1);//强制退出
		}
		else if (pid == 0) {
			printf("currentPid: %d\n", getpid());
			//关闭新管道的读端
			close(p2[RD]);
			while(1){
				int onedata;
				int num = read(p1[RD], (char*)&onedata, sizeof(int));
				if (num < 0) {
					fprintf(2, "read error\n");
					exit(1);
				}
				if (num == 0) break;
				if (onedata % buf != 0) {
					if (write(p2[WR], (char*)&onedata, sizeof(int) ) != sizeof(int)) {
						fprintf(2, "write error\n");
						exit(1);//强制退出
					}
				}
			}
			close(p1[RD]);
			close(p2[WR]);
			exit(0);
		}

		close(p1[RD]);
		close(p2[WR]);
		p1[RD] = p2[RD];
	}
	for (int i = 0; i < cprocessnum; i++) {
		int endcpid = wait(0);
		printf("endprocesspid:%d\n", endcpid);
	}
	exit(0);
}
















/*
	//第一个子进程要输入2-35
	int pid1 = fork();
	if (pid1 < 0) {
		fprintf(2, "fork1 create error!\n");
		exit(1);//强制退出		
	}
	else if (pid1 == 0) {
		close(p1[RD]);
				
		for (int i = 0; i < dataNum; i++) {
			buf[i] = i + 2;
		}
		if (write(p1[WR],(char*)buf,sizeof(buf)) != sizeof(buf)) {
			fprintf(2, "c1 write error\n");
			exit(1);//强制退出
		}//从pi管道写入数据
		
		for (int i = 0; i < dataNum; i++) {
			buf = i + 2;
			if (write(p1[WR],(char*)&buf,sizeof(buf)) != sizeof(buf)) {
				fprintf(2, "c1 write error\n");
				exit(1);//强制退出
			}//从pi管道写入数据	
		}
		close(p1[WR]);
		exit(0);//正常退出
	}
	for (;;) {//循环先不用设定
		cprocessnum++;//记录子进程数量
		int pid = fork();
		if (pid < 0) {
			fprintf(2, "fork2 create error!\n");
			exit(1);//强制退出
		}
		else if (pid == 0) {
			//子进程
			printf("cp:%d\n", cprocessnum);
			int num = read(p1[RD], (char*)&buf, sizeof(buf));
			if (num < 0) {
				fprintf(2, "read error\n");
				exit(1);
			}
			printf("prime %d\n", buf);
			int tmp = buf;
			int lastdata = buf;
			while(1) {
				int num = read(p1[RD], (char*)&buf, sizeof(buf));
				if (num < 0) {
					fprintf(2, "read error\n");
					exit(1);
				}
				if (buf < lastdata) break;
				lastdata = buf;
				if (buf % tmp != 0) {
					if (write(p1[WR],(char*)&buf,sizeof(buf)) != sizeof(buf)) {
						fprintf(2, "write error\n");
						exit(1);//强制退出
					}//从pi管道写入数据	
				}
			}
			if (num == 0) break;
			close(p1[RD]);
			close(p1[WR]);
			exit(0);
		} 
	}
	for (int i = 0; i < cprocessnum; i++) {
		int endcpid = wait(0);
		printf("endprocesspid:%d\n", endcpid);
	}
	exit(0);
*/












/*	int pid1 = fork();
	if (pid1 == 0) {
		//子进程1
		fprintf(1, "i am pid:%d\n", getpid());
		exit(0);
	}
	else if (pid1 > 0) {
		//父进程
		int pid2 = fork();
		if (pid2 == 0) {
			//子进程2
			fprintf(1, "i am pid:%d\n", getpid());
			exit(0);
		}
		else if (pid2 > 0) {
			fprintf(1, "i am pid:%d\n", getpid());
			int endcpid1 = wait(0);
			int endcpid2 = wait(0);
			printf("endprocesspid1:%d, endprocesspid2:%d\n", endcpid1, endcpid2);
			exit(0);
		}
		else {
			fprintf(2, "fork create error!\n");
			exit(1);//强制退出
		}
	}
	else {
		fprintf(2, "fork create error!\n");
		exit(1);//强制退出
	}

}*/




/*	
	if (pid1 < 0) {
		fprintf(2, "fork create error!\n");
		exit(1);//强制退出
	}
	else if (pid1 == 0) {
		//子进程
		printf("inc1pid:%d\n",pid1);
		printf("c1_pid:%d\n", getpid());
		printf("c1_process: p1[0]:%d,p1[1]:%d,p2[0]:%d,p2[1]:%d\n",p1[0],p1[1],p2[0],p2[1]);
		close(p1[RD]);
		close(p1[WR]);
		close(p2[RD]);
		close(p2[WR]);
		exit(0);
	} 
	else {
		//父进程
		printf("outc1pid:%d\n",pid1);
		printf("p1_pid:%d\n", getpid());
	    printf("p_process: p1[0]:%d,p1[1]:%d,p2[0]:%d,p2[1]:%d\n",p1[0],p1[1],p2[0],p2[1]);
		close(p1[RD]);
		close(p1[WR]);
		close(p2[RD]);
		close(p2[WR]);
		wait(0);
		exit(0);
	}
*/


