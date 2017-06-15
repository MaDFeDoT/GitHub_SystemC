#include <climits>
#include "systemc.h"
#include "directive.h"

struct decode : sc_module { 
  	sc_in<unsigned>  		instruction;		// fetched instruction
  	sc_in<bool>  			instruction_valid;  // input valid
  	sc_in<bool>  			destreg_write;      // register write enable
  	sc_in<unsigned>  		destreg_write_src;	// which register to write?      
  	sc_in<signed>  			alu_dataout; 		// data from ALU     
  	sc_in<signed>  			dram_dataout;       // data from Dcache
  	sc_in<bool>  			dram_rd_valid;      // Dcache read data valid
  	sc_in<signed>  			fpu_dout;      		// data from FPU(floating point unit)
  	sc_in<bool>  			fpu_valid;      	// FPU data valid
  	sc_in<unsigned>  		fpu_destout;      	// write to which register
  	sc_out<bool>  			next_pc;      		// next pc ++ ?
  	sc_out<bool>  			branch_valid;      	// branch valid signal
  	sc_out<unsigned >  		branch_target_address;	// branch target address
  	sc_out<bool>  			mem_access;      	// memory access valid 
  	sc_out<unsigned >  		mem_address;      	// memory physical address
  	sc_out<int>  			alu_op;      		// ALU/FPU/MMU Opcode
  	sc_out<bool>  			mem_write;      	// memory write enable
  	sc_out<unsigned>  		alu_src;      		// destination register number
  	sc_out<signed int>      src_A;				// operand A
  	sc_out<signed int>      src_B;				// operand B
  	sc_out<bool>  			decode_valid;      	// decoder output valid
  	sc_out<bool>  			float_valid;      	// enable FPU
	sc_out<bool>			gpio_access;		//gpio access valid
	sc_out<signed int>      dramdata;
	sc_in<bool>				dma_access;			//DMA access
	sc_out<bool>  			ireq0; 				// interrupt request 0
    sc_in_clk 				CLK;


    signed int cpu_reg[16];			//CPU register
	unsigned int dram_write_src;    //Whitch register write data from dram
  //Constructor 
  SC_CTOR(decode) {
      SC_CTHREAD(entry, CLK.pos());
        FILE *fp = fopen("register","r");
        int size=0;
        int mem_word;
		printf("** ALERT ** ID: initialize Architectural Registers\n");
        while (fscanf(fp,"%x", &mem_word) != EOF) {
                cpu_reg[size] = mem_word;
		size++;
	}
	dram_write_src=0;
  }

  // Process functionality in member function below
  void entry();
};


