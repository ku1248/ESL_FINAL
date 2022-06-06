#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <string>
using namespace std;

#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "matrix_mul_def.h"
#define NODE 6
class Testbench : public sc_module {
public:
	sc_in_clk i_clk;
	sc_out < bool >  o_rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_uint<8> >::base_out o_map;
	cynw_p2p< sc_dt::sc_uint<32> >::base_in i_result;
#else
	sc_fifo_out< sc_dt::sc_uint<8> > o_map;
	sc_fifo_in< sc_dt::sc_uint<32> > i_result;
#endif

  SC_HAS_PROCESS(Testbench);

  Testbench(sc_module_name n);
  ~Testbench();

  int read_map(string infile_name);
  int write_map(string outfile_name);

private:
  unsigned char *source_map;
	unsigned char *source_map2;
  unsigned int *target_map;
	
	unsigned int n_txn;
	sc_time max_txn_time;
	sc_time min_txn_time;
	sc_time total_txn_time;
	sc_time total_start_time;
	sc_time total_run_time;
  
  unsigned long total_latency;
  void feed_map();
	void fetch_result();
  int clock_cycle( sc_time time );
	unsigned char num;
	unsigned char tar;
	unsigned char tar_ans;
	unsigned char is_find;

	// char A[64];
	// char B[64];
	// int C[64];

};
#endif
