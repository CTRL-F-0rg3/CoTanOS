#pragma once
#include <string>
#include <vector>
#include <sstream>

class CalculatorEngine {
private:
    std::string current_expression;
    double current_result = 0.0;
    bool has_error = false;

    double evaluate_simple(double a, double b, char op);
    int precedence(char op);

public:
    CalculatorEngine() = default;

    void append_char(char c);
    void clear();
    void backspace();
    void evaluate();

    std::string get_display_text() const;
};