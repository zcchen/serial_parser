BUILD_DIR   = build

pwd         = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))

MAKEFLAGS  += --no-print-directory

CMAKEFLAGS  =

.PHONY:     all
.PHONY:     _all
.PHONY:     cmaking
.PHONY:     cleanall
.PHONY:     Makefile

_all: cmaking
	make -C $(BUILD_DIR) all

cmaking: $(BUILD_DIR)
	cmake $(CMAKEFLAGS) -B $< $(pwd)

%: ${BUILD_DIR}
	make -C $(BUILD_DIR) $@

$(BUILD_DIR):
	mkdir -p $@

cleanall:
	-rm -rf $(BUILD_DIR)

