

2. 为了编译有些make增加了 -DHAVE_LIBDRM

3. gfxItlGmcDrv.h undef GFX_USE_PMAP

4. ICC编译

make CPU=NEHALEM TOOL=icc VXBUILD=SMP ADDED_CFLAGS+=-D__vxworks  ADDED_C++FLAGS+=-D__vxworks

5. GCC编译
make CPU=NEHALEM TOOL=gnu VXBUILD=SMP
6. GCC 4.9.2编译
把4.9.2放到vxWorks安装目录
设置环境变量，然后把原来的path设置成新的GCC