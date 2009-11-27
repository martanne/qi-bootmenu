
#define sstrlen(str) (sizeof(str) - 1)

#ifdef NDEBUG
 #define debug(format, args...)
#else
 #define debug eprint
#endif

void eprint(const char *errstr, ...); 
void skip_until(char** str, char c); 
void skip_spaces(char **str);
int fexecw(const char *path, char *const argv[], char *const envp[]);
