all : mario

mario : build/game.o build/main.o build/sound.o build/tileobject.o \
        build/tileplane.o build/glerror.o build/texture.o build/plumber.o \
		build/interact.o build/mover.o build/enemybase.o build/updater.o
	$(CXX) $(CFLAGS) $^ $(LDFLAGS) -o $@ 

build/%.o : src/%.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

build/%.o : src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

# Extra build dependencies

# Local system configuration - not used right now
include configuration

clean ::
	@-rm -f deps/* build/* mario

install-data : all
	#nothing target, FIXME - fill in (for debian most likely)

install-bin : all
	#nothing target, FIXME - fill in (for debian most likely)
	#assume DESTDIR is the prefix for installing
	mkdir -p $(DESTDIR)/usr/bin/

dpkg :
	#NOTE this depends on ~/src existing and that you are running the repo
	#if you know about reprepro you can fix this to match your setup
	rm -f ~/src/*.dsc
	rm -f ~/src/*.diff.gz
	rm -f ~/src/*.deb
	rm -f ~/src/*.changes
	cvs-buildpackage -W ~/src -UHEAD -THEAD -rfakeroot
	sudo reprepro -b /webroot/apt include sid $(HOME)/src/*.changes

