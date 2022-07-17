PROGRAM=lprgui
PREFIX=/usr
SOURCES=lprgui.cpp

all:
	rm -f $(PROGRAM)
	g++ -Wall `pkg-config --cflags --libs Qt5Widgets` -fPIC $(SOURCES) -o $(PROGRAM)

install:
	install -D $(PROGRAM) $(DESTDIR)$(PREFIX)/bin/$(PROGRAM)

uninstall:
	rm $(DESTDIR)/$(PREFIX)/bin/$(PROGRAM)

clean:
	rm -f $(PROGRAM)


