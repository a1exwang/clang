#include "clang/AST/StmtNvm.h"
#include "CodeGenFunction.h"


using namespace clang;
using namespace CodeGen;

void CodeGenFunction::EmitNvmTxStmt(const NvmTxStmt &S) {
  EmitStmt(S.GetAssociatedStmt());
}
