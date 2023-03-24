#include <stdio.h>

main() {
  char s[] = "asdf\n",*p=s;
  while(*p!=0) putchar(*p++);

  return 0;
}