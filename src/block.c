#include "block.h"
#include "config.h"


void block_init(struct Block *block)
{
    /* Set defaults, is called only once */
    block->text = strdup("");
    block->text_len = 1;
    block->text_prev = strdup("");
}

void block_reset(struct Block *block)
{
    free(block->text);
    block->text = strdup("");
    block->text_len = 1;
}

void block_print(struct Block *block, bool last)
{
    /* Print block to STDOUT */
    // don't print block if empty
    if (strlen(block->text) <= 0)
        return;

    // strip comma + newline if this is last block
    if (last) {
        char *tmp = strdup(block->text);
        tmp[strlen(block->text) -2] = '\0';
        printf("%s", tmp);
        free(tmp);
    }
    else {
        printf("%s", block->text);
    }
}

bool block_is_elapsed(struct Block *block)
{
    /* check if timeout has elapsed, reset time of so */
    uint32_t t_cur = time(NULL);

    if ((t_cur - block->t_last) > block->timeout) {
        block->t_last = t_cur;
        return true;
    }
    return false;
}

bool block_is_changed(struct Block *block)
{
    /* Return true if block text has changed */
    if (strcmp(block->text, block->text_prev) == 0)
        return false;

    free(block->text_prev);
    block->text_prev = strdup(block->text);
    return true;
}

void block_set_error(struct Block *block, char* msg)
{
    block_set_text(block, msg, CS_ERROR, true);
}

void block_set_text(struct Block *block, char *text, char *color, bool separator)
{
    /* Clears and frees text in block, then sets new text */
    block_reset(block);
    block_add_text(block, text, color, separator);
}

void block_add_text(struct Block *block, char *text, char *color, bool separator)
{
    /* Append text to block */
    block->text_len = i3ify_alloc(block, text, color);

    if (separator)
        block_add_text(block, block->sep_chr, CS_NORMAL, false);
}

void block_set_graph(struct Block *block, uint8_t len, uint8_t perc, char* col)
{
    /* Set formatted graph in block */
    char graph_chr1 = GRAPH_CHAR_LEFT;
    char graph_chr2 = GRAPH_CHAR_RIGHT;
    char l_text[BLOCK_MAX_GRAPH_BUF] = {'\0'};
    char r_text[BLOCK_MAX_GRAPH_BUF] = {'\0'};
    uint8_t i;
    len++;

    if (perc > 100)
        perc = 100;

    int8_t level = (perc / 100.0) * len;

    for (i=0 ; i<level ; i++)
        l_text[i] = graph_chr1;
    for (i=0 ; i<len-level ; i++)
        r_text[i] = graph_chr2;

    block->text[0] = '\0';
    block_set_text(block, l_text, col, false);
    block_add_text(block, r_text, CS_NORMAL, true);
}

void block_set_strgraph(struct Block *block, char* str, uint8_t perc, char* col)
{
    /* Set formatted graph in block */
    uint8_t len = strlen(str);
    char l_text[BLOCK_MAX_GRAPH_BUF] = {'\0'};
    char r_text[BLOCK_MAX_GRAPH_BUF] = {'\0'};
    uint8_t index = 0;
    uint8_t i;

    if (perc > 100)
        perc = 100;

    int8_t level = (perc / 100.0) * len;

    for (i=0 ; i<level ; i++) {
        l_text[i] = str[index];
        index++;
    }
    for (i=0 ; i<len-level ; i++) {
        r_text[i] = str[index];
        index++;
    }
    block->text[0] = '\0';
    block_set_text(block, l_text, col, false);
    block_add_text(block, r_text, CS_NORMAL, true);
}
