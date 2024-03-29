#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "headers/ParserUnit.h"
#include "headers/InstanceUnit.h"
#include "headers/TaskUnit.h"
#include "headers/PackUnit.h"
#include "headers/SolutionUnit.h"
#include "headers/SequencerUnit.h"
#include "headers/SolutionInfoUnit.h"
#include "../prod/headers/Utils.h"
#include "headers/SortUnit.h"
#include "headers/TabuListUnit.h"
#include "headers/SearchResultUnit.h"

extern Bool UNIT_FAILED;
extern Bool DEBUG;

int mainUnit()
{
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	
	char * startTest = "\nUNIT: -------------------------------------------------------------------------\n";
	char * endTest = "UNIT: -------------------------------------------------------------------------\n";
	
	printf("%s", startTest);
	printf("UNIT: Executing Task tests...\n");
	taskUnit();
	printf("UNIT: OK\n");
	printf("%s", endTest);
	printf("%s", startTest);
	printf("UNIT: Executing Instance tests...\n");
	instanceUnit();
	printf("UNIT: OK\n");
	printf("%s", endTest);
	printf("%s", startTest);
	printf("UNIT: Executing Parser tests...\n");
	parserUnit();
	printf("UNIT: OK\n");
	printf("%s", endTest);
	
	printf("%s", startTest);
	printf("UNIT: Executing Pack tests...\n");
	packUnit();
	printf("UNIT: OK\n");
	printf("%s", endTest);
	printf("%s", startTest);
	printf("UNIT: Executing SolutionInfo tests...\n");
	solutionInfoUnit();
	printf("UNIT: OK\n");
	printf("%s", endTest);
	printf("%s", startTest);
	printf("UNIT: Executing Solution tests...\n");
	solutionUnit();
	printf("UNIT: OK\n");
	printf("%s", endTest);
	
	printf("%s", startTest);
	printf("UNIT: Executing Sequencer tests...\n");
	sequencerUnit();
	printf("UNIT: OK\n");
	printf("%s", endTest);
	printf("%s", startTest);
	printf("UNIT: Executing Sort tests...\n");
	sortUnit();
	printf("UNIT: OK\n");
	printf("%s", endTest);
	
	printf("UNIT: Executing TabuList tests...\n");
	tabuListUnit();
	printf("UNIT: OK\n");
	printf("%s", endTest);
	printf("UNIT: Executing SearchResult tests...\n");
	searchResultUnit();
	printf("UNIT: OK\n");
	printf("%s", endTest);
	
	if(UNIT_FAILED)
	{
		fprintf(stdout, "UNIT: TESTS FAILED\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("UNIT: TESTS OK - CONGRATULATIONS\n");
	return 0;
}
