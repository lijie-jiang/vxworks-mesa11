# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 16feb16,yat  Add changelog for RETROFIT FEATURE_PROVIDES MESA9 (US24710)
# 11jan16,rtm  added RETROFIT and UI (US73137)
# 11dec15,yat  Update to Mesa 11.0.6.0 (US24710)
# 08oct15,yat  Add changelog for GFX_FIXED_FBMODE_OFFSET (US67554)
# 12jun15,yat  Update version to 9.2.5.1 for xlnxlcvc offset (US58560)
# 02apr15,yat  Add missing AutoReqProv: no
# 22dec14,yat  Port Mesa to VxWorks 7 (US24705)
#
# DESCRIPTION
# RPM spec file.
#

Name: raster_mesa
Summary: Mesa library
Version: 11.0.6.0
Group: ui/raster
Prefix: /vxworks-7/pkgs/ui/raster/mesa
VXDest: /vxworks-7/pkgs/ui/$raster_ns_container/mesa
Requires: khronos_container >= 1.2.0.0
Requires: raster_ns_container, raster_common
Requires: fbdev_common >= 1.0.3.0
BuildArch: noarch

Vendor: Wind River Systems
URL: http://windriver.com
License: WindRiver
Packager: Wind River <http://www.windriver.com>
Distribution: vxworks-7
Release: vx7

AutoReqProv: no

Provides: installonlypkg

#do not strip out binaries
%global __os_install_post %{nil}

%define _unpackaged_files_terminate_build 0

%description
Mesa library

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Tue Feb 16 2016  Wind River 11.0.6.0
- [MESA]
  - add RETROFIT FEATURE_PROVIDES MESA9 (US24710)
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)
  - update to Mesa 11.0.6.0 (US24710)
  - add GFX_FIXED_FBMODE_OFFSET (US67554)

* Fri Jun 12 2015  Wind River 9.2.5.1
- [MESA]
  - cleanup code for the Xilynx Zynq frame buffer driver (US58560)
