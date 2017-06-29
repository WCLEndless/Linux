1.什么是epoll
epoll是当前在Linux下开发大规模并发网络程序的热门人选，epoll 在Linux2.6内核中正式引入，和select相似，都是I/O多路复用(IO multiplexing)技术,按照man手册的说法：是为处理大批量句柄而作了改进的poll。
linux下有以下几个经典的服务器模型：
 
①Apache模型（Process Per Connection，简称PPC） 和 TPC（Thread Per Connection）模型
这两种模型思想类似，就是让每一个到来的连接都有一个进程/线程来服务。这种模型的代价是它要时间和空间。连接较多时，进程/线程切换的开销比较大。因此这类模型能接受的最大连接数都不会高，一般在几百个左右。
 
②select模型
最大并发数限制：因为一个进程所打开的fd（文件描述符）是有限制的，由FD_SETSIZE设置，默认值是1024/2048，因此select模型的最大并发数就被相应限制了。
效率问题：select每次调用都会线性扫描全部的fd集合，这样效率就会呈现线性下降，把FD_SETSIZE改大可能造成这些fd都超时了。
内核/用户空间内存拷贝问题：如何让内核把fd消息通知给用户空间呢？在这个问题上select采取了内存拷贝方法。 
 
③poll模型
虽然解决了select 最大并发数的限制，但是依然存在select的效率问题，select缺点的2和3它都没有改掉。
 
④epoll模型
对比其他模型的问题，epoll的改进如下：
1.支持一个进程打开大数目的socket描述符(FD) 
    select 最不能忍受的是一个进程所打开的FD是有一定限制的，由FD_SETSIZE设置，默认值是2048。对于那些需要支持的上万连接数目的IM服务器来说显然太少了。这时候你一是可以选择修改这个宏然后重新编译内核，不过资料也同时指出这样会带来网络效率的下降，二是可以选择多进程的解决方案(传统的 Apache方案)，不过虽然linux上面创建进程的代价比较小，但仍旧是不可忽视的，加上进程间数据同步远比不上线程间同步的高效，所以也不是一种完美的方案。不过 epoll则没有这个限制，它所支持的FD上限是最大可以打开文件的数目，这个数字一般远大于2048,举个例子,在1GB内存的机器上大约是10万左右，具体数目可以cat /proc/sys/fs/file-max察看,一般来说这个数目和系统内存关系很大。 
  
     2.IO效率不随FD数目增加而线性下降 
    传统的select/poll另一个致命弱点就是当你拥有一个很大的socket集合，不过由于网络延时，任一时间只有部分的socket是"活跃"的，但是select/poll每次调用都会线性扫描全部的集合，导致效率呈现线性下降。但是epoll不存在这个问题，它只会对"活跃"的socket进行操作---这是因为在内核实现中epoll是根据每个fd上面的callback函数实现的。那么，只有"活跃"的socket才会主动的去调用 callback函数，其他idle状态socket则不会，在这点上，epoll实现了一个"伪"AIO，因为这时候推动力在os内核。在一些 benchmark中，如果所有的socket基本上都是活跃的---比如一个高速LAN环境，epoll并不比select/poll有什么效率，相反，如果过多使用epoll_ctl,效率相比还有稍微的下降。但是一旦使用idle connections模拟WAN环境,epoll的效率就远在select/poll之上了。
  
3.使用mmap加速内核与用户空间的消息传递 
    这点实际上涉及到epoll的具体实现了。无论是select,poll还是epoll都需要内核把FD消息通知给用户空间，如何避免不必要的内存拷贝就很重要，在这点上，epoll是通过内核于用户空间mmap同一块内存实现的。而如果你想我一样从2.5内核就关注epoll的话，一定不会忘记手工 mmap这一步的。
  
4.内核微调 
      这一点其实不算epoll的优点了，而是整个linux平台的优点。也许你可以怀疑linux平台，但是你无法回避linux平台赋予你微调内核的能力。比如，内核TCP/IP协议栈使用内存池管理sk_buff结构，那么可以在运行时期动态调整这个内存pool(skb_head_pool)的大小--- 通过echo XXXX>/proc/sys/net/core/hot_list_length完成。再比如listen函数的第2个参数(TCP完成3次握手的数据包队列长度)，也可以根据你平台内存大小动态调整。更甚至在一个数据包面数目巨大但同时每个数据包本身大小却很小的特殊系统上尝试最新的NAPI网卡驱动架构。
 
2.Epoll API
epoll只有epoll_create,epoll_ctl,epoll_wait 3个系统调用。
   #include  <sys/epoll.h>
    
   int  epoll_create(int  size);
    
   int  epoll_ctl(int epfd, int op, int fd, structepoll_event *event);
    
   int  epoll_wait(int epfd, struct epoll_event* events, int maxevents. int timeout);
    
    
