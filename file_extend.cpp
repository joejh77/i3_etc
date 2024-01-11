// file_extend.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define MBYTE (1024 * 1024)

int main(int argc, char* argv[])
{
	if(argc  < 2)
	{
		printf("USAGE: %s [file path]\r\n", argv[0]);
		return 0;
	}

	FILE *file = fopen(argv[1], "rb+");
	if(file){
		int length;

		fseek( file, 0, SEEK_END );
		length = ftell( file );
		printf("file size : %d Byte\r\n", length);


		int size = MBYTE - ((length + MBYTE) % MBYTE);
		if(size){
			char dumy[MBYTE];

			memset((void *)dumy, 0x00, size);
			fwrite((void *)dumy, 1, size, file);
		}

		printf("file extend size : %d (file size %d kByte)\r\n", size, (length + size) / 1024);
		

		fclose(file);
	}
	else {
		printf("file Open Error! : %s \r\n", argv[1]);
	}

	return 0;
}

