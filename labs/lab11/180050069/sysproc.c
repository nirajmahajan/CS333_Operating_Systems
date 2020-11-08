#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

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

int
sys_getNumFreePages(void)
{
  return get_numFreePages();
}
int
sys_getNumPhysPages(void)
{
  int count = 0;
  for (int i = 0; i < NPDENTRIES/2; i++){
    pde_t* addr = &(myproc()->pgdir[i]);
    if (addr!=0 && *addr & PTE_P){
      pte_t* pgtab = (pte_t*)P2V(PTE_ADDR(*addr));
      for (int j = 0; j < NPTENTRIES; j++){
        pte_t* pte = (pte_t*)(&pgtab[j]);
        if (pte!=0 && *pte & PTE_P){
          count++;
        }
      }
    }
  }
  return count;
}
int
sys_getNumVirtPages(void)
{
  int size = myproc()->sz;
  int remainder = size % PGSIZE;
  if (remainder == 0) {
    return size / PGSIZE;
  } else{
    int ans = (size/PGSIZE);
    return ans+1;
  }
}
int
sys_getNumPTPages(void)
{
  int count = 1;
  for (int i = 0; i < NPDENTRIES; i++){
    pde_t* addr = &(myproc()->pgdir[i]);
    if (addr!=0 && *addr & PTE_P){
      count++;
    }
  }
  return count;
}