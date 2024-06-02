.PHONY:	test project all clean

#--------------------------------------------------------------------------
#	start user settings
#--------------------------------------------------------------------------
VERSION					:= Debug
#VERSION					:= Release

PRJ_NAME				:= ptt-test

SRC						:= main.c utils.c crypto.c
SRC						+=

VPATH					:=
INC						:= utils.h crypto.h parameters.h
INC 					+=
DEFINE					:= _DEBUG
LIB						:= ssl crypto

OPTIMIZE_LEVEL			:= 2
DEBUG_LEVEL				:= gdb


#--------------------------------------------------------------------------
#	end user settings
#--------------------------------------------------------------------------

ROOT					:= $(shell pwd)

#CROSS_PREFIX			:= arm-linux-gnueabihf-
CC						:= gcc
AS						:= gcc
LD						:= gcc
CPPC					:= g++
AR						:= ar
RANLIB					:= ranlib
SIZE					:= size
OBJCOPY					:= objcopy
OBJDUMP					:= objdump

BIN_DIR					:= bin
DEBUG_DIR				:= Debug
RELEASE_DIR				:= Release
INC_DIR					:= inc
OBJ_DIR					:= obj
SRC_DIR					:= src

VPATH					+= $(INC_DIR) $(OBJ_DIR) $(SRC_DIR)
INCLUDE_PATH			:= $(INC) $(INC_DIR) $(COMM_INC_DIR) $(TOOL_PATH)/include
LIB_PATH				:= $(TOOL_PATH)/lib

LIBFLAGS				:= $(addprefix -L,$(LIB_PATH)) $(addprefix -l,$(LIB))
ARFLAGS					:= rcs

CCFLAGS					:= -Wall
CCFLAGS					+= $(addprefix -D,$(DEFINE)) $(addprefix -I,$(INCLUDE_PATH))
CCFLAGS					+= -ffunction-sections -fdata-sections

ASFLAGS					:= -Wall
ASFLAGS					+= $(addprefix -D,$(DEFINE)) $(addprefix -I,$(INCLUDE_PATH))

ifeq ($(VERSION),Debug)
IMAGE					:= $(BIN_DIR)/$(DEBUG_DIR)/$(PRJ_NAME)
CCFLAGS					+= -g$(DEBUG_LEVEL) -O0 -DDEBUG
ASFLAGS					+= -g$(DEBUG_LEVEL) -O0 -DDEBUG
endif

ifeq ($(VERSION),Release)
IMAGE					:= $(BIN_DIR)/$(RELEASE_DIR)/$(PRJ_NAME)
CCFLAGS					+= -O$(OPTIMIZE_LEVEL)
ASFLAGS					+= -O0
endif

CPPCFLAGS				:= $(CCFLAGS)
CPPCFLAGS				+= -x c++ -Weffc++
CPPCFLAGS				+= -std=gnu++0x

LDFLAGS					:= -Wl,-Map,$(IMAGE).map,--cref -Wl,--gc-sections

OBJECTS					:= $(addprefix $(OBJ_DIR)/$(VERSION)/,$(patsubst %.c, %.o,$(filter %.c,$(SRC))))
OBJECTS					+= $(addprefix $(OBJ_DIR)/$(VERSION)/,$(patsubst %.cpp, %.o,$(filter %.cpp,$(SRC))))
OBJECTS					+= $(addprefix $(OBJ_DIR)/$(VERSION)/,$(patsubst %.s, %.o,$(filter %.s,$(SRC))))
OBJECTS					+= $(addprefix $(OBJ_DIR)/$(VERSION)/,$(patsubst %.S, %.o,$(filter %.S,$(SRC))))

#--------------------------------------------------------------------------
#	targets
#--------------------------------------------------------------------------

test:
	$(CC) --version
	$(MAKE) --version
	@echo $(OBJECTS)

project:
	@test  -d $(BIN_DIR) || mkdir $(BIN_DIR) $(BIN_DIR)/$(DEBUG_DIR) $(BIN_DIR)/$(RELEASE_DIR)
	@test  -d $(OBJ_DIR) || mkdir $(OBJ_DIR) $(OBJ_DIR)/$(DEBUG_DIR) $(OBJ_DIR)/$(RELEASE_DIR)
	@test  -d $(INC_DIR) || mkdir $(INC_DIR)
	@test  -d $(SRC_DIR) || mkdir $(SRC_DIR)

all: $(VERSION)

$(VERSION): project exe lst size
	@echo "--------------------- COMPLETE -----------------------"

exe:$(IMAGE)

lst:$(IMAGE).lst

size:$(IMAGE)
	@echo
	@echo $@
	@echo "------------------------------------------------------"
	$(SIZE) $(IMAGE)

$(IMAGE).lst:$(IMAGE)
	@echo
	@echo $@
	@echo "------------------------------------------------------"
	$(OBJDUMP) -h -S -z $<  > $@

$(IMAGE):$(OBJECTS)
	@echo
	@echo $@
	@echo "------------------------------------------------------"
	$(LD) $(CCFLAGS) $(LDFLAGS) $^ -o $@ $(LIBFLAGS)

$(OBJ_DIR)/$(VERSION)/%.o:%.c
	@echo
	@echo $<
	@echo "------------------------------------------------------"
	$(CC) $(CCFLAGS) -MD -c $< -o $@

$(OBJ_DIR)/$(VERSION)/%.o:%.cpp
	@echo
	@echo $<
	@echo "------------------------------------------------------"
	$(CPPC) $(CPPCFLAGS) -MD -c $< -o $@

$(OBJ_DIR)/$(VERSION)/%.o:%.s
	@echo
	@echo $<
	@echo "------------------------------------------------------"
	$(AS) $(ASFLAGS) -c $< -o $@

$(OBJ_DIR)/$(VERSION)/%.o:%.S
	@echo
	@echo $<
	@echo "------------------------------------------------------"
	$(AS) $(ASFLAGS) -c $< -o $@

include $(wildcard $(OBJ_DIR)/$(VERSION)/*.d)

clean:
	rm -f $(OBJECTS)
	rm -f $(patsubst %.o, %.d,$(OBJECTS))
	rm -f $(IMAGE) $(IMAGE).map $(IMAGE).lst
	rm -f
	@echo "--------------------- COMPLETE -----------------------"
