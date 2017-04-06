//-----------------------------------------------------
// Testbench for the statemachine ---------------->
//-----------------------------------------------------
#include "systemc.h"
#include "statemashine.h"

#define soft_assert(signal, expected) \
      if (signal.read() != expected) { \
          cerr << "@" << sc_time_stamp() << " Check failed. Expected: " << expected << ". Actual: " << signal.read() << ".\n" << endl; \
      }

int sc_main(int argc, char* argv[]) {
    sc_clock clock("clock", 4, SC_NS);
    sc_signal<bool> reset;
    sc_signal<sc_uint<2> > register_in;
    sc_signal<sc_uint<2> > register_out;
    int i = 0;
    // Connect the DUT
    statemashine test_statemashine("test_statemashine");
    test_statemashine.clock(clock);
    test_statemashine.reset(reset);
    test_statemashine.register_in(register_in);
    test_statemashine.register_out(register_out);

    // Open VCD file
    sc_trace_file *wf = sc_create_vcd_trace_file("statemashine_waveform");
    // Dump the desired signals
    sc_trace(wf, clock, "clock");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, register_in, "din");
    sc_trace(wf, register_out, "dout");
	sc_trace(wf, test_statemashine.state, "state");

    reset = 1; // Assert the reset
    cout << "@" << sc_time_stamp() << " Asserting reset\n" << endl;

    sc_start(6, SC_NS);

    reset = 0; // De-assert the reset
    cout << "@" << sc_time_stamp() << " De-Asserting reset\n" << endl;
	
	assert(register_out.read() == 1);
	
	//from state 0 to state 1 signal 2
	register_in = 2; 
	sc_start(4, SC_NS);
	assert(register_out.read() == 2);

	//from state 1 to state 1 signal 2
	sc_start(4, SC_NS);
	assert(register_out.read() == 2);

	//from state 1 to state 0 signal 0
	register_in = 0; 
	sc_start(4, SC_NS);
	assert(register_out.read() == 1);

	//from state 0 to state 1 signal 1
	register_in = 1; 
	sc_start(4, SC_NS);
	assert(register_out.read() == 2);

	//from state 1 to state 2 signal 1
	//register_in = 1; 
	sc_start(4, SC_NS);
	assert(register_out.read() == 1);

	//from state 2 to state 0 signal 2
	register_in = 2; 
	sc_start(4, SC_NS);
	assert(register_out.read() == 1);

	//from state 0 to state 3 signal 0
	register_in = 0; 
	sc_start(4, SC_NS);
	assert(register_out.read() == 2);

	//from state 3 to state 3 signal 0
	//register_in = 0; 
	sc_start(4, SC_NS);
	assert(register_out.read() == 2);

	//from state 3 to state 2 signal 1
	register_in = 1; 
	sc_start(4, SC_NS);
	assert(register_out.read() == 1);

	reset = 1; // Assert the reset
    cout << "@" << sc_time_stamp() << " Asserting reset\n" << endl;
	sc_start(5, SC_NS);
	assert(register_out.read() == 1);

    cout << "@" << sc_time_stamp() << " Terminating simulation\n" << endl;
    sc_close_vcd_trace_file(wf);
    return 0; // Terminate simulation
}
