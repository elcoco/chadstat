#include "blocks.h"


bool is_elapsed(block_t* block) {
    // check if time has elapsed, reset time of so
    uint32_t t_cur = time(NULL);

    if ((t_cur - block->t_last) > block->timeout) {
        block->t_last = t_cur;
        printf("elapsed\n");
        return true;
    }
    return false;
}

void set_changed(block_t* block) {
    // set flag if value has changed
    if (strcmp(block->text, block->text_prev) == 0) {
        block->is_changed = false;
    }
    else {
        block->is_changed = true;
        strcpy(block->text_prev, block->text);
    }
}



void get_graph(block_t* block, uint8_t graph_len, uint8_t percent, char* color) {
    if (percent > 100)
        percent = 100;
    int8_t level = (percent / 100.0) * graph_len;

    char graph_chr1 = '|';
    char graph_chr2 = '|';

    graph_len = 20;

    char l_text[21] = {'\0'};
    char r_text[21] = {'\0'};
    //char l_text[graph_len+1] = {'\0'};
    //char r_text[graph_len+1] = {'\0'};

    for (uint8_t i=0 ; i<level ; i++) {
        l_text[i] = graph_chr1;
    }
    for (uint8_t i=0 ; i<graph_len-level ; i++) {
        r_text[i] = graph_chr2;
    }

    // handle case where level is maximum graph_len, second line would be empty and therefore skipped and screw up spacing between blocks (see i3 input protocol)
    if (level == graph_len)
        sprintf(block->text, "%s%s", color, l_text);
    else
        sprintf(block->text, "%s%s%s%s", color, l_text, CS_NORMAL, r_text);
}

void get_strgraph(block_t* block, char* str, uint8_t percent, char* color) {
    if (percent > 100)
        percent = 100;

    uint8_t graph_len = strlen(str);
    int8_t level = (percent / 100.0) * graph_len;

    //char l_text[graph_len+1] = {'\0'};
    //char r_text[graph_len+1] = {'\0'};

    char l_text[50] = {'\0'};
    char r_text[50] = {'\0'};

    uint8_t index = 0;
    for (uint8_t i=0 ; i<level ; i++) {
        l_text[i] = str[index];
        index++;
    }
    for (uint8_t i=0 ; i<graph_len-level ; i++) {
        r_text[i] = str[index];
        index++;
    }

    // handle case where level is maximum graph_len, second line would be skipped and screw up spacing between blocks (see i3 input protocol)
    if (level == graph_len)
        sprintf(block->text, "%s%s", color, l_text);
    else
        sprintf(block->text, "%s%s%s%s", color, l_text, CS_NORMAL, r_text);
}

void get_datetime(block_t* block) {
    if (! is_elapsed(block)) {
        block->is_changed = false;
        return;
    }

    time_t t = time(NULL);            // 32bit integer representing time
    struct tm tm = *localtime(&t);    // get struct with time data
    char buffer[80];

    strftime(buffer, 100, DATETIME_FMT, &tm);
    sprintf(block->text, "%s%s", COL_DATETIME, buffer);
    set_changed(block);
}
