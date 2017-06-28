#undef __NFDBITS
#define __NFDBITS    (8 * sizeof(unsigned long))

#undef __FD_SETSIZE
#define __FD_SETSIZE    1024

#undef __FDSET_LONGS
#define __FDSET_LONGS    (__FD_SETSIZE/__NFDBITS)

 
typedef struct {
    unsigned longfds_bits [__FDSET_LONGS];   //1024��bit�����Կ�������֧��1024��������
} __kernel_fd_set;

//ϵͳ���ã��ں�̬��
//����Ϊ maxfd, r_fds, w_fds, e_fds, timeout��
asmlinkage long sys_select(int n, fd_set __user *inp, fd_set __user *outp, fd_set __user *exp, struct timeval __user *tvp)
{
    s64 timeout = -1;
    struct timeval tv;
    int ret;

    //����ʱʱ�任��jiffies����cpuʱ��Ƶ��
    if (tvp) {
        if (copy_from_user(&tv, tvp, sizeof(tv))) //���û�̬�����������ں�̬
            return -EFAULT;
         if (tv.tv_sec < 0 || tv.tv_usec < 0)
            return -EINVAL;
         /* Cast to u64 to make GCC stop complaining */
        if ((u64)tv.tv_sec >= (u64)MAX_INT64_SECONDS)
            timeout = -1;    /* infinite */
        else {
            timeout = ROUND_UP(tv.tv_usec, USEC_PER_SEC/HZ);
            timeout += tv.tv_sec * HZ;
        }
    }
    // (***) ���� core_sys_select
    ret = core_sys_select(n, inp, outp, exp, &timeout);

    //��ʣ��ʱ�俽�����û��ռ����
    if (tvp) {
        struct timeval rtv;
        if (current->personality & STICKY_TIMEOUTS) //�жϵ�ǰ�����Ƿ�֧���޸ĳ�ʱʱ�䣨��ȷ����
            goto sticky;
        rtv.tv_usec = jiffies_to_usecs(do_div((*(u64*)&timeout), HZ));
        rtv.tv_sec = timeout;
        if (timeval_compare(&rtv, &tv) >= 0)
            rtv = tv;
        if (copy_to_user(tvp, &rtv, sizeof(rtv))) {
sticky:
            /*
             * ���Ӧ�ó���timevalֵ����ֻ���洢�У�
             * ���ǲ�ϣ���ڳɹ����select�����������޸�timeval��
             * ���ǣ���Ϊû�޸�timeval���������ǲ����������ϵͳ���á�
             */
            if (ret == -ERESTARTNOHAND)
                ret = -EINTR;
        }
    }
    return ret;
}
//��Ҫ�Ĺ�����������������
staticint core_sys_select(int n, fd_set __user *inp, fd_set __user *outp, fd_set __user *exp, s64 *timeout)
{
    fd_set_bits fds;
    /*  fd_set_bits �ṹ���£�
     typedef struct {
         unsigned long *in, *out, *ex;
         unsigned long *res_in, *res_out, *res_ex;
    } fd_set_bits;

    ����ṹ���ж����ȫ��ָ�룬��Щָ�붼������ָ�����������ϵġ�

     */

    void *bits;
    int ret, max_fds;
    unsigned int size;
    struct fdtable *fdt;
    /* Allocate small arguments on the stack to save memory and be faster �ȳ���ʹ��ջ����Ϊջʡ�ڴ��ҿ��٣�*/

    long stack_fds[SELECT_STACK_ALLOC/sizeof(long)];  // SELECT_STACK_ALLOC=256������ջ��Ԥ�ȿ��ٿռ䣬���������ļ�������̫�࣬�˿ռ䲻���͵�������ĺ�����̬����

    ret = -EINVAL;
    if (n < 0)
        goto out_nofds;

    /* max_fds can increase, so grab it once to avoid race */
    rcu_read_lock(); //rcu��

    fdt = files_fdtable(current->files); //��ȡ�ļ���������ÿ�����̻�Ϊ���򿪵������ļ�����һ���ļ���������
    /*  struct fdtable �ṹ���£�
    struct fdtable {
       unsigned int max_fds;
       struct file **fd;
       ...
    };
     */

    max_fds = fdt->max_fds; //��files��������Ϊ���ļ���������ָ�룩�ṹ�л�ȡ���ֵ����ǰ�����ܹ����������ļ���Ŀ��
    rcu_read_unlock();
    if (n > max_fds)// ��������n���ڵ�ǰ���������ļ�����������������
        n = max_fds;

    /* ������Ҫʹ��6�������������������������,
     * �ֱ���in/out/exception���μ�fd_set_bits�ṹ�壩,
     * ����ÿ����һ�������һ�����(���ڽ������) */
    size = FDS_BYTES(n);// ��һ���ļ�������ռһbit�����㣬���ݽ�������Щfd_set��Ҫ�õ����ٸ��֣�32λ�֣�
    bits = stack_fds;
    if (size > sizeof(stack_fds) / 6) { // ����6����Ϊÿ���ļ���������Ҫ6��bitmaps�ϵ�λ��������д���쳣�Լ����������ķ��ؽ����ռ6λ��
        //ջ�������㣬��ǰ�ĳ���ʧ�ܣ�ֻ��ʹ��kmalloc��ʽ
        /* Not enough space in on-stack array; must use kmalloc */
        ret = -ENOMEM;
        bits = kmalloc(6 * size, GFP_KERNEL);
        if (!bits)
            goto out_nofds;
    }
    //����fds
    fds.in      = bits;
    fds.out     = bits +   size;//һ��size��С��ռ��λ�������ļ����������ܸ���
    fds.ex      = bits + 2*size;
    fds.res_in  = bits + 3*size;
    fds.res_out = bits + 4*size;
    fds.res_ex  = bits + 5*size;

    // get_fd_set��������copy_from_user���û��ռ俽����fd_set,������û��ռ䴫�������ļ�������Ҫ���ӵ�״̬���Ƶ��ںˣ�����ü���״̬����ʱ�䷢���ͽ����������res_in,res_out��λ����һ
    if ((ret = get_fd_set(n, inp, fds.in)) ||
        (ret = get_fd_set(n, outp, fds.out)) ||
        (ret = get_fd_set(n, exp, fds.ex)))
        goto out;

    // ����Щ��ŷ���״̬���ֶ���0
    zero_fd_set(n, fds.res_in);
    zero_fd_set(n, fds.res_out);
    zero_fd_set(n, fds.res_ex);

    //ִ��do_select����ɼ�ع���
    ret = do_select(n, &fds, timeout);
    if (ret < 0) // �д���
        goto out;
    if (!ret) {  // ��ʱ���أ����豸����
        ret = -ERESTARTNOHAND;
        if (signal_pending(current))
            goto out;
        ret = 0;
    }

    if (set_fd_set(n, inp, fds.res_in) ||
        set_fd_set(n, outp, fds.res_out) ||
        set_fd_set(n, exp, fds.res_ex))
        ret = -EFAULT;

out:
    if (bits != stack_fds)
        kfree(bits);

out_nofds:
    return ret;
}
#define POLLIN_SET (POLLRDNORM | POLLRDBAND | POLLIN | POLLHUP | POLLERR)
#define POLLOUT_SET (POLLWRBAND | POLLWRNORM | POLLOUT | POLLERR)
#define POLLEX_SET (POLLPRI)

