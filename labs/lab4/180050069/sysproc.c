#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "processInfo.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// print hello to screen
int
sys_hello(void)
{
	cprintf("Hello\n");
	return 0;
}

// print hello $You$ to screen
int
sys_helloYou(void)
{
	char* you;
	if(argstr(0, &you) < 0)
    	return -1;
	cprintf("%s\n", you);
	return 0;
}

// returns the number of processes
int
sys_getNumProc(void)
{
	return getNumProcHelper();
}

// returns the max pid of the running processes
int sys_getMaxPid(void)
{
	return getMaxPidHelper();
}

// returns the process info for a given pid
int sys_getProcInfo(void){

	int pid; 
	struct processInfo* ptr_processInfo;
	if(argint(0, &pid) < 0)
    	return -1;

	if(argptr(1, (void*)&ptr_processInfo, sizeof(*ptr_processInfo)) < 0)
    	return -1;  

    return getProcInfoHelper(pid, ptr_processInfo);
}

// sets priority of current process
int sys_setprio(void){
	int p;
	if(argint(0, &p) < 0)
    	return -1;
	return setpriority_helper(p);
}

// gets priority of current process
int sys_getprio(void) {
	return getpriority_helper();
}