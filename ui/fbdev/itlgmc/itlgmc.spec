# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 24feb16,yat  Add changelog RETROFIT FEATURE_REQUIRES GPUDEV_LIBDRM (US75033)
# 04feb16,yat  Remove system model check for Intel (US73271)
# 05jan16,rtm  added RETROFIT and UI (US73137)
# 14sep15,yat  Update version for LVDS, DVI and GEM support (US66034)
# 16apr15,yat  Update version for DOC_BUILD NO (V7GFX-245)
# 02apr15,yat  Add missing AutoReqProv: no
# 22dec14,yat  Change fbdev container to fbdev_common (US46449)
# 20dec14,qsn  Initial VxWorks 7 release (US48907)
#
# DESCRIPTION
# RPM spec file.
#

Name: fbdev_itlgmc
Summary: Intel Graphics and Memory Controller frame buffer driver
Version: 1.0.1.0
Group: ui/fbdev
Prefix: /vxworks-7/pkgs/ui/fbdev/itlgmc
VXDest: /vxworks-7/pkgs/ui/$fbdev_ns_container/itlgmc
Requires: fbdev_ns_container, fbdev_common
Requires: gpudev_libdrm >= 2.4.65.0
Requires: gpudev_itli915
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
Intel Graphics and Memory Controller frame buffer driver

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Wed Feb 24 2016  Wind River 1.0.1.0
- [ITLGMC]
  - add RETROFIT FEATURE_REQUIRES GPUDEV_LIBDRM (US75033)
  - remove system model check for Intel (US73271)
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)
  - add LVDS, DVI and GEM support (F3588)
  - add DOC_BUILD NO to layer.vsbl (V7GFX-245)

