/**
 ** Jack Burns and Ryan Ghayour
 ** 
 ** um-operations.h
 ** April 15 2020
 ** Purpose: Interface for um operations
 **/

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <bitpack.h>
#include "um_mem.h"

#ifndef UM_OPERATIONS_H
#define UM_OPERATIONS_H

/* Takes in a pointer to the um's main memory, as well as three indexes 
 * indicating register numbers. Function will move the value in reg index
 * b into reg a if the value in reg c is not zero. It is a checked runtime 
 * error that  a, b, and c are between [0 -7] 
 */
void cMove(umStorage *mem, int a, int b, int c);

/* Takes in a pointer to the um's main memory, as well as three indices 
 * indicating register numbers. Function will take the word from address
 * contained in b offset by the value in c (from um mem) and load it in
 * reg a. It is a checked runtime error that  a, b, and c are between [0 -7]. 
 */
void segLoad(umStorage *mem, int a, int b, int c);

/* Takes in a pointer to the um's main memory, as well as three indices 
 * indicating register numbers. Function will store what is in register c
 * at address contained in b offset by the value of c (um mem). It is a
 * checked runtime error that  a, b, and c are between [0 -7] 
 */
void segStore(umStorage *mem, int a, int b, int c);

/* Takes in a pointer to the um's main memory, as well as three indices 
 * indicating register numbers. Functin will add the value in reg b to reg c
 * and put the sum mod 2^32 in register a. It is a checked runtime error that
 * a, b, and c are between [0 -7] 
 */
void add(umStorage *mem, int a, int b, int c);

/* Takes in a pointer to the um's main memory, as well as three indices 
 * indicating register numbers. Function will multiply the value in reg b and
 * reg c and put the product mod 2^32 in register a. It is a checked runtime
 * error that  a, b, and c are between [0 -7] 
 */
void multiply(umStorage *mem, int a, int b, int c);

/* Takes in a pointer to the um's main memory, as well as three indices 
 * indicating register numbers. Function will divide the value in reg b by
 * reg c and put the qutient mod in register a. It is a checked runtime
 * error that  a, b, and c are between [0 -7] 
 */
void divide(umStorage *mem, int a, int b, int c);

/* Takes in a pointer to the um's main memory, as well as three indices 
 * indicating register numbers. Function will perform a bitwise AND on values
 * in reg b and c, flip the bits, and store it in reg a. It is a checked
 * runtime error that  a, b, and c are between [0 -7] 
 */
void nand(umStorage *mem, int a, int b, int c);

/* Takes in a pointer to the um's main memory, as well as two indices 
 * indicating register numbers. Function will create a new segment in um memory
 * with size equal to value in reg c, and its address is store in reg b. It is
 * a checked runtime error that b and c are between [0 -7] 
 */
void mapSegment(umStorage *mem, int b, int c);

/* Takes in a pointer to the um's main memory, as well as an index 
 * indicating a register number. Function will unmap the segment whose index
 * is equal to the value in reg c. It is a checked runtime error that c is
 * between [0 -7] 
 */
void unmapSegment(umStorage *mem, int c);

/* Takes in a pointer to the um's main memory, as well as an index 
 * indicating a register number. Function will print the ascii value in reg
 * c (0-255). It is a checked runtime error that c is between [0 -7] 
 */
void output(umStorage *mem, int c);

/* Takes in a pointer to the um's main memory, as well as an index 
 * indicating a register number. Function will take an input between [0-255]
 * and store the value in reg c. It is a checked runtime error that c is
 * between [0 -7] 
 */
void input(umStorage *mem, int c);

/* Takes in a pointer to the um's main memory, as well as two indices 
 * indicating register numbers. Function will create a duplicate of the value
 * in reg b and replace the program segment [0] with it. Program counter set to
 * point to the index equal to reg c.It is a checked runtime error that b and
 * c are between [0 -7] 
 */
void loadProgram(umStorage *mem, int b, int c);

/* Takes in a pointer to the um's main memory, an index indicating a register
 * number, and a 32 bit value. Function will set reg a to the 32 bit value. It
 * is a checked runtime error that a is between [0 -7] 
 */
void loadVal(umStorage *mem, uint32_t value, int a);

#endif