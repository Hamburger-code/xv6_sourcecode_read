//lab1中的系统调用sleep
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	if(argc < 2){
		fprintf(2, "Usage: sleep [count]\n");
		exit(1);//异常退出
	}
	sleep(atoi(argv[1]));
	exit(0);
}

