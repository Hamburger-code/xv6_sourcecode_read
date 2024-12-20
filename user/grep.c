// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[1024];
int match(char*, char*);

void
grep(char *pattern, int fd)
{
  int n, m;
  char *p, *q;

  m = 0;
  while((n = read(fd, buf+m, sizeof(buf)-m-1)) > 0){//read返回值为读取的字节数，这句代码先从文件中读出1024个字节的内容
    m += n;
    buf[m] = '\0';//在读取的字符串末尾加结束符
    p = buf;
    while((q = strchr(p, '\n')) != 0){//检查是否到字符串末尾
      *q = 0;
      if(match(pattern, p)){  //pattern 与字符串进行匹配寻找
        *q = '\n';
        write(1, p, q+1 - p);//如果成功找到，就将这部分内容打印下来，fd = 1为标准输出窗口
      }
      p = q+1;
    }
    if(m > 0){
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;
  char *pattern;

  if(argc <= 1){//参数输入不够
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);//exit(1)异常退出
  }
  pattern = argv[1];

  if(argc <= 2){
    grep(pattern, 0);
    exit(0);//exit(0)正常退出
  }

  for(i = 2; i < argc; i++){
    if((fd = open(argv[i], O_RDONLY)) < 0){
      printf("grep: cannot open %s\n", argv[i]);
      exit(1);//异常退出
    }
    grep(pattern, fd);
    close(fd);//关闭文件
  }
  exit(0);//正常退出
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{  // must look at empty string
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}

