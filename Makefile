# TODO (Khangaroo): Make this process a lot less hacky (no, export did not work)
# See MakefileNSO

.PHONY: all clean skyline send xc2 xcde

PYTHON := python3
ifeq (, $(shell which python3))
	# if no python3 alias, fall back to `python` and hope it's py3
	PYTHON   := python
endif

NAME 			:= $(shell basename $(CURDIR))
NAME_LOWER		:= $(shell echo $(NAME) | tr A-Z a-z)
TID				:= 0100FF500E34A000
CODE_NAME		:= bfsw
SUBSDK_NAME		:= subsdk1

SCRIPTS_DIR		:= scripts
BUILD_DIR 		:= build

SEND_PATCH		:= $(SCRIPTS_DIR)/send_over_ftp.py
IP				:= 192.168.1.106

MAKE_NSO		:= nso.mk

xc2: TID=0100E95004038000
xc2: CODE_NAME=bf2
xc2: send
	while true; do nc 192.168.1.106 6969; done

xcde: TID=0100FF500E34A000
xcde: CODE_NAME = bfsw
xcde: send

all: skyline $(CODE_NAME).npdm

skyline:
	$(MAKE) all -f $(MAKE_NSO) MAKE_NSO=$(MAKE_NSO) BUILD=$(BUILD_DIR) TARGET=$(NAME) CODE_NAME=$(CODE_NAME)

$(CODE_NAME).npdm: $(CODE_NAME).json
	npdmtool $(CODE_NAME).json $(CODE_NAME).npdm

send: all
	$(PYTHON) $(SEND_PATCH) $(IP) $(TID) $(CODE_NAME) $(SUBSDK_NAME)

clean:
	$(MAKE) clean -f $(MAKE_NSO)
	@rm $(CODE_NAME).npdm
