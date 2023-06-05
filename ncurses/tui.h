#pragma once

#include <iostream>
#include <ncurses.h>
#include <string>
#include <cctype>
#include <map>
#include <set>
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <unistd.h> // for STDOUT_FILENO

#ifdef __cplusplus
extern "C" {
#endif

// i have no idea how to do this, but it works :D
#define OP_HALT 0xFF
#define OP_INVOKEVIRTUAL 0xB6
#define OP_WIDE 0xC4

int init_ijvm(char *binary_path);
void destroy_ijvm(void);
uint8_t *get_text(void);
int32_t get_constant(int i);
unsigned int get_program_counter(void);
int32_t tos(void);
void step(void);
int32_t get_local_variable(int i);
uint8_t get_instruction(void);
unsigned int get_text_size(void);
unsigned int get_stack_top(void);
unsigned int get_stack_bottom(void);
int32_t* get_stack(void);
unsigned int get_frame_top(void);
unsigned int get_frame_bottom(void);


void setInstructions();
void setup();
std::string get_command_string(WINDOW* win);

void printMenu();
void printMisc();
void printInstructions();
void printFrame();
void printStack();
void printProgram();
void printStep();

void combinedInstruct(std::string input);
void singleInstruct(std::string input);
int checkInstruct(std::string input);
void get_functions_indices();

struct Instruction{
    int args;
    std::string name;
};

WINDOW* prog_win;
WINDOW* ins_win;
WINDOW* stack_win;
WINDOW* frame_data;
WINDOW* misc_win;
WINDOW* input_win;
WINDOW* program_box;
WINDOW* stack_box;
WINDOW* frame_box;
WINDOW* input_box;
WINDOW* instruction_box;
WINDOW* misc_box;

int MAX_LINES;
int MAX_COLS;
int REG_BOX_L;
int REG_BOX_C;
int INSTRUCT_L;
int INSTRUCT_C;
int PADDING = 2;
int MARGIN = 4;

int RIGHT_PADDING = 3;
int RIGHT_MARGIN = 6;
int CHAR_COUNT = 3;
char* FILE_PATH_TUI;
int STEP_COUNT = 0;

std::string last_instruct_tui = "";
std::set<int> breakpoints_tui;
std::map<int,Instruction> lookup_tui;
std::set<int> functions_tui;
#ifdef __cplusplus
}
#endif










