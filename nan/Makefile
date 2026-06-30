CC := $(CROSS_COMPILE)gcc

O ?= $(shell pwd)

NAN_UTIL_SRC ?= .
NAN_API_SRC = $(NAN_UTIL_SRC)/src/rtw_nan_api

NAN_API_OBJS = \
	$(O)/nan_event.o \
	$(O)/nan_scenario.o \
	$(O)/rtw_nan_cmd.o \
	$(O)/sha256.o

INCLUDES += -I$(NAN_API_SRC)
INCLUDES += -I$(NAN_UTIL_SRC)/rtw_aware
INCLUDES += -I$(NAN_UTIL_SRC)/src/vendor_wrapper
INCLUDES += $(GLIB_INC) $(JSON_INC)

LIBS += -L$(O) -lnanapi
LIBS += -lrt

.PHONY: all clean prepare

all: prepare $(O)/nan_vendor_wrapper $(O)/rtw_aware $(O)/iw

prepare:
	mkdir -p $(O)

$(O)/libnanapi.so: $(NAN_API_OBJS)
	$(CC) -shared -o $@ $^ $(LDFLAGS) $(CFLAGS) $(GLIB_LIBS) $(JSON_LIBS)

$(O)/nan_vendor_wrapper: $(NAN_UTIL_SRC)/src/vendor_wrapper/nan_vendor_wrapper.c $(O)/libnanapi.so
	$(CC) -o $@ $< $(INCLUDES) -L$(O) -lnanapi $(LDFLAGS) $(CFLAGS) $(GLIB_LIBS) $(JSON_LIBS)

$(O)/rtw_aware: $(NAN_UTIL_SRC)/rtw_aware/rtw_aware.c $(O)/libnanapi.so
	$(CC) -o $@ $< $(INCLUDES) $(LIBS) $(LDFLAGS) $(CFLAGS) $(GLIB_LIBS) $(JSON_LIBS)

$(O)/%.o: $(NAN_API_SRC)/%.c
	$(CC) -c -fpic $< -o $@ $(INCLUDES) $(LDFLAGS) $(CFLAGS) $(GLIB_LIBS) $(JSON_LIBS)

# ---- iw wireless tool ----

IW_TAR = $(NAN_UTIL_SRC)/src/iw-5.4.tar.gz
IW_DIR = $(O)/iw-5.4

$(IW_DIR)/Makefile: $(IW_TAR) | prepare
	tar -zxf $(IW_TAR) -C $(O)

$(IW_DIR)/iw: $(IW_DIR)/Makefile
	$(MAKE) -C $(IW_DIR) \
	    CC=$(CC) \
	    CFLAGS="$(CFLAGS) -DCONFIG_LIBNL30 $(NL3_INC)" \
	    LDFLAGS="$(LDFLAGS)" \
	    LIBS="$(NL3_LIB) -lnl-genl-3" \
	    NO_PKG_CONFIG=Y

$(O)/iw: $(IW_DIR)/iw
	cp $(IW_DIR)/iw $@

clean:
	rm -f $(O)/nan_vendor_wrapper
	rm -f $(O)/rtw_aware
	rm -f $(O)/libnanapi.so
	rm -f $(O)/iw
	rm -f $(NAN_API_OBJS)
	rm -rf $(IW_DIR)
