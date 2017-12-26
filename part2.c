/*
  CS3013 Project 2
  Ethan Schutzman && Steven McAteer
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <asm/current.h>
#include <asm/errno.h>
#include <linux/sched.h>
#include <linux/time.h>

struct ancestry{
  pid_t parent[10];
  pid_t children[100];
  pid_t sibling[100];
} ;
unsigned long **sys_call_table;


asmlinkage long (*ref_sys_cs3013_syscall2)(void);


asmlinkage long new_sys_cs3013_syscall2(unsigned short *target_pid, struct ancestry* response) {
  //Struct current index locations
  int childNum = 0;
  int parNum = 0;
  int sibNum = 0;

  //create the list heads for each
  struct list_head *children_tasks;
  struct list_head *sibling_tasks;

  //create the task struct pointers for each
  struct task_struct *task;
  struct task_struct *child_task;
  struct task_struct *sibling_task;
  //create tmp ancestry struct
  struct ancestry tmp;

  //task struct for finding the pid
  //struct task_struct* desired_task = current;

  struct task_struct* desired_task = pid_task(find_vpid(*target_pid), PIDTYPE_PID);
  if(desired_task == NULL){
    return 1;
  }


  //Iterate through the children

  list_for_each(children_tasks, &desired_task->children) {
  
    //copy the desired task pid into the list for child
    child_task = list_entry(children_tasks, struct task_struct, sibling);
    tmp.children[childNum] = child_task->pid;
    childNum++;

  }
  //Iterate through the siblings


  list_for_each(sibling_tasks, &desired_task->sibling) {
  
    //copy the desired task pid into the list for sibling
    sibling_task = list_entry(sibling_tasks, struct task_struct, sibling);
    tmp.sibling[sibNum] = sibling_task->pid;
    sibNum++;
                  

  }

  //Iterate through the parents

  for (task = desired_task; task != &init_task; task = task->parent) {

     //copy the desired task pid into the list for parent
     tmp.parent[parNum] = task->pid;
     parNum++;
  }
    //Try to copy back, if failed return error else return 0
    if (copy_to_user(response, &tmp, sizeof(tmp)))
	printk("Copy to user failed");
        return EFAULT; //Copy struct back to user.
    printk(KERN_INFO "Copy to user worked");
    return 0;
}

static unsigned long **find_sys_call_table(void) {
    unsigned long int offset = PAGE_OFFSET;
    unsigned long **sct;

    while (offset < ULLONG_MAX) {
        sct = (unsigned long **)offset;

        if (sct[__NR_close] == (unsigned long *) sys_close) {
            printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX",
                    (unsigned long) sct);
            return sct;
        }

        offset += sizeof(void *);
    }

    return NULL;
}

static void disable_page_protection(void) {
    /*
       Control Register 0 (cr0) governs how the CPU operates.
       Bit #16, if set, prevents the CPU from writing to memory marked as
       read only. Well, our system call table meets that description.
       But, we can simply turn off this bit in cr0 to allow us to make
       changes. We read in the current value of the register (32 or 64
       bits wide), and AND that with a value where all bits are 0 except
       the 16th bit (using a negation operation), causing the write_cr0
       value to have the 16th bit cleared (with all other bits staying
       the same. We will thus be able to write to the protected memory.
       It's good to be the kernel!
       */
    write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
    /*
       See the above description for cr0. Here, we use an OR to set the 
       16th bit to re-enable write protection on the CPU.
       */
    write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
    /* Find the system call table */
    if(!(sys_call_table = find_sys_call_table())) {
        /* Well, that didn't work. 
           Cancel the module loading step. */
        return -1;
    }

    /* Store a copy of all the existing functions */
    ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];

    /* Replace the existing system calls */
    disable_page_protection();

    sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;

    enable_page_protection();

    /* And indicate the load was successful */
    printk(KERN_INFO "Loaded interceptor!");

    return 0;
}

static void __exit interceptor_end(void) {
    /* If we don't know what the syscall table is, don't bother. */
    if(!sys_call_table)
        return;

    /* Revert all system calls to what they were before we began. */
    disable_page_protection();
    sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
    enable_page_protection();

    printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