int do_select(int n, fd_set_bits *fds, s64 *timeout)
{
    struct poll_wqueues table;

    /*
     struct poll_wqueues {
          poll_table pt;
          struct poll_table_page *table;
          struct task_struct *polling_task; //���浱ǰ����select���û�����struct task_struct�ṹ��
          int triggered;         // ��ǰ�û����̱����Ѻ��ó�1������ý��̽��Ž�˯��
          int error;             // ������
          int inline_index;      // ����inline_entries�������±�
          struct poll_table_entry inline_entries[N_INLINE_POLL_ENTRIES];
    };

     */

    poll_table *wait;
    int retval, i;
    rcu_read_lock();
    //�����Ѿ����úõ�fdλͼ����û��򿪵�fd, Ҫ���Ӧfd�����, ���ҷ�������fd��

    retval = max_select_fd(n, fds);
    rcu_read_unlock();
    if (retval < 0)
        return retval;

    n = retval;
    /* һЩ��Ҫ�ĳ�ʼ����
       poll_wqueues.poll_table.qproc����ָ���ʼ����
       �ú���������������poll������fop->poll��ʵ���б���Ҫ���õ�poll_wait()��ʹ�õĺ�����  */
    poll_initwait(&table);
    wait = &table.pt;
    if (!*timeout)
        wait = NULL;        // �û������˳�ʱʱ��Ϊ0
    retval = 0;

    for (;;) {
        unsigned long *rinp, *routp, *rexp, *inp, *outp, *exp;
        long __timeout;
        set_current_state(TASK_INTERRUPTIBLE);
        inp = fds->in; outp = fds->out; exp = fds->ex;
        rinp = fds->res_in; routp = fds->res_out; rexp = fds->res_ex;
        // ����n��fd��ѭ��
        for (i = 0; i < n; ++rinp, ++routp, ++rexp) {
            unsigned long in, out, ex, all_bits, bit = 1, mask, j;
            unsigned long res_in = 0, res_out = 0, res_ex = 0;
            const struct file_operations *f_op = NULL;
            struct file *file = NULL;
             // ��ȡ����ǰѭ�������е�32����longռ32λ�����ļ���������Ӧ��bitmaps
            in = *inp++; out = *outp++; ex = *exp++;
            all_bits = in | out | ex;// ���һ�£��е�fd����ֻ����������д������err������ͬʱ�����
            if (all_bits == 0) {
                i += __NFDBITS; //��������û�д����ҵ�������, �����������(32λ��__NFDBITS=32)��ȡ��һ��32��fd��ѭ����
                continue;

            }
            // ����32��fd��ѭ��������Ҫ����״̬���ڣ������α���ÿһλ���ж�������״̬����һ��״̬���¼�����
            for (j = 0; j < __NFDBITS; ++j, ++i, bit <<= 1) {
                int fput_needed;
                if (i >= n)
                   break;
                if (!(bit & all_bits)) // bitÿ��ѭ��������һλ�������������������û��״̬����fd
                   continue;

                file = fget_light(i, &fput_needed);//�õ�file�ṹָ�룬���������ü����ֶ�f_count
                if (file) {// ���file���ڣ�����ļ���������Ӧ���ļ�ȷʵ���ˣ�
                    f_op = file->f_op;
                    mask = DEFAULT_POLLMASK;
                    if (f_op && f_op->poll) //����ļ���Ӧ�����������ṩ��poll������fop->poll����
                        mask = (*f_op->poll)(file, retval ? NULL : wait);//�������������е�poll������
                    /*  �������������е�poll��������evdev�����е�evdev_poll()Ϊ��
                     *  �ú�������ú���poll_wait(file, &evdev->wait, wait)��
                     *  ��������__pollwait()�ص�������һ��poll_table_entry�ṹ�壬
                     *  �ýṹ����һ����Ƕ�ĵȴ������
                     *  ���ú�wakeʱ���õĻص�����������ӵ����������еĵȴ�����ͷ�С�  */

                    fput_light(file, fput_needed);  // �ͷ�file�ṹָ�룬ʵ�ʾ��Ǽ�С����һ�����ü����ֶ�f_count��
                    //��¼�����poll�������ص�mask���豸��״̬���롣
                    if ((mask & POLLIN_SET) && (in & bit)) {
                        res_in |= bit; //���������������ɶ�, �����λ��λ
                        retval++;   //����������������1
                    }

                    if ((mask & POLLOUT_SET) && (out & bit)) {
                        res_out |= bit;
                        retval++;
                    }

                    if ((mask & POLLEX_SET) && (ex & bit)) {
                        res_ex |= bit;
                        retval++;
                    }
                }
                /*
                 *  cond_resched()���ж��Ƿ��н�����Ҫ��ռ��ǰ���̣�
                 *  ����ǽ������������ȣ���ֻ��Ϊ������ǿռ�㡣
                 *  ����������������һ������ȥִ�У�������ʵʱ�ԣ�
                 *  ��֧����ռʽ���ȵ��ں��У�������CONFIG_PREEMPT����
                 *  cond_resched�ǿղ�����
                 */
                cond_resched();
            }

            //���ؽ��
            if (res_in)
                *rinp = res_in;
            if (res_out)
                *routp = res_out;
            if (res_ex)
               *rexp = res_ex;
        }
        wait = NULL;
        if (retval || !*timeout || signal_pending(current)) // signal_pending(current)��鵱ǰ�����Ƿ����ź�Ҫ����
            break;
        if(table.error) {
            retval = table.error;
            break;
        }
 
        if (*timeout < 0) {
            /* Wait indefinitely �����ڵȴ�*/
            __timeout = MAX_SCHEDULE_TIMEOUT;
        } elseif (unlikely(*timeout >= (s64)MAX_SCHEDULE_TIMEOUT - 1)) {
            /* Wait for longer than MAX_SCHEDULE_TIMEOUT. Do it in a loop */
            __timeout = MAX_SCHEDULE_TIMEOUT - 1;
            *timeout -= __timeout;
        } else {
            __timeout = *timeout;
            *timeout = 0;
       }

         /* schedule_timeout �����ó�CPU��
          * ��ָ����ʱ�������Ժ���������¼����ﲢ���ѽ��̣����������һ���ź�����ʱ��
          * �ý��̲ſ��Լ�������  */
        __timeout = schedule_timeout(__timeout);
        if (*timeout >= 0)
            *timeout += __timeout;
    }
    __set_current_state(TASK_RUNNING);

    poll_freewait(&table);
    return retval;
}