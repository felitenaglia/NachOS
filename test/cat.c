#include <syscall.h>

int main(int argc, char **argv){

	char c;
	int i;
	OpenFileId file;
	for (i=0;i<argc;i++){
		file = Open(argv[i]);
		if (file > -1){
			while(Read(&c, 1, file))
				Write(&c,1,ConsoleOutput);
			Close(file);
		}
	}
}
