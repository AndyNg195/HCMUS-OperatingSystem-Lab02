#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

// Include sysinfo library.
#include "sysinfo.h"

uint64 sys_sysinfo(void)
{
  uint64 addr; // Địa chỉ của struct sysinfo trong không gian người dùng

  // Gọi argaddr và kiểm tra địa chỉ
  argaddr(0, &addr);
  return sysinfo(addr);
}
int sysinfo(uint64 addr)
{
  struct sysinfo info;

  // Thu thập thông tin hệ thống
  info.freemem = free_memory();
  info.nproc = count_active_processes();

  // Sao chép thông tin từ kernel sang không gian người dùng
  if (copyout(myproc()->pagetable, addr, (char *)&info, sizeof(info)) < 0)
    return -1;

  return 0;
}
uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (killed(myproc()))
    {
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

  argint(0, &pid);
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

// kernel/sysproc.h
uint64
sys_trace(void)
{
  int mask;
  argint(0, &mask);
  myproc()-> syscall_trace = mask;
  return 0;
}