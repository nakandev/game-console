SUBDIRS  = ./hw
SUBDIRS += ./sw

all : $(SUBDIRS)
	mv -f hw/lib*.a .
	mv -f sw/lib*.a .

clean : $(SUBDIRS)
	rm ./lib*.a

$(SUBDIRS) : FORCE
	make -C $@ $(MAKECMDGOALS)

FORCE:

