TARGET := rootfs

all: rootfs.yaffs2 rootfs.jffs2

rootfs.yaffs2: mknod
	mkyaffs2image rootfs rootfs.yaffs2

rootfs.jffs2: mknod
	mkfs.jffs2 -n -s 2048 -e 128KiB -d rootfs -o rootfs.jffs2

mknod: delnod
	mkdir -p rootfs/dev; \
	cd rootfs/dev; \
	sudo mknod console c 5 1; \
	sudo mknod null c 1 3;

delnod:
	rm -f rootfs/dev/console; \
	rm -f rootfs/dev/null

clean:
	rm -f rootfs.jffs2 rootfs.yaffs2

distclean: clean

.PHONY: all clean distclean

