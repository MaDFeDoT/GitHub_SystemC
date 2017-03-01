#include "systemc.h"

#ifndef DESIGN_H
#define	DESIGN_H

SC_MODULE(cnt) {
    sc_in_clk clock; // Clock input of the design
    sc_in<bool> reset; // active high, synchronous Reset input
    sc_in<bool> sync_reset;
    sc_in<bool> ena;
    sc_out<sc_uint<8> > cnt_register_out; // 8 bit vector output

    sc_uint<8> myregister;

    void cnt_count();

    SC_CTOR(cnt) :
            clock("clock"),
            reset("reset"),
            cnt_register_out("register_out") {
        cout << "Executing new" << endl;
        SC_CTHREAD(cnt_count, clock.pos());
        reset_signal_is(sync_reset, true);
        async_reset_signal_is(reset, true);
    } // End of Constructor

}; // End of Module

#endif

