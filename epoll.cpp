1.ʲô��epoll
epoll�ǵ�ǰ��Linux�¿������ģ������������������ѡ��epoll ��Linux2.6�ں�����ʽ���룬��select���ƣ�����I/O��·����(IO multiplexing)����,����man�ֲ��˵������Ϊ�����������������˸Ľ���poll��
linux�������¼�������ķ�����ģ�ͣ�
 
��Apacheģ�ͣ�Process Per Connection�����PPC�� �� TPC��Thread Per Connection��ģ��
������ģ��˼�����ƣ�������ÿһ�����������Ӷ���һ������/�߳�����������ģ�͵Ĵ�������Ҫʱ��Ϳռ䡣���ӽ϶�ʱ������/�߳��л��Ŀ����Ƚϴ��������ģ���ܽ��ܵ����������������ߣ�һ���ڼ��ٸ����ҡ�
 
��selectģ��
��󲢷������ƣ���Ϊһ���������򿪵�fd���ļ����������������Ƶģ���FD_SETSIZE���ã�Ĭ��ֵ��1024/2048�����selectģ�͵���󲢷����ͱ���Ӧ�����ˡ�
Ч�����⣺selectÿ�ε��ö�������ɨ��ȫ����fd���ϣ�����Ч�ʾͻ���������½�����FD_SETSIZE�Ĵ���������Щfd����ʱ�ˡ�
�ں�/�û��ռ��ڴ濽�����⣺������ں˰�fd��Ϣ֪ͨ���û��ռ��أ������������select��ȡ���ڴ濽�������� 
 
��pollģ��
��Ȼ�����select ��󲢷��������ƣ�������Ȼ����select��Ч�����⣬selectȱ���2��3����û�иĵ���
 
��epollģ��
�Ա�����ģ�͵����⣬epoll�ĸĽ����£�
1.֧��һ�����̴򿪴���Ŀ��socket������(FD) 
    select ������ܵ���һ���������򿪵�FD����һ�����Ƶģ���FD_SETSIZE���ã�Ĭ��ֵ��2048��������Щ��Ҫ֧�ֵ�����������Ŀ��IM��������˵��Ȼ̫���ˡ���ʱ����һ�ǿ���ѡ���޸������Ȼ�����±����ںˣ���������Ҳͬʱָ���������������Ч�ʵ��½������ǿ���ѡ�����̵Ľ������(��ͳ�� Apache����)��������Ȼlinux���洴�����̵Ĵ��۱Ƚ�С�����Ծ��ǲ��ɺ��ӵģ����Ͻ��̼�����ͬ��Զ�Ȳ����̼߳�ͬ���ĸ�Ч������Ҳ����һ�������ķ��������� epoll��û��������ƣ�����֧�ֵ�FD�����������Դ��ļ�����Ŀ���������һ��Զ����2048,�ٸ�����,��1GB�ڴ�Ļ����ϴ�Լ��10�����ң�������Ŀ����cat /proc/sys/fs/file-max�쿴,һ����˵�����Ŀ��ϵͳ�ڴ��ϵ�ܴ� 
  
     2.IOЧ�ʲ���FD��Ŀ���Ӷ������½� 
    ��ͳ��select/poll��һ������������ǵ���ӵ��һ���ܴ��socket���ϣ���������������ʱ����һʱ��ֻ�в��ֵ�socket��"��Ծ"�ģ�����select/pollÿ�ε��ö�������ɨ��ȫ���ļ��ϣ�����Ч�ʳ��������½�������epoll������������⣬��ֻ���"��Ծ"��socket���в���---������Ϊ���ں�ʵ����epoll�Ǹ���ÿ��fd�����callback����ʵ�ֵġ���ô��ֻ��"��Ծ"��socket�Ż�������ȥ���� callback����������idle״̬socket�򲻻ᣬ������ϣ�epollʵ����һ��"α"AIO����Ϊ��ʱ���ƶ�����os�ںˡ���һЩ benchmark�У�������е�socket�����϶��ǻ�Ծ��---����һ������LAN������epoll������select/poll��ʲôЧ�ʣ��෴���������ʹ��epoll_ctl,Ч����Ȼ�����΢���½�������һ��ʹ��idle connectionsģ��WAN����,epoll��Ч�ʾ�Զ��select/poll֮���ˡ�
  
