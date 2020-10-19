/**
 ** Jack Burns and Ryan Ghayour
 ** 
 ** um.c
 ** April 15 2020
 ** Purpose: Driver File for the um. Links the um memory and operations
 ** to execute a um emulator.
 **/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <bitpack.h>
#include "um_operations.h"

const int WORDSIZE = 4;

void load_seg_zero(char *filename, umStorage *mem);
uint32_t *bitStore(char word[]);
bool run_instruction(umStorage *mem);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage:     um [filename]\n");
        return EXIT_FAILURE;
    }

    umStorage *mem = initialize_memory();
    load_seg_zero(argv[1], mem);

    /* loops through instructions until halt */
    bool halt = false;
    while (!halt) {
        halt = run_instruction(mem);
    }
    release_memory(mem);
    
    return 0;
}
/* Takes in the name of the input file and a pointer to the um's memory.
 * Creates the zero segment, opens and reads every instruction from the file, 
 * and stores it in the zero segment. It is a checked runtime error that the 
 * file is successfully opened.
 */
void load_seg_zero(char *filename, umStorage *mem)
{
    /* creates the segment 0 */
    Seq_T segZero = Seq_new(0);

    FILE *fp = fopen(filename, "r");
    assert(fp);
    char word[WORDSIZE];

    char c = ' ';
    c = fgetc(fp);
    while (!feof(fp)) {
        for (int i = 0; i < WORDSIZE; i++) {
            word[i] = c;
            c = fgetc(fp);
        }
        Seq_addhi(segZero, bitStore(word));
    }
    add_segment(segZero, mem);
    fclose(fp);

}
/* Takes in the 4 char word and converts it to a 32 bit value.
 * A pointer is created, with its value being the 32 bit value.
 * pointer is returned. 
 */
uint32_t *bitStore(char word[])
{
    uint32_t instruction = 0;
    uint32_t mask = 255;
    uint32_t temp = 0;
    
    for (int i = 0; i < WORDSIZE; i++){
        temp = mask & word[i];
        temp = temp << 8 * (3 - i);
        instruction = instruction | temp;
    }

    uint32_t *instPtr = malloc(sizeof(*instPtr));
    assert(instPtr != NULL);
    *instPtr = instruction;

    return instPtr;
}
/* Takes in a pointer to the um's memory. Function gets the next instruction
 * from segment 0 and finds the op code, extracts necessary values (depending
 * on op code) and calls the corrosponding um operation.
 * Returns true after operation is executed, and false if the op code = halt
 */
bool run_instruction(umStorage *mem)
{
    uint32_t instruction = get_next_instruction(mem);

    uint32_t opCode = instruction >> 28; 

    if (opCode == 13) {
        uint32_t a = 7;
        a = a << 25;
        a = (a & instruction) >> 25;
        uint32_t value = 33554431;   /* 25 ones in binary */
        value = value & instruction;
        loadVal(mem, value, a);
    }

    else{
        uint32_t a = ((7 << 6) & instruction) >> 6;
        uint32_t b = ((7 << 3) & instruction) >> 3;
        uint32_t c = 7 & instruction;

        if (opCode == 0){
            cMove(mem, a, b, c);
        }
        else if (opCode == 1){
            segLoad(mem, a, b, c);
        }
        else if (opCode == 2){
            segStore(mem, a, b, c);
        }
        else if (opCode == 3){
            add(mem, a, b, c);
        }
        else if (opCode == 4){
            multiply(mem, a, b, c); 
        }
        else if (opCode == 5){
            divide(mem, a, b, c);
        }
        else if (opCode == 6){
            nand(mem, a, b, c);
        }
        else if (opCode == 7){
            return true;    
        }
        else if (opCode == 8){
            mapSegment(mem, b, c);
        }
        else if (opCode == 9){
            unmapSegment(mem, c);
        }   
        else if (opCode == 10){
            output(mem, c);
        }
        else if (opCode == 11){
            input(mem, c);
        }
        else if (opCode == 12){
            loadProgram(mem, b, c);
        }
        else {
            fprintf(stderr, "Unkown Command\n");
            return true;
        }
    }
    return false;
}