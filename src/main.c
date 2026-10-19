#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../env.h"
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
  char* inp_buf = NULL;
  char* active_model = NULL;

  bool flags[FLAG_COUNT] = {0};

  for (i = 1; i < argc; i++) {
    res = match_arg(argv[i]);

    if (res == ERROR) {
      puts("Bad args, pass -h or --help for help");
      return -1;
    }

    if (res == INPUT) {

      if ( argv[i][0] == '\"' ) {
          size_t inp_size = strlen(argv[i]);
          if (inp_size > INP_BUF_SIZE) {
            printf("Too large input, max %d chars", INP_BUF_SIZE - 2);
            return -1;
          }
          inp_buf = argv[i];
      }

      else {
        
        inp_buf = malloc(INP_BUF_SIZE);
        if (!inp_buf) return -1;

        // initilizing
        inp_buf[0] = '\0';

        for ( i = i; i < argc ; i++) {
            if (match_arg(argv[i]) != INPUT) {
              break;
            }

          size_t used = strlen(inp_buf);
          size_t need = strlen(argv[i]) + (used ? 1 : 0) + 1;

          if (used + need > INP_BUF_SIZE)
              break;

          if (used) strcat(inp_buf, " ");
            strcat(inp_buf, argv[i]);
          }
      }

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
    if (!flags[res]) {
      flags[res] = true;
    }
    else {
      puts("Bad args, pass -h or --help for help");
      return -1;
    }
  }
  // match flags
  if (flags[HELP]) {
    print_help();
    return 0;
  }

  res = read_env(ENV_PATH);

  if (res != SUCESS) {
    perror("Something went wrong with reading the env file.");
    return -1;
  }
  
  char* api_key = getenv("API_KEY");

  if (api_key == NULL) {
    perror("Unable to get API key env variable, check readme for info on setting up a .env file");
    return -1;
  }

  else if (flags[TOKEN]) {
    print_token_info(api_key);
    return 0;
  } 

  else if (flags[QUESTION] ^ flags[CODE]) {
    const char *flag = flags[QUESTION] ? "-q " : "-c ";
    size_t curr = strlen(inp_buf);
    size_t need = strlen(flag);

    if (curr + need < INP_BUF_SIZE) {
        memmove(inp_buf + need, inp_buf, curr + 1);  // +1 moves the '\0'
        memcpy(inp_buf, flag, need);
    }

    llm_out(inp_buf, api_key, active_model, MAX_TOKEN);
}

  else {
      perror("Bad args, pass -h or --help for help");
      return -1;
  }

  if (flags[PRICE]) {
    print_price_info();
  }

  if (inp_buf) free(inp_buf);

  return 0;
}
