#include "systemc.h"

#ifndef DESIGN_H
#define	DESIGN_H

template <class T>
class FIFOParam : public sc_module {
public:
    sc_in_clk clk; // Clock input of the design
    sc_in<bool> sreset_n; // active high, synchronous Reset input
    sc_in<sc_uint<8>> data_in; // 8 bit vector input
    sc_in<bool> push;
	sc_in<bool> pop;
	sc_out<sc_uint<8>> data_out; // 8 bit vector output
	sc_out<bool> empty;
	sc_out<bool> full;
    //------------Local Variables Here---------------------
			//sc_signal<sc_uint<2> > state;

	T *data;
	int size;
	sc_signal<sc_uint<32>> pop_pointer;
	sc_signal<sc_uint<32>> push_pointer;
	bool flag_push_on_pop;

	void write_FIFO();
	void read_FIFO();
    void reset();
    // Constructor
	SC_HAS_PROCESS(FIFOParam);
	FIFOParam(sc_module_name name, int max) :
            clk("clk"),
            sreset_n("sreset_n"),
            data_in("data_in"),
            data_out("data_out") {
		data=new T [max];
		size=max;
		flag_push_on_pop=false;
		push_pointer=0;
		pop_pointer=0;
	//reset
        SC_CTHREAD(reset, clk.pos());
        reset_signal_is(sreset_n, false);
	//write_FIFO
		SC_CTHREAD(write_FIFO, clk.pos());
		reset_signal_is(sreset_n, false);
	//read_FIFO
		SC_CTHREAD(read_FIFO, clk.pos());
		reset_signal_is(sreset_n, false);
    } // End of Constructor

}; // End of Module


template <class T>
void FIFOParam<T>::write_FIFO() {
	wait();
	while (true) {
		if (push==1) {
			if(flag_push_on_pop==false)
			{
				data[push_pointer.read()]=data_in.read();
				cout<<sc_time_stamp()<<" Write data = "<<(T) data_in.read()<<"; push_pointer change from "<<push_pointer;
				push_pointer=push_pointer.read()+1;
				int push_pointer_temp=push_pointer.read()+1;
				empty=0;
				if(push_pointer_temp==size){ push_pointer=0; push_pointer_temp=0;}
				cout<< " to "<< push_pointer_temp <<endl;
				if(push_pointer_temp==pop_pointer.read())
				{
					flag_push_on_pop=true;
					full=1;
				}
			}
		}
		wait();
	}
}

template <class T>
void FIFOParam<T>::read_FIFO() {
	wait();
	while (true) {
		if (pop==1){
			if (flag_push_on_pop==false)
			{
				if(pop_pointer.read()!=push_pointer.read())
				{
					data_out=data[pop_pointer.read()];
					cout<<sc_time_stamp()<<" Read data = "<<(T) data[pop_pointer.read()]<<"; pop_pointer change from "<<pop_pointer;
					pop_pointer=pop_pointer.read()+1;
					int pop_pointer_temp=pop_pointer.read()+1;
					if (pop_pointer_temp==size) {pop_pointer=0; pop_pointer_temp=0;}
					cout<< " to "<< pop_pointer_temp<<endl;
					if (pop_pointer_temp==push_pointer.read()) empty=1;
				}
			}
			//flag_push_on_pop=true
			else 
			{
				data_out=data[pop_pointer.read()];
				pop_pointer=pop_pointer.read()+1;
				int pop_pointer_temp=pop_pointer.read()+1;
				if (pop_pointer_temp==size) pop_pointer=0;
				flag_push_on_pop=false;
				full=0;
			}
		}
		wait();
	}
}

template <class T>
void FIFOParam<T>::reset(){
	flag_push_on_pop=false;
	push_pointer=0;
	pop_pointer=0;
	data_out=0;
	empty=1;
	full=0;
	wait();
	while (true){
		wait();
	}
}

#endif	/* DESIGN_H */