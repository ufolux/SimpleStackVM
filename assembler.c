#include <stdio.h>
#include <getopt.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ExecStack.h"

/**
 *  https://blog.csdn.net/judyge/article/details/41171673
 *  https://www.cnblogs.com/unixfy/p/3357761.html
 *  基于堆栈的指令集
 *  +──────+──────────────+────────────────────────────────────────+
 *  | 操作符  | 操作数个数        | 说明
 *  +──────+──────────────+────────────────────────────────────────+
 *  | HALT | 0，HALT       | 终止
 *  | IN   | 0，IN         | 从标准输入中读入整型值并压栈
 *  | OUT  | 0，OUT        | 从栈中弹出，从标准输出
 *  | ADD  | 0，ADD        | 从栈中弹出a，弹出b，计算b+a，并将结果压入栈中
 *  | SUB  | 0，SUB        | 从栈中弹出a，弹出b，计算b-a，并将结果压入栈中
 *  | MUL  | 0，MUL        | 从栈中弹出a，弹出b，计算b*a，并将结果压入栈中
 *  | DIV  | 0，DIV        | 从栈中弹出a，弹出b，计算b/a，并将结果压入栈中
 *  | DUP  | 0，DUP        | 压入栈顶值的拷贝
 *  | LD   | 0，LD         | 从栈中弹出地址，并压入该地址里的整数值
 *  | ST   | 0，ST         | 从栈中弹出值，再弹出地址，并将该值存储到该地址中
 *  | LDC  | 1，LDC value  | 压入value
 *  | JLT  | 1，JLT loc    | 弹出value，检测value是否小于0，如果小于，则pc=loc
 *  | JLE  | 1，JLE loc    | 弹出value，检测value是否小于等于0，如果小于等于，则pc=loc
 *  | JGT  | 1，JGT loc    | 弹出value，检测value是否大于0，如果大于，则pc=loc
 *  | JGE  | 1，JGE loc    | 弹出value，检测value是否大于等于0，如果大于等于，则pc=loc
 *  | JEQ  | 1，JEQ loc    | 弹出value，检测value是否等于0，如果等于，则pc=loc
 *  | JNE  | 1，JNE loc    | 弹出value，检测value是否不等于0，如果不等于，则pc=loc
 *  | JMP  | 0，JMP        | 不用弹出value，pc=loc
 *  +──────+──────────────+────────────────────────────────────────+
 *
 *  SM中还是有一个寄存器，即指令指针寄存器(pc)，永远指向将要执行的指令。在实现中，
 *  所有指令都被保存一个数组里，所以pc就是一个指向数组索引的整数值。
 */

typedef struct Instruction {
    char *opcode;
    int arg;
} Instruction;

/// global variable

FILE *fp = NULL;
ExecStack *execStack;
bool vFlag = false;

/// End global variable

void execHALT() {
    exit(0);
}

void execIN() {
    // FIXME: can't accept any length str
    char buff[20];
    fgets(buff, sizeof(buff), stdin);
    int oprand = atoi(buff);
    es_push(execStack, oprand);
    fflush(stdin);
}

void execOUT() {
    int top = es_pop(execStack);
    printf("%d\n", top);
    fflush(stdout);
}

void execADD() {
    int b = es_pop(execStack);
    int a = es_pop(execStack);
    es_push(execStack, b + a);
}

void execSUB() {
    int b = es_pop(execStack);
    int a = es_pop(execStack);
    es_push(execStack, b - a);
}

void execMUL() {
    int b = es_pop(execStack);
    int a = es_pop(execStack);
    es_push(execStack, b * a);
}

void execDIV() {
    int b = es_pop(execStack);
    int a = es_pop(execStack);
    es_push(execStack, b / a);
}

void execDUP() {
    int val = es_peek(execStack);
    es_push(execStack, val);
}

void execLD() {
    int *addr = (int *)es_pop(execStack);
    es_push(execStack, *addr);
}

void execST() {
    int val = es_pop(execStack);
    int *addr = (int *)es_pop(execStack);
    *addr = val;
}

