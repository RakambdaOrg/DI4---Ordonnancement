#ifndef TABOU_FLAGS_H
#define TABOU_FLAGS_H

//!Logs the score of each iteration.
#define DEBUG_ACTIVATED 0
#define DEV_LOG_SCORE 1
#define DEV_LOG_SCORE_FULL 0

//!Maximum iterations for the Tabu algorithm.
#define TABU_ITERATIONS 20000
//!If no improvement was found, try a new, different solution.
#define TABU_DIVERSIFICATION 1
//!Maximum size of the Tabu list.
#define TABU_LIST_SIZE 5

//!Use EBSR for Tabu.
#define TABU_SEARCH_EBSR 1
//!Use EFSR for Tabu.
#define TABU_SEARCH_EFSR 1
//!Use swap for Tabu.
#define TABU_SEARCH_SWAP 1

//!2-OPT
#define TABU_2OPT 0
//!Selects the first better solution instead of the best solution.
#define TABU_FIRST_IMPROVE 0
//!Prevents swapping if already swapped the inverse.
#define TABU_LOGIC 1
//!Maximum batch gap between swaps, EBSRs, EFSRs.
#define TABU_DELTA_BATCH 4
//!Change the current solution after each method used.
#define TABU_PROPAGATE 1

//!Use the score cache system.
#define CACHED_SCORE 1
//!Sort deliveries using the given method. 0 -> Due date; 1 -> Nearest neighbor.
#define DELIVERY_METHOD 0
//!Random diversification.
#define TABU_RANDOM 1

//!Seed for random number generation. Value can be fixed for tests.
#define SEED 42
#if SEED
#else
	#define SEED time(NULL)
#endif

#endif //TABOU_FLAGS_H
