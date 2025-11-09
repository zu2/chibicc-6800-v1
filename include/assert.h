#if defined(NDEBUG)
#define #define assert(ignore)((void) 0)
#else
#define assert(expr) \
    ((expr) ? (void)0 : __assert_fail(#expr, __FILE__, __LINE__, __func__))
#endif