① int epoll_create(int size);
创建一个epoll的句柄。自从linux2.6.8之后，size参数是被忽略的。需要注意的是，当创建好epoll句柄后，它就是会占用一个fd值，在linux下如果查看/proc/进程id/fd/，是能够看到这个fd的，所以在使用完epoll后，必须调用close()关闭，否则可能导致fd被耗尽。
②int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
epoll的事件注册函数，它不同于select()是在监听事件时告诉内核要监听什么类型的事件，而是在这里先注册要监听的事件类型。
第一个参数是epoll_create()的返回值。 
第二个参数表示动作，用三个宏来表示： 
EPOLL_CTL_ADD：注册新的fd到epfd中； 
EPOLL_CTL_MOD：修改已经注册的fd的监听事件； 
EPOLL_CTL_DEL：从epfd中删除一个fd； 
  
第三个参数是需要监听的fd。 
第四个参数是告诉内核需要监听什么事，struct epoll_event结构如下：
 //保存触发事件的某个文件描述符相关的数据（与具体使用方式有关）
  
 typedef union epoll_data {
     void *ptr;
     int fd;
     __uint32_t u32;
     __uint64_t u64;
 } epoll_data_t;
  //感兴趣的事件和被触发的事件
 struct epoll_event {
     __uint32_t events; /* Epoll events */
     epoll_data_t data; /* User data variable */
 };
events可以是以下几个宏的集合： 
EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）； 
EPOLLOUT：表示对应的文件描述符可以写； 
EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）； 
EPOLLERR：表示对应的文件描述符发生错误； 
EPOLLHUP：表示对应的文件描述符被挂断； 
EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。 
EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里
③ int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
收集在epoll监控的事件中已经发送的事件。参数events是分配好的epoll_event结构体数组，epoll将会把发生的事件赋值到events数组中（events不可以是空指针，内核只负责把数据复制到这个events数组中，不会去帮助我们在用户态中分配内存）。maxevents告之内核这个events有多大，这个 maxevents的值不能大于创建epoll_create()时的size，参数timeout是超时时间（毫秒，0会立即返回，-1将不确定，也有说法说是永久阻塞）。如果函数调用成功，返回对应I/O上已准备好的文件描述符数目，如返回0表示已超时。
3.Epoll  工作模式
①LT模式：Level Triggered水平触发
这个是缺省的工作模式。同时支持block socket和non-block socket。内核会告诉程序员一个文件描述符是否就绪了。如果程序员不作任何操作，内核仍会通知。
 
②ET模式：Edge Triggered 边缘触发
是一种高速模式。仅当状态发生变化的时候才获得通知。这种模式假定程序员在收到一次通知后能够完整地处理事件，于是内核不再通知这一事件。注意：缓冲区中还有未处理的数据不算状态变化，所以ET模式下程序员只读取了一部分数据就再也得不到通知了，正确的用法是程序员自己确认读完了所有的字节（一直调用read/write直到出错EAGAIN为止）。
 
如下图：
0：表示文件描述符未准备就绪
1：表示文件描述符准备就绪
image_thumb[2]
 
对于水平触发模式(LT)：在1处，如果你不做任何操作，内核依旧会不断的通知进程文件描述符准备就绪。
对于边缘出发模式(ET): 只有在0变化到1处的时候，内核才会通知进程文件描述符准备就绪。之后如果不在发生文件描述符状态变化，内核就不会再通知进程文件描述符已准备就绪。
 
