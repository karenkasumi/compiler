typedef enum {
  TK_RESERVED, 
  TK_NUM,     
  TK_EOF,
  TK_IDENT,
  TK_RETURN,
  TK_IF,
  TK_ELSE,
  TK_WHILE,
  TK_FOR,
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind; 
  Token *next;    
  int val;      
  char *str;
  int len;
};

int is_alnum(char c);

typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // êÆêî
  ND_equal,
  ND_nequal,
  ND_big,
  ND_ebig,
  ND_small,
  ND_esmall,
  ND_ASSIGN,
  ND_LVAR,
  ND_RETURN,
  ND_IF,
  ND_IF_ELSE,
  ND_WHILE,
  ND_FOR,
  ND_BLOCK,
  ND_FUNC,
  ND_FUNC_DEF,
} NodeKind;

typedef struct Node Node;


struct Node {
  NodeKind kind; 
  Node *lhs;     
  Node *rhs;     
  int val;
  int offset;
  Node *argu[10];
  Node *block[100];
  int gloloc;       
};

typedef struct LVar LVar;


struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
  Node *def;
  int gloloc;
};

LVar *find_lvar(Token *tok, int gloloc);
extern LVar *locals[];





Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
void *program();
Node *stmt(int);
Node *equality(int);
Node *expr(int);
Node *relational(int);
Node *add(int);
Node *mul(int);
Node *unary(int);
Node *primary(int);
extern Node *code[];

void gen(Node *node);
void gen_lval(Node *node);

void error(char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);

int main(int argc, char **argv);
extern Token *token;