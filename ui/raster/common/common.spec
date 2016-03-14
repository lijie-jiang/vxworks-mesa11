# Copyright (c) 2013-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 11jan16,rtm  added RETROFIT and UI, and updated version to 1.0.1.1 (US73137)
# 02apr15,yat  Add missing AutoReqProv: no
# 22dec14,yat  Update version to add DOC_BUILD NO (US46449)
# 09feb14,rdl  standardized formatting.
#              changes to reflect container dependencies. Fixes VXW7-1623.
# 22jan14,rdl  added copyright notice.
#
# DESCRIPTION
# RPM spec file.
#

Name: raster_common
Summary: Common raster resources
Version: 1.0.1.1
Group: ui/raster
Prefix: /vxworks-7/pkgs/ui/raster/common
VXDest: /vxworks-7/pkgs/ui/$raster_ns_container/common
Requires: raster_ns_container
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
Common raster resources 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Mon Jan 11 2016  Wind River 1.0.1.1
- [COMMON]
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)

