#include "clang/AST/StmtNvm.h"
#include "clang/AST/ASTContext.h"


using namespace clang;

NvmTxStmt::NvmTxStmt(Stmt *stmt) :Stmt(NvmTxStmtClass), AssociatedStmt(stmt){}

NvmTxStmt*
NvmTxStmt::Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc, Stmt *AssociatedStmt) {

  unsigned Size =
      llvm::alignTo(sizeof(NvmTxStmt), alignof(Stmt *));
  void *Mem =
      C.Allocate(Size + sizeof(Stmt *));
  return new (Mem) NvmTxStmt(AssociatedStmt);
}

bool NvmTxStmt::classof(const Stmt *T) {
  return T->getStmtClass() == NvmTxStmtClass;
}
