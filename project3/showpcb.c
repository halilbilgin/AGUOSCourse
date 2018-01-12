#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/fdtable.h>
#include <linux/dcache.h>
#include<linux/slab.h>
#include <linux/rcupdate.h>
#include <linux/fs.h> 
#include <linux/fs_struct.h>
#include<linux/string.h>

MODULE_AUTHOR("HALIL BILGIN");
MODULE_VERSION("0.4");

static void traverse_linkedlist(struct task_struct *task)
{   
    struct files_struct;
    struct task_struct *child;
    struct list_head *list;
    int i = 0;
    struct fdtable *files_table;
    char *cwd;
    char *buf = (char *)kmalloc(10*sizeof(char), GFP_KERNEL);

    printk("name: %s, pid: [%d], state: %li\n", task->comm, task->pid, task->state);
    if(task->mm != NULL) {    
        printk("code memory start 0x%lx,",task->mm->start_code);
        printk("code memory end 0x%lx,",task->mm->end_code);
        printk("code memory size %lu,",task->mm->end_code - task->mm->start_code);

        printk("data start 0x%lx,",task->mm->start_data);
        printk("data end 0x%lx,",task->mm->end_data);

        printk("data memory size %lu,",task->mm->end_data - task->mm->start_data);
        printk("stack start 0x%lx,",task->mm->start_stack);
        if(task->files != NULL){
            files_table = files_fdtable(task->files);
	     while(files_table->fd[i] != NULL) {
		 cwd = d_path(&files_table->fd[i]->f_path, buf, 10*sizeof(char));

                 if(! IS_ERR(cwd))
  	             printk("Open file with fd %d  %s", i, cwd);
	         i++;   
            }
            printk("\n");
        }
    
    }
    kfree(buf);

    list_for_each(list, &task->children) {
        child = list_entry(list, struct task_struct, sibling);
        traverse_linkedlist(child);
    }
    
}

static int __init showpcb_init(void)
{
    printk(KERN_INFO "Loading showpcb Module...\n");
    traverse_linkedlist(&init_task);

    return 0;
}

static void __exit showpcb_exit(void)
{
    printk(KERN_INFO "Removing showpcb Module...\n");
}


// Macros for registering module entry and exit points.
module_init(showpcb_init);
module_exit(showpcb_exit);

