//find程序，写一个简化版本的unix的find程序
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"




/*
void find(char *path, char *file) {
	char buf[512], *p;
	int fd;
	struct dirent de;//目录结构体
	struct stat st;//文件结构体
	//首先是读取文件夹，再判断是什么类型的
	if((fd = open(path, O_RDONLY)) < 0){ //打开path目录文件，里边记录了子目录的直接目录信息和递归目录信息以及最终文件
		fprintf(2, "find: cannot open %s\n", path);//目录不存在
		return;
	}
	//目录存在
	if(fstat(fd, &st) < 0){ //应将是将目录的内部文件结构体读出到st中
		fprintf(2, "find: cannot stat %s\n", path);//该目录文件读不出相关信息
		close(fd);
		return;
	}
	switch(st.type){//判断该文件类型
		case T_DEVICE://设备类型，直接和T_FILE文件一块打印信息
		case T_FILE://文件类型，应该和目标file进行比较，看是否是要找的文件，使用strcmp
			//首先对path进行切分,取到最后一级名称
			for(p=path+strlen(path); p >= path && *p != '/'; p--){};p++;
			if (!strcmp(p, file)) {
				printf("%s",path);
			}
			break;

		case T_DIR://目录类型
			if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
				printf("ls: path too long\n");   //目录长度限制
				break;
			}
			strcpy(buf, path);//如果目录长度不长就复制到buf中
			p = buf+strlen(buf);//定位到buf中目录字符的末尾
			*p++ = '/';
			while(read(fd, &de, sizeof(de)) == sizeof(de)) {//从目录中读取文件信息
				if(de.inum == 0)//目录无效标志
        			continue;        
				memmove(p, de.name, DIRSIZ);//把名称添加到buf末尾
				p[DIRSIZ] = 0;//添加结束符
				if(stat(buf, &st) < 0){
					printf("ls: cannot stat %s\n", buf); 
					continue;
				}
				//如果再分析st的话就在递归了没有必要，st如果是目录就需要再进行递归
				if (st.type == T_FILE) {
					if (!strcmp(p, file)) {
						printf("%s\n",buf);
					}
				}	
			}
			break;
	}
}
*/
/*
从题目角度来讲，这个是要写成递归写法：
第一步：终止条件
第二步：顺序执行
*/

void find(char *path, char *file) {
	//先写顺序执行
	char buf[512], *p;
	int fd;
	struct dirent de;//目录结构体
	struct stat st;//文件结构体
	//首先是读取文件夹，再判断是什么类型的
	if((fd = open(path, O_RDONLY)) < 0){ //打开path目录文件，里边记录了子目录的直接目录信息和递归目录信息以及最终文件
		fprintf(2, "find: cannot open %s\n", path);//目录不存在
		return;
	}
	//目录存在
	if(fstat(fd, &st) < 0){ //应将是将目录的内部文件结构体读出到st中
		fprintf(2, "find: cannot stat %s\n", path);//该目录文件读不出相关信息
		close(fd);//这个fd非常非常重要的，不关这个内存后面撑不住的
		return;
	}
	switch(st.type){//判断该文件类型
		case T_DEVICE://设备类型，直接和T_FILE文件一块打印信息
		case T_FILE://文件类型，应该和目标file进行比较，看是否是要找的文件，使用strcmp
			//首先对path进行切分,取到最后一级名称
			for(p=path+strlen(path); p >= path && *p != '/'; p--){};p++;
			if (!strcmp(p, file)) {
				printf("%s\n",path);
			}
			break;
		case T_DIR://目录类型
			if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
				printf("ls: path too long\n");	 //目录长度限制
				break;
			}
			strcpy(buf, path);//如果目录长度不长就复制到buf中
			p = buf+strlen(buf);//定位到buf中目录字符的末尾
			*p++ = '/';
			while(read(fd, &de, sizeof(de)) == sizeof(de)) {//从目录中读取文件信息
				if(de.inum == 0)//目录无效标志
					continue;	
				if(!strcmp(de.name, ".") || !strcmp(de.name, "..")) continue;
				memmove(p, de.name, DIRSIZ);//把名称添加到buf末尾
				p[DIRSIZ] = 0;//添加结束符
				find(buf,file);
			}
			break;
	}
	close(fd);//这个fd一定要关呀，最后发现文件描述符会不够的。
	return;
}



int main(int argc, char **argv) {
	if (argc < 3) {
		fprintf(2, "Usage: find [dir] [file...]\n"); //find + 目录 + 文件名
		exit(1);
	}
	//先完成题目的简单demo
	find(argv[1],argv[2]);
	exit(0);
}

