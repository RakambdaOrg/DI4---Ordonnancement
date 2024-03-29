#include <stddef.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#ifdef __WIN32__
#include <mem.h>
#endif
#include "headers/Parser.h"

extern Bool DEBUG;

Instance * parser_readInstanceFromFile(char * filepath)
{
	debugPrint("Parsing file %s\n", filepath);
	FILE * file = fopen(filepath, "r");
	if(file == NULL)
	{
		warn("parser_readInstanceFromFile: Couldn't open file \"%s\" to parse it.", filepath);
		return NULL;
	}
	
	Instance * instance = instance_create();
	instance->origin = last_strstr(filepath, "/") + 1;
	instance = parser_fillInstance(file, instance);
	
	fclose(file);
	
	if(DEBUG)
	{
		debugPrint("Parsed instance :\n");
		instance_print(instance);
	}
	
	return instance;
}

const char * last_strstr(const char * haystack, const char * needle)
{
	if(*needle == '\0')
		return (char *) haystack;
	
	const char * result = haystack;
	for(;;)
	{
		char * p = strstr(haystack, needle);
		if(p == NULL)
			break;
		result = p;
		haystack = p + 1;
	}
	
	return result;
}

Instance * parser_fillInstance(FILE * file, Instance * instance)
{
	//Parse the machine and task counts.
	char * currentLine = parser_readLine(file);
	int * currentLineValues = parser_lineToIntArray(currentLine, 2);
	free(currentLine);
	instance->machineCount = (machine_t) currentLineValues[0];
	instance->taskCount = (task_t) currentLineValues[1];
	free(currentLineValues);
	
	//Create tasks & distance matrix.
	MMALLOC(instance->tasks, Task*, instance->taskCount, "parser_fillInstance");
	for(task_t i = 0; i < instance->taskCount; i++)
		instance->tasks[i] = task_create(instance);
	MMALLOC(instance->distancesMatrix, unsigned int*, instance->taskCount + 1, "parser_fillInstance");
	for(task_t i = 0; i <= instance->taskCount; i++)
	MMALLOC(instance->distancesMatrix[i], unsigned int, instance->taskCount + 1, "parser_fillInstance");
	
	//Parse the time on each machine for each task.
	for(machine_t machineID = 0; machineID < instance->machineCount; machineID++)
	{
		currentLine = parser_readLine(file);
		currentLineValues = parser_lineToIntArray(currentLine, instance->taskCount);
		for(task_t taskID = 0; taskID < instance->taskCount; taskID++)
			task_setMachineDuration(instance->tasks[taskID], machineID, (unsigned int) currentLineValues[taskID]);
		free(currentLine);
		free(currentLineValues);
	}
	
	//Parse the due date for each task.
	currentLine = parser_readLine(file);
	currentLineValues = parser_lineToIntArray(currentLine, instance->taskCount);
	for(task_t taskID = 0; taskID < instance->taskCount; taskID++)
		task_setDueDate(instance->tasks[taskID], (unsigned int) currentLineValues[taskID]);
	free(currentLine);
	free(currentLineValues);
	
	//Parse the distance matrix.
	for(task_t i = 0; i <= instance->taskCount; i++)
	{
		currentLine = parser_readLine(file);
		currentLineValues = parser_lineToIntArray(currentLine, instance->taskCount + 1);
		for(task_t j = 0; j <= instance->taskCount; j++)
			instance_setDistance(instance, i, j, (unsigned int) currentLineValues[j]);
		free(currentLine);
		free(currentLineValues);
	}
	
	return instance;
}

char * parser_readLine(FILE * file)
{
	char * lineRead = NULL;
	size_t size = 0;
	do
	{
		if(lineRead != NULL) //If an empty line was read before, free it.
		{
			free(lineRead);
			lineRead = NULL;
		}
		if(parser_getLine(&lineRead, &size, file) == -1) //Read a line, and return NULL if end of file.
			return NULL;
	} while(*lineRead == '\n' || (*lineRead == '\r' && lineRead[1] == '\n')); //While we have a non empty line.
	return lineRead;
}

int * parser_lineToIntArray(char * line, int valuesNumber)
{
	int * values = NULL;
	MMALLOC(values, int, valuesNumber, "parser_lineToIntArray");
	
	int valuesLength = 0; //Number of values actually put in the array.
	
	int index = 0; //The reading index.
	int reading = 0; //Boolean to know if we are currently reading a number.
	
	char * start = line; //The beginning of our number we are reading.
	int length = 0; //The length of the number we are reading.
	
	do
	{
		if(line[index] == '\t' || line[index] == ' ' || line[index] == '\0' || line[index] == '\n') //If we don't read a number.
		{
			if(reading) //If we were reading, add the number to the array.
			{
				reading = 0;
				
				valuesLength++;
				
				char * next;
				char buffer[10] = {0};
				memcpy(buffer, start, (unsigned int) length);
				buffer[length] = '\0';
				values[valuesLength - 1] = (int) strtol(buffer, &next, 10);
				
				length = 0;
				
				if(valuesLength == valuesNumber) //If we read enough, stop.
					break;
			}
		}
		else //If we read a number.
		{
			if(!reading) //If we just started reading it, change start and reading values.
			{
				reading = 1;
				start = line + index;
			}
			length++;
		}
		index++;
	} while(line[index - 1] != '\0'); //Read while we didn't reached the end of the string.
	
	for(int i = valuesLength; i < valuesNumber; i++) //Set missing values to 0.
		values[i] = 0;
	return values;
}

int parser_getLine(char ** linePtr, size_t * lineSize, FILE * file)
{
	char * bufferPtr = NULL; //Buffer string.
	unsigned int writingHead = 0; //Pointer to the writing position in the buffer.
	size_t size = 0; //The size of the buffer.
	int charRead; //The char read.
	
	if(linePtr == NULL || file == NULL || lineSize == NULL)
		return -1;
	
	bufferPtr = *linePtr;
	size = *lineSize;
	charRead = fgetc(file);
	if(charRead == EOF)
		return -1;
	if(bufferPtr == NULL) //If the string passed as parameter is NULL, initialize it.
	{
		MMALLOC(bufferPtr, char, 50, "parser_getLine");
		size = 50;
	}
	while(charRead != EOF) //While we didn't reach the end of the file.
	{
		if(writingHead > size - 2U) //If we went over the buffer size (letting space for \0), make it bigger.
		{
			size += 50;
			RREALLOC(bufferPtr, char, size, "parser_getLine");
		}
		if(bufferPtr != NULL)
			bufferPtr[writingHead++] = (char) charRead; //Write the char read into out buffer.
		if(charRead == '\n') //If it's the end of the line, get out of the while.
			break;
		charRead = fgetc(file); //Read first char.
	}
	
	bufferPtr[writingHead++] = '\0'; //Write the terminating byte.
	*linePtr = bufferPtr; //Set the buffer at being pointed by linePtr.
	*lineSize = size; //Set size being pointer by lineSize.
	
	return writingHead - 1; //Return the length of the read string, not counting the terminating byte.
}
