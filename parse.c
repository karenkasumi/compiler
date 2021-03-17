#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C.h"
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

Node *code[100];

Node *expr(){
    Node *node = equality();
    if(consume("="))
        node = new_node(ND_ASSIGN, node, expr());
    return node;    
}

Node *stmt(){
    Node *node = expr();
    expect(";");
    return node;
}

void *program(){
    int i = 0;
    while(!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
}

Node *equality() {
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

  if(token->kind == TK_IDENT){
      Node *node = calloc(1, sizeof(Node));
      node->kind = ND_LVAR;
      node->offset = (token->str[0] - 'a' + 1)*8;
      token = token->next;
      return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_number());


}
