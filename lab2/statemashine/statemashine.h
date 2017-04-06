#include "systemc.h"

#ifndef DESIGN_H
#define	DESIGN_H

SC_MODULE(statemashine) {
    sc_in_clk clock; // Clock input of the design
    sc_in<bool> reset; // active high, asynchronous Reset input
    sc_in<sc_uint<2> > register_in; // 2 bit vector input
    sc_out<sc_uint<2> > register_out; // 2 bit vector output
  
    //------------Local Variables Here---------------------
	sc_signal<sc_uint<2> > state;
    //------------Code Starts Here-------------------------

    void register_store();
    
    void data_out_method();

    // Constructor for the statemachine
    // Since this register is a positive edge trigged one,
    // We trigger the below block with respect to positive
    // edge of the clock and also when ever reset changes state

    SC_CTOR(statemashine) :
            clock("clock"),
            reset("reset"),
            register_in("register_in"),
            register_out("register_out") {
        cout << "Executing new" << endl;
        SC_CTHREAD(register_store, clock.pos());
        async_reset_signal_is(reset, true);
        SC_METHOD(data_out_method);
		sensitive << state;
    } // End of Constructor

}; // End of Module

#endif	/* DESIGN_H */

