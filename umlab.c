/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction loadval(unsigned ra, unsigned val);


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction cMove(Um_register a, Um_register b, Um_register c) 
{
        return three_register(CMOV, a, b, c);
}

static inline Um_instruction segLoad(Um_register a, Um_register b, Um_register c) 
{
        return three_register(SLOAD, a, b, c);
}

static inline Um_instruction segStore(Um_register a, Um_register b, Um_register c) 
{
        return three_register(SSTORE, a, b, c);
}

static inline Um_instruction multiply(Um_register a, Um_register b, Um_register c) 
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction divide(Um_register a, Um_register b, Um_register c) 
{
        return three_register(DIV, a, b, c);
}

static inline Um_instruction nand(Um_register a, Um_register b, Um_register c) 
{
        return three_register(NAND, a, b, c);
}

static inline Um_instruction mapSeg(Um_register b, Um_register c) 
{
        return three_register(ACTIVATE, 0, b, c);
}

static inline Um_instruction unmapSeg(Um_register c) 
{
        return three_register(INACTIVATE, 0, 0, c);
}

static inline Um_instruction output(Um_register c)
{
    return three_register(OUT, 0, 0, c);
}

static inline Um_instruction input(Um_register c) 
{
        return three_register(IN, 0, 0, c);
}

static inline Um_instruction loadProg(Um_register b, Um_register c) 
{
        return three_register(LOADP, 0, b, c);
}

/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
    assert(ra < 7 && ra >= 0);
    assert(rb < 7 && rb >= 0);
    assert(rc < 7 && rc >= 0);

    Um_instruction word;
    word = op << 28;

    rb = rb << 3;
    ra = ra << 6;
    word = word | ra;
    word = word | rb;
    word = word | rc;

    return word;
}

Um_instruction loadval(unsigned ra, unsigned val)
{
    assert(ra < 7);
    int opCode = LV;
    Um_instruction word;

    ra = ra << 25;
    word = opCode << 28;
    word = word | ra;

    uint32_t mask = ~0 >> 7;
    val = val & mask;

    word = word | val;    

    return word;
}

/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_loadVal_test(Seq_T stream)
{
    append(stream, loadval(r1, 'A'));
    append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
}

void build_add_test(Seq_T stream) 
{
    append(stream, add(r1, r2, r3));
    append(stream, halt());   
}

void build_output_6_test(Seq_T stream)
{
    append(stream, loadval(r1, 48));
    append(stream, loadval(r2, 6));
    append(stream, add(r3, r2, r1));
    append(stream, output(r3));
    append(stream, halt());
}

void build_loadProgram_test(Seq_T stream){
    append(stream, loadval(r1, 0));
    append(stream, loadval(r4, 0));
    append(stream, loadval(r3, 1));
    append(stream, mapSeg(r2, r3));
    append(stream, segStore(r2, r4, r1));
    append(stream, loadProg(r2, r4));
    append(stream, halt());
}

void build_mulTo2_test(Seq_T stream)
{
    append(stream, loadval(r1, 10));
    append(stream, loadval(r2, 5));
    append(stream, multiply(r3, r1, r2));
    append(stream, output(r3));
    append(stream, loadval(r1, 10));
    append(stream, output(r1));
    append(stream, halt());
}

void build_divTo4_test(Seq_T stream)
{
    append(stream, loadval(r1, 208));
    append(stream, loadval(r2, 4));
    append(stream, divide(r3, r1, r2));
    append(stream, output(r3));
    append(stream, loadval(r1, 10));
    append(stream, output(r1));
    append(stream, halt());
}

void build_cMov_test(Seq_T stream)
{
   append(stream, loadval(r1, 52));
   append(stream, loadval(r2, 81));
   append(stream, loadval(r3, 0));
   append(stream, loadval(r4, 10));
   append(stream, cMove(r1, r2, r3));
   /* should output a 4 */
   append(stream, output(r1));
   append(stream, output(r4));
   append(stream, loadval(r3, 4));
   append(stream, cMove(r1, r2, r3));
   /* should output a Q */
   append(stream, output(r1));
   append(stream, output(r4));
   append(stream, halt());
}

void build_nand_test(Seq_T stream)
{
    append(stream, loadval(r4, 10));
    append(stream, loadval(r1, 39403369));
    append(stream, loadval(r2, 109));
    append(stream, multiply(r3, r2, r1));

    append(stream, loadval(r5, 16843009));
    append(stream, loadval(r2, 255));
    append(stream, multiply(r1, r2, r5));

    append(stream, nand(r2, r3, r1));
    append(stream, output(r2));
    append(stream, output(r4));
    append(stream, halt());
}

void build_map_test(Seq_T stream)
{
    append(stream, loadval(r1, 10));
    append(stream, mapSeg(r2, r1));
    append(stream, halt());
}

void build_unmap_test(Seq_T stream){
    append(stream, loadval(r1, 27));
    append(stream, mapSeg(r2, r1));
    append(stream, unmapSeg(r2));
    append(stream, halt());
}

void build_remap_test(Seq_T stream){
    append(stream, loadval(r1, 27));
    append(stream, loadval(r4, 250));
    append(stream, loadval(r5, 33));
    append(stream, mapSeg(r2, r1));
    append(stream, unmapSeg(r2));
    append(stream, mapSeg(r2, r4));
    append(stream, mapSeg(r6, r5));
    append(stream, unmapSeg(r2));
    append(stream, unmapSeg(r6));
    append(stream, mapSeg(r2, r4));
    append(stream, halt());
}

void build_segLoad_test(Seq_T stream){
    append(stream, loadval(r1, 69));
    append(stream, loadval(r3, 5));
    append(stream, mapSeg(r2, r1));
    append(stream, segLoad(r1, r2, r3));
    append(stream, output(r1));
    append(stream, halt());
}

void build_segStore_test(Seq_T stream){
    append(stream, loadval(r1, 69));
    append(stream, loadval(r2, 42));
    append(stream, loadval(r4, 25));
    append(stream, mapSeg(r3, r1));
    append(stream, segStore(r3, r4, r2));
    append(stream, halt());
}

void build_input_test(Seq_T stream)
{
    append(stream, input(r1));
    append(stream, output(r1));
    append(stream, halt());
}







