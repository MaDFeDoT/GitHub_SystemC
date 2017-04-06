#include "FIFO.h"

void FIFO::write_FIFO() {
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

void FIFO::read_FIFO() {
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

void FIFO::elem_counter() {
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

void FIFO::compare_pointer(){
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