3.ʹ��mmap�����ں����û��ռ����Ϣ���� 
    ���ʵ�����漰��epoll�ľ���ʵ���ˡ�������select,poll����epoll����Ҫ�ں˰�FD��Ϣ֪ͨ���û��ռ䣬��α��ⲻ��Ҫ���ڴ濽���ͺ���Ҫ��������ϣ�epoll��ͨ���ں����û��ռ�mmapͬһ���ڴ�ʵ�ֵġ������������һ����2.5�ں˾͹�עepoll�Ļ���һ�����������ֹ� mmap��һ���ġ�
  
4.�ں�΢�� 
      ��һ����ʵ����epoll���ŵ��ˣ���������linuxƽ̨���ŵ㡣Ҳ������Ի���linuxƽ̨���������޷��ر�linuxƽ̨������΢���ں˵����������磬�ں�TCP/IPЭ��ջʹ���ڴ�ع���sk_buff�ṹ����ô����������ʱ�ڶ�̬��������ڴ�pool(skb_head_pool)�Ĵ�С--- ͨ��echo XXXX>/proc/sys/net/core/hot_list_length��ɡ��ٱ���listen�����ĵ�2������(TCP���3�����ֵ����ݰ����г���)��Ҳ���Ը�����ƽ̨�ڴ��С��̬��������������һ�����ݰ�����Ŀ�޴�ͬʱÿ�����ݰ������Сȴ��С������ϵͳ�ϳ������µ�NAPI���������ܹ���
 
2.Epoll API
epollֻ��epoll_create,epoll_ctl,epoll_wait 3��ϵͳ���á�
   #include  <sys/epoll.h>
    
   int  epoll_create(int  size);
    
   int  epoll_ctl(int epfd, int op, int fd, structepoll_event *event);
    
   int  epoll_wait(int epfd, struct epoll_event* events, int maxevents. int timeout);
    
    
�� int epoll_create(int size);
����һ��epoll�ľ�����Դ�linux2.6.8֮��size�����Ǳ����Եġ���Ҫע����ǣ���������epoll����������ǻ�ռ��һ��fdֵ����linux������鿴/proc/����id/fd/�����ܹ��������fd�ģ�������ʹ����epoll�󣬱������close()�رգ�������ܵ���fd���ľ���
��int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
epoll���¼�ע�ắ��������ͬ��select()���ڼ����¼�ʱ�����ں�Ҫ����ʲô���͵��¼���������������ע��Ҫ�������¼����͡�
��һ��������epoll_create()�ķ���ֵ�� 
�ڶ���������ʾ������������������ʾ�� 
EPOLL_CTL_ADD��ע���µ�fd��epfd�У� 
EPOLL_CTL_MOD���޸��Ѿ�ע���fd�ļ����¼��� 
EPOLL_CTL_DEL����epfd��ɾ��һ��fd�� 
  
��������������Ҫ������fd�� 
���ĸ������Ǹ����ں���Ҫ����ʲô�£�struct epoll_event�ṹ���£�
 //���津���¼���ĳ���ļ���������ص����ݣ������ʹ�÷�ʽ�йأ�
  
 typedef union epoll_data {
     void *ptr;
     int fd;
     __uint32_t u32;
     __uint64_t u64;
 } epoll_data_t;
  //����Ȥ���¼��ͱ��������¼�
 struct epoll_event {
     __uint32_t events; /* Epoll events */
     epoll_data_t data; /* User data variable */
 };
