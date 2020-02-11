#include "9cc.h"

char *user_input;
Token *token;

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// 期待する演算子かどうかを判定して、当てはまる場合は次のトークンに進める
// これによる判定を条件式に含むことで、次のトークンにさせたい処理を行わせる
bool consume(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

// 期待する演算子かどうかを判定して、当てはまる場合は次のトークンに進める
void expect(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "expected '%s'!", op);
  token = token->next;
}

int expect_number(void) {
  if (token->kind != TK_NUM)
    error_at(token->str, "expected a number!");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof(void) {
  return token->kind == TK_EOF;
}

static Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

static bool startswitch(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

static bool is_alpha(char c) {
  return ('a' <= c && c <= 'z') ||
    ('A' <= c && c <= 'Z') ||
    c == '_';
}

static bool is_alnum(char c) {
  return is_alpha(c) || ('0' <= c && c <= '9');
}

Token *tokenize() {
  char *p = user_input;
  Token head = {};
  Token *cur = &head;

  while (*p) {
    // Skip whitespace characters
    if (isspace(*p)) {
      p++;
      continue;
    }

    // Keyword
    if (startswitch(p, "return") && !is_alnum(p[6])) {
      cur = new_token(TK_RESERVED, cur, p, 6);
      p += 6;
      continue;
    }

    // Multi-letter punctuator
    if (startswitch(p, "==") || startswitch(p, "!=") ||
        startswitch(p, "<=") || startswitch(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    // Single-letter punctuator
    if (ispunct(*p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    // Interger literal
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    error_at(p, "invalid token!");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}
