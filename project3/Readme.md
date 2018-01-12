# Linux Kernel Module

Gains: Getting familiar with the kernel of the widely used open-source operating system -Linux-, exercising development of kernel modules, practicing with virtual memory, understanding the layout of a process’ virtual memory.
In this project, you will be developing a Linux kernel module to understand kernels. You need to work on a Linux operating system, either installed on a real machine or a virtual machine. We advise you to use a stable build of Ubuntu (16.04.3 LTS) or Linux Mint (18.3 Sylvia) distribution. These are the steps you will follow:
1. Build a Linux kernel: As the first step, you need to learn how to compile (build) and run a Linux kernel, in this way you can get ready to write your own kernel. You can find information on the internet about how to build and run a kernel. Download the source code of Linux kernel, build it and run it. If you are doing it on a real machine instead of virtual machine, backup your data first, and if you brick the file system or partitions on the disk, you can recover back. In the report, briefly point out how you built and run the kernel. Note that this can take some time, but you will need to do it only once.
2. Write a Kernel Module: Now it is time for you to exercise on developing and running (load-insert) a new kernel module. If you have the correct development environment setup, then compiling and loading/running a module is painless. You can find sufficient documentation on the internet about Linux kernel module programming. These are two good references to start with:
The Linux Kernel Module Programming Guide, http://www.tldp.org/LDP/lkmpg/2.6/html/index.html
Linux Device Drivers, Third Edition, (chiefly Chapter 2)
http://lwn.net/Kernel/LDD3/
3. Develop the Module: Implement a Linux kernel module to get data about processes created in the system, about the memory usage of a process, and about the files opened by a process. The module will get the required data from the related kernel data structures (PCBs, virtual memory regions of a process, and open file structures). Your module will be a kernel code that is loadable and unloadable during run of a kernel. Load it using the insmod command, it will take one argument, a process id (integer). When loaded, it is then a part of the kernel and runs in kernel mode and space. These are what your module must do:
i. Print the process tree: Every node will contain data about a process (id), its parent and its children. You should write the name of the process as well. Your module must traverse the process list (PCB list). There is a current variable in kernel pointing to the PCB (of type tast_struct *) of the currently running process. You can traverse the list of PCBs by starting from the current. The PCBs in ready and running states are linked together (double linked list). There are other ways to traverse the process list that you can learn from web.
Use the printk() function to print, and output will go to a kernel log file to be inspected by using commands like dmesg, more, cat, tail, etc.
ii. Print the virtual memory (VM) layout data of the process whose pid is given as an argument while your module is inserted. Print the data about the VM regions used by the process. For each VM region, print out the starting virtual address, the ending virtual address, and size of the region. This time you need to traverse the PCB list to obtain the PCB of the given process. After getting the PCB, you need to reach memory management related information from the PCB. Check the mm field of the PCB and the definition of the related structure. Also evaluate the total virtual memory used by the process.
A nice book on Linux Virtual Memory Manager: https://www.kernel.org/doc/gorman/pdf/understand.pdf
You can justify your results using some tools/commands such as: top; ps aux; cat /proc/pid/maps; cat /proc/meminfo; cat /proc/vmstat; cat /proc/zoneinfo.
You can justify your results with the output you can get from the /proc file system as well. Explore /proc directory. There are some directories with integer names. These integers are process ids. Change directory into the folder with name 'pid'. Type 'more' or 'cat'. Now there are some files. Those are actually virtual/special files whose content is not sitting on disk. The content for these files is obtained from memory resident kernel structures. You can type 'cat maps', to see the virtual memory regions of a process. If you want to find information about the kernel state, you may go into /proc directory, traverse and get kernel state information. It is the interface of the kernel state to the user.
iii. Print opened files information. Find the information about the opened files of a process via its PCB. Obtain and print out the data about each file opened by the process. You may print the handle of the file (file descriptor), name of the file, size of the file, first 3 block numbers used by the file, etc.
Report: Describe your solution. Write how you implemented and tested your kernel module. Describe the data you are getting and printing in step 3 including your output.

## Solution description

Build:
make

Installing module:
sudo insmod showpcb.ko

Read the output:
tail -f /var/log/kern.log -n 300