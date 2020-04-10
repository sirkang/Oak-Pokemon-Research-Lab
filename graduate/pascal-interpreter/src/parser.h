//
// Created by pikachu on 2020/4/10.
//

#ifndef PASCAL_INTERPRETER_PARSER_H
#define PASCAL_INTERPRETER_PARSER_H
#include "token.h"
#include "lexer.h"
#include <string>
#include <iostream>
#include <any>
#include <unordered_set>
#include <utility>
#include <vector>
enum class ast_node_type {
    binary_operator, number, unknown
};
class abstract_node_visitor;
struct ast {
    ast_node_type type = ast_node_type::unknown;
    friend ostream &operator<<(ostream &os, const ast &ast) {
        os << "ast";
        return os;
    }
    virtual int accept(abstract_node_visitor* visitor) = 0;
};
struct binary_operator: public ast {
    ast* left;
    token op;
    ast* right;

    binary_operator(ast *left, token op, ast *right)
        : left(left), op(std::move(op)), right(right) {
        type = ast_node_type::binary_operator;
    }

    int accept(abstract_node_visitor *visitor) override;

    friend ostream &operator<<(ostream &os, const binary_operator &ast) {
        os << "binary_operator";
        os << "(";
        os << *ast.left << ", " << ast.op << ", " << *ast.right;
        os << ")";
        return os;
    }

};
struct unary_operator: public ast {
    token op;
    ast* value;

    unary_operator(token op, ast *value) : op(std::move(op)), value(value) {}

    friend ostream &operator<<(ostream &os, const unary_operator &ast) {
        os << "unary_operator";
        os << "(";
        os << ast.op << ", " << *ast.value;
        os << ")";
        return os;
    }

    int accept(abstract_node_visitor *visitor) override;
};
struct compound: public ast {
    vector<ast*> children;

    compound(vector<ast*> children) : children(std::move(children)) {}

    int accept(abstract_node_visitor *visitor) override;

};
struct noop: public ast {
    int accept(abstract_node_visitor *visitor) override;
};
struct assignment: public ast {
    ast* left;
    ast* right;

    assignment(ast *left, ast *right) : left(left), right(right) {}

    int accept(abstract_node_visitor *visitor) override;
};
struct variable_node: public ast {
    token id;

    variable_node(token id) : id(std::move(id)) {}

    int accept(abstract_node_visitor *visitor) override;
};
struct number: public ast {
    int value;

    explicit number(const token &token) {
        value = token.get_value<int>();
        type = ast_node_type::number;
    }

    int accept(abstract_node_visitor *visitor) override;

    friend ostream &operator<<(ostream &os, const number &ast) {
        os << "number";
        os << "(";
        os << ast.value;
        os << ")";
        return os;
    }
};
class parser {
public:
    explicit parser(lexer lexer);
    ast* parse();
    ast* expr();
private:
    void eat(token_type type);
    void error(string msg, token_type expect_type) {
        std::cout << "Error: " << msg << std::endl;
        std::cout << _lexer.get_text() << std::endl;
        for(int i=0;i<cur_token.pos;i++) {
            std::cout << " ";
        }
        std::cout << "^" << std::endl;
        std::cout << "Expect " << expect_type << std::endl;
        throw msg;
    }
    void error(string msg = "Invalid syntax") {
        std::cout << "Error: " << msg << std::endl;
        exit(1);
    }
    ast* factor();
    ast* term();
    ast* program();
    ast* compound_statement();
    vector<ast*> statement_list();
    ast* statement();
    ast* assignment_statement();
    ast* variable();
    ast* empty();
private:
    lexer _lexer;
    token cur_token;
    bool first = true;
};
struct abstract_node_visitor {
    virtual int visit(number* node) = 0;

    virtual int visit(binary_operator* node) = 0;

    virtual int visit(unary_operator* node) = 0;

    virtual int visit(noop* node) = 0;

    virtual int visit(compound* node) = 0;
    virtual int visit(assignment* node) = 0;
    virtual int visit(variable_node* node) = 0;

};
struct node_visitor: public abstract_node_visitor {
    int visit(number* node) override {
        return node->value;
    }

    int visit(unary_operator *node) override {
        auto val = node->value->accept(this);
        if (node->op.get_value<string>() == "-") {
            val = -val;
        }
        return val;
    }

    int visit(binary_operator* node) override {
        auto a = node->left->accept(this);
        auto b = node->right->accept(this);
        switch (node->op.type) {
            case token_type::plus:
                return a + b;
            case token_type::minus:
                return a - b;
            case token_type::multiplication:
                return a * b;
            case token_type::division:
                return a / b;
            default:
                break;
        }
        std::cout << "Error" << std::endl;
        exit(1);
    }

    int visit(noop *node) override {
        return 0;
    }

    int visit(compound *node) override {
        return 0;
    }

    int visit(assignment *node) override {
        return 0;
    }

    int visit(variable_node *node) override {
        return 0;
    }
};

#endif //PASCAL_INTERPRETER_PARSER_H
