// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct kmem{
  struct spinlock lock;
  struct run *freelist;
  uint64 free_run;
};
struct kmem cpu_mem[NCPU];
void
kinit()
{
  for(int i=0; i<NCPU; i++)
  {
    struct kmem kmem = cpu_mem[i];
    kmem.free_run = 0;
    initlock(&kmem.lock, "kmem");
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;
  push_off();
  struct kmem *kmem = &cpu_mem[cpuid()];
  pop_off();
  acquire(&kmem->lock);
  r->next = kmem->freelist;
  kmem->freelist = r;
  kmem->free_run ++;
  release(&kmem->lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r = (struct run*)0;
  push_off();
  int cpu_id = cpuid();
  pop_off();
  struct kmem *kmem = &cpu_mem[cpu_id];
  acquire(&kmem->lock);
  if(kmem->freelist)
  {
    r = kmem->freelist;
    if(r)
      kmem->freelist = r->next;
    release(&kmem->lock);

    if(r)
      memset((char*)r, 5, PGSIZE); // fill with junk
    return (void*)r;
  }else{
    goto empty;
  }
empty:
  release(&kmem->lock);
  for(int i=0; i<NCPU; i++)
  {
    if(cpu_mem[i].freelist)
    {
      acquire(&cpu_mem[i].lock);
      r = cpu_mem[i].freelist;
      cpu_mem[i].freelist = r->next;
      r->next = (void*)0;
      release(&cpu_mem[i].lock);
      break;
    }
  }

  if(r)
    memset((char*)r, 5, PGSIZE);
  return r;
}
