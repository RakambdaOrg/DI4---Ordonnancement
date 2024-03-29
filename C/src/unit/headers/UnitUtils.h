#ifndef UNIT
#define UNIT

#include <sys/param.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../prod/headers/Utils.h"

/**
 * Add a "breakpoint" and exit the program.
 *
 * @param str The error to print.
 */
void unit_error(char * str, ...);

/**
 * Add a "breakpoint".
 */
void unit_breakpoint();

/**
 * Test if two int arrays are the same.
 *
 * @param base The array containing expected values.
 * @param compare The array containing the computed values.
 * @param length The arrays length.
 * @return True if the same, False else.
 */
Bool unit_intArrayEquals(const int * base, const int * compare, int length);

/**
 * Test if two unsigned int arrays are the same.
 * @param base The array containing expected values.
 * @param compare The array containing the computed values.
 * @param length The arrays length.
 * @return True if the same, False else.
 */
Bool unit_uintArrayEquals(const unsigned int * base, const unsigned int * compare, int length);

#endif
