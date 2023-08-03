#ifndef IJVM_H
#define IJVM_H

#include <stdio.h> 
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t byte_t;
typedef int32_t word_t;

#define MAGIC_NUMBER 0x1DEADFAD

#define OP_BIPUSH         ((byte_t) 0x10)
#define OP_DUP            ((byte_t) 0x59)
#define OP_ERR            ((byte_t) 0xFE)
#define OP_GOTO           ((byte_t) 0xA7)
#define OP_HALT           ((byte_t) 0xFF)
#define OP_IADD           ((byte_t) 0x60)
#define OP_IAND           ((byte_t) 0x7E)
#define OP_IFEQ           ((byte_t) 0x99)
#define OP_IFLT           ((byte_t) 0x9B)
#define OP_IF_ICMPEQ      ((byte_t) 0x9F)
#define OP_IINC           ((byte_t) 0x84)
#define OP_ILOAD          ((byte_t) 0x15)
#define OP_IN             ((byte_t) 0xFC)
#define OP_INVOKEVIRTUAL  ((byte_t) 0xB6)
#define OP_IOR            ((byte_t) 0xB0)
#define OP_IRETURN        ((byte_t) 0xAC)
#define OP_ISTORE         ((byte_t) 0x36)
#define OP_ISUB           ((byte_t) 0x64)
#define OP_LDC_W          ((byte_t) 0x13)
#define OP_NOP            ((byte_t) 0x00)
#define OP_OUT            ((byte_t) 0xFD)
#define OP_POP            ((byte_t) 0x57)
#define OP_SWAP           ((byte_t) 0x5F)
#define OP_WIDE           ((byte_t) 0xC4)

/**
 * Sets the input of the IJVM instance to the provided file.
 **/
void set_input(FILE *fp);


/**
 * Sets the output of the IJVM instance to the provided file
 **/
void set_output(FILE *fp);


/**
 * Initializes the IJVM with the binary file found at the provided argument
 * Note. You need to be able to re-initialize the IJVM after it has been started.
 *
 * Returns  0 on success
 *         -1 on failure
 **/
int init_ijvm(char *binary_path);


/**
 * Destroys a vm, that is to say, free all memory associated with the machine
 * and allow for a new call to init_ijvm().
 */
void destroy_ijvm(void);

/**
 * Returns the currently loaded program text as a byte array.
 **/
byte_t *get_text(void);


/**
 * Returns the size of the currently loaded program text.
 **/
unsigned int get_text_size(void);

/**
 * @param i index of the constant to obtain
 * @return The constant at location i in the constant pool.
 **/
word_t get_constant(int i);


/**
 * Returns the value of the program counter (as an offset from the first instruction).
 **/
unsigned int get_program_counter(void);

/**
 * This function should return the word at the top of the stack of the current
 * frame, interpreted as a signed integer.
 **/
word_t tos(void);

/**
 * Step (perform) one instruction and return.
 * In the case of WIDE, perform the whole WIDE_ISTORE, WIDE_ILOAD or WIDE IINC.
 * If machine has halted or encountered an error, finished() should report
 * false afterward. 
 **/
void step(void);


/**
 * Check whether the machine has any more instructions to execute.
 *
 * A machine will stop running after:
 * - reaching the end of the text section
 * - encountering either the HALT/ERR instructions
 * - encountering an invalid instruction
 */
bool finished(void);


/**
 * Run the vm with the current state until the machine halts.
 **/
void run(void);

/**
 * @param i index of variable to obtain.
 * @return Returns the i:th local variable of the current frame.
 **/
word_t get_local_variable(int i);

/**
 * @return The value of the current instruction represented as a byte_t.
 *
 * This should NOT increase the program counter.
 **/
byte_t get_instruction(void);


/**
 * @return The index at the top of the stack
 **/
unsigned int get_stack_top(void);

/**
 * @return The index at the bottom of the stack(default 0)
 **/
unsigned int get_stack_bottom(void);

/**
 * @return The current stack of the ijvm.
 **/
int32_t* get_stack(void);

/**
 * @return The index at the top of the frame
 **/
unsigned int get_frame_top(void);

/**
 * @return The index at the bottom of the frame
 **/
unsigned int get_frame_bottom(void);


#endif
