#include <stdio.h>
#include <string.h>

int main() {
int result = 1;
char * aGdb = "gdb";  
char * str = "gdb";
char * tmp;

asm volatile ("nop;"
"lea (%%edx),%3;"
"push %1;"
"push %2;"
"call strstr;"
"add $0x10,%%esp;"
"pop %%edx;"
"test %%eax,%%eax;"
"setne %%al;"
"test %%al,%%al;"
"je no;"
"mov $0xa,%0;"
"jmp end;"
"no:"
"mov $0x1,%0;"
"end:"
"lea (%3),%%edx;"
"nop"
:"=c"(result)
:"a"(aGdb),"b"(str),"r"(tmp)
:
);
printf("result is %d", result);
return 0;
}


