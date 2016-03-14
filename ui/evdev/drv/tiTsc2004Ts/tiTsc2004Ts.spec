# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 05jan16,rtm  added RETROFIT and UI, and updated version to 1.0.2.1 (US73137)
# 14sep15,jnl  support querying size of the touchscreen area and update version 
#              to 1.0.2.0. (V7GFX-238)
# 01sep14,y_f  updated version to 1.0.1.0 for fixing the defect about driver
#              register (V7GFX-208)
# 10jun14,y_f  created (US42301)
#
# DESCRIPTION
# RPM spec file for TI TSC2004 support
#

Name: evdev_tiTsc2004Ts
Summary: TI TSC2004 touch screen driver
Version: 1.0.2.1
Group: ui/evdev
Prefix: /vxworks-7/pkgs/ui/evdev/drv/tiTsc2004Ts
VXDest: /vxworks-7/pkgs/ui/$evdev_ns_container/drv/tiTsc2004Ts
Requires: evdev_lib, fdt, vxbus_core, vxbus_buslib, evdev_ns_container
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
This provides the TI TSC2004 touch screen driver. 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Tue Jan 05 2016  Wind River 1.0.2.1
- [TI_TSC2004_TS]
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)

* Mon Sep 14 2015  Wind River 1.0.2.0
- [TI_TSC2004_TS] 
  - support querying size of the touchscreen area. (V7GFX-238)
