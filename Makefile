# Paul Sladen, 2009-07-29

MODULES=br_safety.o misc.o powercontrol.o speedcontrol.o traction.o traincontrol.o vigilance.o windshield.o
UTILS=confload.o tools.o
CFLAGS=-shared -fPIC
LDFLAGS=-shared

OS_Ats1.so: atsplugin.o confload.o tools.o $(MODULES)
	$(LD) -o $@ $(LDFLAGS) $^

# Over-enthusiastic circular inclusion...
atsplugin.cpp: atsplugin.h \
  defs.h confload.h tools.h \
  traincontrol.h powercontrol.h windshield.h br_safety.h vigilance.h speedcontrol.h misc.h traction.h

%.o: %.cpp %.h atsplugin.h
	$(CC) -c -o $@ $(CFLAGS) $<

clean:
	-rm -f atsplugin.o $(UTILS) $(MODULES) 
	-rm -f OS_Ats1.so

.PHONY: clean