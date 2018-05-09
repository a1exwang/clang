#include "clang/Sema/SemaDiagnostic.h"
#include "clang/Sema/Lookup.h"
#include "clang/Sema/Sema.h"

#include <iostream>
#include "clang/AST/StmtNvm.h"

using namespace clang;
using namespace sema;


StmtResult Sema::ActOnPragmaNvm(StmtResult &Result, Expr *Pool, Expr *Tx, SourceLocation *loc) {
  std::cerr << "SemaNvm.cpp: ActOnPragmaNvm(), stmts: " << std::endl;
  return NvmTxStmt::Create(
      Context,
      Result.get(),
      *loc,
      *loc,
      Pool,
      Tx
  );
}

ExprResult Sema::ActOnPragmaNvmIdExpression(Scope *CurScope,
                                            CXXScopeSpec &ScopeSpec,
                                            const DeclarationNameInfo &Id) {
  LookupResult Lookup(*this, Id, LookupOrdinaryName);
  LookupParsedName(Lookup, CurScope, &ScopeSpec, true);

  if (Lookup.isAmbiguous())
    return ExprError();

  VarDecl *VD;
  if (!Lookup.isSingleResult()) {
    Diag(Id.getLoc(), Lookup.empty() ? diag::err_undeclared_var_use
                                     : diag::err_omp_expected_var_arg)
        << Id.getName();
    return ExprError();
  } else {
    if (!(VD = Lookup.getAsSingle<VarDecl>())) {
      Diag(Id.getLoc(), diag::err_omp_expected_var_arg) << Id.getName();
      Diag(Lookup.getFoundDecl()->getLocation(), diag::note_declared_at);
      return ExprError();
    }
  }
  Lookup.suppressDiagnostics();

//  VarDecl *CanonicalVD = VD->getCanonicalDecl();
//  NamedDecl *ND = cast<NamedDecl>(CanonicalVD);
//  // OpenMP [2.9.2, Restrictions, C/C++, p.2]
//  //   A threadprivate directive for file-scope variables must appear outside
//  //   any definition or declaration.
//  if (CanonicalVD->getDeclContext()->isTranslationUnit() &&
//      !getCurLexicalContext()->isTranslationUnit()) {
//    Diag(Id.getLoc(), diag::err_omp_var_scope)
//        << getOpenMPDirectiveName(OMPD_threadprivate) << VD;
//    bool IsDecl =
//        VD->isThisDeclarationADefinition(Context) == VarDecl::DeclarationOnly;
//    Diag(VD->getLocation(),
//         IsDecl ? diag::note_previous_decl : diag::note_defined_here)
//        << VD;
//    return ExprError();
//  }
  // OpenMP [2.9.2, Restrictions, C/C++, p.3]
  //   A threadprivate directive for static class member variables must appear
  //   in the class definition, in the same scope in which the member
  //   variables are declared.
//  if (CanonicalVD->isStaticDataMember() &&
//      !CanonicalVD->getDeclContext()->Equals(getCurLexicalContext())) {
//    Diag(Id.getLoc(), diag::err_omp_var_scope)
//        << getOpenMPDirectiveName(OMPD_threadprivate) << VD;
//    bool IsDecl =
//        VD->isThisDeclarationADefinition(Context) == VarDecl::DeclarationOnly;
//    Diag(VD->getLocation(),
//         IsDecl ? diag::note_previous_decl : diag::note_defined_here)
//        << VD;
//    return ExprError();
//  }
  // OpenMP [2.9.2, Restrictions, C/C++, p.4]
  //   A threadprivate directive for namespace-scope variables must appear
  //   outside any definition or declaration other than the namespace
  //   definition itself.
//  if (CanonicalVD->getDeclContext()->isNamespace() &&
//      (!getCurLexicalContext()->isFileContext() ||
//       !getCurLexicalContext()->Encloses(CanonicalVD->getDeclContext()))) {
//    Diag(Id.getLoc(), diag::err_omp_var_scope)
//        << getOpenMPDirectiveName(OMPD_threadprivate) << VD;
//    bool IsDecl =
//        VD->isThisDeclarationADefinition(Context) == VarDecl::DeclarationOnly;
//    Diag(VD->getLocation(),
//         IsDecl ? diag::note_previous_decl : diag::note_defined_here)
//        << VD;
//    return ExprError();
//  }
  // OpenMP [2.9.2, Restrictions, C/C++, p.6]
  //   A threadprivate directive for static block-scope variables must appear
  //   in the scope of the variable and not in a nested scope.
//  if (CanonicalVD->isStaticLocal() && CurScope &&
//      !isDeclInScope(ND, getCurLexicalContext(), CurScope)) {
//    Diag(Id.getLoc(), diag::err_omp_var_scope)
//        << getOpenMPDirectiveName(OMPD_threadprivate) << VD;
//    bool IsDecl =
//        VD->isThisDeclarationADefinition(Context) == VarDecl::DeclarationOnly;
//    Diag(VD->getLocation(),
//         IsDecl ? diag::note_previous_decl : diag::note_defined_here)
//        << VD;
//    return ExprError();
//  }

  VD->setReferenced(true);
  VD->markUsed(Context);
  QualType ExprType = VD->getType().getNonReferenceType();
  return DeclRefExpr::Create(Context, NestedNameSpecifierLoc(),
                             SourceLocation(), VD,
      /*RefersToEnclosingVariableOrCapture=*/false,
                             Id.getLoc(), ExprType, VK_LValue);
}

StmtResult Sema::ActOnPragmaNvmPtrDecl(StmtResult &Result, SourceLocation *StartLoc, SourceLocation *EndLoc) {
  auto declStmt = cast<DeclStmt>(Result.get());
  std::cerr << "ActOnPragmaNvmPtrDecl" << std::endl;
  for (auto &decl : declStmt->decls()) {
    auto varDecl = cast<VarDecl>(decl);
    varDecl->addAttr(NvmPtrDeclAttr::CreateImplicit(Context, SourceRange(*StartLoc, *EndLoc)));
    decl->dump();
  }
  std::cerr << "ActOnPragmaNvmPtrDecl end" << std::endl;
  return Result;
}
