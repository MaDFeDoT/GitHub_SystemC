#include "systemc.h"

#ifndef DESIGN_H
#define	DESIGN_H

SC_MODULE(sd_register) {
    sc_in_clk clock; // Clock input of the design
    sc_in<bool> reset; // active high, synchronous Reset input
    sc_in<bool> sync_reset;
    sc_in<bool> left_right;
	sc_in<bool> d;
    sc_out<sc_uint<8> > register_out; // 8 bit vector output

    sc_uint<8> myregister;

    void reg_sd();

    SC_CTOR(sd_register) :
            clock("clock"),
            reset("reset"),
            register_out("register_out") {
        cout << "Executing new" << endl;
        SC_CTHREAD(reg_sd, clock.pos());
        reset_signal_is(sync_reset, true);
        async_reset_signal_is(reset, true);
    } // End of Constructor

}; // End of Module

#endif

