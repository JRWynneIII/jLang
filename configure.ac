AC_INIT(configure)
echo "Testing for a C compiler"
AC_PROG_CC
echo "Testing for a C++ compiler"
LIBS=`llvm-config --cppflags --ldflags --libs core mcjit native bitwriter --system-libs`
LIBS=$(echo $LIBS | tr -d '\n' )
CXXFLAGS="-g -std=c++11"
CPPFLAGS="$CXXFLAGS $FLAGS"
AC_PROG_CXX
AC_LANG([C++])
AC_CHECK_HEADERS(iostream)
AC_CHECK_HEADERS(fstream)

LLVM_PASSES=1
AC_CHECK_HEADERS(llvm/Analysis/Passes.h, [], [LLVM_PASSES=0])
if test $LLVM_PASSES == 0 ; then
	AC_MSG_ERROR([Missing llvm/Analysis/Passes.h header file! Make sure you have LLVM 3.6 installed!])
fi

LLVM_READERWRITER=1
AC_CHECK_HEADERS(llvm/Bitcode/ReaderWriter.h, [], [LLVM_READERWRITER=0])
if test $LLVM_READERWRITER == 0 ; then
	AC_MSG_ERROR([Missing llvm/Bitcode/ReaderWriter.h header file! Make sure you have LLVM 3.6 installed!])
fi

LLVM_DATALAYOUT=1
AC_CHECK_HEADERS(llvm/IR/DataLayout.h, [], [LLVM_DATALAYOUT=0])
if test $LLVM_DATALAYOUT == 0 ; then
	AC_MSG_ERROR([Missing llvm/IR/DataLayout.h header file! Make sure you have LLVM 3.6 installed!])
fi

LLVM_RAW_OSTREAM=1
AC_CHECK_HEADERS(llvm/Support/raw_ostream.h, [], [LLVM_RAW_OSTREAM=0])
if test $LLVM_RAW_OSTREAM == 0 ; then
	AC_MSG_ERROR([Missing llvm/Support/raw_ostream.h header file! Make sure you have LLVM 3.6 installed!])
fi

LLVM_EXECUTIONENGINE=1
AC_CHECK_HEADERS(llvm/ExecutionEngine/ExecutionEngine.h, [], [LLVM_EXECUTIONENGINE=0])
if test $LLVM_EXECUTIONENGINE == 0 ; then
	AC_MSG_ERROR([Missing llvm/ExecutionEngine/ExecutionEngine.h header file! Make sure you have LLVM 3.6 installed!])
fi

LLVM_DERIVEDTYPES=1
AC_CHECK_HEADERS(llvm/IR/DerivedTypes.h, [], [LLVM_DERIVEDTYPES=0])
if test $LLVM_DERIVEDTYPES == 0 ; then
	AC_MSG_ERROR([Missing llvm/IR/DerivedTypes.h header file! Make sure you have LLVM 3.6 installed!])
fi

LLVM_IRBUILDER=1
AC_CHECK_HEADERS(llvm/IR/IRBuilder.h, [], [LLVM_IRBUILDER=0])
if test $LLVM_IRBUILDER == 0 ; then
	AC_MSG_ERROR([Missing llvm/IR/IRBuilder.h header file! Make sure you have LLVM 3.6 installed!])
fi

LLVM_LLVMCONTEXT=1
AC_CHECK_HEADERS(llvm/IR/LLVMContext.h, [], [LLVM_LLVMCONTEXT=0])
if test $LLVM_LLVMCONTEXT == 0 ; then
	AC_MSG_ERROR([Missing llvm/IR/LLVMContext.h header file! Make sure you have LLVM 3.6 installed!])
fi

LLVM_MODULE=1
AC_CHECK_HEADERS(llvm/IR/Module.h, [], [LLVM_MODULE=0])
if test $LLVM_MODULE == 0 ; then
	AC_MSG_ERROR([Missing llvm/IR/Module.h header file! Make sure you have LLVM 3.6 installed!])
fi

LLVM_PASSMANAGER=1
AC_CHECK_HEADERS(llvm/IR/PassManager.h, [], [LLVM_PASSMANAGER=0])
if test $LLVM_PASSMANAGER == 0 ; then
	AC_MSG_ERROR([Missing llvm/IR/PassManager.h header file! Make sure you have LLVM 3.6 installed!])
fi

LLVM_TARGETSELECT=1
AC_CHECK_HEADERS(llvm/Support/TargetSelect.h, [], [LLVM_TARGETSELECT=0])
if test $LLVM_TARGETSELECT == 0 ; then
	AC_MSG_ERROR([Missing llvm/Support/TargetSelect.h header file! Make sure you have LLVM 3.6 installed!])
fi

LLVM_SCALAR=1
AC_CHECK_HEADERS(llvm/Transforms/Scalar.h, [], [LLVM_SCALAR=0])
if test $LLVM_SCALAR == 0 ; then
	AC_MSG_ERROR([Missing llvm/Transforms/Scalar.h header file! Make sure you have LLVM 3.6 installed!])
fi

LLVM_STRINGREF=1
AC_CHECK_HEADERS(llvm/ADT/StringRef.h, [], [LLVM_STRINGREF=0])
if test $LLVM_STRINGREF == 0 ; then
	AC_MSG_ERROR([Missing llvm/ADT/StringRef.h header file! Make sure you have LLVM 3.6 installed!])
fi

LLVM_INSTRUCTION=1
AC_CHECK_HEADERS(llvm/IR/Instruction.h, [], [LLVM_INSTRUCTION=0])
if test $LLVM_INSTRUCTION == 0 ; then
	AC_MSG_ERROR([Missing llvm/IR/Instruction.h header file! Make sure you have LLVM 3.6 installed!])
fi

LLVM_USER=1
AC_CHECK_HEADERS(llvm/IR/User.h, [], [LLVM_USER=0])
if test $LLVM_USER == 0 ; then
	AC_MSG_ERROR([Missing llvm/IR/User.h header file! Make sure you have LLVM 3.6 installed!])
fi

AC_CHECK_PROGS(YACC, 'bison' byacc yacc, [])
AC_CHECK_PROGS(LEX, flex lex, [])

YACCLEX="yes"
if test -z "$YACC" || test -z "$LEX"; then
	YACCLEX="no"
	AC_MSG_ERROR(You need bison 3.0+ and flex!)
fi

LIBEDIT=1
AC_CHECK_LIB(edit,strlcpy,[],[LIBEDIT=0])
if test $LIBEDIT == 0 ; then
	AC_MSG_ERROR([libedit is required! (On Ubuntu, sudo apt-get install libedit-dev)])
fi

LIBMATH=1
AC_CHECK_LIB(m,cos,[],[LIBMATH=0])
if test $LIBMATH == 0 ; then
	AC_MSG_ERROR([libmath is required!])
fi

PTHREADS=1
AC_CHECK_LIB(pthread,pthread_create,[],[PTHREADS=0])
if test $PTHREADS == 0 ; then
	AC_MSG_ERROR([pthread library is required!])
fi
AC_OUTPUT(Makefile)
