/**
 ** Jack Burns and Ryan Ghayour
 ** 
 ** um-operations.h
 ** April 15 2020
 ** Purpose: Interface for um memory
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "assert.h"
#include "seq.h"

#ifndef UM_MEM_H
#define UM_MEM_H

typedef struct umStorage umStorage;

/* Allocates space for the main memory used by the um. Initializes 8
 * registers, as well as the memory pool, and the list of unmapped identifiers,
 * and sets the program counter to zero. It is a checked runtime error that 
 * memory is able to be allocated.
 */
umStorage* initialize_memory();

/* Takes in a pointer to the um memory, and frees all associated memory
 * used by the um.
 */
void release_memory(umStorage *mem);

/* Takes in apointer to um memory, and a sequence of words to be stored. 
 * Adds a new segment containing words to the back of the memory pool, or to 
 * the index of the last unmapped segment.
 */
uint32_t add_segment(Seq_T words, umStorage *mem);

/* Takes in a poiter to the um memory and the identifier corrosponding to the 
 * segment to be unmapped. Function allows identifier to be mapped over the 
 * next time a new segment is mapped.
 */
void remove_segment(umStorage *mem, uint32_t *identifier);

/* Takes in a pointer to the um memory. Function returns the index of segment
 * zero that the counter is pointing to, and increments the counter.
 */
int get_counter(umStorage *mem);

/* Takes in a pointer to the um memory. Function returns the main memory field
 * in the um memory.
 */
Seq_T *get_main_mem(umStorage *mem);

/* Takes in a pointer to the um memory. Function returns the next word in the 
 * 0th element of main memory, which is the next coded instrucion.
 */
uint32_t get_next_instruction(umStorage *mem);

/* Takes in a pointer to the um memory, and the index of a register. Function
 * returns the value in the register with the corrosponding index. It is a
 * checked runtime error that the index is between [0-7].
 */
uint32_t get_reg_val(umStorage *mem, int index);

/* Takes in a pointer to the um memory, the index of a register, and a 32 bit
 * value. Function stores the 32 bit value into the register indicated by the
 * index. It is a checked runtime error that the index is between [0-7].
 */
void edit_register(umStorage *mem, int index, uint32_t value);

/* Takes in a pointer to the um memory, and the value the counter will be set
 * to. Function sets the program counter the indicated value. It is a checked
 * runtime error to set the counter to a negative number or a value greater
 * than the size of the 0th index.
 */
void edit_counter(umStorage *mem, int setTo);

#endif