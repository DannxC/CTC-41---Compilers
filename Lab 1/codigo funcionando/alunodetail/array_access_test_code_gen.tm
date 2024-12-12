* TINY Compilation to TM Code
* Standard prelude:
  0:     LD  6,0(0) 	load maxaddress from location 0
  1:     LD  2,0(0) 	load maxaddress from location 0
  2:     ST  0,0(0) 	clear location 0
* End of standard prelude.
* -> Init Function (main)
  3:    LDA  7,0(7) 	
* -> declare vector
  4:    LDA  0,-2(2) 	guard addr of vector
  5:     ST  0,-2(2) 	store addr of vector
* <- declare vector
* -> declare vector
  6:    LDA  0,-13(2) 	guard addr of vector
  7:     ST  0,-13(2) 	store addr of vector
* <- declare vector
* -> declare vector
  8:    LDA  0,-24(2) 	guard addr of vector
  9:     ST  0,-24(2) 	store addr of vector
* <- declare vector
* -> declare var
* <- declare var
* -> assign
* -> Const
 10:    LDC  0,0(0) 	load const
* <- Const
* TESTANDO NOME ESCOPO: global
 11:     ST  0,-35(2) 	assign: store value
* <- assign
* -> while
* <- while
* -> while
* <- while
* <- End Function
* End of execution.
 12:   HALT  0,0,0 	