events���������¼�����ļ��ϣ� 
EPOLLIN ����ʾ��Ӧ���ļ����������Զ��������Զ�SOCKET�����رգ��� 
EPOLLOUT����ʾ��Ӧ���ļ�����������д�� 
EPOLLPRI����ʾ��Ӧ���ļ��������н��������ݿɶ�������Ӧ�ñ�ʾ�д������ݵ������� 
EPOLLERR����ʾ��Ӧ���ļ��������������� 
EPOLLHUP����ʾ��Ӧ���ļ����������Ҷϣ� 
EPOLLET�� ��EPOLL��Ϊ��Ե����(Edge Triggered)ģʽ�����������ˮƽ����(Level Triggered)��˵�ġ� 
EPOLLONESHOT��ֻ����һ���¼���������������¼�֮���������Ҫ�����������socket�Ļ�����Ҫ�ٴΰ����socket���뵽EPOLL������
�� int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
�ռ���epoll��ص��¼����Ѿ����͵��¼�������events�Ƿ���õ�epoll_event�ṹ�����飬epoll����ѷ������¼���ֵ��events�����У�events�������ǿ�ָ�룬�ں�ֻ��������ݸ��Ƶ����events�����У�����ȥ�����������û�̬�з����ڴ棩��maxevents��֮�ں����events�ж����� maxevents��ֵ���ܴ��ڴ���epoll_create()ʱ��size������timeout�ǳ�ʱʱ�䣨���룬0���������أ�-1����ȷ����Ҳ��˵��˵������������������������óɹ������ض�ӦI/O����׼���õ��ļ���������Ŀ���緵��0��ʾ�ѳ�ʱ��
3.Epoll  ����ģʽ
��LTģʽ��Level Triggeredˮƽ����
�����ȱʡ�Ĺ���ģʽ��ͬʱ֧��block socket��non-block socket���ں˻���߳���Աһ���ļ��������Ƿ�����ˡ��������Ա�����κβ������ں��Ի�֪ͨ��
 
��ETģʽ��Edge Triggered ��Ե����
��һ�ָ���ģʽ������״̬�����仯��ʱ��Ż��֪ͨ������ģʽ�ٶ�����Ա���յ�һ��֪ͨ���ܹ������ش����¼��������ں˲���֪ͨ��һ�¼���ע�⣺�������л���δ��������ݲ���״̬�仯������ETģʽ�³���Աֻ��ȡ��һ�������ݾ���Ҳ�ò���֪ͨ�ˣ���ȷ���÷��ǳ���Ա�Լ�ȷ�϶��������е��ֽڣ�һֱ����read/writeֱ������EAGAINΪֹ����
 
����ͼ��
0����ʾ�ļ�������δ׼������
1����ʾ�ļ�������׼������
image_thumb[2]
 
����ˮƽ����ģʽ(LT)����1��������㲻���κβ������ں����ɻ᲻�ϵ�֪ͨ�����ļ�������׼��������
���ڱ�Ե����ģʽ(ET): ֻ����0�仯��1����ʱ���ں˲Ż�֪ͨ�����ļ�������׼��������֮��������ڷ����ļ�������״̬�仯���ں˾Ͳ�����֪ͨ�����ļ���������׼��������
 
Nginx Ĭ�ϲ��õľ���ET��
 
 
4.ʵ��
 
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
 
 
5.EpollΪʲô��Ч
Epoll��Ч��Ҫ�����������������棺
�ٴ�����ĵ��÷�ʽ�Ϳ��Կ���epoll��select/poll��һ�����ƣ�select/pollÿ�ε��ö�Ҫ������Ҫ��ص�����fd��select/pollϵͳ���ã�����ζ��ÿ�ε��ö�Ҫ��fd�б���û�̬�������ں�̬����fd��Ŀ�ܶ�ʱ�������ɵ�Ч������ÿ�ε���epoll_waitʱ�������൱�ڵ���select/poll��������Ҫ�ٴ���fd�б���ںˣ���Ϊ�Ѿ���epoll_ctl�н���Ҫ��ص�fd�������ںˣ�epoll_ctl����Ҫÿ�ζ��������е�fd��ֻ��Ҫ��������ʽ�����������ԣ��ڵ���epoll_create֮���ں��Ѿ����ں�̬��ʼ׼�����ݽṹ���Ҫ��ص�fd�ˡ�ÿ��epoll_ctlֻ�Ƕ�������ݽṹ���м򵥵�ά����
 
�� ���⣬�ں�ʹ����slab���ƣ�Ϊepoll�ṩ�˿��ٵ����ݽṹ��
���ں��һ�н��ļ������ԣ�epoll���ں�ע����һ���ļ�ϵͳ�����ڴ洢�����ı���ص�fd���������epoll_createʱ���ͻ�����������epoll�ļ�ϵͳ�ﴴ��һ��file��㡣��Ȼ���file������ͨ�ļ�����ֻ������epoll��epoll�ڱ��ں˳�ʼ��ʱ������ϵͳ��������ͬʱ�Ὺ�ٳ�epoll�Լ����ں˸���cache�������ڰ���ÿһ���������ص�fd����Щfd���Ժ��������ʽ�������ں�cache���֧�ֿ��ٵĲ��ҡ����롢ɾ��������ں˸���cache�������ǽ��������������ڴ�ҳ��Ȼ����֮�Ͻ���slab�㣬�򵥵�˵�����������Ϸ��������Ҫ��size���ڴ����ÿ��ʹ��ʱ����ʹ�ÿ��е��ѷ���õĶ���
 