Nginx 默认采用的就是ET。
 
 
4.实例
 
   #include <stdio.h>
   #include <stdlib.h>
   #include <unistd.h>
   #include <sys/socket.h>
   #include <errno.h>
   #include <sys/epoll.h>
   #include <netinet/in.h>
   #include <fcntl.h>
   #include <string.h>
    #include <netdb.h>
    
    
    
   struct epoll_event  *events = NULL;
   int epollFd = -1;
    
   const int MAX_SOCK_NUM = 1024;
    
    
   int epoll_init();
   int epoll_socket(int domain, int type, int protocol);
   int epoll_cleanup();
   int epoll_new_conn(int sfd);
    
    
   int main()
   {
         struct sockaddr_in listenAddr;
         int listenFd = -1;
    
         if(-1 == epoll_init())
         {
             printf("epoll_init err\n");
             return -1;
         }
    
         if((listenFd = epoll_socket(AF_INET,SOCK_STREAM,0)) == -1)
         {
             printf("epoll_socket err\n");
             epoll_cleanup();
             return -1;
         }
    
         listenAddr.sin_family = AF_INET;
         listenAddr.sin_port = htons(999);
         listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
         if(-1 == bind(listenFd,(struct sockaddr*)&listenAddr,sizeof(listenAddr)))
         {
             printf("bind err %d\n",errno);
             epoll_cleanup();
             return -1;
         }
    
         if(-1 == listen(listenFd,1024))
         {
             printf("listen err\n");
             epoll_cleanup();
             return -1;
         }
    
         //Add ListenFd into epoll
         if(-1 == epoll_new_conn(listenFd))
         {
             printf("eph_new_conn err\n");
             close(listenFd);
           epoll_cleanup();
           return -1;
         }
    
    
         //LOOP
         while(1)
         {
             int n;
             n = epoll_wait(listenFd,events,MAX_SOCK_NUM,-1);
             for (int i = 0; i < n; i++)
             {
                  if( (events[i].events & EPOLLERR) || ( events[i].events & EPOLLHUP ) || !(events[i].events & EPOLLIN) )
                  {
                      printf("epoll err\n");
                      close(events[i].data.fd);
                      continue;
                  }
                  else if(events[i].data.fd == listenFd)
                  {
                      while(1)
                      {
                          struct sockaddr inAddr;
                          char hbuf[1024],sbuf[NI_MAXSERV];
                          socklen_t inLen = -1;
                          int inFd = -1;
                          int s = 0;
                          int flag = 0;
    
                          inLen = sizeof(inAddr);
                          inFd = accept(listenFd,&inAddr,&inLen);
    
                          if(inFd == -1)
 1                        {
 1                            if( errno == EAGAIN || errno == EWOULDBLOCK )
 1                            {
 1                                break;
 1                            }
 1                            else
 1                            {
 1                                printf("accept error\n");
 1                                break;
 1                            }
 1                        }
 1  
 1                     if (s ==  getnameinfo (&inAddr, inLen, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV)) 
 1                     {
 1                         printf("Accepted connection on descriptor %d (host=%s, port=%s)\n", inFd, hbuf, sbuf);
 1                     }
 1  
 1                     //Set Socket to non-block
 1                     if((flag = fcntl(inFd,F_GETFL,0)) < 0 || fcntl(inFd,F_SETFL,flag | O_NONBLOCK) < 0)
 1                     {
 1                         close(inFd);
 1                         return -1;
 1                     }
 1  
 1                     epoll_new_conn(inFd);
 1                    }
 1                }
 1                else
 1                {
 1                         while (1) 
 1                         {
 1                         ssize_t count;
 1                         char buf[512];
 1  
 1                         count = read (events[i].data.fd, buf, sizeof buf);
 1  
 1                         if (count == -1) 
 1                         {
 1                             if (errno != EAGAIN)
 1                              { 
 1                                 printf("read err\n");
 1                                 }
 1  
 1                             break;
 1  
 1                         } 
 1                         else if (count == 0) 
 1                         {  
 1                             break;
 1                         }
 1  
 1                         write (1, buf, count); 
 1                     }
 1                 }
 1           }
 1  
 1       }
 1  
 1       epoll_cleanup();
 1 }
 1  
 1  
 1 int epoll_init()
 1 {
 1     if(!(events = (struct epoll_event* ) malloc ( MAX_SOCK_NUM * sizeof(struct epoll_event))))
 1     {
 1         return -1;
 1     }
 1  
 1     if( (epollFd = epoll_create(MAX_SOCK_NUM)) < 0 )
 1     {
 1         return -1;
 1     }
 1  
 1     return 0;
 1 }
 1  
 1 int epoll_socket(int domain, int type, int protocol)
 1 {
 1     int sockFd = -1;
 1     int flag = -1;
 1  
 1     if ((sockFd = socket(domain,type,protocol)) < 0)
 1     {
 1         return -1;
 1     }
 1  
 1     //Set Socket to non-block
 1     if((flag = fcntl(sockFd,F_GETFL,0)) < 0 || fcntl(sockFd,F_SETFL,flag | O_NONBLOCK) < 0)
 1     {
 1         close(sockFd);
 1         return -1;
 1     }
 1  
 1     return sockFd;
 1 }
 1  
 1 int epoll_cleanup()
 1 {
 1     free(events);
 2     close(epollFd);
 2     return 0;
 2 }
 2  
 2 int epoll_new_conn(int sfd)
 2 {
 2  
 2       struct epoll_event  epollEvent;
 2       memset(&epollEvent, 0, sizeof(struct epoll_event));
 2       epollEvent.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET;
 2       epollEvent.data.ptr = NULL;
 2       epollEvent.data.fd  = sfd;
 2  
 2       if (epoll_ctl(epollFd, EPOLL_CTL_ADD, sfd, &epollEvent) < 0)
 2       {
 2         return -1;
 2       }
 2  
 2     epollEvent.data.fd  = sfd;
 2  
 2     return 0;
 2 }
 
 
