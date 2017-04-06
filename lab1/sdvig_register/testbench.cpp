//-----------------------------------------------------
// Testbench for the sdvig_rgister ---------------->
//-----------------------------------------------------
#include "systemc.h"
#include "sdvig_register.h"

#define soft_assert(signal, expected) \
      if (signal.read() != expected) { \
          cerr << "@" << sc_time_stamp() << " Check failed. Expected: " << expected << ". Actual: " << signal.read() << ".\n" << endl; \
      }

int sc_main(int argc, char* argv[]) {
    sc_clock clock("clock", 4, SC_NS);
    sc_signal<bool> reset;
    sc_signal<bool> sync_reset;
    sc_signal<bool> left_right;
	sc_signal<bool> d;
    sc_signal<sc_uint<8> > register_out;
    int i = 0;
    // Connect the DUT
    sd_register test_cnt("test_cnt");
    test_cnt.clock(clock);
    test_cnt.reset(reset);
    test_cnt.sync_reset(sync_reset);
    test_cnt.left_right(left_right);
	test_cnt.d(d);
    test_cnt.register_out(register_out);

    // Open VCD file
    sc_trace_file *wf = sc_create_vcd_trace_file("reg_waveform");
    // Dump the desired signals
    sc_trace(wf, clock, "clock");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, sync_reset, "sync_reset");
    sc_trace(wf, left_right, "left_right");
	sc_trace(wf, d, "d");
    sc_trace(wf, register_out, "register_out");

    reset = 0;
    sync_reset = 0;
    left_right = 0;
	d=1;
    sc_start(5, SC_NS);
    assert(register_out.read() == 1);
    sc_start(2, SC_NS);
//7ns
    reset = 1;
    sc_start(10, SC_NS);
//17ns
	assert(register_out.read() == 0);
    reset = 0;
    sc_start(6, SC_NS);
//23ns
    assert(register_out.read() == 1);

    sync_reset = 1;
	sc_start(1, SC_NS);
//24ns f_clk
	assert(register_out.read() == 1);

    sc_start(6, SC_NS);
//30ns
	assert(register_out.read() == 0);
    sync_reset = 0;

    sc_start(7, SC_NS);
//37ns
	assert(register_out.read() == 3);
	d=0;
    sc_start(13, SC_NS);
//50ns
	assert(register_out.read() == 24);

	left_right = 1;
	sc_start(23, SC_NS);
//63ns
	assert(register_out.read() == 0);
	d=1;
    sc_start(25, SC_NS);
//88s
	assert(register_out.read() == 252);

	reset = 1; // Assert the reset
    cout << "@" << sc_time_stamp() << " Asserting reset\n" << endl;
    sc_start(2, SC_NS);
	assert(register_out.read() == 0);
    sc_start(20, SC_NS);
	assert(register_out.read() == 0);
    cout << "@" << sc_time_stamp() << " Terminating simulation\n" << endl;
    sc_close_vcd_trace_file(wf);
    return 0; // Terminate simulation

}