�� epoll�ĵ������������ڣ������ǵ���epoll_ctl������������fdʱ��epoll_wait��Ȼ���Էɿ�ķ��أ�����Ч�Ľ������¼���fd�������û����������������ڵ���epoll_createʱ���ں˳��˰�������epoll�ļ�ϵͳ�ｨ�˸�file��㣬���ں�cache�ｨ�˸���������ڴ洢�Ժ�epoll_ctl������fd�⣬�����ٽ���һ��list�������ڴ洢׼���������¼�����epoll_wait����ʱ�������۲����list��������û�����ݼ��ɡ������ݾͷ��أ�û�����ݾ�sleep���ȵ�timeoutʱ�䵽��ʹ����û����Ҳ���ء����ԣ�epoll_wait�ǳ���Ч�����ң�ͨ������¼�ʹ����Ҫ��ذ���Ƶ�fd�����һ��Ҳֻ���غ�������׼������fd���ѣ����ԣ�epoll_wait����Ҫ���ں�̬copy������fd���û�̬���ѡ���ô�����׼������list��������ôά�����أ�������ִ��epoll_ctlʱ�����˰�fd�ŵ�epoll�ļ�ϵͳ��file�����Ӧ�ĺ������֮�⣬������ں��жϴ������ע��һ���ص������������ںˣ�������fd���жϵ��ˣ��Ͱ����ŵ�׼������list��������ԣ���һ��fd������socket���������ݵ��ˣ��ں��ڰ��豸�������������ϵ�����copy���ں��к������fd��socket�����뵽׼������list�������ˡ�
��ˣ�һ�ź������һ��׼������fd�����������ں�cache���Ͱ����ǽ���˴󲢷��µ�fd��socket���������⡣
1.ִ��epoll_createʱ�������˺�����;���list����
2.ִ��epoll_ctlʱ���������fd��socket���������ں�������Ƿ���ڣ������������أ�����������ӵ�������ϣ�Ȼ�����ں�ע��ص����������ڵ��ж��¼�����ʱ��׼������list�����в������ݡ�
3.ִ��epoll_waitʱ���̷���׼����������������ݼ��ɡ�
6.EpollԴ�����
 
   static int __init eventpoll_init(void)
   {
     mutex_init(&pmutex);
    
     ep_poll_safewake_init(&psw);
    
     epi_cache = kmem_cache_create("eventpoll_epi", sizeof(struct epitem), 0, SLAB_HWCACHE_ALIGN|EPI_SLAB_DEBUG|SLAB_PANIC, NULL);
    
     pwq_cache = kmem_cache_create("eventpoll_pwq", sizeof(struct eppoll_entry), 0, EPI_SLAB_DEBUG|SLAB_PANIC, NULL);
    
     return 0;
   }
 
epoll��kmem_cache_create��slab�������������ڴ��������struct epitem��struct eppoll_entry��
 
����ϵͳ�����һ��fdʱ���ʹ���һ��epitem�ṹ�壬�����ں˹���epoll�Ļ������ݽṹ��
 struct epitem 
 {
     struct rb_node  rbn;        //�������ṹ����ĺ����
  
     struct list_head  rdllink;  //�¼���������
  
     struct epitem  *next;       //�������ṹ���е�����
  
     struct epoll_filefd  ffd;   //����ṹ���Ӧ�ı��������ļ���������Ϣ
  
     int  nwait;                 //poll�������¼��ĸ���
  
     struct list_head  pwqlist;  //˫�����������ű������ļ��ĵȴ����У�����������select/poll�е�poll_table
  
     struct eventpoll  *ep;      //���������ĸ����ṹ�壨���epitm������һ��eventpoll��
  
     struct list_head  fllink;   //˫�������������ӱ����ӵ��ļ���������Ӧ��struct file����Ϊfile����f_ep_link,�����������м�������ļ���epoll�ڵ�
  
     struct epoll_event  event;  //ע��ĸ���Ȥ���¼�,Ҳ�����û��ռ��epoll_event
  
 }
 
