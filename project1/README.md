# Processes and Threads, IPC


**Project description:**

In this project, you are supposed to implement several programs which will search words provided by the user in a number of input files and will output the matching lines. The programs will be named psearch. They need to take the following command line parameters: an alphanumeric keyword, number of input files (n), input filenames, and an output filename. Number of input files will be greater than or equal to 1. Each input file is a text file; containing a sequence of lines. All of these lines may contain one or more (alphanumeric) words. A line of an input file matched if the given keyword is found in the line as a separate word. For instance, the keyword &quot;name&quot; appears in &quot;His name is Ali.&quot;, but does not appear in &quot;He is named Ali&quot;. The output file will contain the matched lines along with the name of the input file that the match is found in, and the line number of the match in the format below:

&lt;input filename&gt;, &lt;matched line number&gt;: &lt;matched line&gt;. The output must be sorted according to first &lt;input filename&gt; and then &lt;line number&gt;. An example execution of your program should be:

psearch name 3 input1.txt input2.txt input3.txt output.txt

**Task 1** :

**a)** Implement the program using multiple processes created by the parent main program. Call the program as psearch1a. Create children via the fork() system call. For n input files, n children must be created. Each child will process one input file and will write the result to a temporary output file. When all children end, the parent process will read the temporary output files and will write one final output file in the above format.

In psearch1a, **files will be used to pass information** from children to the parent process.

**b)** Repeat a, but this time name your program as psearch1b and use **Shared memory with shared anonymous mapping mmap()**. Also you need to use unnamed semaphores for synchronization.

**c)** Repeat a, but this time name your program as psearch1c and use an **anonymous (unnamed) pipeline pipe()**.

**Task 2** :

**a)** In this task, you will have separate processes for different jobs. There will be **a master and slave processes**.

Slave: Implement a program psearch2aslave which will take two arguments, first one is the search word, second one is the input file.

Master: Implement a program psearch2a which will take search word, input files and output file names from the user. For each input file, it should create a process psearch2slave and call it with proper arguments . (hint: exec() family of functions). So, the master will not search anything. It will make slaves do this. The master will only receive results from the slaves and saves the result into the output file.

To communicate with master, slaves should use **named shared memory (shared file mapping mmap() with a file descriptor)**

**b)** Repeat a), but the communication should be implemented using **Posix shared memory shm\_open() (without file mapping) + named semaphores (sem\_open() )**. Program names: psearch2bslave and psearch2b.

**c)** Repeat a), but the communication should be implemented using **Message passing (Message queues in Linux, mq\_open() )**. Program names: psearch2cslave and psearch2c.

**d)** Repeat a), but the communication should be implemented using **Named Pipes mkfifo()**. Program names: psearch2dslave and psearch2d.

**Task 3** : Implement the program, called psearch3, **using threads**. Instead of creating children processes, create one thread per input file. Each thread will add the matching line information to a **shared (global) linked list**. When all threads end, the main thread will process the linked lists and will write the output file in the above format. Take care of any possible synchronization problems (use semaphores). You need to use POSIX Pthreads library.


