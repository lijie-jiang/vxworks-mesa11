# Copyright (c) 2015-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 24feb16,yat  Add changelog for fixing static analysis defects (US75033)
# 19feb16,yat  Add VXDest (US74879)
# 16sep15,yat  Written (US24710)
#
# DESCRIPTION
# RPM spec file.
#

Name: gpudev_libdrm_demos
Summary: libDRM demo programs
Version: 1.0.0.0
Group: ui/gpudev
Prefix: /vxworks-7/pkgs/ui/gpudev/libdrm_demos
VXDest: /vxworks-7/pkgs/ui/$gpudev_ns_container/libdrm_demos
Requires: gpudev_libdrm >= 2.4.65.0
BuildArch: noarch

License: WindRiver
Vendor: Wind River Systems
Packager: Wind River <http://www.windriver.com>
Release: vx7
Distribution: vxworks-7
Provides: installonlypkg

AutoReqProv: no

#do not strip out binaries
%global __os_install_post %{nil}

%define _unpackaged_files_terminate_build 0

%description
libDRM demo programs

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Wed Feb 24 2016  Wind River 1.0.0.0
- [LIBDRM_DEMOS]
  - fix static analysis defects (US75033)
  - Add libDRM demo programs (F3588)
