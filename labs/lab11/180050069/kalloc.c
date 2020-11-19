// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld


int reference_counts[PHYSTOP/PGSIZE];
struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  int num_free_pages;
  struct run *freelist;
} kmem;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.num_free_pages = 0;
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree(p);
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  uint pa = V2P(v);
  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  if (reference_counts[pa/PGSIZE] > 1){
    reference_counts[pa/PGSIZE] -= 1;
    if(kmem.use_lock)
      release(&kmem.lock);
    return;
  }
  if(kmem.use_lock)
    release(&kmem.lock);

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  reference_counts[pa/PGSIZE] -= 1;
  r->next = kmem.freelist;
  kmem.freelist = r;
  kmem.num_free_pages += 1;
  if(kmem.use_lock)
    release(&kmem.lock);  
  
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  uint pa = V2P((char *)r);
  if(r){
    kmem.freelist = r->next;
    reference_counts[pa/PGSIZE] = 1;
    kmem.num_free_pages -= 1;
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

int get_numFreePages(void){
  return kmem.num_free_pages;
}

void increment_RC(uint pa){
  if(kmem.use_lock)
    acquire(&kmem.lock);
  reference_counts[pa/PGSIZE] += 1;
  if(kmem.use_lock)
    release(&kmem.lock);
}

void decrement_RC(uint pa){
  if(kmem.use_lock)
    acquire(&kmem.lock);
  reference_counts[pa/PGSIZE] -= 1;
  if(kmem.use_lock)
    release(&kmem.lock);
}

int get_RC(uint pa){
  return reference_counts[pa/PGSIZE];
}
