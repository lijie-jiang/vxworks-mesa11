# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 05jan16,rtm  added RETROFIT and UI, and updated version to 1.0.1.1 (US73137)
# 14sep15,jnl  support querying size of the touchscreen area and update version 
#              to 1.0.1.0. (V7GFX-238)
# 07aug14,y_f  created (US41404)
#
# DESCRIPTION
# RPM spec file for FocalTech Systems FT5X06 Multi-touch Controller support
#

Name: evdev_ft5x06Ts
Summary: FocalTech FT5X06 multi-touch controller driver
Version: 1.0.1.1
Group: ui/evdev
Prefix: /vxworks-7/pkgs/ui/evdev/drv/ft5x06Ts
VXDest: /vxworks-7/pkgs/ui/$evdev_ns_container/drv/ft5x06Ts
Requires: fdt, vxbus_core, vxbus_buslib, vxbus_subsystem, evdev_ns_container
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
This provides the FocalTech Systems FT5X06 series multi-touch controller driver.  

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Tue Jan 05 2016  Wind River 1.0.1.1
- [FT_5X06_TS]
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)

* Mon Sep 14 2015  Wind River 1.0.1.0
- [FT_5X06_TS] 
  - support querying size of the touchscreen area. (V7GFX-238)
