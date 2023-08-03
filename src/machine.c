#include <stdio.h> 
#include <stdlib.h>
#include "ijvm.h"

FILE *in;     
FILE *out;

void set_input(FILE *fp) 
{ 
  in = fp; 
}

void set_output(FILE *fp) 
{ 
  out = fp; 
}

int init_ijvm(char *binary_path) 
{
  in = stdin;
  out = stdout;
  return 0;
}

void destroy_ijvm(void) 
{

}

byte_t *get_text(void) 
{
  return NULL;
}

unsigned int get_text_size(void) 
{
  return 0;
}

word_t get_constant(int i) 
{
  return 0;
}

unsigned int get_program_counter(void) 
{

  return 0;
}

word_t tos(void) 
{

  return -1;
}

bool finished(void) 
{

  return false;
}

word_t get_local_variable(int i) 
{

  return 0;
}

void step(void) 
{

}

void run(void) 
{
  while (!finished()) 
  {
    step();
  }
}

byte_t get_instruction(void) 
{ 
  return get_text()[get_program_counter()]; 
}

unsigned int get_stack_top(void){
  return 0;
}

unsigned int get_stack_bottom(){
  return 257;
}

int32_t* get_stack(void){
  return 0;
}

unsigned int get_frame_top(void){
  return 0;
}

unsigned int get_frame_bottom(void){
  return 0;
}
