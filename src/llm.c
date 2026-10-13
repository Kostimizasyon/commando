#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <curl/curl.h>
#include <ctype.h>

#include "../cJSON/cJSON.h"
#include "llm.h"

#define API_URL        "https://api.openai.com/v1/chat/completions"
#define FALLBACK_MODEL "gpt-6-luna"

typedef struct {
    const char *inp;
    const char *api_key;
    const char *model;
    int max_tokens;          
} inp_req;

/* state saved from the most recent request */
static int  last_in = 0, last_out = 0;
static long rl_tokens = -1, rl_requests = -1;   /* -1 = not returned */

struct buf { char *data; size_t len; };

static size_t write_cb(char *ptr, size_t size, size_t nmemb, void *ud) {
    struct buf *b = ud;
    size_t n = size * nmemb;
    char *tmp = realloc(b->data, b->len + n + 1);
    if (!tmp) return 0;
    b->data = tmp;
    memcpy(b->data + b->len, ptr, n);
    b->len += n;
    b->data[b->len] = '\0';
    return n;
}

/* called once per response header line */
static size_t header_cb(char *p, size_t size, size_t nmemb, void *ud) {
    (void)ud;
    size_t len = size * nmemb;
    if (!strncasecmp(p, "x-ratelimit-remaining-tokens:", 29))
        rl_tokens = atol(p + 29);
    else if (!strncasecmp(p, "x-ratelimit-remaining-requests:", 31))
        rl_requests = atol(p + 31);
    return len;
}

/* returns malloc'd reply text (caller frees) or NULL on failure */
static char *send_request(inp_req req) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "model", req.model);
    if (req.max_tokens > 0)
        cJSON_AddNumberToObject(root, "max_completion_tokens", req.max_tokens);

    cJSON *msgs = cJSON_AddArrayToObject(root, "messages");
    cJSON *sys = cJSON_CreateObject();
    cJSON_AddStringToObject(sys, "role", "system");
    cJSON_AddStringToObject(sys, "content", INIT_PROMPT);
    cJSON_AddItemToArray(msgs, sys);
    cJSON *usr = cJSON_CreateObject();
    cJSON_AddStringToObject(usr, "role", "user");
    cJSON_AddStringToObject(usr, "content", req.inp);
    cJSON_AddItemToArray(msgs, usr);
    char *body = cJSON_PrintUnformatted(root);

    char auth[512];
    snprintf(auth, sizeof auth, "Authorization: Bearer %s", req.api_key);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, auth);
    headers = curl_slist_append(headers, "Content-Type: application/json");

    struct buf resp = { NULL, 0 };
    char *out = NULL;

    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, API_URL);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_cb);

    CURLcode rc = curl_easy_perform(curl);
    if (rc != CURLE_OK) {
        fprintf(stderr, "curl error: %s\n", curl_easy_strerror(rc));
    } else {
        cJSON *j = cJSON_Parse(resp.data);
        cJSON *err = cJSON_GetObjectItem(j, "error");
        if (!j || err) {
            fprintf(stderr, "API error:\n%s\n", resp.data ? resp.data : "(empty)");
        } else {
            cJSON *usage = cJSON_GetObjectItem(j, "usage");
            cJSON *in  = cJSON_GetObjectItem(usage, "prompt_tokens");
            cJSON *ot  = cJSON_GetObjectItem(usage, "completion_tokens");
            if (cJSON_IsNumber(in)) last_in  = in->valueint;
            if (cJSON_IsNumber(ot)) last_out = ot->valueint;

            cJSON *choice  = cJSON_GetArrayItem(cJSON_GetObjectItem(j, "choices"), 0);
            cJSON *message = cJSON_GetObjectItem(choice, "message");
            cJSON *content = cJSON_GetObjectItem(message, "content");
            out = strdup(cJSON_IsString(content) ? content->valuestring : "");
        }
        cJSON_Delete(j);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    cJSON_Delete(root);
    free(body);
    free(resp.data);
    return out;
}

/* C has no default arguments: pass NULL to use FALLBACK_MODEL.
   Returns malloc'd text (caller frees) or NULL on error. */
void
llm_out(const char *inp, const char *api_key, const char *model, int max_tokens) {
    if (!model) model = FALLBACK_MODEL;
    inp_req req = { inp, api_key, model, 0 };

    char *out = send_request(req);
    if (!out) return;          // error already printed to stderr

    puts(out);
    free(out);                 // send_request returns malloc'd memory
}

/* shows REMAINING RATE LIMIT from the response headers (see notes) */
void
print_token_info(const char *api_key) {
    inp_req req = { "hi", api_key, FALLBACK_MODEL, 1 };   /* tiny request, 1 output token */
    char *ignored = send_request(req);
    free(ignored);

    puts("Rate limit status for this key/model:");
    if (rl_tokens >= 0)   printf("  tokens left (this minute):   %ld\n", rl_tokens);
    else                  puts("  tokens left: not returned");
    if (rl_requests >= 0) printf("  requests left (this minute): %ld\n", rl_requests);
    else                  puts("  requests left: not returned");
}

/* cost of the LAST request, printed after a bar */
void
print_price_info(void) {
    puts("\n==============================================");
    printf("tokens: %d in, %d out |\n", last_in, last_out);
}

void str_lower(char *s) {
  for (; *s; s++)
    *s = tolower((unsigned char)*s);
}

void
match_model(char* model) {

    str_lower(model);

    if (strcmp(model, "astra")) {
        printf("No do not do this");
        model = NULL;
    }
    else if (strcmp(model, "sol")) {
        printf("\033[31mWill use a lot of tokens ngl\033[0m\n");
        model = "gpt-6.1-sol";
    }
    else if (strcmp(model, "luna")) {
        model = "gpt-6-luna";
    }
    else {
        model = NULL;
    }

}

