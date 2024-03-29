#ifndef TABOU_SOLUTION_H
#define TABOU_SOLUTION_H

struct _Solution;

#include "SolutionInfo.h"
#include "Sequencer.h"
#include "Pack.h"

//!Contains the solution being computed.
typedef struct _Solution
{
	//!Reference to the instance.
	Instance * instance;
	//!Number of packs in this solution.
	unsigned int packCount;
	//!Ordered list of the packs in shipping order.
	Pack ** packList;
	//!The solution value.
	struct _SolutionInfo * info;
} Solution;

/**
 * Creates an empty solution.
 *
 * @param instance The instance the solution is for. Not null.
 * @return A new solution.
 * @remark Needs to be freed with solution_destroy.
*/
Solution * solution_create(Instance * instance);

/**
 * Frees a solution.
 *
 * @param solution The solution to destroy.
*/
void solution_destroy(Solution * solution);

/**
 * Copies a solution.
 *
 * @param solution The solution to copy.
 * @return A new solution.
 * @remark Needs to be freed with solution_destroy.
 */
Solution * solution_copy(Solution * solution);

/**
 * Gets the corresponding pack index for the given task.
 *
 * @param solution The solution. Not null.
 * @param task The task to search.
 * @return The index of the pack containing the task. If the task doesn't exist, returns -1. If the task exists, but is not in any pack, a fatal error is thrown (and it returns value -1).
 */
int solution_getTaskPack(Solution * solution, task_t task);

/**
 * Assigns a pack to the given task.
 * If the task doesn't exist, nothing is done.
 * If the pack doesn't exist, we create it and move the task only if it is the n+1 pack.
 *
 * @param solution The solution. Not null.
 * @param task The task.
 * @param pack The pack to assign.
 */
void solution_moveTaskPack(Solution * solution, task_t task, unsigned int pack);

/**
 * Switches two tasks between two packs.
 * If one of the tasks isn't in the pack, nothing is done.
 *
 * @param solution The solution. Not null.
 * @param task1 The first task.
 * @param task2 The second task.
 */
void solution_switchTaskPack(Solution * solution, task_t task1, task_t task2);

/**
 * Evaluates the score of the given solution and saves it in the info.
 * If the score has already been computed and CACHED_SCORE is set to 1, the score is not computed again.
 *
 * @param solution The solution. Not null.
 * @return The info of the solution.
 * @remark Needs to be freed with solutionInfo_destroy.
 */
struct _SolutionInfo * solution_eval(Solution * solution);

/**
 * Prints the solution.
 *
 * @param solution The solution.
 */
void solution_print(Solution * solution);

/**
 * Create a string to save the solution in a csv.
 *
 * @param solution The solution.
 * @param file The file to write to.
 * @remark The file must be open.
 */
void solution_printCSV(Solution * solution, FILE * file);

/**
 * Saves the solution to a file.
 *
 * @param file The file to write to, opened.
 * @param solution The solution to write.
 * @param time The time taken by the tabu algorithm.
 */
void solution_save(FILE * file, Solution * solution, double time);

/**
 * Compare two solutions based on their score.
 *
 * @param solution1 The first solution to compare.
 * @param solution2 The second to compare.
 * @param diversification Equivalent of doing (*-1) to the output.
 * @return Positive if S1 have a bigger score than S2.
 */
long solutionCompare(Solution * solution1, Solution * solution2, Bool diversification);

#endif //TABOU_SOLUTION_H
