#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "matrix_mul_def.h"
#define V 6
class Matrix_mul: public sc_module
{
public:
	sc_in_clk i_clk;
	sc_in < bool >  i_rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_uint<8> >::in i_map;
	cynw_p2p< sc_dt::sc_uint<32> >::out o_result;
#else
	sc_fifo_in< sc_dt::sc_uint<8> > i_map;
	sc_fifo_out< sc_dt::sc_uint<32> > o_result;
#endif

	SC_HAS_PROCESS( Matrix_mul );
	Matrix_mul( sc_module_name n );
	~Matrix_mul();
private:
	sc_dt::sc_uint<8> map[8];
	sc_dt::sc_uint<8> tar_num;
	sc_dt::sc_uint<8> ans;
	sc_dt::sc_uint<1> tar_read;
	sc_dt::sc_uint<1> find;
	sc_dt::sc_uint<8> n;
	sc_dt::sc_uint<8> B[64];
	sc_dt::sc_uint<8> A[64];

	void do_matrix_mul();
};
#endif