void execLDC(int arg) {
    es_push(execStack, arg);
}

void execJLT(int loc) {
    int val = es_pop(execStack);
    if (val < 0) {
        execStack->pc = (int *) loc;
    }
}

void execJLE(int loc) {
    int val = es_pop(execStack);
    if (val <= 0) {
        execStack->pc = (int *) loc;
    }
}

void execJGT(int loc) {
    int val = es_pop(execStack);
    if (val > 0) {
        execStack->pc = (int *) loc;
    }
}

void execJGE(int loc) {
    int val = es_pop(execStack);
    if (val >= 0) {
        execStack->pc = (int *) loc;
    }
}

void execJEQ(int loc) {
    int val = es_pop(execStack);
    if (val == 0) {
        execStack->pc = (int *) loc;
    }
}

void execJNE(int loc) {
    int val = es_pop(execStack);
    if (val != 0) {
        execStack->pc = (int *) loc;
    }
}

void execJMP(int loc) {
    execStack->pc = (int *) loc;
}

void execInstr(Instruction *instr) {
    const char *opcode = instr->opcode;
    const int arg = instr->arg;
    if (strcmp(opcode, "HALT") == 0) {
        execHALT();
    } else if (strcmp(opcode, "IN") == 0) {
        execIN();
    } else if (strcmp(opcode, "OUT") == 0) {
        execOUT();
    } else if (strcmp(opcode, "ADD") == 0) {
        execADD();
    } else if (strcmp(opcode, "SUB") == 0) {
        execSUB();
    } else if (strcmp(opcode, "MUL") == 0) {
        execMUL();
    } else if (strcmp(opcode, "DIV") == 0) {
        execDIV();
    } else if (strcmp(opcode, "DUP") == 0) {
        execDUP();
    } else if (strcmp(opcode, "LD") == 0) {
        execLD();
    } else if (strcmp(opcode, "ST") == 0) {
        execST();
    } else if (strcmp(opcode, "LDC") == 0) {
        execLDC(arg);
    } else if (strcmp(opcode, "JLT") == 0) {
        execJLT(arg);
    } else if (strcmp(opcode, "JLE") == 0) {
        execJLE(arg);
    } else if (strcmp(opcode, "JGT") == 0) {
        execJGT(arg);
    } else if (strcmp(opcode, "JGE") == 0) {
        execJGE(arg);
    } else if (strcmp(opcode, "JEQ") == 0) {
        execJEQ(arg);
    } else if (strcmp(opcode, "JNE") == 0) {
        execJNE(arg);
    } else if (strcmp(opcode, "JMP") == 0) {
        execJMP(arg);
    } else {
        puts("Fatal: Illegal Instruction");
        exit(-1);
    }
}

int consumeInputs(const char *filepath) {
    fp = fopen(filepath, "r");
    assert(fp);
    char line[100];
    const char *delimiter = " \t\r\n";
    while (fgets(line, sizeof(line), fp) != NULL) {
        Instruction *instr = (Instruction *) malloc(sizeof(Instruction));
        assert(instr);
        instr->opcode = NULL;
        instr->arg = 0;

        char *tk;
        tk = strtok(line, delimiter);

        // cpy opcode
        int tkl = (int) strlen(tk);
        instr->opcode = malloc(sizeof(char) * (tkl + 1));
        assert(instr->opcode);
        strncpy(instr->opcode, tk, tkl);
        instr->opcode[tkl] = 0;

        tk = strtok(NULL, delimiter);
        if (tk) {
            // cpy arg
            instr->arg = atoll(tk);
        }

        execInstr(instr);

        free(instr->opcode);
        free(instr);
    }
}

void initEnv() {
    execStack = es_new();
}

void cleanEnv() {
    es_destroy(execStack);
}

int main(int argc, char *argv[]) {
    initEnv();
    int c;
    while ((c = getopt(argc, argv, "v")) != -1) {
        switch (c) {
            case 'v':
                vFlag = true;
                break;
        }
    }
    const char *filename = argv[optind];
    consumeInputs(filename);
    cleanEnv();
    return 0;
}
