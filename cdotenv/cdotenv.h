#ifdef __cplusplus
extern "C" {
#endif

#define SUCESS          0
#define FILE_NOT_FOUND -1
#define KEY_OVERFLOW   -3
#define VALUE_OVERFLOW -5
#define SET_ENV_FAIL   -7
#define WRITE_FAIL     -9
#define CLOSE_FAIL     -11

#define KEY_BUF_SIZE 256
#define VALUE_BUF_SIZE 512

// read the env values and add them to the  enviroment to be got with getenv
int read_env(const char* path);

#ifdef __cplusplus
}
#endif
