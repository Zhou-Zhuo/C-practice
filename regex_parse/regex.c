#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "translate.h"
#include "regex.h"
#include "symbol_table.h"

struct symbol_table symbol_tbl;

struct grammar_tree_node *gram_tree_gen_child(
        struct grammar_tree_node *parent,
        int nr,
        int type,
        int value)
{
    struct grammar_tree_node *child;

    child = malloc(sizeof(struct grammar_tree_node));
    if (!child)
        error("malloc child fail\n");
    child->type = type;
    child->value = value;
    child->parent = parent;
    child->child[0] = NULL;
    child->child[1] = NULL;
    parent->child[nr] = child;

    return child;
}

static inline int newpos()
{
    static int pos = 0;
    return ++ pos;
}

/* indexed by position */
static struct bitmap **followpos_tbl;

static struct grammar_tree_node **pos_to_grammar_tree_node;

int accept_position = 0;

static struct grammar_tree_node *suffix_to_grammar_tree(const char *suffix,
        int len, int *pleft_len)
{
    int rest = len;
    char c;
    struct grammar_tree_node *node;
    struct grammar_tree_node *child[2];
    struct grammar_tree_node *left;
    struct grammar_tree_node *right;
    int pos;

    node = malloc(sizeof(struct grammar_tree_node));
    if (!node)
        error("malloc grammar_tree_node failed\n");

    c = suffix[--rest];
    if (c == '-')
        node->type = CAT;
    else if (c == '*') {
        node->type = STAR;
        node->child[1] = NULL;
    } else if (c == '|')
        node->type = OR;
    else if (c == '#') {
        node->type = END;
        node->value = c;
        node->child[0] = NULL;
        node->child[1] = NULL;
    } else {
        node->type = CHAR;
        node->value = c;
        node->child[0] = NULL;
        node->child[1] = NULL;
    }
    node->parent = NULL;

    if (node->type == CHAR || node->type == END) {
        *pleft_len = rest;
        goto children_done;
    } else {
        if (node->type != STAR) {
            child[1] = suffix_to_grammar_tree(suffix, rest, &rest);
            if (child[1]) {
                node->child[1] = child[1];
                child[1]->parent = node;
            }
            if (!rest)
                goto children_done;
        }

        child[0] = suffix_to_grammar_tree(suffix, rest, &rest);
        if (child[0]) {
            node->child[0] = child[0];
            child[0]->parent = node;
        }
        if (!rest)
            goto children_done;

        *pleft_len = rest;
        goto children_done;
    }

children_done:
    left = node->child[0];
    right = node->child[1];
    switch (node->type) {
        case CHAR:
        case END:
            node->pos = newpos();
            node->nullable = 0;
            node->subpos_max = node->pos;
            assert(!node->firstpos);
            node->firstpos = new_bitmap(node->subpos_max);
            node->lastpos = new_bitmap(node->subpos_max);
            bitmap_set_bit(node->firstpos, node->pos);
            bitmap_set_bit(node->lastpos, node->pos);
            pos_to_grammar_tree_node[node->pos] = node;
            if (node->type == END)
                accept_position = node->pos;
            break;
        case STAR:
            node->nullable = 1;
            node->subpos_max = left->subpos_max;
            assert(!node->firstpos);
            node->firstpos = new_bitmap_cpy(left->firstpos);
            node->lastpos = new_bitmap_cpy(left->lastpos);
            for_each_set_bit(pos, node->lastpos) {
                bitmap_set_bm(followpos_tbl[pos], node->firstpos);
            }
            break;
        case CAT:
            node->nullable = left->nullable && right->nullable;
            assert(!node->firstpos);
            if (left->nullable) {
                node->subpos_max = left->subpos_max > right->subpos_max ?
                    left->subpos_max : right->subpos_max;
                node->firstpos = new_bitmap(node->subpos_max);
                node->lastpos = new_bitmap(node->subpos_max);
                bitmap_or(node->firstpos, left->firstpos, right->firstpos);
                bitmap_or(node->lastpos, left->lastpos, right->lastpos);
            } else {
                node->subpos_max = left->subpos_max;
                node->firstpos = new_bitmap(node->subpos_max);
                node->lastpos = new_bitmap(node->subpos_max);
                bitmap_set_bm(node->firstpos, left->firstpos);
                bitmap_set_bm(node->lastpos, right->lastpos);
            }
            for_each_set_bit(pos, left->lastpos) {
                bitmap_set_bm(followpos_tbl[pos], right->firstpos);
            }
            break;
        case OR:
            node->nullable = left->nullable || right->nullable;
            node->subpos_max = left->subpos_max > right->subpos_max ?
                left->subpos_max : right->subpos_max;
            assert(!node->firstpos);
            node->firstpos = new_bitmap(node->subpos_max);
            node->lastpos = new_bitmap(node->subpos_max);
            bitmap_or(node->firstpos, left->firstpos, right->firstpos);
            bitmap_or(node->lastpos, left->lastpos, right->lastpos);
            break;
        default: break;
    }

    return node;
}

