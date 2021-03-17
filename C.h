typedef enum {
  TK_RESERVED, 
  TK_NUM,     
  TK_EOF,
  TK_IDENT,      
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind; 
  Token *next;    
  int val;        
  char *str;
  int len;
};

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
} NodeKind;

typedef struct Node Node;


struct Node {
  NodeKind kind; 
  Node *lhs;     
  Node *rhs;     
  int val;
  int offset;       
};
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
void *program();
Node *stmt();
Node *equality();
Node *expr();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
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