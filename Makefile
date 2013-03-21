CONTIKI_PROJECT = foo
all: $(CONTIKI_PROJECT)

#UIP_CONF_IPV6=1

CONTIKI = contiki
TARGETDIRS += platform
include $(CONTIKI)/Makefile.include
