#include "runtime.h"

#include <parser.h>

void OLRuntime::OLRuntime::execute()
{
    for (const auto &[type, data] : program.instructions) {
        switch (type) {
        case Instruction::Type::LoadNumber:
            stack.push_back(data.number);
            break;
        case Instruction::Type::LoadLocal:
            stack.push_back(local_vars.at(data.index));
            break;
        case Instruction::Type::StoreLocal:
            if (data.index >= local_vars.size())
                local_vars.resize(data.index + 1);
            local_vars[data.index] = stack.back();
            stack.pop_back();
            break;
        case Instruction::Type::Add: {
            const auto x = stack.back();
            stack.pop_back();
            const auto y = stack.back();
            stack.pop_back();
            stack.push_back(x + y);
        }
        break;
        case Instruction::Type::Sub: {
            const auto x = stack.back();
            stack.pop_back();
            const auto y = stack.back();
            stack.pop_back();
            stack.push_back(y - x);
        }
        break;
        case Instruction::Type::Mul: {
            const auto x = stack.back();
            stack.pop_back();
            const auto y = stack.back();
            stack.pop_back();
            stack.push_back(x * y);
        }
        break;
        case Instruction::Type::Div: {
            const auto x = stack.back();
            stack.pop_back();
            const auto y = stack.back();
            stack.pop_back();
            stack.push_back(y / x);
        }
        break;
        case Instruction::Type::End:
            return;
        default: ;
        }
    }
}

void OLRuntime::OLRuntime::run(const std::string &source)
{
    for (const auto AST = parse(source); auto &node : AST) {
        node->compile(program);
        delete node;
    }
    execute();
}

std::optional<double> OLRuntime::OLRuntime::getLastValue() const
{
    if (stack.empty())
        return std::nullopt;
    return stack.back();
}