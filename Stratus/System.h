#ifndef SYSTEM_H_
#define SYSTEM_H_
#include <systemc>
using namespace sc_core;

#include "Testbench.h"
#ifndef NATIVE_SYSTEMC
#include "Matrix_mul_wrap.h"
#else
#include "Matrix_mul.h"
#endif

class System: public sc_module
{
public:
	SC_HAS_PROCESS( System );
	System( sc_module_name n, std::string input_bmp, std::string output_bmp );
	~System();
private:
  Testbench tb;
#ifndef NATIVE_SYSTEMC
	Matrix_mul_wrapper matrix_mul;
#else
	Matrix_mul matrix_mul;
#endif
	sc_clock clk;
	sc_signal<bool> rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_uint<8> > map;
	cynw_p2p< sc_dt::sc_uint<32> > result;
#else
	sc_fifo< sc_dt::sc_uint<8> > map;
	sc_fifo< sc_dt::sc_uint<32> > result;
#endif
	std::string _output_bmp;

};
#endif
