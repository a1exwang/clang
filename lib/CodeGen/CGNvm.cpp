#include "clang/AST/StmtNvm.h"
#include "CodeGenFunction.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"


using namespace clang;
using namespace CodeGen;

void CodeGenFunction::EmitNvmTxStmt(const NvmTxStmt &S) {
  auto pool = EmitAnyExpr(S.GetPool()).getScalarVal();
  auto txPtr = EmitLValue(S.GetTx()).getPointer();

  auto &CurTx = GetCurNvmTx();

  auto txVal = Builder.CreateCall(CurTx.GetFnNvmStartTx(&CGM.getModule()), {pool});
  Builder.CreateAlignedStore(txVal, txPtr, txPtr->getPointerAlignment(CGM.getModule().getDataLayout()));

  PushNvmTx({pool, txVal});
  EmitStmt(S.GetAssociatedStmt());

  Builder.CreateCall(CurTx.GetFnNvmCommit(&CGM.getModule()), {pool, txVal});
  PopNvmTx();
}

llvm::Value *CodeGenFunction::NvmTransaction::GetFnNvmAdd(llvm::Module *mod) {
  return mod->getOrInsertFunction(
    "nvm_add", llvm::FunctionType::get(
      llvm::IntegerType::getInt32Ty(mod->getContext()),
      {
        llvm::Type::getInt8PtrTy(mod->getContext()),
        llvm::Type::getInt8PtrTy(mod->getContext()),
        llvm::Type::getInt8PtrTy(mod->getContext()),
        llvm::IntegerType::getInt64Ty(mod->getContext()),
      },
      false
    )
  );
}
llvm::Value *CodeGenFunction::NvmTransaction::GetFnNvmStartTx(llvm::Module *mod) {
  return mod->getOrInsertFunction(
      "nvm_start_tx", llvm::FunctionType::get(
          llvm::Type::getInt8PtrTy(mod->getContext()),
          {
              llvm::Type::getInt8PtrTy(mod->getContext()),
          },
          false
      )
  );
}

llvm::Value *CodeGenFunction::NvmTransaction::GetFnNvmCommit(llvm::Module *mod){
  return mod->getOrInsertFunction(
      "nvm_commit", llvm::FunctionType::get(
          llvm::IntegerType::getInt32Ty(mod->getContext()),
          {
              llvm::Type::getInt8PtrTy(mod->getContext()),
              llvm::Type::getInt8PtrTy(mod->getContext()),
          },
          false
      )
  );

}
