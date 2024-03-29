#include <sys/timeb.h>

#include "headers/Tabu.h"
#include "FLAGS.h"
#include "headers/Sort.h"

unsigned int TABU_DELTA;
unsigned int TABU_ITERATIONS_NOIMPROVE;

TabuSolution * tabuSolution_create(Solution * solution, unsigned int iterations, double time)
{
	TabuSolution * tabuSolution = NULL;
	MMALLOC(tabuSolution, TabuSolution, 1, "tabu_search");
	tabuSolution->solution = solution;
	tabuSolution->iterations = iterations;
	tabuSolution->time = time;
	return tabuSolution;
}

void tabuSolution_destroy(TabuSolution * solution)
{
	solution_destroy(solution->solution);
	free(solution);
}

Solution * tabu_solutionInit(Instance * instance)
{
	debugPrint("Creating initial solution for instance %p\n", instance);
	Solution * bestSolution = NULL;
	task_t * sortedDueDates = instance_sortByDueDate(instance);
	unsigned int maxTasksPerPack = 1;
	while(maxTasksPerPack <= instance->taskCount / 2) //Try every size of packs.
	{
		debugPrint("\tCreating packs of size %d\n", maxTasksPerPack);
		Solution * solution = solution_create(instance);
		for(unsigned int i = 0; i < instance->taskCount; i++) //Move tasks to do packs of max maxTasksPerPack tasks per pack.
			solution_moveTaskPack(solution, sortedDueDates[i], i / maxTasksPerPack);
		if(bestSolution == NULL || solution_eval(solution)->score < solution_eval(bestSolution)->score)
		{
			solution_destroy(bestSolution);
			bestSolution = solution;
		}
		else
			solution_destroy(solution);
		maxTasksPerPack++;
	}
	free(sortedDueDates);
	debugPrint("Initial solution is %p\n", bestSolution);
	return bestSolution;
}

double tabu_getTimeDiff(struct timeb start, struct timeb end)
{
	return end.time - start.time + (end.millitm - start.millitm) / 1000.0f;
}

int tabu_flagsFingerprint()
{
	int result = 0;
	if(TABU_SEARCH_SWAP)
		result += 1;
	if(TABU_SEARCH_EBSR)
		result += 10;
	if(TABU_SEARCH_EFSR)
		result += 100;
	if(TABU_DIVERSIFICATION)
		result += 1000;
	if(TABU_FIRST_IMPROVE)
		result += 10000;
	return result;
}

void tabu_setConstants(Instance * instance)
{
	switch(instance->taskCount)
	{
		case 100:
			TABU_DELTA = 8;
			TABU_ITERATIONS_NOIMPROVE = 5;
			break;
		case 50:
			TABU_DELTA = 10;
			TABU_ITERATIONS_NOIMPROVE = 10;
			break;
		default:
			TABU_DELTA = instance->taskCount / 4;
			TABU_ITERATIONS_NOIMPROVE = 15;
			break;
	}
}

