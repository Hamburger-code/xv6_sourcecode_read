#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"

#define PIPESIZE 512

//管道类似于消息队列和消息邮箱，带有阻塞机制的消息传递

struct pipe {     //管道结构体
  struct spinlock lock;//包含一个自旋锁
  char data[PIPESIZE]; //数组用于保存管道数据
  uint nread;     // number of bytes read  //管道读取的字节数量
  uint nwrite;    // number of bytes written //管道写入的字节数量
  int readopen;   // read fd is still open //读取的文件描述符仍然打开
  int writeopen;  // write fd is still open //写入的文件描述符仍然打开
};

int
pipealloc(struct file **f0, struct file **f1)
{
  struct pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;//指针初始化为0，一般指的是空指针
  if((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)//感觉像是文件申请内存，申请失败则goto
    goto bad;
  if((pi = (struct pipe*)kalloc()) == 0)//管道申请内存，失败则goto
    goto bad;
  pi->readopen = 1; //读取文件描述符标志置1
  pi->writeopen = 1;//写入文件描述符标志置1
  pi->nwrite = 0;
  pi->nread = 0;
  initlock(&pi->lock, "pipe");
  (*f0)->type = FD_PIPE;//文件描述符类型为管道类型
  (*f0)->readable = 1;
  (*f0)->writable = 0;//是可读类型的文件描述符
  (*f0)->pipe = pi; //指向管道pipe
  (*f1)->type = FD_PIPE;
  (*f1)->readable = 0;
  (*f1)->writable = 1;//是可写类型的文件描述符
  (*f1)->pipe = pi;//指向管道pipe
  return 0; //内存申请成功并初始化完成

 bad:    //如果申请失败，全部释放后返回-1
  if(pi)
    kfree((char*)pi);
  if(*f0)
    fileclose(*f0);
  if(*f1)
    fileclose(*f1);
  return -1;
}

void
pipeclose(struct pipe *pi, int writable) //并不是用来同时关闭两个端口的，关闭的是其中一端的端口。
{
  acquire(&pi->lock);//获取锁
  if(writable){
    pi->writeopen = 0;//可写文件描述符关闭
    wakeup(&pi->nread);//唤醒读取
  } else {
    pi->readopen = 0;//可读文件描述符关闭
    wakeup(&pi->nwrite);//唤醒写入
  }
  if(pi->readopen == 0 && pi->writeopen == 0){//两段读写文件描述符都释放关闭了
    release(&pi->lock);//释放锁
    kfree((char*)pi);//释放管道内存
  } else
    release(&pi->lock);//释放锁
}

int
pipewrite(struct pipe *pi, uint64 addr, int n)
{
  int i = 0;
  struct proc *pr = myproc();

  acquire(&pi->lock); //获取锁，拿到临界区权限
  while(i < n){
    if(pi->readopen == 0 || killed(pr)){//如果可读文件描述符消失，或者进程被杀死，释放锁，返回-1，简单理解：管道另一面没有人读的话，这个管道就废了
      release(&pi->lock);
      return -1;
    }
    if(pi->nwrite == pi->nread + PIPESIZE){ //DOC: pipewrite-full //缓存区被写满，但是内容还没写完
      wakeup(&pi->nread);//唤醒读的一边去读
      sleep(&pi->nwrite, &pi->lock);//同时写阻塞，暂时释放锁
    } else {//缓存区没满就往里面继续写
      char ch;
      if(copyin(pr->pagetable, &ch, addr + i, 1) == -1)
        break;
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);//内容写完之后就唤醒另一边去读。
  release(&pi->lock);//释放锁

  return i;
}

int
piperead(struct pipe *pi, uint64 addr, int n)
{
  int i;
  struct proc *pr = myproc();
  char ch;

  acquire(&pi->lock);//获取锁
  while(pi->nread == pi->nwrite && pi->writeopen){  //DOC: pipe-empty，如果另一端可写文件描述符还在，管道满了
    if(killed(pr)){    //进程被杀死才会释放
      release(&pi->lock);
      return -1;
    }
    sleep(&pi->nread, &pi->lock); //DOC: piperead-sleep //管道为空，另一端可写入的文件描述符还在，阻塞等待，这就是pingpong.c中提到的当管道为空
    //以及对面可写文件描述符还在的话，读取端会一直阻塞下去
  }
  for(i = 0; i < n; i++){  //DOC: piperead-copy
    if(pi->nread == pi->nwrite)//读完了就退出
      break;
    ch = pi->data[pi->nread++ % PIPESIZE];//没读完就继续读
    if(copyout(pr->pagetable, addr + i, &ch, 1) == -1)
      break;
  }
  wakeup(&pi->nwrite);  //DOC: piperead-wakeup 内容读完就唤醒写的一端（如果有写的一端的话）
  release(&pi->lock);//释放锁
  return i;
}
