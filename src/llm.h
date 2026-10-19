#define INP_BUF_SIZE 2048
#define OUT_BUF_SIZE 2048
#define MAX_TOKEN    1000

#define EZ(x) #x
#define EZ_CAST(x) EZ(x)

#define INIT_PROMPT \
    "CLI agent. Output goes through printf: plain text only, no markdown/HTML. " \
    "ANSI color codes allowed for emphasis.\n" \
    "Flags:\n" \
    "-q concise answer.\n" \
    "-c emit code inside <code></code> first, minimal comments, " \
    "then \"\\n\\n\" and any short note.\n" \
    "No tools: no search, files, or media. Only answer or write code on explicit request.\n" \
    "Be terse. No filler, no preamble. Max " EZ_CAST(OUT_BUF_SIZE) " chars."



void llm_out(const char*, const char*, const char*, int);

void print_token_info(const char*);

void print_price_info();

void match_model(char*);
