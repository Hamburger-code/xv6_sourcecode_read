#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
wc(int fd, char *name)
{
  int i, n;
  int l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  while((n = read(fd, buf, sizeof(buf))) > 0){
    for(i=0; i<n; i++){
      c++;//c记录总共有多少个字符，统计字符数
      if(buf[i] == '\n')
        l++;//l记录总共有几个换行符，统计行数
      if(strchr(" \r\t\n\v", buf[i]))
        inword = 0;//表示此时不是单词部分
      else if(!inword){
        w++; //w是非特殊字符（空格，回车，制表符，换行符，垂直制表符）,w统计的是单词word的数量
        inword = 1;//表示是单词部分
      }
    }
  }
  if(n < 0){
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    wc(0, "");//直接统计字符数量
    exit(0);
  }

  for(i = 1; i < argc; i++){
    if((fd = open(argv[i], O_RDONLY)) < 0){
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);//统计字符数量
    close(fd);
  }
  exit(0);
}
