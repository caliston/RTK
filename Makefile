PREFIX = RTK-0/0/0

all:
	make -C !RTK

dist:
	Remove $(PREFIX)/zip
	CDir $(PREFIX)
	Wipe $(PREFIX) ~CFR~V
	CDir $(PREFIX)
	ln -s !RTK $(PREFIX).!RTK
	Copy Makefile $(PREFIX).* ~CF~V
	Copy */lst $(PREFIX).* ~CF~V
	zip -r $(PREFIX)/zip $(PREFIX) -i@src/lst
	Wipe $(PREFIX) ~CFR~V
