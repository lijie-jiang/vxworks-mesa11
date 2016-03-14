# Copyright (c) 2015-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 24feb16,yat  Add changelog RETROFIT FEATURE_PROVIDES LIBDRM3 (US75033)
# 19feb16,yat  Add VXDest (US74879)
# 05jan16,rtm  added RETROFIT and UI (US73137)
# 06nov15,yat  Update version to 2.4.65.0 (US67563)
# 22oct15,yat  Add LP64 and update version as this layer supports LP64 only
# 02apr15,yat  Add missing AutoReqProv: no
# 07jan15,yat  Port libDRM to VxWorks 7 (US24705)
#
# DESCRIPTION
# RPM spec file.
#

Name: gpudev_libdrm
Summary: libDRM GPU driver
Version: 2.4.65.0
Group: ui/gpudev
Prefix: /vxworks-7/pkgs/ui/gpudev/libdrm
VXDest: /vxworks-7/pkgs/ui/$gpudev_ns_container/libdrm
Requires: gpudev_common
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
libDRM GPU driver

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Wed Feb 24 2016  Wind River 2.4.65.0
- [LIBDRM]
  - add RETROFIT FEATURE_PROVIDES LIBDRM3 (US75033)
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)
  - update libdrm to 2.4.65 (F3588)
  - add DOC_BUILD NO to layer.vsbl (V7GFX-245)

* Thu Oct 22 2015  Wind River 2.4.58.1
- [LIBDRM]
  - add LP64 requires as this layer supports LP64 only