��ÿ��epoll fd��epfd����Ӧ����Ҫ���ݽṹΪ��
   struct eventpoll 
   {
       spin_lock_t       lock;             //�Ա����ݽṹ�ķ���
    
       struct mutex      mtx;              //��ֹʹ��ʱ��ɾ��
    
       wait_queue_head_t     wq;           //sys_epoll_wait() ʹ�õĵȴ�����
    
       wait_queue_head_t   poll_wait;      //file->poll()ʹ�õĵȴ�����
    
       struct list_head    rdllist;        //�¼���������������
    
       struct rb_root      rbr;            //���ڹ�������fd�ĺ������������
    
       struct epitem      *ovflist;       //���¼������fd�������������������û��ռ�
    
   }
    
 
eventpoll��epoll_createʱ����:
    long sys_epoll_create(int size) 
    {
     
        struct eventpoll *ep;
     
        ...
     
        ep_alloc(&ep); //Ϊep�����ڴ沢���г�ʼ��
     
  1 /* ����anon_inode_getfd �½�һ��file instance��Ҳ����epoll���Կ���һ���ļ��������ļ�����������ǿ��Կ���epoll_create�᷵��һ��fd��epoll����������е�fd���Ƿ���һ����Ľṹeventpoll(�����)�У�
  1 �����ṹ��struct eventpoll *ep����file->private���н��б��棨sys_epoll_ctl��ȡ�ã�*/
  1  
  1  fd = anon_inode_getfd("[eventpoll]", &eventpoll_fops, ep, O_RDWR | (flags & O_CLOEXEC));
  1  
  1      return fd;
  1  
  1 }
 
���У�ep_alloc(struct eventpoll **pep)Ϊpep�����ڴ棬����ʼ����
���У�����ע��Ĳ���eventpoll_fops�������£�
   static const struct file_operations eventpoll_fops = {
    
       .release=  ep_eventpoll_release,
    
       .poll    =  ep_eventpoll_poll,
    
   };
 
����˵�����ں���ά����һ�ú���������µĽṹ���£�
 
03152919-51d2e2ac3a51422bace3e4b0009225e1[2]_thumb[3]
 