5.Epoll为什么高效
Epoll高效主要体现在以下三个方面：
①从上面的调用方式就可以看出epoll比select/poll的一个优势：select/poll每次调用都要传递所要监控的所有fd给select/poll系统调用（这意味着每次调用都要将fd列表从用户态拷贝到内核态，当fd数目很多时，这会造成低效）。而每次调用epoll_wait时（作用相当于调用select/poll），不需要再传递fd列表给内核，因为已经在epoll_ctl中将需要监控的fd告诉了内核（epoll_ctl不需要每次都拷贝所有的fd，只需要进行增量式操作）。所以，在调用epoll_create之后，内核已经在内核态开始准备数据结构存放要监控的fd了。每次epoll_ctl只是对这个数据结构进行简单的维护。
 
② 此外，内核使用了slab机制，为epoll提供了快速的数据结构：
在内核里，一切皆文件。所以，epoll向内核注册了一个文件系统，用于存储上述的被监控的fd。当你调用epoll_create时，就会在这个虚拟的epoll文件系统里创建一个file结点。当然这个file不是普通文件，它只服务于epoll。epoll在被内核初始化时（操作系统启动），同时会开辟出epoll自己的内核高速cache区，用于安置每一个我们想监控的fd，这些fd会以红黑树的形式保存在内核cache里，以支持快速的查找、插入、删除。这个内核高速cache区，就是建立连续的物理内存页，然后在之上建立slab层，简单的说，就是物理上分配好你想要的size的内存对象，每次使用时都是使用空闲的已分配好的对象。
 
③ epoll的第三个优势在于：当我们调用epoll_ctl往里塞入百万个fd时，epoll_wait仍然可以飞快的返回，并有效的将发生事件的fd给我们用户。这是由于我们在调用epoll_create时，内核除了帮我们在epoll文件系统里建了个file结点，在内核cache里建了个红黑树用于存储以后epoll_ctl传来的fd外，还会再建立一个list链表，用于存储准备就绪的事件，当epoll_wait调用时，仅仅观察这个list链表里有没有数据即可。有数据就返回，没有数据就sleep，等到timeout时间到后即使链表没数据也返回。所以，epoll_wait非常高效。而且，通常情况下即使我们要监控百万计的fd，大多一次也只返回很少量的准备就绪fd而已，所以，epoll_wait仅需要从内核态copy少量的fd到用户态而已。那么，这个准备就绪list链表是怎么维护的呢？当我们执行epoll_ctl时，除了把fd放到epoll文件系统里file对象对应的红黑树上之外，还会给内核中断处理程序注册一个回调函数，告诉内核，如果这个fd的中断到了，就把它放到准备就绪list链表里。所以，当一个fd（例如socket）上有数据到了，内核在把设备（例如网卡）上的数据copy到内核中后就来把fd（socket）插入到准备就绪list链表里了。
如此，一颗红黑树，一张准备就绪fd链表，少量的内核cache，就帮我们解决了大并发下的fd（socket）处理问题。
1.执行epoll_create时，创建了红黑树和就绪list链表。
2.执行epoll_ctl时，如果增加fd（socket），则检查在红黑树中是否存在，存在立即返回，不存在则添加到红黑树上，然后向内核注册回调函数，用于当中断事件来临时向准备就绪list链表中插入数据。
3.执行epoll_wait时立刻返回准备就绪链表里的数据即可。
6.Epoll源码分析
 
   static int __init eventpoll_init(void)
   {
     mutex_init(&pmutex);
    
     ep_poll_safewake_init(&psw);
    
     epi_cache = kmem_cache_create("eventpoll_epi", sizeof(struct epitem), 0, SLAB_HWCACHE_ALIGN|EPI_SLAB_DEBUG|SLAB_PANIC, NULL);
    
     pwq_cache = kmem_cache_create("eventpoll_pwq", sizeof(struct eppoll_entry), 0, EPI_SLAB_DEBUG|SLAB_PANIC, NULL);
    
     return 0;
   }
 
epoll用kmem_cache_create（slab分配器）分配内存用来存放struct epitem和struct eppoll_entry。
 
当向系统中添加一个fd时，就创建一个epitem结构体，这是内核管理epoll的基本数据结构：
 struct epitem 
 {
     struct rb_node  rbn;        //用于主结构管理的红黑树
  
     struct list_head  rdllink;  //事件就绪队列
  
     struct epitem  *next;       //用于主结构体中的链表
  
     struct epoll_filefd  ffd;   //这个结构体对应的被监听的文件描述符信息
  
     int  nwait;                 //poll操作中事件的个数
  
     struct list_head  pwqlist;  //双向链表，保存着被监视文件的等待队列，功能类似于select/poll中的poll_table
  
     struct eventpoll  *ep;      //该项属于哪个主结构体（多个epitm从属于一个eventpoll）
  
     struct list_head  fllink;   //双向链表，用来链接被监视的文件描述符对应的struct file。因为file里有f_ep_link,用来保存所有监视这个文件的epoll节点
  
     struct epoll_event  event;  //注册的感兴趣的事件,也就是用户空间的epoll_event
  
 }
 
