//-----------------------------------------------------
// Testbench for the counter ---------------->
//-----------------------------------------------------
#include "systemc.h"
#include "counter.h"

#define soft_assert(signal, expected) \
      if (signal.read() != expected) { \
          cerr << "@" << sc_time_stamp() << " Check failed. Expected: " << expected << ". Actual: " << signal.read() << ".\n" << endl; \
      }

int sc_main(int argc, char* argv[]) {
    sc_clock clock("clock", 4, SC_NS);
    sc_signal<bool> reset;
    sc_signal<bool> sync_reset;
    sc_signal<bool> ena;
    sc_signal<sc_uint<8> > cnt_register_out;
    int i = 0;
    // Connect the DUT
    cnt test_cnt("test_cnt");
    test_cnt.clock(clock);
    test_cnt.reset(reset);
    test_cnt.sync_reset(sync_reset);
    test_cnt.ena(ena);
    test_cnt.cnt_register_out(cnt_register_out);

    // Open VCD file
    sc_trace_file *wf = sc_create_vcd_trace_file("cnt_waveform");
    // Dump the desired signals
    sc_trace(wf, clock, "clock");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, sync_reset, "sync_reset");
    sc_trace(wf, ena, "ena");
    sc_trace(wf, cnt_register_out, "cnt_register_out");

    reset = 0;
    sync_reset = 0;
    ena = 0;
    sc_start(4, SC_NS);
    assert(cnt_register_out.read() == 0);
    sc_start(3, SC_NS);
	//7ns
    assert(cnt_register_out.read() == 0);

    ena = 1;
    sc_start(3, SC_NS);
	//10ns
    assert(cnt_register_out.read() == 1);

    reset = 1;
    sc_start(9, SC_NS);
	//19ns
    assert(cnt_register_out.read() == 0);
    
    reset = 0;
    sc_start(2, SC_NS);
    //21ns

    sync_reset = 1;

	sc_start(3, SC_NS);
	//24ns
	assert(cnt_register_out.read() == 1);

    sc_start(7, SC_NS);
	//31ns
	assert(cnt_register_out.read() == 0);

    sync_reset = 0;
    sc_start(1, SC_NS);
	//32ns

    sc_start(27, SC_NS);
	//59ns
    assert(cnt_register_out.read() == 7);
    ena = 0;
    sc_start(26, SC_NS);
	//85 ns
    assert(cnt_register_out.read() == 7);

    reset = 1; // Assert the reset
    cout << "@" << sc_time_stamp() << " Asserting reset\n" << endl;
    sc_start(2, SC_NS);
    assert(cnt_register_out.read() == 0);
    sc_start(20, SC_NS);

    cout << "@" << sc_time_stamp() << " Terminating simulation\n" << endl;
    sc_close_vcd_trace_file(wf);
    return 0; // Terminate simulation

}
