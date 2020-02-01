#include "9cc.h"

Node *node_base(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

// For symbol node
Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = node_base(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

// For number node
Node *new_node_num(int val) {
  Node *node = node_base(ND_NUM);
  node->val = val;
  return node;
}

Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *primary(void);

// expr = equality
Node *expr(void) {
  return equality();
}

// equality = relational ( "==" relational | "!=" relational)*
Node *equality(void) {
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
Node *relational(void) {
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
Node *add(void) {
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
Node *mul(void) {
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
Node *unary(void) {
  if (consume("+"))
    return unary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), unary());
  return primary();
}

// primary = "(" expr ")" | num
Node *primary(void) {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  return new_node_num(expect_number());
}
