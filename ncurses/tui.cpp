#include <iostream>
#include <ncurses.h>
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <unistd.h> // for STDOUT_FILENO
#include <string>
#include <cctype>
#include <map>
#include "../include/ijvm.h"



// WINDOW * win = newwin(nlines, ncols, y0, x0);
WINDOW* program_data;
WINDOW* instruction_data;
WINDOW* stack_data;
WINDOW* frame_data;
WINDOW* misc_data;

int MAX_LINES;
int MAX_COLS;
int p_l;
int p_c;
int i_l;
int i_c;

void setup();
void setInstructions();

struct Instruction{
    int args;
    std::string name;
};

std::map<int,Instruction> lookup_instruction;

void printMisc(){
    werase(misc_data);
    
    wprintw(misc_data, "TOS: 0x%.2X (%d)\n", tos(), tos());
    wprintw(misc_data, "PC: %d\n", get_program_counter());
    wprintw(misc_data, "SP: %d\n", get_stack_top());
    wprintw(misc_data, "FP: %d\n", get_frame_top());
    wrefresh(misc_data);
}

void printInstructions(){

    werase(instruction_data);
    uint8_t* text = get_text();
    unsigned int text_size = get_text_size(); 

    for(int i = 0; i < text_size; i++){
        uint8_t instruction = text[i];
        Instruction ins = lookup_instruction[instruction];
        if(i == get_program_counter()) wattron(instruction_data, COLOR_PAIR(1));
        wprintw(instruction_data, "0x%.2X %s ", instruction, ins.name.c_str());

        if(ins.args >= 1){
            uint8_t arg1 = text[++i];
            wprintw(instruction_data, "0x%.2X(%d) ", arg1, arg1);
        }
        if(ins.args >= 2){
            uint8_t arg2 = text[++i];
            wprintw(instruction_data, "0x%.2X(%d) ", arg2, arg2);
        }
        wprintw(instruction_data, "\n");
        wattroff(instruction_data, COLOR_PAIR(1));
    }
    wrefresh(instruction_data);

}

void printFrame(){

    werase(frame_data);
    unsigned int frame_top = get_frame_top();
    unsigned int frame_bottom = get_frame_bottom();
    
    int chars = 0;
    for(int i = frame_bottom; i < frame_top; i++){
        wprintw(frame_data, "%02X ", get_local_variable(i));
        chars += 3;
        if(p_c - chars < 8){
            wprintw(frame_data, "\n");
            chars = 0;
        }
    }
    wrefresh(frame_data);

}

void printStack(){
    
    werase(stack_data);
    int32_t* stack = get_stack();
    unsigned int stack_top = get_stack_top();
    unsigned int stack_bottom = get_stack_bottom();

    int chars = 0;
    for(int i = stack_bottom; i <= stack_top; i++){
        wprintw(stack_data, "%02X ", stack[i]);
        chars += 3;
        if(p_c - chars < 8){
            wprintw(stack_data, "\n");
            chars = 0;
        }
    }
    wrefresh(stack_data);
}

void printProgram(){

    uint8_t* text = get_text();
    unsigned int text_size = get_text_size();

    int chars = 0;
    for(int i = 0; i < text_size; i++){
        wprintw(program_data, "%02X ", text[i]);
        chars += 3;
        if(p_c - chars < 8){
            wprintw(program_data, "\n");
            chars = 0;
        }
    }
    wrefresh(program_data);
}

int main(int argc, char** argv){

    if(argc < 2){
        std::cout << "Usage: ./ijvm <binary file>" << std::endl;
        return -1;
    }

    if(init_ijvm(argv[1]) != 0){
        std::cout << "Error: Could not load binary file" << std::endl;
        return -1;
    }
    

    setInstructions();

    initscr();
    noecho();
    setup();

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    printFrame();
    printProgram();
    printStack();
    printInstructions();
    printMisc();


    int ch = 0;
    int step_count = 0;
    while(ch != 'q' && get_instruction() != OP_HALT){
        ch = getch();
        if(ch == 'n'){
            step();
            printFrame();
            printStack();
            printMisc();
            printInstructions();
            step_count++;
        }
        if(ch == 'b'){
            destroy_ijvm();
            init_ijvm(argv[1]);
            for(int i = 0; i < step_count - 1; i++) step();
            step_count--;
            printFrame();
            printStack();
            printMisc();
            printInstructions();
        }
    }

    endwin();

    destroy_ijvm();
    return 0;
}

