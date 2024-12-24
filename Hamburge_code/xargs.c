//xargs命令
//这道题耗费的时间有点久，半天没读懂题目的意思
//输入参数为xargs + command,然后从标准输入中读取每一行代码去执行命令

#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"//这里可以用到一个MAXARG的参数
int main(int argc, char **argv) {
	if (argc <= 1) {//最起码要两个参数
		fprintf(2, "Usage: xargs [command]\n"); //find + 目录 + 文件名
		exit(1);
	}
	if (argc > MAXARG) {
		fprintf(2, "too many arg !\n");
		exit(1);
	}
	char buf[512], *p;//默认全部初始化为0
	p = buf;
	//首先保存传入的命令,这个感觉不是很有必要，先放在这里
	char *xargs[MAXARG] = {0};//xargs中第一个是命令标识,整体是exec执行参数,=0是全部指向空指针
	for (int i = 1; i < argc; i++) {
		xargs[i - 1] = argv[i];
		printf("%s ",xargs[i-1]);
	}
	printf("\n");
	//然后要读取标准输入行的字符，1次读取1个字符，标准输入的句柄fd = 0
	while (1) {
		int num = read(0,p,sizeof(char));
		if (num == sizeof(char)) {
			//如果读取顺利，判断是否是终止符
			if (*p == '\n') {
				*p = '\0';
				//printf("databuf:%s\n",buf);
				/*目的地址必须有足够内存，不然会出现段错误
				memmove(xargs[argc - 1], buf, p - buf + 1);
				strcpy(xargs[argc - 1], buf);
				*/
				xargs[argc-1] = buf;
				p = buf;
				int pid  = fork();
				if (pid < 0) {
					fprintf(2, "fork create error\n");
					exit(1);
				}
				else if (pid == 0) {
					//进入子进程
					printf("newprocesspid:%d\n",getpid());
				//	printf("dataxargs:%s\n",xargs);
					exec(xargs[0], xargs);
					exit(0);
				}
				int enprocesspid = wait(0);//wait(0)会让主进程一直阻塞的
				printf("endprocess:%d\n", enprocesspid);
				
			}
			else {
				p++;
				if (p - buf >= 512) {
					fprintf(2, "too long arg!\n");
					exit(1);
				}
			}
		}
		else if (num == 0) break;
		else {
			fprintf(2,"read error");
			exit(1);
		}	
	}
	exit(0);	
}


