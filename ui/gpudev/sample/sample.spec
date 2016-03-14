# Copyright (c) 2015 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 19feb16,yat  Add VXDest (US74879)
# 24jul15,yat  Written (US60452)
#
# DESCRIPTION
# RPM spec file.
#

Name: gpudev_sampledrm
Summary: Sample DRM device driver
Version: 1.0.0.0
Group: ui/gpudev
Prefix: /vxworks-7/pkgs/ui/gpudev/sample
VXDest: /vxworks-7/pkgs/ui/$gpudev_ns_container/sample
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
Sample DRM device driver

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Fri Jul 24 2015  Wind River 1.0.0.0
- [SAMPLEDRM]
  - Create sample DRM device (F3588)
