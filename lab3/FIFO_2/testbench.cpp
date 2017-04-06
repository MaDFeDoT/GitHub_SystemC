//-----------------------------------------------------
// Testbench for the FIFO ---------------->
//-----------------------------------------------------
#include "systemc.h"
#include "FIFOParam.h"

#define soft_assert(signal, expected) \
      if (signal.read() != expected) { \
          cerr << "@" << sc_time_stamp() << " Check failed. Expected: " << expected << ". Actual: " << signal.read() << ".\n" << endl; \
      }

int sc_main(int argc, char* argv[]) {
	sc_core::sc_report_handler::set_actions( "/IEEE_Std_1666/deprecated", sc_core::SC_DO_NOTHING );
    sc_clock clk("clk", 4, SC_NS);
    sc_signal<bool> sreset_n;
    sc_signal<char> data_in;
	sc_signal<bool> push;
	sc_signal<bool> pop;
    sc_signal<char> data_out;
	sc_signal<bool> empty;
	sc_signal<bool> full;
    // Connect the DUT
    FIFOParam<char> test_FIFO("test_FIFO", 2);
    test_FIFO.clk(clk);
    test_FIFO.sreset_n(sreset_n);
    test_FIFO.data_in(data_in);
	test_FIFO.push(push);
	test_FIFO.pop(pop);
    test_FIFO.data_out(data_out);
	test_FIFO.empty(empty);
	test_FIFO.full(full);

    // Open VCD file
    sc_trace_file *wf = sc_create_vcd_trace_file("FIFO_waveform");
    // Dump the desired signals
    sc_trace(wf, clk, "clk");
    sc_trace(wf, sreset_n, "sreset_n");
    sc_trace(wf, data_in, "data_in");
	sc_trace(wf, push, "push");
	sc_trace(wf, pop, "pop");
    sc_trace(wf, data_out, "data_out");
	sc_trace(wf, empty, "empty");
	sc_trace(wf, full, "full");

    sreset_n = 0; // Assert the reset
	push=0;
	pop=0;
    cout << "@" << sc_time_stamp() << " Asserting reset\n" << endl;

    sc_start(6, SC_NS);
//6ns
	assert(empty.read() == 1);
	assert(full.read() == 0);

    sreset_n = 1; // De-assert the reset
    cout << "@" << sc_time_stamp() << " De-Asserting reset\n" << endl;	
	data_in='a';
	push=1;
	sc_start(4, SC_NS);
//10ns
	assert(empty.read() == 0);

	push=0;
	data_in=0;
	pop=1;

	sc_start(4, SC_NS);
//14ns
	assert(empty.read() == 1);
	assert(data_out.read() == 'a');

	data_in='2';
	pop=0;
	push=1;
	sc_start(4, SC_NS);
//18ns
	assert(empty.read() == 0);

	pop=1;
	data_in='f';
	sc_start(4, SC_NS);
//22ns
	assert(data_out.read()  == '2');
	
	pop=0;
	data_in='a';
	sc_start(4, SC_NS);
//26ns
	assert(full.read() == 1);

	pop=1;
	push=0;
	sc_start(4, SC_NS);
//30ns
	assert(full.read() == 0);
	assert(data_out.read() == 'f');

	sc_start(4, SC_NS);

//34ns
	assert(full.read() == 0);
	assert(data_out.read() == 'a');
	assert(empty.read() == 1);

	sc_start(8, SC_NS);
//38ns

	sreset_n = 0; 
	sc_start(17, SC_NS);
	cout << "@" << sc_time_stamp() << " Asserting reset\n" << endl;


    cout << "@" << sc_time_stamp() << " Terminating simulation\n" << endl;
    sc_close_vcd_trace_file(wf);
    return 0; // Terminate simulation
}