#include "3ac.hpp"
#include "vector"
#include "type_analysis.hpp"

namespace a_lang {

Procedure * IRProgram::makeProc(std::string name){
	Procedure * proc = new Procedure(this, name);
	procs->push_back(proc);
	return proc;
}

std::list<Procedure *> * IRProgram::getProcs(){
	return procs;
}

const DataType * IRProgram::nodeType(ASTNode * node){
	return ta->nodeType(node);
}

size_t IRProgram::opWidth(ASTNode * node){
	return Opd::width(nodeType(node));
}

Label * IRProgram::makeLabel(){
	Label * label = new Label("lbl_" + std::to_string(max_label++));
	return label;
}

SymOpd * IRProgram::getGlobal(SemSymbol * sym){
	if (globals.find(sym) != globals.end()){
		return globals[sym];
	}
	return nullptr;
}

void IRProgram::gatherGlobal(SemSymbol * sym){
	size_t width = Opd::width(sym->getDataType());
	SymOpd * res = new SymOpd(sym, width);
	globals[sym] = res;
}

Opd * IRProgram::makeString(std::string val){
	std::string name = "str_" + std::to_string(str_idx++);
	LitOpd * opd = new LitOpd(name, 8);
	strings[opd] = val;
	return opd;
}

std::string IRProgram::toString(bool verbose){
  std::string res = "";
  res += "[BEGIN GLOBALS]\n";
  for (auto entry : globals){
    res += entry.second->getName() + " (global var of "
      + std::to_string(entry.second->getWidth())
      + " bytes)\n";
  }
  for (auto entry : strings){
    res += entry.first->valString();
    res += " " + entry.second;
    res += "\n";
  }

  res += "[END GLOBALS]\n";

  // Skip the init procedure in the output
  // If init is the first procedure in the list, start from the second one
  for (Procedure * proc : *procs){
    if (proc != init) {  // Skip the init procedure
      res += proc->toString(verbose);
    }
  }
  return res;
}

std::set<Opd *> IRProgram::globalSyms(){
	std::set<Opd *> result;
	for (auto gItr : globals){
		result.insert(gItr.second);
	}
	return result;
}

}
