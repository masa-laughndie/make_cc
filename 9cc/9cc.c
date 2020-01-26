#include "9cc.h"

//
// Parser
//


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

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// expr = equality
Node *expr() {
  return equality();
}

// equality = relational ( "==" relational | "!=" relational)*
Node *equality() {
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
Node *relational() {
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
Node *add() {
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

// unary = ("+" | "-")? primary
Node *unary() {
  if (consume("+"))
    return unary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), unary());
  return primary();
}

// primary = "(" expr ")" | num
Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  return new_node_num(expect_number());
}

//
// Code generator
//

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
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("%s: Wrong number of arguments!", argv[0]);
    return 1;
  }

  // Tokinize and parse
  user_input = argv[1];
  token = tokenize();
  Node *node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
