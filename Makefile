NUM_LANGUAGES		?=   2
MAX_ITEMS_PER_DIR	?=  30
MAX_PKMN			?= 1030
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
CXXFLAGS    :=	$(CFLAGS) -std=c++20 -I/opt/homebrew/include/
LDFLAGS     := -L /opt/homebrew/lib/ -lpng

DATA_FILES	:=  $(addprefix $(DATA)/, $(foreach dir, $(DATA),$(notdir $(wildcard $(dir)/*.csv))))
CPPFILES	:=	gfxdata.cpp bitmap.cpp
OFILES		:=	$(addprefix $(BUILD)/, $(CPPFILES:.cpp=.o) )

fsdata:  berrytree_sprite pkmn_follow pkmn_fsdata item_icon block_icon npc_sprite
	touch fsdata

npc_sprite: npcsprite
ifdef LOCAL
	@mkdir -p $(FSROOT)
	@mkdir -p $(OUT)
endif
	@mkdir -p $(BUILD)
	./npcsprite data/npc/ npc
	touch npc_sprite

trainer_sprite: trainer
ifdef LOCAL
	@mkdir -p $(FSROOT)
	@mkdir -p $(OUT)
endif
	@mkdir -p $(BUILD)
	./trainer data/trainer/ TRAINER/
	touch trainer_sprite


#berrytree_sprite: berrytree
berrytree_sprite: npcsprite
ifdef LOCAL
	@mkdir -p $(FSROOT)
	@mkdir -p $(OUT)
endif
	@mkdir -p $(BUILD)
	#./berrytree data/berrytree/ BERRIES/
	./npcsprite data/berrytree/ berry 6
	touch berrytree_sprite

item_icon: icon iconArray
ifdef LOCAL
	@mkdir -p $(FSROOT)
	@mkdir -p $(OUT)
endif
	@mkdir -p $(BUILD)
	./iconArray data/pokeballs/ pb 32 32 1 3
	./iconArray data/tmhm/ tmhm 32 32 1
	./icon data/item/ item 32 32 1
	touch item_icon

block_icon: icon
ifdef LOCAL
	@mkdir -p $(FSROOT)
	@mkdir -p $(OUT)
endif
	@mkdir -p $(BUILD)
	./icon data/pokeblock/ pokeblock 32 32 1 6
	touch block_icon

pkmn_follow_1: convertfollowpkmn pkmnowsprite2rsd
ifdef LOCAL
	@mkdir -p $(FSROOT)
	@mkdir -p $(OUT)
endif
	@mkdir -p $(BUILD)/img
	@echo "Converting follow pkmn spritesheets"
	@echo "Gen 1"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-1/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	@echo "Gen 2"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-2/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	@echo "Gen 3"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-3/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	@echo "Gen 4"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-4/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	@echo "Gen 5"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-5/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	@echo "Gen 6"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-6/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	@echo "Gen 7"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-7/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	@echo "Gen 7.1"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-7.1/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	@echo "Gen 7.2"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-7.2/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	@echo "Gen 8"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-8/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	@echo "Gen 8.1"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-8.1/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	@echo "Gen 8.2"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-8.2/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	@echo "Gen 8.3"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-8.3/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	@echo "Gen 9"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-9/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	@echo "Gen 9.1"
	@$(foreach img,$(wildcard data/pkmn-follow/gen-9.1/*.png),./convertfollowpkmn $(img) $(BUILD)/img/$(notdir $(img));)
	touch pkmn_follow_1

pkmn_follow: pkmn_follow_1 pkmnowsprite2rsd
ifdef LOCAL
	@mkdir -p $(FSROOT)
	@mkdir -p $(OUT)
endif
	@mkdir -p $(BUILD)
	@echo "Converting to rsd"
	./pkmnowsprite2rsd $(BUILD)/img NPCP
	touch pkmn_follow

pkmn_fsdata: pkmnSprite
ifdef LOCAL
	@mkdir -p $(FSROOT)
	@mkdir -p $(OUT)
endif
	@mkdir -p $(BUILD)
	./pkmnSprite data/pkmn/ frnt 96 96 1
	./pkmnSprite data/pkmn-back/ back 96 96 1
	./pkmnSprite data/pkmn-icon/ icon 32 32 1
	touch pkmn_fsdata


pkmnSprite: $(OFILES) $(BUILD)/pkmnSprite.o
	$(CC) $(LDFLAGS) -o $@ $^

npcsprite: $(OFILES) $(BUILD)/npcsprite.o
	$(CC) $(LDFLAGS) -o $@ $^

trainer: $(OFILES) $(BUILD)/trainer.o
	$(CC) $(LDFLAGS) -o $@ $^

berrytree: $(OFILES) $(BUILD)/berrytree.o
	$(CC) $(LDFLAGS) -o $@ $^

icon: $(OFILES) $(BUILD)/icon.o
	$(CC) $(LDFLAGS) -o $@ $^

iconArray: $(OFILES) $(BUILD)/iconArray.o
	$(CC) $(LDFLAGS) -o $@ $^

convertfollowpkmn: $(OFILES) $(BUILD)/convertfollowpkmn.o
	$(CC) $(LDFLAGS) -o $@ $^

pkmnowsprite2rsd: $(OFILES) $(BUILD)/pkmnowsprite2rsd.o
	$(CC) $(LDFLAGS) -o $@ $^

clean-binary:
	@rm -r $(BUILD)
	@rm pkmnSprite
	@rm icon
	@rm iconArray
	@rm convertfollowpkmn
	@rm pkmnowsprite2rsd

clean:
	@rm -r $(BUILD)
	@rm pkmnSprite
	@rm icon
	@rm iconArray
	@rm convertfollowpkmn
	@rm pkmnowsprite2rsd
	@rm pkmn_follow
	@rm pkmn_follow_1
	@rm pkmn_fsdata
	@rm item_icon
	@rm berrytree_sprite
	@rm fsdata
	@rm npc_sprite

$(BUILD)/%.o: $(SOURCES)/%.cpp
	@mkdir -p $(BUILD)
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(SOURCES)/%.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(OBJFILES:.o=.d)

