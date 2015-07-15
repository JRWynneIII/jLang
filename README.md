jLang Compiler
========

[![Join the chat at https://gitter.im/JRWynneIII/jLang](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/JRWynneIII/jLang?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
jLang is a compiled programming language that aims to have built in acceleration and parallelization structures. The grammar was designed to be simple, easily readable, and easy to transision to from other languages. jLang borrows syntax and constructs from various different languages to increase this ease of transition, including C, Fortran, Rust, D, Go, and Python. There is no line terminating character in this language, so it can be written all in one line like C but without the need for useless semicolons. jLang is designed to be fully compatable with all C types and codes/functions. An `extern` keyword is provided to give access to external C functions.
Heres some example code:
```
import stdio;

kernel newKern() <- (1,10) {            #Launches a PTX kernel of 1 block of 10 threads
  println("Hello from %d", id.thread)
}                                       #Kernels do NOT return values!

func newFunc() -> int {                 #Functions MUST returns something. Even if its just an int
  newKern()
  1                                     #Implicitly returns what the last line evaluates to
}

func main() -> int {
  string hello = "hello world!"
  for i, 10 {                           #For loop structure is similar to Fortran's do loop
    println(hello)
  }
  int a = newFunc()
  if !a
    0                                     #Implicitly returns 0
  else
    1                                    
}

```
This is an ongoing and very new/alpha project so many features are either buggy or do NOT work! Feel free to open issues or pull requests!

The currently supported datatypes are `int`,`char`, `string` (which is an array of `char`s), and `double`.  jLang is built on LLVM 3.4. 

Dependencies for jLang include:
* LLVM 3.4 and up (Tested as working on 3.4 and 3.7)
* Boost
* clang++ with C++11 support
