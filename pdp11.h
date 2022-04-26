#ifndef MY_PDP_11_EMULATOR_DO_FUNCTIONS_H
#define MY_PDP_11_EMULATOR_DO_FUNCTIONS_H
#define pc reg[7]
#define LEN_DD 6
#define LEN_SS 6
#define HAS_SS (1<<1)
#define HAS_DD (1)
#define POSITION_B 15
#define LEN_BYTE 8
#define LEN_WORD 16
#define MEMSIZE 64*1024
#define LEN_BYTE 8
#define LEN_WORD 16
typedef unsigned char byte;
typedef unsigned short int word;
typedef word adr;


typedef struct {
    word mask;
    word opcode;
    char * name;
    word params;
    void (*do_func) (void);
} Command;

typedef struct
{
    word val;    // значение аргумента
    word adr;    // адрес аргумента
} Arg;

void do_halt();
void do_mov();
void do_add();
void do_nothing();
void run();
void trace(char * format, ...);
byte b_read (adr a);
word w_read (adr a);
void b_write (adr a, byte val);
void w_write (adr a, word val);
void print_reg();
#endif //MY_PDP_11_EMULATOR_DO_FUNCTIONS_H
