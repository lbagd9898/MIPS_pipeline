#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;

#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab csa23, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;  
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;  
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;  
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        wrt_enable;    
    bool        nop;    
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;     
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr)
        {   
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
		 
		void outputRF(int cycle)
		{
			ofstream rfout;
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF (Cycle="<<cycle<<")\t"<<endl;
                
				for (int j = 0; j<32; j++)
				{        
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open file";
			rfout.close();               
		} 
			
	private:
		vector<bitset<32> >Registers;	
};

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {       
			IMem.resize(MemSize); 
            ifstream imem;
			string line;
			int i=0;
			imem.open("imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{      
					IMem[i] = bitset<8>(line);
					i++;
				}                    
			}
            else cout<<"Unable to open file";
			imem.close();                     
		}
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public:
        bitset<32> ReadData;  
        DataMem()
        {
            DMem.resize(MemSize); 
            ifstream dmem;
            string line;
            int i=0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {      
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();          
        }
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("dmemresult.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {     
                    dmemout << DMem[j]<<endl;
                }
                     
            }
            else cout<<"Unable to open file";
            dmemout.close();               
        }             
      
    private:
		vector<bitset<8> > DMem;      
};  

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;        
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
    }
    else cout<<"Unable to open file";
    printstate.close();
}
 

