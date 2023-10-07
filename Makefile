NUM_LANGUAGES		?=   2
MAX_ITEMS_PER_DIR	?=  30
MAX_PKMN			?= 1010
ACHIEVEMENT_LEN		?= 100
UISTRING_LEN		?= 250
MAPSTRING_LEN		?= 800
PKMNPHRS_LEN		?= 150
BADGENAME_LEN		?=  50

ifdef LOCAL
FSROOT				?= out/FSROOT/PICS/SPRITES
OUT					?= out/include
else
FSROOT				?= ../../FSROOT/PICS/SPRITES
OUT					?= ../../arm9/include
endif

CC			=	g++
BUILD       :=	build
SOURCES     :=	source
DATA     	:=	data

CFLAGS      :=	-Wall -DNUM_LANGUAGES=$(NUM_LANGUAGES) \
				-DMAX_ITEMS_PER_DIR=$(MAX_ITEMS_PER_DIR) -DMAX_PKMN=$(MAX_PKMN) \
				-DFSROOT=\"$(FSROOT)\" -DOUT=\"$(OUT)\" -g3 -ggdb
CXXFLAGS    :=	$(CFLAGS) -std=c++2a -fsanitize=undefined
LDFLAGS     := -lubsan -lpng

DATA_FILES	:=  $(addprefix $(DATA)/, $(foreach dir, $(DATA),$(notdir $(wildcard $(dir)/*.csv))))
CPPFILES	:=	gfxdata.cpp bitmap.cpp
OFILES		:=	$(addprefix $(BUILD)/, $(CPPFILES:.cpp=.o) )

fsdata: pkmnSprite icon iconArray berrytree
ifdef LOCAL
	@mkdir -p $(FSROOT)
	@mkdir -p $(OUT)
endif
	@mkdir -p $(BUILD)
	./berrytree data/berrytree/ BERRIES/
	./iconArray data/tmhm/ tmhm 32 32 1
	./icon data/item/ item 32 32 1
	./pkmnSprite data/pkmn/ frnt 96 96 1
	./pkmnSprite data/pkmn-back/ back 96 96 1
	./pkmnSprite data/pkmn-icon/ icon 32 32 1
	touch fsdata

pkmn_fsdata: pkmnSprite
ifdef LOCAL
	@mkdir -p $(FSROOT)
	@mkdir -p $(OUT)
endif
	@mkdir -p $(BUILD)
	./pkmnSprite data/pkmn/ frnt 96 96 1
	./pkmnSprite data/pkmn-back/ back 96 96 1
	./pkmnSprite data/pkmn-icon/ icon 32 32 1
	touch fsdata


pkmnSprite: $(OFILES) $(BUILD)/pkmnSprite.o
	$(CC) $(LDFLAGS) -o $@ $^

berrytree: $(OFILES) $(BUILD)/berrytree.o
	$(CC) $(LDFLAGS) -o $@ $^

icon: $(OFILES) $(BUILD)/icon.o
	$(CC) $(LDFLAGS) -o $@ $^

iconArray: $(OFILES) $(BUILD)/iconArray.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	@rm -r $(BUILD)
	@rm pkmnSprite
	@rm icon
	@rm iconArray

$(BUILD)/%.o: $(SOURCES)/%.cpp
	@mkdir -p $(BUILD)
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(SOURCES)/%.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(OBJFILES:.o=.d)

