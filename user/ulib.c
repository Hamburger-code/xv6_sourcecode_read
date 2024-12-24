#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//
void
start()
{
  extern int main();
  main();
  exit(0);
}

char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)//当s被赋值为‘\0’时则退出。
    ;
  return os;
}

int
strcmp(const char *p, const char *q)//比较两字符串的差异
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;//如果完全相同直接返回0，不同则返回相对大小
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  char *cdst = (char *) dst;
  int i;
  for(i = 0; i < n; i++){
    cdst[i] = c;
  }
  return dst;
}

char*
strchr(const char *s, char c)//检查字符c有没有在字符串s中。有则返回字符串中指向该字符的位置
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);//fd = 0 ,1 ,2 标准输入，标准输出，标准错误输出
    if(cc < 1)  //cc的返回值<1应该是读取错误，即读取字节数小于1
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r') //换行，回车
      break;
  }
  buf[i] = '\0';//最后一个字符
  return buf;
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)//将数字字符串转化为数字
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';  （*s++,*优先级高于++，则先取s指针指向的字符，再对指针变量s加1）
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  if (src > dst) {
    while(n-- > 0)
      *dst++ = *src++;
  } else {
    dst += n;
    src += n;
    while(n-- > 0)
      *--dst = *--src;
  }
  return vdst;
}

int
memcmp(const void *s1, const void *s2, uint n)
{
  const char *p1 = s1, *p2 = s2;
  while (n-- > 0) {
    if (*p1 != *p2) {
      return *p1 - *p2;
    }
    p1++;
    p2++;
  }
  return 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  return memmove(dst, src, n);
}
