int msvc_printf(const char *format, ...);

#define __PRINTF(...)	msvc_printf("[" __FILE__ "]\t" __VA_ARGS__);

#ifdef DEBUG_SUBST
#define __DEBUG_SUBST(...)	__PRINTF(" " __VA_ARGS__)
#else
#define __DEBUG_SUBST(...)
#endif

#ifdef DEBUG_JOBS
#define __DEBUG_JOBS(...)	__PRINTF(" " __VA_ARGS__)
#else
#define __DEBUG_JOBS(...)
#endif

#ifdef DEBUG_EXIT
#define __DEBUG_EXIT(...)	__PRINTF(" " __VA_ARGS__)
#else
#define __DEBUG_EXIT(...)
#endif

#ifdef DEBUG
#define __DEBUG(...)		__PRINTF(__VA_ARGS__)
#else
#define __DEBUG(...)
#endif

extern const char const* __E_TYPE[];
extern const char const* __STATE[];
extern const char const* __L_TYPE[];
extern const char const* __T_TYPE[];
extern const char const* __C_TYPE[];
extern const char const* __X_TYPE[];

char *__X_FLAGS(int flags);
char *__JW_FLAGS(int flags);
