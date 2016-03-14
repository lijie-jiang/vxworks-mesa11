# Copyright (c) 2015-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 16feb16,yat  Add changelog RETROFIT FEATURE_REQUIRES MESA9 (US24710)
# 02feb16,yat  Update version for updating tests
# 09oct15,rpc  Created from demos spec file.
#
# DESCRIPTION
# RPM spec file.
#

Name: raster_mesa_tests
Summary: Mesa OpenGL, OpenGL ES and OpenVG test programs
Version: 1.0.1.0
# Group MUST be 'test' so that RPMs do NOT get included in DVDs.
Group: test
Prefix: /vxworks-7/pkgs/ui/raster/mesa_tests
VXDest: /vxworks-7/pkgs/ui/$raster_ns_container/mesa_tests
BuildArch: noarch

License: WindRiver
Vendor: Wind River Systems
Packager: Wind River <http://www.windriver.com>
Release: vx7
Distribution: vxworks-7
Provides: installonlypkg
Requires: raster_ns_container, raster_mesa >= 11.0.6.0

AutoReqProv: no

URL: http://windriver.com
#do not strip out binaries
%global __os_install_post %{nil}
%define _unpackaged_files_terminate_build 0

#do not strip out binaries
%global __os_install_post %{nil}

# this is == summary right now
%description
Mesa OpenGL, OpenGL ES and OpenVG test programs 

%prep

%clean 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Tue Feb 16 2016  Wind River 1.0.1.0
- [MESA_TESTS]
  - add RETROFIT FEATURE_REQUIRES MESA9 (US24710)
  - Update tests

* Fri Oct 9 2015  Wind River 1.0.0.0
- [MESA_TESTS]
  - Created
