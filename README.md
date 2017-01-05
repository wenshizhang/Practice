#Practice
This repo stores small project, have fun!

#Codes

* `get_task_struct.c`
	user input PID, then get process memory physical address
* `mem.c`
	intercept mkdir syscall module, new mkdir print one line message to ring buffer then return
* `hijack.c`
	intercept open syscall module, new open print one line messafe to ring buffer can call old open syscall
* `README.md`
	This document
* `Makefile`
	simple Makefile, only define compile and clean
