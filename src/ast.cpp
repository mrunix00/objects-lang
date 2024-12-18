#include "ast.h"

#include <cassert>
#include <utility>

bool operator==(const std::vector<ASTNode *> &left, const std::vector<ASTNode *> &right)
{
    if (left.size() != right.size())
        return false;
    for (size_t i = 0; i < left.size(); ++i) {
        if (*left[i] != *right[i])
            return false;
    }
    return true;
}

void destroy_ast(const std::vector<ASTNode *> &ast)
{
    for (auto &node : ast)
        delete node;
}

SingleNode::SingleNode(Token token)
    : token(std::move(token))
{
    type = Type::SingleNode;
}
void SingleNode::compile(OLRuntime::Program &program) const
{
    switch (token.type) {
    case Token::Type::Number: {
        program.instructions.push_back(
        {
            .type = OLRuntime::Instruction::Type::LoadNumber,
            .data = {.number = {std::stod(token.value)}},
        });
    }
    break;
    case Token::Type::Identifier: {
        assert(program.local_vars.contains(token.value));
        program.instructions.push_back(
        {
            .type = OLRuntime::Instruction::Type::LoadLocal,
            .data = {.index = program.local_vars.at(token.value)},
        });
    }
    break;
    default:
        throw std::runtime_error("Unimplemented method!");
    }
}
bool SingleNode::operator==(const ASTNode &other) const
{
    return type == other.type && token == dynamic_cast<const SingleNode &>(other).token;
}

VarDeclaration::VarDeclaration(Token name)
    : name(std::move(name))
{
    type = Type::VarDeclaration;
}
void VarDeclaration::compile(OLRuntime::Program &program) const
{
    assert(!program.local_vars.contains(name.value));
    program.local_vars.insert({name.value, {program.local_vars.size()}});
}
bool VarDeclaration::operator==(const ASTNode &other) const
{
    return type == other.type && name == dynamic_cast<const VarDeclaration &>(other).name;
}

BinaryExpression::BinaryExpression(ASTNode *left, ASTNode *right, Token op)
    : left(left)
      , right(right)
      , op(std::move(op))
{
    type = Type::BinaryExpression;
    assert(left != nullptr && right != nullptr);
}
BinaryExpression::~BinaryExpression()
{
    delete left;
    delete right;
}
void BinaryExpression::compile(OLRuntime::Program &program) const
{
    if (op.type == Token::Type::Equals) {
        right->compile(program);
        left->compile(program);
        if (left->type == Type::VarDeclaration) {
            const auto name = dynamic_cast<VarDeclaration *>(left)->name.value;
            program.instructions.push_back(
            {
                .type = OLRuntime::Instruction::Type::StoreLocal,
                .data = {.index = program.local_vars[name]},
            });
        }
        return;
    }
    left->compile(program);
    right->compile(program);
    switch (op.type) {
    case Token::Type::Plus:
        program.instructions.push_back(
        {
            .type = OLRuntime::Instruction::Type::Add,
        });
        break;
    case Token::Type::Minus:
        program.instructions.push_back(
        {
            .type = OLRuntime::Instruction::Type::Sub,
        });
        break;
    case Token::Type::Asterisk:
        program.instructions.push_back(
        {
            .type = OLRuntime::Instruction::Type::Mul,
        });
        break;
    case Token::Type::Slash:
        program.instructions.push_back(
        {
            .type = OLRuntime::Instruction::Type::Div,
        });
        break;
    default:
        throw std::runtime_error("Unimplemented method!");
    }
}
bool BinaryExpression::operator==(const ASTNode &other) const
{
    if (type != other.type)
        return false;
    auto &expr = dynamic_cast<const BinaryExpression &>(other);
    return *left == *expr.left && *right == *expr.right && op == expr.op;
}

UnaryExpression::UnaryExpression(Token op, ASTNode *operand)
    : op(std::move(op))
      , operand(operand)
{
    type = Type::UnaryExpression;
    assert(operand != nullptr);
}
UnaryExpression::~UnaryExpression()
{
    delete operand;
}
bool UnaryExpression::operator==(const ASTNode &other) const
{
    if (type != other.type)
        return false;
    auto &expr = dynamic_cast<const UnaryExpression &>(other);
    return op == expr.op && operand == expr.operand;
}

ParenthesizedExpression::ParenthesizedExpression(ASTNode *expression)
    : expression(expression)
{
    type = Type::ParenthesizedExpression;
    assert(expression != nullptr);
}
ParenthesizedExpression::~ParenthesizedExpression()
{
    delete expression;
}
bool ParenthesizedExpression::operator==(const ASTNode &other) const
{
    if (type != other.type)
        return false;
    auto &expr = dynamic_cast<const ParenthesizedExpression &>(other);
    return *expression == *expr.expression;
}