int main()
{
    
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
			
    stateStruct state, new_state;

    int cycle = 0;
    printState(state, cycle);
             
    while (1) {

        //initializes branching logic
        bool branch = false;
        bitset<32> branch_address;
        /* --------------------- WB stage --------------------- */
        //write back to register provided wrt_enable and register addr isn't 0
        if (!state.WB.nop && state.WB.wrt_enable && state.WB.Wrt_reg_addr != 0) {
            myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
        }
        
        /* --------------------- MEM stage --------------------- */
        if (!state.MEM.nop) {
            new_state.WB.nop = 0; 
            // sw - wrote store data to address calculated by ALU
            if (state.MEM.wrt_mem) {
                myDataMem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);
                // lw
            } else if (state.MEM.rd_mem) {
                bitset<32> lw = myDataMem.readDataMem(state.MEM.ALUresult);
                new_state.WB.Wrt_data = lw;
                // r type;
            } else {
                new_state.WB.Wrt_data = state.MEM.ALUresult;
            }
            //passes data into new state for WB
            new_state.WB.Rs = state.MEM.Rs;
            new_state.WB.Rt = state.MEM.Rt;
            new_state.WB.wrt_enable = state.MEM.wrt_enable;
            new_state.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
            new_state.WB.nop = 0;
            new_state.WB.wrt_enable = state.MEM.wrt_enable;
        } else {
            new_state.WB.nop = 1;
        }
        /* --------------------- EX stage --------------------- */
        if (!state.EX.nop) {
            //calculate lw/sw address
            if (state.EX.is_I_type) {
                bool bit_15 = state.EX.Imm[15];
                bitset<16> extension;
                if (bit_15) extension.set(); else extension = 0;
                bitset<32>sign_ext_imm((extension.to_ulong() << 16) | (state.EX.Imm.to_ulong())); 
                new_state.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() + sign_ext_imm.to_ulong());
                new_state.MEM.Store_data = state.EX.Read_data2;
                //register operations
            } else {
                if (state.EX.alu_op == 1) {
                    new_state.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong());
                } else if (state.EX.alu_op == 0) {
                    new_state.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() - state.EX.Read_data2.to_ulong());
                }
            }
            //pass the rest of the data forward
            new_state.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
            new_state.MEM.Rs = state.EX.Rs;
            new_state.MEM.Rt = state.EX.Rt;
            new_state.MEM.rd_mem = state.EX.rd_mem;
            new_state.MEM.wrt_mem = state.EX.wrt_mem; 
            new_state.MEM.wrt_enable = state.EX.wrt_enable;       
        } else {
            new_state.MEM.nop = 1;
        }
          
        bool stall = false;
        /* --------------------- ID stage --------------------- */
        if (!state.ID.nop) {
            new_state.EX.nop = 0;
            unsigned long int_ins = state.ID.Instr.to_ulong();
            bitset<6> opcode(int_ins >> 26);
            //declare rs and rt
            new_state.EX.Rs = bitset<5>((int_ins >>21) & 0x1F);
            new_state.EX.Rt = bitset<5>((int_ins >> 16) & 0x1F);
            // for r and i type we read rs
            new_state.EX.Read_data1 = myRF.readRF(new_state.EX.Rs);
            new_state.EX.Read_data2 = myRF.readRF(new_state.EX.Rt);
            //r-type
            if (opcode  == 0) {
                //read data from registers
                new_state.EX.Wrt_reg_addr = bitset<5>((int_ins >> 11) & 0x1F);
                bitset<6> funct(int_ins & 0x3F); 
                if (funct == 0x21) {
                    new_state.EX.alu_op = 1;
                } else if (funct == 0x23) {
                    new_state.EX.alu_op = 0;
                }
                new_state.EX.rd_mem = 0;
                new_state.EX.wrt_mem = 0;
                new_state.EX.wrt_enable = 1;
                new_state.EX.is_I_type = 0;
                new_state.EX.Imm = 0;
                //I-type
            } else {
                new_state.EX.is_I_type = 1;
                new_state.EX.Imm = bitset<16>(int_ins & 0xFFFF);
                //lw
                if (opcode == 0x23) {
                    new_state.EX.wrt_enable = 1;
                    new_state.EX.rd_mem = 1;
                    new_state.EX.wrt_mem = 0;
                    new_state.EX.Wrt_reg_addr = new_state.EX.Rt;
                    new_state.EX.alu_op = 1;
                    //sw
                } else if (opcode == 0x2B) {
                    new_state.EX.wrt_enable = 0;
                    new_state.EX.rd_mem = 0;
                    new_state.EX.wrt_mem = 1;
                    new_state.EX.Wrt_reg_addr = bitset<5>(0);
                    new_state.EX.alu_op = 1;
                    //bne
                } else if (opcode == 0x05) {
                    //new address
                    if (new_state.EX.Read_data1 != new_state.EX.Read_data2) {
                        bool bit_15 = new_state.EX.Imm[15];
                        bitset<16> extension;
                        if (bit_15) extension.set(); else extension = 0;
                        bitset<32>sign_ext_imm((extension.to_ulong() << 18) | (new_state.EX.Imm.to_ulong() << 2)); 
                        branch = true;
                        branch_address = bitset<32>(state.IF.PC.to_ulong() + sign_ext_imm.to_ulong());
                    }
                    new_state.EX.nop = 0;
                    new_state.EX.rd_mem = 0;
                    new_state.EX.wrt_mem = 0;
                    new_state.EX.wrt_enable = 0;
                    new_state.EX.alu_op = 1;
                    new_state.EX.Wrt_reg_addr = bitset<5>(0);
                }
            }
        } else {
            new_state.EX.nop = 1;
        }
        
        /* --------------------- IF stage --------------------- */
        if (!state.IF.nop) {
            //if branch, we use new branch address
            if (branch) {
                new_state.IF.nop = 0;
                //pc at new branch address
                new_state.IF.PC = branch_address;
                //previous instruction is squashed
                new_state.ID.nop   = 1;
            // or we use PC + 4 and update counter
            } else {
                bitset<32> instr = myInsMem.readInstr(state.IF.PC);
                //halt
                if (instr == bitset<32>(0xFFFFFFFF)) {     
                    new_state.ID.nop = 1;
                    new_state.IF.nop = 1;  
                    new_state.IF.PC = state.IF.PC;
                    //normal instruction 
                } else {
                    new_state.IF.PC  = bitset<32>(state.IF.PC.to_ulong() + 4);
                    new_state.ID.Instr = instr;
                    new_state.ID.nop = 0;
                }
            }
        } else if (state.IF.nop) {
            new_state.ID.nop = 1;
            new_state.IF.nop = 1;
        }

             
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop) {
            break;
        }
            
        
        printState(new_state, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
       
        state = new_state; /*** The end of the cycle and updates the current state with the values calculated in this cycle. csa23 ***/ 
        
        myRF.outputRF(cycle); // dump RF;

        cycle++;
    }
    	
	myDataMem.outputDataMem(); // dump data mem 
	
	return 0;
}

// a06ab8e681fd21c64d696c93bec4b1ef