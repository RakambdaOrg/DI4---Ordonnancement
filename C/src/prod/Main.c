#include <string.h>
#include <stdlib.h>

#ifdef __WIN32__
#include <io.h>

#else
#include <sys/stat.h>
#endif

#include "headers/Utils.h"
#include "headers/Parser.h"
#include "headers/Solution.h"
#include "FLAGS.h"
#include "headers/Tabu.h"

#ifdef __MINGW32__
#define printf __mingw_printf
#endif

Bool DEBUG = False;

int main(int argc, char * argv[])
{
	char * filepath = "./Input.txt";
	if(argc >= 2)
	{
		for(int i = 1; i < argc; i++)
		{
			if(strcmp(argv[i], "--debug") == 0)
				DEBUG = True;
			else
				filepath = argv[i];
		}
	}
	
	//Main, read file and compute a solution.
	Instance * instance = parser_readInstanceFromFile(filepath);
	if(instance != NULL)
	{
#ifdef __WIN32__
		mkdir("log");
#else
		mkdir("log", 0777);
#endif
		//instance_print(instance);
		TabuSolution * solution = tabu_search(instance);
		printf("Tabu found solution in %f (%d iterations) : \n", solution->time, solution->iterations);
		solution_print(solution->solution);
		FILE * file = fopen("./output.txt", "w");
		solution_save(file, solution->solution, solution->time);
		fclose(file);
		tabuSolution_destroy(solution);
		instance_destroy(instance);
	}
	return 0;
}

void warn(char * format, ...)
{
	va_list args;
	va_start(args, format);
	fprintf(stderr, "\nWARNING: ");
	vfprintf(stderr, format, args);
	va_end(args);
}

void fatalError(char * format, ...)
{
	va_list args;
	va_start(args, format);
	fprintf(stderr, "FATAL ERROR!\n |-\t");
	vfprintf(stderr, format, args);
	va_end(args);
	exit(EXIT_FAILURE);
}
