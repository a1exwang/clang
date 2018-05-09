#include "clang/AST/StmtNvm.h"
#include "CodeGenFunction.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include <iostream>


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
        llvm::PointerType::get(llvm::Type::getInt8PtrTy(mod->getContext()), 0),
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

void CodeGenFunction::EmitNvmTxAdd(Address Addr, bool isVolatile) {
  llvm::LLVMContext &C = getLLVMContext();
  std::string s;
  llvm::raw_string_ostream ss(s);
  Addr.getPointer()->print(ss);

  std::cout << "EmitNvmTxAdd(): \n  Addr: " << s << std::endl;
  auto &curTx = GetCurNvmTx();
  auto FnNvmAdd = curTx.GetFnNvmAdd(&CGM.getModule());

  auto ConstSize = CGM.getModule().getDataLayout().getTypeAllocSize(Addr.getElementType());
  llvm::Value* Params[] = {
      Builder.CreatePointerCast(curTx.NvmPool, llvm::Type::getInt8PtrTy(C, 0)),
      Builder.CreatePointerCast(curTx.NvmTx, llvm::Type::getInt8PtrTy(C, 0)),
      Builder.CreatePointerCast(Addr.getPointer(), llvm::PointerType::get(llvm::Type::getInt8PtrTy(C, 0), 0)),
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(C), ConstSize)
  };
  // TODO: fix this call
  Builder.CreateCall(FnNvmAdd, Params);
}
