# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 04feb16,yat  add changelog for moving PSL layer from LAYER_REQUIRES to
#              VSB_REQUIRES (V7GFX-294)
# 05jan16,rtm  added RETROFIT and UI, and updated version to 1.1.1.2 (US73137)
# 18dec15,cfm  version 1.1.1.1, updated the PSL layer name (us69494)
# 14sep15,jnl  support querying size of the touchscreen area and update version 
#              to 1.1.1.0. (V7GFX-238)
# 01sep14,y_f  updated version to 1.1.0.0 for fixing the defect about driver
#              register (V7GFX-208)
# 15apr14,kkz  Use commas as separtors on Requires line
# 09feb14,rdl  standardized formatting.
#              changes to reflect container dependencies. Fixes VXW7-1623.
# 03feb14,rdl  update group tag
# 22jan14,rdl  added copyright notice.
#
# DESCRIPTION
# RPM spec file.
#

Name: evdev_tiam335xts
Summary: TI AM335X touch screen driver
Version: 1.1.1.2
Group: ui/evdev
Prefix: /vxworks-7/pkgs/ui/evdev/drv/tiAm335xTs
VXDest: /vxworks-7/pkgs/ui/$evdev_ns_container/drv/tiAm335xTs
Requires: evdev_lib, fdt, vxbus_core, vxbus_buslib, evdev_ns_container, ti_am3x
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
TI AM335X touch screen driver 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Tue Jan 05 2016  Wind River 1.1.1.2
- [TI_AM335X_TS]
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)
  - move PSL layer from LAYER_REQUIRES to VSB_REQUIRES (V7GFX-294)

* Fri Dec 15 2015  Wind River 1.1.1.1
- [TI_AM335X_TS] 
  - updated the PSL layer name (us69494)

* Mon Sep 14 2015  Wind River 1.1.1.0
- [TI_AM335X_TS] 
  - support querying size of the touchscreen area. (V7GFX-238)
