#include "calculator_engine.hpp"
#include <stack>
#include <cctype>
#include <cmath>

void CalculatorEngine::append_char(char c) {
    if (has_error) {
        clear();
    }
    current_expression += c;
}

void CalculatorEngine::clear() {
    current_expression.clear();
    current_result = 0.0;
    has_error = false;
}

void CalculatorEngine::backspace() {
    if (!current_expression.empty()) {
        current_expression.pop_back();
    }
}

int CalculatorEngine::precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

double CalculatorEngine::evaluate_simple(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0.0) { has_error = true; return 0.0; }
            return a / b;
        default: return 0.0;
    }
}

void CalculatorEngine::evaluate() {
    if (current_expression.empty()) return;

    std::stack<double> values;
    std::stack<char> ops;

    for (size_t i = 0; i < current_expression.length(); i++) {
        if (current_expression[i] == ' ') continue;

        if (std::isdigit(current_expression[i]) || current_expression[i] == '.') {
            std::string val_str;
            while (i < current_expression.length() && 
                  (std::isdigit(current_expression[i]) || current_expression[i] == '.')) {
                val_str += current_expression[i++];
            }
            i--;
            values.push(std::stod(val_str));
        } else if (current_expression[i] == '(') {
            ops.push('(');
        } else if (current_expression[i] == ')') {
            while (!ops.empty() && ops.top() != '(') {
                if (values.size() < 2) { has_error = true; return; }
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(evaluate_simple(val1, val2, op));
            }
            if (!ops.empty()) ops.pop();
        } else { // Operator
            while (!ops.empty() && precedence(ops.top()) >= precedence(current_expression[i])) {
                if (values.size() < 2) { has_error = true; return; }
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(evaluate_simple(val1, val2, op));
            }
            ops.push(current_expression[i]);
        }
    }

    while (!ops.empty()) {
        if (values.size() < 2) { has_error = true; return; }
        double val2 = values.top(); values.pop();
        double val1 = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(evaluate_simple(val1, val2, op));
    }

    if (!has_error && !values.empty()) {
        current_result = values.top();
        std::ostringstream ss;
        ss << current_result;
        current_expression = ss.str();
    }
}

std::string CalculatorEngine::get_display_text() const {
    if (has_error) return "Błąd";
    return current_expression.empty() ? "0" : current_expression;
}