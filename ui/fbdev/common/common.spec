# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 25jan16,yat  Update version for GFX_FIXED_FBMODE_OFFSET (US67554)
# 05jan16,rtm  added RETROFIT and UI (US73137)
# 12jun15,yat  Update version to 1.0.2.0 for xlnxlcvc offset (US58560)
# 16apr15,yat  Update version to 1.0.1.1 for fbdev docs fix (V7GFX-245)
# 02apr15,yat  Add missing AutoReqProv: no
# 22dec14,yat  Update version to 1.0.1.0 for LP64 support (US50456)
# 01oct14,yat  Update version to 1.0.0.1 for change to snprintf (V7GFX-220)
# 09feb14,rdl  standardized formatting.
#              changes to reflect container dependencies. Fixes VXW7-1623.
# 22jan14,rdl  added copyright notice.
#
# DESCRIPTION
# RPM spec file.
#

Name: fbdev_common
Summary: Common frame buffer resources
Version: 1.0.3.0
Group: ui/fbdev
Prefix: /vxworks-7/pkgs/ui/fbdev/common
VXDest: /vxworks-7/pkgs/ui/$fbdev_ns_container/common
Requires: fbdev_ns_container
BuildArch: noarch

License: WindRiver
Vendor: Wind River Systems
URL: http://windriver.com
Packager: Wind River <http://www.windriver.com>
Distribution: vxworks-7
Release: vx7

AutoReqProv: no

Provides: installonlypkg

#do not strip out binaries
%global __os_install_post %{nil}

%define _unpackaged_files_terminate_build 0

%description
Common frame buffer resources 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Mon Jan 25 2016  Wind River 1.0.3.0
- [COMMON]
  - replace gfxFbSplashBlit1 with integer version (US73127)
  - add GFX_FIXED_FBMODE_OFFSET (US67554)
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)

* Fri Jun 12 2015  Wind River 1.0.2.0
- [COMMON]
  - cleanup code for the Xilynx Zynq frame buffer driver (US58560)
