* TINY Compilation to TM Code
* Standard prelude:
  0:     LD  6,0(0) 	load maxaddress from location 0
  1:     LD  2,0(0) 	load maxaddress from location 0
  2:     ST  0,0(0) 	clear location 0
* End of standard prelude.
* -> Init Function (funOne)
* -> declare var
* <- declare var
* -> declare var
* <- declare var
* -> assign
* -> Op
* <- Op
* TESTANDO NOME ESCOPO: global
  3:     ST  0,-3(2) 	assign: store value
* <- assign
* -> assign
* -> Op
* <- Op
* TESTANDO NOME ESCOPO: global
  4:     ST  0,-2(2) 	assign: store value
* <- assign
* <- End Function
* End of execution.
  5:   HALT  0,0,0 	
