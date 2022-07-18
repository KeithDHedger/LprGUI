QT += widgets

SOURCES=lprgui.cpp
TARGET=lprgui

target.path=/usr/bin/
desktopfile.path=/usr/share/applications
desktopfile.files=lprgui.desktop

INSTALLS=target
INSTALLS += desktopfile