������epoll_ctl������ʡ���˳�����ȴ��룩��
 asmlinkage long sys_epoll_ctl(int epfd,int op,int fd,struct epoll_event __user *event) {
  
    int error;
  
    struct file *file,*tfile;
  
    struct eventpoll *ep;
  
    struct epoll_event epds;
  
  
  
    error = -FAULT;
  
    //�жϲ����ĺϷ��ԣ��� __user *event ���Ƹ� epds��
  
    if(ep_op_has_event(op) && copy_from_user(&epds,event,sizeof(struct epoll_event)))
  
            goto error_return; //ʡ����ת���Ĵ���
  
  
  
    file  = fget (epfd); // epoll fd ��Ӧ���ļ�����
  
    tfile = fget(fd);    // fd ��Ӧ���ļ�����
  
  
  
    //��createʱ�����ȥ�ģ�anon_inode_getfd��������ȡ�á�
  
    ep = file->private->data;
  
  
  
    mutex_lock(&ep->mtx);
  
  
  
    //��ֹ�ظ���ӣ���ep�ĺ�����в����Ƿ��Ѿ��������fd��
  
    epi = epi_find(ep,tfile,fd);
  
  
  
    switch(op)
  
    {
  
       ...
  
        case EPOLL_CTL_ADD:  //���Ӽ���һ��fd
  
            if(!epi)
  
            {
  
                epds.events |= EPOLLERR | POLLHUP;     //Ĭ�ϰ���POLLERR��POLLHUP�¼�
  
                error = ep_insert(ep,&epds,tfile,fd);  //��ep�ĺ�����в������fd��Ӧ��epitm�ṹ�塣
  
            } else  //�ظ���ӣ���ep�ĺ�����в����Ѿ��������fd����
  
                error = -EEXIST;
  
            break;
  
        ...
  
    }
  
    return error;
  
  
  
 
ep_insert��ʵ�����£�
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
  
  
  
    //����һ��epitem�ṹ��������ÿ�������fd
  
    if(!(epi = kmem_cache_alloc(epi_cache,GFP_KERNEL)))
  
       goto error_return;
  
    //��ʼ���ýṹ��
  
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
  
    //��װpoll�ص�����
  
    init_poll_funcptr(&epq.pt, ep_ptable_queue_proc );
  
    /* ����poll��������ȡ��ǰ�¼�λ����ʵ��������������ע�ắ��ep_ptable_queue_proc��poll_wait�е��ã���
 
        ���fd���׽��֣�f_opΪsocket_file_ops��poll������
 
        sock_poll()�������TCP�׽��ֵĻ������������
 
        ��tcp_poll()�������˴�����poll�����鿴��ǰ
 
        �ļ���������״̬���洢��revents�С�
 
        ��poll�Ĵ�����(tcp_poll())�У������sock_poll_wait()��
 
        ��sock_poll_wait()�л���õ�epq.pt.qprocָ��ĺ�����
 
        Ҳ����ep_ptable_queue_proc()��  */ 
  
  
  
    revents = tfile->f_op->poll(tfile, &epq.pt);
  
  
  
    spin_lock(&tfile->f_ep_lock);
  
    list_add_tail(&epi->fllink,&tfile->f_ep_lilnks);
  
    spin_unlock(&tfile->f_ep_lock);
  
  
  
    ep_rbtree_insert(ep,epi); //����epi���뵽ep�ĺ������
  
  
  
    spin_lock_irqsave(&ep->lock,flags);
  
  
  
 //  revents & event->events���ղ�fop->poll�ķ���ֵ�б�ʶ���¼����û�event���ĵ��¼�������
  
 // !ep_is_linked(&epi->rdllink)��epi��ready�����������ݡ�ep_is_linked�����ж϶����Ƿ�Ϊ�ա�
  
 /*  ���Ҫ���ӵ��ļ�״̬�Ѿ��������һ�û�м��뵽����������,�򽫵�ǰ��
 
     epitem���뵽����������.����н������ڵȴ����ļ���״̬����,��
 
     ����һ���ȴ��Ľ��̡�  */ 
  
  
  
 if((revents & event->events) && !ep_is_linked(&epi->rdllink)) {
  
       list_add_tail(&epi->rdllink,&ep->rdllist); //����ǰepi���뵽ep->ready�����С�
  
 /* ����н������ڵȴ��ļ���״̬������
 
 Ҳ���ǵ���epoll_wait˯�ߵĽ������ڵȴ���
 
 ����һ���ȴ����̡�
 
 waitqueue_active(q) �ȴ�����q���еȴ��Ľ��̷���1�����򷵻�0��
 
 */
  
  
  
       if(waitqueue_active(&ep->wq))
  
          __wake_up_locked(&ep->wq,TAKS_UNINTERRUPTIBLE | TASK_INTERRUPTIBLE);
  
  
  
 /*  ����н��̵ȴ�eventpoll�ļ�����???�����¼�������
 
            ��������ʱ����pwake��ֵ��pwake��ֵ��Ϊ0ʱ��
 
            ���ͷ�lock�󣬻ỽ�ѵȴ����̡� */ 
  
  
  
       if(waitqueue_active(&ep->poll_wait))
  
          pwake++;
  
    }
  
    spin_unlock_irqrestore(&ep->lock,flags);
  
   
  
  
  
 if(pwake)
  
       ep_poll_safewake(&psw,&ep->poll_wait);//���ѵȴ�eventpoll�ļ�״̬�����Ľ���
  
    return 0;
  
 }
 
init_poll_funcptr(&epq.pt, ep_ptable_queue_proc);
revents = tfile->f_op->poll(tfile, &epq.pt);
������������ep_ptable_queue_procע�ᵽepq.pt�е�qproc��
 
  typedef struct poll_table_struct {
   
  poll_queue_proc qproc;
   
  unsigned long key;
   
  }poll_table;
ִ��f_op->poll(tfile, &epq.pt)ʱ��XXX_poll(tfile, &epq.pt)������ִ��poll_wait()��poll_wait()�����epq.pt.qproc��������ep_ptable_queue_proc��
ep_ptable_queue_proc�������£�
 /*  ���ļ������е�poll�����е��ã���epoll�Ļص��������뵽Ŀ���ļ��Ļ��Ѷ����С�
 
     ������ӵ��ļ����׽��֣�����whead����sock�ṹ��sk_sleep��Ա�ĵ�ַ��  */
  
 static void ep_ptable_queue_proc(struct file *file, wait_queue_head_t *whead, poll_table *pt) {
  
 /* struct ep_queue{
 
          poll_table pt;
 
          struct epitem *epi;
 
       } */
  
     struct epitem *epi = ep_item_from_epqueue(pt); //pt��ȡstruct ep_queue��epi�ֶΡ�
  
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
 
          * ��������ڴ�ʧ�ܣ���nwait��Ϊ-1����ʾ
 
          * �������󣬼��ڴ����ʧ�ܣ������ѷ�������
 
          */
  
         epi->nwait = -1;
  
     }
  
 }
 
