1. mtx_init(mtx_t *mtx, int type)
注释掉了
#ifdef VXWOKRS_7
if ((type & mtx_recursive) != 0)
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#endif

2. 为了编译有些make增加了 -DHAVE_LIBDRM

3. gfxItlGmcDrv.h undef GFX_USE_PMAP