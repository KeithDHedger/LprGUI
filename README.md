LprGUI

QT5 based frontend for lpr cli command.

This is a simple gui frontend to the lpr command, as qt5 doesn't play well with cups, if your app allows you to set a print command you can use this.
There are other GU@I's for lp/lpr but they seem to have large dependencies, or need specific librarary versions, this application just needs lpr and lpoptions which are part of CUPS and a reasonable new QT5, which most distros will have installed as default.

You will also of course need a build system ( g++,make qt5 dev file etc ).

building/installing:

make
sudo make install
or make DESTDIR="/path/to/install/location" install

Or you can build with qmake:
mkdir build && build
qmake ..
make
sudo make install
or make INSTALL_ROOT="/path/to/install/location" install

You can run the application without needing to install it if you just want to try it.

Files to be printed can be selected via the 'Select Files' button or entered into the files box manuall, files should be sepereated by a colon like so:
/media/LinuxData/Development64/CPPScripts/qt5/hilighting/C++.txt.cpp:/media/LinuxData/Development64/CPPScripts/qt5/hilighting/testqthilite.cpp

Options can be entered into the options box seperated by a colon eg:
orientation-requested=6:number-up=4

Options are printer dependant and can be found with:
lpoptions -p NAMEOFPRINTER
