#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C.h"



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



void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error("bbbbb\n");
  token = token->next;
}


int expect_number() {
  if (token->kind != TK_NUM)
    error("%d\n",token->kind);
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
    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])){
      cur = new_token(TK_RETURN,cur,p,6);
      p +=6;
      continue;
    }
    if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])){
      cur = new_token(TK_IF, cur, p, 2);
      p += 2;
      continue;
    }
    if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])){
      cur = new_token(TK_ELSE, cur, p, 4);
      p +=4;
      continue;
    }
    if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])){
      cur = new_token(TK_WHILE, cur, p, 5);
      p += 5;
      continue;
    }
    if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])){
      cur =new_token(TK_FOR, cur, p, 3);
      p += 3;
      continue;
    }

    if('a' <= *p && *p <= 'z'){
      int i=0;
      char *q = p;
      while('a' <= *q && *q <= 'z'){
        i++;
        q++;
      }
      cur = new_token(TK_IDENT, cur, p, i);
      p=p+i;
      continue;
    }


    if (*p == '+' || *p == '-' || 
        *p =='*'  || *p == '/' || 
        *p == ')' || *p == '(' ||
        *p == '>' || *p == '<' ||
        *p == ';' || *p == '=' ||
        *p == '{' || *p == '}') {
      cur = new_token(TK_RESERVED, cur, p++,1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p,0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p-q;
      continue;
    }

    error("eeee");
  }

  new_token(TK_EOF, cur, p,0);
  return head.next;
}

int is_alnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
}