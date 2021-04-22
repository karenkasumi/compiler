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

Node *expr(int gloloc){
    Node *node = equality(gloloc);
    if(consume("="))
        node = new_node(ND_ASSIGN, node, expr(gloloc));
    return node;    
}

Node *stmt(int gloloc){
    Node *node;
    if(token->kind == TK_RETURN){
      node = calloc(1, sizeof(Node));
      node->kind = ND_RETURN;
      token = token->next;
      node->lhs = expr(gloloc);
    }else if(token->kind == TK_IF){
      node = calloc(1, sizeof(Node));
      node->rhs = calloc(1, sizeof(Node));
      node->kind = ND_IF;
      token = token->next;
      consume("(");
      node->lhs = expr(gloloc);
      consume(")");
      node->rhs->rhs = stmt(gloloc);
      if(token->kind == TK_ELSE){
        node->kind = ND_IF_ELSE;
        token = token->next;
        node->rhs->lhs = stmt(gloloc);
      }
      return node;
    }else if(token->kind == TK_WHILE){
      node = calloc(1, sizeof(Node));
      node->kind = ND_WHILE;
      token = token->next;
      consume("(");
      node->lhs = expr(gloloc);
      consume(")");
      node->rhs = stmt(gloloc);
      return node;
    }else if(token->kind == TK_FOR){
      node = calloc(1, sizeof(Node));
      node->lhs=calloc(1, sizeof(Node));
      node->lhs->lhs=calloc(1,sizeof(Node));
      node->kind = ND_FOR;
      token = token->next;
      consume("(");
      if(!consume(";")) {
        node->lhs->lhs->lhs = expr(gloloc);
        consume(";");
      }
      if(!consume(";")) {
        node->lhs->lhs->rhs = expr(gloloc);
        consume(";");
      }
      if(!consume(")")) {
        node->lhs->rhs = expr(gloloc);
        consume(")");
      }
      node->rhs = stmt(gloloc);
      return node;
    }else if(consume("{")){
      node = calloc(1, sizeof(Node));
      node->kind = ND_BLOCK;
      int i=0;

      while(!consume("}")){
        node->block[i++] = stmt(gloloc);
      }
      return node;
    }else if(token->kind == TK_IDENT &&
             !memcmp(token->next->str,"(",1)){
      static int glolocloc = 1;
      node = calloc(1, sizeof(LVar));
      LVar *lvar = calloc(1,sizeof(LVar));
      lvar->next = locals[glolocloc];
      lvar->name = token->str;
      lvar->len =  token->len;
      lvar->gloloc = glolocloc;
      node->offset = lvar->offset;
      locals[glolocloc] = lvar;
      node->kind = ND_FUNC_DEF;
      Token *save = token;
      token = token->next->next;
      int i = 0;
      while(!consume(")")){
        node->argu[i++] = primary(glolocloc);
        consume(",");
      }
      if(consume(";")){
        token = save;
        locals[glolocloc] = locals[glolocloc]->next;
        node = expr(glolocloc);
        consume(";");
        return node;
      }
      node->rhs = stmt(glolocloc);
      node->gloloc = glolocloc;
      lvar->def = node->rhs;
      gloloc++;
      return node;
    }else{
      node = expr(gloloc);
    }
    if(!consume(";"))
      error("cccc\n");
    return node;
}

void *program(){
    int i = 0;
    while(!at_eof())
        code[i++] = stmt(0);
    code[i] = NULL;
}

Node *equality(int gloloc) {
  Node *node = relational(gloloc);

  for (;;) {
    if (consume("=="))
      node = new_node(ND_equal, node, relational(gloloc));
    else if (consume("!="))
      node = new_node(ND_nequal, node, relational(gloloc));
    else
      return node;
  }
}

Node *relational(int gloloc){
  Node *node = add(gloloc);
    if (consume("<"))
      node = new_node(ND_big, node, add(gloloc));
    else if (consume("<="))
      node = new_node(ND_ebig, node, add(gloloc));
    else if (consume(">"))
      node = new_node(ND_small, node, add(gloloc));
    else if (consume(">="))
      node = new_node(ND_small, node, add(gloloc));
    else
      return node;  

}

Node *add(int gloloc){
  Node *node = mul(gloloc);

  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul(gloloc));
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul(gloloc));
    else
      return node;
  }
}

Node *mul(int gloloc) {
  Node *node = unary(gloloc);

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary(gloloc));
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary(gloloc));
    else
      return node;
  }
}

Node *unary(int gloloc) {
  if (consume("+"))
    return primary(gloloc);
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary(0));
  return primary(gloloc);
}

Node *primary(int gloloc) {
  if (consume("(")) {
    Node *node = expr(gloloc);
    expect(")");
    return node;
  }

  if(token->kind == TK_IDENT){
    Node *node = calloc(1,sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(token, gloloc);
    if(lvar){
      node->offset = lvar->offset;
      node->gloloc = lvar->gloloc;
    }else{
      lvar = calloc(1,sizeof(LVar));
      lvar->next = locals[gloloc];
      lvar->name = token->str;
      lvar->len =  token->len;
      lvar->offset = locals[gloloc]->offset + 8;
      lvar->gloloc = gloloc;
      node->offset = lvar->offset;
      locals[gloloc] = lvar;
    }
    token = token->next;
    if(consume("(")){
        node->kind = ND_FUNC;
        node->rhs = lvar->def;
        int i= 0;
        while(!consume(")")){
          node->argu[i++] = primary(0);
          consume(",");
        }
    }
    return node;
  }

  // ‚»‚¤‚Å‚È‚¯‚ê‚Î”’l‚Ì‚Í‚¸
  return new_node_num(expect_number());


}


LVar *find_lvar(Token *tok,int gloloc){
  for (LVar *var = locals[gloloc]; var; var = var->next)
    if (var->len == tok->len && gloloc == var->gloloc && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}