#include <kernel/syscall.h>

void set_string(char * string, char * newvalue)
{
    for (int z = 0; z < std::len(string); z++)
    {
        string[z] = ' ';
    }

    for (int z = 0; z < std::len(newvalue); z++)
    {
        string[z] = newvalue[z];
        string[z+1] = 0;
    }
}

// ----------------------------- //

void print(char * text)
{
    printf(text);
}

void s_putchar(char text)
{
    putchar(text);
}

DEFN_SYSCALL1(print, 0, char *);

DEFN_SYSCALL1(s_putchar, 1, char);

// ----------------------------- //

void sys_putchar(char text)
{
    syscall_s_putchar(text);
}

void sys_print(char * text)
{
    syscall_print(text);
}

// ----------------------------- //


void syscall_append(void *func)
{
    syscalls.push_back((void *)func);
}

void syscall_handler(registers_t regs)
{
   if (regs.eax >= syscalls.size())
       return;

   void *location = syscalls.get(regs.eax);
   int ret;

   asm volatile (" \
     push %1; \
     push %2; \
     push %3; \
     push %4; \
     push %5; \
     call *%6; \
     pop %%ebx; \
     pop %%ebx; \
     pop %%ebx; \
     pop %%ebx; \
     pop %%ebx; \
   " : "=a" (ret) : "r" (regs.edi), "r" (regs.esi), "r" (regs.edx), "r" (regs.ecx), "r" (regs.ebx), "r" (location));
   //regs.eax = ret;
}

namespace Kernel {

void init_syscalls()
{
    syscall_append((void *)print);
    syscall_append((void *)s_putchar);
    Kernel::register_interrupt_handler(IRQ16, syscall_handler);
    Kernel::system_log("Syscalls initialized at interrupt 48!");
}

}