/*************************************************************************
    > File Name: get_task_struct.c
    > Author: wenshizhang
    > Mail: wenshizhang555@foxmail.com 
    > Created Time: 2016年05月19日 星期四 15时43分54秒
	> Descript:需要覆写的包括进程的代码段、数据段、brk段、栈段、命令行参数和环境变量
	> Implementation:先得到进程的pcb、然后根据pcb得到mm，根据mm得到每一个段的起始地址和结束地址
					 把这些地址转换成物理地址
 ************************************************************************/

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/kernel.h>
#include <asm/pgtable.h>
#include <asm/page.h>

MODULE_ALIAS("get_task_struct");

static int pid_num;

unsigned long  vir_to_real(unsigned long  vir_addr,struct task_struct *task);

static int __init get_task_init(void)
{
	struct pid		*pid;
	struct task_struct	*task;
	unsigned long code_start_real;
	unsigned long code_end_real;

	pid = find_get_pid(pid_num);
	task = pid_task(pid,PIDTYPE_PID);

	printk("task state is %ld\n",task->state);
	printk("task mem addr is  0x%lx\n",task->mm);
	printk("code start addr is  0x%lx\n",task->mm->start_code);
	printk("code end addr is  0x%lx\n",task->mm->end_code);
	printk("data start addr is  0x%lx\n",task->mm->start_data);
	printk("data end addr is  0x%lx\n",task->mm->end_data);
	printk("brk start addr is  0x%lx\n",task->mm->start_brk);
	printk("brk end addr is  0x%lx\n",task->mm->brk);
	printk("stack start addr is  0x%lx\n",task->mm->start_stack);
	printk("stack end addr is  0x%lx\n",task->mm->end_code);
	printk("arg start addr is  0x%lx\n",task->mm->arg_start);
	printk("arg end addr is  0x%lx\n",task->mm->arg_end);
	printk("env start addr is  0x%lx\n",task->mm->env_start);
	printk("env end addr is  0x%lx\n",task->mm->env_end);

	code_start_real = vir_to_real(task->mm->start_code,task);
	code_end_real = vir_to_real(task->mm->end_code,task);

	printk("real code start addr is  0x%lx\n",code_start_real);
	printk("real code end addr is  0x%lx\n",code_end_real);
	return 0;
}


unsigned long  vir_to_real(unsigned long  vir_addr,struct task_struct *task)
{
	 unsigned long pa = 0;
	 
	 pgd_t *pgd_tmp = NULL;
	 pud_t *pud_tmp = NULL;
	 pmd_t *pmd_tmp = NULL;
	 pte_t *pte_tmp = NULL;

	pgd_tmp = pgd_offset(task->mm,vir_addr);
	pud_tmp = pud_offset(pgd_tmp,vir_addr);
	pmd_tmp = pmd_offset(pud_tmp,vir_addr);
	pte_tmp = pte_offset_kernel(pmd_tmp,vir_addr);

	pa = (pte_val(*pte_tmp) & PAGE_MASK)|(vir_addr & ~PAGE_MASK);
	printk(KERN_INFO"virt_addr  0x%lx in RAM is  0x%lx .\n",vir_addr,pa);
	printk(KERN_INFO"contect in  0x%lx is  0x%lx\n",pa,
				    *(unsigned long *)((char *)pa + PAGE_OFFSET));

	return pa;
}


static void __exit get_task_exit(void)
{
	printk("exiting\n");
}

module_init(get_task_init);
module_exit(get_task_exit);
module_param(pid_num,int,S_IRUGO);

MODULE_LICENSE("GPL");