����struct eppoll_entry�������£�
   struct eppoll_entry {
    
   struct list_head llink;
    
   struct epitem *base;
    
   wait_queue_t wait;
    
   wait_queue_head_t *whead;
    
   };
 
ep_ptable_queue_proc ������� epitem ���뵽�ض��ļ���wait��������
ep_ptable_queue_proc������������
struct file *file; ��fd��Ӧ���ļ�����
wait_queue_head_t *whead; ��fd��Ӧ���豸�ȴ����У�ͬselect�е�mydev->wait_address��
poll_table *pt; f_op->poll(tfile, &epq.pt)�е�epq.pt
��ep_ptable_queue_proc�����У�����������һ���ǳ���Ҫ�����ݽṹeppoll_entry��eppoll_entry��Ҫ���epitem��epitem�¼�����ʱ��callback��ep_poll_callback������֮��Ĺ��������Ƚ�eppoll_entry��wheadָ��fd���豸�ȴ����У�ͬselect�е�wait_address����Ȼ���ʼ��eppoll_entry��base����ָ��epitem�����ͨ��add_wait_queue��epoll_entry���ص�fd���豸�ȴ������ϡ�������������epoll_entry�Ѿ������ص�fd���豸�ȴ����С�
 
����ep_ptable_queue_proc���������˵ȴ����е�ep_poll_callback�ص��������������豸Ӳ�����ݵ���ʱ��Ӳ���жϴ������лỽ�Ѹõȴ������ϵȴ��Ľ���ʱ������û��Ѻ���ep_poll_callback
 
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
  1    //�ж�ע��ĸ���Ȥ�¼�
  1  
  1 //#define EP_PRIVATE_BITS  (EPOLLONESHOT | EPOLLET)
  1  
  1 //�з�EPOLLONESHONT��EPOLLET�¼�
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
  4     //***�ؼ�***������fd���뵽epoll�����ľ���������
  4  
  4    list_add_tail(&epi->rdllink, &ep->rdllist);
  5  
  5    //���ѵ���epoll_wait()����ʱ˯�ߵĽ��̡��û���epoll_wait(...) ��ʱǰ���ء�
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
 
