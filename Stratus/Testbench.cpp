#include <cstdio>
#include <cstdlib>
using namespace std;

#include "Testbench.h"

Testbench::Testbench(sc_module_name n) : sc_module(n) {
  SC_THREAD(feed_map);
  sensitive << i_clk.pos();
  dont_initialize();
  SC_THREAD(fetch_result);
  sensitive << i_clk.pos();
  dont_initialize();
}

Testbench::~Testbench() {
	//cout<< "Max txn time = " << max_txn_time << endl;
	//cout<< "Min txn time = " << min_txn_time << endl;
	//cout<< "Avg txn time = " << total_txn_time/n_txn << endl;
  cout << "Total latency = " << total_latency << endl;
	cout << "Total run time = " << total_run_time << endl;
}

int Testbench::read_map(string infile_name) {
  FILE *fp_s = NULL; // source file handler
  fp_s = fopen(infile_name.c_str(), "rb");
  if (fp_s == NULL) {
    printf("fopen %s error\n", infile_name.c_str());
    return -1;
  }
  //fscanf(fp_s, "%d", &node);
  //fscanf(fp_s, "%d", &num);
  source_map = (unsigned char *)malloc((size_t)64);
  source_map2 = (unsigned char *)malloc((size_t)64);
  target_map = (unsigned int *)malloc((size_t)64);
  for (int i = 0; i < 64; i++) {
    fscanf(fp_s, "%d", &source_map[i]);
    //printf("%d ", source_map[i]);
  }
  for (int i = 0; i < 64; i++) {
    fscanf(fp_s, "%d", &source_map2[i]);
    //printf("%d ", source_map[i]);
  }
  //fscanf(fp_s, "%d", &tar);
  fclose(fp_s);
  return 0;
}

int Testbench::write_map(string outfile_name) {
  FILE *fp_t = NULL; // target file handler

  fp_t = fopen(outfile_name.c_str(), "wb");
  if (fp_t == NULL) {
    printf("fopen %s error\n", outfile_name.c_str());
    return -1;
  }
  fprintf(fp_t, "out matrix:\n");
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (j == 7)
        fprintf(fp_t, "%d\n", *(target_map+i*8+j));
      else        
        fprintf(fp_t, "%d ", *(target_map+i*8+j));
    }

    //fprintf(fp_t, "\n");
  }
  //fprintf(fp_t, "\n");
  fclose(fp_t);
  return 0;
}

void Testbench::feed_map() {
  //unsigned int x, y, i, j, v, u; // for loop counter
  unsigned char R, G, B;      // color of R, G, B
  int adjustX, adjustY, xBound, yBound;
	n_txn = 0;
	max_txn_time = SC_ZERO_TIME;
	min_txn_time = SC_ZERO_TIME;
	total_txn_time = SC_ZERO_TIME;
  // A[64] = {85, 84, 31, 54, 60, 5, 94, 59, 34, 16, 19, 45, 36, 32, 42, 77, 83, 14, 6, 28, 50, 98, 71, 38, 99, 4, 95, 40, 62, 17, 63, 1, 88, 82, 78, 52, 68, 91, 41, 75, 23, 97, 10, 18, 2, 93, 57, 44, 81, 92, 86, 100, 30, 80, 48, 73, 55, 49, 8, 33, 9, 15, 61, 47};
  // B[64] = {49, 75, 45, 31, 86, 97, 33, 98, 81, 100, 47, 94, 57, 43, 18, 58, 51, 92, 41, 93, 5, 44, 95, 2, 12, 36, 40, 62, 60, 68, 64, 13, 37, 10, 7, 29, 71, 46, 38, 32, 67, 90, 66, 15, 26, 22, 54, 4, 59, 83, 28, 74, 1, 63, 77, 96, 48, 99, 88, 35, 55, 16, 39, 34}; 
#ifndef NATIVE_SYSTEMC
	o_map.reset();
#endif
	o_rst.write(false);
	wait(5);
	o_rst.write(true);
	wait(1);
	total_start_time = sc_time_stamp();
  //o_map.put(tar);
  //o_map.put(num);
  // if (num % 8 == 0)
  //   num = num / 8;
  // else
  //   num = num / 8 + 1;
  for (int j = 0; j != 64; j++) {
#ifndef NATIVE_SYSTEMC
		o_map.put(*(source_map2 + j));
#else
		o_map.write(*(source_map2 + j));
#endif
  }
  for (int i = 0; i != 64; ++i) {
#ifndef NATIVE_SYSTEMC
		o_map.put(*(source_map + i));
#else
		o_map.write(*(source_map + i));
#endif
  }
}

int Testbench::clock_cycle( sc_time time )
{
    sc_clock * clk_p = DCAST < sc_clock * >( i_clk.get_interface() );
    sc_time clock_period = clk_p->period(); // get period from the sc_clock object.
    return ( int )( time / clock_period );

}

void Testbench::fetch_result() {
  //static std::queue<sc_time> time_queue;
  unsigned int x, y, i; // for loop counter
  int total;
  sc_dt::sc_uint<32> tmp;
  //time_queue.push(sc_time_stamp());
#ifndef NATIVE_SYSTEMC
	i_result.reset();
#endif
	wait(5);
	wait(1);
  for (int i = 0; i < 64; i++) {
#ifndef NATIVE_SYSTEMC
  tmp = i_result.get();
#else
  tmp = i_result.read();
#endif
  *(target_map + i) = tmp;
  }
	total_run_time = sc_time_stamp() - total_start_time;
  total_latency = clock_cycle(total_run_time);
  sc_stop();
}
