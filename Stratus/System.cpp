#include "System.h"
System::System( sc_module_name n, string input_bmp, string output_bmp ): sc_module( n ), 
	tb("tb"), matrix_mul("matrix_mul"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst"), _output_bmp(output_bmp)
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	matrix_mul.i_clk(clk);
	matrix_mul.i_rst(rst);
	tb.o_map(map);
	tb.i_result(result);
	matrix_mul.i_map(map);
	matrix_mul.o_result(result);

  tb.read_map(input_bmp);
}

System::~System() {
  tb.write_map(_output_bmp);
}
