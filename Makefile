PROGRAM=lprgui
PREFIX=/usr
SOURCES=lprgui.cpp
DESKTOPFILE=lprgui.desktop

all:
	rm -f $(PROGRAM)
	g++ -Wall `pkg-config --cflags --libs Qt5Widgets` -fPIC $(SOURCES) -o $(PROGRAM)

install:
	install -D $(PROGRAM) $(DESTDIR)$(PREFIX)/bin/$(PROGRAM)
	install -D $(PROGRAM) $(DESTDIR)$(PREFIX)/share/applications/$(DESKTOPFILE)

uninstall:
	rm $(DESTDIR)/$(PREFIX)/bin/$(PROGRAM)
	rm $(DESTDIR)/$(PREFIX)/share/applications/$(DESKTOPFILE)

clean:
	rm -f $(PROGRAM)


