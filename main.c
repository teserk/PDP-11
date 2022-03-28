#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#define MEMSIZE 64*1024
#define LEN_BYTE 8
#define LEN_WORD 16
typedef unsigned char byte;
typedef unsigned short int word;
typedef word adr;
byte mem[MEMSIZE];

byte b_read (adr a) {
    return mem[a];
}

void b_write (adr a, byte val) {
    mem[a] = val;
}

word w_read(adr a) {
    word w = ((word) mem[a + 1]) << LEN_BYTE;
    w = w | mem[a];
    return w;
}

void w_write (adr a, word val) {
    mem[a + 1] = (byte) (val >> LEN_BYTE);
    mem[a] = (byte) (val);
}

void print_bin(unsigned char x, char end){
    char a[LEN_BYTE+1] = {};

    for(int i = LEN_BYTE-1; x > 0 && i >= 0; i--) {
        a[i] = x % 2;
        x = x / 2;
    }
    for(int i = 0; i < LEN_BYTE; i++)
        printf("%d", a[i]);
    if (end)
        printf("%c", end);
}

void load_file() {
    adr a, n;
    byte k;

    while (fscanf(stdin, "%hx%hx", &a, &n) != EOF) {
        for (adr i = 0; i < n; i++) {
            fscanf(stdin, "%hhx", &k);
            b_write(a + i, k);
        }
    }
    fclose(stdin);
}

void mem_dump(adr start, word n) {
    for (int i = 0; i < n; i += 2) {
        printf("%06o : %06o\n", start + i, w_read(start + i));
    }
}