// file_checksum_add.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define MBYTE (1024 * 1024)
#define 	DA_FIRMWARE_UPDATE_FILE_NAME		"firstview_i3_fw"
#define	DA_FIRMWARE_VERSION1	"VRHD/" 
#define DA_FIRMWARE_VERSION2	"VRSE/"
#define DA_FIRMWARE_VERSION3	"VRVX/"
#define DA_FIRMWARE_VERSION4	"VRSS/"

int main(int argc, char* argv[])
{
	char file_path[266];
	char file_path2[266];
	char fw_version[64] = {0,};

	if(argc  < 3)
	{
		printf("USAGE: %s [file path] [recorder]\r\n", argv[0]);
		return 0;
	}
	strcpy(file_path, argv[1]);
	strcpy(file_path2, argv[2]);

	FILE *file = fopen(file_path2, "rb+");
	if(file){
		const char * version_string1 = DA_FIRMWARE_VERSION1;
		const char * version_string2 = DA_FIRMWARE_VERSION2;
		const char * version_string3 = DA_FIRMWARE_VERSION3;
		const char * version_string4 = DA_FIRMWARE_VERSION4;

		int version_string_check = 0;

		int length = 0, file_size = 0;
		int nReadSize;

		fseek( file, 0, SEEK_END );
		file_size = ftell( file );
		fseek( file, 0, SEEK_SET );

		printf("file size : %d Byte\r\n", file_size);

		do{
			char dumy[MBYTE];
			nReadSize = fread( (void *)dumy, 1, sizeof(dumy), file );
			if(nReadSize){
				for(int i = 0; i < nReadSize; i++){
					// version string searching
					if(version_string_check < 5){
						if(dumy[i] == version_string1[version_string_check] || dumy[i] == version_string2[version_string_check] || dumy[i] == version_string3[version_string_check] || dumy[i] == version_string4[version_string_check]){
							if(dumy[i] == '/')
								fw_version[version_string_check++] = '_';
							else
								fw_version[version_string_check++] = dumy[i];						
						}
						else {
							version_string_check = 0;
						}
					}
					else if(version_string_check >= 5 && version_string_check < sizeof(fw_version)){
						if(version_string_check < sizeof(fw_version) - 1){
							fw_version[version_string_check++] = dumy[i];
						}

						if(dumy[i] == 0){
							fw_version[version_string_check] = 0;
							printf("fw_version : %s\r\n", fw_version);
							version_string_check = sizeof(fw_version);
						}
					}
				}
			}
		}while(nReadSize > 0);
		fclose(file);
	}

	file = fopen(file_path, "rb+");
	if(file){
		int length = 0, file_size = 0;
		int nReadSize;
		unsigned int nChecksum = 0;

		fseek( file, 0, SEEK_END );
		file_size = ftell( file );
		fseek( file, 0, SEEK_SET );

		printf("file size : %d Byte\r\n", file_size);

		do{
			unsigned int dumy[MBYTE];
			nReadSize = fread( (void *)dumy, 1, sizeof(dumy), file );
			if(nReadSize){
				length += nReadSize;
				for(int i = 0; i < nReadSize / sizeof(unsigned int); i++){
					nChecksum += dumy[i];
				}
			}
		}while(nReadSize > 0);

		printf("file size : %d Byte (checksum 0x%x)\r\n", length, nChecksum);

		fclose(file);
		if(length){
			char cmd[256];
			char *ext = strrchr(file_path, '.');
			char *last_slush = strrchr(file_path, '/');
			char new_file_name[256];

			if(last_slush){
				last_slush++;
				*last_slush = 0;
			}
			else
				strcpy(file_path, "");

			ext++;

			printf("%s : %s %s\r\n", argv[1], file_path, ext);

			sprintf(new_file_name, "%s%s_%s.%s", file_path, DA_FIRMWARE_UPDATE_FILE_NAME, fw_version, ext);

			sprintf(cmd, "cp %s %s", argv[1], new_file_name);
			system(cmd);
			
			FILE *file = fopen(new_file_name, "rb+");
			if(file){
				nChecksum = 0xffffffff - nChecksum + 1; // 0 checksum
				fseek( file, 0, SEEK_END );
				fwrite((void *)&nChecksum, sizeof(nChecksum), 1, file);
				fclose(file);
				printf("%s : checksum 0x%x (file size %lu Byte)\r\n", new_file_name, nChecksum, (length + sizeof(nChecksum)));
			}
		}
		
	}
	else {
		printf("file Open Error! : %s \r\n", argv[1]);
	}

	return 0;
}

