# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 17feb16,yat  Update changelog for adding GFX_FIXED_FBMODE_OFFSET (US67554)
# 05jan16,rtm  added RETROFIT and UI (US73137)
# 02dec15,yat  Update version to support INCLUDE_FBDEV_INIT (US71171)
# 22sep15,yat  Update version to fix demos cdf (V7GFX-284)
# 12jun15,yat  Update version to 1.0.3.0 for xlnxlcvc (US58560)
# 02apr15,yat  Add missing AutoReqProv: no
# 22dec14,yat  Update version to 1.0.2.0 for Intel and LP64 support (US50456)
# 18jun14,yat  Update version to 1.0.1.0 for dynamic RTP demos (US11227)
# 09feb14,rdl  standardized formatting.
#              changes to reflect container dependencies. Fixes VXW7-1623.
# 22jan14,rdl  added copyright notice.
#
# DESCRIPTION
# RPM spec file.
#

Name: fbdev_demos
Summary: Frame buffer driver demo programs
Version: 1.0.4.0
Group: ui/fbdev
Prefix: /vxworks-7/pkgs/ui/fbdev/demos
VXDest: /vxworks-7/pkgs/ui/$fbdev_ns_container/demos
Requires: fbdev_ns_container, fbdev_common
BuildArch: noarch

Vendor: Wind River Systems
URL: http://windriver.com
Packager: Wind River <http://www.windriver.com>
License: WindRiver
Distribution: vxworks-7
Release: vx7

AutoReqProv: no

Provides: installonlypkg

#do not strip out binaries
%global __os_install_post %{nil}

%define _unpackaged_files_terminate_build 0

%description
Frame buffer driver demo programs 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Wed Feb 17 2016  Wind River 1.0.4.0
- [DEMOS]
  - add GFX_FIXED_FBMODE_OFFSET to draw demo (US67554)
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)
  - add support for INCLUDE_FBDEV_INIT (US71171)
  - add missing REQUIRES INCLUDE_RTP in demos cdf (V7GFX-284)
  - remove build restriction for ARCH_i86 dynamic RTP (US24710)

* Fri Jun 12 2015  Wind River 1.0.3.0
- [DEMOS]
  - cleanup code for the Xilynx Zynq frame buffer driver (US58560)
