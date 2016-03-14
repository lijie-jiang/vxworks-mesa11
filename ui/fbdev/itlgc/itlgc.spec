# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 25jan16,yat  Update version and add changelog (V7GFX-245)
# 02apr15,yat  Add missing AutoReqProv: no
# 22dec14,yat  Change fbdev_container to fbdev_common (US46449)
# 18sep14,yat  Update version for pmapPrivateUnmap message fix (V7GFX-214)
# 09feb14,rdl  standardized formatting.
#              changes to reflect container dependencies. Fixes VXW7-1623.
# 22jan14,rdl  added copyright notice.
#
# DESCRIPTION
# RPM spec file.
#

Name: fbdev_itlgc
Summary: Intel graphics controller frame buffer driver
Version: 1.0.2.0
Group: ui/fbdev
Prefix: /vxworks-7/pkgs/ui/fbdev/itlgc
VXDest: /vxworks-7/pkgs/ui/$fbdev_ns_container/itlgc
Requires: fbdev_ns_container, fbdev_common
BuildArch: noarch

License: WindRiver
Vendor: Wind River Systems
Packager: Wind River <http://www.windriver.com>
Distribution: vxworks-7
Release: vx7

AutoReqProv: no

Provides: installonlypkg

#do not strip out binaries
%global __os_install_post %{nil}

%define _unpackaged_files_terminate_build 0

%description
Intel graphics controller frame buffer driver 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Mon Jan 25 2016  Wind River 1.0.2.0
- [ITLGCFB]
  - add DOC_BUILD NO to layer.vsbl (V7GFX-245)
  - fix ios device cleanup in gfxFbIosDrv.inl (US73564)
  - add notifyFbAddrChangeFuncPtr feature in gfxFbIosDrv.inl (US73564)
  - fix static analysis defect in gfxFbIosDrv.inl (US71171)
  - add GFX_FIXED_FBMODE_OFFSET in gfxFbIosDrv.inl (US67342)
  - add support for GFX_USE_CURRENT_VIDEO_MODES in gfxFbIosDrv.inl (US66034)
  - do not free firstVirtAddr for pmap virtAddr in gfxFbIosDrv.inl (US66034)
  - add offset for xlnxlcvc in gfxFbIosDrv.inl (US58560)
  - updated for VxBus GEN2 in gfxFbIosDrv.inl (US58560)
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)

