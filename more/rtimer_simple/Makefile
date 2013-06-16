CONTIKI_PROJECT = rtimer_simple
all: $(CONTIKI_PROJECT)

CONTIKI = ../../contiki
OPENCM3_BASE=../../libopencm3
#OPENCM3_BASE=libopencm3
TARGETDIRS += ../../platform
APPDIRS += ${addprefix ../../karl-apps/, $(APPS)}
APPS+=

include $(CONTIKI)/Makefile.include
