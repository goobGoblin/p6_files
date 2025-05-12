#include "ast.hpp"

namespace a_lang{

IRProgram * ProgramNode::to3AC(TypeAnalysis * ta){
	IRProgram * prog = new IRProgram(ta);
	for (auto global : *myGlobals){
		global->to3AC(prog);
	}
	return prog;
}

void FnDeclNode::to3AC(IRProgram * prog){
  // Create new procedure object and add to program
  Procedure * proc = prog->makeProc(ID()->getName());

  // Process formal parameters
  for (auto formal : *myFormals) {
    formal->to3AC(proc);
  }

  // Add getarg quads for each formal parameter
  size_t i = 1;
  for (SymOpd * opd : proc->getFormals()) {
    proc->addQuad(new GetArgQuad(i, opd));
    i++;
  }

  // Process function body statements
  for (auto stmt : *myBody) {
    stmt->to3AC(proc);
  }
}

void FnDeclNode::to3AC(Procedure * proc){
	//This never needs to be implemented,
	// the function only exists because of
	// inheritance needs (A function declaration
	// never occurs within another function)
	throw new InternalError("FnDecl at a local scope");
}

void FormalDeclNode::to3AC(IRProgram * prog){
	//This never needs to be implemented,
	// the function only exists because of
	// inheritance needs (A formal never
	// occurs at global scope)
	throw new InternalError("Formal at a global scope");
}

void FormalDeclNode::to3AC(Procedure * proc){
  // Add formal parameter to procedure's formals list
  proc->gatherFormal(ID()->getSymbol());
}

Opd * IntLitNode::flatten(Procedure * proc){
	const DataType * type = proc->getProg()->nodeType(this);
	return new LitOpd(std::to_string(myNum), 8);
}

Opd * StrLitNode::flatten(Procedure * proc){
	Opd * res = proc->getProg()->makeString(myStr);
	return res;
}

Opd * TrueNode::flatten(Procedure * proc){
  return new LitOpd("1", 8);
}

Opd * FalseNode::flatten(Procedure * proc){
  return new LitOpd("0", 8);
}

Opd * EhNode::flatten(Procedure * proc){
  AuxOpd * tmp = proc->makeTmp(8);
  proc->addQuad(new MagicQuad(tmp));
  return tmp;
}

Opd * CallExpNode::flatten(Procedure * proc){
  // Process and gather all argument expressions
  std::list<Opd *> argOpds;
  for(auto arg : *myArgs) {
    argOpds.push_back(arg->flatten(proc));
  }

  // Generate SetArg quads for each argument
  size_t i = 1;
  for(auto argOpd : argOpds) {
    proc->addQuad(new SetArgQuad(i, argOpd));
    i++;
  }

  SemSymbol * fnSym = myCallee->getSymbol();
  proc->addQuad(new CallQuad(fnSym));

  const FnType * fnType = fnSym->getDataType()->asFn();
  const DataType * retType = fnType->getReturnType();

  // Handle return value if function is not void
  if(!retType->isVoid()) {
    AuxOpd * retOpd = proc->makeTmp(8);
    proc->addQuad(new GetRetQuad(retOpd));
    return retOpd;
  }

  return nullptr;
}

Opd * NegNode::flatten(Procedure * proc){
  Opd * srcOpd = myExp->flatten(proc);
  AuxOpd * dstOpd = proc->makeTmp(8);
  proc->addQuad(new UnaryOpQuad(dstOpd, NEG64, srcOpd));
  return dstOpd;
}

Opd * NotNode::flatten(Procedure * proc){
  Opd * srcOpd = myExp->flatten(proc);
  AuxOpd * dstOpd = proc->makeTmp(8);
  proc->addQuad(new UnaryOpQuad(dstOpd, NOT64, srcOpd));
  return dstOpd;
}

Opd * PlusNode::flatten(Procedure * proc){
  Opd * op2 = myExp2->flatten(proc);
  Opd * op1 = myExp1->flatten(proc);
  AuxOpd * tmp = proc->makeTmp(8);
  proc->addQuad(new BinOpQuad(tmp, ADD64, op1, op2));
  return tmp;
}

Opd * MinusNode::flatten(Procedure * proc){
  Opd * op1 = myExp1->flatten(proc);
  Opd * op2 = myExp2->flatten(proc);
  AuxOpd * tmp = proc->makeTmp(8);
  proc->addQuad(new BinOpQuad(tmp, SUB64, op1, op2));
  return tmp;
}

Opd * TimesNode::flatten(Procedure * proc){
  Opd * op1 = myExp1->flatten(proc);
  Opd * op2 = myExp2->flatten(proc);
  AuxOpd * tmp = proc->makeTmp(8);
  proc->addQuad(new BinOpQuad(tmp, MULT64, op1, op2));
  return tmp;
}

Opd * DivideNode::flatten(Procedure * proc){
  Opd * op2 = myExp2->flatten(proc);
  Opd * op1 = myExp1->flatten(proc);
  AuxOpd * tmp = proc->makeTmp(8);
  proc->addQuad(new BinOpQuad(tmp, DIV64, op1, op2));
  return tmp;
}

Opd * AndNode::flatten(Procedure * proc){
  Opd * op1 = myExp1->flatten(proc);
  Opd * op2 = myExp2->flatten(proc);
  AuxOpd * tmp = proc->makeTmp(8);
  proc->addQuad(new BinOpQuad(tmp, AND64, op1, op2));
  return tmp;
}

Opd * OrNode::flatten(Procedure * proc){
  Opd * op1 = myExp1->flatten(proc);
  Opd * op2 = myExp2->flatten(proc);
  AuxOpd * tmp = proc->makeTmp(8);
  proc->addQuad(new BinOpQuad(tmp, OR64, op1, op2));
  return tmp;
}

Opd * EqualsNode::flatten(Procedure * proc){
  Opd * op1 = myExp1->flatten(proc);
  Opd * op2 = myExp2->flatten(proc);
  AuxOpd * tmp = proc->makeTmp(8);
  proc->addQuad(new BinOpQuad(tmp, EQ64, op1, op2));
  return tmp;
}

Opd * NotEqualsNode::flatten(Procedure * proc){
  Opd * op1 = myExp1->flatten(proc);
  Opd * op2 = myExp2->flatten(proc);
  AuxOpd * tmp = proc->makeTmp(8);
  proc->addQuad(new BinOpQuad(tmp, NEQ64, op1, op2));
  return tmp;
}

Opd * LessNode::flatten(Procedure * proc){
  Opd * op1 = myExp1->flatten(proc);
  Opd * op2 = myExp2->flatten(proc);
  AuxOpd * tmp = proc->makeTmp(8);
  proc->addQuad(new BinOpQuad(tmp, LT64, op1, op2));
  return tmp;
}

Opd * GreaterNode::flatten(Procedure * proc){
  Opd * op1 = myExp1->flatten(proc);
  Opd * op2 = myExp2->flatten(proc);
  AuxOpd * tmp = proc->makeTmp(8);
  proc->addQuad(new BinOpQuad(tmp, GT64, op1, op2));
  return tmp;
}

Opd * LessEqNode::flatten(Procedure * proc){
  Opd * op1 = myExp1->flatten(proc);
  Opd * op2 = myExp2->flatten(proc);
  AuxOpd * tmp = proc->makeTmp(8);
  proc->addQuad(new BinOpQuad(tmp, LTE64, op1, op2));
  return tmp;
}

Opd * GreaterEqNode::flatten(Procedure * proc){
  Opd * op1 = myExp1->flatten(proc);
  Opd * op2 = myExp2->flatten(proc);
  AuxOpd * tmp = proc->makeTmp(8);
  proc->addQuad(new BinOpQuad(tmp, GTE64, op1, op2));
  return tmp;
}

void AssignStmtNode::to3AC(Procedure * proc){
  Opd * dstOpd = myDst->flatten(proc);
  Opd * srcOpd = mySrc->flatten(proc);
  proc->addQuad(new AssignQuad(dstOpd, srcOpd));
}

void MaybeStmtNode::to3AC(Procedure * proc){
    Label * elseLbl = proc->makeLabel();
    Label * endLbl = proc->makeLabel();
    
    // Generate EhNode-like behavior to decide which branch to take
    AuxOpd * condTmp = proc->makeTmp(8);
    proc->addQuad(new MagicQuad(condTmp));  // Random true/false
    proc->addQuad(new IfzQuad(condTmp, elseLbl));
    
    // True branch - assign mySrc1 to myDst
    Opd * dstOpd = myDst->flatten(proc);
    Opd * src1Opd = mySrc1->flatten(proc);
    proc->addQuad(new AssignQuad(dstOpd, src1Opd));
    proc->addQuad(new GotoQuad(endLbl));
    
    // False branch - assign mySrc2 to myDst
    Quad * elseQuad = new NopQuad();
    elseQuad->addLabel(elseLbl);
    proc->addQuad(elseQuad);
    
    Opd * src2Opd = mySrc2->flatten(proc);
    proc->addQuad(new AssignQuad(dstOpd, src2Opd));
    
    // End label
    Quad * endQuad = new NopQuad();
    endQuad->addLabel(endLbl);
    proc->addQuad(endQuad);
}

void PostIncStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void PostDecStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void ToConsoleStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void FromConsoleStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void IfStmtNode::to3AC(Procedure * proc){
    // Generate condition code
    Opd * condOpd = myCond->flatten(proc);
    
    // Create exit label
    Label * exitLbl = proc->makeLabel();
    
    // If condition is false, jump to exit label
    proc->addQuad(new IfzQuad(condOpd, exitLbl));
    
    // Generate code for if body
    for (auto stmt : *myBody) {
        stmt->to3AC(proc);
    }
    
    // Add nop with exit label
    Quad * exitQuad = new NopQuad();
    exitQuad->addLabel(exitLbl);
    proc->addQuad(exitQuad);
}

void IfElseStmtNode::to3AC(Procedure * proc){
    // Generate condition code
    Opd * condOpd = myCond->flatten(proc);
    
    // Create labels for else branch and exit
    Label * elseLbl = proc->makeLabel();
    Label * exitLbl = proc->makeLabel();
    
    // If condition is false, jump to else label
    proc->addQuad(new IfzQuad(condOpd, elseLbl));
    
    // Generate code for if body
    for (auto stmt : *myBodyTrue) {
        stmt->to3AC(proc);
    }
    
    // Jump to exit after true branch
    proc->addQuad(new GotoQuad(exitLbl));
    
    // Add else label
    Quad * elseQuad = new NopQuad();
    elseQuad->addLabel(elseLbl);
    proc->addQuad(elseQuad);
    
    // Generate code for else body
    for (auto stmt : *myBodyFalse) {
        stmt->to3AC(proc);
    }
    
    // Add exit label
    Quad * exitQuad = new NopQuad();
    exitQuad->addLabel(exitLbl);
    proc->addQuad(exitQuad);
}

void WhileStmtNode::to3AC(Procedure * proc){
    // Create labels for loop head and exit
    Label * headLbl = proc->makeLabel();
    Label * exitLbl = proc->makeLabel();
    
    // Add loop head label
    Quad * headQuad = new NopQuad();
    headQuad->addLabel(headLbl);
    proc->addQuad(headQuad);
    
    // Generate condition code
    Opd * condOpd = myCond->flatten(proc);
    
    // If condition is false, jump to exit label
    proc->addQuad(new IfzQuad(condOpd, exitLbl));
    
    // Generate code for loop body
    for (auto stmt : *myBody) {
        stmt->to3AC(proc);
    }
    
    // Jump back to loop head
    proc->addQuad(new GotoQuad(headLbl));
    
    // Add exit label
    Quad * exitQuad = new NopQuad();
    exitQuad->addLabel(exitLbl);
    proc->addQuad(exitQuad);
}

void CallStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void ReturnStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void VarDeclNode::to3AC(Procedure * proc){
	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	proc->gatherLocal(sym);
}

void VarDeclNode::to3AC(IRProgram * prog){
	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	prog->gatherGlobal(sym);
}

//We only get to this node if we are in a stmt
// context (DeclNodes protect descent)
Opd * IDNode::flatten(Procedure * proc){
	TODO(Implement me)
}

}
