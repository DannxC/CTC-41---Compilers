* TINY Compilation to TM Code
* Standard prelude:
  0:     LD  6,0(0) 	load maxaddress from location 0
  1:     LD  2,0(0) 	load maxaddress from location 0
  2:     ST  0,0(0) 	clear location 0
* End of standard prelude.
* -> Init Function (teste)
* -> Param
* <- Param
* -> Param
* <- Param
* -> Call
* <- Call
* -> Call
* <- Call
* -> declare var
* <- declare var
* -> declare var
* <- declare var
* -> assign
* -> Const
  3:    LDC  0,3(0) 	load const
* <- Const
* TESTANDO NOME ESCOPO: teste
  4:     ST  0,-4(2) 	assign: store value
* <- assign
* -> assign
* -> Const
  5:    LDC  0,4(0) 	load const
* <- Const
* TESTANDO NOME ESCOPO: teste
  6:     ST  0,-5(2) 	assign: store value
* <- assign
* -> Call
* <- Call
* -> Call
* <- Call
* -> assign
* -> Const
  7:    LDC  0,5(0) 	load const
* <- Const
* TESTANDO NOME ESCOPO: global
  8:     ST  0,-3(2) 	assign: store value
* <- assign
* -> Call
* <- Call
* <- End Function
* End of execution.
  9:   HALT  0,0,0 	