void setup(){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    MAX_LINES = w.ws_row;
    MAX_COLS = w.ws_col;

    int c_diff = 0.05 * MAX_COLS;
    int r_diff = 0.05 * MAX_LINES;

    // leftscreen boxes
    p_l = MAX_LINES * 0.3;
    p_c = MAX_COLS * 0.7;

    // instructions
    i_l = MAX_LINES * 0.7;
    i_c = MAX_COLS * 0.25;

    // misc
    int m_l = MAX_LINES * 0.25;
    int m_c = MAX_COLS * 0.25;

    // create boxes for borders, then create windows inside boxes to avoid printing over borders

    // left screen
    WINDOW* program_box = newwin(p_l, p_c, 0, 0);
    WINDOW* stack_box = newwin(p_l, p_c, p_l + r_diff, 0);
    WINDOW* frame_box = newwin(p_l, p_c, 2 * p_l + 2 * r_diff, 0);

    program_data = newwin(p_l - 4, p_c - 4, 2, 2);
    stack_data = newwin(p_l - 4, p_c - 4, p_l + r_diff + 2, 2);
    frame_data = newwin(p_l - 4, p_c - 4, 2 * p_l + 2 * r_diff + 2, 2);

    // right screen
    WINDOW* instruction_box = newwin(i_l, i_c, 0, p_c + c_diff);
    WINDOW* misc_box = newwin(m_l, m_c, i_l + r_diff, p_c + c_diff);
    instruction_data = newwin(i_l - 6, i_c - 6, 3, p_c + c_diff + 3);
    misc_data = newwin(m_l - 6, m_c - 6, i_l + r_diff + 3, p_c + c_diff + 3);

    // refresh to get screens
    refresh();
    
    // put all borders
    box(program_box, 0, 0);
    box(instruction_box, 0, 0);
    box(stack_box, 0, 0);
    box(frame_box, 0, 0);
    box(misc_box, 0, 0);

    // print the box headers
    mvwprintw(program_box, 0, 2, "Program");
    mvwprintw(instruction_box, 0, 2, "Instructions");
    mvwprintw(stack_box, 0, 2, "Stack");
    mvwprintw(frame_box, 0, 2, "Frame");
    mvwprintw(misc_box, 0, 2, "Misc");

    

    // refresh to get borders
    wrefresh(program_box);
    wrefresh(instruction_box);
    wrefresh(stack_box);
    wrefresh(frame_box);
    wrefresh(misc_box);

}


void setInstructions(){

    lookup_instruction[0x10] = {1, "BIPUSH"};
    lookup_instruction[0x59] = {0, "DUP"};
    lookup_instruction[0xFE] = {0, "ERR"};
    lookup_instruction[0xA7] = {2, "GOTO"};
    lookup_instruction[0xFF] = {0, "HALT"};
    lookup_instruction[0x60] = {0, "IADD"};
    lookup_instruction[0x7E] = {0, "IAND"};
    lookup_instruction[0x99] = {2, "IFEQ"};
    lookup_instruction[0x9B] = {2, "IFLT"};
    lookup_instruction[0x9F] = {2, "IF_ICMPEQ"};
    lookup_instruction[0x84] = {2, "IINC"};
    lookup_instruction[0x15] = {1, "ILOAD"};
    lookup_instruction[0xFC] = {0, "IN"};
    lookup_instruction[0xB6] = {2, "INVOKEVIRTUAL"};
    lookup_instruction[0xB0] = {0, "IOR"};
    lookup_instruction[0xAC] = {0, "IRETURN"};
    lookup_instruction[0x36] = {1, "ISTORE"};
    lookup_instruction[0x64] = {0, "ISUB"};
    lookup_instruction[0x13] = {2, "LDC_W"};
    lookup_instruction[0x00] = {0, "NOP"};
    lookup_instruction[0xFD] = {0, "OUT"};
    lookup_instruction[0x57] = {0, "POP"};
    lookup_instruction[0x5F] = {0, "SWAP"};

    // still have to handle this properly
    lookup_instruction[0xC4] = {1, "WIDE"};
}


