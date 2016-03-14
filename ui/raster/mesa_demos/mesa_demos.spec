# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 16feb16,yat  Add changelog RETROFIT FEATURE_REQUIRES MESA9 (US24710)
# 11jan16,rtm  added RETROFIT and UI (US73137)
# 14sep15,yat  Update version to 1.0.1.0 for Mesa GPU DRI (US24710)
# 10sep15,yat  Add changelog for missing numConfigs (V7GFX-279)
# 23apr15,yat  Update version to 1.0.0.1 for RTP pthread check (V7GFX-249)
# 02apr15,yat  Add missing AutoReqProv: no
# 22dec14,yat  Create demo for Mesa (US24712)
#
# DESCRIPTION
# RPM spec file.
#

Name: raster_mesa_demos
Summary: Mesa OpenGL, OpenGL ES and OpenVG demo programs
Version: 1.0.1.0
Group: ui/raster

Prefix: /vxworks-7/pkgs/ui/raster/mesa_demos
VXDest: /vxworks-7/pkgs/ui/$raster_ns_container/mesa_demos
# host arch = BuildArch  default to noarch
BuildArch: noarch

Vendor: Wind River Systems
URL: http://windriver.com
License: WindRiver
Packager: Wind River <http://www.windriver.com>
Distribution: vxworks-7
Release: vx7

AutoReqProv: no

Provides: installonlypkg
Requires: raster_ns_container, raster_mesa >= 11.0.6.0

#do not strip out binaries
%global __os_install_post %{nil}

%define _unpackaged_files_terminate_build 0

%description
Mesa OpenGL, OpenGL ES and OpenVG demo programs 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Tue Feb 16 2016  Wind River 1.0.1.0
- [MESA_DEMOS]
  - add RETROFIT FEATURE_REQUIRES MESA9 (US24710)
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)
  - add support for Mesa GPU DRI (F3588)
  - fix missing numConfigs (V7GFX-279)

* Thu Apr 23 2015  Wind River 1.0.0.1
- [MESA_DEMOS]
  - Mesa RTP demos did not check for pthread (V7GFX-249)
