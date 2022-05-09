#include "define.h"
#include <iostream>
#include <ac_int.h>
#include <ac_channel.h>
#include <mc_scverify.h>

#pragma hls_design top
class RISC {
    // PC and Flags
    ac_int<10,false> pc, previous_pc;
    // RegisterFile
    ac_int<32,true> RegisterFile[128];  
public:
    // Input Output

    RISC(){
		#ifndef __SYNTHESIS_
        std::cout << "RISC, or Reduced Instruction Set Computer. is a type of microprocessor architecture that utilizes a small, highly-optimized set of instructions" << std::endl << std::endl;
        #endif 
        pc = 0;

        ac::init_array<AC_VAL_0>(RegisterFile,128);
    }
    void fetch_decode(
        ac_int<32,false> InstructionMemory[1024],
        ac_int<3,false> &opcode,
        ac_int<2,false> &skip,
        ac_int<2,false> &shift,
        ac_int<3,false> &function,
        ac_int<32,true> &operand_1,
        ac_int<32,true> &operand_2,

        ac_int<7,false> &RegisterSrcB,
        ac_int< 7, false> &RegisterDst,

        ac_int<32,false> &constant,
        ac_int<1,false> &ld_constant

        ) {
        
        ac_int<32,false> instruction = InstructionMemory[pc];	
        ac_int<7,false> RegisterSrcA;

        opcode = instruction.slc<3>(0);
        skip = instruction.slc<2>(3);
        shift = instruction.slc<2>(5);
        function = instruction.slc<3>(7);
        RegisterSrcB = instruction.slc<7>(10);
        RegisterSrcA = instruction.slc<7>(17);
        ld_constant = instruction.slc<1>(24);
        RegisterDst = instruction.slc<7>(25);
        operand_1=RegisterFile[RegisterSrcA];
		operand_2=RegisterFile[RegisterSrcB];
        constant=instruction.slc<24>(0);
    }

    void execute(
    ac_int<32,true> operand_1,
    ac_int<32,true> operand_2,
    ac_int<2,false> skip,
    ac_int<2,false> shift,
    ac_int<3,false> function,
    ac_int<3,false> opcode,
    ac_int<1,false> ld_constant,
    ac_int<32,true> &ALU_output) {
    bool doskip;
    if(!ld_constant){    
        switch(function) {
            case AplusB:
                ALU_output = operand_1 + operand_2;
                break;
            case AminusB:
                ALU_output = operand_1 - operand_2;
                break;
            case Bplusone:
                ALU_output =  operand_2 + 1;
                break;
            case Bminusone:
                ALU_output =  operand_2 - 1;
                break;
            case AND:
                ALU_output = operand_1 & operand_2;
                break;
            case OR:
                ALU_output = operand_1 | operand_2;
                break;
            case XOR:
                ALU_output = operand_1 ^ operand_2;
                break;
            case RsrvdFun:
            	break;
        }
        switch(skip) {
            case Never:
                doskip = 0;
                break;
            case IfNegative:
                doskip = (ALU_output < 0);
                break;
            case IfZero:
                doskip = (ALU_output == 0);
                break;
            case IfPositive:
                doskip = (ALU_output > 0);
                break;
        }
        ac_int<1,false>  bit = ALU_output.slc<1>(0);
        ac_int<8,false>  bit8 = ALU_output.slc<8>(0);
        ac_int<16,false> bit16 = ALU_output.slc<16>(0);
        switch(shift) {
            case 0:
                break;
            case 1:
                ALU_output = ALU_output >> 1;
                ALU_output.set_slc(31,bit);
                break;
            case 2:
                ALU_output = ALU_output >> 8;
                ALU_output.set_slc(24,bit8);
                break;
            case 3:
                ALU_output = ALU_output >> 16;
                ALU_output.set_slc(16,bit16);
                break;
        }
    }
    previous_pc = pc;
    pc_update(opcode,doskip,ALU_output);
        
    }
    void memory_write_back(
        ac_int<3,false> opcode,
        ac_int<32,false> constant,
        ac_int<1,false> ld_constant,
        ac_int<7,false> RegisterSrcB,
        ac_int<32,true> ALU_output,
        ac_int<32,true> operand_1,
        ac_int< 7, false> RegisterDst,

        ac_channel<ac_int<32,true> > &in,
        ac_channel<ac_int<32, true> > &out,
        ac_int<32,false> InstructionMemory[1024],
        ac_int<32,true> DataMemory[1024]
        
        ) {

        ac_int<32,true> Register_write;
	    ac_int<32,true> Data_read;


		if(ld_constant) {
            Register_write = constant;
        } else {
            switch(opcode) {
                case Normal:
                    Register_write = ALU_output;
                    break;
                case StoreDM:
                    DataMemory[RegisterSrcB]= operand_1;
                    Register_write = ALU_output;
                    break;
                case StoreIM:
                    InstructionMemory[RegisterSrcB]= operand_1;
                    Register_write = ALU_output;
                    break;
                case Output:
                    out.write(operand_1);
                    Register_write = ALU_output;
                    break;
                case LoadDM:
                    Data_read=DataMemory[RegisterSrcB];
                    Register_write = Data_read;
                    break;
                case Input:
                    if(in.nb_read(Register_write))    
                    	break;
                case Jump:
                    Register_write = previous_pc + 1;
                    break;
                case RsrvdOp:
                	break;
            }
        }
        RegisterFile[RegisterDst]=Register_write;
        
    }
    void pc_update(
        ac_int<3,false> opcode,
        bool doskip,
        ac_int<32,true> ALU_output
        ){
        if(opcode != Jump) {
            pc = (doskip) ? (pc+2) : (pc+1);
         }
         else{
         	pc = ALU_output;
		 }
    }
    #pragma hls_design interface
    void CCS_BLOCK(run)(
        ac_channel<ac_int<32,true> > &input,
        ac_channel<ac_int<32,true> > &output,
        ac_int<32,false> InstructionMemory[1024],
        ac_int<32,true> DataMemory[1024]) {
        
        ac_int<32,true> ALU_output;
        ac_int<3,false> opcode;
        ac_int<2,false> skip;
        ac_int<2,false> shift;
        ac_int<3,false> function;
        ac_int<32,true> operand_1;
        ac_int<32,true> operand_2;
        ac_int<7,false> RegisterSrcB;
        ac_int< 7, false> RegisterDst;
        ac_int<32,false> constant;
        ac_int<1,false> ld_constant;


        fetch_decode(InstructionMemory, 
                    opcode, skip, shift, function, operand_1, operand_2,
                    RegisterSrcB,
                    RegisterDst,
                    constant,
                    ld_constant);
        execute(operand_1,
                operand_2,
                skip,
                shift,
                function,
                opcode,
                ld_constant,
                ALU_output);
        memory_write_back(opcode,
                        constant,
                        ld_constant,
                        RegisterSrcB,
                        ALU_output,
                        operand_1,
                        RegisterDst,
                        input,
                        output,
                        InstructionMemory,
                        DataMemory);
    };


    //helper function not for HLS
    void printRegister(int i) {
	#ifndef __SYNTHESIS_
        std::cout << "Register[" << i << "] = " << RegisterFile[i] << std::endl;
    #endif 
    };
    
    void printPC(){
	#ifndef __SYNTHESIS_
        std::cout << "PC = " << pc << std::endl;
    #endif 
    }

};



