#include "systemc.h"

#ifndef DESIGN_H
#define	DESIGN_H

class FIFO : public sc_module {
public:
//input
    sc_in_clk clk;				// Clock input of the design
    sc_in<bool> sreset_n;		// active high, synchronous Reset input
    sc_in<sc_uint<8>> data_in;	// 8 bit vector input
    sc_in<bool> push;
	sc_in<bool> pop;

//output
	sc_out<sc_uint<8>> data_out; // 8 bit vector output
	sc_out<bool> empty;
	sc_out<bool> full;

    //------------Local Variables Here---------------------
	int *data;
	int size;
	sc_signal<sc_uint<32>> pop_pointer;
	sc_signal<sc_uint<32>> push_pointer;
	sc_signal<bool> push_event;
	sc_signal<bool> pop_event;
	sc_signal<bool> flag_can_push;
	sc_signal<bool> flag_can_pop;
	sc_signal<int> count_of_elements;

	void write_FIFO();
	void read_FIFO();
	void elem_counter();
	void compare_pointer();

    // Constructor
	SC_HAS_PROCESS(FIFO);
	FIFO(sc_module_name name, int max) :
            clk("clk"),
            sreset_n("sreset_n"),
            data_in("data_in"),
            data_out("data_out") {
		data=new int [max];
		size=max;
		//flag_push_on_pop=false;
		push_pointer=0;
		pop_pointer=0;
	//compare_pointer
		SC_METHOD(compare_pointer);
		sensitive << push_pointer << pop_pointer;
	//elements count
		SC_CTHREAD(elem_counter, clk.pos());
		reset_signal_is(sreset_n, false);
	//write_FIFO
		SC_CTHREAD(write_FIFO, clk.pos());
		reset_signal_is(sreset_n, false);
	//read_FIFO
		SC_CTHREAD(read_FIFO, clk.pos());
		reset_signal_is(sreset_n, false);
    } // End of Constructor

}; // End of Module

#endif	/* DESIGN_H */