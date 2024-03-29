#include "headers/SortUnit.h"
#include "../prod/headers/Solution.h"
#include "../prod/headers/Parser.h"
#include "../prod/headers/Sort.h"
#include "headers/UnitUtils.h"

void sortUnit()
{
	Instance * instance = parser_readInstanceFromFile("./unitResources/Instance2.txt");
	Solution * solution = solution_create(instance);
	
	solution_moveTaskPack(solution, 3, 1);
	solution_moveTaskPack(solution, 4, 1);
	
	Solution * result = sort_swapDeliveries(solution, 1, 4);
	if(result == solution)
		unit_error("Sort 1: Swap - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 0 || solution_getTaskPack(result, 1) != 1 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 1 || solution_getTaskPack(result, 4) != 0)
		unit_error("Sort 2: Wrong swap");
	solution_destroy(result);
	
	result = sort_swapDeliveries(solution, 99, 4);
	if(result == solution)
		unit_error("Sort 2.5: Swap - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 0 || solution_getTaskPack(result, 1) != 0 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 1 || solution_getTaskPack(result, 4) != 1)
		unit_error("Sort 2.6: Wrong swap");
	solution_destroy(result);
	
	result = sort_swapDeliveries(solution, 0, 1);
	if(result == solution)
		unit_error("Sort 3: Swap - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 0 || solution_getTaskPack(result, 1) != 0 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 1 || solution_getTaskPack(result, 4) != 1)
		unit_error("Sort 4: Wrong swap");
	solution_destroy(result);
	
	solution_moveTaskPack(solution, 0, 2);
	result = sort_moveDeliveriesEBSR(solution, 1, 0);
	if(result == solution)
		unit_error("Sort 5: EBSR - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 2 || solution_getTaskPack(result, 1) != 0 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 1 || solution_getTaskPack(result, 4) != 1)
		unit_error("Sort 6: Wrong EBSR");
	solution_destroy(result);
	
	result = sort_moveDeliveriesEBSR(solution, 0, 1);
	if(result == solution)
		unit_error("Sort 7: EBSR - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 1 || solution_getTaskPack(result, 1) != 0 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 1 || solution_getTaskPack(result, 4) != 1)
		unit_error("Sort 8: Wrong EBSR");
	solution_destroy(result);
	
	result = sort_moveDeliveriesEBSR(solution, 0, 2);
	if(result == solution)
		unit_error("Sort 9: EBSR - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 0 || solution_getTaskPack(result, 1) != 0 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 1 || solution_getTaskPack(result, 4) != 1)
		unit_error("Sort 10: Wrong EBSR");
	solution_destroy(result);
	
	result = sort_moveDeliveriesEBSR(solution, 0, 3);
	if(result == solution)
		unit_error("Sort 11: EBSR - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 0 || solution_getTaskPack(result, 1) != 0 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 1 || solution_getTaskPack(result, 4) != 1)
		unit_error("Sort 12: Wrong EBSR");
	solution_destroy(result);
	
	result = sort_moveDeliveriesEBSR(solution, 1, 0xFFFFFFFF);
	if(result == solution)
		unit_error("Sort 13: EBSR - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 2 || solution_getTaskPack(result, 1) != 0 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 1 || solution_getTaskPack(result, 4) != 1)
		unit_error("Sort 14: Wrong EBSR");
	solution_destroy(result);
	
	result = sort_moveDeliveriesEBSR(solution, 99, 1);
	if(result == solution)
		unit_error("Sort 15: EBSR - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 2 || solution_getTaskPack(result, 1) != 0 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 1 || solution_getTaskPack(result, 4) != 1)
		unit_error("Sort 16: Wrong EBSR");
	solution_destroy(result);
	
	solution_moveTaskPack(solution, 3, 0);
	result = sort_moveDeliveriesEFSR(solution, 3, 0);
	if(result == solution)
		unit_error("Sort 17: EFSR - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 2 || solution_getTaskPack(result, 1) != 0 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 0 || solution_getTaskPack(result, 4) != 1)
		unit_error("Sort 18: Wrong EFSR");
	solution_destroy(result);
	
	result = sort_moveDeliveriesEFSR(solution, 3, 1);
	if(result == solution)
		unit_error("Sort 19: EFSR - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 2 || solution_getTaskPack(result, 1) != 0 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 1 || solution_getTaskPack(result, 4) != 1)
		unit_error("Sort 20: Wrong EFSR");
	solution_destroy(result);
	
	result = sort_moveDeliveriesEFSR(solution, 3, 3);
	if(result == solution)
		unit_error("Sort 21: EFSR - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 2 || solution_getTaskPack(result, 1) != 0 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 3 || solution_getTaskPack(result, 4) != 1)
		unit_error("Sort 21: Wrong EFSR");
	solution_destroy(result);
	
	result = sort_moveDeliveriesEFSR(solution, 3, 4);
	if(result == solution)
		unit_error("Sort 22: EFSR - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 2 || solution_getTaskPack(result, 1) != 0 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 3 || solution_getTaskPack(result, 4) != 1)
		unit_error("Sort 23: Wrong EFSR");
	solution_destroy(result);
	
	result = sort_moveDeliveriesEFSR(solution, 3, 0xFFFFFFFF);
	if(result == solution)
		unit_error("Sort 24: EFSR - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 2 || solution_getTaskPack(result, 1) != 0 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 3 || solution_getTaskPack(result, 4) != 1)
		unit_error("Sort 25: Wrong EFSR");
	solution_destroy(result);
	
	result = sort_moveDeliveriesEFSR(solution, 99, 1);
	if(result == solution)
		unit_error("Sort 26: EFSR - Solution is not duplicated");
	if(solution_getTaskPack(result, 0) != 2 || solution_getTaskPack(result, 1) != 0 || solution_getTaskPack(result, 2) != 0 || solution_getTaskPack(result, 3) != 0 || solution_getTaskPack(result, 4) != 1)
		unit_error("Sort 27: Wrong EFSR");
	solution_destroy(result);
	
	solution_destroy(solution);
	instance_destroy(instance);
}
