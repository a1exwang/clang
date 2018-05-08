//===- StmtOpenMP.h - Classes for OpenMP directives  ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
/// \brief This file defines OpenMP AST classes for executable directives and
/// clauses.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_AST_STMTNVM_H
#define LLVM_CLANG_AST_STMTNVM_H

#include "clang/AST/Expr.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceLocation.h"

namespace clang {

//===----------------------------------------------------------------------===//
// AST classes for NVM.
//===----------------------------------------------------------------------===//

/// \brief This is a basic class for representing single OpenMP executable
/// directive.
///
  class NvmTxStmt: public Stmt {
  protected:
    NvmTxStmt(
        Stmt *AssociatedStmt,
        SourceLocation StartLoc,
        SourceLocation EndLoc,
        Expr *Pool,
        Expr *Tx
    );
  public:
    static NvmTxStmt* Create(
        const ASTContext &C,
        Stmt *AssociatedStmt,
        SourceLocation StartLoc,
        SourceLocation EndLoc,
        Expr *Pool,
        Expr *Tx
    );

    child_range children() {
      return child_range(&AssociatedStmt, &AssociatedStmt+1);
    }

    static bool classof(const Stmt *T);

    SourceLocation getLocStart() const { return SourceLocation(); }
    /// \brief Returns ending location of directive.
    SourceLocation getLocEnd() const { return SourceLocation(); }
    Stmt *GetAssociatedStmt() const { return AssociatedStmt; }
    Expr *GetPool() const { return Pool; }
    Expr *GetTx() const { return Tx; }

    void SetAssociatedStmt(Stmt *stmt) { AssociatedStmt = stmt; }
  private:
    Stmt *AssociatedStmt;
    SourceLocation StartLoc;
    SourceLocation EndLoc;
    Expr *Pool;
    Expr *Tx;
  };

} // end namespace clang

#endif
