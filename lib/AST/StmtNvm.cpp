#include "clang/AST/StmtNvm.h"
#include "clang/AST/ASTContext.h"


using namespace clang;

NvmTxStmt::NvmTxStmt(
    Stmt *AssociatedStmt,
    SourceLocation StartLoc,
    SourceLocation EndLoc,
    Expr *Pool,
    Expr *Tx
) :Stmt(NvmTxStmtClass), AssociatedStmt(AssociatedStmt), StartLoc(StartLoc), EndLoc(EndLoc), Pool(Pool), Tx(Tx) {}

NvmTxStmt*
NvmTxStmt::Create(
    const ASTContext &C,
    Stmt *AssociatedStmt,
    SourceLocation StartLoc,
    SourceLocation EndLoc,
    Expr *Pool,
    Expr *Tx
) {

  uint64_t Size = llvm::alignTo(sizeof(NvmTxStmt), alignof(Stmt *));
  void *Mem = C.Allocate(Size + sizeof(Stmt *));
  return new (Mem) NvmTxStmt(AssociatedStmt, StartLoc, EndLoc, Pool, Tx);
}

bool NvmTxStmt::classof(const Stmt *T) {
  return T->getStmtClass() == NvmTxStmtClass;
}
