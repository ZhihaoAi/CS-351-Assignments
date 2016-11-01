#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAXLEN 80
#define WORD 64

typedef struct {
    int valid;
    int tag;
    int count;
} line;

int hit, miss, evic;
int s, E, b;
int verbose;
line** cache;

void help(){
    printf("\
Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n\
Options:\n\
-h              Optional help flag that prints usage info\n\
-v              Optional verbose flag that displays trace info\n\
-s <s>          Number of set index bits\n\
-E <E>          Associativity\n\
-b <b>          Number of block bits\n\
-t <tracefile>  Name of the valgrind trace to replay\n");
}

void init(){
    int S=1<<s;
    cache = malloc (S * sizeof(line*));
    for (int i = 0; i < S; i++) {
        cache[i] = malloc(E * sizeof(line));
        for (int j = 0; j < E; j++) {
            cache[i][j].valid = 0;
        }
    }
}

void eval(int argc, char* argv[], char* tracefile){
    char opt;
    while ((opt = getopt(argc,argv,"hvs:E:b:t:")) != EOF) {
        switch (opt) {
            case 'h':
                help();
                break;
            case 'v':
                verbose=1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                strcpy(tracefile, optarg);
                break;
            default:
                break;
        }
    }
}

void updateCount(int set, int i){
    for (int j=0; j < E; j++) {
        if ((cache[set][j].valid == 1) && 
            (cache[set][j].count > cache[set][i].count)) {
           cache[set][j].count--;
        }
    } 
    cache[set][i].count = E;
}

void run(char *command{
    char opt;
    unsigned long address, tag;
    int set, i;

    sscanf(command, " %c %lx", &opt, &address);
    set = (address << (WORD - s - b)) >> (WORD - s);
    tag = address >> (s + b);

    for (i = 0; i < E; i++) {
        if ((cache[set][i].valid) &&
            (cache[set][i].tag == tag)) {
            // hit
            hit += (opt == 'M') ? 2 : 1;
            updateCount(set, i);
            return;
        }
    } 

    miss++;
    for (i = 0; i < E; i++) {
        if (!cache[set][i].valid) {
            // no need for eviction
            cache[set][i].valid = 1;
            cache[set][i].tag = tag;
            
            if (opt == 'M')
                hit++;
            updateCount(set, i);
            return;
        }
    }

    evic++;
    // miss + eviction
    for (i = 0; i < E; i++) {
      if (cache[set][i].count == 1) {
            cache[set][i].tag = tag;
            if (opt == 'M')
                hit++;
            updateCount(set, i);
            return;
        }
    }
}

int main(int argc, char* argv[]){
    char tracefile[MAXLEN];
    char command[MAXLEN];
    FILE *file;

    eval(argc, argv, tracefile);
    if(!(s && E && b)){
        help();
        exit(1);
    }

    file = fopen(tracefile, "r");
    if(!file){
        printf("%s: No such file.\n", tracefile);
        exit(1);
    }

    init();
    while (fgets(command, MAXLEN, file)){
        if (command[0] == ' '){
            command[strlen(command)-1] = '\0';
            run(command);
        }
    }
    fclose(file);
    printSummary(hit, miss, evic);
    return 0;
}
