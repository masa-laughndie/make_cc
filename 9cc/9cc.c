#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2)
    error("%s: Wrong number of arguments!", argv[0]);

  // Tokinize and parse
  user_input = argv[1];
  token = tokenize();
  Node *node = program();

  // Travers the AST to emit assembly
  codegen(node);
  return 0;
}
