CONTIKI_PROJECT = foo
all: $(CONTIKI_PROJECT)

CONTIKI = contiki
TARGETDIRS += platform
APPDIRS += ${addprefix karl-apps/, $(APPS)}
APPS+=blipper

include $(CONTIKI)/Makefile.include
