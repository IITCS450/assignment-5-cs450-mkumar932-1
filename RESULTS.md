#RESULTS

In this assignment, I implemented user-level threads in xv6. In doing this, my xv6 kernel does not schedule the threads I am created. I keep 
a small thread table in user space and when switching threads, save the state of the current thread and restore the state of the next thread,
including registers and the stack pointer. All threads have their own stack, allowing them to pause and resume from the same place. When a 
thread calls yield, the scheduler chooses another thread to run.

I also added a mutex to protect shared sections of code. Without a mutex, two threads can acess shared data at the same time causing corruption
or mixed output. A mutex allows only one thread to access shared data at a time while others wait.

#Test results

compiled xv6 and ran test_pc in the xv6 shell:

$ test_pc
consumer got 100 items
consumer got 200 items
consumer got 300 items
consumer got 400 items
test_pc: done

thread runs correctly and program finishes without hanging.
