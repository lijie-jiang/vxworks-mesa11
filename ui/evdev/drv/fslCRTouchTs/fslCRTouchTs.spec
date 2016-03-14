# Copyright (c) 2015-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 05jan16,rtm  added RETROFIT and UI, and updated version to to 1.0.0.1 (US73137)
# 18Mar15,c_l  create. (US55213)
#
# DESCRIPTION
# RPM spec file for Freescale CRTouch screen support
#

Name: evdev_fslCRTouchTs
Summary: Freescale CRTouch screen driver
Version: 1.0.0.1
Group: ui/evdev
Prefix: /vxworks-7/pkgs/ui/evdev/drv/fslCRTouchTs
VXDest: /vxworks-7/pkgs/ui/$evdev_ns_container/drv/fslCRTouchTs
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
This provides the Freescale CRTouch screen driver.

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Tue Jan 05 2016  Wind River 1.0.0.1
- [FSL_CRTOUCH_TS]
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)

* Wed Mar 18 2015  Wind River 1.0.0.0
- [FSL_CRTOUCH_TS]
  - add Freescale capacitive and resistive touch (CRTouch) screen support. 
    (US55213)
