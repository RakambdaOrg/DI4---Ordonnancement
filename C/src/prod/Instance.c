#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "headers/Instance.h"

Instance * instance_create()
{
	Instance * instance;
	MMALLOC(instance, Instance, 1, "parser_readInstanceFromFile");
	instance->machineCount = 0;
	instance->taskCount = 0;
	instance->distancesMatrix = NULL;
	instance->tasks = NULL;
	instance->origin = NULL;
	debugPrint("Instance created : %p\n", instance);
	return instance;
}

void instance_destroy(Instance * instance)
{
	if(instance == NULL)
		return;
	
	//Free the distance matrix.
	if(instance->distancesMatrix != NULL)
		for(task_t i = 0; i <= instance->taskCount; i++)
			free(instance->distancesMatrix[i]);
	free(instance->distancesMatrix);
	
	//Free the tasks.
	if(instance->tasks != NULL)
		for(task_t i = 0; i < instance->taskCount; i++)
			task_destroy(instance->tasks[i]);
	free(instance->tasks);
	
	free(instance);
	debugPrint("Instance destroyed : %p\n", instance);
}

unsigned int instance_getDueDate(Instance * instance, task_t task)
{
	if(task > instance->taskCount)
		fatalError("instance_getDueDate : Missing task %d (Only %d tasks).\n", task, instance->taskCount);
	return instance->tasks[task]->dueDate;
}

unsigned int * instance_sortByDueDate(Instance * instance)
{
	// Initializing a new array
	unsigned int * sorted = NULL;
	MMALLOC(sorted, task_t, instance->taskCount, "instance_sortByDueDate");
	for(unsigned int i = 0; i < instance->taskCount; i++)
		sorted[i] = i;
	
	// Bubble sort
	for(unsigned int i = 0; i < instance->taskCount - 1; i++)
		for(unsigned int j = 0; j < instance->taskCount - i - 1; j++)
			if(instance_getDueDate(instance, sorted[j]) > instance_getDueDate(instance, sorted[j + 1]))
			{
				task_t temp = sorted[j];
				sorted[j] = sorted[j + 1];
				sorted[j + 1] = temp;
			}
	return sorted;
}

void instance_print(Instance * instance)
{
	printf("Instance %s:\n\tMachine count : %d\n\tTask count : %d\n\tTasks :\n\t\t     ", instance->origin, instance->machineCount, instance->taskCount);
	// Tasks
	for(task_t i = 0; i < instance->taskCount; i++)
		printf("T%-3d", i);
	for(machine_t i = 0; i < instance->machineCount; i++)
	{
		printf("\n\t\tM%-4d", i);
		for(task_t j = 0; j < instance->taskCount; j++)
			printf("%-4d", task_getMachineDuration(instance->tasks[j], i));
	}
	//Due dates
	printf("\n\tDue :\n\t\t     ");
	for(task_t i = 0; i < instance->taskCount; i++)
		printf("%-4d", task_getDueDate(instance->tasks[i]));
	// Distances
	printf("\n\tDistances :\n\t\t     ");
	for(task_t i = 0; i <= instance->taskCount; i++)
	{
		if(i == instance->taskCount)
			printf("ORG");
		else
			printf("T%-3d", i);
	}
	for(task_t i = 0; i <= instance->taskCount; i++)
	{
		if(i == instance->taskCount)
			printf("\n\t\tORG  ");
		else
			printf("\n\t\tT%-4d", i);
		for(task_t j = 0; j <= instance->taskCount; j++)
			printf("%-4d", instance_getDistance(instance, i, j));
	}
	printf("\n");
}
