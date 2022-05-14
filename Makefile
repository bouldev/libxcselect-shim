XCSELECT_VER = 2384

PREFIX  := /usr
CFLAGS  := -D_XC_INTN -DXCSELECT_VER=\"$(XCSELECT_VER)\" -I.
LDFLAGS := -dynamiclib

ifeq ($(shell sysctl -n hw.product | grep "Mac" &> /dev/null && echo 0), 0)
CC	:= xcrun --sdk iphoneos cc
CFLAGS	+= -target arm64-apple-ios12.0 -mios-version-min=12.0
LDFLAGS	+= -liosexec
DEBUG	:= 0
else
ifeq ($(shell sysctl -n hw.machine | grep -E "iPhone|iPad|iPod" &> /dev/null && echo 0), 0)
CC	:= cc
CFLAGS	+= -target arm64-apple-ios12.0 -mios-version-min=12.0
ifeq ($(shell [ -d /usr/share/SDKs/iPhoneOS.sdk ] && echo 0), 0)
CFLAGS	+= -isysroot /usr/share/SDKs/iPhoneOS.sdk
endif
LDFLAGS += -liosexec
DEBUG	:= 0
endif
endif

ifeq ($(DEBUG), 1)
CFLAGS += -g -O0
endif

all:
	$(CC) $(CFLAGS) src/libxcselect/*.c $(LDFLAGS) \
		-Wl,-install_name,$(PREFIX)/lib/libxcselect.dylib \
		-Wl,-current_version,1.0.0,-compatibility_version,1.0.0,-source_version,$(XCSELECT_VER) \
		-o libxcselect.dylib -Wl,-no_encryption

install: all
	install -d $(DESTDIR)/$(PREFIX)/{lib,include}
	install -m0644 xcselect{,_private}.h $(DESTDIR)/$(PREFIX)/include
	sed -i "/#ifdef _XC_INTN/,/\/\/ _XC_INTN/d" $(DESTDIR)/$(PREFIX)/include/xcselect_private.h
	install -m0755 libxcselect.dylib $(DESTDIR)/$(PREFIX)/lib
	install -d $(DESTDIR)/etc
	install -m0644 xcselect.conf.sample $(DESTDIR)/etc
