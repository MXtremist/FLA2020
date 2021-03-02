; This program checks if the input string is a^i*b^j*a^i*b^j while i and j are positive int
; Input: a string of a's and b's, e.g. 'abbabb'
; 该图灵机的设计思路为：将纸带1第一部分的a^i*b^j剪切到纸带2，再依次进行a的个数以及b的个数的比较
; 在剪切和比较过程中任意位置出错都将进入reject
; 状态总数如下方#Q所示，主要状态的转移为：
; cp_a不断剪切a到纸带2，在遇到b后进入cp_b，cp_b不断剪切b到纸带2，在遇到a后进入cmpa
; cmpa比较两个纸带的a串，遇到b后进入cmpb，cmpb比较两个纸带的b串

; the finite set of states
#Q = {0,cp_a,cp_b,mh,cmpa,cmpb,accept,accept2,accept3,accept4,halt_accept,reject,reject2,reject3,reject4,reject5,halt_reject}

; the finite set of input symbols
#S = {a,b}

; the complete set of tape symbols
#G = {a,b,_,t,r,u,e,f,l,s}

; the start state
#q0 = 0

; the blank symbol
#B = _

; the set of final states
#F = {halt_accept}

; the number of tapes
#N = 2

; the transition functions

; State 0: start state
0 a_ a_ ** cp_a
0 b_ b_ ** reject ; i>0
0 __ __ ** reject; empty input

; State cp_a: copy the string a^i to the 2nd tape 
cp_a a_ _a rr cp_a
cp_a b_ b_ ** cp_b
cp_a __ __ ** reject

; State cp_b: copy the string b^j to the 2nd tape 
cp_b b_ _b rr cp_b
cp_b a_ a_ *l mh
cp_b __ __ ** reject

; State mh: move 2nd head to the left
mh aa aa *l mh
mh ab ab *l mh
mh ba ba *l mh
mh bb bb *l mh
mh a_ a_ *r cmpa
mh b_ b_ *r cmpa

; State cmpa: compare two strings
cmpa aa __ rr cmpa
cmpa ab __ rr reject
cmpa a_ __ r* reject
cmpa bb bb ** cmpb
cmpa ba __ rr reject
cmpa b_ __ r* reject
cmpa _a __ *r reject
cmpa _b __ *r reject
cmpa __ __ ** reject

; State cmpa: compare two strings
cmpb aa __ rr reject
cmpb ab __ rr reject
cmpb a_ __ r* reject
cmpb bb __ rr cmpb
cmpb ba __ rr reject
cmpb b_ __ r* reject
cmpb _a __ *r reject
cmpb _b __ *r reject
cmpb __ __ ** accept

; State accept*: write 'true' on 1st tape
accept __ t_ r* accept2
accept2 __ r_ r* accept3
accept3 __ u_ r* accept4
accept4 __ e_ ** halt_accept

; State reject*: write 'false' on 1st tape
reject aa __ r* reject
reject ba __ r* reject
reject ab __ r* reject
reject bb __ r* reject
reject a_ __ r* reject
reject b_ __ r* reject
reject _a __ ** reject
reject _b __ ** reject
reject __ f_ r* reject2
reject2 __ a_ r* reject3
reject3 __ l_ r* reject4
reject4 __ s_ r* reject5
reject5 __ e_ ** halt_reject