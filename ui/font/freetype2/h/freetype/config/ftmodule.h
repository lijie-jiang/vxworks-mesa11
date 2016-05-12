/*
 *  This file registers the FreeType modules compiled into the library.
 *
 *  If you use GNU make, this file IS NOT USED!  Instead, it is created in
 *  the objects directory (normally `<topdir>/objs/') based on information
 *  from `<topdir>/modules.cfg'.
 *
 *  Please read `docs/INSTALL.ANY' and `docs/CUSTOMIZE' how to compile
 *  FreeType without GNU make.
 *
 */


#define INCLUDE_TRUETYPE_FONTS_SUPPORT
#define INCLUDE_TYPE1_FONTS_SUPPORT
#define INCLUDE_CFF_FONTS_SUPPORT
#define INCLUDE_TYPE1CID_FONTS_SUPPORT
#define INCLUDE_PFR_FONTS_SUPPORT
#define INCLUDE_TYPE42_FONTS_SUPPORT
#define INCLUDE_WINDOWS_FONTS_SUPPORT
#define INCLUDE_PCF_FONTS_SUPPORT
#define INCLUDE_BDF_FONTS_SUPPORT
 
#ifdef INCLUDE_TRUETYPE_FONTS_SUPPORT
FT_USE_MODULE(tt_driver_class)
#endif

#ifdef INCLUDE_TYPE1_FONTS_SUPPORT
FT_USE_MODULE(t1_driver_class)
#endif

#ifdef INCLUDE_CFF_FONTS_SUPPORT
FT_USE_MODULE(cff_driver_class)
#endif

#ifdef INCLUDE_TYPE1CID_FONTS_SUPPORT
FT_USE_MODULE(t1cid_driver_class)
#endif

#ifdef INCLUDE_PFR_FONTS_SUPPORT
FT_USE_MODULE(pfr_driver_class)
#endif

#ifdef INCLUDE_TYPE42_FONTS_SUPPORT
FT_USE_MODULE(t42_driver_class)
#endif

#ifdef INCLUDE_WINDOWS_FONTS_SUPPORT
FT_USE_MODULE(winfnt_driver_class)
#endif

#ifdef INCLUDE_PCF_FONTS_SUPPORT
FT_USE_MODULE(pcf_driver_class)
#endif

#ifdef INCLUDE_BDF_FONTS_SUPPORT
FT_USE_MODULE(bdf_driver_class)
#endif

FT_USE_MODULE(autofit_module_class)
FT_USE_MODULE(ft_raster1_renderer_class)
FT_USE_MODULE(ft_smooth_renderer_class)
FT_USE_MODULE(ft_smooth_lcd_renderer_class)
FT_USE_MODULE(ft_smooth_lcdv_renderer_class)
FT_USE_MODULE(psaux_module_class)
FT_USE_MODULE(psnames_module_class)
FT_USE_MODULE(pshinter_module_class)
FT_USE_MODULE(sfnt_module_class)

/* EOF */
