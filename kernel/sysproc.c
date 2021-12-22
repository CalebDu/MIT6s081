#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

void save();
void restore();
uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
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

uint64
sys_sleep(void)
{
  backtrace();
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

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_sigalarm(void)
{
  struct proc *p = myproc();
  int ticks;
  uint64 addr;
  if((argint(0, &ticks)<0)||(argaddr(1, &addr)<0))
    return -1;
  p->handler = addr;
  p->ticks = ticks;

  return 0;
}

uint64
sys_sigreturn(void)
{
  restore();
  struct proc *p = myproc();
  p->handler_run = 0;
  return 0;
}
void save()
{
  struct proc *p = myproc();
  p->trapframe->_kernel_satp = p->trapframe->kernel_satp;   // kernel page table
  p->trapframe->_kernel_sp = p->trapframe->kernel_sp;     // top of process's kernel stack
  p->trapframe->_kernel_trap = p->trapframe->kernel_trap;   // usertrap()
  p->trapframe->_epc = p->trapframe->epc;           // saved user program counter
  p->trapframe->_kernel_hartid = p->trapframe->kernel_hartid; // saved kernel tp
  p->trapframe->_ra = p->trapframe->ra;
  p->trapframe->_sp = p->trapframe->sp;
  p->trapframe->_gp = p->trapframe->gp;
  p->trapframe->_tp = p->trapframe->tp;
  p->trapframe->_t0 = p->trapframe->t0;
  p->trapframe->_t1 = p->trapframe->t1;
  p->trapframe->_t2 = p->trapframe->t2;
  p->trapframe->_s0 = p->trapframe->s0;
  p->trapframe->_s1 = p->trapframe->s1;
  p->trapframe->_a0 = p->trapframe->a0;
  p->trapframe->_a1 = p->trapframe->a1;
  p->trapframe->_a2 = p->trapframe->a2;
  p->trapframe->_a3 = p->trapframe->a3;
  p->trapframe->_a4 = p->trapframe->a4;
  p->trapframe->_a5 = p->trapframe->a5;
  p->trapframe->_a6 = p->trapframe->a6;
  p->trapframe->_a7 = p->trapframe->a7;
  p->trapframe->_s2 = p->trapframe->s2;
  p->trapframe->_s3 = p->trapframe->s3;
  p->trapframe->_s4 = p->trapframe->s4;
  p->trapframe->_s5 = p->trapframe->s5;
  p->trapframe->_s6 = p->trapframe->s6;
  p->trapframe->_s7 = p->trapframe->s7;
  p->trapframe->_s8 = p->trapframe->s8;
  p->trapframe->_s9 = p->trapframe->s9;
  p->trapframe->_s10= p->trapframe->s10;
  p->trapframe->_s11= p->trapframe->s11;
  p->trapframe->_t3 = p->trapframe->t3;
  p->trapframe->_t4 = p->trapframe->t4;
  p->trapframe->_t5 = p->trapframe->t5;
  p->trapframe->_t6 = p->trapframe->t6;
 
}

void 
restore()
{
  struct proc *p = myproc();
  p->trapframe->kernel_satp = p->trapframe->_kernel_satp;   // kernel page table
  p->trapframe->kernel_sp = p->trapframe->_kernel_sp;     // top of process's kernel stack
  p->trapframe->kernel_trap = p->trapframe->_kernel_trap;   // usertrap()
  p->trapframe->epc = p->trapframe->_epc;           // saved user program counter
  p->trapframe->kernel_hartid = p->trapframe->_kernel_hartid; // saved kernel tp
  p->trapframe->ra = p->trapframe->_ra;
  p->trapframe->sp = p->trapframe->_sp;
  p->trapframe->gp = p->trapframe->_gp;
  p->trapframe->tp = p->trapframe->_tp;
  p->trapframe->t0 = p->trapframe->_t0;
  p->trapframe->t1 = p->trapframe->_t1;
  p->trapframe->t2 = p->trapframe->_t2;
  p->trapframe->s0 = p->trapframe->_s0;
  p->trapframe->s1 = p->trapframe->_s1;
  p->trapframe->a0 = p->trapframe->_a0;
  p->trapframe->a1 = p->trapframe->_a1;
  p->trapframe->a2 = p->trapframe->_a2;
  p->trapframe->a3 = p->trapframe->_a3;
  p->trapframe->a4 = p->trapframe->_a4;
  p->trapframe->a5 = p->trapframe->_a5;
  p->trapframe->a6 = p->trapframe->_a6;
  p->trapframe->a7 = p->trapframe->_a7;
  p->trapframe->s2 = p->trapframe->_s2;
  p->trapframe->s3 = p->trapframe->_s3;
  p->trapframe->s4 = p->trapframe->_s4;
  p->trapframe->s5 = p->trapframe->_s5;
  p->trapframe->s6 = p->trapframe->_s6;
  p->trapframe->s7 = p->trapframe->_s7;
  p->trapframe->s8 = p->trapframe->_s8;
  p->trapframe->s9 = p->trapframe->_s9;
  p->trapframe->s10= p->trapframe->_s10;
  p->trapframe->s11= p->trapframe->_s11;
  p->trapframe->t3 = p->trapframe->_t3;
  p->trapframe->t4 = p->trapframe->_t4;
  p->trapframe->t5 = p->trapframe->_t5;
  p->trapframe->t6 = p->trapframe->_t6;
 
}