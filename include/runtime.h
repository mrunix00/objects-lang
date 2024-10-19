#pragma once
#include <optional>
#include <string>
#include <vector>

namespace OLRuntime {

struct Instruction
{
    enum class Type {
        Invalid,
        LoadNumber,
        Add,
        End,
    } type
        = Type::Invalid;

    union {
        void *other;
        double number;
    } data = {.other = nullptr};
};

struct Program
{
    std::vector<Instruction> instructions;
};

class OLRuntime
{
    Program program;
    std::vector<double> stack;

    void execute();

public:
    OLRuntime() = default;
    ~OLRuntime() = default;

    void run(const std::string &source);
    [[nodiscard]] std::optional<double> getLastValue() const;
};
} // namespace OLRuntime