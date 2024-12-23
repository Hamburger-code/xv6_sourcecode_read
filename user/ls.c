#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];//C语言默认初始化为'\0'
  char *p;

  // Find first character after last slash.//寻找最后一个斜杠后的第一个字符
  for(p=path+strlen(path); p >= path && *p != '/'; p--) //strlen求字符串的长度，不包含末尾的"\0"字符
    ;
  p++;//得到最后一级目录文件的名称

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)//如果文件名称过长，直接返回
    return p;
  memmove(buf, p, strlen(p));//文件名不长就直接搬去buf中
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));//将buf尾部区域变为空格字符，保证这个名称长度为DIRSIZ个字符
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;//目录结构体
  struct stat st;//文件结构体

  if((fd = open(path, O_RDONLY)) < 0){ //打开path目录文件，里边记录了子目录的直接目录信息和递归目录信息以及最终文件
    fprintf(2, "ls: cannot open %s\n", path);//目录不存在
    return;
  }

  if(fstat(fd, &st) < 0){ //应将是将目录的内部文件结构体读出到st中
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }
  //到这里就读到了目录文件的信息

  switch(st.type){//判断该文件类型
  case T_DEVICE://设备类型
  case T_FILE://文件类型
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (int) st.size);//打印文件名称，文件类型，文件节点编号，文件尺寸大小（转化成int类型变量）
    break;

  case T_DIR://目录类型
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");   //目录长度限制
      break;
    }
    strcpy(buf, path);//如果目录长度不长就复制到buf中
    p = buf+strlen(buf);//定位到buf中目录字符的末尾
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){ //继续从文件中读出目录信息
      if(de.inum == 0)//空目录标志
        continue;
      memmove(p, de.name, DIRSIZ);//把名称添加到buf末尾
      p[DIRSIZ] = 0;//添加结束符
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf); 
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (int) st.size);//读取成功则打印该目录信息
    }
    break;
  }
  close(fd);
}

/*
ls函数解析
对于传入的path参数：
path有三种可能：分别是设备文件，普通文件，目录
1.读取path类型
2.如果是设备文件就不管，普通文件直接打印出对应信息，目录的话开始从目录文件中读取目录下一级的文件和目录并打印这些信息（包括下一级的目录信息和文件信息）
3.通过阅读代码，学到了学懂了很多操作。
*/




int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
