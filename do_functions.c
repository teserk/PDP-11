#include <stdio.h>
#include <stdlib.h>
#include "pdp11.h"

Command cmd[] = {
        {0070000, 0010000, "mov", HAS_SS | HAS_DD, do_mov},
        {0170000, 0060000, "add", HAS_DD | HAS_SS, do_add},
        {0177000, 0077000, "sob", HAS_R | HAS_NN, do_sob},
        {0077700, 0005000, "clr", HAS_DD, do_clr},
        {0177777, 000000,  "halt", 0, do_halt},
        {0000000, 000000,  "unknown command", 0, do_nothing}
};

extern word reg[];
extern byte mem[];
extern Arg ss, dd, b_flag, r, nn, xx;
extern char N_flag, Z_flag, V_flag, C_flag;

Arg get_modereg(word w) {
    Arg res;
    int r = w & 7;
    int mode = (w >> 3) & 7;
    switch (mode) {
        case 0:             // Rn
            res.adr = r;
            res.val = reg[r];
            trace("R%o ", r);
            break;
        case 1:             //  (Rn)
            res.adr = reg[r];
            res.val = w_read(res.adr);
            trace("(R%o) ", r);
            break;
        case 2:
            res.adr = reg[r];
            res.val = w_read(res.adr);      //  #nn
            if (b_flag.val && r != 7 && r != 6)
                reg[r] += 1;
            else
                reg[r] += 2;
            if (r == 7)
                trace("#%o ", res.val);
            else
                trace("(R%o)+ ", r);
            break;
        case 3:
            res.adr = reg[r];
            res.adr = w_read(res.adr);      //  @(Rn)+
            word tmp_for_trace = res.adr;
            res.val = w_read(res.adr);
            reg[r] += 2;
            if (r == 7)
                trace("@#%o ", tmp_for_trace);
            else
                trace("@(R%o)+ ", r);
            break;
        case 4:
            if (b_flag.val && r != 7 && r != 6)
                reg[r] -= 1;
            else
                reg[r] -= 2;
            res.adr = reg[r];
            res.val = w_read(res.adr);      //  -(Rn)
            trace("-(R%o) ", r);
            break;
        case 5:
            reg[r] -= 2;
            res.adr = reg[r];
            res.adr = w_read(res.adr);      //  @-(Rn)
            res.val = w_read(res.adr);
            trace("@-(R%o) ", r);
            break;
        default:
            trace("mode %o not implemented yet", mode);
            exit(1);
    }
    return res;
}

void set_NZ(word val) {
    if (val == 0) {
        N_flag = 0;
        Z_flag = 1;
    } else if ((val >> 15) & 1) {
        N_flag = 1;
        Z_flag = 0;
    } else if (val > 0) {
        N_flag = 0;
        Z_flag = 0;
    }
}

void do_halt() {
    trace("\n----------halted------------\n");
    print_reg();
    exit(0);
}

void do_mov() {
    if (b_flag.val) {
        b_write(dd.adr, (byte) ss.val);
    } else {
        w_write(dd.adr, ss.val);
    }
}
void do_add() {
    word res = dd.val + ss.val;
    w_write(dd.adr, res);
}

void do_nothing() {
    trace("unknown command\n");
    print_reg();
    exit(1);
}

void do_sob() {
    if ( --reg[r.val] != 0) {
        pc = pc - 2 * nn.val;
    }
}

void do_clr() {
    if (b_flag.val) {
        b_write(dd.adr, 0);
    } else {
        w_write(dd.adr, 0);
    }
}

void run() {
    trace("----------running----------\n");
    pc = 01000;
    int i;
    while (1) {
        word w = w_read(pc);
        trace("%06o %06o ", pc, w);
        pc += 2;
        i = 0;
        while ((w & cmd[i].mask) != cmd[i].opcode) {
            i++;
        }
        trace("%s   ", cmd[i].name);
        if (((w >> POSITION_B) == 1) && ((cmd[i].mask >> POSITION_B) == 0))
            b_flag.val = 1;
        else
            b_flag.val = 0;
        if (cmd[i].params & HAS_SS) {
            ss = get_modereg(w>>LEN_DD);
        }
        if (cmd[i].params & HAS_DD) {
            dd = get_modereg(w);
            w = w >> LEN_DD;
            if (cmd[i].params & HAS_SS) {
                w = w >> LEN_SS;
            }
        }
        if (cmd[i].params & HAS_NN) {
            nn.val = (w & 077);
            w = w >> LEN_NN;
        }
        if (cmd[i].params & HAS_R) {
            r.val = (w & 07);
            w = w >> LEN_R;
            trace("R%o ", r.val);
            if (cmd[i].params & HAS_NN) {
                trace("%o", pc - 2 * nn.val);
            }
        }
        cmd[i].do_func();
        trace("\n");
    }
}