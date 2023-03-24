#include <stdio.h>



void main() {
  int a=10, b=20, c;

  asm ("mov %0, %eax" : "+r" (a));
  asm ("mov %0, %ebx" : "+r" (b));
  asm ("add %eax, %ebx");
  asm ("mov %eax, %0" : "+r" (c));
  // asm {
  //   mov ax, a
  //   mov bx, b
  //   add ax, bx
  //   mov cx, ax
  // }

  printf("c=%i", c);
}