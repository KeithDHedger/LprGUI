LprGUI

QT5 based frontend for lpr cli command.

This is a simple gui frontend to the lpr command, as qt5 doesn't play well with cups, if your app allows you to set a print command you can use this.
There are other GU@I's for lp/lpr but they seem to have large dependencies, or need specific librarary versions, this application just needs lpr which is part of CUPS and a reasonable new QT5, which most distros will have installed as default.

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

