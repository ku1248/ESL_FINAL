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
		for (unsigned i = 0; i < 64; i++) {
#ifndef NATIVE_SYSTEMC
			{
				HLS_DEFINE_PROTOCOL("input");
				A[i] = i_map.get();
				wait();
			}
#else
			A[i] = i_map.read();
#endif
		}
		sc_dt::sc_uint<32> result;
		for (unsigned int i = 0; i < 8; i++) {
			for (unsigned int j = 0; j < 8; j++) {
				result = 0;
				for (unsigned int k = 0; k < 4; k++) {
					HLS_CONSTRAIN_LATENCY(0, 3, "lat01");
				
					#ifdef UNROLL
					#   define MAIN_LOOP  HLS_UNROLL_LOOP(ALL, "my_loop")
					#endif
					#ifndef UNROLL
					#   define MAIN_LOOP
					#endif
					MAIN_LOOP;
					result += A[i*8+k*2] * B[16*k+j] + A[i*8+k*2+1] * B[8*(k*2+1)+j];
					//result = A[i*8] * B[j] + A[i*8+1] * B[8+j] + A[i*8+2] * B[16+j] + A[i*8+3] * B[24+j] + A[i*8+4] * B[32+j] + A[i*8+5] * B[40+j] + A[i*8+6] * B[48+j] + A[i*8+7] * B[56+j];
					// if (k == 0) {
					// 	result = A[i*8] * B[j] + A[i*8+1] * B[8+j];
					// 	// tmp = result;
					// 	// printf("%d %d\n",k, tmp);
					// }
					// else if (k == 1) {
					// 	result += A[i*8+2] * B[16+j] + A[i*8+3] * B[24+j];
					// 	// tmp = result;
					// 	// printf("%d %d\n",k, tmp);
					// }
					// else if (k == 2) {
					// 	result += A[i*8+4] * B[32+j] + A[i*8+5] * B[40+j];
					// 	// tmp = result;
					// 	// printf("%d %d\n",k, tmp);
					// }
					// else if (k == 3) {
					// 	result += A[i*8+6] * B[48+j] + A[i*8+7] * B[56+j];
					// 	// tmp = result;
					// 	// printf("%d %d\n",k, tmp);
					// }
					// tmp = result;
					// printf("%d\n", tmp);
				}
#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("output");
					o_result.put(result);
					wait();
				}
#else
				o_result.write(result);
#endif
			}
		}
// 		for (unsigned int i = 0; i<8; ++i) {
// 			HLS_CONSTRAIN_LATENCY(0, 1, "lat00");
// 			map[i] = 0;
// 		}
// 		find = 0;
// 		ans = 0;
// 		sc_dt::sc_uint<8> n_ = n;
// 		if ((n.range(2, 0) || 0) == 0) {
// 			n = n >> 3;
// 		}
// 		else {
// 			n = (n >> 3) + 1;
// 		}
		
// 		for (int i = 0; i < n; i++) {
// 			#if defined(II)
// 			HLS_PIPELINE_LOOP( SOFT_STALL, II, "main_loop_pipeline" ); 
// 			#endif
// 			{for (unsigned int j = 0; j<8; ++j) {
			
// #ifndef NATIVE_SYSTEMC
// 				{
// 					HLS_DEFINE_PROTOCOL("input");
// 					map[j] = i_map.get();
// 					int b = map[j];
// 					wait();
// 				}
// #else
// 				map[j] = i_map.read();
// #endif
// 			}
// 			}
// 			#ifdef UNROLL
// 			#   define MAIN_LOOP  HLS_UNROLL_LOOP(ALL, "my_loop")
// 			#endif
// 			#ifndef UNROLL
// 			#   define MAIN_LOOP
// 			#endif
// 			MAIN_LOOP;
// 			{
// 			sc_dt::sc_uint<8> mid;
// 			sc_dt::sc_uint<8> start = 0;
// 			sc_dt::sc_uint<8> end = 7;
// 			for (unsigned int k = 0; k < 4; k++) {						
// 				mid = (start + end) >> 1;
// 				HLS_CONSTRAIN_LATENCY(0, 2, "lat01");
// 				if(map[mid] == tar_num && ans < n_) {
// 					ans = 8 * i+ mid;
// 					find = 1;
// 				}
// 				else if (map[mid] > tar_num) {
// 					end = mid - 1;
// 				}
// 				else {
// 					start = mid + 1;
// 				}
// 			}
// 			}
// 		}
	}
}
