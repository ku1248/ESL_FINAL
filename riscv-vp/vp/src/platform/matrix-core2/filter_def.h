#ifndef MATRIX_DEF_H_
#define MATRIX_DEF_H_

#define CLOCK_PERIOD 10

const int DMA_TRANS = 64;

// Gaussian Filter inner transport addresses
// Used between blocking_transport() & do_filter()
const int MATRIX_MUL_R_ADDR = 0x00000000;
const int MATRIX_MUL_RESULT_ADDR = 0x00000004;

const int GAUSSIAN_FILTER_RS_R_ADDR   = 0x00000000;
const int GAUSSIAN_FILTER_RS_W_WIDTH  = 0x00000004;
const int GAUSSIAN_FILTER_RS_W_HEIGHT = 0x00000008;
const int GAUSSIAN_FILTER_RS_W_DATA   = 0x0000000C;
const int GAUSSIAN_FILTER_RS_RESULT_ADDR = 0x00800000;


union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

#endif
