all:
	$(CC) $(CFLAGS) shim_marker.c shim_clt.c \
		$(LDFLAGS) -lxcselect \
		-o shim_clt

	$(CC) $(CFLAGS) shim_marker.c shim_xcode.c \
		$(LDFLAGS) -lxcselect \
		-o shim_xcode

	$(CC) $(CFLAGS) shim_marker.c xcrun.c \
		$(LDFLAGS) -lxcselect \
		-o xcrun

install: all
	install -d $(DESTDIR)/$(PREFIX)/bin
	for clt in `cat shim_clt.txt`; do \
		cp shim_clt $(DESTDIR)/$(PREFIX)/bin/$$clt; \
	done
	for xcode in `cat shim_xcode.txt`; do \
		cp shim_xcode $(DESTDIR)/$(PREFIX)/bin/$$xcode; \
	done
	install -m0755 xcrun $(DESTDIR)/$(PREFIX)/bin