而每个epoll fd（epfd）对应的主要数据结构为：
   struct eventpoll 
   {
       spin_lock_t       lock;             //对本数据结构的访问
    
       struct mutex      mtx;              //防止使用时被删除
    
       wait_queue_head_t     wq;           //sys_epoll_wait() 使用的等待队列
    
       wait_queue_head_t   poll_wait;      //file->poll()使用的等待队列
    
       struct list_head    rdllist;        //事件满足条件的链表
    
       struct rb_root      rbr;            //用于管理所有fd的红黑树（树根）
    
       struct epitem      *ovflist;       //将事件到达的fd进行链接起来发送至用户空间
    
   }
    
 
eventpoll在epoll_create时创建:
    long sys_epoll_create(int size) 
    {
     
        struct eventpoll *ep;
     
        ...
     
        ep_alloc(&ep); //为ep分配内存并进行初始化
     
  1 /* 调用anon_inode_getfd 新建一个file instance，也就是epoll可以看成一个文件（匿名文件）。因此我们可以看到epoll_create会返回一个fd。epoll所管理的所有的fd都是放在一个大的结构eventpoll(红黑树)中，
  1 将主结构体struct eventpoll *ep放入file->private项中进行保存（sys_epoll_ctl会取用）*/
  1  
  1  fd = anon_inode_getfd("[eventpoll]", &eventpoll_fops, ep, O_RDWR | (flags & O_CLOEXEC));
  1  
  1      return fd;
  1  
  1 }
 
其中，ep_alloc(struct eventpoll **pep)为pep分配内存，并初始化。
其中，上面注册的操作eventpoll_fops定义如下：
   static const struct file_operations eventpoll_fops = {
    
       .release=  ep_eventpoll_release,
    
       .poll    =  ep_eventpoll_poll,
    
   };
 
这样说来，内核中维护了一棵红黑树，大致的结构如下：
 
03152919-51d2e2ac3a51422bace3e4b0009225e1[2]_thumb[3]
 
