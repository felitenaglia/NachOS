#include "syscall.h"

int strlen(char *a) {
	int i=0;
	while(a[i]) i++;
	return i;
}

void strcpy (char *s1, char *s2) {
	do *(s1++) = *(s2); while(*(s2++));
}

void printNum(int num){
	char itoa[11];
	int i, neg=0;
	if (num < 0){
		neg = 1;
		num = (-1)*num;
	}
	if (num == 0){
		itoa[10] = '0';
		Write(itoa+10,1,ConsoleOutput);
	}else{
		for (i=10; num; i--, num = num/10)
			itoa[i] = (num % 10) + '0';
		if (neg)
			itoa[i--] = '-';
		Write(itoa+i+1,10-i,ConsoleOutput);
	}
}

int 
main()
{
	int i,pid,argc,flag_space,background;
	char buffer[128];
	char path[40];
	strcpy(path,"../test/");

	char *argv[32];
	Write("       ,-~~-.___.\n      / |  '     \\\n     (  )         0\n      \\_/-, ,----'\n         ====            //\n        /  \\-'~;    /~~~(O)\n       /  __/~|   /       |\n     =(  _____| (_________|\n",189,ConsoleOutput);
	while(1){
		Write("NachOS-$ ",9,ConsoleOutput);

		i=0;
		do {
			Read(&buffer[i],1,ConsoleInput);
		} while(buffer[i++]!='\n');
		buffer[--i]='\0';

		if (i>0) {
			argc=0;
			flag_space=1;
			for(i=0 ; buffer[i] ; i++) 
				if (buffer[i]==' ') {
					buffer[i]='\0';
					flag_space=1;
				}
				else if (flag_space==1) {
 					argv[argc++]=buffer+i;
					flag_space=0;
				}

			if (argv[0][0]=='&') {
				background=1;
				argv[0]++;
			} else
				background=0;
			strcpy(path+8,argv[0]);

			pid = Exec(path,argc-1,argv+1);
			if (pid==-1) {
				Write(path, strlen(path),ConsoleOutput);
				Write(": no se encontró la orden\n", 27, ConsoleOutput);
			}else if(pid==-2){
				Write("No hay suficiente memoria para ejecutar la orden\n", 49, ConsoleOutput);
			}else if (pid >= 0 && !background){
				Join(pid);
			}else if (pid >= 0 && background){
				Write("[Background pid ",16,ConsoleOutput);
				printNum(pid);
				Write("]\n",2,ConsoleOutput);
			}

		}

	}

}
