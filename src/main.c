#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cli.h"
#include "llm.h"

#include "../cdotenv/cdotenv.h"

int main(int argc, char **argv) {

  if (argc == 1) {
      print_help();
      return -1;
  }

  int i;
  int res;
  char*inp_buf;
  char* active_model = NULL;

  bool flags[FLAG_COUNT] = {0};

  for (i = 1; i < argc; i++) {
    res = match_arg(argv[i]);

    if (res == ERROR) {
      puts("Bad args, pass -h or --help for help");
      return -1;
    }
    
    if (res == INPUT) {
      size_t inp_size = sizeof(argv[i]);
      if (inp_size > INP_BUF_SIZE) {
        printf("Too large input, max %d chars", INP_BUF_SIZE);
        return -1;
      }
      inp_buf = argv[i];
    }

    else if (res == MODEL) {
      if (i + 1 >= argc) {
        puts("Model flag needs a model name");
        return -1;
      }
      active_model = argv[++i];
      match_model(active_model);
      if (active_model == NULL) {
        printf("Wrong model name, -h to see all options");
        return -1;
      }
    }

    // turn on whichever flag is passed
    flags[res] = true;
  }

  // match flags
  if (flags[HELP]) {
    print_help();
    return 0;
  }

  read_env(".env.local");

  char* api_key = getenv("API_KEY");

  if (api_key == NULL) {
    puts("Unable to get API key env variable, check readme for info");
    return -1;
  }
  else if (flags[TOKEN]) {
    print_token_info(api_key);
    return 0;
  } 
  else {
    llm_out(inp_buf,api_key,active_model,1000);
  }

  if (flags[PRICE]) {
    print_price_info();
  }

}
