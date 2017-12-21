#include <stdio.h>
#include <string.h>

int main(){
    int edx_val = 0x563bf0b3;
    int array[25] = {
        //0x1e,0xcd,0x2a,0xd5,0x34,0x87,0xfc,0x78,
        //0x64,0x35,0x9d,0xec,0xde,0x15,0xac,0x97,
        //0x99,0xaf,0x96,0xda,0x79,0x26,0x4f,0x32,
        //0xe0
        0xe0, 0x32, 0x4f, 0x26, 0x79, 0xda, 0x96, 0xaf, 0x99, 0x97, 0xac, 0x15, 0xde, 0xec, 0x9d, 0x35, 0x64, 0x78, 0xfc, 0x87, 0x34, 0xd5, 0x2a, 0xcd, 0x1e
    };
    int flag[25] = {0};
    for (int i=0;i<25;i++){
        for (int n=0x0;n<0xff;n++){
          int result = 0;
          asm volatile (
            "nop;"
            "mov %[array],%%eax;"
            "mov %[num],%%ebx;"
            "xor %%bl,%%al;"
            "xor %%dl,%%al;"
            "jne false;"
            "ror $0x1,%%edx;"
            "movl $0x1,%[result];"
            "jmp end;"
            "false:"
            "movl $0x0,%[result];"
            "end:"
            "nop"
            :[result]"=m"(result),"=d"(edx_val)
            :[array]"r"(array[i]),[num]"r"(n),"d"(edx_val)
            :"%ebx","%eax"
            ); 
          if(result == 1){
            flag[i] = n;
            break;
            }
        }
    }
    
    for(int i=24;i>=0;i--){
    printf("s", flag[i]);
    }
    return 0;
    }
