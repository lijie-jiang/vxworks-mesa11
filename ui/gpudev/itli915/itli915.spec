# Copyright (c) 2015-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 24feb16,yat  Add changelog RETROFIT FEATURE_REQUIRES DRM3 (US75033)
# 19feb16,yat  Add VXDest (US74879)
# 05jan16,rtm  added RETROFIT and UI, and updated version to 3.12.0.1 (US73137)
# 29oct15,yat  Update version to 4.4.0.0 (F3588)
# 02apr15,yat  Add missing AutoReqProv: no
# 22jan15,qsn  Port Intel HD graphics driver to VxWorks 7 (US50702)
#
# DESCRIPTION
# RPM spec file.
#

Name: gpudev_itli915
Summary: Intel HD graphics driver
Version: 4.4.0.0
Group: ui/gpudev
Prefix: /vxworks-7/pkgs/ui/gpudev/itli915
VXDest: /vxworks-7/pkgs/ui/$gpudev_ns_container/itli915
Requires: gpudev_drm >= 4.4.0.0
BuildArch: noarch

Vendor: Wind River Systems
URL: http://windriver.com
License: WindRiver
Packager: Wind River <http://www.windriver.com>
Distribution: vxworks-7
Release: vx7

AutoReqProv: no

Provides: installonlypkg

#do not strip out binaries
%global __os_install_post %{nil}

%define _unpackaged_files_terminate_build 0

%description
Intel HD graphics GPU driver

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Wed Feb 24 2016  Wind River 4.4.0.0
- [ITLI915]
  - add RETROFIT FEATURE_REQUIRES DRM3 (US75033)
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)
  - update i915 to Intel i915 DRIVER_DATE 20150928 (F3588)
  - add DOC_BUILD NO to layer.vsbl (V7GFX-245)

