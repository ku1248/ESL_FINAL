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

// Gaussian Filter ACC
static char* const MATRIXMUL_START_ADDR = reinterpret_cast<char* const>(0x73000000);
static char* const MATRIXMUL_READ_ADDR  = reinterpret_cast<char* const>(0x73000004);

static char* const MATRIXMUL_START_ADDR2 = reinterpret_cast<char* const>(0x74000000);
static char* const MATRIXMUL_READ_ADDR2  = reinterpret_cast<char* const>(0x74000004);
// DMA 
static volatile uint32_t * const DMA_SRC_ADDR  = (uint32_t * const)0x70000000;
static volatile uint32_t * const DMA_DST_ADDR  = (uint32_t * const)0x70000004;
static volatile uint32_t * const DMA_LEN_ADDR  = (uint32_t * const)0x70000008;
static volatile uint32_t * const DMA_OP_ADDR   = (uint32_t * const)0x7000000C;
static volatile uint32_t * const DMA_STAT_ADDR = (uint32_t * const)0x70000010;

static volatile uint32_t * const DMA_SRC_ADDR2  = (uint32_t * const)0x70002000;
static volatile uint32_t * const DMA_DST_ADDR2 = (uint32_t * const)0x70002004;
static volatile uint32_t * const DMA_LEN_ADDR2  = (uint32_t * const)0x70002008;
static volatile uint32_t * const DMA_OP_ADDR2  = (uint32_t * const)0x7000200C;
static volatile uint32_t * const DMA_STAT_ADDR2 = (uint32_t * const)0x70002010;

static const uint32_t DMA_OP_MEMCPY = 1;

bool _is_using_dma = true;

// unsigned int *target_bitmap;
// int write_map(std::string outfile_name) {
//   FILE *fp_t = NULL; // target file handler

//   fp_t = fopen(outfile_name.c_str(), "wb");
//   if (fp_t == NULL) {
//     printf("fopen %s error\n", outfile_name.c_str());
//     return -1;
//   }
//   target_bitmap = (unsigned int *)malloc((size_t)64);
//   fprintf(fp_t, "out matrix:\n");
//   for (int i = 0; i < 8; i++) {
//     for (int j = 0; j < 8; j++) {
//       if (j == 7)
//         fprintf(fp_t, "%d\n", *(target_bitmap+i*8+j));
//       else        
//         fprintf(fp_t, "%d ", *(target_bitmap+i*8+j));
//     }

//     //fprintf(fp_t, "\n");
//   }
//   //fprintf(fp_t, "\n");
//   fclose(fp_t);
//   return 0;
// }

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

void write_data_to_ACC2(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){  
    // Using DMA 
    *DMA_SRC_ADDR2 = (uint32_t)(buffer);
    *DMA_DST_ADDR2 = (uint32_t)(ADDR);
    *DMA_LEN_ADDR2 = len;
    *DMA_OP_ADDR2  = DMA_OP_MEMCPY;
  }else{
    // Directly Send
    memcpy(ADDR, buffer, sizeof(unsigned char)*len);
  }
}

void read_data_from_ACC2(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){
    // Using DMA 
    *DMA_SRC_ADDR2 = (uint32_t)(ADDR);
    *DMA_DST_ADDR2 = (uint32_t)(buffer);
    *DMA_LEN_ADDR2 = len;
    *DMA_OP_ADDR2  = DMA_OP_MEMCPY;
  }else{
    // Directly Read
    memcpy(buffer, ADDR, sizeof(unsigned char)*len);
  }
}

int sem_init (uint32_t *__sem, uint32_t count) __THROW
{
  *__sem=count;
  return 0;
}

int sem_wait (uint32_t *__sem) __THROW
{
  uint32_t value, success; //RV32A
  __asm__ __volatile__("\
L%=:\n\t\
     lr.w %[value],(%[__sem])            # load reserved\n\t\
     beqz %[value],L%=                   # if zero, try again\n\t\
     addi %[value],%[value],-1           # value --\n\t\
     sc.w %[success],%[value],(%[__sem]) # store conditionally\n\t\
     bnez %[success], L%=                # if the store failed, try again\n\t\
"
    : [value] "=r"(value), [success]"=r"(success)
    : [__sem] "r"(__sem)
    : "memory");
  return 0;
}

int sem_post (uint32_t *__sem) __THROW
{
  uint32_t value, success; //RV32A
  __asm__ __volatile__("\
L%=:\n\t\
     lr.w %[value],(%[__sem])            # load reserved\n\t\
     addi %[value],%[value], 1           # value ++\n\t\
     sc.w %[success],%[value],(%[__sem]) # store conditionally\n\t\
     bnez %[success], L%=                # if the store failed, try again\n\t\
"
    : [value] "=r"(value), [success]"=r"(success)
    : [__sem] "r"(__sem)
    : "memory");
  return 0;
}

