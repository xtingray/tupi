# This is the spec file for Tupi

Name: tupi
Version: 0.2
Release: 1%{?dist}
Summary: Tupi: Open 2D Magic
License: GPLv3+
URL: http://www.maefloresta.com
Source: http://www.maefloresta.com/portal/files/tupi-0.2.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires: ruby, zlib-devel, quazip-devel
BuildRequires: qconf, qt4-devel >= 4.7
# BuildRequires: ffmpeg, ffmpeg-libs, ffmpeg-devel
BuildRequires: libtheora-devel, libogg-devel
# Requires: qt, quazip, libtheora

%description
2D vectorial/animation environment for digital artists

%prep
%setup -q

%build
%configure --prefix=%{buildroot}/usr --bindir=%{buildroot}/%{_bindir} --sharedir=%{buildroot}/%{_datadir}/tupi --libdir=%{buildroot}/%{_libdir}/tupi --package-build
make

%install
make install DESTDIR=%{buildroot}
#make install
find $RPM_BUILD_ROOT -name \*.la | xargs rm -f

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{_bindir}/tupi
%{_bindir}/tupi.bin
%{_datadir}/applications
%{_datadir}/pixmaps/tupi.png
%{_datadir}/tupi
%{_libdir}/tupi
%{_datadir}/man/man1

%changelog
* Sat Oct 13 2012 Tupi
- Making of RPM