void decode::entry()
{
  unsigned int 	instr_tmp = 0;
  unsigned int 	opcode_tmp = 0;
  unsigned int  regA_tmp = 0;
  unsigned int  regB_tmp = 0;
  unsigned int  regC_tmp = 0;
  unsigned int  imm_tmp = 0;
  unsigned int  offset_tmp = 0;
  signed   int  label_tmp = 0;
  unsigned int  longlabel_tmp = 0;
  unsigned int  lastreg_tmp = 0;
  signed int	srcA_tmp = 0;
  signed int	srcB_tmp = 0;
  signed int	srcC_tmp = 0;
  int		i;
  unsigned int start_addr = 9000;
  unsigned int fin_addr =10000;
  

  branch_valid.write(false);
  decode_valid.write(false);
  float_valid.write(false);
  wait(2);

  while (true) {
	if (destreg_write.read() == true) {
		cpu_reg[destreg_write_src.read()] = alu_dataout.read();
		if (PRINT_ID) {
			cout << "\t\t\t-------------------------------" << endl;
			printf("\t\t\tID: R%d=0x%x(%d) fr ALU", destreg_write_src.read(), alu_dataout.read(),alu_dataout.read());
			cout.setf(ios::dec,ios::basefield);
   			cout << " at CSIM " << sc_time_stamp() << endl;
			cout << "\t\t\t-------------------------------" << endl;
		}

	}
	if (dram_rd_valid.read() == true && dma_access.read()!=true) {
		cpu_reg[dram_write_src] = dram_dataout.read();
		if (PRINT_ID) {
			cout << "\t\t\t-------------------------------" << endl;
			printf("\t\t\tID: R%d=0x%x(%d) fr MemLd/GPIO", dram_write_src, dram_dataout.read(), dram_dataout.read());
			cout.setf(ios::dec,ios::basefield);
   			cout << " at CSIM " << sc_time_stamp() << endl;
			cout << "\t\t\t-------------------------------" << endl;
		}
		dram_write_src=0;
	}
	if (fpu_valid.read() == true) {
		cpu_reg[fpu_destout.read()] = fpu_dout.read();
		if (PRINT_ID) {
			cout << "\t\t\t-------------------------------" << endl;
			printf("\t\t\tID: R%d=0x%x fr MMX", fpu_destout.read(), fpu_dout.read());
			cout.setf(ios::dec,ios::basefield);
   			cout << " at CSIM " << sc_time_stamp() << endl;
			cout << "\t\t\t-------------------------------" << endl;
		}
	}
	if ((instruction_valid.read() == true)) {
   		instr_tmp  = instruction.read();
		opcode_tmp 	= (instr_tmp & 0xff000000) >> 24; 
		regC_tmp   	= (instr_tmp & 0x00f00000) >> 20; 
		regA_tmp   	= (instr_tmp & 0x000f0000) >> 16; 
		regB_tmp   	= (instr_tmp & 0x0000f000) >> 12; 
		imm_tmp    	= (instr_tmp & 0x0000ffff); 
		offset_tmp 	= (instr_tmp & 0x00000fff); 
		label_tmp  	= (instr_tmp & 0x0000ffff); 
		longlabel_tmp   = (instr_tmp & 0x00ffffff); 

		//printf("opcode = %d regC = %d regA = %d regB = %d\n",opcode_tmp, regC_tmp, regA_tmp, regB_tmp);
		srcA_tmp = cpu_reg[regA_tmp];
		srcB_tmp = cpu_reg[regB_tmp];
		srcC_tmp = cpu_reg[regC_tmp];
		wait();
        lastreg_tmp = regC_tmp;
		if (PRINT_ID) {
			cout << "\t\t\t-------------------------------" << endl;
		}

		switch(opcode_tmp) {
			case 0x0:	// halt
				if (PRINT_ID) {
					printf("\n\n\t\t\t*******************************\n");
					printf("\t\t\tID: REGISTERS DUMP");
					cout << " at CSIM " << sc_time_stamp() << endl;
					printf("\t\t\t*******************************\n");
					printf("REG :==================================================================\n");
					for(i =0; i<16; i++){
						printf("  R%2d(%08x)  ",i, cpu_reg[i]);
						if ((i==3) || (i== 11) ||(i==7) || (i==15)){
							printf("\n");
						}
					}
					printf("=======================================================================\n\n");
				}
					wait();
					wait();
					break;
			case 0x01:	// add R1, R2, R3
				if (PRINT_ID) {
					printf("\t\t\tID: R%d= R%d(=%d)+R%d(=%d)", 
					regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
					src_A.write(srcA_tmp);
					src_B.write(srcB_tmp);
					alu_src.write(regC_tmp);	
					alu_op.write(3);	
					decode_valid.write(true);
					wait();
					decode_valid.write(false);
					wait();
					break;
			case 0x02:	// addi R1, R2, #value
				if (PRINT_ID) {
					printf("\t\t\tID: R%d= R%d(=%d)+%d", 
					regC_tmp, regA_tmp, srcA_tmp, imm_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
					src_A.write(srcA_tmp);
					src_B.write(imm_tmp);
					alu_src.write(regC_tmp);
					alu_op.write(3);	
					decode_valid.write(true);
					wait();
					decode_valid.write(false);
					wait();
					break;
			case 0x03:	// addc R1, R2, R3 + Carrybit
				if (PRINT_ID) {
					printf("\t\t\tID: R%d=  R%d(=%d)+R%d(=%d)+C", 
					regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
					src_A.write(srcA_tmp);
					src_B.write(srcB_tmp);
					alu_src.write(regC_tmp);
					alu_op.write(1);
					decode_valid.write(true);
					wait();
					decode_valid.write(false);
					wait();
					break;
			case 0x04:	// sub R1, R2, R3 
				if (PRINT_ID) {
					printf("\t\t\tID: R%d=R%d(=%d)-R%d(=%d)", 
					regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
                    src_A.write(srcA_tmp);
                    src_B.write(srcB_tmp);
                    alu_src.write(regC_tmp);
                    alu_op.write(4);
                    decode_valid.write(true);
                    wait();
                    decode_valid.write(false);
                    wait();
					break;
			case 0x05:	// subi R1, R2,  #value
				if (PRINT_ID) {
					printf("\t\t\tID: R%d=R%d(=%d)-%d", 
					regC_tmp, regA_tmp, srcA_tmp, imm_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
                    src_A.write(srcA_tmp);
                    src_B.write(imm_tmp);
                    alu_src.write(regC_tmp);
                    alu_op.write(4);
                    decode_valid.write(true);
                    wait();
                    decode_valid.write(false);
                    wait();
					break;
			case 0x06:	// subc R1, R2, R3 - Carrybit
				if (PRINT_ID) {
                    printf("\t\t\tID: R%d=R%d(=%d)-R%d(=%d)-C", 
					regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
                    src_A.write(srcA_tmp);
                    src_B.write(srcB_tmp);
                    alu_src.write(regC_tmp);
                    alu_op.write(2);
                    decode_valid.write(true);
                    wait();
                    decode_valid.write(false);
                    wait();
					break;
			case 0x07:	// mul R1, R2, R3 
				if (PRINT_ID) {
					printf("\t\t\tID: R%d=R%d(=%d)*R%d(=%d)", 	
					regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
                    src_A.write(srcA_tmp);
                    src_B.write(srcB_tmp);
                    alu_src.write(regC_tmp);
                    alu_op.write(5);
                    decode_valid.write(true);
                    wait();
                    decode_valid.write(false);
                    wait();
					break;
			case 0x08:	// div R1, R2, R3 
				if (PRINT_ID) {
					printf("\t\t\tID: R%d=R%d(=%d)/R%d(=%d)", 	
					regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
                    src_A.write(srcA_tmp);
                    src_B.write(srcB_tmp);
                    alu_src.write(regC_tmp);
                    alu_op.write(6);
                    decode_valid.write(true);
                    wait();
                    decode_valid.write(false);
                    wait();
					break;
			case 0x09:	// nand R1, R2, R3 
				if (PRINT_ID) {
					printf("\t\t\tID: R%d=R%d(=%x) nand R%d(=%x)", 	
					regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
                    src_A.write(srcA_tmp);
                    src_B.write(srcB_tmp);
                    alu_src.write(regC_tmp);
                    alu_op.write(7);
                    decode_valid.write(true);
                    wait();
                    decode_valid.write(false);
                    wait();
					break;
			case 0x0a:	// and R1, R2, R3 
				if (PRINT_ID) {
					printf("\t\t\tID: R%d=R%d(=%x) and R%d(=%x)", 	
					regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
                    src_A.write(srcA_tmp);
                    src_B.write(srcB_tmp);
                    alu_src.write(regC_tmp);
                    alu_op.write(8);
                    decode_valid.write(true);
                    wait();
                    decode_valid.write(false);
                    wait();
					break;
			case 0x0b:	// or R1, R2, R3 
				if (PRINT_ID) {
                    printf("\t\t\tID: R%d=R%d(=%x) or R%d(=%x)",
                    regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
                    src_A.write(srcA_tmp);
                    src_B.write(srcB_tmp);
                    alu_src.write(regC_tmp);
                    alu_op.write(9);
                    decode_valid.write(true);
                    wait();
                    decode_valid.write(false);
                    wait();
                    break;
			case 0x0c:	// xor R1, R2, R3 
				if (PRINT_ID) {
                    printf("\t\t\tID: R%d=R%d(=%x) xor R%d(=%x)",
                    regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
                    src_A.write(srcA_tmp);
                    src_B.write(srcB_tmp);
                    alu_src.write(regC_tmp);
                    alu_op.write(10);
                    decode_valid.write(true);
                    wait();
                    decode_valid.write(false);
                    wait();
                    break;
            case 0x0d:      // NOT R1, R2 
				if (PRINT_ID) {
                    printf("\t\t\tID: R%d= NOT R%d(=%x)",
                    regC_tmp, regA_tmp, srcA_tmp);
                    cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                    cout << "\t\t\t-------------------------------" << endl;
				}
                    src_A.write(srcA_tmp);
                    src_B.write(0);
                    alu_src.write(regC_tmp);
                    alu_op.write(11);
                    decode_valid.write(true);
                    wait();
                    decode_valid.write(false);
                    wait();
                    break;
            case 0x0e:      // modulo R1 = R2 mod R3 
				if (PRINT_ID) {
                    printf("\t\t\tID: R%d= R%d(=%x) mod R%d(=%x)", 
					regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
                    cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                    cout << "\t\t\t-------------------------------" << endl;
				}
                    src_A.write(srcA_tmp);
                    src_B.write(srcB_tmp);
                    alu_src.write(regC_tmp);
                    alu_op.write(14);
                    decode_valid.write(true);
                    wait();
                    decode_valid.write(false);
                    wait();
                    break;
			case 0x4b:	// ldgpio R1
				if (PRINT_ID) {
					printf("\t\t\tID: R%d<=gpio", 
					regC_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
					dram_write_src=regC_tmp;
					gpio_access.write(true);
					mem_write.write(false);
					wait();
					gpio_access.write(false);
					wait();
					break;
			case 0x4c:	// wrgpio R1
				if (PRINT_ID) {
					printf("\t\t\tID: R%d(0x%x)=>gpio", 
					regC_tmp, srcC_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
					gpio_access.write(true);
					mem_write.write(true);
					src_A.write(srcC_tmp);
					wait();
					gpio_access.write(false);
					mem_write.write(false);
					wait();
					break;
			case 0x4d:	// ld R1, R2, offset
				if (PRINT_ID) {
					printf("\t\t\tID: R%d<=mem[R%d=(%d)+%d]", 
					regC_tmp, regA_tmp, srcA_tmp, offset_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
					dram_write_src=regC_tmp;
					mem_access.write(true);
					mem_write.write(false);
					offset_tmp = offset_tmp + srcA_tmp;
					mem_address.write(offset_tmp);
					wait();
					mem_access.write(false);
					wait();
					break;
			case 0x4e:	// wr R1, R2, offset
				if (PRINT_ID) {
					printf("\t\t\tID: R%d(0x%x)=>mem[R%d(=%d) + %d]", 
					regC_tmp, srcC_tmp, regA_tmp, srcA_tmp, offset_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
					offset_tmp = offset_tmp + srcA_tmp;
				if (!(offset_tmp>=start_addr && offset_tmp<=fin_addr))
				{
					mem_access.write(true);
					mem_write.write(true);
					mem_address.write(offset_tmp);
					dramdata.write(srcC_tmp);
					wait();
					mem_access.write(false);
					mem_write.write(false);
					wait();
				}
				else
				{
					printf("\t\t\tID: You dont have permission to write in this mem area=0x%x", 
					offset_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
					cpu_reg[15]=0xf0000000+offset_tmp;
					ireq0.write(true);
					wait();
					ireq0.write(false);
				}
					break;
			case 0x50:	// sec R1, R2
				if(srcC_tmp<srcA_tmp)
				{
					start_addr=srcC_tmp;
					fin_addr=srcA_tmp;
					printf("\t\t\tID: Change secure area: start=0x%x fin=0x%x", 
					srcC_tmp, srcA_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
				else
				{
					printf("\t\t\tID: Error in change secure area: start=0x%x fin=0x%x", 
					srcC_tmp, srcA_tmp);
					cout << " \n\t\t\tStart>Fin\n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
				break;

			case 0x0f:	// mov R1, R2
				if (PRINT_ID) {
					printf("\t\t\tID: R%d=R%d(=%d)", 
					regC_tmp, regA_tmp, srcA_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
					src_A.write(srcA_tmp);
					src_B.write(0);
					alu_src.write(regC_tmp);
					alu_op.write(3);	
					decode_valid.write(true);
					wait();
					decode_valid.write(false);
					wait();
					break;
			case 0x10:	// beq R1, R2, label
					if (srcC_tmp == srcA_tmp) {
						branch_target_address.write(label_tmp);
						branch_valid.write(true);
						if (PRINT_ID) {
							printf("\t\t\tID: beq R%d(=%d), R%d(=%d), pc=(%d).\n", 
								regC_tmp, srcC_tmp, regA_tmp, srcA_tmp, label_tmp);
						}
					} else {
						if (PRINT_ID) {
							printf("\t\t\tID: beq R%d(=%d) != R%d(=%d),pc++.\n", 
							regC_tmp, srcC_tmp, regA_tmp, srcA_tmp);
						}
					}
					if (PRINT_ID) {
						cout << "\t\t\tID: at CSIM " << sc_time_stamp() << endl;
                		cout << "\t\t\t-------------------------------" << endl;
					}
					wait();
					decode_valid.write(false);
					branch_valid.write(false);
					wait();
					break;
			case 0x11:	// bne R1, R2, label
                    if (srcC_tmp != srcA_tmp) {
						branch_target_address.write(label_tmp);
						branch_valid.write(true);
						if (PRINT_ID) {
							printf("\t\t\tID: bne R%d(=%d), R%d(=%d), pc=(%d).\n",
							regC_tmp, srcC_tmp, regA_tmp, srcA_tmp, label_tmp);
						}
                    } else {
						if (PRINT_ID) {
							printf("\t\t\tID: bne R%d(=%d) = R%d(=%d),pc++.\n",
							regC_tmp, srcC_tmp, regA_tmp, srcA_tmp);
						}
                    }
					if (PRINT_ID) {
						cout << "\t\t\tID: at CSIM " << sc_time_stamp() << endl;
                		cout << "\t\t\t-------------------------------" << endl;
					}
                    wait();
                    decode_valid.write(false);
					branch_valid.write(false);
                    wait();
                    break;
			case 0x12:	// bgt R1, R2, label
                    if (srcC_tmp > srcA_tmp) {
						branch_target_address.write(label_tmp);
                        branch_valid.write(true);
						if (PRINT_ID) {
							printf("\t\t\tID: bgt R%d(=%d)>R%d(=%d), pc=(%d).\n",
							regC_tmp, srcC_tmp, regA_tmp, srcA_tmp, label_tmp);
						}
                    } else {
						if (PRINT_ID) {
							printf("\t\t\tID: bgt R%d(=%d) <= R%d(=%d),pc++.\n",
							regC_tmp, srcC_tmp, regA_tmp, srcA_tmp);
						}
                     }
					if (PRINT_ID) {
						cout << "\t\t\tID: at CSIM " << sc_time_stamp() << endl;
                		cout << "\t\t\t-------------------------------" << endl;
					}
                    wait();
                    decode_valid.write(false);
					branch_valid.write(false);
                    wait();
                    break;
			case 0x13:	// bge R1, R2, label
                    if (srcC_tmp >= srcA_tmp) {
						branch_target_address.write(label_tmp);
                        branch_valid.write(true);
						if (PRINT_ID) {
							printf("\t\t\tID: bge R%d(=%d)>=R%d(=%d), pc+=(%d).\n",
							regC_tmp, srcC_tmp, regA_tmp, srcA_tmp, label_tmp);
						}
                    } else {
						if (PRINT_ID) {
							printf("\t\t\tID: bge R%d(=%d) < R%d(=%d),pc++.\n",
							regC_tmp, srcC_tmp, regA_tmp, srcA_tmp);
						}
                    }
					if (PRINT_ID) {
						cout << "\t\t\tID: at CSIM " << sc_time_stamp() << endl;
                		cout << "\t\t\t-------------------------------" << endl;
					}
                    wait();
                    decode_valid.write(false);
					branch_valid.write(false);
                    wait();
                    break;
			case 0x14:	// blt R1, R2, label
                    if (srcC_tmp < srcA_tmp) {
						branch_target_address.write(label_tmp);
                        branch_valid.write(true);
						if (PRINT_ID) {
							printf("\t\t\tID: blt R%d(=%d)<R%d(=%d), pc=(%d).\n",
							regC_tmp, srcC_tmp, regA_tmp, srcA_tmp, label_tmp);
						}
                     } else {
						 if (PRINT_ID) {
							printf("\t\t\tID: blt R%d(=%d) >= R%d(=%d), pc++.\n",
							regC_tmp, srcC_tmp, regA_tmp, srcA_tmp);
						 }
                     }
					if (PRINT_ID) {
						cout << "\t\t\tID: at CSIM " << sc_time_stamp() << endl;
                		cout << "\t\t\t-------------------------------" << endl;
					}
                     wait();
                     decode_valid.write(false);
					 branch_valid.write(false);
                     wait();
                     break;
			case 0x15:	// ble R1, R2, label
                     if (srcC_tmp <= srcA_tmp) {
						branch_target_address.write(label_tmp);
                        branch_valid.write(true);
						if (PRINT_ID) {
							printf("\t\t\tID: ble R%d(=%d)<=R%d(=%d), pc=(%d).\n",
							regC_tmp, srcC_tmp, regA_tmp, srcA_tmp, label_tmp);
						}
                     } else {
						 if (PRINT_ID) {
							printf("\t\t\tID: ble R%d(=%d)>R%d(=%d), pc++.\n",
							regC_tmp, srcC_tmp, regA_tmp, srcA_tmp);
						 }
                     }
					 if (PRINT_ID) {
						cout << "\t\t\tID: at CSIM " << sc_time_stamp() << endl;
                		cout << "\t\t\t-------------------------------" << endl;
					 }
                     wait();
                     decode_valid.write(false);
					 branch_valid.write(false);
                     wait();
                     break;
			case 0x16:	// j label
                    branch_target_address.write(longlabel_tmp);
                    branch_valid.write(true);
					if (PRINT_ID) {
						printf("\t\t\tID: pc jump to => (%d).", longlabel_tmp);
						cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                		cout << "\t\t\t-------------------------------" << endl;
					}
                    wait();
                    decode_valid.write(false);
					branch_valid.write(false);
                    wait();
                    break;
			/*case 0x17:	// jal label for procedure call
					printf("\t\t\tID: j pc(%d) and sp <- pc", longlabel_tmp);
					cout << " at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
					wait();
					wait();
					break;
			case 0x18:	// jr
					printf("\t\t\tID: jr and pc <- sp\n");
					cout << " at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
					wait();
					wait();
					break;*/
            case 0x29:      // fadd R1, R2, R3
				if (PRINT_ID) {
                    printf("\t\t\tID: FR%d=FR%d(=%x)+FR%d(=%x)",
                    regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
                    src_A.write(srcA_tmp);
                    src_B.write(srcB_tmp);
                    alu_src.write(regC_tmp);
                    alu_op.write(3);
                    float_valid.write(true);
                    wait();
                    float_valid.write(false);
                    wait();
                    break;
             case 0x2a:      // fsub R1, R2, R3
				 if (PRINT_ID) {
                    printf("\t\t\tID: FR%d=FR%d(=%x)-FR%d(=%x)",
                    regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				 }
                    src_A.write(srcA_tmp);
                    src_B.write(srcB_tmp);
                    alu_src.write(regC_tmp);
                    alu_op.write(4);
                    float_valid.write(true);
                    wait();
                    float_valid.write(false);
                    wait();
                    break;
             case 0x2b:      // fmul R1, R2, R3
				 if (PRINT_ID) {
                    printf("\t\t\tID: FR%d=FR%d(=%x)*FR%d(=%x)",
                    regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				 }
                    src_A.write(srcA_tmp);
                    src_B.write(srcB_tmp);
                    alu_src.write(regC_tmp);
                    alu_op.write(5);
                    float_valid.write(true);
                    wait();
                    float_valid.write(false);
                    wait();
                    break;
              case 0x2c:      // fdiv R1, R2, R3
				  if (PRINT_ID) {
                    printf("\t\t\tID: FR%d=FR%d(=%x)/FR%d(=%x)",
                    regC_tmp, regA_tmp, srcA_tmp, regB_tmp, srcB_tmp);
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				  }
                    src_A.write(srcA_tmp);
                    src_B.write(srcB_tmp);
                    alu_src.write(regC_tmp);
                    alu_op.write(6);
                    float_valid.write(true);
                    wait();
                    float_valid.write(false);
                    wait();
                    break;
			  case 0xe0:      // flush register
				  if (PRINT_ID) {
                    printf("\t\t\tID: flush all registers");
                    cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				  }
                    src_A.write(0);
                    src_B.write(0);
                    alu_src.write(0);
                    alu_op.write(0);
					for (i = 0; i< 16; i++) {
						cpu_reg[i] = 0;
					}
                    wait();
                    wait();
                    break;
			case 0xf1:	// movi R1, #value
				if (PRINT_ID) {
					printf("\t\t\tID: R%d=%d", 
					regC_tmp, imm_tmp);
					cout << " at CSIM " << sc_time_stamp() << endl;
                	cout << "\t\t\t-------------------------------" << endl;
				}
					src_A.write(imm_tmp);
					src_B.write(0);
					alu_src.write(regC_tmp);
					alu_op.write(3);	
					decode_valid.write(true);
					wait();
					decode_valid.write(false);
					wait();
					break;
            case 0xff:      // QUIT
					printf("\t\t\tID: - SHUTDOWN - ");
					cout << "at CSIM " << sc_time_stamp() << endl;
					decode_valid.write(false);
					float_valid.write(false);
					wait();
					printf("\t\t\tID: - PLEASE WAIT ...... - \n");
                	cout << "\t\t\t-------------------------------" << endl;
					sc_stop();
					printf("\n\n\n////////////////////////////////////////////////////////////////////////////////\n");
                    wait();
					wait();
					break;
			default :
					printf("\t\t\tID: INVALID OPCODE");
					cout << " \n\t\t\t  : at CSIM " << sc_time_stamp() << endl;
					wait();
					break;
		}
		next_pc.write(true);
		wait();
	} else {
		next_pc.write(true);
		wait();
	}
  }
} // end of entry function
