
#include "tui.h"

void setInstructions(){
    lookup_tui[0x10] = {1, "BIPUSH"};
    lookup_tui[0x59] = {0, "DUP"};
    lookup_tui[0xFE] = {0, "ERR"};
    lookup_tui[0xA7] = {2, "GOTO"};
    lookup_tui[0xFF] = {0, "HALT"};
    lookup_tui[0x60] = {0, "IADD"};
    lookup_tui[0x7E] = {0, "IAND"};
    lookup_tui[0x99] = {2, "IFEQ"};
    lookup_tui[0x9B] = {2, "IFLT"};
    lookup_tui[0x9F] = {2, "IF_ICMPEQ"};
    lookup_tui[0x84] = {2, "IINC"};
    lookup_tui[0x15] = {1, "ILOAD"};
    lookup_tui[0xFC] = {0, "IN"};
    lookup_tui[0xB6] = {2, "INVOKEVIRTUAL"};
    lookup_tui[0xB0] = {0, "IOR"};
    lookup_tui[0xAC] = {0, "IRETURN"};
    lookup_tui[0x36] = {1, "ISTORE"};
    lookup_tui[0x64] = {0, "ISUB"};
    lookup_tui[0x13] = {2, "LDC_W"};
    lookup_tui[0x00] = {0, "NOP"};
    lookup_tui[0xFD] = {0, "OUT"};
    lookup_tui[0x57] = {0, "POP"};
    lookup_tui[0x5F] = {0, "SWAP"};
    lookup_tui[0xC4] = {1, "WIDE"};
}

