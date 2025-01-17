#include "9cc.h"

static Node *node_base(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

// For symbol node
static Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = node_base(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static Node *new_unary(NodeKind kind, Node *expr) {
  Node *node = node_base(kind);
  node->lhs = expr;
  return node;
}

// For number node
static Node *new_num_node(int val) {
  Node *node = node_base(ND_NUM);
  node->val = val;
  return node;
}

// For variable node
static Node *new_var_node(char name) {
  Node *node = node_base(ND_VAR);
  node->name = name;
  return node;
}

static Node *stmt(void);
static Node *expr(void);
static Node *assign(void);
static Node *equality(void);
static Node *relational(void);
static Node *add(void);
static Node *mul(void);
static Node *unary(void);
static Node *primary(void);

//  program = stmt*
Node *program(void) {
  Node head = {};
  Node *cur = &head;

  while (!at_eof())  {
    cur->next = stmt();
    cur = cur->next;
  }

  return head.next;
}

// stmt = "return" expr ";" | expr ";"
static Node *stmt(void) {
  if (consume("return")) {
    Node *node = new_unary(ND_RETURN, expr());
    expect(";");
    return node;
  }

  Node *node = new_unary(ND_EXPR_STMT, expr());
  expect(";");
  return node;
}

// expr = assign
static Node *expr(void) {
  return assign();
}

// assign = equality ("=" assign)?
static Node *assign(void) {
  Node *node = equality();

  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

// equality = relational ( "==" relational | "!=" relational)*
static Node *equality(void) {
  Node *node = relational();

  for(;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}

// relational = add ( "<" add | "<=" add | ">" add | ">=" add)*
static Node *relational(void) {
  Node *node = add();

  for(;;) {
    if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume("<="))
      node = new_node(ND_LE, node, add());
    else if (consume(">"))
      node = new_node(ND_LT, add(), node);
    else if (consume(">="))
      node = new_node(ND_LE, add(), node);
    else
      return node;
  }
}

// add = mul ("+" mul | "-" mul)*
static Node *add(void) {
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

// mul = unary ("*" unary | "/" unary)*
static Node *mul(void) {
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

// unary = ("+" | "-")? primary
static Node *unary(void) {
  if (consume("+"))
    return unary();
  if (consume("-"))
    return new_node(ND_SUB, new_num_node(0), unary());
  return primary();
}

// primary = "(" expr ")" | ident | num
static Node *primary(void) {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok)
    return new_var_node(*tok->str);

  return new_num_node(expect_number());
}
