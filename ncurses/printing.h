#pragma once

#include "common.h"
int char_format = 3;
extern int margin;
extern int right_margin;

extern int reg_box_l;
extern int reg_box_c;
extern int instruct_l;
extern int instruct_c;

extern WINDOW* prog_win;
extern WINDOW* stack_win;
extern WINDOW* frame_data;
extern WINDOW* input_win;
extern WINDOW* ins_win;
extern WINDOW* misc_win;

extern std::set<int> breakpoints;


void printMisc(){
    werase(misc_win);
    
    wprintw(misc_win, "TOS: 0x%.2X (%d)\n", tos(), tos());
    wprintw(misc_win, "PC: %d\n", get_program_counter());
    wprintw(misc_win, "SP: %d\n", get_stack_top());
    wprintw(misc_win, "LV: %d\n", get_frame_bottom());
    wrefresh(misc_win);
}

void printInstructions(){

    werase(ins_win);
    uint8_t* text = get_text();
    unsigned int text_size = get_text_size(); 

    
    for(int i = get_program_counter(), k = 0; i < text_size && k < instruct_l - right_margin; i++, k++){

        uint8_t op_code = text[i];

        if(i == get_program_counter()) wattron(ins_win, COLOR_PAIR(1));
        else if(breakpoints.count(i)) wattron(ins_win, COLOR_PAIR(2));

        if(!lookup.count(op_code)){
            wprintw(ins_win, "0x%.2X ILLEGAL \n", op_code);
            wattroff(ins_win, COLOR_PAIR(1));
            continue;
        }

        Instruction ins = lookup[op_code];

        wprintw(ins_win, "0x%.2X %s ", op_code, ins.name.c_str());

        for(int j = 0; j < ins.args; j++){
            uint8_t arg = text[++i];
            wprintw(ins_win, "0x%.2X(%d) ", arg, arg);
        }

        wprintw(ins_win, "\n");
        wattroff(ins_win, COLOR_PAIR(1));
        wattroff(ins_win, COLOR_PAIR(2));   
    }
    wrefresh(ins_win);

}

void printFrame(){

    werase(frame_data);
    unsigned int frame_top = get_frame_top();
    unsigned int frame_bottom = get_frame_bottom();
    
    int chars = 0;
    for(int i = frame_bottom; i < frame_top; i++){
        wprintw(frame_data, "%02X ", get_local_variable(i));
        chars += 3;
        if(reg_box_c - chars < char_format + margin){
            wprintw(frame_data, "\n");
            chars = 0;
        }
    }
    wrefresh(frame_data);

}

void printStack(){
    
    werase(stack_win);
    int32_t* stack = get_stack();
    unsigned int stack_top = get_stack_top();
    unsigned int stack_bottom = get_stack_bottom();

    int chars = 0;
    for(int i = stack_bottom; i <= stack_top; i++){
        wprintw(stack_win, "%02X ", stack[i]);
        chars += 3;
        if(reg_box_c - chars < char_format + margin){
            wprintw(stack_win, "\n");
            chars = 0;
        }
    }
    wrefresh(stack_win);
}

void printProgram(){

    werase(prog_win);
    uint8_t* text = get_text();
    unsigned int text_size = get_text_size();

    int chars = 0;
    for(int i = 0; i < text_size; i++){
        wprintw(prog_win, "%02X ", text[i]);
        chars += 3;
        if(reg_box_c - chars < char_format + margin){
            wprintw(prog_win, "\n");
            chars = 0;
        }
    }
    wrefresh(prog_win);
}

void printStep(){
    printFrame();
    printStack();
    printMisc();
    printInstructions();
}