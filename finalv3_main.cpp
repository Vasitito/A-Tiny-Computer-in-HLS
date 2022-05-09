#include "define.h"
#include <iostream>
#include <ac_int.h>
#include <ac_channel.h>
#include <mc_scverify.h>
#include "finalv3.h"


CCS_MAIN(int argc,char* argv[]){ 
    
    RISC processor;

    ac_int<32,false> InstructionMemory[1024];
    ac_int<32,true> DataMemory[1024];

    ac_channel<ac_int<32,true> > input;
    ac_channel<ac_int<32, true> > output;
    input.write(75);
                                        //  Rw(7) LC(1) Ra(7) Rb(7) Fu(3) Sh(2) Sk(2) Op(3)
    InstructionMemory[0] = 4;           // 0000000 0 0000000 0000000 000 00 00 100  LOAD RF[0] <- DM[0] 
    InstructionMemory[1] = 33555460;    // 0000001 0 0000000 0000001 000 00 00 100  LOAD RF[1] <- DM[1]
    InstructionMemory[2] = 67110916;    // 0000010 0 0000000 0000010 000 00 00 100  LOAD RF[2] <- DM[2]
    InstructionMemory[3] = 335675392;   // 0001010 0 0000001 0000000 000 00 00 000  RF[10] <- RF[1] + RF[0]
    InstructionMemory[4] = 369232000;   // 0001011 0 0000001 0000010 001 00 00 000  RF[11] <- RF[1] - RF[2]
    InstructionMemory[5] = 402653440;   // 0001100 0 0000000 0000000 010 00 00 000  RF[12] <- RF[0] + 1
    InstructionMemory[6] = 436209024;   // 0001101 0 0000000 0000001 011 00 00 000  RF[13] <- RF[1] - 1
    InstructionMemory[7] = 2115243009;  // 0111111 0 0001010 0000011 000 00 00 001  DM[3] <- RF[10]
    InstructionMemory[8] = 2115375105;  // 0111111 0 0001011 0000100 000 00 00 001  DM[4] <- RF[11]
    InstructionMemory[9] = 2115507201;  // 0111111 0 0001100 0000101 000 00 00 001  DM[5] <- RF[12]
    InstructionMemory[10] = 100673540;  // 0000011 0 0000000 0001010 000 00 00 100  LOAD RF[3] <- DM[10]
    InstructionMemory[11] = 134228996;  // 0000100 0 0000000 0001011 000 00 00 100  LOAD RF[4] <- DM[11]
    InstructionMemory[12] = 168300032;  // 0000101 0 0000100 0000011 100 00 00 000  RF[5] = RF[3] AND RF[4]
    InstructionMemory[13] = 201854592;  // 0000110 0 0000100 0000011 101 00 00 000  RF[6] = RF[3] OR RF[4]
    InstructionMemory[14] = 235409152;  // 0000111 0 0000100 0000011 110 00 00 000  RF[7] = RF[3] XOR RF[4]
    InstructionMemory[15] = 4262865032; // 1111111 0 0001011 0001010 001 00 01 000 SKIP IF ALU < 0
    InstructionMemory[16] = 0;          // SKIPPED
    InstructionMemory[17] = 4262999184; // 1111111 0 0001100 0001101 001 00 10 000 SKIP IF ALU = 0
    InstructionMemory[18] = 0;          // SKIPPED
    InstructionMemory[19] = 4262734872; // 1111111 0 0001010 0001011 000 00 11 000 SKIP IF ALU > 0
    InstructionMemory[20] = 0;          // SKIPPED
    InstructionMemory[21] =	4262734880; // 1111111 0 0001010 0001011 000 01 00 000 RF[127]<-RCY 1(RF[10] + RF[11])
    InstructionMemory[22] =	4262734912; // 1111111 0 0001010 0001011 000 10 00 000 RF[127]<-RCY 8(RF[10] + RF[11])
    InstructionMemory[23] =	4262734944; // 1111111 0 0001010 0001011 000 11 00 000 RF[127]<-RCY 16(RF[10] + RF[11])
    InstructionMemory[24] =	4261421061; // 1111111 0 0000000 0001000 000 00 00 101 RF[127]<-INPUT_CHANNEL
    InstructionMemory[25] =	4261543939; // 1111111 0 0000001 0000000 000 00 00 011 RF[1]->OUTPUT_CHANNEL && RF[127]<-RF[1] + RF[0]
	InstructionMemory[26] =	4261940226; // 1111111 0 0000100 0000011 000 00 00 010 RF[127]<-(RF[3]+RF[4]) && IM[3]<-RF[4]
	InstructionMemory[27] =	2130791129; // 0111111 1 0000000 1010010 101 10 11 001 RF[63]<-CONSTANT(84697)
	InstructionMemory[28] = 4262997126; // 1111111 0 0001100 0001011 001 00 00 110 RF[127]<-PC+1, PC<-(RF[12]-RF[11])
    
	// DataMemory
    DataMemory[0] = 19;
    DataMemory[1] = 21;
    DataMemory[2] = 11;
	
    DataMemory[10] = 43690; // 00000000000000001010101010101010
    DataMemory[11] = 21845; // 00000000000000000101010101010101
    
    //DataMemory[10] results

    
    for(int i = 0; i < 27; i++) {
        processor.printPC();
        processor.run(input, output, InstructionMemory,DataMemory);
        processor.printRegister(0);
        processor.printRegister(1);
        processor.printRegister(2);
        processor.printRegister(3);
        processor.printRegister(4);
        processor.printRegister(5);
        processor.printRegister(6);
        processor.printRegister(7);
        processor.printRegister(10);
        processor.printRegister(11);
        processor.printRegister(12);
        processor.printRegister(13);
        processor.printRegister(63);
        processor.printRegister(127);
        std::cout << "DataMemory[0] = " << DataMemory[0] << std::endl;
        std::cout << "DataMemory[1] = " << DataMemory[1] << std::endl;
        std::cout << "DataMemory[2] = " << DataMemory[2] << std::endl;
        std::cout << "DataMemory[3] = " << DataMemory[3] << std::endl;
        std::cout << "DataMemory[4] = " << DataMemory[4] << std::endl;
        std::cout << "DataMemory[5] = " << DataMemory[5] << std::endl;
        if(i==22)
        {
        	std::cout<<"The output channel has the value: "<<output.read()<<std::endl;
		}
        
        if(i==23)
        {
        	std::cout<<"InstructionMemory[3]:"<<InstructionMemory[3]<<std::endl;
		}
		
		std::cout << std::endl;
    }
    
    CCS_RETURN(0);
}