int barrier(uint32_t *__sem, uint32_t *__lock, uint32_t *counter, uint32_t thread_count) {
	sem_wait(__lock);
	if (*counter == thread_count - 1) { //all finished
		*counter = 0;
		sem_post(__lock);
		for (int j = 0; j < thread_count - 1; ++j) sem_post(__sem);
	} else {
		(*counter)++;
		sem_post(__lock);
		sem_wait(__sem);
	}
	return 0;
}
#define PROCESSORS 2

//the barrier synchronization objects
uint32_t barrier_counter=0; 
uint32_t barrier_lock; 
uint32_t barrier_sem; 
//the mutex object to control global summation
uint32_t lock;  

char A[64] = {85, 84, 31, 54, 60, 5, 94, 59, 34, 16, 19, 45, 36, 32, 42, 77, 83, 14, 6, 28, 50, 98, 71, 38, 99, 4, 95, 40, 62, 17, 63, 1, 88, 82, 78, 52, 68, 91, 41, 75, 23, 97, 10, 18, 2, 93, 57, 44, 81, 92, 86, 100, 30, 80, 48, 73, 55, 49, 8, 33, 9, 15, 61, 47};
char B[64] = {49, 75, 45, 31, 86, 97, 33, 98, 81, 100, 47, 94, 57, 43, 18, 58, 51, 92, 41, 93, 5, 44, 95, 2, 12, 36, 40, 62, 60, 68, 64, 13, 37, 10, 7, 29, 71, 46, 38, 32, 67, 90, 66, 15, 26, 22, 54, 4, 59, 83, 28, 74, 1, 63, 77, 96, 48, 99, 88, 35, 55, 16, 39, 34}; 
int C[64];
int main(unsigned hart_id) {
  unsigned char buffer[4] = {0};
  unsigned char buffer2[4] = {0};

  word data;
  word data2;
  int total;
  if (hart_id == 0) {
    sem_init(&barrier_lock, 1);
		sem_init(&barrier_sem, 0); //lock all cores initially
    sem_init(&lock, 1);
  }
  sem_wait(&lock);
  sem_post(&lock);

  //barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);

  if (hart_id == 0) {
    for (int i = 0; i < 16; i++) {
      buffer[0] = B[i * 4];
      buffer[1] = B[i * 4 + 1];
      buffer[2] = B[i * 4 + 2];
      buffer[3] = B[i * 4 + 3];
      write_data_to_ACC(MATRIXMUL_START_ADDR, buffer, 4);
    }
  }
  else if (hart_id == 1) {
    for (int j = 1; j < PROCESSORS; ++j) {
      for (int i = 0; i < 16; i++) {
        buffer[0] = B[i * 4];
        buffer[1] = B[i * 4 + 1];
        buffer[2] = B[i * 4 + 2];
        buffer[3] = B[i * 4 + 3];
        write_data_to_ACC2(MATRIXMUL_START_ADDR2, buffer, 4);
      }
    }
  }
  //printf("------\n");
  if (hart_id == 0) {
    for (int i = 0; i < 4; i++) {
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
        // *(target_bitmap + i * 8 + k) = C[i * 8 + k];
      } 
    }
  }
  else {
    for (int i = 4; i < 8; i++) {
      for (int j = 0; j < 2; j++) {
        buffer2[0] = A[8 * i + 4 * j];
        buffer2[1] = A[8 * i + 4 * j + 1];
        buffer2[2] = A[8 * i + 4 * j + 2];
        buffer2[3] = A[8 * i + 4 * j + 3];
        
        write_data_to_ACC2(MATRIXMUL_START_ADDR2, buffer2, 4);
      }
      for (int k = 0; k < 8; k++) {
        read_data_from_ACC2(MATRIXMUL_READ_ADDR2, buffer, 4);
        memcpy(data.uc, buffer, 4);
        C[i * 8 + k] = data.sint;
        // *(target_bitmap + i * 8 + k) = C[i * 8 + k];
      } 
    }
  }

  // comment below to observe simulation time more precisely becuase printf occupy too much simulation time

  // barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);
  // if (hart_id == 0) {
  //   printf("out matrix:\n");
  //   for (int i = 0; i < 8; i++) {
  //     for (int j = 0; j < 8; j++) {
  //       printf("%d ", C[i * 8 + j]);
  //     }
  //     printf("\n");
  //   }
  // }
  
}
