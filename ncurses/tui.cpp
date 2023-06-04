
#include "printing.h"
#include "setup.h"
#include "common.h"
// WINDOW * win = newwin(nlines, ncols, y0, x0);
extern WINDOW* prog_win;
extern WINDOW* ins_win;
extern WINDOW* stack_win;
extern WINDOW* frame_data;
extern WINDOW* misc_win;
extern WINDOW* input_win;

extern WINDOW* program_box;
extern WINDOW* stack_box;
extern WINDOW* frame_box;
extern WINDOW* input_box;
extern WINDOW* instruction_box;
extern WINDOW* misc_box;


int MAX_LINES;
int MAX_COLS;
extern int reg_box_l;
extern int reg_box_c;
extern int instruct_l;
extern int instruct_c;

int curr_breakpoint = 0;

char* file_path;
int step_count = 0;

std::string last_instruct = "";

std::set<int> breakpoints;
std::map<int,Instruction> lookup;

std::string get_string(WINDOW* win){
    std::string input;

    werase(win);
    wmove(input_win, 0, 0);
    nocbreak();
    int ch = wgetch(win);

    while ( ch != '\n' ){
        input.push_back( ch );
        ch = wgetch(win);
    }

    // restore your cbreak / echo settings here
    cbreak();
    return input;
}

void printMenu(){

    clear();
    int margin = MAX_LINES * 0.25;
    int padding = MAX_COLS * 0.25;

    
    WINDOW* menu_box = newwin(MAX_LINES * 0.5, MAX_COLS * 0.5, margin, padding);
    WINDOW* instruct_menu = newwin(MAX_LINES * 0.5 - 4, MAX_COLS * 0.5 - 4, margin + 2, padding + 2);
    refresh();
    box(menu_box, 0, 0);
    wrefresh(menu_box);

    wprintw(instruct_menu, "Welcome to the IJVM Debugger!\n\n");
    // print a horizontal row
    whline(instruct_menu, '-', MAX_COLS * 0.5 - 4);
    // show all the commmands:
    wprintw(instruct_menu, "Commands\n");
    wprintw(instruct_menu, "\tb <address> : set breakpoint at address\n");
    wprintw(instruct_menu, "\td <address> : delete breakpoint at address\n");
    wprintw(instruct_menu, "\tn <steps>   : step n steps\n");
    wprintw(instruct_menu, "\tp <steps>   : step back n steps\n");
    wprintw(instruct_menu, "\tr           : run until breakpoint or halt\n");
    wprintw(instruct_menu, "\tq           : quit\n\n");
    // note that the steps are optional, the terminal also might bug out if the terminal is too zoomed in
    wprintw(instruct_menu, "Note: \n\n");
    wprintw(instruct_menu, " - This program requires you to implement all the functions in machine.c\n");
    wprintw(instruct_menu, " - The steps are optional, if no steps are given, the debugger will step once.\n");
    wprintw(instruct_menu, " - If you press enter, the last instruction will be executed.\n");
    wprintw(instruct_menu, " - If the terminal is too zoomed in, the debugger might not work properly.\n\n");
    wprintw(instruct_menu, "Press any key to continue...\n");
    wrefresh(instruct_menu);


    getch();
    
    delwin(instruct_menu);
    delwin(menu_box);
    clear();
}

void combinedInstruct(std::string input){
    try{
        char command = input[0];
        int arg = std::stoi(input.substr(1));
        switch(command){
            case 'b':
                breakpoints.insert(arg);
                break;
            case 'd':
                breakpoints.erase(arg);
                break;
            case 'n':
                for(int i = 0; i < arg; i++) step();
                step_count += arg;
                break;
            case 'p':
                destroy_ijvm();
                init_ijvm(file_path);
                for(int i = 0; i < step_count - arg; i++) step(); 
                step_count -= arg;
                break;
        }
    } catch(const std::exception& e){
        endwin();
        destroy_ijvm();
        std::cerr << "Error : " << e.what() << '\n';
        exit(1);
    }
}

void deleteWindows(){

    delwin(prog_win);
    delwin(ins_win);
    delwin(stack_win);
    delwin(frame_data);
    delwin(misc_win);
    delwin(input_win);

    delwin(program_box);
    delwin(stack_box);
    delwin(frame_box);
    delwin(input_box);
    delwin(instruction_box);
    delwin(misc_box);

    clear();
}

void singleInstruct(std::string input){
    if(input == "n"){
        step();
        step_count++;
    }
    if(input == "p" && step_count > 0){
        destroy_ijvm();
        init_ijvm(file_path);
        for(int i = 0; i < step_count - 1; i++) step();
        step_count--;
    }
    if(input == "r"){
        while(get_instruction() != OP_HALT){
            if(breakpoints.find(get_program_counter()) != breakpoints.end()){
                break;
            }
            step();
            step_count++;
        }
    }
    if(input == "h"){
        deleteWindows();
        printMenu();
        setup();
    }
}

int checkInstruct(std::string input){
    if(input == "q" || get_instruction() == OP_HALT) return -1;

    if(input.size() == 0)   input = last_instruct;
    if(input.size() == 1)   singleInstruct(input);
    if(input.size() >= 2)   combinedInstruct(input);
    
    return 0;
}

int main(int argc, char** argv){


    if(argc < 2){
        std::cout << "Usage: ./ijvm <binary file>" << std::endl;
        return -1;
    }

    if(init_ijvm(argv[1]) != 0){
        std::cout << "Error: File not found" << std::endl;
        return -1;
    }

    

    file_path = argv[1];
    initscr();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    MAX_LINES = w.ws_row;
    MAX_COLS = w.ws_col;
    printMenu();
    setup();
    wattron(input_win, A_STANDOUT);

    while(true){

        std::string input = get_string(input_win);
        // get minimum value in breakpoints set

        int res = checkInstruct(input);
        if(res == -1) break;
        if(input.size() != 0) last_instruct = input;
        printStep();
    }


    endwin();

    destroy_ijvm();
    return 0;
}




