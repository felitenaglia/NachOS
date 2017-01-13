#include <syscall.h>

int main(int argc, char **argv){

	char c;
	if (argc < 2){
		Write("Faltan argumentos para cp\n", 26, ConsoleOutput);
		Exit(-1);
	}

	Create(argv[1]);

	OpenFileId out = Open(argv[1]);
	OpenFileId in = Open(argv[0]);

	if (out == -1 || in == -1){
		Write("Falló al abrir uno de los archivos\n", 36, ConsoleOutput);
		Exit(-1);
	}
	
	while(Read(&c, 1, in))
		Write(&c,1,out);
	
	Close(in);
	Close(out);

}
