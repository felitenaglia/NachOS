#include "syscall.h"

int
main(int argc, char** argv)
{
    int p=Exec("../test/matmult",0,0);
    int q=Exec("../test/matmult",0,0);
    Join(p);
    Join(q);
    Halt();
}
