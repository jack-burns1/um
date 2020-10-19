/**
 ** Jack Burns and Ryan Ghayour
 ** 
 ** um-operations.h
 ** April 15 2020
 ** Purpose: Implementation for um operations
 **/

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <bitpack.h>
#include "um_operations.h"

void cMove(umStorage *mem, int a, int b, int c)
{
    assert(a >= 0 && a < 8);
    assert(b >= 0 && b < 8);
    assert(c >= 0 && c < 8);

    if (get_reg_val(mem, c) != 0){
        uint32_t toSet = get_reg_val(mem, b);
        edit_register(mem, a, toSet);
    }
}

void segLoad(umStorage *mem, int a, int b, int c)
{
    assert(a >= 0 && a < 8);
    assert(b >= 0 && b < 8);
    assert(c >= 0 && c < 8);

    uint32_t rb = get_reg_val(mem, b);
    uint32_t rc = get_reg_val(mem, c);
    Seq_T segToRead = Seq_get(*(Seq_T*)get_main_mem(mem), rb);
    uint32_t toLoad = *(uint32_t*)Seq_get(segToRead, rc);
    edit_register(mem, a, toLoad);
}

void segStore(umStorage *mem, int a, int b, int c)
{
    assert(a >= 0 && a < 8);
    assert(b >= 0 && b < 8);
    assert(c >= 0 && c < 8);

    uint32_t ra = get_reg_val(mem, a);
    uint32_t rb = get_reg_val(mem, b);
    uint32_t *store = malloc(sizeof(*store));
    assert(store != NULL);

    *store = get_reg_val(mem, c);
    free(Seq_put(Seq_get(*(Seq_T*)get_main_mem(mem), ra), rb, store));
}

void add(umStorage *mem, int a, int b, int c)
{
    assert(a >= 0 && a < 8);
    assert(b >= 0 && b < 8);
    assert(c >= 0 && c < 8);
    
    uint32_t rb = get_reg_val(mem, b);
    uint32_t rc = get_reg_val(mem, c);
    uint32_t value = (rb + rc) % (4294967296);
    edit_register(mem, a, value);
}

void multiply(umStorage *mem, int a, int b, int c)
{
    assert(a >= 0 && a < 8);
    assert(b >= 0 && b < 8);
    assert(c >= 0 && c < 8);
    
    uint32_t rb = get_reg_val(mem, b);
    uint32_t rc = get_reg_val(mem, c);
    uint32_t value = (rb * rc) % (4294967296);
    edit_register(mem, a, value);
}

void divide(umStorage *mem, int a, int b, int c)
{
    assert(a >= 0 && a < 8);
    assert(b >= 0 && b < 8);
    assert(c >= 0 && c < 8);
    
    uint32_t rb = get_reg_val(mem, b);
    uint32_t rc = get_reg_val(mem, c);
    uint32_t value = rb / rc;
    edit_register(mem, a, value);
}

void nand(umStorage *mem, int a, int b, int c)
{
    assert(a >= 0 && a < 8);
    assert(b >= 0 && b < 8);
    assert(c >= 0 && c < 8);
    
    uint32_t rb = get_reg_val(mem, b);
    uint32_t rc = get_reg_val(mem, c);
    uint32_t value = ~(rb & rc);
    edit_register(mem, a, value);
}

void mapSegment(umStorage *mem, int b, int c)
{
    assert(b >= 0 && b < 8);
    assert(c >= 0 && c < 8);
    
    uint32_t length = get_reg_val(mem, c);
    Seq_T newSeg = Seq_new(length);

    for (uint32_t i = 0; i < length; i++) {
        uint32_t *zeroPtr = malloc(sizeof(*zeroPtr));
        assert(zeroPtr != NULL);
        *zeroPtr = 0;
        Seq_addhi(newSeg, zeroPtr);
    }

    uint32_t id = add_segment(newSeg, mem);
    edit_register(mem, b, id);
}

void unmapSegment(umStorage *mem, int c)
{
    assert(c >= 0 && c < 8);
    
    uint32_t rc = get_reg_val(mem, c);
    uint32_t *rcPntr = malloc(sizeof(*rcPntr));
    assert(rcPntr != NULL);
    *rcPntr = rc;
    remove_segment(mem, rcPntr);
}

void output(umStorage *mem, int c)
{
    assert(c >= 0 && c < 8);

    uint32_t rc = get_reg_val(mem, c);
    assert(rc < 256);

    putchar(rc);
}

void input(umStorage *mem, int c)
{
    assert(c >= 0 && c < 8);

    char input;
    scanf("%c", &input);

    if (input == EOF) {
        uint32_t value = ~0;
        edit_register(mem, c, value);
    }
    edit_register(mem, c, input);
}

void loadProgram(umStorage *mem, int b, int c)
{
    assert(b >= 0 && b < 8);
    assert(c >= 0 && c < 8);

    uint32_t rb = get_reg_val(mem, b);
    if (rb != 0){
        Seq_T source = Seq_get(*(Seq_T*)get_main_mem(mem), rb);
        int length = Seq_length(source);
        Seq_T duplicate = Seq_new(length);

        for (int i = 0; i < length; i++) {
            uint32_t *copy = malloc(sizeof(*copy));
            assert(copy != NULL);
            *copy = *(uint32_t*)Seq_get(source, i);
            Seq_addhi(duplicate, copy);
        }

        Seq_T temp = Seq_put(*(Seq_T*)get_main_mem(mem), 0, duplicate);
        int tempSize = Seq_length(temp);
        for (int j = 0; j < tempSize; j++) {
            free(Seq_get(temp, j));
        }
        Seq_free(&temp);
    }
    int rc = get_reg_val(mem, c);
    edit_counter(mem, rc);
}

void loadVal(umStorage *mem, uint32_t value, int a)
{
    assert(a >= 0 && a < 8);
    edit_register(mem, a, value);
}