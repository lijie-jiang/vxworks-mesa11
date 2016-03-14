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
# 01dec14,y_f  created (US50218)
#
# DESCRIPTION
# RPM spec file for EETI EXC7200 series Multi-touch Controller support
#

# run `vxprj vsb genPkg LayerName` to regenerate
# Some manual modification may be required

# Define rpmbuild macros

Name: evdev_eetiExc7200Ts

Summary: EETI EXC7200 multi-touch controller driver

Version: 1.0.1.1

Group: ui/evdev

#Prefix == install directory
Prefix: /vxworks-7/pkgs/ui/evdev/drv/eetiExc7200Ts
VXDest: /vxworks-7/pkgs/ui/$evdev_ns_container/drv/eetiExc7200Ts

Requires: fdt, vxbus_core, vxbus_buslib, vxbus_subsystem, evdev_ns_container
Requires: evdev_lib >= 1.1.2.0

# host arch = BuildArch  default to noarch
BuildArch: noarch

License: WindRiver
Vendor: Wind River Systems
Packager: Wind River <http://www.windriver.com>
Release: vx7
Distribution: vxworks-7
Provides: installonlypkg
AutoReqProv: no


# URL: N/A
#Requires: vxworks-core >= 0:1.0-7.0
#do not strip out binaries
%global __os_install_post %{nil}
#%define _missing_doc_files_terminate_build 0

%define _unpackaged_files_terminate_build 0

#do not strip out binaries
%global __os_install_post %{nil}

# this is == summary right now
%description
This provides the EETI EXC7200 series multi-touch controller driver. 

%prep

#echo PWD=$PWD
#echo RPM_SOURCE_DIR=$RPM_SOURCE_DIR
#echo RPM_BUILD_DIR=$RPM_BUILD_DIR
#echo RPM_DOC_DIR=$RPM_DOC_DIR
#echo RPM_OPT_FLAGS=$RPM_OPT_FLAGS
#echo RPM_ARCH=$RPM_ARCH
#echo RPM_OS=$RPM_OS
#echo RPM_ROOT_DIR=$RPM_ROOT_DIR
#echo RPM_BUILD_ROOT=$RPM_BUILD_ROOT
#echo RPM_PACKAGE_NAME=$RPM_PACKAGE_NAME
#echo RPM_PACKAGE_VERSION=$RPM_PACKAGE_VERSION
#echo RPM_PACKAGE_RELEASE=$RPM_PACKAGE_RELEASE

%clean 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Tue Jan 05 2016  Wind River 1.0.1.1
- [EETI_EXC7200_TS]
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)

* Mon Sep 14 2015  Wind River 1.0.1.0
- [EETI_EXC7200_TS] 
  - support querying size of the touchscreen area. (V7GFX-238)

