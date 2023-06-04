#pragma once

#include "common.h"

#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <unistd.h> // for STDOUT_FILENO

int padding = 2;
int margin = 4;
int right_padding = 3;
int right_margin = 6;

void setInstructions();

WINDOW* program_box;
WINDOW* stack_box;
WINDOW* frame_box;
WINDOW* input_box;
WINDOW* instruction_box;
WINDOW* misc_box;

WINDOW* prog_win; 
WINDOW* stack_win;
WINDOW* frame_data;
WINDOW* input_win;
WINDOW* ins_win;
WINDOW* misc_win;

int reg_box_l;
int reg_box_c;
int instruct_l;
int instruct_c;
extern int MAX_LINES;
extern int MAX_COLS;


void setup(){

    setInstructions();

    int columns_diff = 0.05 * MAX_COLS;
    int lines_diff = 0.05 * MAX_LINES;

    // leftscreen boxes
    reg_box_l = MAX_LINES * 0.25;
    reg_box_c = MAX_COLS * 0.7;

    // instructions
    instruct_l = MAX_LINES * 0.7;
    instruct_c = MAX_COLS * 0.25;

    // misc
    int m_l = MAX_LINES * 0.20;
    int m_c = MAX_COLS * 0.25;

    int input_l = MAX_LINES * 0.1;
    int input_c = MAX_COLS * 0.5;
    
    int box_l = reg_box_l + lines_diff;

    int prog_off_l = input_l + lines_diff;
    int stack_off_l = prog_off_l + box_l;
    int frame_off_l = stack_off_l + box_l;

    // create boxes for borders, then create windows inside boxes to avoid printing over borders

    // border boxes
    program_box = newwin(reg_box_l, reg_box_c, prog_off_l, 0);
    stack_box = newwin(reg_box_l, reg_box_c, stack_off_l, 0);
    frame_box = newwin(reg_box_l, reg_box_c, frame_off_l, 0);
    input_box = newwin(input_l, input_c, 0, 0);
    instruction_box = newwin(instruct_l, instruct_c, 0, reg_box_c + columns_diff);
    misc_box = newwin(m_l, m_c, instruct_l + lines_diff, reg_box_c + columns_diff);


    // create windows inside boxes left screen
    prog_win = newwin(reg_box_l - margin, reg_box_c - margin, prog_off_l + padding, padding);
    stack_win = newwin(reg_box_l - margin, reg_box_c - margin, stack_off_l + padding, padding);
    frame_data = newwin(reg_box_l - margin, reg_box_c - margin,  frame_off_l + padding, padding);
    input_win = newwin(input_l - margin, input_c - margin, padding, padding);


    // right screen
    ins_win = newwin(instruct_l - right_margin, instruct_c - right_margin, right_padding, reg_box_c + columns_diff + right_padding);
    misc_win = newwin(m_l - right_margin, m_c - right_margin, instruct_l + lines_diff + right_padding, reg_box_c + columns_diff + right_padding);


    // refresh to get screens
    refresh();
    
    // put all borders
    box(program_box, 0, 0);
    box(instruction_box, 0, 0);
    box(stack_box, 0, 0);
    box(frame_box, 0, 0);
    box(misc_box, 0, 0);
    box(input_box, 0, 0);

    // print the box headers
    mvwprintw(program_box, 0, 2, "Program");
    mvwprintw(instruction_box, 0, 2, "Instructions");
    mvwprintw(stack_box, 0, 2, "Stack");
    mvwprintw(frame_box, 0, 2, "Frame");
    mvwprintw(misc_box, 0, 2, "Misc");
    mvwprintw(input_box, 0, 2, "Input");

    
    // refresh to get borders
    wrefresh(program_box);
    wrefresh(instruction_box);
    wrefresh(stack_box);
    wrefresh(frame_box);
    wrefresh(misc_box);
    wrefresh(input_box);

    printProgram();
    printStep();

}

void setInstructions(){

    lookup[0x10] = {1, "BIPUSH"};
    lookup[0x59] = {0, "DUP"};
    lookup[0xFE] = {0, "ERR"};
    lookup[0xA7] = {2, "GOTO"};
    lookup[0xFF] = {0, "HALT"};
    lookup[0x60] = {0, "IADD"};
    lookup[0x7E] = {0, "IAND"};
    lookup[0x99] = {2, "IFEQ"};
    lookup[0x9B] = {2, "IFLT"};
    lookup[0x9F] = {2, "IF_ICMPEQ"};
    lookup[0x84] = {2, "IINC"};
    lookup[0x15] = {1, "ILOAD"};
    lookup[0xFC] = {0, "IN"};
    lookup[0xB6] = {2, "INVOKEVIRTUAL"};
    lookup[0xB0] = {0, "IOR"};
    lookup[0xAC] = {0, "IRETURN"};
    lookup[0x36] = {1, "ISTORE"};
    lookup[0x64] = {0, "ISUB"};
    lookup[0x13] = {2, "LDC_W"};
    lookup[0x00] = {0, "NOP"};
    lookup[0xFD] = {0, "OUT"};
    lookup[0x57] = {0, "POP"};
    lookup[0x5F] = {0, "SWAP"};

    // still have to handle this properly
    lookup[0xC4] = {1, "WIDE"};
}
