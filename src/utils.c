#include "utils.h"
#include "config.h"


int8_t do_request(const char* url, long* response_code)
{
    CURL *curl = curl_easy_init();
    if(curl) {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, HTTP_MAX_TIMEOUT);

        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, response_code);
        curl_easy_cleanup(curl);
        return res;
    }
    return -1;
}

int strcat_alloc(char **dest, int old_size, char *buf)
{
    /* Grow/realloc dest and strncat buf to it
     * If old_size == 0, do initial allocation
     * If old_size  > 0, do realloc
     * If buf is empty do nothing
     * Returns new buffer size or -1 on error */
    if (strlen(buf) == 0 )
        return old_size;

    int new_size = old_size + strlen(buf) + 1;

    if (old_size == 0) {
        *dest = malloc(new_size);
        **dest = '\0';
    }
    else {
        DEBUG("doing realloc\n");
        DEBUG("  old size:   %d\n", old_size);
        DEBUG("  strlen old: %ld\n", strlen(*dest));
        DEBUG("  strlen new: %d\n", new_size);
        DEBUG("  to append:  %s\n", buf);
        *dest = realloc(*dest, new_size);
    }

    if (*dest == NULL) {
        //DEBUG("ERROR\n");
        return -1;
    }

    strncat(*dest, buf, strlen(buf));
    DEBUG("  result:        %s\n\n", *dest);
    DEBUG("  strlen result: %ld\n\n", strlen(*dest));
    return new_size;
}

int i3ify_alloc(struct Block *block, char *text, char *color)
{
    /* Append a formatted JSON string to dest
     * Allocate or extend space if necessary.
     */
    int tmp_size = strlen(block->name) + strlen(text) + strlen(color) + strlen(I3_FMT) + 1;
    char *tmp = malloc(tmp_size);
    sprintf(tmp, I3_FMT, block->name, text, color);
    int new_size = strcat_alloc(&(block->text), block->text_len, tmp);
    free(tmp);
    return new_size;
}
