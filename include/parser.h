#pragma once

#include "lexer.h"
#include "ast.h"

std::vector<ASTNode *> parse(const std::string &source);
