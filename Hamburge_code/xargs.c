//xargs命令
//[命令1] [参数] | xargs [命令2]
//含义是：对‘|’左端的结果，执行命令2，暂时这么理解
#include "user/user.h"
#include "user/types.h"
#include "kernel/param.h"//这里可以用到一个MAXARG的参数
int main(int argc, char **argv) {
	if (argc > MAXARG) {
		fprintf(2, "Usage: [command1] | xargs [command2]\n"); //find + 目录 + 文件名
		exit(1);
	}
	//解析单行的命令
	
}


