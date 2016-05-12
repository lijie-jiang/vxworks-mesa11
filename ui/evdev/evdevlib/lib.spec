# Copyright (c) 2013-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 05jan16,rtm  added RETROFIT and UI, and updated version to 1.1.2.2 (US73137)
# 21dec15,jnl  fixed null pointer defect and update version to 1.1.2.1. 
#              (V7GFX-302)
# 14sep15,jnl  support querying size of the touchscreen area and update version 
#              to 1.1.2.0. (V7GFX-238)
# 29dec14,y_f  fixed enabling device error in SMP mode and updated version to
#              1.1.1.1. (V7GFX-228)
# 21oct14,y_f  add evdev to core and update version to 1.1.1.0 (VXW7-3211)
# 23sep14,rbc  Upgrade version number to 1.1.0.1 to init keyboard and mouse
#              before network driver (V7CON-179)
# 15apr14,kkz  Use commas as separtors on Requires line
# 24jun14,y_f  added multitouch support and updated version to 1.1.0.0 (US41403)
# 15apr14,kkz  used commas as separtors on Requires line
# 09feb14,rdl  standardized formatting.
#              changes to reflect container dependencies. Fixes VXW7-1623.
# 22jan14,rdl  added copyright notice.
#
# DESCRIPTION
# RPM spec file.
#

Name: evdev_lib
Summary: Event devices framework libraries
Version: 1.1.2.2
Group: ui/evdev
Prefix: /vxworks-7/pkgs/ui/evdev/lib
VXDest: /vxworks-7/pkgs/ui/$evdev_ns_container/lib
Requires: core_kernel, evdev_ns_container
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
Event devices framework libraries 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Tue Jan 05 2016  Wind River 1.1.2.2
- [LIB]
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)

* Mon Dec 21 2015  Wind River 1.1.2.1
- [LIB] 
  - fixed null pointer defect. (V7GFX-302)

* Mon Sep 14 2015  Wind River 1.1.2.0
- [LIB] 
  - support querying size of the touchscreen area. (V7GFX-238)

