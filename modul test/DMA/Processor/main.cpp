#include "directive.h"
#include "systemc.h"
#include "icache.h"
#include "fetch.h"
#include "decode.h"
#include "exec.h"
#include "floating.h"
#include "dcache.h"
#include "pic.h"
#include "GPIO.h"
#include "dma.h"
#include <climits>
#include <cstdlib>
#include <time.h> 
#include <limits.h>

int sc_main(int ac, char *av[])
{
  sc_core::sc_report_handler::set_actions( "/IEEE_Std_1666/deprecated",
                                           sc_core::SC_DO_NOTHING );
  sc_trace_file *wf = sc_create_vcd_trace_file("Processor_waveform");
  // ************************ ICACHE ***********************************
  sc_signal<bool>			ram_cs("RAM_CS") ;
  sc_signal<bool>			ram_we("RAM_WE") ;
  sc_signal<unsigned >		addr("Address") ;
  sc_signal<unsigned >		ram_datain("RAM_DATAIN") ;
  sc_signal<unsigned >		ram_dataout("RAM_DATAOUT") ;
  sc_signal<bool>			icache_valid("ICACHE_VALID") ;
  const int delay_cycles = 2;
		// Dump the desired signals
		sc_trace(wf, ram_cs, "ICACHE_ram_cs");
		sc_trace(wf, ram_we, "ICACHE_ram_we");
		sc_trace(wf, addr, "ICACHE_addr");
		sc_trace(wf, ram_datain, "ICACHE_ram_datain");
		sc_trace(wf, ram_dataout, "ICACHE_ram_dataout");
		sc_trace(wf, icache_valid, "ICACHE_icache_valid");

  // ************************ Fetch ***********************************
  // IFU ramdata = ram_dataout
  sc_signal<unsigned >		branch_target_address("BRANCH_TARGET_ADDRESS") ;
  sc_signal<bool>			next_pc("NEXT_PC") ;
  sc_signal<bool>			branch_valid("BRANCH_VALID") ;
  sc_signal<bool>			stall_fetch("STALL_FETCH") ;
  // IFU ram_cs = ram_cs
  // IFU ram_we = ram_we
  // IFU address = addr
  sc_signal<unsigned>		instruction("INSTRUCTION") ;
  sc_signal<bool>			instruction_valid("INSTRUCTION_VALID") ;
  sc_signal<bool>			busy("busy") ;

		sc_trace(wf, branch_target_address, "FETCH_branch_target_address");
		sc_trace(wf, next_pc, "FETCH_next_pc");
		sc_trace(wf, branch_valid, "FETCH_branch_valid");
		sc_trace(wf, stall_fetch, "FETCH_stall_fetch");
		sc_trace(wf, instruction, "FETCH_instruction");
		sc_trace(wf, instruction_valid, "FETCH_instruction_valid");
		sc_trace(wf, busy, "FETCH_busy");

  // ************************ Decode ***********************************
  // ID instruction = instruction
  // ID instruction = instruction_valid
  // ID destreg_write = out_valid
  // ID destreg_write_src = destout
  // ID alu_dataout = dout from EXEC 
  sc_signal<signed>     dram_dataout("DRAM_DATAOUT") ;
  sc_signal<bool>       dram_rd_valid("DRAM_RD_VALID") ;
  // ID next_pc     = next_pc
  // ID branch_valid = branch_valid
  // ID branch_target_address = branch_target_address
  sc_signal<bool>			mem_access("MEM_ACCESS") ;
  sc_signal<unsigned >		mem_address("MEM_ADDRESS") ;
  sc_signal<int>			alu_op("ALU_OP") ;
  sc_signal<bool>			mem_write("MEM_WRITE") ;
  sc_signal<unsigned>		alu_src("ALU_SRC") ;
  sc_signal<bool>			reg_write("REG_WRITE") ;
  sc_signal<signed int>		src_A("SRC_A") ;
  sc_signal<signed int>		src_B("SRC_B") ;
  // ID stall_fetch = stall_fetch
  sc_signal<bool>			decode_valid("DECODE_VALID") ;
  sc_signal<bool>			float_valid("FLOAT_VALID") ;
  sc_signal<bool>			gpio_access("GPIO_ACCESS");
  sc_signal<signed int>      dramdata("dramdata");
		sc_trace(wf, dram_dataout, "DECODE_dram_dataout");
		sc_trace(wf, dram_rd_valid, "DECODE_dram_rd_valid");
		sc_trace(wf, mem_access, "DECODE_mem_access");
		sc_trace(wf, mem_address, "DECODE_mem_address");
		sc_trace(wf, alu_op, "DECODE_alu_op");
		sc_trace(wf, mem_write, "DECODE_mem_write");
		sc_trace(wf, alu_src, "DECODE_alu_src");
		sc_trace(wf, reg_write, "DECODE_reg_write");
		sc_trace(wf, src_A, "DECODE_src_A");
		sc_trace(wf, src_B, "DECODE_src_B");
		sc_trace(wf, decode_valid, "DECODE_decode_valid");
		sc_trace(wf, float_valid, "DECODE_float_valid");
		sc_trace(wf, gpio_access, "DECODE_gpio_access");
		sc_trace(wf, dramdata, "DECODE_dramdata");

  // ************************ DCACHE  ***********************************
  //DECODE sc_signal<signed int>	src_A("SRC_A") ;
  //DECODE sc_signal<bool>			mem_access("MEM_ACCESS") ;
  //DECODE sc_signal<bool>			mem_write("MEM_WRITE") ;
  //DECODE sc_signal<unsigned >		mem_address("MEM_ADDRESS") ;


  // ************************ Execute ***********************************
  // EXEC in_valid = decode_valid
  sc_signal<bool>   	in_valid("IN_VALID") ;
  // EXEC opcode = alu_op
  // EXEC dina = src_A
  // EXEC dinb = src_B
  // EXEC dest = alu_src
  sc_signal<bool>   		c("C") ;
  sc_signal<bool>   		v("V") ;
  sc_signal<bool>   		z("Z") ;
  sc_signal<signed>   		dout("DOUT") ;
  sc_signal<bool>   		out_valid("OUTPUT_VALID") ;
  sc_signal<unsigned> 		destout("DESTOUT") ;

		sc_trace(wf, in_valid, "EXEC_in_valid");
		sc_trace(wf, c, "EXEC_c");
		sc_trace(wf, v, "EXEC_v");
		sc_trace(wf, z, "EXEC_z");
		sc_trace(wf, dout, "EXEC_dout");
		sc_trace(wf, out_valid, "EXEC_out_valid");
		sc_trace(wf, destout, "EXEC_destout");

  // ************************ Floating point ******************************
  // FPU in_valid = float_valid
  // FPU opcode = alu_op
  // FPU floata = src_A
  // FPU floatb = src_B
  // FPU dest = alu_src
  sc_signal<signed> 		fdout("FDOUT") ;
  sc_signal<bool> 			fout_valid("FOUT_VALID") ;
  sc_signal<unsigned> 		fdestout("FDESTOUT") ;

		sc_trace(wf, fdout, "FLOATING_fdout");
		sc_trace(wf, fout_valid, "FLOATING_fout_valid");
		sc_trace(wf, fdestout, "FLOATING_fdestout");
  // ************************ PIC *****************************************
  sc_signal<bool> 		ireq0("IREQ0") ;
  sc_signal<bool> 		ireq1("IREQ1") ;
  sc_signal<bool> 		ireq2("IREQ2") ;
  sc_signal<bool> 		ireq3("IREQ3") ;
  sc_signal<bool> 		intreq("INTREQ") ;
  sc_signal<unsigned> 		vectno("VECTNO") ;
  sc_signal<bool> 		intack_cpu("INTACK_CPU") ;
		sc_trace(wf, ireq0, "PIC_ireq0");
		sc_trace(wf, ireq1, "PIC_ireq1");
		sc_trace(wf, ireq2, "PIC_ireq2");
		sc_trace(wf, ireq3, "PIC_ireq3");
		sc_trace(wf, intreq, "PIC_intreq");
		sc_trace(wf, vectno, "PIC_vectno");
		sc_trace(wf, intack_cpu, "PIC_intack_cpu");
  // ************************ GPIO ***************************************** 
  sc_signal<signed>       gpio_data_in("GPIO_DATA_IN");	    //input data from gpio
  sc_signal<signed>		  gpio_dataout("GPIO_DATA_OUT");	//output data from gpio
		sc_trace(wf, gpio_data_in, "GPIO_gpio_data_in");
		sc_trace(wf, gpio_dataout, "GPIO_gpio_dataout");
  
  // ************************ DMA ***************************************** 
  	sc_signal<unsigned >  	dma_addr("addr");  						//	address
  	sc_signal<signed> 		dma_data_in_dma("data_in_dma"); 		//	input data
	sc_signal<bool>			dma_i_d_cache("i_d_cache");				//	icache or dcache
	sc_signal<bool>			dma_valid_data("valid_data");			//	input data valid
	sc_signal<bool>			dma_read_write("read_write");			//	read or write
	sc_signal<signed>		dma_data_out("data_out");				//	data from mem
	sc_signal<bool>		    dma_access("dma_access");
		sc_trace(wf, dma_addr, "DMA_addr");
		sc_trace(wf, dma_data_in_dma, "DMA_data_in_dma");
		sc_trace(wf, dma_i_d_cache, "DMA_i_d_cache");
		sc_trace(wf, dma_valid_data, "DMA_valid_data");
		sc_trace(wf, dma_read_write, "DMA_read_write");
		sc_trace(wf, dma_data_out, "DMA_data_out");
		sc_trace(wf, dma_access, "DMA_dma_access");

  ////////////////////////////////////////////////////////////////////////////
  // 				MAIN PROGRAM 
  ////////////////////////////////////////////////////////////////////////////
  sc_clock clk("Clock", 1, SC_NS, 0.5, 0.0, SC_NS);
  sc_trace(wf, clk, "CLK");
  fetch 	IFU("FETCH_BLOCK");
        IFU.init_param(delay_cycles);
		IFU << ram_dataout << branch_target_address << next_pc << branch_valid
		<< stall_fetch << intreq << vectno << icache_valid << ram_cs << ram_we 
		<< addr << instruction << instruction_valid << intack_cpu << busy << clk;

  decode 	IDU("DECODE_BLOCK");
		IDU << instruction << instruction_valid << out_valid << destout << dout 
		<< dram_dataout << dram_rd_valid << fdout << fout_valid << fdestout
	    << next_pc << branch_valid << branch_target_address << mem_access
		<< mem_address << alu_op << mem_write << alu_src << src_A << src_B 
		<< decode_valid << float_valid << gpio_access << dramdata << dma_access << ireq0 <<clk;

  exec 		IEU("EXEC_BLOCK");
		IEU << decode_valid << alu_op << src_A << src_B << alu_src << c << v << z
		<< dout << out_valid << destout << clk;

  floating 	FPU("FLOAT_BLOCK");		
		FPU << float_valid << alu_op << src_A << src_B << alu_src << fdout 
		<< fout_valid << fdestout << clk;

  icache 	ICACHE("ICACHE_BLOCK"); 
		ICACHE.init_param(delay_cycles);
		ICACHE << ram_datain << ram_cs << ram_we << addr << ram_dataout 
		<< icache_valid << clk;

  dcache 	DCACHE("DCACHE_BLOCK");
		DCACHE.init_param(delay_cycles);
		DCACHE << dramdata << mem_access << mem_write << mem_address << dram_dataout 
		<< dram_rd_valid << clk;

  gpio GPIO("GPIO_BLOCK");
	   GPIO.init_param(delay_cycles);
	   GPIO << src_A << gpio_data_in << gpio_access << mem_write << dram_dataout << gpio_dataout
		<< dram_rd_valid << clk;

  dma DMA("DMA_BLOCK");
		DMA.init_param(delay_cycles);
		DMA << dma_addr	<< dma_data_in_dma << dma_i_d_cache << dma_valid_data << dma_read_write << dma_data_out 
			<< ram_datain << ram_we << addr << ram_cs << mem_access << dma_access << stall_fetch << ram_dataout << icache_valid << busy 
			<< dramdata << mem_write << mem_address << dram_dataout << dram_rd_valid << clk;

  pic		APIC("PIC_BLOCK");
		APIC << ireq0 << ireq1 << ireq2 << ireq3 <<intack_cpu
		<< intreq << vectno;
  time_t tbuffer = time(NULL);
  gpio_data_in = 0xff;




  sc_start(6, SC_NS);
	dma_addr=3;
	dma_i_d_cache=1;
	dma_read_write=1;
	dma_valid_data=1;
	dma_data_in_dma=0xffff;

	cout << "@" << sc_time_stamp() << " Write icache mem[0x3]\n" << endl;

    sc_start(8, SC_NS);
//14ns
	assert(addr.read() == 3);
	assert(ram_cs.read() == 1);
	assert(stall_fetch.read() == 1);
	assert(ram_datain.read() == 0xffff);
	
	dma_addr=3;
	dma_i_d_cache=1;
	dma_read_write=0;
	dma_valid_data=0;
	dma_data_in_dma=0xffff;

	sc_start(4, SC_NS);
//18ns
	dma_addr=3;
	dma_i_d_cache=1;
	dma_read_write=0;
	dma_valid_data=1;
	sc_start(4, SC_NS);
	dma_valid_data=0;
	sc_start(6, SC_NS);
//28ns
	assert(dma_data_out.read() == 0xffff);
//18ns
	dma_addr=3;
	dma_i_d_cache=0;
	dma_read_write=1;
	dma_valid_data=1;
	dma_data_in_dma=0xeeee;
	sc_start(4, SC_NS);
	dma_valid_data=0;
	sc_start(8, SC_NS);

	dma_addr=3;
	dma_i_d_cache=0;
	dma_read_write=0;
	dma_valid_data=1;

	sc_start(4, SC_NS);
	

  cout << "Time for simulation = " << (time(NULL) - tbuffer) << endl;
  sc_close_vcd_trace_file(wf);

  
  return 0;	
}
