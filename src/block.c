#include "block.h"
#include "config.h"


static void block_print_separator(struct Block *block);

void block_init(struct Block *block)
{
    /* Set defaults, is called only once */
    block->text = strdup("");
    block->text_len = 1;
    block->text_prev = strdup("");
    block->instance[0] = '\0';
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
        block_print_separator(block);
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
    block_set_text(block, "error", msg, block->cs->error);
}

void block_set_text(struct Block *block, const char *instance, const char *text, const char *color)
{
    /* Clears and frees text in block, then sets new text */
    block_reset(block);
    block_add_text(block, instance, text, color);
}

void block_add_text(struct Block *block, const char *instance, const char *text, const char *color)
{
    /* Append text to block */
    char *text_escaped = get_escaped_alloc(text);
    block->text_len = i3ify_alloc(block, instance, text_escaped, color);
    free(text_escaped);
}

static void block_print_separator(struct Block *block)
{
    int tmp_size = strlen(block->name) + strlen(block->instance) + strlen(block->sep_chr) + strlen(block->cs->separator) + strlen(I3_FMT) + 1;
    char *tmp = malloc(tmp_size);
    sprintf(tmp, I3_FMT, block->name, block->instance, block->sep_chr, block->cs->separator);
    printf("%s", tmp);
    free(tmp);
}

void block_set_graph(struct Block *block, const char *instance, uint8_t len, uint8_t perc, const char* lcol, const char *rcol)
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

    //block->text[0] = '\0';
    block_add_text(block, instance, l_text, lcol);
    block_add_text(block, instance, r_text, rcol);
}

void block_set_strgraph(struct Block *block, const char *instance, char* str, uint8_t perc, const char *lcol, const char *rcol)
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
    //block->text[0] = '\0';
    block_add_text(block, instance, l_text, lcol);
    block_add_text(block, instance, r_text, rcol);
}


static void block_event_debug(struct BlockClickEvent *ev)
{
    printf("x:         %d\n", ev->x);
    printf("y:         %d\n", ev->y);
    printf("xsize:     %d\n", ev->xsize);
    printf("ysize:     %d\n", ev->ysize);
    printf("modifiers: ");
    print_bin(ev->mod);
}

int block_event_init(struct JSONObject *jo, struct BlockClickEvent *ev)
{
    /* Parse the JSON received by I3 and put data in struct */
    ev->mod = 0x00;

    if (jo == NULL)
        return -1;

    // Parse modifier keys from json
    struct JSONObject *modifiers_obj = json_get_path(jo, "modifiers");
    if (jo == NULL)
        return -1;

    struct JSONObject *mod = modifiers_obj->value;
    while (mod != NULL) {
        char *mod_str = json_get_string(mod);
        if (mod_str == NULL)
            continue;

        if (strcmp(mod_str, "Control") == 0)
            ev->mod = ev->mod | BLOCK_CTRL_PRESSED;
        else if (strcmp(mod_str, "Lock") == 0)
            ev->mod = ev->mod | BLOCK_LOCK_PRESSED;
        else if (strcmp(mod_str, "Shift") == 0)
            ev->mod = ev->mod | BLOCK_SHIFT_PRESSED;
        else if (strcmp(mod_str, "Mod1") == 0)
            ev->mod = ev->mod | BLOCK_MOD1_PRESSED;
        else if (strcmp(mod_str, "Mod2") == 0)
            ev->mod = ev->mod | BLOCK_MOD2_PRESSED;
        else if (strcmp(mod_str, "Mod3") == 0)
            ev->mod = ev->mod | BLOCK_MOD3_PRESSED;
        else if (strcmp(mod_str, "Mod4") == 0)
            ev->mod = ev->mod | BLOCK_MOD4_PRESSED;
        else if (strcmp(mod_str, "Mod5") == 0)
            ev->mod = ev->mod | BLOCK_MOD5_PRESSED;

        mod = mod->next;
    }

    // Parse mouse actions from json
    struct JSONObject *mouse_obj = json_get_path(jo, "button");
    if (mouse_obj == NULL)
        return -1;

    switch ((int)json_get_number(mouse_obj)) {
        case 1:
            ev->mod = ev->mod | BLOCK_LMB_PRESSED;
            break;
        case 2:
            ev->mod = ev->mod | BLOCK_MMB_PRESSED;
            break;
        case 3:
            ev->mod = ev->mod | BLOCK_RMB_PRESSED;
            break;
        case 4:
            ev->mod = ev->mod | BLOCK_MOUSE_SCROLL_UP;
            break;
        case 5:
            ev->mod = ev->mod | BLOCK_MOUSE_SCROLL_DOWN;
            break;
        default:
            DEBUG("Unknown input event: %lf\n", json_get_number(mouse_obj));
            break;
    }

    struct JSONObject *relx_obj = json_get_path(jo, "relative_x");
    if (relx_obj == NULL)
        return -1;

    struct JSONObject *rely_obj = json_get_path(jo, "relative_y");
    if (rely_obj == NULL)
        return -1;

    struct JSONObject *width_obj = json_get_path(jo, "width");
    if (width_obj == NULL)
        return -1;

    struct JSONObject *height_obj = json_get_path(jo, "height");
    if (height_obj == NULL)
        return -1;

    ev->x = (int)json_get_number(relx_obj);
    ev->y = (int)json_get_number(rely_obj);
    ev->xsize = (int)json_get_number(width_obj);
    ev->ysize = (int)json_get_number(height_obj);

    //block_event_debug(ev);
    struct JSONObject *instance_obj = json_get_path(jo, "instance");
    if (instance_obj == NULL)
        return -1;

    char *instance_str = json_get_string(instance_obj);
    if (instance_str == NULL)
        return -1;

    strcpy(ev->instance, instance_str);

    return 1;
}
