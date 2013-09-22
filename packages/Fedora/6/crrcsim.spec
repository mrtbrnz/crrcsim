Summary: A Model-Airplane Flight Simulation Program
Name: crrcsim
Version: 0.9.8
Release: 1.fc6
License: GPL
Group: Amusements/Games
URL: http://crrcsim.sourceforge.net/
Source0: %{name}-src-%{version}.tar.gz
Source1: %{name}-addon-models-0.2.0.zip
Source2: CRRCsim.desktop

Requires: plib
Requires: portaudio
Requires: SDL

BuildPrereq: SDL-devel
BuildPrereq: portaudio
BuildPrereq: freeglut-devel
BuildPrereq: plib-devel
BuildPrereq: libXi-devel
BuildPrereq: libXt-devel
BuildPrereq: libXmu-devel
BuildRequires: desktop-file-utils

%description
Crrcsim is a model-airplane flight simulation program. Using it, you can learn
how to fly model aircraft, test new aircraft designs, and improve your
skills by practicing on your computer. 	

It rules! The flight model is very realistic. The flight model parameters are
calculated based on a 3D representation of the aircraft. Stalls are properly
modelled as well. Model control is possible with your own rc transmitter, or
any input device such as joystick, mouse, keyboard ...

%prep
%setup -q
%setup -a 1

%build
make

%install
rm -rf %{buildroot}

make DESTDIR=%{buildroot} install
desktop-file-install --vendor=""                        \
        --dir=${RPM_BUILD_ROOT}%{_datadir}/applications \
	--add-category X-Red-Hat-Extra                  \
        ${RPM_SOURCE_DIR}/CRRCsim.desktop

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
/usr/local/share/games/crrcsim
/usr/local/bin/crrcsim
/usr/share/applications/CRRCsim.desktop


%changelog
* Sat Feb 24 2007 Jerry Williams <jwilliam@xmission.com> 0.9.8-1
- initial build


