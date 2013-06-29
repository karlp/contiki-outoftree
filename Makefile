CONTIKI_PROJECT = foo
all: $(CONTIKI_PROJECT)

CONTIKI = contiki
OPENCM3_BASE=libopencm3
TARGETDIRS += platform
APPDIRS += ${addprefix karl-apps/, $(APPS)}
APPS+=blipper serial-shell

include $(CONTIKI)/Makefile.include