接着是epoll_ctl函数（省略了出错检查等代码）：
 asmlinkage long sys_epoll_ctl(int epfd,int op,int fd,struct epoll_event __user *event) {
  
    int error;
  
    struct file *file,*tfile;
  
    struct eventpoll *ep;
  
    struct epoll_event epds;
  
  
  
    error = -FAULT;
  
    //判断参数的合法性，将 __user *event 复制给 epds。
  
    if(ep_op_has_event(op) && copy_from_user(&epds,event,sizeof(struct epoll_event)))
  
            goto error_return; //省略跳转到的代码
  
  
  
    file  = fget (epfd); // epoll fd 对应的文件对象
  
    tfile = fget(fd);    // fd 对应的文件对象
  
  
  
    //在create时存入进去的（anon_inode_getfd），现在取用。
  
    ep = file->private->data;
  
  
  
    mutex_lock(&ep->mtx);
  
  
  
    //防止重复添加（在ep的红黑树中查找是否已经存在这个fd）
  
    epi = epi_find(ep,tfile,fd);
  
  
  
    switch(op)
  
    {
  
       ...
  
        case EPOLL_CTL_ADD:  //增加监听一个fd
  
            if(!epi)
  
            {
  
                epds.events |= EPOLLERR | POLLHUP;     //默认包含POLLERR和POLLHUP事件
  
                error = ep_insert(ep,&epds,tfile,fd);  //在ep的红黑树中插入这个fd对应的epitm结构体。
  
            } else  //重复添加（在ep的红黑树中查找已经存在这个fd）。
  
                error = -EEXIST;
  
            break;
  
        ...
  
    }
  
    return error;
  
  
  
 
ep_insert的实现如下：
 static int ep_insert(struct eventpoll *ep, struct epoll_event *event, struct file *tfile, int fd)
  
 {
  
    int error ,revents,pwake = 0;
  
    unsigned long flags ;
  
    struct epitem *epi;
  
    /*
 
       struct ep_queue{
 
          poll_table pt;
 
          struct epitem *epi;
 
       }   */
  
  
  
    struct ep_pqueue epq;
  
  
  
    //分配一个epitem结构体来保存每个加入的fd
  
    if(!(epi = kmem_cache_alloc(epi_cache,GFP_KERNEL)))
  
       goto error_return;
  
    //初始化该结构体
  
    ep_rb_initnode(&epi->rbn);
  
    INIT_LIST_HEAD(&epi->rdllink);
  
    INIT_LIST_HEAD(&epi->fllink);
  
    INIT_LIST_HEAD(&epi->pwqlist);
  
    epi->ep = ep;
  
    ep_set_ffd(&epi->ffd,tfile,fd);
  
    epi->event = *event;
  
    epi->nwait = 0;
  
    epi->next = EP_UNACTIVE_PTR;
  
  
  
    epq.epi = epi;
  
    //安装poll回调函数
  
    init_poll_funcptr(&epq.pt, ep_ptable_queue_proc );
  
    /* 调用poll函数来获取当前事件位，其实是利用它来调用注册函数ep_ptable_queue_proc（poll_wait中调用）。
 
        如果fd是套接字，f_op为socket_file_ops，poll函数是
 
        sock_poll()。如果是TCP套接字的话，进而会调用
 
        到tcp_poll()函数。此处调用poll函数查看当前
 
        文件描述符的状态，存储在revents中。
 
        在poll的处理函数(tcp_poll())中，会调用sock_poll_wait()，
 
        在sock_poll_wait()中会调用到epq.pt.qproc指向的函数，
 
        也就是ep_ptable_queue_proc()。  */ 
  
  
  
    revents = tfile->f_op->poll(tfile, &epq.pt);
  
  
  
    spin_lock(&tfile->f_ep_lock);
  
    list_add_tail(&epi->fllink,&tfile->f_ep_lilnks);
  
    spin_unlock(&tfile->f_ep_lock);
  
  
  
    ep_rbtree_insert(ep,epi); //将该epi插入到ep的红黑树中
  
  
  
    spin_lock_irqsave(&ep->lock,flags);
  
  
  
 //  revents & event->events：刚才fop->poll的返回值中标识的事件有用户event关心的事件发生。
  
 // !ep_is_linked(&epi->rdllink)：epi的ready队列中有数据。ep_is_linked用于判断队列是否为空。
  
 /*  如果要监视的文件状态已经就绪并且还没有加入到就绪队列中,则将当前的
 
     epitem加入到就绪队列中.如果有进程正在等待该文件的状态就绪,则
 
     唤醒一个等待的进程。  */ 
  
  
  
 if((revents & event->events) && !ep_is_linked(&epi->rdllink)) {
  
       list_add_tail(&epi->rdllink,&ep->rdllist); //将当前epi插入到ep->ready队列中。
  
 /* 如果有进程正在等待文件的状态就绪，
 
 也就是调用epoll_wait睡眠的进程正在等待，
 
 则唤醒一个等待进程。
 
 waitqueue_active(q) 等待队列q中有等待的进程返回1，否则返回0。
 
 */
  
  
  
       if(waitqueue_active(&ep->wq))
  
          __wake_up_locked(&ep->wq,TAKS_UNINTERRUPTIBLE | TASK_INTERRUPTIBLE);
  
  
  
 /*  如果有进程等待eventpoll文件本身（???）的事件就绪，
 
            则增加临时变量pwake的值，pwake的值不为0时，
 
            在释放lock后，会唤醒等待进程。 */ 
  
  
  
       if(waitqueue_active(&ep->poll_wait))
  
          pwake++;
  
    }
  
    spin_unlock_irqrestore(&ep->lock,flags);
  
   
  
  
  
 if(pwake)
  
       ep_poll_safewake(&psw,&ep->poll_wait);//唤醒等待eventpoll文件状态就绪的进程
  
    return 0;
  
 }
 
init_poll_funcptr(&epq.pt, ep_ptable_queue_proc);
revents = tfile->f_op->poll(tfile, &epq.pt);
这两个函数将ep_ptable_queue_proc注册到epq.pt中的qproc。
 
  typedef struct poll_table_struct {
   
  poll_queue_proc qproc;
   
  unsigned long key;
   
  }poll_table;
执行f_op->poll(tfile, &epq.pt)时，XXX_poll(tfile, &epq.pt)函数会执行poll_wait()，poll_wait()会调用epq.pt.qproc函数，即ep_ptable_queue_proc。
ep_ptable_queue_proc函数如下：
 /*  在文件操作中的poll函数中调用，将epoll的回调函数加入到目标文件的唤醒队列中。
 
     如果监视的文件是套接字，参数whead则是sock结构的sk_sleep成员的地址。  */
  
 static void ep_ptable_queue_proc(struct file *file, wait_queue_head_t *whead, poll_table *pt) {
  
 /* struct ep_queue{
 
          poll_table pt;
 
          struct epitem *epi;
 
       } */
  
     struct epitem *epi = ep_item_from_epqueue(pt); //pt获取struct ep_queue的epi字段。
  
     struct eppoll_entry *pwq;
  
  
  
     if (epi->nwait >= 0 && (pwq = kmem_cache_alloc(pwq_cache, GFP_KERNEL))) {
  
         init_waitqueue_func_entry(&pwq->wait, ep_poll_callback);
  
         pwq->whead = whead;
  
         pwq->base = epi;
  
         add_wait_queue(whead, &pwq->wait);
  
         list_add_tail(&pwq->llink, &epi->pwqlist);
  
         epi->nwait++;
  
     } else {
  
         /* We have to signal that an error occurred */
  
         /*
 
          * 如果分配内存失败，则将nwait置为-1，表示
 
          * 发生错误，即内存分配失败，或者已发生错误
 
          */
  
         epi->nwait = -1;
  
     }
  
 }
 
其中struct eppoll_entry定义如下：
   struct eppoll_entry {
    
   struct list_head llink;
    
   struct epitem *base;
    
   wait_queue_t wait;
    
   wait_queue_head_t *whead;
    
   };
 
ep_ptable_queue_proc 函数完成 epitem 加入到特定文件的wait队列任务。
ep_ptable_queue_proc有三个参数：
struct file *file; 该fd对应的文件对象
wait_queue_head_t *whead; 该fd对应的设备等待队列（同select中的mydev->wait_address）
poll_table *pt; f_op->poll(tfile, &epq.pt)中的epq.pt
在ep_ptable_queue_proc函数中，引入了另外一个非常重要的数据结构eppoll_entry。eppoll_entry主要完成epitem和epitem事件发生时的callback（ep_poll_callback）函数之间的关联。首先将eppoll_entry的whead指向fd的设备等待队列（同select中的wait_address），然后初始化eppoll_entry的base变量指向epitem，最后通过add_wait_queue将epoll_entry挂载到fd的设备等待队列上。完成这个动作后，epoll_entry已经被挂载到fd的设备等待队列。
 
由于ep_ptable_queue_proc函数设置了等待队列的ep_poll_callback回调函数。所以在设备硬件数据到来时，硬件中断处理函数中会唤醒该等待队列上等待的进程时，会调用唤醒函数ep_poll_callback
 
    static int ep_poll_callback(wait_queue_t *wait, unsigned mode, int sync, void *key) {
     
       int pwake = 0;
     
       unsigned long flags;
     
       struct epitem *epi = ep_item_from_wait(wait);
     
       struct eventpoll *ep = epi->ep;
  1  
  1  
  1  
  1    spin_lock_irqsave(&ep->lock, flags);
  1  
  1    //判断注册的感兴趣事件
  1  
  1 //#define EP_PRIVATE_BITS  (EPOLLONESHOT | EPOLLET)
  1  
  1 //有非EPOLLONESHONT或EPOLLET事件
  2  
  2    if (!(epi->event.events & ~EP_PRIVATE_BITS))
  2  
  2       goto out_unlock;
  2  
  2  
  2  
  2    if (unlikely(ep->ovflist != EP_UNACTIVE_PTR)) {
  2  
  2       if (epi->next == EP_UNACTIVE_PTR) {
  3  
  3          epi->next = ep->ovflist;
  3  
  3          ep->ovflist = epi;
  3  
  3       }
  3  
  3       goto out_unlock;
  3  
  3    }
  4  
  4  
  4  
  4    if (ep_is_linked(&epi->rdllink))
  4  
  4       goto is_linked;
  4  
  4     //***关键***，将该fd加入到epoll监听的就绪链表中
  4  
  4    list_add_tail(&epi->rdllink, &ep->rdllist);
  5  
  5    //唤醒调用epoll_wait()函数时睡眠的进程。用户层epoll_wait(...) 超时前返回。
  5  
  5 if (waitqueue_active(&ep->wq))
  5  
  5       __wake_up_locked(&ep->wq, TASK_UNINTERRUPTIBLE | TASK_INTERRUPTIBLE);
  5  
  5    if (waitqueue_active(&ep->poll_wait))
  5  
  5       pwake++;
  6  
  6    out_unlock: spin_unlock_irqrestore(&ep->lock, flags);
  6  
  6    if (pwake)
  6  
  6       ep_poll_safewake(&psw, &ep->poll_wait);
  6  
  6    return 1;
  6  
  6 }
 
所以ep_poll_callback函数主要的功能是将被监视文件的等待事件就绪时，将文件对应的epitem实例添加到就绪队列中，当用户调用epoll_wait()时，内核会将就绪队列中的事件报告给用户。
epoll_wait实现如下：
 SYSCALL_DEFINE4(epoll_wait, int, epfd, struct epoll_event __user *, events, int, maxevents, int, timeout)  {
  
    int error;
  
    struct file *file;
  
    struct eventpoll *ep;
  
     /* 检查maxevents参数。 */
  
    if (maxevents <= 0 || maxevents > EP_MAX_EVENTS)
  
       return -EINVAL;
  
     /* 检查用户空间传入的events指向的内存是否可写。参见__range_not_ok()。 */
  
    if (!access_ok(VERIFY_WRITE, events, maxevents * sizeof(struct epoll_event))) {
  
       error = -EFAULT;
  
       goto error_return;
  
    }
  
     /* 获取epfd对应的eventpoll文件的file实例，file结构是在epoll_create中创建。 */
  
    error = -EBADF;
  
    file = fget(epfd);
  
    if (!file)
  
       goto error_return;
  
     /* 通过检查epfd对应的文件操作是不是eventpoll_fops 来判断epfd是否是一个eventpoll文件。如果不是则返回EINVAL错误。 */
  
    error = -EINVAL;
  
    if (!is_file_epoll(file))
  
       goto error_fput;
  
     /* At this point it is safe to assume that the "private_data" contains  */
  
    ep = file->private_data;
  
     /* Time to fish for events ... */
  
    error = ep_poll(ep, events, maxevents, timeout);
  
     error_fput:
  
    fput(file);
  
 error_return:
  
    return error;
  
 }
  
  
  
 epoll_wait调用ep_poll，ep_poll实现如下：
  
  static int ep_poll(struct eventpoll *ep, struct epoll_event __user *events, int maxevents, long timeout) {
  
     int res, eavail;
  
    unsigned long flags;
  
    long jtimeout;
  
    wait_queue_t wait;
  
     /* timeout是以毫秒为单位，这里是要转换为jiffies时间。这里加上999(即1000-1)，是为了向上取整。 */
  
    jtimeout = (timeout < 0 || timeout >= EP_MAX_MSTIMEO) ?MAX_SCHEDULE_TIMEOUT : (timeout * HZ + 999) / 1000;
  
  retry:
  
    spin_lock_irqsave(&ep->lock, flags);
  
     res = 0;
  
    if (list_empty(&ep->rdllist)) {
  
       /* 没有事件，所以需要睡眠。当有事件到来时，睡眠会被ep_poll_callback函数唤醒。*/
  
       init_waitqueue_entry(&wait, current); //将current进程放在wait这个等待队列中。
  
       wait.flags |= WQ_FLAG_EXCLUSIVE;
  
       /* 将当前进程加入到eventpoll的等待队列中，等待文件状态就绪或直到超时，或被信号中断。 */
  
       __add_wait_queue(&ep->wq, &wait);
  
        for (;;) {
  
          /* 执行ep_poll_callback()唤醒时应当需要将当前进程唤醒，所以当前进程状态应该为“可唤醒”TASK_INTERRUPTIBLE  */
  
          set_current_state(TASK_INTERRUPTIBLE);
  
          /* 如果就绪队列不为空，也就是说已经有文件的状态就绪或者超时，则退出循环。*/
  
          if (!list_empty(&ep->rdllist) || !jtimeout)
  
             break;
  
          /* 如果当前进程接收到信号，则退出循环，返回EINTR错误 */
  
          if (signal_pending(current)) {
  
             res = -EINTR;
  
             break;
  
          }
  
           spin_unlock_irqrestore(&ep->lock, flags);
  
          /* 主动让出处理器，等待ep_poll_callback()将当前进程唤醒或者超时,返回值是剩余的时间。
 
 从这里开始当前进程会进入睡眠状态，直到某些文件的状态就绪或者超时。
 
 当文件状态就绪时，eventpoll的回调函数ep_poll_callback()会唤醒在ep->wq指向的等待队列中的进程。*/
  
          jtimeout = schedule_timeout(jtimeout);
  
          spin_lock_irqsave(&ep->lock, flags);
  
       }
  
       __remove_wait_queue(&ep->wq, &wait);
  
        set_current_state(TASK_RUNNING);
  
    }
  
     /* ep->ovflist链表存储的向用户传递事件时暂存就绪的文件。
 
     * 所以不管是就绪队列ep->rdllist不为空，或者ep->ovflist不等于
 
     * EP_UNACTIVE_PTR，都有可能现在已经有文件的状态就绪。
 
     * ep->ovflist不等于EP_UNACTIVE_PTR有两种情况，一种是NULL，此时
 
     * 可能正在向用户传递事件，不一定就有文件状态就绪，
 
     * 一种情况时不为NULL，此时可以肯定有文件状态就绪，
 
     * 参见ep_send_events()。
 
     */
  
    eavail = !list_empty(&ep->rdllist) || ep->ovflist != EP_UNACTIVE_PTR;
  
     spin_unlock_irqrestore(&ep->lock, flags);
  
     /* Try to transfer events to user space. In case we get 0 events and there's still timeout left over, we go trying again in search of more luck. */
  
    /* 如果没有被信号中断，并且有事件就绪，但是没有获取到事件(有可能被其他进程获取到了)，并且没有超时，则跳转到retry标签处，重新等待文件状态就绪。 */
  
    if (!res && eavail && !(res = ep_send_events(ep, events, maxevents)) && jtimeout)
  
       goto retry;
  
     /* 返回获取到的事件的个数或者错误码 */
  
    return res;
  
 }
 
 
ep_send_events函数向用户空间发送就绪事件。
ep_send_events()函数将用户传入的内存简单封装到ep_send_events_data结构中，然后调用ep_scan_ready_list() 将就绪队列中的事件传入用户空间的内存。
用户空间访问这个结果，进行处理。
