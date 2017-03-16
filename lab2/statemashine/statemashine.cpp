#include "statemashine.h"

void statemashine::register_store() {  
	state=0;
    wait();
    while (true) {
		cout << "******"<<endl;
		cout << "Previous state == " << state<<"  Input signal == "<<register_in.read()<<endl;
		if(state.read()==0)
		{
			if(register_in.read()==0) {state=3; cout << "  Now state == 3"<<endl;}
			if((register_in.read()==1)||(register_in.read()==2)) {state=1; cout << "  Now state == 1"<<endl;}
			if(register_in.read()==3) cout << "  Now state == 0"<<endl;
		}
		if(state.read()==1)
		{
			if(register_in.read()==0) {state=0; cout << "  Now state == 0"<<endl;}
			if(register_in.read()==1) {state=2; cout << "  Now state == 2"<<endl;}
			if(register_in.read()==2) {state=1; cout << "  Now state == 1"<<endl;}
			if(register_in.read()==3) cout << "  Now state == 1"<<endl;
		}
		if(state.read()==2)
		{
			if(register_in.read()==2) {state=0; cout << "  Now state == 0"<<endl;}
			if(register_in.read()!=2) cout << "  Now state == 2"<<endl;
		}
		if(state.read()==3)
		{
			if(register_in.read()==0) {state=3; cout << "  Now state == 3"<<endl;}
			if(register_in.read()==1) {state=2; cout << "  Now state == 2"<<endl;}
			if((register_in.read()!=1)&(register_in.read()!=0)) cout << "  Now state == 3"<<endl;
		}
        wait();
    }
}


void statemashine::data_out_method() {
	if((state.read()==0)||(state.read()==2)) register_out = 1;
	else register_out = 2;
}
