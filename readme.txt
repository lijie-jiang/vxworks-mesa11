1. mtx_init(mtx_t *mtx, int type)
ע�͵���
#ifdef VXWOKRS_7
if ((type & mtx_recursive) != 0)
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#endif

2. Ϊ�˱�����Щmake������ -DHAVE_LIBDRM

3. gfxItlGmcDrv.h undef GFX_USE_PMAP