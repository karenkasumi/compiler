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
    Node *node;
    if(token->kind == TK_RETURN){
      node = calloc(1, sizeof(Node));
      node->kind = ND_RETURN;
      token = token->next;
      node->lhs = expr();
    }else if(token->kind == TK_IF){
      node = calloc(1, sizeof(Node));
      node->kind = ND_IF;
      token = token->next;
      consume("(");
      node->lhs = expr();
      consume(")");
      node->rhs = stmt();
      return node;
    }else{
      node = expr();
    }
    if(!consume(";"))
      error("cccc\n");
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
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  if(token->kind == TK_IDENT){
    Node *node = calloc(1,sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(token);
    if(lvar){
      node->offset = lvar->offset;
    }else{
      lvar = calloc(1,sizeof(LVar));
      lvar->next = locals;
      lvar->name = token->str;
      lvar->len =  token->len;
      lvar->offset = locals->offset + 8;
      node->offset = lvar->offset;
      locals = lvar;
    }
    token = token->next;
    return node;
  }

  // ‚»‚¤‚Å‚È‚¯‚ê‚Î”’l‚Ì‚Í‚¸
  return new_node_num(expect_number());


}


LVar *find_lvar(Token *tok){
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}