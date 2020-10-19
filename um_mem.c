/**
 ** Jack Burns and Ryan Ghayour
 ** 
 ** um-operations.h
 ** April 15 2020
 ** Purpose: Implementation for um memory.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "um_mem.h"

struct umStorage {
    Seq_T registers;
    Seq_T memory;
    Seq_T unmappedID;
    int counter;
}; 

umStorage* initialize_memory()
{
    umStorage *mem = malloc(sizeof(*mem));
    assert(mem != NULL);

    mem->registers = Seq_new(8);
    for (int i = 0; i < 8; i++) {
        uint32_t *value = malloc(sizeof(*value));
        assert(value != NULL);
        *value = 0;
        Seq_addhi(mem->registers, value);
    }
     
    mem->memory = Seq_new(0);
    mem->unmappedID = Seq_new(0);
    mem->counter = 0;

    return mem;
}

void release_memory(umStorage *mem)
{
    int length = Seq_length(mem->memory);
    /* free every sequence in memory pool */
    Seq_T temp;
    for (int i = 0; i < length; i++) {
        temp = Seq_get(mem->memory, i);
        if (temp == NULL) { 
            continue;
        }
        int tempSize = Seq_length(temp);
        /* free every word in every sequence */
        for (int j = 0; j < tempSize; j++) {
            free(Seq_get(temp, j));
        }
        Seq_free(&temp);
    }
    Seq_free(&(mem->memory));

    /* free every value in the registers */
    for (int i = 0; i < 8; i++) {
        free(Seq_get(mem->registers, i));
    }
    Seq_free(&(mem->registers));

    int unmappedSize = Seq_length(mem->unmappedID);
   // fprintf(stderr, "size = %d\n")
    for (int i = 0; i < unmappedSize; i++) {
        free(Seq_get(mem->unmappedID, i));
    }
    Seq_free(&(mem->unmappedID));

    free(mem);
}

uint32_t add_segment(Seq_T words, umStorage *mem)
{
    uint32_t identifier = 0;

    /* checks if unmapped segments can be remapped */
    if (Seq_length(mem->unmappedID) > 0) {
        uint32_t *id = (uint32_t*)Seq_remhi(mem->unmappedID);
        Seq_put(mem->memory, *id, words);
        identifier = *id;
        free(id);
    }       
    /* maps segment to the end of main memory */
    else {
        long length = Seq_length(mem->memory);
        assert(length < 4294967296);
        identifier = (uint32_t)length;
        Seq_addhi(mem->memory, words);
    }

    return identifier;
}

void remove_segment(umStorage *mem, uint32_t *identifier)
{
    Seq_addlo(mem->unmappedID, identifier);

    Seq_T unmapped = Seq_get(mem->memory, *identifier);
    int unmappedSize = Seq_length(unmapped);

    for (int i = 0; i < unmappedSize; i++) {
        free(Seq_get(unmapped, i));
    }

    Seq_free(&unmapped);
    Seq_put(mem->memory, *identifier, NULL);
}

int get_counter(umStorage *mem)
{
    mem->counter++;
    return mem->counter - 1;
}

Seq_T *get_main_mem(umStorage *mem)
{
    return &mem->memory;
}

uint32_t get_next_instruction(umStorage *mem)
{
    Seq_T segZero = Seq_get(mem->memory, 0);
    return *(uint32_t *)Seq_get(segZero, get_counter(mem));
}

uint32_t get_reg_val(umStorage *mem, int index)
{
    assert(index >= 0 && index < 8)
;   return *(uint32_t*)Seq_get(mem->registers, index);
}

void edit_register(umStorage *mem, int index, uint32_t value)
{
    assert(index >= 0 && index < 8);
    *(uint32_t*)Seq_get(mem->registers, index) = value;
}

void edit_counter(umStorage *mem, int setTo)
{
    int max = Seq_length(Seq_get(mem->memory, 0));
    assert(setTo >= 0 && setTo < max);
    mem->counter = setTo;
}