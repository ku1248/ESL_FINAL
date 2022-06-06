#include "string"
#include "string.h"
#include "cassert"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

static char* const MATRIXMUL_START_ADDR = reinterpret_cast<char* const>(0x73000000);
static char* const MATRIXMUL_READ_ADDR  = reinterpret_cast<char* const>(0x73000004);

// DMA 
static volatile uint32_t * const DMA_SRC_ADDR  = (uint32_t * const)0x70000000;
static volatile uint32_t * const DMA_DST_ADDR  = (uint32_t * const)0x70000004;
static volatile uint32_t * const DMA_LEN_ADDR  = (uint32_t * const)0x70000008;
static volatile uint32_t * const DMA_OP_ADDR   = (uint32_t * const)0x7000000C;
static volatile uint32_t * const DMA_STAT_ADDR = (uint32_t * const)0x70000010;
static const uint32_t DMA_OP_MEMCPY = 1;

bool _is_using_dma = true;

void write_data_to_ACC(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){  
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(buffer);
    *DMA_DST_ADDR = (uint32_t)(ADDR);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
  }else{
    // Directly Send
    memcpy(ADDR, buffer, sizeof(unsigned char)*len);
  }
}
void read_data_from_ACC(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(ADDR);
    *DMA_DST_ADDR = (uint32_t)(buffer);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
  }else{
    // Directly Read
    memcpy(buffer, ADDR, sizeof(unsigned char)*len);
  }
}

char A[64] = {85, 84, 31, 54, 60, 5, 94, 59, 34, 16, 19, 45, 36, 32, 42, 77, 83, 14, 6, 28, 50, 98, 71, 38, 99, 4, 95, 40, 62, 17, 63, 1, 88, 82, 78, 52, 68, 91, 41, 75, 23, 97, 10, 18, 2, 93, 57, 44, 81, 92, 86, 100, 30, 80, 48, 73, 55, 49, 8, 33, 9, 15, 61, 47};
char B[64] = {49, 75, 45, 31, 86, 97, 33, 98, 81, 100, 47, 94, 57, 43, 18, 58, 51, 92, 41, 93, 5, 44, 95, 2, 12, 36, 40, 62, 60, 68, 64, 13, 37, 10, 7, 29, 71, 46, 38, 32, 67, 90, 66, 15, 26, 22, 54, 4, 59, 83, 28, 74, 1, 63, 77, 96, 48, 99, 88, 35, 55, 16, 39, 34}; 
int C[64];

int main(int argc, char *argv[]) {
  unsigned char  buffer[4] = {0};
  word data;
  int total;

  for (int i = 0; i < 16; i++) {
    buffer[0] = B[i * 4];
    buffer[1] = B[i * 4 + 1];
    buffer[2] = B[i * 4 + 2];
    buffer[3] = B[i * 4 + 3];
    write_data_to_ACC(MATRIXMUL_START_ADDR, buffer, 4);
  }
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 2; j++) {
      buffer[0] = A[8 * i + 4 * j];
      buffer[1] = A[8 * i + 4 * j + 1];
      buffer[2] = A[8 * i + 4 * j + 2];
      buffer[3] = A[8 * i + 4 * j + 3];
      write_data_to_ACC(MATRIXMUL_START_ADDR, buffer, 4);
    }
    for (int k = 0; k < 8; k++) {
      read_data_from_ACC(MATRIXMUL_READ_ADDR, buffer, 4);
      memcpy(data.uc, buffer, 4);
      C[i * 8 + k] = data.sint;
    } 
  }
  // comment below to observe simulation time more precisely becuase printf occupy too much simulation time
  printf("out matrix:\n");
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      printf("%d ", C[i * 8 + j]);
    }
    printf("\n");
  }
}
