.SUFFIXES:
ifeq ($(strip $(PSL1GHT)),)
$(error "PSL1GHT must be set in the environment.")
endif

include $(PSL1GHT)/Makefile.base

TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCE		:=	source
INCLUDE		:=	include
DATA		:=	data
LIBS		:=	
XDATA		:=	xdata
TITLE		:=	MrGatz85 & Cael's Open Asbestos Loader
APPID		:=	LOAD00000
CONTENTID	:=	UP0001-$(APPID)_00-0000000000000000

CFLAGS		+= -g -O2 -Wall --std=gnu99
CXXFLAGS	+= -g -O2 -Wall

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCE),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))
export BUILDDIR	:=	$(CURDIR)/$(BUILD)
export DEPSDIR	:=	$(BUILDDIR)

CFILES		:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.c)))
CXXFILES	:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:= $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.bin)))

export OFILES	:=	$(CFILES:.c=.o) \
					$(CXXFILES:.cpp=.o) \
					$(SFILES:.S=.o)

export BINFILES	:=	$(BINFILES:.bin=.bin.h)

export INCLUDES	:=	$(foreach dir,$(INCLUDE),-I$(CURDIR)/$(dir)) \
					-I$(CURDIR)/$(BUILD)

.PHONY: $(BUILD) clean

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo Clean...
	@rm -rf $(BUILD) $(OUTPUT).elf $(OUTPUT).self $(OUTPUT).a $(OUTPUT).pkg

pkg: $(BUILD)
	@echo Creating PKG...
	@mkdir -p $(BUILD)/pkg
	@mkdir -p $(BUILD)/pkg/USRDIR
	@cp $(ICON0) $(BUILD)/pkg/ 
	-@cp $(PIC0) $(BUILD)/pkg/	#Backgrounds anyone??
	-@cp $(PIC1) $(BUILD)/pkg/	#i like pictures
	@cp $(XDATA)/* $(BUILD)/pkg/USRDIR	#Kernel and any thing else you want
	@$(FSELF) -n $(BUILD)/$(TARGET).elf $(BUILD)/pkg/USRDIR/EBOOT.BIN
	@$(SFO) --title "$(TITLE)" --appid "$(APPID)" -f $(SFOXML) $(BUILD)/pkg/PARAM.SFO
	@$(PKG) --contentid $(CONTENTID) $(BUILD)/pkg/ $(OUTPUT).pkg

run: $(BUILD)
	@$(PS3LOADAPP) $(OUTPUT).self

else

DEPENDS	:= $(OFILES:.o=.d)

$(OUTPUT).self: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)
$(OFILES): $(BINFILES)

-include $(DEPENDS)

endif
