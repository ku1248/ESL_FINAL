#ifndef MATRIX_MUL_H_
#define MATRIX_MUL_H_
#include <systemc>
#include <cmath>
#include <iomanip>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

struct MatrixMul : public sc_module {
  tlm_utils::simple_target_socket<MatrixMul> tsock;

  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  sc_fifo<unsigned char> i_x;
  sc_fifo<int> o_result_r;
  sc_fifo<int> o_result_g;
  sc_fifo<int> o_result_b;
  sc_fifo<int> o_result_x;
  SC_HAS_PROCESS(MatrixMul);

  MatrixMul(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &MatrixMul::blocking_transport);
    SC_THREAD(do_filter);
  }

  ~MatrixMul() {
	}
  
  unsigned int base_offset;

  void do_filter(){
    { wait(CLOCK_PERIOD, SC_NS); }
    while (true) {
      char B[64];
      char A[8];
      int result[8];
      for (int i = 0; i < 16; i++) {
        B[i * 4] = i_r.read();
        B[i * 4 + 1] = i_g.read();
        B[i * 4 + 2] = i_b.read();
        B[i * 4 + 3] = i_x.read();
        // printf("B:%d %d %d %d\n", B[i*4], B[i*4+1], B[i*4+2], B[i*4+3]);
      }
      for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 2; i++) {
          A[i*4] = i_r.read();
          A[i*4+1] = i_g.read();
          A[i*4+2] = i_b.read();
          A[i*4+3] = i_x.read();
          // printf("A:%d %d %d %d\n", A[i*4], A[i*4+1], A[i*4+2], A[i*4+3]);
        }
        for (int i = 0; i < 8; i++) {
          result[i] = 0;
          result[i] += (int) A[0] * B[i] + A[1] * B[8+i] + A[2] * B[16+i] + A[3] * B[24+i] + A[4] * B[32+i] + A[5] * B[40+i] + A[6] * B[48+i] + A[7] * B[56+i];
          //printf("result[%d]=%d\n", i, result[i]);
          //wait(CLOCK_PERIOD, SC_NS);
          //printf("%d\n", result[i]);
          //printf("result2[%d]=%d\n", i, result[i]);
          o_result_x.write(result[i]);
          //printf("result2[%d]=%d\n", i, o_result_x.read());
          wait(CLOCK_PERIOD, SC_NS);
        }
      }
      // for (int i = 0; i < 8; i++) {
      //   o_result_x.write(result[i]);
      // }
    }
  }

  void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay){
    wait(delay);
    // unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    // auto len = payload.get_data_length();
    tlm::tlm_command cmd = payload.get_command();
    sc_dt::uint64 addr = payload.get_address();
    unsigned char *data_ptr = payload.get_data_ptr();

    addr -= base_offset;


    // cout << (int)data_ptr[0] << endl;
    // cout << (int)data_ptr[1] << endl;
    // cout << (int)data_ptr[2] << endl;
    word buffer;

    switch (cmd) {
      case tlm::TLM_READ_COMMAND:
        // cout << "READ" << endl;
        switch (addr) {
          case MATRIX_MUL_RESULT_ADDR:
            buffer.sint = o_result_x.read();
            // printf("buffer:%d\n", buffer.sint);
            break;
          default:
            std::cerr << "READ Error! MatrixMul::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
        }
        data_ptr[0] = buffer.uc[0];
        data_ptr[1] = buffer.uc[1];
        data_ptr[2] = buffer.uc[2];
        data_ptr[3] = buffer.uc[3];
        break;
      case tlm::TLM_WRITE_COMMAND:
        // cout << "WRITE" << endl;
        switch (addr) {
          case MATRIX_MUL_R_ADDR:
            i_r.write(data_ptr[0]);
            i_g.write(data_ptr[1]);
            i_b.write(data_ptr[2]);
            i_x.write(data_ptr[3]);
            break;
          default:
            std::cerr << "WRITE Error! MatrixMul::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
        }
        break;
      case tlm::TLM_IGNORE_COMMAND:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      default:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      }
      payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
  }
};
#endif
