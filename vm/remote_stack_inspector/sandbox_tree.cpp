#include <vector>
#include <string>
#include <cstring>
#include <sstream>

#include "main.h"

#define BUF_SIZE 1024

struct node {
    int key;
    std::string token;
    std::vector<node> chdlist;
};

static node root;

inline void tokenize_dot(std::string& str, std::vector<std::string>& tokens)
{
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '.') {
            str[i] = ' ';
        }
    }
    std::stringstream ss(str);
    std::string tk;
    while (ss >> tk) {
        tokens.push_back(tk);
    }
}

void create_sandbox_tree(const char *buf)
{
    std::stringstream ss(buf);
    std::string str;
    int cnt = 0;

    root.key = -1;
    while (ss >> str) {
        std::vector<std::string> tokens;
        tokenize_dot(str, tokens);

        if (tokens.size() == 0) {
            ++cnt;
            continue;
        }

        node *parent = &root;
        for (size_t i = 0; i < tokens.size(); ++i) {
            node *matching_chd = NULL;
            for (size_t j = 0; j < parent->chdlist.size(); ++j) {
                if (!(parent->chdlist[j].token).compare(tokens[i])) {
                    matching_chd = &(parent->chdlist[j]);
                    break;
                }
            }
            if (matching_chd == NULL) {
                node new_node;
                new_node.key = -1;
                new_node.token = tokens[i];
                parent->chdlist.push_back(new_node);
                matching_chd = &parent->chdlist[parent->chdlist.size()-1];
            }
            parent = matching_chd;
        }
        parent->key = cnt++;
    }
}

int query_sandbox_key(const std::string& query)
{
    char word[BUF_SIZE];
    size_t offset = 0, i = 0;
    node *parent = &root;
    for (offset = 0; offset < query.size(); ++offset) {
        if (query[offset] == '.') {
            word[i] = '\0';
            bool is_matched = false;
            for (size_t j = 0; j < parent->chdlist.size(); ++j) {
                if (!strncmp(word, parent->chdlist[j].token.c_str(), i)) {
                    parent = &(parent->chdlist[j]);
                    is_matched = true;
                    break;
                }
            }
            if (is_matched) {
                i = 0;
                continue;
            } else {
                break;
            }
        }
        word[i++] = query[offset];
    }
    if (offset == query.size()) {
        word[i] = '\0';
        for (size_t j = 0; j < parent->chdlist.size(); ++j) {
            if (!strncmp(word, parent->chdlist[j].token.c_str(), i)) {
                parent = &(parent->chdlist[j]);
                break;
            }
        }
    }
    return parent->key;
}
