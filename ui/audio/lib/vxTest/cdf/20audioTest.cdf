/* 20AudioTest.cdf - BSPVTS audio test suite test components  */

/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
Modification history
--------------------
10nov15,zjl  update for vxTest
08oct15,c_l  create
 */

Component    INCLUDE_TM_AUDIO {
        NAME            Audio Test Module
        SYNOPSIS        This component adds the audio stand IO Test Module
        REQUIRES        INCLUDE_VXTEST_DRIVER \
                        INCLUDE_RAM_DISK \
                        INCLUDE_AUDIO_LIB_CORE \
                        INCLUDE_AUDIO_LIB_WAV
        MODULES         tmAudio.o
        INCLUDE_WHEN    INCLUDE_TM_AUDIO_LIB_TEST
        PROTOTYPE       void atInit();
        INIT_RTN        atInit();
        LINK_SYMS       atInit
}
/*
 * Test Init Group
 */
InitGroup       usrVxTestAudioTestsInit {
    INIT_RTN        usrVxTestAudioTestsInit ();
    SYNOPSIS        VxTest audio tests initialization sequence
    INIT_ORDER      INCLUDE_TM_AUDIO
    _INIT_ORDER     usrVxTestAudioInit
}

/*
 * audio VTS Tests Folder
 */
Folder          FOLDER_VXTEST_AUDIO_LIB {
    NAME        audio test components
    SYNOPSIS    Used to group audio test components
    CHILDREN    INCLUDE_TM_AUDIO 
    DEFAULTS    INCLUDE_TM_AUDIO
    _CHILDREN   FOLDER_VXTEST_AUDIO_TESTS
}
