# Copyright (c) 2015-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 24feb16,yat  Add changelog RETROFIT FEATURE_PROVIDES DRM3 (US75033)
# 19feb16,yat  Add VXDest (US74879)
# 05jan16,rtm  added RETROFIT and UI (US73137)
# 06nov15,yat  Update version to 4.4.0.0 (F3588)
# 28sep15,wap  Update version for fix to vxoal code
# 02apr15,yat  Add missing AutoReqProv: no
# 22jan15,qsn  Port DRM to VxWorks 7 (US50702)
#
# DESCRIPTION
# RPM spec file.
#

Name: gpudev_drm
Summary: DRM driver
Version: 4.4.0.0
Group: ui/gpudev
Prefix: /vxworks-7/pkgs/ui/gpudev/drm
VXDest: /vxworks-7/pkgs/ui/$gpudev_ns_container/drm
Requires: gpudev_libdrm >= 2.4.65.0
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
DRM GPU driver

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Wed Feb 24 2016  Wind River 4.4.0.0
- [DRM]
  - add RETROFIT FEATURE_PROVIDES DRM3 (US75033)
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)
  - update DRM to Intel DRM DRIVER_DATE 20150928 (F3588)
  - add DOC_BUILD NO to layer.vsbl (V7GFX-245)

* Mon Sep 28 2015  Wind River 3.12.0.1
- [DRM]
  - fix vxoal code
