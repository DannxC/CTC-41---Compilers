* TINY Compilation to TM Code
* Standard prelude:
  0:     LD  6,0(0) 	load maxaddress from location 0
  1:     LD  2,0(0) 	load maxaddress from location 0
  2:     ST  0,0(0) 	clear location 0
* End of standard prelude.
* -> Init Function (binario)
  4:     ST  0,-1(2) 	store return address from ac
* -> Param
* <- Param
* -> if
* -> Op
* -> Id
  5:     LD  0,-2(2) 	load id value
* <- Id
  6:     ST  0,-3(2) 	op: push left
* -> Const
  7:    LDC  0,2(0) 	load const
* <- Const
  8:     LD  1,-3(2) 	op: load left
  9:    SUB  0,1,0 	op <
 10:    JLT  0,2(7) 	br if true
 11:    LDC  0,0(0) 	false case
 12:    LDA  7,1(7) 	unconditional jmp
 13:    LDC  0,1(0) 	true case
* <- Op
* if: jump to else belongs here
* -> Function Call (output)
* -> Id
 15:     LD  0,-2(2) 	load id value
* <- Id
 16:    OUT  0,0,0 	print value
* if: jump to end belongs here
 14:    JEQ  0,3(7) 	if: jmp to else
* -> Function Call (binario)
 18:     ST  2,-3(2) 	Guard fp
* -> Op
* -> Id
 19:     LD  0,-2(2) 	load id value
* <- Id
 20:     ST  0,-5(2) 	op: push left
* -> Const
 21:    LDC  0,2(0) 	load const
* <- Const
 22:     LD  1,-5(2) 	op: load left
 23:    DIV  0,1,0 	op /
* <- Op
 24:     ST  0,-5(2) 	Store value of func argument
 25:    LDA  2,-3(2) 	change fp
 26:    LDC  0,28(0) 	Guard return adress
 27:    LDA  7,-24(7) 	jump to function
* <- Function Call
* -> Function Call (output)
* -> Op
* -> Id
 28:     LD  0,-2(2) 	load id value
* <- Id
 29:     ST  0,-3(2) 	op: push left
* -> Op
* -> Const
 30:    LDC  0,2(0) 	load const
* <- Const
 31:     ST  0,-4(2) 	op: push left
* -> Op
* -> Id
 32:     LD  0,-2(2) 	load id value
* <- Id
 33:     ST  0,-5(2) 	op: push left
* -> Const
 34:    LDC  0,2(0) 	load const
* <- Const
 35:     LD  1,-5(2) 	op: load left
 36:    DIV  0,1,0 	op /
* <- Op
 37:     LD  1,-4(2) 	op: load left
 38:    MUL  0,1,0 	op *
* <- Op
 39:     LD  1,-3(2) 	op: load left
 40:    SUB  0,1,0 	op -
* <- Op
 41:    OUT  0,0,0 	print value
 17:    LDA  7,24(7) 	jmp to end
* <- if
 42:    LDA  1,0(2) 	save current fp into ac1
 43:     LD  2,0(2) 	make fp = ofp
 44:     LD  7,-1(1) 	return to caller
* <- End Function
* -> Init Function (main)
  3:    LDA  7,41(7) 	jump to main
* -> declare var
* <- declare var
* -> assign
* -> Function Call (input)
 45:     IN  0,0,0 	read input
 46:     ST  0,-2(2) 	assign: store value
* <- assign
* -> if
* -> Op
* -> Id
 47:     LD  0,-2(2) 	load id value
* <- Id
 48:     ST  0,-3(2) 	op: push left
* -> Const
 49:    LDC  0,0(0) 	load const
* <- Const
 50:     LD  1,-3(2) 	op: load left
 51:    SUB  0,1,0 	op >=
 52:    JGE  0,2(7) 	br if true
 53:    LDC  0,0(0) 	false case
 54:    LDA  7,1(7) 	unconditional jmp
 55:    LDC  0,1(0) 	true case
* <- Op
* if: jump to else belongs here
* -> Function Call (binario)
 57:     ST  2,-3(2) 	Guard fp
* -> Id
 58:     LD  0,-2(2) 	load id value
* <- Id
 59:     ST  0,-5(2) 	Store value of func argument
 60:    LDA  2,-3(2) 	change fp
 61:    LDC  0,63(0) 	Guard return adress
 62:    LDA  7,-59(7) 	jump to function
* <- Function Call
* if: jump to end belongs here
 56:    JEQ  0,7(7) 	if: jmp to else
* -> Function Call (binario)
 64:     ST  2,-3(2) 	Guard fp
* -> Op
* -> Const
 65:    LDC  0,0(0) 	load const
* <- Const
 66:     ST  0,-5(2) 	op: push left
* -> Id
 67:     LD  0,-2(2) 	load id value
* <- Id
 68:     LD  1,-5(2) 	op: load left
 69:    SUB  0,1,0 	op -
* <- Op
 70:     ST  0,-5(2) 	Store value of func argument
 71:    LDA  2,-3(2) 	change fp
 72:    LDC  0,74(0) 	Guard return adress
 73:    LDA  7,-70(7) 	jump to function
* <- Function Call
 63:    LDA  7,10(7) 	jmp to end
* <- if
* <- End Function
* End of execution.
 74:   HALT  0,0,0 	
