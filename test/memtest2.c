#include "syscall.h"
#define NUMTEST 15

int
main(int argc, char** argv)
{
    int p[NUMTEST],i;
    for(i=0;i<NUMTEST;i++)
        p[i] = Exec("../test/sort",0,0);
    for(i=0;i<NUMTEST;i++)
        Join(p[i]);
   Halt();
}
