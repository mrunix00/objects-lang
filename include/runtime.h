#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace OLRuntime {
struct Instruction
{
    enum class Type
    {
        Invalid,
        LoadNumber,
        LoadLocal,
        StoreLocal,
        Add,
        Sub,
        Mul,
        Div,
        End,
    } type
        = Type::Invalid;

    union
    {
        void *other;
        double number;
        size_t index;
    } data = {.other = nullptr};
};

struct Program
{
    std::vector<Instruction> instructions;
    std::unordered_map<std::string, size_t> local_vars;
};

class OLRuntime
{
    Program program;
    std::vector<double> stack;
    std::vector<double> local_vars;

    void execute();

public:
    OLRuntime() = default;

    ~OLRuntime() = default;

    void run(const std::string &source);

    [[nodiscard]] std::optional<double> getLastValue() const;
};
} // namespace OLRuntime