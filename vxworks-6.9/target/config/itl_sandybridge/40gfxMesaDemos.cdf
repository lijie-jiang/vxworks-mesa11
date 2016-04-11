/* 40gfxMesaDemos.cdf - Mesa demos CDF file */

/*
 * Copyright (c) 2013-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */

/*
modification history
--------------------
01mar16,yat  Port Mesa eglkms demo to VxWorks (US76256)
14sep15,yat  Add support for Mesa GPU DRI (US24710)
22dec14,yat  Create demo for Mesa (US24712)
*/

Folder FOLDER_MESA_DEMOS {
    NAME            Mesa demos components
    SYNOPSIS        Mesa demos
    DEFAULTS        INCLUDE_MESA_MAIN_DEMO INCLUDE_MESA_ESMAIN_DEMO
    DEFAULTS        INCLUDE_MESA_MAIN_DEMO
    _CHILDREN       FOLDER_RASTER_MESA
}

Component INCLUDE_MESA_SWAP_DEMO {
    NAME            OpenGL swap demo
    SYNOPSIS        This component includes the OpenGL swap demo that can be started from the VxWorks shell by executing "gfxMesaSwapDemoStart" for software rendering or "gfxMesaDriSwapDemoStart" for GPU rendering if supported
    _CHILDREN       FOLDER_MESA_DEMOS
    ARCHIVE         libgfxMesaDemos.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaSwapDemoStart
}

Component INCLUDE_MESA_FBO_DEMO {
    NAME            OpenGL framebuffer object demo
    SYNOPSIS        This component includes the OpenGL framebuffer object demo that can be started from the VxWorks shell by executing "gfxMesaFboDemoStart" for software rendering or "gfxMesaDriFboDemoStart" for GPU rendering if supported
    _CHILDREN       FOLDER_MESA_DEMOS
    ARCHIVE         libgfxMesaDemos.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaFboDemoStart
}

Component INCLUDE_MESA_GEAR_DEMO {
    NAME            OpenGL gear demo
    SYNOPSIS        This component includes the OpenGL gear demo that can be started from the VxWorks shell by executing "gfxMesaGearDemoStart" for software rendering or "gfxMesaDriGearDemoStart" for GPU rendering if supported
    _CHILDREN       FOLDER_MESA_DEMOS
    ARCHIVE         libgfxMesaDemos.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaGearDemoStart
}


Component INCLUDE_MESA_ES2SWAP_DEMO {
    NAME            OpenGL ES2 swap demo
    SYNOPSIS        This component includes the OpenGL ES2 swap demo that can be started from the VxWorks shell by executing "gfxMesaDriEs2SwapDemoStart" for GPU rendering
    _CHILDREN       FOLDER_MESA_DEMOS
    ARCHIVE         libgfxMesaDemos.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaDriEs2SwapDemoStart
}

Component INCLUDE_MESA_ES2FBO_DEMO {
    NAME            OpenGL ES2 framebuffer object demo
    SYNOPSIS        This component includes the OpenGL ES2 framebuffer object demo that can be started from the VxWorks shell by executing "gfxMesaDriEs2FboDemoStart" for GPU rendering
    _CHILDREN       FOLDER_MESA_DEMOS
    ARCHIVE         libgfxMesaDemos.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaDriEs2FboDemoStart
}

Component INCLUDE_MESA_ES2GEAR_DEMO {
    NAME            OpenGL ES2 gear demo
    SYNOPSIS        This component includes the OpenGL ES2 gear demo that can be started from the VxWorks shell by executing "gfxMesaDriEs2GearDemoStart" for GPU rendering
    _CHILDREN       FOLDER_MESA_DEMOS
    ARCHIVE         libgfxMesaDemos.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaDriEs2GearDemoStart
}

Component INCLUDE_MESA_ES2LOGO_DEMO {
    NAME            OpenGL ES2 logo demo
    SYNOPSIS        This component includes the OpenGL ES2 logo demo that can be started from the VxWorks shell by executing "gfxMesaDriEs2LogoDemoStart" for GPU rendering
    _CHILDREN       FOLDER_MESA_DEMOS
    ARCHIVE         libgfxMesaDemos.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaDriEs2LogoDemoStart
}

Component INCLUDE_MESA_ES3SWAP_DEMO {
    NAME            OpenGL ES3 swap demo
    SYNOPSIS        This component includes the OpenGL ES3 swap demo that can be started from the VxWorks shell by executing "gfxMesaDriEs3SwapDemoStart" for GPU rendering
    _CHILDREN       FOLDER_MESA_DEMOS
    ARCHIVE         libgfxMesaDemos.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaDriEs3SwapDemoStart
}

Component INCLUDE_MESA_EGLKMS_DEMO {
    NAME            EGL KMS demo
    SYNOPSIS        This component includes the EGL KMS demo that can be started from the VxWorks shell by executing "gfxMesaEglKmsDemoStart"
    _CHILDREN       FOLDER_MESA_DEMOS
    ARCHIVE         libgfxMesaDemos.a
    REQUIRES        INCLUDE_MESA \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxMesaEglKmsDemoStart
}



Component INCLUDE_MESA_MAIN_DEMO {
    NAME            OpenGL main demo
    SYNOPSIS        This component includes the OpenGL main demo that can be started from the VxWorks shell by executing "gfxMesaMainDemoStart" for software rendering or "gfxMesaDriMainDemoStart" for GPU rendering if supported
    _CHILDREN       FOLDER_MESA_DEMOS
    ARCHIVE         libgfxMesaDemos.a
    REQUIRES        INCLUDE_MESA_SWAP_DEMO \
                    INCLUDE_MESA_GEAR_DEMO
    LINK_SYMS       gfxMesaMainDemoStart
}


Component INCLUDE_MESA_ESMAIN_DEMO {
    NAME            OpenGL ES main demo
    SYNOPSIS        This component includes the OpenGL ES main demo that can be started from the VxWorks shell by executing "gfxMesaDriEsMainDemoStart" for GPU rendering
    _CHILDREN       FOLDER_MESA_DEMOS
    ARCHIVE         libgfxMesaDemos.a
    REQUIRES        INCLUDE_MESA_ES2SWAP_DEMO \
                    INCLUDE_MESA_ES2GEAR_DEMO \
                    INCLUDE_MESA_ES2LOGO_DEMO \
                    INCLUDE_MESA_ES3SWAP_DEMO
    LINK_SYMS       gfxMesaDriEsMainDemoStart
}

