# Copyright (c) 2015-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 17feb16,yat  Add changelog frame buffer copy test (US73758)
# 02feb16,yat  Update version for updating tests
# 08oct15,rpc  Creating test rpms.
#
# DESCRIPTION
# RPM spec file for the frame buffer driver test folder.
#

Name: fbdev_tests
Summary: Frame buffer driver test programs
Version: 1.0.1.0
# Group MUST be 'test' so that RPMs do NOT get included in DVDs.
Group: test
Prefix: /vxworks-7/pkgs/ui/fbdev/tests
VXDest: /vxworks-7/pkgs/ui/$fbdev_ns_container/tests
Requires: fbdev_ns_container, fbdev_common
BuildArch: noarch

Vendor: Wind River Systems
URL: http://windriver.com
Packager: Wind River <http://www.windriver.com>
License: WindRiver
Distribution: vxworks-7
Release: vx7

AutoReqProv: no

#do not strip out binaries
%global __os_install_post %{nil}
%define _unpackaged_files_terminate_build 0

%description
Frame buffer driver test programs

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Tue Feb 16 2016  Wind River 1.0.1.0
- [TESTS]
  - Add frame buffer copy test (US73758)
  - Update tests

* Fri Oct 9 2015  Wind River 1.0.0.0
- [TESTS]
  - Created