TabuSolution * tabu_search(Instance * instance)
{
	struct timeb timeStart;
	ftime(&timeStart);
	
	Solution * currentSolution = tabu_solutionInit(instance);
	struct timeb timeNow;
	ftime(&timeNow);
	printf("Initial solution found in %fs:\n", tabu_getTimeDiff(timeStart, timeNow));
	solution_print(currentSolution);
#if DEV_LOG_SCORE
#if DEV_LOG_SCORE_FULL
	FILE * logScoreFile = fopen("./log/logScores.csv", "w");
	fprintf(logScoreFile, "%s,%s\n", "C_BestEver", "C_BestIter");
	
	FILE * logScoreFullFile = fopen("./log/logScoresFull.csv", "w");
	fprintf(logScoreFullFile, "%s,%s,%s\n", "C_Iter", "C_Current", "C_Score");
	fprintf(logScoreFullFile, "0,");
	solution_printCSV(currentSolution, logScoreFullFile);
	fprintf(logScoreFullFile, ",%u\n", solution_eval(currentSolution)->score);
#endif
	char filenameCompact[512];
	sprintf(filenameCompact, "./log/logScoresCCompact_%s_%d.csv", instance->origin, tabu_flagsFingerprint());
	FILE * logScoreCompactFile = fopen(filenameCompact, "w");
	fprintf(logScoreCompactFile, "%s,%s,%s\n", "C_Iter", "C_Score", "C_Time");
#endif
	Solution * bestSolution = NULL;
	Solution * bestBestSolution = solution_copy(currentSolution);
	solution_eval(currentSolution);
	unsigned int nbIterations = 0;
	unsigned int nbNoBetterIterations = 0;
	Bool diversification = False;
	TabuList * tabuList = tabuList_create(TABU_LIST_SIZE);
	
	tabu_setConstants(instance);
	
	double timeLimit = (instance->taskCount * instance->machineCount) / 4.0;
	double currentTime = 0;
	
	int searchFunctionAvailable = 3;
	searchFunction searchFunctions[searchFunctionAvailable];
	searchFunctions[0] =
#if TABU_SEARCH_SWAP
			tabu_searchSwap
#else
			NULL
#endif
			;
	
	searchFunctions[1] =
#if TABU_SEARCH_EBSR
			tabu_searchEBSR
#else
			NULL
#endif
			;
	
	searchFunctions[2] =
#if TABU_SEARCH_EFSR
			tabu_searchEFSR
#else
			NULL
#endif
	;

#if DEV_LOG_SCORE
	fprintf(logScoreCompactFile, "%u,%u,%f\n", 0, solution_eval(currentSolution)->score, tabu_getTimeDiff(timeStart, timeNow));
#endif
	printf("Time limit for this instance is: %f\n", timeLimit);
	while((currentTime = tabu_getTimeDiff(timeStart, timeNow)) < timeLimit && nbIterations < TABU_ITERATIONS)
	{
		debugPrint("----- Tabu iteration %d on solution %p\n", nbIterations, currentSolution);
		SearchResult * bestMethodResult = NULL;
		for(int i = 0; i < searchFunctionAvailable; i++)
		{
			if(searchFunctions[i] == NULL)
				continue;
			SearchResult * iterSolution = searchFunctions[i](currentSolution, tabuList, diversification);
			if(iterSolution != NULL && iterSolution->tabuItem != NULL)
			{
				if(bestMethodResult != NULL)
				{
					if(solution_eval(bestMethodResult->solution)->score > solution_eval(iterSolution->solution)->score)
					{
						searchResult_destroy(bestMethodResult);
						bestMethodResult = iterSolution;
					}
					else
						searchResult_destroy(iterSolution);
				}
				else
					bestMethodResult = iterSolution;
#if TABU_PROPAGATE
				solution_destroy(currentSolution);
				currentSolution = solution_copy(bestMethodResult->solution);
#endif
			}
			else
				searchResult_destroy(iterSolution);
		}
		
#if TABU_DIVERSIFICATION
		if(diversification == True)
		{
			printf("Diversification !\n");
			diversification = False;
			nbNoBetterIterations = 0;
		}
#endif
		
		if(bestMethodResult != NULL && bestMethodResult->solution != NULL)
		{
			if(bestMethodResult->tabuItem != NULL)
			{
				tabuList_addItem(tabuList, tabuItem_copy(bestMethodResult->tabuItem));
			}
			if(solutionCompare(bestSolution, bestMethodResult->solution, diversification) < 0)
			{
				debugPrint("Found better solution %p, replacing %p\n", bestMethodResult, bestSolution);
				printf("New better: %d\n", solution_eval(bestMethodResult->solution)->score);
				if(bestSolution != currentSolution)
				{
					solution_destroy(bestSolution);
				}
				solution_destroy(currentSolution);
				bestSolution = solution_copy(bestMethodResult->solution);
				nbNoBetterIterations = 0;
#if DEV_LOG_SCORE
				if(solutionCompare(bestBestSolution, bestSolution, False) < 0)
				{
					ftime(&timeNow);
					fprintf(logScoreCompactFile, "%u,%u,%f\n", nbIterations + 1, solution_eval(bestSolution)->score, tabu_getTimeDiff(timeStart, timeNow));
				}
#endif
			}
			else
			{
				if(currentSolution != bestSolution)
					solution_destroy(currentSolution);
				nbNoBetterIterations++;
			}
			currentSolution = solution_copy(bestMethodResult->solution);
			//tabuList_print(tabuList);
			//printf("\n");
		}
		else
		{
#if TABU_DIVERSIFICATION
			diversification = True;
			tabuList_clear(tabuList);
			nbNoBetterIterations++;
#endif
		}

#if TABU_DIVERSIFICATION
		if(nbNoBetterIterations > TABU_ITERATIONS_NOIMPROVE)
		{
#if TABU_RANDOM
			tabu_randomize(currentSolution);
			nbNoBetterIterations = 0;
#else
			diversification = True;
#endif
			tabuList_clear(tabuList);
			if(solutionCompare(bestBestSolution, bestSolution, False) < 0)
			{
				solution_destroy(bestBestSolution);
				bestBestSolution = bestSolution;
				printf("New best best : %d\n", solution_eval(bestBestSolution)->score);
			}
			else
				solution_destroy(bestSolution);
			bestSolution = NULL;
		}
#endif

#if DEV_LOG_SCORE
#if DEV_LOG_SCORE_FULL
		fprintf(logScoreFile, "%u,%u\n", (bestSolution == NULL) ? UINT_MAX : solution_eval(bestSolution)->score, (bestMethodResult == NULL || bestMethodResult->solution == NULL) ? UINT_MAX : solution_eval(bestMethodResult->solution)->score);
		fprintf(logScoreFullFile, "%u,", nbIterations + 1);
		solution_printCSV(currentSolution, logScoreFullFile);
		fprintf(logScoreFullFile, ",%u\n", solution_eval(currentSolution)->score);
#endif
#endif
		searchResult_destroy(bestMethodResult);
		nbIterations++;
		ftime(&timeNow);
	}
	if(solutionCompare(bestBestSolution, bestSolution, False) < 0)
	{
		solution_destroy(bestBestSolution);
		bestBestSolution = bestSolution;
	}
	else
		solution_destroy(bestSolution);

#if DEV_LOG_SCORE
	fprintf(logScoreCompactFile, "%u,%u,%f\n", nbIterations + 1, solution_eval(bestBestSolution)->score, tabu_getTimeDiff(timeStart, timeNow));
#if DEV_LOG_SCORE_FULL
	fclose(logScoreFile);
	
	fclose(logScoreFullFile);
#endif
	fclose(logScoreCompactFile);
#endif
	debugPrint("Iterated %d times, with best solution %p\n", nbIterations, bestBestSolution);
	tabuList_destroy(tabuList);
	if(currentSolution != bestSolution)
		solution_destroy(currentSolution);
	return tabuSolution_create(bestBestSolution, nbIterations, currentTime);
}

