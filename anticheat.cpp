#include <cstdlib>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <cmath>

#include "lang.h"
#include "visitor.h"

class StructureAnalyzer : public Visitor<std::string> {
 public:
  std::string visitProgram(Program *node) override {
    std::string result = "P{";
    for (auto func : node->body) {
      result += visitFunctionDeclaration(func) + ";";
    }
    result += "}";
    return result;
  }
  
  std::string visitFunctionDeclaration(FunctionDeclaration *node) override {
    return "F(" + std::to_string(node->params.size()) + "):" + visitStatement(node->body);
  }

  std::string visitExpressionStatement(ExpressionStatement *node) override {
    return "E[" + visitExpression(node->expr) + "]";
  }
  
  std::string visitSetStatement(SetStatement *node) override {
    return "S[" + visitExpression(node->value) + "]";
  }
  
  std::string visitIfStatement(IfStatement *node) override {
    return "I{" + visitExpression(node->condition) + ":" + visitStatement(node->body) + "}";
  }
  
  std::string visitForStatement(ForStatement *node) override {
    return "L{" + visitStatement(node->init) + ";" + visitExpression(node->test) + ";" + 
           visitStatement(node->update) + ":" + visitStatement(node->body) + "}";
  }
  
  std::string visitBlockStatement(BlockStatement *node) override {
    std::string result = "B{";
    for (auto stmt : node->body) {
      result += visitStatement(stmt) + ";";
    }
    result += "}";
    return result;
  }
  
  std::string visitReturnStatement(ReturnStatement *node) override { 
    return "R[" + visitExpression(node->value) + "]"; 
  }

  std::string visitIntegerLiteral(IntegerLiteral *node) override { 
    return "N"; 
  }
  
  std::string visitVariable(Variable *node) override { 
    return "V"; 
  }
  
  std::string visitCallExpression(CallExpression *node) override {
    std::string result = "C(" + std::to_string(node->args.size()) + "){";
    for (auto expr : node->args) {
      result += visitExpression(expr) + ",";
    }
    result += "}";
    return result;
  }
};

double calculateSimilarity(const std::string &s1, const std::string &s2) {
  int len1 = s1.length();
  int len2 = s2.length();
  
  if (len1 == 0 && len2 == 0) return 1.0;
  if (len1 == 0 || len2 == 0) return 0.0;
  
  // Levenshtein distance
  std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));
  
  for (int i = 0; i <= len1; i++) dp[i][0] = i;
  for (int j = 0; j <= len2; j++) dp[0][j] = j;
  
  for (int i = 1; i <= len1; i++) {
    for (int j = 1; j <= len2; j++) {
      if (s1[i-1] == s2[j-1]) {
        dp[i][j] = dp[i-1][j-1];
      } else {
        dp[i][j] = 1 + std::min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
      }
    }
  }
  
  int distance = dp[len1][len2];
  int maxLen = std::max(len1, len2);
  return 1.0 - (double)distance / maxLen;
}

int main() {
  try {
    Program *prog1 = scanProgram(std::cin);
    Program *prog2 = scanProgram(std::cin);
    
    std::string input;
    int c;
    while ((c = std::cin.get()) != EOF) {
      input += c;
    }

    // Get structural signatures
    StructureAnalyzer analyzer;
    std::string struct1 = analyzer.visitProgram(prog1);
    std::string struct2 = analyzer.visitProgram(prog2);
    
    // Calculate structural similarity
    double structSim = calculateSimilarity(struct1, struct2);
    
    // Compare string representations
    std::string str1 = prog1->toString();
    std::string str2 = prog2->toString();
    double stringSim = calculateSimilarity(str1, str2);
    
    // Combined similarity
    double similarity = 0.7 * structSim + 0.3 * stringSim;
    
    // Adjust to be centered around 0.5
    double score = 0.5 + (similarity - 0.5) * 0.8;
    
    // Clamp to [0, 1]
    score = std::max(0.0, std::min(1.0, score));
    
    std::cout << score << std::endl;
  } catch (...) {
    std::cout << 0.5 << std::endl;
  }
  
  return 0;
}
