# Copyright (c) 2013-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 05jan16,rtm  added RETROFIT and UI, and updated version to 1.1.1.1 (US73137)
# 14sep15,jnl  support querying size of the touchscreen area and update version 
#              to 1.1.1.0. (V7GFX-238)
# 24jun14,y_f  added multitouch support and updated version to 1.1.0.0 (US41403)
# 15apr14,kkz  Use commas as separtors on Requires line
# 09feb14,rdl  standardized formatting.
#              changes to reflect container dependencies. Fixes VXW7-1623.
# 22jan14,rdl  added copyright notice.
#
# DESCRIPTION
# RPM spec file.
#

Name: evdev_demo
Summary: Event Device Demos
Version: 1.1.1.1
Group: ui/evdev
Prefix: /vxworks-7/pkgs/ui/evdev/demo
VXDest: /vxworks-7/pkgs/ui/$evdev_ns_container/demo
Requires: evdev_ns_container
Requires: evdev_lib >= 1.1.2.0
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
Event Device Demos 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Tue Jan 05 2016  Wind River 1.1.1.1
- [DEMO]
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)

* Mon Sep 14 2015  Wind River 1.1.1.0
- [DEMO] 
  - support querying size of the touchscreen area. (V7GFX-238)

