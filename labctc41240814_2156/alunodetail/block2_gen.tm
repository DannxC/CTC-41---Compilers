* TINY Compilation to TM Code
* Standard prelude:
  0:     LD  6,0(0) 	load maxaddress from location 0
  1:     LD  2,0(0) 	load maxaddress from location 0
  2:     ST  0,0(0) 	clear location 0
* End of standard prelude.
* -> Init Function (teste)
  4:     ST  0,-1(2) 	store return address from ac
* -> Param
* <- Param
* -> Param
* <- Param
* -> Function Call (output)
* -> Id
  5:     LD  0,-3(2) 	load id value
* <- Id
  6:    OUT  0,0,0 	print value
* -> Function Call (output)
* -> Id
  7:     LD  0,-2(2) 	load id value
* <- Id
  8:    OUT  0,0,0 	print value
* -> declare var
* <- declare var
* -> declare var
* <- declare var
* -> assign
* -> Const
  9:    LDC  0,3(0) 	load const
* <- Const
 10:     ST  0,-4(2) 	assign: store value
* <- assign
* -> assign
* -> Const
 11:    LDC  0,4(0) 	load const
* <- Const
 12:     ST  0,-5(2) 	assign: store value
* <- assign
* -> Function Call (output)
* -> Id
 13:     LD  0,-4(2) 	load id value
* <- Id
 14:    OUT  0,0,0 	print value
* -> Function Call (output)
* -> Id
 15:     LD  0,-5(2) 	load id value
* <- Id
 16:    OUT  0,0,0 	print value
* -> assign
* -> Const
 17:    LDC  0,5(0) 	load const
* <- Const
 18:     ST  0,-3(2) 	assign: store value
* <- assign
* -> Function Call (output)
* -> Id
 19:     LD  0,-3(2) 	load id value
* <- Id
 20:    OUT  0,0,0 	print value
 21:    LDA  1,0(2) 	save current fp into ac1
 22:     LD  2,0(2) 	make fp = ofp
 23:     LD  7,-1(1) 	return to caller
* <- End Function
* -> Init Function (main)
  3:    LDA  7,20(7) 	jump to main
* -> declare var
* <- declare var
* -> declare var
* <- declare var
* -> assign
* -> Const
 24:    LDC  0,1(0) 	load const
* <- Const
 25:     ST  0,-2(2) 	assign: store value
* <- assign
* -> assign
* -> Const
 26:    LDC  0,2(0) 	load const
* <- Const
 27:     ST  0,-3(2) 	assign: store value
* <- assign
* -> Function Call (teste)
 28:     ST  2,-4(2) 	Guard fp
* -> Id
 29:     LD  0,-2(2) 	load id value
* <- Id
 30:     ST  0,-6(2) 	Store value of func argument
* -> Id
 31:     LD  0,-3(2) 	load id value
* <- Id
 32:     ST  0,-7(2) 	Store value of func argument
 33:    LDA  2,-4(2) 	change fp
 34:    LDC  0,36(0) 	Guard return adress
 35:    LDA  7,-32(7) 	jump to function
* <- Function Call
* -> Function Call (output)
* -> Id
 36:     LD  0,-2(2) 	load id value
* <- Id
 37:    OUT  0,0,0 	print value
* -> Function Call (output)
* -> Id
 38:     LD  0,-3(2) 	load id value
* <- Id
 39:    OUT  0,0,0 	print value
* <- End Function
* End of execution.
 40:   HALT  0,0,0 	
