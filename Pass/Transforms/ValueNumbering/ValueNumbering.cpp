#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/PassManager.h"

#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <sstream>

using namespace std;

using namespace llvm;

namespace {

// This method implements what the pass does
void visitor(Function &F){

    // Here goes what you want to do with a pass
    
        string func_name = "main";
        errs() << "ValueNumbering: " << F.getName() << "\n";
        
        // Comment this line
        if (F.getName() != func_name) return;
        
        for (auto& basic_block : F)
        {
            for (auto& inst : basic_block)
            {
                errs() << inst << "\n";
                if(inst.getOpcode() == Instruction::Load){
                    errs() << "This is Load"<<"\n";
                }
                if(inst.getOpcode() == Instruction::Store){
                    errs() << "This is Store"<<"\n";
                }
                if (inst.isBinaryOp())
                {
                    errs() << "Op Code:" << inst.getOpcodeName()<<"\n";
                    if(inst.getOpcode() == Instruction::Add){
                        errs() << "This is Addition"<<"\n";
                    }
                    if(inst.getOpcode() == Instruction::Load){
                        errs() << "This is Load"<<"\n";
                    }
                    if(inst.getOpcode() == Instruction::Mul){
                        errs() << "This is Multiplication"<<"\n";
                    }
                    
                    // see other classes, Instruction::Sub, Instruction::UDiv, Instruction::SDiv
                    // errs() << "Operand(0)" << (*inst.getOperand(0))<<"\n";
                    auto* ptr = dyn_cast<User>(&inst);
                    //errs() << "\t" << *ptr << "\n";
                    for (auto it = ptr->op_begin(); it != ptr->op_end(); ++it) {
                        errs() << "\t" <<  *(*it) << "\n";
                        // if ((*it)->hasName()) 
                        // errs() << (*it)->getName() << "\n";                      
                    }
                } // end if
            } // end for inst
        } // end for block

}


map<string, int> hashTable;
int value_num = 1;
bool found = false;

int inHashTable(string temp){
    auto search = hashTable.find(temp);
    auto result = value_num;
    if(search == hashTable.end()){
        found = false;
        hashTable.insert(pair<string,int>(temp,value_num++));
    }else{
        found =  true;
        result = search->second;
    }
    return result;
}

// New PM implementation
struct ValueNumberingPass : public PassInfoMixin<ValueNumberingPass> {

  // The first argument of the run() function defines on what level
  // of granularity your pass will run (e.g. Module, Function).
  // The second argument is the corresponding AnalysisManager
  // (e.g ModuleAnalysisManager, FunctionAnalysisManager)
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    //visitor(F);

    string operation = "";
    string load_op="", store_op="";
    string name = "";
    string temp1, temp2;
    string temp3;
    int op=0, op1=0, op2=0, op3=0;
    string answer = "";

    string func_name = "test";

    errs() << "ValueNumbering: ";
    errs() << F.getName() << "\n";
    /*if (F.getName() != func_name) 
        return PreservedAnalyses::all();*/
    
