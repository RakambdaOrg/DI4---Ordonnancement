#ifndef TABOU_FLAGS_H
#define TABOU_FLAGS_H

#define DELTA_NB_BATCH 4
#define NB_ITERATION_MAX 2000
#define AVEC_DIVERSIFICATION 1
#define TAILLE_TABOU 5

#define TABU_SEARCH_EBSR 1
#define TABU_SEARCH_EFSR 1
#define TABU_SEARCH_SWAP 1
#define FLAG_2OPT 0
#define FLAG_FIRST_IMPROVE 1
#define FLAG_TABOU_LOGIC 0

#define CACHED_SCORE 1 //Use the score cache system.
#define DELIVERY_NEAREST_NEIGHBOR 1 //Sort deliveries using the nearest neighbor, else sort by lowest due date first.

#define SEED 42
#ifndef SEED
	#define SEED time(NULL)
#endif

#endif //TABOU_FLAGS_H
