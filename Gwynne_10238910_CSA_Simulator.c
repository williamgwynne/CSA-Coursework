//William Gwynne
//ID 10238910
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <math.h>

struct block
{
    bool valid; //0 denotes invalid
    bool dirty; //0 denotes not dirty
    int tag_bits;
    //block naturally contains no data as no data exists
} ;

//global because it's easier
char trace_file_name[] = "bubble_sort_trace_049.trc"; //The name of the trace file being analysed (without the folder path)
uint32_t mode_ID; //The ID number of the cache controller configuration mode (1 … 12)
uint32_t NRA = 0; //Total number of read accesses to the external memory
uint32_t NWA = 0; //Total number of write accesses to the external memory
uint32_t NCRH = 0; //Number of cache read hits
uint32_t NCRM = 0; //Number of cache read misses
uint32_t NCWH = 0; //Number of cache write hits
uint32_t NCWM = 0; //Number of cache write misses

void access(int cache_size, int block_size) {
    struct block cache[cache_size]; //4 blocks
    for (int i=0; i<cache_size; i++){ //initialising cache
        cache[i].valid = false;
        cache[i].dirty = false;
        cache[i].tag_bits = 0;
    }
    NRA = 0;
    NWA = 0;
    NCRH = 0;
    NCRM = 0;
    NCWH = 0;
    NCWM = 0;
    FILE *fp;
    fp = fopen(trace_file_name, "r"); //opening the trace file
    char operation; //either R or W
    int mm_address;
    while (!feof(fp)){
    //for (int i=0; i<10; i++) { //for-loop structure used for debugging only
        fscanf(fp, "%c %x\n", &operation, &mm_address); //reads in each line, stores operation(r/w) and address
        int MMBID = mm_address/block_size;
        int CMBID = MMBID % cache_size;
        int block_offset = mm_address - MMBID*block_size;
        int tag_bits = MMBID >> (int)log2(cache_size); //bit-shifting to remove CMBID from MMBID to extract tag bits
        bool cacheHit;
        //printf("MMBID = 0x%04x\nCMBID = 0x%04x\nOffset = 0x%04x\nTag bits = 0x%04x\n", MMBID, CMBID, block_offset, tag_bits); //used for debugging
        if ((cache[CMBID].valid) && (cache[CMBID].tag_bits == tag_bits)) { //i.e. cache hit (valid bit is true and MMBIDs are same); note, comparing tag bits as CMBID will always be the same for MMBID comparisons
            //printf("Cache HIT\n\n"); //used for debugging
            cacheHit = true;
        } else { //cache miss, so cache replacement occurs
            //printf("Cache MISS\n\n"); //used for debugging
            if (cache[CMBID].dirty) //dirty bit used for write-back policy, when replacing a page the dirty bit==1 should increment NWA
                NWA+=block_size; //increments number of main-memory write accesses, whole block is replaced so needs to increment by block size
            NRA+=block_size; //increments number of read accesses by the block size

            cache[CMBID].tag_bits = tag_bits;
            cache[CMBID].valid = true;
            cache[CMBID].dirty = false; //not actually recording write-back operations but it would occur here
            cacheHit = false;
        }
        if (operation == 'R') {
            if (cacheHit)
                NCRH++;
            else //miss
            {
                NCRM++;
            }
        } else if (operation = 'W') {
            cache[CMBID].dirty = true; //dirty bit indicates data in cache is not the same as main memory, occurs at any cached read operation
            if (cacheHit)
                NCWH++;
            else //miss
                NCWM++;
        }
    }
    printf("Cache Size = %d blocks\nBlock Size = %d words\n", cache_size, block_size);
    printf("NRA = %d\tNWA = %d\nNCRH = %d\tNCWH = %d\nNCRM = %d\tNCWM = %d\n\n", NRA, NWA, NCRH, NCWH, NCRM, NCWM);
    fclose(fp); //closing the trace file
}

int main()
{
    char outputFileName[] = "Gwynne_10238910_CSA_Results.csv";
    FILE *fp;
    fp = fopen(outputFileName, "w");
    fprintf(fp, "trace_file_name, mode_ID, NRA, NWA, NCRH, NCWH, NCRM, NCWM\n");

    for (int mode=1; mode<=12; mode++)
    {
        printf("Mode: %d\n", mode);
        switch(mode) //switches between cache configurations
            { //used a switch statement as I wasn't sure if mode was selected by user or if the program just cycles through all modes; if it was chosen by user, for loop would be replaced with prompt
            case 1:
                access(128, 4); //128 blocks of 4 words
                break;
            case 2:
                access(64, 8); //64 blocks of 8 words
                break;
            case 3:
                access(32, 16); //32 blocks of 16 words
                break;
            case 4:
                access(16, 32); //16 blocks of 32 words
                break;
            case 5:
                access(8, 4); //8 blocks of 64 words
                break;
            case 6:
                access(4, 128); //4 blocks of 128 words
                break;
            case 7:
                access(4, 16); //4 blocks of 16 words
                break;
            case 8:
                access(8, 16); //8 blocks of 16 words
                break;
            case 9:
                access(16, 16); //16 blocks of 16 words
                break;
            case 10:
                access(64, 16); //64 blocks of 16 words
                break;
            case 11:
                access(128, 16); //128 blocks of 16 words
                break;
            case 12:
                access(256, 16); //256 blocks of 16 words
            break;
            }
        fprintf(fp, "%s, %d, %d, %d, %d, %d, %d, %d\n", trace_file_name, mode, NRA, NWA, NCRH, NCWH, NCRM, NCWM); //after each mode, results are stored in csv file
    }
    fclose(fp); //closing CSV file

    //access(4, 16); //test configuration

    return 0;
}
