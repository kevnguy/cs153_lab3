#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

int shm_open(int id, char **pointer) {
    unsigned char i;
    struct proc *p = myproc();
    char resMap;

    initlock(&(shm_table.lock), "SHM lock");

    acquire(&(shm_table.lock));
    for (i = 0; i< 64; i++) {
        // page already exists
        if(shm_table.shm_pages[i].id == id){
            resMap = mappages(p->pgdir, (void*) PGROUNDUP(p->sz),
                           PGSIZE, V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U);
            if(resMap == -1){
                cprintf("shm_open: mappages failed\n");
                release(&(shm_table.lock));
                return -1;
            } else{ // return va and increase size
                //cprintf("Case1: Mapped\n");
                shm_table.shm_pages[i].refcnt++;
                release(&(shm_table.lock));
                *pointer = (char*)PGROUNDUP(p->sz);
                p->sz += PGSIZE;
                return 0;
            }
        }
    }

    // Case2: doesnt exists
    for (i = 0; i< 64; i++) {
        // Find empty entry in shm_pages
        if(shm_table.shm_pages[i].id == 0){
            shm_table.shm_pages[i].id = id;
            shm_table.shm_pages[i].frame = kalloc();
            if(shm_table.shm_pages[i].frame == 0){
                cprintf("shm_open: kalloc() failed\n");
                release(&(shm_table.lock));
                return -1;
            }
            shm_table.shm_pages[i].refcnt = 1;
            // Map page
            memset(shm_table.shm_pages[i].frame, 0, PGSIZE);
            resMap = mappages(p->pgdir, (void*) PGROUNDUP(p->sz),
                           PGSIZE, V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U);
            if(resMap == -1){
                cprintf("shm_open: mappages failed\n");
                release(&(shm_table.lock));
                return -1;
            } else{
                //cprintf("Case2: Mapped\n");
                release(&(shm_table.lock));
                *pointer = (char*)PGROUNDUP(p->sz);
                p->sz += PGSIZE;
                return 0;
            }
        }
    }
    cprintf("Nothing happened\n");
    release(&(shm_table.lock));
    return 0; //added to remove compiler warning -- you should decide what to return
}


int shm_close(int id) {
//you write this too!




return 0; //added to remove compiler warning -- you should decide what to return
}

