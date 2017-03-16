#include "systemc.h"

#ifndef DESIGN_H
#define	DESIGN_H

SC_MODULE(eightbit_register) {
    sc_in_clk clock; // Clock input of the design
    sc_in<bool> reset; // active high, asynchronous Reset input
    sc_in<sc_uint<8> > register_in; // 8 bit vector input
    sc_out<sc_uint<8> > register_out; // 8 bit vector output
    sc_out<bool> parity; // 8 bit vector output

    //------------Local Variables Here---------------------
    sc_signal<sc_uint<8> > myregister;

    //------------Code Starts Here-------------------------
    // Below function implements actual register logic

    void register_store();
    
    void calculate_parity();
    
    void data_out_method();

    // Constructor for the register
    // Since this register is a positive edge trigged one,
    // We trigger the below block with respect to positive
    // edge of the clock and also when ever reset changes state

    SC_CTOR(eightbit_register) :
            clock("clock"),
            reset("reset"),
            register_in("register_in"),
            register_out("register_out"),
            parity("parity") {
        cout << "Executing new" << endl;
        SC_CTHREAD(register_store, clock.pos());
        async_reset_signal_is(reset, true);
        SC_METHOD(calculate_parity);
        sensitive << myregister;
        SC_METHOD(data_out_method);
        sensitive << myregister;
    } // End of Constructor

}; // End of Module

#endif	/* DESIGN_H */

