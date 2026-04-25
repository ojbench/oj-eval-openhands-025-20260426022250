#include <iostream>
#include <random>
#include <sstream>

#include "lang.h"
#include "transform.h"

class Cheat : public Transform {
 private:
  std::mt19937 rng;
  int counter;
  
 public:
  Cheat() : rng(42), counter(0) {}
  
  Variable *transformVariable(Variable *node) override {
    return new Variable("v_" + node->name + "_x");
  }
  
  FunctionDeclaration *transformFunctionDeclaration(FunctionDeclaration *node) override {
    std::vector<Variable *> params;
    for (auto param : node->params) {
      params.push_back(transformVariable(param));
    }
    std::string newName = (node->name == "main") ? "main" : ("f_" + node->name + "_y");
    return new FunctionDeclaration(newName, params, transformStatement(node->body));
  }
  
  Expression *transformIntegerLiteral(IntegerLiteral *node) override {
    if (node->value == 0) {
      return new CallExpression("-", {new IntegerLiteral(1), new IntegerLiteral(1)});
    } else if (node->value == 1) {
      return new CallExpression("-", {new IntegerLiteral(2), new IntegerLiteral(1)});
    } else if (node->value > 1 && node->value < 100) {
      return new CallExpression("+", {new IntegerLiteral(node->value - 1), new IntegerLiteral(1)});
    }
    return new IntegerLiteral(node->value);
  }
  
  Expression *transformCallExpression(CallExpression *node) override {
    std::vector<Expression *> args;
    for (auto arg : node->args) {
      args.push_back(transformExpression(arg));
    }
    if (node->func == "main") {
      return new CallExpression("main", args);
    } else if (builtinFunctions.count(node->func)) {
      return new CallExpression(node->func, args);
    } else {
      return new CallExpression("f_" + node->func + "_y", args);
    }
  }
  
  Statement *transformIfStatement(IfStatement *node) override {
    auto cond = transformExpression(node->condition);
    auto body = transformStatement(node->body);
    auto negCond = new CallExpression("!", {cond});
    auto emptyBlock = new BlockStatement({});
    return new BlockStatement({
      new IfStatement(negCond, emptyBlock),
      new IfStatement(cond, body)
    });
  }
};

int main() {
  auto code = scanProgram(std::cin);
  auto cheat = Cheat().transformProgram(code);
  std::cout << cheat->toString();
  return 0;
}
