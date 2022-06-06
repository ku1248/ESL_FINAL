#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

#include "Matrix_mul.h"

Matrix_mul::Matrix_mul( sc_module_name n ): sc_module( n )
{
#ifndef NATIVE_SYSTEMC
	HLS_FLATTEN_ARRAY(A);
	HLS_FLATTEN_ARRAY(B);
#endif
	SC_THREAD( do_matrix_mul );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
        
#ifndef NATIVE_SYSTEMC
	i_map.clk_rst(i_clk, i_rst);
  o_result.clk_rst(i_clk, i_rst);
#endif
}

Matrix_mul::~Matrix_mul() {}

void Matrix_mul::do_matrix_mul() {
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_map.reset();
		o_result.reset();
#endif
		wait();
	}
	while (true) {
		#if defined(II)
			HLS_PIPELINE_LOOP( SOFT_STALL, II, "main_loop_pipeline" ); 
		#endif
		for (unsigned i = 0; i < 64; i++) {
#ifndef NATIVE_SYSTEMC
			{
				HLS_DEFINE_PROTOCOL("input");
				B[i] = i_map.get();
				wait();
			}
#else
			B[i] = i_map.read();
#endif
		}
		
		sc_dt::sc_uint<32> result;
		sc_dt::sc_uint<32> result2;
		sc_dt::sc_uint<32> result3;
		sc_dt::sc_uint<32> result4;
		int tmp;
		for (unsigned int i = 0; i < 8; i++) {
			#ifdef UNROLL
			#   define MAIN_LOOP  HLS_UNROLL_LOOP(COMPLETE, 2, "my_loop")
			#endif
			#ifndef UNROLL
			#   define MAIN_LOOP
			#endif
			MAIN_LOOP;
			for (unsigned int j = 0; j < 2; j++) {
				for (unsigned int k = 0; k < 4; k++) {
#ifndef NATIVE_SYSTEMC
					{
						HLS_DEFINE_PROTOCOL("input");
						A[j*4+k] = i_map.get();
						wait();
					}
#else
					A[j*4+k] = i_map.read();
#endif
				}
			}
			for (int j = 0; j < 2; j++) {
					HLS_CONSTRAIN_LATENCY(0, 6, "lat00");
					//result = 0;
					result = (int) A[0] * B[j*4] + A[1] * B[8+j*4] + A[2] * B[16+4*2] + A[3] * B[24+j*4] + A[4] * B[32+j*4] + A[5] * B[40+j*4] + A[6] * B[48+j*4] + A[7] * B[56+j*4];
					result2 = (int) A[0] * B[j*4+1] + A[1] * B[8+j*4+1] + A[2] * B[16+j*4+1] + A[3] * B[24+j*4+1] + A[4] * B[32+j*4+1] + A[5] * B[40+j*4+1] + A[6] * B[48+j*4+1] + A[7] * B[56+j*4+1];
					result3 = (int) A[0] * B[j*4+2] + A[1] * B[8+j*4+2] + A[2] * B[16+j*4+2] + A[3] * B[24+j*4+2] + A[4] * B[32+j*4+2] + A[5] * B[40+j*4+2] + A[6] * B[48+j*4+2] + A[7] * B[56+j*4+2];
					result4 = (int) A[0] * B[j*4+3] + A[1] * B[8+j*4+3] + A[2] * B[16+j*4+3] + A[3] * B[24+j*4+3] + A[4] * B[32+j*4+3] + A[5] * B[40+j*4+3] + A[6] * B[48+j*4+3] + A[7] * B[56+j*4+3];

					//tmp = result;
					//printf("%d\n", tmp);
#ifndef NATIVE_SYSTEMC
					{
						HLS_DEFINE_PROTOCOL("output");
						o_result.put(result);
						o_result.put(result2);
						o_result.put(result3);
						o_result.put(result4);
						wait();
					}
#else
					o_result.write(result);
					o_result.write(result2);
					o_result.write(result3);
					o_result.write(result4);
#endif
			}
		}
		// for (unsigned int i = 0; i < 8; i++) {
		// 	for (unsigned int j = 0; j < 8; j++) {
		// 		result = 0;	
		// 		for (unsigned int k = 0; k < 4; k++) {
		// 			HLS_CONSTRAIN_LATENCY(0, 3, "lat00");
		// 			// #ifdef UNROLL
		// 			// #   define MAIN_LOOP  HLS_UNROLL_LOOP(ALL, "my_loop")
		// 			// #endif
		// 			// #ifndef UNROLL
		// 			// #   define MAIN_LOOP
		// 			// #endif
		// 			// MAIN_LOOP;
		// 			result += A[i*8+k*2] * B[16*k+j] + A[i*8+k*2+1] * B[8*(k*2+1)+j];
		// 			//tmp = 1;

		// 		}
		// 		//result.range(31, 0) = result;
// #ifndef NATIVE_SYSTEMC
// 				{
// 					HLS_DEFINE_PROTOCOL("output");
// 					o_result.put(result);
// 					wait();
// 				}
// #else
// 				o_result.write(result);
// #endif
	}
}