SearchResult * tabu_searchSwap(Solution * currentSolution, TabuList * tabuList, Bool diversification)
{
	if(currentSolution == NULL)
		return searchResult_create(NULL, NULL, SWAP);
	Solution * bestSolution = NULL;
	TabuItem * bestItem = NULL;
#if TABU_FIRST_IMPROVE
	Bool first_done = False;
#endif
	for(unsigned int packIndex1 = 0; packIndex1 < currentSolution->packCount; packIndex1++)
	{
		for(unsigned int taskIndex1 = 0; taskIndex1 < currentSolution->packList[packIndex1]->taskCount; taskIndex1++)
		{
			task_t task1 = currentSolution->packList[packIndex1]->tasks[taskIndex1];
			for(unsigned int packIndex2 = packIndex1 + 1; packIndex2 < currentSolution->packCount; packIndex2++)
			{
				for(unsigned int taskIndex2 = 0; taskIndex2 < currentSolution->packList[packIndex2]->taskCount; taskIndex2++)
				{
					task_t task2 = currentSolution->packList[packIndex2]->tasks[taskIndex2];
					if(task1 == task2)
						continue;
					if((taskIndex2 > taskIndex1 ? taskIndex2 - taskIndex1 + 1 : taskIndex1 - taskIndex2 + 1) > TABU_DELTA)
						continue;
					Solution * newSolution = sort_swapDeliveries(currentSolution, task1, task2);
					TabuItem * item = tabuItem_create(task1, task2, SWAP);
					if(!tabuList_contains(tabuList, item) && (bestSolution == NULL || solutionCompare(bestSolution, newSolution, diversification) < 0))
					{
						debugPrint("Solution %p is better than %p with swapped values %d and %d\n", currentSolution, newSolution, task1, task2);
						solution_destroy(bestSolution);
						tabuItem_destroy(bestItem);
						bestItem = item;
						bestSolution = newSolution;
#if TABU_FIRST_IMPROVE
						first_done = True;
#endif
					}
					else
					{
						solution_destroy(newSolution);
						tabuItem_destroy(item);
					}
#if TABU_FIRST_IMPROVE
					if(first_done == True)
						break;
#endif
				}
#if TABU_FIRST_IMPROVE
				if(first_done == True)
					break;
#endif
			}
#if TABU_FIRST_IMPROVE
			if(first_done == True)
				break;
#endif
		}
#if TABU_FIRST_IMPROVE
		if(first_done == True)
			break;
#endif
	}
	return searchResult_create(bestSolution, bestItem, SWAP);
}

