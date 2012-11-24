# This is the spec file for Tupi

Name: tupi
Version: 0.2
Release: 3%{?dist}
Summary: 2D vector-based animation environment 
License: GPLv3+
URL: http://www.maefloresta.com
Source: http://www.maefloresta.com/portal/files/%{name}-%{version}.tar.gz

BuildRequires: ruby, zlib-devel, quazip-devel
BuildRequires: qconf, desktop-file-utils, qt4-devel
BuildRequires: libtheora-devel, libogg-devel
# BuildRequires: ffmpeg, ffmpeg-libs, ffmpeg-devel

%description
2D vector-based animation environment for digital artists

%prep
%setup -q

%build
%configure
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}
%find_lang %{name} --with-qt
find %{buildroot} -name \*.la | xargs rm -f

%post
/usr/bin/update-mime-database %{_datadir}/mime &> /dev/null || :

%postun
/usr/bin/update-mime-database %{_datadir}/mime &> /dev/null || :

desktop-file-validate %{buildroot}/%{_datadir}/applications/%{name}.desktop

%files -f %{name}.lang
%doc README COPYING

%dir %_datadir/%{name}/data/
%dir %_datadir/%{name}/data/cs/
%dir %_datadir/%{name}/data/da/
%dir %_datadir/%{name}/data/de/
%dir %_datadir/%{name}/data/en/
%dir %_datadir/%{name}/data/es/
%dir %_datadir/%{name}/data/gl/
%dir %_datadir/%{name}/data/pt/
%dir %_datadir/%{name}/data/ru/
%{_datadir}/%{name}/data/*/gpl.css
%{_datadir}/%{name}/data/*/*.xml
%{_datadir}/%{name}/data/*/*.html

%dir %_datadir/%{name}/data/palettes/
%{_datadir}/%{name}/data/palettes/*.tpal

%dir %_datadir/%{name}/data/help/
%dir %_datadir/%{name}/data/help/css/  
%{_datadir}/%{name}/data/help/css/tupi.ini

%dir %_datadir/%{name}/data/help/en/  
%dir %_datadir/%{name}/data/help/es/  
%dir %_datadir/%{name}/data/help/ru/
%dir %_datadir/%{name}/data/help/examples/  
%{_datadir}/%{name}/data/help/examples/example.*

%dir %_datadir/%{name}/data/help/gl/  

%dir %_datadir/%{name}/data/help/images/
%{_datadir}/%{name}/data/help/images/*.png
%{_datadir}/%{name}/data/help/*/*.html
%{_datadir}/%{name}/data/help/*/*.xml
%{_datadir}/%{name}/data/help/*/*.css
%{_datadir}/%{name}/data/help/*/images/*.png

%dir %_datadir/%{name}/data/help/to_translate/
%_datadir/%{name}/data/help/to_translate/*

%dir %_datadir/%{name}/data/help/README/
%{_datadir}/%{name}/data/help/README/*

%dir %_datadir/%{name}/data/storyboard/
%{_datadir}/%{name}/data/storyboard/tupi.css

%dir %_datadir/%{name}/data/translations/
%{_datadir}/%{name}/data/translations/*.ts

%dir %_datadir/%{name}/themes/
%dir %_datadir/%{name}/themes/default/
%dir %_datadir/%{name}/themes/default/cursors/
%dir %_datadir/%{name}/themes/default/icons/
%dir %_datadir/%{name}/themes/default/images/
%_datadir/%{name}/themes/*/*/*.png

%dir %_libdir/%{name}/
%{_libdir}/%{name}/*

%{_bindir}/%{name}
%{_bindir}/%{name}.bin
%{_datadir}/applications/tupi.desktop
%{_datadir}/man/man1/*.1*
%{_datadir}/mime/packages/tupi.xml
%{_datadir}/pixmaps/%{name}.png

%changelog

* Mon Nov 19 2012 Gustav Gonzalez <xtingray@maefloresta.com> - 0.2-3
- Extended line "files"
- Added lines post and postrun to update mime database
- Added a detailed list of application directories and files

* Sat Nov 17 2012 Gustav Gonzalez <xtingray@maefloresta.com> - 0.2-2
- Replacing variable %%buildroot instead of RPM_BUILD_ROOT
- Fixing rpmlint errores
- Removing the line "ExcludeArch: ppc ppc64"
- Removing the ownership of the "man" directory   
- Changing global permissions to 644
- Added licence/readme line 

* Fri Oct 26 2012 Gustav Gonzalez <xtingray@maefloresta.com> - 0.2-1
- Several lines of the spec file were adjusted according to the Fedora standard 
