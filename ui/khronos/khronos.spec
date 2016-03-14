# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 05jan16,rtm  added RETROFIT and UI, and updated version to 1.1.0.1 (US73137)
# 23nov15,rpc  Update for Mesa 11.0 (US70717)
# 10nov14,yat  Update version for Mesa (US24713) and OpenCL (US48896)
# 15apr14,kkz  Use commas as separtors on Requires line
# 21feb14,ed   Changed to require infrastructure_container.
#              Cleaned up Summary and Description.
# 09feb14,rdl  standardized formatting.
#              changes to reflect container dependencies. Fixes VXW7-1623.
# 03feb14,rdl  update group tag
# 22jan14,rdl  added copyright notice.
#
# DESCRIPTION
# RPM spec file for the Khronos specification headers
#

Name: khronos_container
Summary: Build support for Khronos specification headers
Version: 1.2.0.0
Group: ui/khronos
Prefix: /vxworks-7/pkgs/ui/khronos
Requires: fbdev_ns_container, infrastructure_container
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
Build support for Khronos specification headers

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Tue Jan 05 2016  Wind River 1.2.0.0
- [KHRONOS]
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)
  - update Khronos header files for Mesa 11.0 (US70717)
