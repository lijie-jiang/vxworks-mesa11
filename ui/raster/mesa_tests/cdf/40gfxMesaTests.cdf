/* 40gfxMesaTests.cdf - Mesa tests CDF file */

/*
 * Copyright (c) 2013-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */

/*
modification history
--------------------
14sep15,yat  Add support for Mesa GPU DRI (US24710)
22dec14,yat  Create test for Mesa (US24712)
*/

Folder FOLDER_MESA_TESTS {
    NAME            Mesa tests components
    SYNOPSIS        Mesa tests
    DEFAULTS        INCLUDE_MESA_MAIN_TEST
    _CHILDREN       FOLDER_RASTER_MESA
}

Component INCLUDE_MESA_TEXTURING_TEST {
    NAME            OpenGL texturing test
    SYNOPSIS        This component includes the OpenGL texturing test that can be started from the VxWorks shell by executing "gfxMesaTexturingTestStart" for software rendering or "gfxMesaDriTexturingTestStart" for GPU rendering if supported
    _CHILDREN       FOLDER_MESA_TESTS
    ARCHIVE         libgfxMesaTests.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaTexturingTestStart
}

Component INCLUDE_MESA_POLYGON_TEST {
    NAME            OpenGL polygon test
    SYNOPSIS        This component includes the OpenGL polygon test that can be started from the VxWorks shell by executing "gfxMesaPolygonTestStart" for software rendering or "gfxMesaDriPolygonTestStart" for GPU rendering if supported
    _CHILDREN       FOLDER_MESA_TESTS
    ARCHIVE         libgfxMesaTests.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaPolygonTestStart
}

#if defined(_WRS_CONFIG_MESA_GPUDEV_DRI_I965)
Component INCLUDE_MESA_ES2MULTIGEAR_TEST {
    NAME            OpenGL ES2 multi gear test
    SYNOPSIS        This component includes the OpenGL ES2 multi gear test that can be started from the VxWorks shell by executing "gfxMesaDriEs2MultiGearTestStart" for GPU rendering
    _CHILDREN       FOLDER_MESA_TESTS
    ARCHIVE         libgfxMesaTests.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaDriEs2MultiGearTestStart
}

Component INCLUDE_MESA_ES2MULTISHADER_TEST {
    NAME            OpenGL ES2 multi shader test
    SYNOPSIS        This component includes the OpenGL ES2 multi shader test that can be started from the VxWorks shell by executing "gfxMesaDriEs2MultiShaderTestStart" for GPU rendering
    _CHILDREN       FOLDER_MESA_TESTS
    ARCHIVE         libgfxMesaTests.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaDriEs2MultiShaderTestStart
}

Component INCLUDE_MESA_ES2POLYGON_TEST {
    NAME            OpenGL ES2 polygon test
    SYNOPSIS        This component includes the OpenGL ES2 polygon test that can be started from the VxWorks shell by executing "gfxMesaDriEs2PolygonTestStart" for GPU rendering
    _CHILDREN       FOLDER_MESA_TESTS
    ARCHIVE         libgfxMesaTests.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaDriEs2PolygonTestStart
}

Component INCLUDE_MESA_ES2TEXTURING_TEST {
    NAME            OpenGL ES2 texturing test
    SYNOPSIS        This component includes the OpenGL ES2 texturing test that can be started from the VxWorks shell by executing "gfxMesaDriEs2TexturingTestStart" for GPU rendering
    _CHILDREN       FOLDER_MESA_TESTS
    ARCHIVE         libgfxMesaTests.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaDriEs2TexturingTestStart
}
#endif

Component INCLUDE_MESA_MAIN_TEST {
    NAME            Mesa OpenGL main test
    SYNOPSIS        This component includes the Mesa OpenGL main test that can be started from the VxWorks shell by executing "gfxMesaMainTestStart" for software rendering or "gfxMesaDriMainTestStart" for GPU rendering if supported
    _CHILDREN       FOLDER_MESA_TESTS
    ARCHIVE         libgfxMesaTests.a
#if defined(_WRS_CONFIG_RASTER_MESA_DEMOS)
    REQUIRES        INCLUDE_MESA_POLYGON_TEST \
                    INCLUDE_MESA_TEXTURING_TEST \
                    INCLUDE_MESA_SWAP_DEMO \
                    INCLUDE_MESA_GEAR_DEMO
#else
    REQUIRES        INCLUDE_MESA_POLYGON_TEST \
                    INCLUDE_MESA_TEXTURING_TEST
#endif
    LINK_SYMS       gfxMesaMainTestStart
}

#if defined(_WRS_CONFIG_MESA_GPUDEV_DRI_I965)
Component INCLUDE_MESA_ESMAIN_TEST {
    NAME            Mesa OpenGL ES main test
    SYNOPSIS        This component includes the Mesa OpenGL ES main test that can be started from the VxWorks shell by executing "gfxMesaDriEsMainTestStart" for GPU rendering
    _CHILDREN       FOLDER_MESA_TESTS
    ARCHIVE         libgfxMesaTests.a
#if defined(_WRS_CONFIG_RASTER_MESA_DEMOS)
    REQUIRES        INCLUDE_MESA_ES2MULTIGEAR_TEST \
                    INCLUDE_MESA_ES2MULTISHADER_TEST \
                    INCLUDE_MESA_ES2POLYGON_TEST \
                    INCLUDE_MESA_ES2TEXTURING_TEST \
                    INCLUDE_MESA_SWAP_DEMO \
                    INCLUDE_MESA_GEAR_DEMO
#else
    REQUIRES        INCLUDE_MESA_ES2MULTIGEAR_TEST \
                    INCLUDE_MESA_ES2MULTISHADER_TEST \
                    INCLUDE_MESA_ES2POLYGON_TEST \
                    INCLUDE_MESA_ES2TEXTURING_TEST
#endif
    LINK_SYMS       gfxMesaDriEsMainTestStart
}
#endif