SearchResult * tabu_searchEBSR(Solution * currentSolution, TabuList * tabuList, Bool diversification)
{
	if(currentSolution == NULL)
		return searchResult_create(solution_copy(currentSolution), NULL, EBSR);
	Bool stop = False;
	unsigned int packIndex1 = 0;
	Solution * bestSol = NULL;
	TabuItem * bestTabuItem = NULL;
	while(packIndex1 < currentSolution->packCount - 1 && !stop)
	{
		unsigned int packIndex2 = packIndex1 + 1;
		while(packIndex2 < currentSolution->packCount && packIndex2 <= packIndex1 + TABU_DELTA_BATCH && !stop)
		{
			for(unsigned int packElemIndex = 0; packElemIndex < currentSolution->packList[packIndex2]->taskCount; packElemIndex++)
			{
				if(!stop)
				{
					task_t taskID = currentSolution->packList[packIndex2]->tasks[packElemIndex];
					Solution * neighbor = sort_moveDeliveriesEBSR(currentSolution, taskID, packIndex2 - packIndex1);
					TabuItem * tabuItem = tabuItem_create(packIndex1, taskID, EBSR);
					if(!tabuList_contains(tabuList, tabuItem) && (bestSol == NULL || solutionCompare(bestSol, neighbor, diversification) < 0))
					{
						solution_destroy(bestSol);
						tabuItem_destroy(bestTabuItem);
						bestSol = neighbor;
						bestTabuItem = tabuItem;
#if TABU_FIRST_IMPROVE
						stop = True;
#endif
					}
					else
					{
						solution_destroy(neighbor);
						tabuItem_destroy(tabuItem);
					}
				}
			}
			packIndex2++;
		}
		packIndex1++;
	}
	return searchResult_create((bestSol == NULL ? solution_copy(currentSolution) : bestSol), bestTabuItem, EBSR);
}

SearchResult * tabu_searchEFSR(Solution * currentSolution, TabuList * tabuList, Bool diversification)
{
	if(currentSolution == NULL)
		return searchResult_create(solution_copy(currentSolution), NULL, EFSR);
	Bool stop = False;
	unsigned int packIndex1 = 0;
	Solution * bestSol = NULL;
	TabuItem * bestTabuItem = NULL;
	while(packIndex1 < currentSolution->packCount - 1 && !stop)
	{
		for(unsigned int packElemIndex = 0; packElemIndex < currentSolution->packList[packIndex1]->taskCount; packElemIndex++)
		{
			task_t taskID = currentSolution->packList[packIndex1]->tasks[packElemIndex];
			unsigned int packIndex2 = packIndex1 + 1;
			while(packIndex2 < currentSolution->packCount && packIndex2 <= packIndex1 + TABU_DELTA_BATCH && !stop)
			{
				Solution * neighbor = sort_moveDeliveriesEFSR(currentSolution, taskID, packIndex2 - packIndex1);
				TabuItem * tabuItem = tabuItem_create(taskID, packIndex2, EFSR);
				if(!tabuList_contains(tabuList, tabuItem) && (bestSol == NULL || solutionCompare(bestSol, neighbor, diversification) < 0))
				{
					solution_destroy(bestSol);
					tabuItem_destroy(bestTabuItem);
					bestSol = neighbor;
					bestTabuItem = tabuItem;
#if TABU_FIRST_IMPROVE
					stop = True;
#endif
				}
				else
				{
					solution_destroy(neighbor);
					tabuItem_destroy(tabuItem);
				}
				packIndex2++;
			}
		}
		packIndex1++;
	}
	return searchResult_create((bestSol == NULL ? solution_copy(currentSolution) : bestSol), bestTabuItem, EFSR);
}

void tabu_randomize(Solution * solution)
{
	printf("Diversification !\n");
	unsigned int nbSwaps = RRAND(3, solution->packCount * 3);
	for(unsigned int i = 0; i < nbSwaps; i++)
	{
		task_t task1 = RRAND(0, solution->instance->taskCount);
		task_t task2 = RRAND(0, solution->instance->taskCount);
		solution_switchTaskPack(solution, task1, task2);
	}
//#if TABU_SEARCH_EBSR
//	unsigned int nbEBSR = RRAND(3, solution->packCount * 3);
//	for(unsigned int i = 0; i < nbEBSR; i++)
//	{
//		task_t task1 = RRAND(0, solution->instance->taskCount);
//		unsigned int p = solution_getTaskPack(solution, task1);
//		unsigned int pack = RRAND(0, p);
//		solution_moveTaskPack(solution, task1, pack);
//	}
//#endif
//#if TABU_SEARCH_EFSR
//	unsigned int nbEFSR = RRAND(3, solution->packCount * 3);
//	for(unsigned int i = 0; i < nbEFSR; i++)
//	{
//		task_t task1 = RRAND(0, solution->instance->taskCount);
//		unsigned int p = solution_getTaskPack(solution, task1);
//		unsigned int pack = RRAND(p, solution->packCount);
//		solution_moveTaskPack(solution, task1, pack);
//	}
//#endif
}