struct grammar_tree_node *generate_grammar_tree(char *regex)
{
    static char suffix_expr[4096];
    int ret, dummy, i, pos_nr = 0;
    char c;

    ret = to_suffix_expr(regex, suffix_expr, 4096 - 2);
    if (ret)
        error("Failed to get suffix expression.\n");

    for (i = 0; (c = suffix_expr[i]); ++ i)
        if (isalpha(c))
            ++ pos_nr;
    strcat(suffix_expr, "#-");
    ++ pos_nr;
    pos_to_grammar_tree_node =
        malloc((pos_nr + 1) * sizeof(struct grammar_tree_node *));
    if (!pos_to_grammar_tree_node)
        error("malloc pos_to_grammar_tree_node table failed.\n");

    followpos_tbl = malloc((pos_nr + 1) * sizeof(struct bitmap *));
    if (!followpos_tbl)
        error("malloc followpos_tbl failed.\n");
    for (i = 1; i <= pos_nr; ++ i) {
        followpos_tbl[i] = new_bitmap(pos_nr);
        if (!followpos_tbl[i])
            error("malloc followpos failed.\n");
    }

    return suffix_to_grammar_tree(suffix_expr, strlen(suffix_expr), &dummy);
}

struct dfa_state_internal {
    struct bitmap *positions;
    struct dfa_state_internal **trans_map;
    int acceptable;
    struct dfa_state_internal *next;
};

static struct dfa_state_internal *new_dfa_state_internal(struct bitmap *positions)
{
    struct dfa_state_internal *state;
    state = malloc(sizeof(*state));
    if (!state)
        error("malloc dfa_state_internal failed.\n");
    state->positions = new_bitmap_cpy(positions);
    state->trans_map = malloc(get_symbol_nr(&symbol_tbl) * sizeof(state));
    if (!state->trans_map)
        error("malloc state trans_map failed.\n");
    memset(state->trans_map, 0, get_symbol_nr(&symbol_tbl) * sizeof(state));
    state->acceptable = 0;
    state->next = NULL;
    return state;
}

static void free_dfa_state_internal(struct dfa_state_internal* state)
{
    free(state->positions);
    free(state->trans_map);
    free(state);
}

static void dump_trans(struct dfa_state_internal* new,
        struct dfa_state_internal* old, char c)
{
    int pos;
    printf("[ ");
    for_each_set_bit(pos, old->positions)
        printf("%d ", pos);
    printf("] -%c> ", c);
    printf("[ ");
    if (new) {
        for_each_set_bit(pos, new->positions)
            printf("%d ", pos);
        printf("]%s\n", new->acceptable ? "o" : "");
    } else {
        printf(" ]\n");
    }
}

struct dfa_state_internal *generate_dfa(struct grammar_tree_node *tree)
{
    struct dfa_state_internal *unmarked_list = NULL;
    struct dfa_state_internal *marked_list = NULL;
    struct dfa_state_internal *init_state;
    struct dfa_state_internal *state;
    struct bitmap *nextpos = new_bitmap(followpos_tbl[1]->size_in_bit);
    struct dfa_state_internal *newstate;
    int c_plus1;

    state = new_dfa_state_internal(tree->firstpos);
    state->next = unmarked_list;
    unmarked_list = state;
    init_state = state;

    while (1) {
        // Go on if there is unmarked state.
        state = unmarked_list;
        if (!state)
            break;
        unmarked_list = state->next;

        state->next = marked_list;
        marked_list = state;

        for_each_symbol_plus_one_in_table(c_plus1, &symbol_tbl) {
            newstate = NULL;
            int c = c_plus1 - 1;
            bitmap_reset(nextpos);
            // Calculate next positions with input symbol c in current state.
            int pos;
            for_each_set_bit(pos, state->positions) {
                if (pos_to_grammar_tree_node[pos]->value == c) {
                    bitmap_set_bm(nextpos, followpos_tbl[pos]);
                }
            }

            if (!is_bitmap_empty(nextpos)) {
                newstate = new_dfa_state_internal(nextpos);
                int marked = 0;
                struct dfa_state_internal *s;
                // Is new state we got already marked ?
                // TODO use hash method
                for (s = marked_list; s; s = s->next)
                    if (bitmap_eq(s->positions, newstate->positions)) {
                        free_dfa_state_internal(newstate);
                        newstate = s;
                        marked = 1;
                        break;
                    }
                if (!marked) {
                    // Is new state we got not marked but already existing ?
                    // TODO use hash method
                    for (s = unmarked_list; s; s = s->next)
                        if (bitmap_eq(s->positions, newstate->positions)) {
                            free_dfa_state_internal(newstate);
                            newstate = s;
                            break;
                        }
                    newstate->next = unmarked_list;
                    unmarked_list = newstate;
                }
                newstate->acceptable =
                    is_bit_set(newstate->positions, accept_position);
            }
            //            c
            // Add state ---> newstate to state transaction map.
            state->trans_map[c] = newstate;
            dump_trans(newstate, state, c);
        }
    }