FunctionDeclaration::FunctionDeclaration(Token name, std::vector<ASTNode *> args, ASTNode *body)
    : name(std::move(name))
      , args(std::move(args))
      , body(body)
{
    type = Type::FunctionDeclaration;
    assert(body != nullptr);
}
FunctionDeclaration::~FunctionDeclaration()
{
    delete body;
    for (auto &arg : args)
        delete arg;
}
bool FunctionDeclaration::operator==(const ASTNode &other) const
{
    if (type != other.type)
        return false;
    auto &func = dynamic_cast<const FunctionDeclaration &>(other);
    return name == func.name && args == func.args && *body == *func.body;
}

ScopeBlock::ScopeBlock(std::vector<ASTNode *> statements)
    : statements(std::move(statements))
{
    type = Type::ScopeBlock;
}
ScopeBlock::~ScopeBlock()
{
    for (auto &statement : statements)
        delete statement;
}
bool ScopeBlock::operator==(const ASTNode &other) const
{
    if (type != other.type)
        return false;
    auto &block = dynamic_cast<const ScopeBlock &>(other);
    return statements == block.statements;
}

IfStatement::IfStatement(ASTNode *condition, ASTNode *body, ASTNode *else_body)
    : condition(condition)
      , body(body)
      , else_body(else_body)
{
    type = Type::IfStatement;
}
IfStatement::IfStatement(ASTNode *condition, ASTNode *body)
    : condition(condition)
      , body(body)
{
    type = Type::IfStatement;
}
IfStatement::~IfStatement()
{
    delete condition;
    delete body;
    if (else_body.has_value())
        delete else_body.value();
}
bool IfStatement::operator==(const ASTNode &other) const
{
    if (type != other.type)
        return false;
    auto &if_stmt = dynamic_cast<const IfStatement &>(other);
    if (if_stmt.else_body.has_value() != else_body.has_value())
        return false;
    if (if_stmt.else_body.has_value() && *if_stmt.else_body.value() != *else_body.value())
        return false;
    return *condition == *if_stmt.condition && *body == *if_stmt.body;
}

WhileStatement::WhileStatement(ASTNode *condition, ASTNode *body)
    : condition(condition)
      , body(body)
{
    type = Type::WhileStatement;
}
WhileStatement::~WhileStatement()
{
    delete condition;
    delete body;
}
bool WhileStatement::operator==(const ASTNode &other) const
{
    if (type != other.type)
        return false;
    auto &while_stmt = dynamic_cast<const WhileStatement &>(other);
    return *condition == *while_stmt.condition && *body == *while_stmt.body;
}

ArrayAccess::ArrayAccess(ASTNode *array, ASTNode *index)
    : array(array)
      , index(index)
{
    type = Type::ArrayAccess;
}
ArrayAccess::~ArrayAccess()
{
    delete array;
    delete index;
}
bool ArrayAccess::operator==(const ASTNode &other) const
{
    if (type != other.type)
        return false;
    auto &array_access = dynamic_cast<const ArrayAccess &>(other);
    return *array == *array_access.array && *index == *array_access.index;
}

FieldAccess::FieldAccess(ASTNode *record, ASTNode *field)
    : record(record)
      , field(field)
{
    type = Type::FieldAccess;
    assert(record != nullptr && field != nullptr);
}
FieldAccess::~FieldAccess()
{
    delete record;
    delete field;
}
bool FieldAccess::operator==(const ASTNode &other) const
{
    if (type != other.type)
        return false;
    auto &field_access = dynamic_cast<const FieldAccess &>(other);
    return *record == *field_access.record && *field == *field_access.field;
}

FunctionCall::FunctionCall(ASTNode *name, std::vector<ASTNode *> args)
    : name(name)
      , args(std::move(args))
{
    type = Type::FunctionCall;
}
FunctionCall::~FunctionCall()
{
    delete name;
    for (const auto &arg : args)
        delete arg;
}
bool FunctionCall::operator==(const ASTNode &other) const
{
    if (type != other.type)
        return false;
    auto &func_call = dynamic_cast<const FunctionCall &>(other);
    return *name == *func_call.name && args == func_call.args;
}

Constructor::Constructor(ASTNode *record)
    : record(record)
{
    type = Type::Constructor;
}
Constructor::~Constructor()
{
    delete record;
}
bool Constructor::operator==(const ASTNode &other) const
{
    if (type != other.type)
        return false;
    auto &constructor = dynamic_cast<const Constructor &>(other);
    return *record == *constructor.record;
}
