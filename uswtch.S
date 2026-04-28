#user-level context switch
#
#  void uswtch(struct context **old, struct context *new);
#
# Same logic as kernel swtch, but for user threads.


.globl uswtch
uswtch:
movl 4(%esp), %eax
movl 8(%esp), %edx

# Save old callee-saved registers
pushl %ebp
pushl %ebx
pushl %esi
pushl %edi

# Switch stacks
movl %esp, (%eax)
movl %edx, %esp

#Load new callee-saved registers
popl %edi
popl %edi
popl %ebx
popl %ebp
ret

