/*#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }
  char *p = argv[1];
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  printf("  mov rax, %ld\n", strtol(p,&p,10));
  while(*p){
      if(*p=='+'){
          p++;
          printf("add rax, %ld\n",strtol(p,&p,10));
          continue;
      }

      if(*p=='-'){
          p++;
          printf("sub rax ,%ld\n",strtol(p,&p,10));
          continue;
      }

      fprintf(stderr,"'%c'\n",*p);

  }
  printf("  ret\n");
  return 0;
}
*/
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
  TK_RESERVED, 
  TK_NUM,     
  TK_EOF,      
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind; 
  Token *next;    
  int val;        
  char *str;
  int len;
};


Token *token;

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}


bool consume(char *op) {
  if (token->kind != TK_RESERVED || 
      strlen(op) != token->len ||
      memcmp(token->str,op,token->len))
    return false;
  token = token->next;
  return true;
}


/*void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error("'%c'ではありません", op);
  token = token->next;
}*/
void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error("\n");
  token = token->next;
}


int expect_number() {
  if (token->kind != TK_NUM)
    error("数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}


Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}
bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {

    if (isspace(*p)) {
      p++;
      continue;
    }
    if (startswith(p,"==")||
        startswith(p,"!=")||
        startswith(p,"<=")||
        startswith(p,">=")){
      cur = new_token(TK_RESERVED, cur, p,2);
      p+=2;
      continue;
    }
    if (*p == '+' || *p == '-' || 
        *p =='*'  || *p == '/' || 
        *p == ')' || *p == '(' ||
        *p == '>' || *p == '<') {
      cur = new_token(TK_RESERVED, cur, p++,1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p,0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len= p-q;
      continue;
    }

    error("トークナイズできません");
  }

  new_token(TK_EOF, cur, p,0);
  return head.next;
}
// 抽象構文木のノードの種類
typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // 整数
  ND_equal,
  ND_nequal,
  ND_big,
  ND_ebig,
  ND_small,
  ND_esmall,
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  int val;       // kindがND_NUMの場合のみ使う
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *expr();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary();

Node *expr() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node(ND_equal, node, relational());
    else if (consume("!="))
      node = new_node(ND_nequal, node, relational());
    else
      return node;
  }
}

Node *relational(){
  Node *node = add();
    if (consume("<"))
      node = new_node(ND_big, node, add());
    else if (consume("<="))
      node = new_node(ND_ebig, node, add());
    else if (consume(">"))
      node = new_node(ND_small, node, add());
    else if (consume(">="))
      node = new_node(ND_small, node, add());
    else
      return node;  

}

Node *add(){
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

Node *primary() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_number());
}

void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_equal:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_nequal:
    printf("  setne rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_big:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_ebig:
    printf("  cmp rax, rdi\n");
     printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_small:
    printf("  cmp rdi, rax\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_esmall:      
    printf("  cmp rdi, rax\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break; 

  }


    printf("  push rax\n");
}


int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  
  token = tokenize(argv[1]);
  Node *node = expr();
  
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

//  printf("  mov rax, %d\n", expect_number());
gen(node);

/*while (!at_eof()) {
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }
*/
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}