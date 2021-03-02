; This program checks if the input string is 1^m * 1^n = 1^(m+n) while m and n are positive int
; Input: a string of Multiplication formula with 1's , e.g. '11X111=111111'
; 该图灵机的设计思路为：顺序检测纸带1，将算式的第一个乘数剪切到纸带2，之后每遇到一个1就将纸带2的结果复制到纸带3
; 这样在检测到=时，纸带3上的结果就是乘数相乘之后的结果，将其与纸带1上的目的结果比较，如果相等就accept，其中任意步骤出错就reject
; 状态总数如下方#Q所示，主要状态的转移为：
; cp拷贝第一个乘数，遇到x后进入multy，multy进行乘法的计算，计算完成后进入cmp，cmp比较算出的结果和输入的结果
; 由于进入reject时纸带2和3上指针指向有多种可能性，故额外加入状态mt2和mt3用于将相应纸带转移到末尾，从而方便转移函数的编写

; the finite set of states
#Q = {0,cp,multy,mh2,mh3,mt2,mt3,cmp,accept,accept2,accept3,accept4,halt_accept,reject,reject2,reject3,reject4,reject5,halt_reject}

; the finite set of input symbols
#S = {1,x,=}

; the complete set of tape symbols
#G = {1,x,=,_,t,r,u,e,f,a,l,s}

; the start state
#q0 = 0

; the blank symbol
#B = _

; the set of final states
#F = {halt_accept}

; the number of tapes
#N = 3

; the transition functions

; State 0: start state
0 1__ 1__ *** cp
0 =__ =__ *** reject
0 x__ x__ *** reject
0 ___ ___ *** accept ; empty input

; State cp: copy the first num to the 2nd tape
cp 1__ _1_ rr* cp
cp x__ ___ rl* mh2
cp =__ ___ r** reject
cp ___ ___ *** reject

; State mh2: move 2nd head to the left
mh2 11_ 11_ *l* mh2
mh2 x1_ x1_ *l* mh2
mh2 =1_ =1_ *l* mh2
mh2 _1_ _1_ *l* mh2
mh2 1__ 1__ *r* multy
mh2 x__ x__ *r* multy
mh2 =__ =__ *r* multy
mh2 ___ ___ *r* multy

; State multy: when meet 1, cp 2nd tape to 3rd tape
multy 11_ 111 *rr multy
multy 1__ ___ rl* mh2
multy =1_ _1_ r*l mh3
multy =__ ___ r** reject
multy x1_ _1_ r** reject
multy x__ ___ r** reject
multy _1_ _1_ *** reject
multy ___ ___ *** reject

; State mh3: move 3rd head to the left
mh3 111 111 **l mh3
mh3 x11 x11 **l mh3
mh3 =11 =11 **l mh3
mh3 _11 _11 **l mh3
mh3 11_ 11_ **r cmp
mh3 x1_ x1_ **r cmp
mh3 =1_ =1_ **r cmp
mh3 _1_ _1_ **r cmp

; State cmp: compare result with input
cmp 111 _1_ r*r cmp
cmp _1_ _1_ *** accept
cmp _11 _11 *** reject
cmp x11 _11 r** reject
cmp =11 _11 r** reject
cmp 11_ _1_ r** reject
cmp x1_ _1_ r** reject
cmp =1_ _1_ r** reject

; State accept*: write 'true' on 1st tape
accept ___ t1_ r** accept2
accept _1_ t1_ r** accept2
accept2 _1_ r1_ r** accept3
accept3 _1_ u1_ r** accept4
accept4 _1_ e1_ *** halt_accept

; State mt2 and mt3: move to the tail to make it easier
mt2 11_ 11_ *r* mt2
mt2 x1_ x1_ *r* mt2
mt2 =1_ =1_ *r* mt2
mt2 _1_ _1_ *r* mt2
mt2 111 111 *r* mt2
mt2 x11 x11 *r* mt2
mt2 =11 =11 *r* mt2
mt2 _11 _11 *r* mt2

mt2 1__ 1__ *** reject
mt2 x__ x__ *** reject
mt2 =__ =__ *** reject
mt2 ___ ___ *** reject
mt2 1_1 1_1 *** reject
mt2 x_1 x_1 *** reject
mt2 =_1 =_1 *** reject
mt2 __1 __1 *** reject

mt3 1_1 1_1 **r mt3
mt3 x_1 x_1 **r mt3
mt3 =_1 =_1 **r mt3
mt3 __1 __1 **r mt3 

mt3 1__ 1__ *** reject
mt3 x__ x__ *** reject
mt3 =__ =__ *** reject
mt3 ___ ___ *** reject

; State reject*: write 'false' on 1st tape
reject 11_ _1_ r** mt2
reject x1_ _1_ r** mt2
reject =1_ _1_ r** mt2
reject _1_ _1_ *** mt2
reject 111 _11 r** mt2
reject x11 _11 r** mt2
reject =11 _11 r** mt2
reject _11 _11 *** mt2

reject 1_1 __1 r** mt3
reject x_1 __1 r** mt3
reject =_1 __1 r** mt3
reject __1 __1 *** mt3

reject 1__ ___ r** reject
reject x__ ___ r** reject
reject =__ ___ r** reject
reject ___ f__ r** reject2
reject2 ___ a__ r** reject3
reject3 ___ l__ r** reject4
reject4 ___ s__ r** reject5
reject5 ___ e__ *** halt_reject