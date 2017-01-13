#include <syscall.h>

int atoi(char* nro){
	int ret = 0;
	while(*nro)
	{
		ret = ret*10;
		ret = ret + ((*nro)-'0');
		nro++;
	}
	return ret;
}

int strlen(char *a) {
	int i=0;
	while(a[i]) i++;
	return i;
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

int main(int argc, char** argv)
{
	int ret=0, i;
	for (i=0; i<argc; i++){
		ret = Join(atoi(argv[i]));
		Write("Pid ", 4, ConsoleOutput);
		Write(argv[i],strlen(argv[i]),ConsoleOutput);
		Write(" retornó ",10,ConsoleOutput);
		printNum(ret);
		Write("\n",1,ConsoleOutput);
	}

	return 0;
}
