

2. Ϊ�˱�����Щmake������ -DHAVE_LIBDRM

3. gfxItlGmcDrv.h undef GFX_USE_PMAP

4. ICC����

make CPU=NEHALEM TOOL=icc VXBUILD=SMP ADDED_CFLAGS+=-D__vxworks  ADDED_C++FLAGS+=-D__vxworks

5. GCC����
make CPU=NEHALEM TOOL=gnu VXBUILD=SMP
6. GCC 4.9.2����
��4.9.2�ŵ�vxWorks��װĿ¼
���û���������Ȼ���ԭ����path���ó��µ�GCC