void setup(){

    setInstructions();

    int columns_diff = 0.05 * MAX_COLS;
    int lines_diff = 0.05 * MAX_LINES;

    // leftscreen boxes
    REG_BOX_L = MAX_LINES * 0.25;
    REG_BOX_C = MAX_COLS * 0.7;

    // instructions
    INSTRUCT_L = MAX_LINES * 0.7;
    INSTRUCT_C = MAX_COLS * 0.25;

    // misc
    int m_l = MAX_LINES * 0.20;
    int m_c = MAX_COLS * 0.25;

    int input_l = MAX_LINES * 0.1;
    int input_c = MAX_COLS * 0.5;
    
    int box_l = REG_BOX_L + lines_diff;

    int prog_off_l = input_l + lines_diff;
    int stack_off_l = prog_off_l + box_l;
    int frame_off_l = stack_off_l + box_l;

    // create boxes for borders, then create windows inside boxes to avoid printing over borders

    // border boxes
    program_box = newwin(REG_BOX_L, REG_BOX_C, prog_off_l, 0);
    stack_box = newwin(REG_BOX_L, REG_BOX_C, stack_off_l, 0);
    frame_box = newwin(REG_BOX_L, REG_BOX_C, frame_off_l, 0);
    input_box = newwin(input_l, input_c, 0, 0);
    instruction_box = newwin(INSTRUCT_L, INSTRUCT_C, 0, REG_BOX_C + columns_diff);
    misc_box = newwin(m_l, m_c, INSTRUCT_L + lines_diff, REG_BOX_C + columns_diff);

    // create windows inside boxes left screen
    prog_win = newwin(REG_BOX_L - MARGIN, REG_BOX_C - MARGIN, prog_off_l + PADDING, PADDING);
    stack_win = newwin(REG_BOX_L - MARGIN, REG_BOX_C - MARGIN, stack_off_l + PADDING, PADDING);
    frame_data = newwin(REG_BOX_L - MARGIN, REG_BOX_C - MARGIN,  frame_off_l + PADDING, PADDING);
    input_win = newwin(input_l - MARGIN, input_c - MARGIN, PADDING, PADDING);

    // right screen
    ins_win = newwin(INSTRUCT_L - RIGHT_MARGIN, INSTRUCT_C - RIGHT_MARGIN, RIGHT_PADDING, REG_BOX_C + columns_diff + RIGHT_PADDING);
    misc_win = newwin(m_l - RIGHT_MARGIN, m_c - RIGHT_MARGIN, INSTRUCT_L + lines_diff + RIGHT_PADDING, REG_BOX_C + columns_diff + RIGHT_PADDING);

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


std::string get_command_string(WINDOW* win){
    std::string input;

    werase(win);
    wmove(input_win, 0, 0);
    nocbreak();
    int ch = wgetch(win);

    while ( ch != '\n' ){
        input.push_back(ch);
        ch = wgetch(win);
    }
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
    
    // show all the commmands:
    wprintw(instruct_menu, "Commands\n");
    wprintw(instruct_menu, "\tb <address> : set breakpoint at address\n");
    wprintw(instruct_menu, "\td <address> : delete breakpoint at address\n");
    wprintw(instruct_menu, "\tn <steps>   : step n steps\n");
    wprintw(instruct_menu, "\tp <steps>   : step back n steps\n");
    wprintw(instruct_menu, "\tr           : run until breakpoint or halt\n");
    wprintw(instruct_menu, "\th           : show this help menu\n");
    wprintw(instruct_menu, "\tq           : quit\n\n");

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

    
    for(int i = get_program_counter(), k = 0; i < text_size && k < INSTRUCT_L - RIGHT_MARGIN; i++, k++){

        uint8_t op_code = text[i];

        if(i == get_program_counter()) wattron(ins_win, COLOR_PAIR(1));
        else if(breakpoints_tui.count(i)) wattron(ins_win, COLOR_PAIR(2));

        if(!lookup_tui.count(op_code)){
            wattroff(ins_win, COLOR_PAIR(1));
            wattroff(ins_win, COLOR_PAIR(2));
            continue;
        }

        if(functions_tui.count(i)){
            uint32_t arg = (text[i] << 8) | text[i + 1];
            uint32_t vars = (text[i + 2] << 8) | text[i + 3];
            wprintw(ins_win, "%3d func: vars = %d args = %d\n", i, arg, vars);
            wattroff(ins_win, COLOR_PAIR(1));
            wattroff(ins_win, COLOR_PAIR(2));
            i += 3;
            continue;
        }

        Instruction ins = lookup_tui[op_code];
        
        wprintw(ins_win, "%3d %s ", i, ins.name.c_str());
        if(op_code == OP_WIDE){
            ins = lookup_tui[text[++i]];
            uint16_t arg = (text[i + 1] << 8) | text[i + 2];
            i += 2;
            wprintw(ins_win, "%s 0x%X(%d) ", ins.name.c_str(), arg, arg);
            ins.args -= 1;
        }


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
        if(REG_BOX_C - chars < CHAR_COUNT + MARGIN){
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
        if(REG_BOX_C - chars < CHAR_COUNT + MARGIN){
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
        if(REG_BOX_C - chars < CHAR_COUNT + MARGIN){
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

void combinedInstruct(std::string input){
    try{
        char command = input[0];
        int arg = std::stoi(input.substr(1));
        switch(command){
            case 'b':
                breakpoints_tui.insert(arg);
                break;
            case 'd':
                breakpoints_tui.erase(arg);
                break;
            case 'n':
                for(int i = 0; i < arg; i++) step();
                STEP_COUNT += arg;
                break;
            case 'p':
                destroy_ijvm();
                init_ijvm(FILE_PATH_TUI);
                for(int i = 0; i < STEP_COUNT - arg; i++) step(); 
                STEP_COUNT -= arg;
                break;
        }
    } catch(const std::exception& e){
        endwin();
        destroy_ijvm();
        std::cerr << "Error : " << e.what() << '\n';
        exit(1);
    }
}

void singleInstruct(std::string input){
    if(input == "n"){
        step();
        STEP_COUNT++;
    }
    if(input == "p" && STEP_COUNT > 0){
        destroy_ijvm();
        init_ijvm(FILE_PATH_TUI);
        for(int i = 0; i < STEP_COUNT - 1; i++) step();
        STEP_COUNT--;
    }
    if(input == "r"){
        while(get_instruction() != OP_HALT){
            if(breakpoints_tui.count(get_program_counter())){
                breakpoints_tui.erase(get_program_counter());
                break;
            }
            step();
            STEP_COUNT++;
        }
    }
    if(input == "h"){
        printMenu();
        setup();
    }
}

int checkInstruct(std::string input){
    if(input == "q" || get_instruction() == OP_HALT) return -1;

    if(input.size() == 0)   input = last_instruct_tui;
    if(input.size() == 1)   singleInstruct(input);
    if(input.size() >= 2)   combinedInstruct(input);
    
    return 0;
}

void get_functions_indices(){
    for(uint32_t i = 0; i < get_text_size(); i++){
        if(get_text()[i] == OP_INVOKEVIRTUAL){
            int index = get_constant(get_program_counter());
            functions_tui.insert(index);
        }
    }
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

    get_functions_indices();

    FILE_PATH_TUI = argv[1];

    initscr();
    
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);

    // get terminal size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    MAX_LINES = w.ws_row;
    MAX_COLS = w.ws_col;

    printMenu();
    setup();
    wattron(input_win, A_STANDOUT);

    while(true){
        std::string input = get_command_string(input_win);
        // get minimum value in breakpoints set
        int res = checkInstruct(input);
        if(res == -1) break;
        if(input.size() != 0) last_instruct_tui = input;
        printStep();
    }

    endwin();
    destroy_ijvm();
    return 0;
}


