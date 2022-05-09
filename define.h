// MAJOR OPCODES
#define Normal   0
#define StoreDM  1
#define StoreIM  2
#define Output   3
#define LoadDM   4
#define Input    5
#define Jump     6
#define RsrvdOp  7
// FUNCTIONS
#define AplusB      0
#define AminusB     1
#define Bplusone    2
#define Bminusone   3
#define AND         4
#define OR          5
#define XOR         6
#define RsrvdFun    7
// SKIP
#define Never       0
#define IfNegative  1
#define IfZero      2
#define IfPositive  3