#include "systemc.h"
#include "directive.h"

struct dma : sc_module { 
	sc_in<unsigned >  	addr;  			//	address
  	sc_in<signed> 		data_in_dma; 		//	input data
	sc_in<bool>			i_d_cache;		//	icache or dcache
	sc_in<bool>			valid_data;		//	input data valid
	sc_in<bool>			read_write;		//	read or write
	sc_out<signed>		data_out;		//	data from mem

	sc_out<unsigned>	imem_data;
	sc_out<bool>		imem_write_read;
	sc_out<unsigned>	imem_addr;
	sc_out<bool>		icache_access;
	sc_out<bool>		dcache_access;
	sc_out<bool>		dma_access;
	sc_out<bool>		stall_fetch;
	sc_in<unsigned>		idata_in_cpu;
	sc_in<bool>			imem_valid;
	sc_in<bool>			busy;
	sc_out<signed>		dmem_data;
	sc_out<bool>		dmem_write_read;
	sc_out<unsigned>	dmem_addr;
	sc_in<signed>		ddata_in_cpu;
	sc_in<bool>			dmem_valid;

    sc_in_clk 			CLK;


  // Parameter
	unsigned addr_var;
	unsigned data_in_var;
	bool where_var;
	bool read_write_var;
	unsigned data_out_var;
	int wait_cycles; 					// cycles # it takes to access dmemory

	void init_param(int given_delay_cycles) {
		wait_cycles = given_delay_cycles; 
	}
  //Constructor 
  SC_CTOR(dma) {
        SC_CTHREAD(entry, CLK.pos());
		addr_var=0;
		data_in_var=0;
		where_var=false;
		read_write_var=false;
		data_out_var=0;
  }
  // Process functionality in member function below
  void entry();
};

void dma::entry()
{
  while (true) {
    	do { 
			wait();
		} while ( !(valid_data.read() == true) ); 
		addr_var=addr.read();
		data_in_var=data_in_dma.read();
		where_var=i_d_cache.read();
		read_write_var=read_write.read();
		
		if(where_var==true)// icache access
		{			
			stall_fetch=true;
			do{ wait(); } while ( !(busy.read() == false) ); 
			icache_access=true;
			imem_write_read=read_write_var;
			imem_addr=addr_var;
			if (read_write_var==true)
			{
				imem_data=data_in_var;
				if (PRINT_DMA) {
					cout << "-------------------------------" << endl;
					printf("DMA: data=0x%x(%d) to icache[%x]",data_in_var, data_in_var, addr_var);
					cout.setf(ios::dec,ios::basefield);
   					cout << " at CSIM " << sc_time_stamp() << endl;
					cout << "-------------------------------" << endl;
				}
				wait(wait_cycles);
				icache_access=false;
				stall_fetch=false;
				imem_write_read=0;
				imem_addr=0;
				imem_data=0;
				wait();
			}
			else
			{	
				do{ wait(); } while ( !(imem_valid.read() == true) ); 
				data_out = idata_in_cpu.read();
				if (PRINT_DMA) {
					cout << "-------------------------------" << endl;
					printf("DMA: data=0x%x(%d) fr icache", idata_in_cpu.read(), idata_in_cpu.read());
					cout.setf(ios::dec,ios::basefield);
   					cout << " at CSIM " << sc_time_stamp() << endl;
					cout << "-------------------------------" << endl;
				icache_access=false;
				stall_fetch=false;
				imem_write_read=0;
				imem_addr=0;
				}
			}
		}// dcache access
		else
		{
			dcache_access=true;
			dmem_write_read=read_write_var;
			dmem_addr=addr_var;
			if (read_write_var==true)
			{
				dmem_data=data_in_var;
				if (PRINT_DMA) {
					cout << "-------------------------------" << endl;
					printf("DMA: data=0x%x(%d) to dcache[%x]",data_in_var, data_in_var, addr_var);
					cout.setf(ios::dec,ios::basefield);
   					cout << " at CSIM " << sc_time_stamp() << endl;
					cout << "-------------------------------" << endl;
				}
				wait(wait_cycles);
				dcache_access=false;
				dmem_write_read=0;
				dmem_addr=0;
				dmem_data=0;
				wait();
			}
			else
			{	
				dma_access=true;
				do{ wait(); } while ( !(dmem_valid.read() == true) ); 
				data_out = ddata_in_cpu.read();
				if (PRINT_DMA) {
					cout << "-------------------------------" << endl;
					printf("DMA: data=0x%x(%d) fr dcache", ddata_in_cpu.read(), ddata_in_cpu.read());
					cout.setf(ios::dec,ios::basefield);
   					cout << " at CSIM " << sc_time_stamp() << endl;
					cout << "-------------------------------" << endl;
				dcache_access=false;
				dma_access=false;
				dmem_write_read=0;
				dmem_addr=0;
				}
			}
		}
		do { 
			wait();
		} while ( !(valid_data.read() == false) );
  }
} // end of entry function