����ep_poll_callback������Ҫ�Ĺ����ǽ��������ļ��ĵȴ��¼�����ʱ�����ļ���Ӧ��epitemʵ����ӵ����������У����û�����epoll_wait()ʱ���ں˻Ὣ���������е��¼�������û���
epoll_waitʵ�����£�
 SYSCALL_DEFINE4(epoll_wait, int, epfd, struct epoll_event __user *, events, int, maxevents, int, timeout)  {
  
    int error;
  
    struct file *file;
  
    struct eventpoll *ep;
  
     /* ���maxevents������ */
  
    if (maxevents <= 0 || maxevents > EP_MAX_EVENTS)
  
       return -EINVAL;
  
     /* ����û��ռ䴫���eventsָ����ڴ��Ƿ��д���μ�__range_not_ok()�� */
  
    if (!access_ok(VERIFY_WRITE, events, maxevents * sizeof(struct epoll_event))) {
  
       error = -EFAULT;
  
       goto error_return;
  
    }
  
     /* ��ȡepfd��Ӧ��eventpoll�ļ���fileʵ����file�ṹ����epoll_create�д����� */
  
    error = -EBADF;
  
    file = fget(epfd);
  
    if (!file)
  
       goto error_return;
  
     /* ͨ�����epfd��Ӧ���ļ������ǲ���eventpoll_fops ���ж�epfd�Ƿ���һ��eventpoll�ļ�����������򷵻�EINVAL���� */
  
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
  
  
  
 epoll_wait����ep_poll��ep_pollʵ�����£�
  
  static int ep_poll(struct eventpoll *ep, struct epoll_event __user *events, int maxevents, long timeout) {
  
     int res, eavail;
  
    unsigned long flags;
  
    long jtimeout;
  
    wait_queue_t wait;
  
     /* timeout���Ժ���Ϊ��λ��������Ҫת��Ϊjiffiesʱ�䡣�������999(��1000-1)����Ϊ������ȡ���� */
  
    jtimeout = (timeout < 0 || timeout >= EP_MAX_MSTIMEO) ?MAX_SCHEDULE_TIMEOUT : (timeout * HZ + 999) / 1000;
  
  retry:
  
    spin_lock_irqsave(&ep->lock, flags);
  
     res = 0;
  
    if (list_empty(&ep->rdllist)) {
  
       /* û���¼���������Ҫ˯�ߡ������¼�����ʱ��˯�߻ᱻep_poll_callback�������ѡ�*/
  
       init_waitqueue_entry(&wait, current); //��current���̷���wait����ȴ������С�
  
       wait.flags |= WQ_FLAG_EXCLUSIVE;
  
       /* ����ǰ���̼��뵽eventpoll�ĵȴ������У��ȴ��ļ�״̬������ֱ����ʱ�����ź��жϡ� */
  
       __add_wait_queue(&ep->wq, &wait);
  
        for (;;) {
  
          /* ִ��ep_poll_callback()����ʱӦ����Ҫ����ǰ���̻��ѣ����Ե�ǰ����״̬Ӧ��Ϊ���ɻ��ѡ�TASK_INTERRUPTIBLE  */
  
          set_current_state(TASK_INTERRUPTIBLE);
  
          /* ����������в�Ϊ�գ�Ҳ����˵�Ѿ����ļ���״̬�������߳�ʱ�����˳�ѭ����*/
  
          if (!list_empty(&ep->rdllist) || !jtimeout)
  
             break;
  
          /* �����ǰ���̽��յ��źţ����˳�ѭ��������EINTR���� */
  
          if (signal_pending(current)) {
  
             res = -EINTR;
  
             break;
  
          }
  
           spin_unlock_irqrestore(&ep->lock, flags);
  
          /* �����ó����������ȴ�ep_poll_callback()����ǰ���̻��ѻ��߳�ʱ,����ֵ��ʣ���ʱ�䡣
 
 �����￪ʼ��ǰ���̻����˯��״̬��ֱ��ĳЩ�ļ���״̬�������߳�ʱ��
 
 ���ļ�״̬����ʱ��eventpoll�Ļص�����ep_poll_callback()�ỽ����ep->wqָ��ĵȴ������еĽ��̡�*/
  
          jtimeout = schedule_timeout(jtimeout);
  
          spin_lock_irqsave(&ep->lock, flags);
  
       }
  
       __remove_wait_queue(&ep->wq, &wait);
  
        set_current_state(TASK_RUNNING);
  
    }
  
     /* ep->ovflist����洢�����û������¼�ʱ�ݴ�������ļ���
 
     * ���Բ����Ǿ�������ep->rdllist��Ϊ�գ�����ep->ovflist������
 
     * EP_UNACTIVE_PTR�����п��������Ѿ����ļ���״̬������
 
     * ep->ovflist������EP_UNACTIVE_PTR�����������һ����NULL����ʱ
 
     * �����������û������¼�����һ�������ļ�״̬������
 
     * һ�����ʱ��ΪNULL����ʱ���Կ϶����ļ�״̬������
 
     * �μ�ep_send_events()��
 
     */
  
    eavail = !list_empty(&ep->rdllist) || ep->ovflist != EP_UNACTIVE_PTR;
  
     spin_unlock_irqrestore(&ep->lock, flags);
  
     /* Try to transfer events to user space. In case we get 0 events and there's still timeout left over, we go trying again in search of more luck. */
  
    /* ���û�б��ź��жϣ��������¼�����������û�л�ȡ���¼�(�п��ܱ��������̻�ȡ����)������û�г�ʱ������ת��retry��ǩ�������µȴ��ļ�״̬������ */
  
    if (!res && eavail && !(res = ep_send_events(ep, events, maxevents)) && jtimeout)
  
       goto retry;
  
     /* ���ػ�ȡ�����¼��ĸ������ߴ����� */
  
    return res;
  
 }
 
 
ep_send_events�������û��ռ䷢�;����¼���
ep_send_events()�������û�������ڴ�򵥷�װ��ep_send_events_data�ṹ�У�Ȼ�����ep_scan_ready_list() �����������е��¼������û��ռ���ڴ档
�û��ռ���������������д���
