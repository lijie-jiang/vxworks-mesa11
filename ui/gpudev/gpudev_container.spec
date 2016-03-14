# Copyright (c) 2013-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 19feb16,yat  RPM name change to gpudev_ns_container (US74879)
# 05jan16,rtm  added UI, and updated version to 1.0.0.2 (US73137)
# 02apr15,yat  Add missing AutoReqProv: no
# 22dec14,yat  Update version due to changes in layer.vsbl
# 15apr14,kkz  Use commas as separtors on Requires line
# 21feb14,ed   Changed to require infrastructure_container.
#              Cleaned up Summary and Description.
# 09feb14,rdl  standardized formatting.
#              changes to reflect container dependencies. Fixes VXW7-1623.
# 03feb14,rdl  update group tag
# 22jan14,rdl  added copyright notice.
#
# DESCRIPTION
# RPM spec file for GPU driver interface
#

Name: gpudev_ns_container
Summary: Build support for the GPU drivers
Version: 1.0.0.2
Group: ui/gpudev
Prefix: /vxworks-7/pkgs/ui/gpudev
Provides: installonlypkg
Requires: khronos_container, fbdev_ns_container, infrastructure_container
BuildArch: noarch

Vendor: Wind River Systems
URL: http://windriver.com
Packager: Wind River <http://www.windriver.com>
License: WindRiver
Distribution: vxworks-7
Release: vx7

AutoReqProv: no

#do not strip out binaries
%global __os_install_post %{nil}

%define _unpackaged_files_terminate_build 0

%description
Build support for the GPU drivers 

%files
%defattr(-, root, root, 0755)
%{prefix}/layer.vsbl
%{prefix}/Makefile

%changelog
* Tue Jan 05 2016  Wind River 1.0.0.2
- [GPUDEV]
  - RPM name change from gpudev_container to gpudev_ns_container
  - added UI to layer for Layer metadata change: add FEATURE field (F3140)