    for (auto& basic_block : F)
    {
        for (auto& inst : basic_block)
        {
            //errs() << inst << "\n";
            if(inst.getOpcode() == Instruction::Load){
                //errs() << "This is Load"<<"\n";
                load_op = inst.getOperand(0)->getName().str();
                op = inHashTable(load_op);
            }
            if(inst.getOpcode() == Instruction::Store){
                //errs() << "This is Store"<<"\n";
                store_op = inst.getOperand(0)->getName().str();
                if(store_op == ""){
                    User* abcde = dyn_cast<User>(inst.getOperand(0));

                    if(abcde->getOperand(0)->getName() == ""){
                        stringstream ss;
                        auto* constantVal = dyn_cast<ConstantInt>(abcde);
                        ss << constantVal->getZExtValue();
                        auto search = hashTable.find(inst.getOperand(1)->getName().str());
                        if(search == hashTable.end()){
                            hashTable.insert(pair<string,int>(inst.getOperand(1)->getName().str(),value_num++));
                        }else{
                            hashTable.erase(inst.getOperand(1)->getName().str());
                            hashTable.insert(pair<string,int>(inst.getOperand(1)->getName().str(),value_num++));
                        }
                        op = hashTable.find(inst.getOperand(1)->getName().str())->second;
                        hashTable.insert(pair<string,int>(ss.str(),op));
                    }else{
                        auto search = hashTable.find(abcde->getOperand(0)->getName().str());
                        if(search == hashTable.end()){
                        }else{
                            op = search->second;
                            map<string, int>::iterator itr;
                            itr = hashTable.find(inst.getOperand(1)->getName().str());
                            if(itr != hashTable.end()){
                                hashTable.erase(inst.getOperand(1)->getName().str());
                                hashTable.insert(pair<string,int>(inst.getOperand(1)->getName().str(),op));
                            }
                        }
                    }
                    
                }else{
                    auto search = hashTable.find(store_op);
                    if(search == hashTable.end()){
                        //errs() << "inside false" << "\n";
                        store_op = inst.getOperand(1)->getName().str();
                        op = inHashTable(store_op);
                    }else{
                        op = search->second;
                        //hashTable.insert(pair<string,int>(inst.getOperand(1)->getName().str(),op));
                        map<string, int>::iterator itr;
                        itr = hashTable.find(inst.getOperand(1)->getName().str());
                        if(itr != hashTable.end()){
                            itr->second = op;
                        }else{
                            hashTable.insert(pair<string,int>(inst.getOperand(1)->getName().str(),op));
                        }
                    }
                }
            }

            answer = to_string(op) + " = " + to_string(op);

            if (inst.isBinaryOp()){
                //errs() << "Op Code:" << inst.getOpcodeName()<<"\n";
                if(inst.getOpcode() == Instruction::Add){
                    operation = " add ";
                }
                if(inst.getOpcode() == Instruction::Sub){
                    operation = " sub ";
                }
                if(inst.getOpcode() == Instruction::Mul){
                    operation = " mul ";
                }
                if(inst.getOpcode() == Instruction::UDiv){
                    operation = " div ";
                }

                auto* ptr = dyn_cast<User>(&inst);
                int i=0;
                for (auto it = ptr->op_begin(); it != ptr->op_end(); ++it) 
                {                   
                    User* instr = dyn_cast<User>(it);
                    User* abcde = dyn_cast<User>(instr->getOperand(0));
                    //errs() << "\t" << *(*it) << "\n";
                    if(i == 0){
                        temp1 = instr->getOperand(0)->getName().str();
                        if((temp1 == "") && (abcde->getOperand(0)->getName() == "")){
                            stringstream ss1;
                            auto* constantVal = dyn_cast<ConstantInt>((it));
                            ss1 << constantVal->getZExtValue();
                            temp1 = ss1.str();
                        }
                    }else{
                        temp2 = instr->getOperand(0)->getName().str();
                        if((temp2 == "") && (abcde->getOperand(0)->getName() == "")){
                            stringstream ss2;
                            auto* constantVal = dyn_cast<ConstantInt>((it));
                            ss2 << constantVal->getZExtValue();
                            temp2 = ss2.str();
                        }
                    }              
                    i++;
                }
                
                op1 = inHashTable(temp1);
                op2 = inHashTable(temp2);
                temp3 = to_string(op1) + operation + to_string(op2);
                op3 = inHashTable(temp3);

                if(found == true){
                    answer = to_string(op3) + " = " + to_string(op1) + operation + to_string(op2) + " (redundant) ";
                    found = false;
                }else{
                    answer = to_string(op3) + " = " + to_string(op1) + operation + to_string(op2);
                }
                hashTable.insert(pair<string,int>(ptr->getName().str(),op3));
            }
            if((op!=0) && (inst.getOpcode()!=1){
                errs() << inst << "\t" << answer << "\n";
            }
            
        }
    }
    return PreservedAnalyses::all();
  }
  
    static bool isRequired() { return true; }
};
}



//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "ValueNumberingPass", LLVM_VERSION_STRING,
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
        ArrayRef<PassBuilder::PipelineElement>) {
          if(Name == "value-numbering"){
            FPM.addPass(ValueNumberingPass());
            return true;
          }
          return false;
        });
    }};
}
