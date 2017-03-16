//-----------------------------------------------------
// Testbench for the register ---------------->
//-----------------------------------------------------
#include "systemc.h"
#include "register.h"

#define soft_assert(signal, expected) \
      if (signal.read() != expected) { \
          cerr << "@" << sc_time_stamp() << " Check failed. Expected: " << expected << ". Actual: " << signal.read() << ".\n" << endl; \
      }

int sc_main(int argc, char* argv[]) {
    sc_clock clock("clock", 4, SC_NS);
    sc_signal<bool> reset;
    sc_signal<sc_uint<8> > register_in;
    sc_signal<sc_uint<8> > register_out;
    sc_signal<bool > parity;
    int i = 0;
    // Connect the DUT
    eightbit_register test_register("test_register");
    test_register.clock(clock);
    test_register.reset(reset);
    test_register.register_in(register_in);
    test_register.register_out(register_out);
    test_register.parity(parity);

    // Open VCD file
    sc_trace_file *wf = sc_create_vcd_trace_file("register_waveform");
    // Dump the desired signals
    sc_trace(wf, clock, "clock");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, register_in, "din");
    sc_trace(wf, register_out, "dout");
    sc_trace(wf, parity, "parity");

    reset = 1; // Assert the reset
    cout << "@" << sc_time_stamp() << " Asserting reset\n" << endl;

    sc_start(6, SC_NS);

    reset = 0; // De-assert the reset
    cout << "@" << sc_time_stamp() << " De-Asserting reset\n" << endl;

    sc_uint<8> data = 0;
    for (i = 0; i < 5; i++) {
        data += i * i;
        register_in = data;
        sc_start(4, SC_NS);
        assert(register_out.read() == data);
        assert(parity.read() == data.xor_reduce());
//        soft_assert(register_out, data);
//        soft_assert(parity, data.xor_reduce());
    }

    reset = 1; // Assert the reset
    cout << "@" << sc_time_stamp() << " Asserting reset\n" << endl;
    sc_start(1, SC_NS);
    assert(register_out.read() == 0);
    sc_start(5, SC_NS);

    cout << "@" << sc_time_stamp() << " Terminating simulation\n" << endl;
    sc_close_vcd_trace_file(wf);
    return 0; // Terminate simulation

}