    return init_state;
}

int dfa_match(struct dfa_state_internal *init_state, const char *str)
{
    int i = 0;
    struct dfa_state_internal *state = init_state;

    printf("\nstart matching\n");
    while (1) {
        if (!state)
            return 0;
        if (!str[i])
            return state->acceptable;
        dump_trans(state->trans_map[str[i]], state, str[i]);
        state = state->trans_map[str[i++]];
    }
}

typedef void (*gram_tree_node_func_t)(struct grammar_tree_node *node);

int traver_grammar_tree(struct grammar_tree_node *tree,
        gram_tree_node_func_t node_func)
{
    assert(tree);

    if (tree->child[0])
        traver_grammar_tree(tree->child[0], node_func);

    if (tree->child[1])
        traver_grammar_tree(tree->child[1], node_func);

    node_func(tree);

    return 0;
}

static inline void diagram_putc(char c)
{
    putchar(c);
}

static inline void diagram_puts(const char *s)
{
    fputs(s, stdout);
}

#define LEFT_1ST  0x00000001
#define EXT_SYTLE 0x00000002

int draw_tree(struct grammar_tree_node *tree, uint32_t flag)
{
    static char prefix[1024] = {0};
    int pos = strlen(prefix);
    int c1, c2;

    if (flag & LEFT_1ST)
        c2 = !(c1 = 0);
    else
        c1 = !(c2 = 0);

    if (flag & EXT_SYTLE) {
        if (tree->parent) {
            if (tree->parent->child[c1] == tree || tree->parent->type == STAR) {
                diagram_puts("───");
            } else {
                diagram_putc('\n');
                diagram_puts(prefix);
                diagram_puts("│\n");
                diagram_puts(prefix);
            }
        }
    } else {
        if (tree->parent) {
            diagram_puts(prefix);
            if (tree->parent->child[c1] == tree && tree->parent->type != STAR)
                diagram_puts("├── ");
            else
                diagram_puts("└── ");
        }
    }

    switch (tree->type) {
        case CHAR:
        case END: printf("%c:%d", tree->value, tree->pos); break;
        case CAT: diagram_putc('.'); break;
        case OR: diagram_putc('|'); break;
        case STAR: diagram_putc('*'); break;
        default: break;
    }

    if (flag & EXT_SYTLE) {
        if (tree->type == CHAR || tree->type == END) {
            return 0;
        }
        if (tree->child[c1])
            strcat(prefix, "│   ");
        if (tree->type == STAR) {
            prefix[pos] = '\0';
            strcat(prefix, "    ");
        }
        if (tree->child[c1])
            draw_tree(tree->child[c1], flag);
        if (tree->type != STAR)
            prefix[pos] = '\0';
        if (tree->child[c2])
            draw_tree(tree->child[c2], flag);
        if (!tree->parent)
            diagram_putc('\n');
        return 0;
    } else {
        diagram_putc('\n');
        if (tree->type == CHAR || tree->type == END)
            return 0;

        if (tree->child[c1]) {
            if (tree->parent) {
                if (tree->parent->child[c1] == tree && tree->parent->type != STAR)
                    strcat(prefix, "│   ");
                else
                    strcat(prefix, "    ");
            }
            draw_tree(tree->child[c1], flag);
        }

        if (tree->child[c2]) {
            if (tree->parent)
                if (tree->type == STAR) {
                    if (tree->parent->child[c1] == tree)
                        strcat(prefix, "│   ");
                    else
                        strcat(prefix, "    ");
                }
            draw_tree(tree->child[c2], flag);
        }

        prefix[pos] = '\0';

        return 0;
    }
}

void print_node(struct grammar_tree_node *node)
{
    assert(node);

    char *type;
    switch (node->type) {
        case CHAR: type = "CHAR"; break;
        case CAT: type = "CAT"; break;
        case STAR: type = "STAR"; break;
        case OR: type = "OR"; break;
        default: break;
    }
    printf("node type: %s", type);
    if (node->type == CHAR)
        printf(": %c\n", node->value);
    else
        printf("\n");
}

int main(int argc, char **argv)
{
    struct grammar_tree_node *tree;
    struct dfa_state_internal *init_state;

    if (argc != 3)
        error("usage:\n"
              "%s <regex> <string>\n", argv[0]);

    symbol_table_init(&symbol_tbl);

    tree = generate_grammar_tree(argv[1]);
    if (!tree)
        return 1;

    draw_tree(tree, 0);
    putchar('\n');
    draw_tree(tree, EXT_SYTLE);

    init_state = generate_dfa(tree);

    if (dfa_match(init_state, argv[2])) {
        printf("match\n");
        return 0;
    } else {
        printf("not match\n");
        return 1;
    }
}
