#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// tokenize.c
//

typedef enum {
  TK_RESERVED, // input symbol
  TK_NUM,      // input number
  TK_EOF,      // end of input
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind; // token kind
  Token *next;    // next token
  int val;        // number(when kind == TK_NUM)
  char *str;      // token string
  int len;        // length of token string
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number(void);
bool at_eof(void);
Token *tokenize(void);

extern char *user_input;
extern Token *token;

//
// parse.c
//

typedef enum {
  ND_ADD,      // +
  ND_SUB,      // -
  ND_MUL,      // *
  ND_DIV,      // /
  ND_EQ,       // ==
  ND_NE,       // !=
  ND_LT,       // <
  ND_LE,       // <=
  ND_RETURN,   // "return"
  ND_NUM,      // number(int)
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;  // type of node
  Node *next;     // next node
  Node *lhs;      // left hand side
  Node *rhs;      // right hand side
  int val;        // use only ND_NUM type
};

Node *program(void);

//
// codegen.c
//

void codegen(Node *node);
