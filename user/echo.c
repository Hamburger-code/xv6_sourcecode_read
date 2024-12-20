#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;

  for(i = 1; i < argc; i++){
    write(1, argv[i], strlen(argv[i]));//向fd = 1的文件写入fd = 1应该是标准输出文件
    if(i + 1 < argc){ 
      write(1, " ", 1); //在界面上打一个空格，在界面上打一个换行
    } else {
      write(1, "\n", 1);//在界面上打一个换行
    }
  }
  exit(0);//exit系统调用导致进程停止执行并释放资源
}
