#include "systemc.h"

#ifndef DESIGN_H
#define	DESIGN_H

template <class T>
class FIFOParam : public sc_module {
public:
//input
    sc_in_clk clk;				// Clock input of the design
    sc_in<bool> sreset_n;		// active high, synchronous Reset input
    sc_in<T> data_in;			// vector input with format T
    sc_in<bool> push;
	sc_in<bool> pop;

//output
	sc_out<T> data_out;			// vector output with format T
	sc_out<bool> empty;
	sc_out<bool> full;

    //------------Local Variables Here---------------------
	T *data;
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
	SC_HAS_PROCESS(FIFOParam);
	FIFOParam(sc_module_name name, int max) :
            clk("clk"),
            sreset_n("sreset_n"),
            data_in("data_in"),
            data_out("data_out") {
		data=new T [max];
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


template <class T>
void FIFOParam<T>::write_FIFO() {
	push_pointer=0;
	push_event=false;
	empty=1;
	full=0;
	wait();
	while (true) {
		if ((push==1) && (flag_can_push==true)) {
				data[push_pointer.read()]=data_in.read();
				cout<<sc_time_stamp()<<" Write data = "<<data_in.read()<<"; push_pointer change from "<<push_pointer;
				push_pointer=push_pointer.read()+1;
				int push_pointer_temp=push_pointer.read()+1;
				push_event=true;
				if(push_pointer_temp==size){ push_pointer=0; push_pointer_temp=0;}
				cout<< " to "<< push_pointer_temp <<endl;
		}
		else
		{
			push_event=false;
		}
		wait();
	}
}

template <class T>
void FIFOParam<T>::read_FIFO() {
	pop_pointer=0;
	pop_event==false;
	data_out=0;
	wait();
	while (true) {
		if ((pop==1) && (flag_can_pop==true)){
				data_out=data[pop_pointer.read()];
				cout<<sc_time_stamp()<<" Read data = "<<data[pop_pointer.read()]<<"; pop_pointer change from "<<pop_pointer;
				pop_pointer=pop_pointer.read()+1;
				pop_event=true;
				int pop_pointer_temp=pop_pointer.read()+1;
				if (pop_pointer_temp==size) {pop_pointer=0; pop_pointer_temp=0;}
				cout<< " to "<< pop_pointer_temp<<endl;
		}
		else
		{
			pop_event=false;
		}
		wait();
	}
}

template <class T>
void FIFOParam<T>::elem_counter() {
	count_of_elements=0;
	wait();
	while (true) {
		if((push_event==true) && (pop_event==false))
		{
			count_of_elements=count_of_elements+1;
		}
		else
		if((push_event==false) && (pop_event==true))
		{
			count_of_elements=count_of_elements-1;
				
		}
		wait();
	}
}

template <class T>
void FIFOParam<T>::compare_pointer(){
//	Формируем флаг can_push
	if((push_pointer.read()==pop_pointer.read()) && ((count_of_elements==size) || ((count_of_elements==size-1) && (push_event==true))) && (pop_event!=true))
	{
		flag_can_push=false;
	}
	else{
		flag_can_push=true;
	}
//	формируем флаг can_pop	
	if((push_pointer.read()==pop_pointer.read()) && ((count_of_elements==0) || ((count_of_elements==1) && (pop_event==true))) && (push_event!=true))
	{
		flag_can_pop=false;
	}
	else{
		flag_can_pop=true;
	}
//	Формируем флаг empty
	if(((count_of_elements==0) || ((count_of_elements==1) && (pop_event==true))) && (push_event!=true))
	{
		empty=1;
	}
	else
	{
		empty=0;
	}
//	Формируем флаг full	
	if(((count_of_elements==size) || ((count_of_elements==size-1) && (push_event==true))) && (pop_event!=true))
	{
		full=1;
	}
	else
	{
		full=0;
	}
}

#endif	/* DESIGN_H */