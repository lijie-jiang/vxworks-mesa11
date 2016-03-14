# Copyright (c) 2013-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 05jan16,rtm  added RETROFIT and UI, and updated version to 1.0.1.1 (US73137)
# 15oct14,y_f  updated version to 1.0.1.0 for moving to a new group for the
#              driver
# 12aug14,y_f  updated version to 1.0.0.2 for cleaning compiler warnings
#              (V7GFX-204)
# 15apr14,kkz  Use commas as separtors on Requires line
# 09feb14,rdl  standardized formatting.
#              changes to reflect container dependencies. Fixes VXW7-1623.
# 03feb14,rdl  update group tag
# 22jan14,rdl  added copyright notice.
#
# DESCRIPTION
# RPM spec file.
#

Name: evdev_vxsimkbd
Summary: VxWorks simulator keyboard driver
Version: 1.0.1.1
Group: ui/evdev/drv/vxsim
Prefix: /vxworks-7/pkgs/ui/evdev/drv/vxSimKbd
VXDest: /vxworks-7/pkgs/ui/$evdev_ns_container/drv/vxSimKbd
Requires: evdev_lib, fbdev_vxsim, evdev_ns_container
BuildArch: noarch

Vendor: Wind River Systems
URL: http://windriver.com
Packager: Wind River <http://www.windriver.com>
License: WindRiver
Distribution: vxworks-7
Release: vx7

Provides: installonlypkg
AutoReqProv: no

#do not strip out binaries
%global __os_install_post %{nil}

%define _unpackaged_files_terminate_build 0

%description
VxWorks simulator keyboard driver 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Tue Jan 05 2016  Wind River 1.0.1.1
- [VXSIM_KBD]
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)
