/*#########################################################################
# File Name: mem.c
# Author: echo zhang
# mail: wenshizhang555@hoxmail.com
# Created Time: Tue 31 May 2016 10:34:02 AM CST
# Description:
#########################################################################*/


#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/fs_struct.h>
#include <linux/mm.h>
#include <linux/syscalls.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/jiffies.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/stat.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>
#include <net/sock.h>
#include <net/inet_sock.h>
#include <asm/cpufeature.h>


unsigned long **sys_call_table = (unsigned long **)0xffffffff81801460;
unsigned long **orig_mkdir = NULL;

//make this page writable

int make_rw(unsigned long address)
{
	unsigned int level;
	//查找虚拟地址在页表(pte)中的位置
	pte_t	*pte = lookup_address(address,&level);
	
	//设置页表为读写属性
	pte->pte |= _PAGE_RW;

	return 0;
}

//make this page write protect

int make_ro(unsigned long address)
{
	unsigned int level;

	//查找虚拟地址在页表(pte)中的位置
	pte_t	*pte = lookup_address(address,&level);

	//设置只读属性
	pte->pte &= _PAGE_RW;

	return 0;
}



//编写新系统调用执行的操作

asmlinkage long hacked_exit(long code )
{
	//new systemcall doing nothing
	printk("");
	printk(KERN_ALERT "mkdir do nothing \n");

	return 0;
}

static int syscall_init_module(void)
{
	printk(KERN_ALERT "sys_call_table:0x%p\n",sys_call_table);

	orig_mkdir = (unsigned long *)(sys_call_table[__NR_mkdir]);			//获取原系统调用的地址

	printk(KERN_ALERT "orig_mkdir:0x%p\n",orig_mkdir);

	make_rw((unsigned long )sys_call_table);
	sys_call_table[__NR_mkdir] = (unsigned long *)hacked_mkdir;			//设置新的系统调用的地址

	make_ro((unsigned long )sys_call_table);

	return 0;
}

static void syscall_cleanup_module(void)
{
	printk(KERN_ALERT "Module syscall unloaded \n");

	make_rw((unsigned long )sys_call_table);
	sys_call_table[__NR_mkdir] = (unsigned long *)orig_mkdir;
	make_ro((unsigned long )sys_call_table);

}

module_init(syscall_init_module);
module_exit(syscall_cleanup_module);

MODULE_LICENSE("GPL");
