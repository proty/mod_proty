APXS_FLAGS = -lproty -lprotyc -I/usr/include/proty
SOURCES = mod_proty.c

all:
	apxs -c $(APXS_FLAGS) $(SOURCES)

install:
	apxs -c -i $(APXS_FLAGS) $(SOURCES)
