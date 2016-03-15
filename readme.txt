1. mtx_init(mtx_t *mtx, int type)
в╒йм╣Так
#ifdef VXWOKRS_7
if ((type & mtx_recursive) != 0)
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#endif