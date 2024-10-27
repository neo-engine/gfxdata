#include <filesystem>
#include <map>
#include <string>
#include <vector>

using namespace std;
namespace fs = filesystem;
typedef long long unsigned u64;
typedef long long int      s64;
typedef unsigned           u32;
typedef int                s32;
typedef unsigned short     u16;
typedef short int          s16;
typedef unsigned char      u8;
typedef signed char        s8;

struct fsdataInfo {
    u16 m_maxPkmn = 0;
    u16 m_maxItem = 0;

    u16 m_maxMove   = 0;                 // valid move idx 0..m_maxMove
    u8  m_languages = NUM_LANGUAGES;     // num languages
    u8  m_fileSplit = MAX_ITEMS_PER_DIR; // max num files per dir

    u8 m_maxNavBG     = 0; // valid subscrn wallpaper 0..m_maxNavBg
    u8 m_defaultNavBG = 0;
};

struct pkmnFormeData {
    u8  m_types[ 2 ];
    u16 m_abilities[ 4 ]; // abilities
    u8  m_bases[ 6 ];     // base values (hp, atk, def, satk, sdef, spd)
    u16 m_expYield;
    u8  m_genderRatio;  // from pkmnGenderType
    u8  m_size;         // in dm
    u16 m_weight;       // in 100g
    u8  m_colorShape;   // (color << 4) | shape
    u8  m_evYield[ 6 ]; // (hp, atk, def, satk, sdef, spd)
    u16 m_items[ 4 ];   // possible held items: 1%, 5%, 50%, 100%
    u8  m_eggGroups;    // (eg1 << 4) | eg2;
};

struct pkmnData {
    pkmnFormeData m_baseForme;
    u8            m_expTypeFormeCnt; // (ExpType << 5) | FormeCnt
    u8            m_eggCycles;
    u8            m_catchrate;
    u8            m_baseFriend;
};

enum evolutionType : u8 {
    NO_TYPE    = 0,
    LEVEL_UP   = 20, // level up
    ITEM       = 1,  // evolution item
    TRADE      = 2,  // trade
    TRADE_ITEM = 3,  // trade while holding item
    TRADE_PKMN = 4,  // trade with specific species

    TIME                = 5,  // level up at specific time
    PLACE               = 6,  // level up at specific place
    MOVE                = 7,  // level up with certain move known
    FRIEND              = 8,  // level up with high friendship
    LEVEL_UP_AND_PLACE  = 10, // level up at specific place with a minimum level
    FRIEND_AND_TIME     = 11, // level up with high friendship at a specific time
    LEVEL_UP_AND_TIME   = 12, // level up at a specific time with a minimum level
    LEVEL_UP_AND_GENDER = 13, // level up with a specific gender

    ITEM_PLACE  = 22, // evolution item at a special place
    ITEM_GENDER = 25, // evolution item with special condition
    ITEM_HOLD   = 24, // hold item + level up + time

    CONTEST = 15, // A specific contest stat

    SPECIAL_EVOL = 16, // totally crazy stuff
};

struct pkmnEvolution {
    evolutionType m_type;
    u8            m_targetForme;
    u16           m_target;
    u16           m_param1;
    u16           m_param2;
};

struct pkmnEvolveData {
    u16           m_evolutionCount;
    u16           m_preEvolution;
    u32           m_baseEvolution;
    pkmnEvolution m_evolutions[ 10 ];
};

typedef std::vector<std::pair<u16, u16>> pkmnLearnsetData; // (level, moveid)

struct itemData {
    u8  m_itemType;
    u8  m_effect; // Effect index
    u16 m_param1; // Effect param 1
    u16 m_param2; // Effect param 2
    u16 m_param3; // Effect param 3
    u16 m_sellPrice;
    u16 m_buyPrice; // unused
};

struct berry {
    u16 m_size;     // fluff
    u8  m_firmness; // fluff
    u8  m_growthTime;
    u8  m_yield;
    u8  m_naturalGiftPower;
    u8  m_naturalGiftType;
    u8  m_smoothness;
    u8  m_flavor[ 5 ];
};

enum moveFlags : long long unsigned {
    /** Ignores a target's substitute. */
    AUTHENTIC = ( 1 << 0 ),
    /** Power is multiplied by 1.5 when used by a Pokemon with the Strong Jaw Ability. */
    BITE = ( 1 << 1 ),
    /** Has no effect on Pokemon with the Bulletproof Ability. */
    BULLET = ( 1 << 2 ),
    /** The user is unable to make a move between turns. */
    CHARGE = ( 1 << 3 ),
    /** Makes contact. */
    CONTACT = ( 1 << 4 ),
    /** When used by a Pokemon, other Pokemon with the Dancer Ability can attempt to execute the
       same move. */
    DANCE = ( 1 << 5 ),
    /** Thaws the user if executed successfully while the user is frozen. */
    DEFROST = ( 1 << 6 ),
    /** Can target a Pokemon positioned anywhere in a Triple Battle. */
    DISTANCE = ( 1 << 7 ),
    /** Prevented from being executed or selected during Gravity's effect. */
    GRAVITY = ( 1 << 8 ),
    /** Prevented from being executed or selected during Heal Block's effect. */
    HEAL = ( 1 << 9 ),
    /** Can be copied by Mirror Move. */
    MIRROR = ( 1 << 10 ),
    /** Recoil halves HP */
    MINDBLOWNRECOIL = ( 1 << 11 ),
    /** Prevented from being executed or selected in a Sky Battle. */
    NONSKY = ( 1 << 12 ),
    /** Has no effect on Grass-type Pokemon, Pokemon with the Overcoat Ability, and Pokemon holding
       Safety Goggles. */
    POWDER = ( 1 << 13 ),
    /** Blocked by Detect, Protect, Spiky Shield, and if not a Status move, King's Shield. */
    PROTECT = ( 1 << 14 ),
    /** Power is multiplied by 1.5 when used by a Pokemon with the Mega Launcher Ability. */
    PULSE = ( 1 << 15 ),
    /** Power is multiplied by 1.2 when used by a Pokemon with the Iron Fist Ability. */
    PUNCH = ( 1 << 16 ),
    /** If this move is successful, the user must recharge on the following turn and cannot make a
       move. */
    RECHARGE = ( 1 << 17 ),
    /** Bounced back to the original user by Magic Coat or the Magic Bounce Ability. */
    REFLECTABLE = ( 1 << 18 ),
    /** Can be stolen from the original user and instead used by another Pokemon using Snatch. */
    SNATCH = ( 1 << 19 ),
    /** Has no effect on Pokemon with the Soundproof Ability. */
    SOUND = ( 1 << 20 ),
    /** Forces the pokemon to use the move for another 1-2 turns. */
    LOCKEDMOVE = ( 1 << 21 ),
    /** Maintains the rage counter */
    RAGE = ( 1 << 22 ),
    /** Roost */
    ROOST = ( 1 << 23 ),
    /** Uproar */
    UPROAR = ( 1 << 24 ),
    /** Self-Switch: User switches after successful use */
    SELFSWITCH = ( 1 << 25 ),
    /** Use source defensive stats as offensive stats */
    DEFASOFF = ( 1 << 26 ),
    /** User is damaged if the attack misses */
    CRASHDAMAGE = ( 1 << 27 ),
    /** OHKO move */
    OHKO = ( 1 << 28 ),
    /** PKMN self-destructs */
    SELFDESTRUCT = ( 1 << 29 ),
    /** PKMN self-destructs */
    SELFDESTRUCTHIT = ( 1 << 30 ),
    /** move cannot be sketched */
    NOSKETCH = ( 1LLU << 31 ),
    /** move pp cannot be increased */
    NOPPBOOST = ( 1LLU << 32 ),
    /** move can be used while asleep */
    SLEEPUSABLE = ( 1LLU << 33 ),
    /** target cannot faint due to move */
    NOFAINT = ( 1LLU << 34 ),
    /** breaks protect */
    BREAKSPROTECT = ( 1LLU << 35 ),
    /** ignores type immunities */
    IGNOREIMMUNITY = ( 1LLU << 36 ),
    /** ignores ground type immunity */
    IGNOREIMMUNITYGROUND = ( 1LLU << 37 ),
    /** ignores abilities */
    IGNOREABILITY = ( 1LLU << 38 ),
    /** ignores defense boosts */
    IGNOREDEFS = ( 1LLU << 39 ),
    /** ignores evasion boosts */
    IGNOREEVASION = ( 1LLU << 40 ),
    /** Defrosts the target */
    DEFROSTTARGET = ( 1LLU << 41 ),
    /** Forces the target to switch out */
    FORCESWITCH = ( 1LLU << 42 ),
    /** Will always land a critical hit */
    WILLCRIT = ( 1LLU << 43 ),
    /** Move uses atk/satk of the target */
    TARGETOFFENSIVES = ( 1LLU << 44 ),
    /** Move hits at a later time */
    FUTUREMOVE = ( 1LLU << 45 ),
    /** Beton pass */
    BATONPASS = ( 1LLU << 46 ),
    /** OHKO move (useless on ice-type pkmn) */
    OHKOICE = ( 1LLU << 47 ),
};

enum targets : u8 {
    NO_TARGET             = 0,
    ANY                   = 1,
    NORMAL                = 1,
    ANY_FOE               = 3,
    ADJACENT_FOE          = 3,
    ADJACENT_ALLY         = 2,
    ALLY                  = 2,
    SELF                  = 5,
    ALLY_OR_SELF          = 4,
    ADJACENT_ALLY_OR_SELF = 4,
    RANDOM                = 6, // single-target, automatic
    ALL_ALLIES            = 13,
    ALLIES                = 13,
    ALL_FOES              = 8, // spread
    ALL_ADJACENT_FOES     = 8,
    ALL_FOES_AND_ALLY     = 7,
    ALL_ADJACENT          = 7,
    ALLY_SIDE             = 9,
    FOE_SIDE              = 10,
    FIELD                 = 11, // field
    ALL                   = 11,
    SCRIPTED              = 14,
    ALLY_TEAM             = 15,
};

enum moveHitTypes : u8 { NOOP = 0, PHYSICAL = 1, SPECIAL = 2, STATUS = 3 };

#define HP       0
#define ATK      1
#define DEF      2
#define SATK     3
#define SDEF     4
#define SPEED    5
#define EVASION  6
#define ACCURACY 7

struct boosts {
    u32 m_boosts;

    constexpr boosts( ) {
        m_boosts = 0;
        for( u8 i = 0; i < 8; ++i ) { setBoost( i, 0 ); }
    }

    constexpr void setBoost( u8 p_stat, s8 p_val ) {
        p_val += 7;

        m_boosts &= ( 0xFFFFFFFF - ( 0xF << ( 4 * p_stat ) ) );
        m_boosts |= ( p_val << ( 4 * p_stat ) );
    }
    constexpr s8 getBoost( u8 p_stat ) const {
        return ( ( m_boosts >> ( 4 * p_stat ) ) & 0xF ) - 7;
    }

    /*
     * @returns the boost for the specified stat in the range [ 0, 15 ]; 7 being no
     * boost.
     */
    constexpr u8 getShiftedBoost( u8 p_stat ) const {
        return ( ( m_boosts >> ( 4 * p_stat ) ) & 0xF );
    }

    /*
     * @brief: Adds the given boosts to the boosts.
     * @returns: the change in boosts.
     */
    inline boosts addBoosts( boosts p_other ) {
        boosts res = boosts( );
        for( u8 i = 0; i < 8; ++i ) {
            s8 old = getBoost( i );
            setBoost(
                i, std::min( s8( 6 ), std::max( s8( -6 ), s8( old + p_other.getBoost( i ) ) ) ) );
            res.setBoost( i, getBoost( i ) - old );
        }
        return res;
    }

    /*
     * @brief: Inverts all boosts.
     */
    inline void invert( ) {
        for( u8 i = 0; i < 8; ++i ) { setBoost( i, -getBoost( i ) ); }
    }

    constexpr auto operator<=>( const boosts& ) const = default;
};

enum volatileStatus : u64 {
    NONE             = 0,
    CONFUSION        = ( 1 << 0 ),
    OBSTRUCT         = ( 1 << 1 ),
    PARTIALLYTRAPPED = ( 1 << 2 ),
    FLINCH           = ( 1 << 3 ),
    OCTOLOCK         = ( 1 << 4 ),
    TARSHOT          = ( 1 << 5 ),
    NORETREAT        = ( 1 << 6 ),
    LASERFOCUS       = ( 1 << 7 ),
    SPOTLIGHT        = ( 1 << 8 ),
    BANEFULBUNKER    = ( 1 << 9 ),
    SMACKDOWN        = ( 1 << 10 ),
    POWDERED         = ( 1 << 11 ),
    SPIKYSHIELD      = ( 1 << 12 ),
    KINGSSHIELD      = ( 1 << 13 ),
    ELECTRIFY        = ( 1 << 14 ),
    RAGEPOWDER       = ( 1 << 15 ),
    TELEKINESIS      = ( 1 << 16 ),
    MAGNETRISE       = ( 1 << 17 ),
    AQUARING         = ( 1 << 18 ),
    GASTROACID       = ( 1 << 19 ),
    POWERTRICK       = ( 1 << 20 ),
    HEALBLOCK        = ( 1 << 21 ),
    EMBARGO          = ( 1 << 22 ),
    MIRACLEEYE       = ( 1 << 23 ),
    SUBSTITUTE       = ( 1 << 24 ),
    BIDE             = ( 1 << 25 ),
    FOCUSENERGY      = ( 1 << 26 ),
    DEFENSECURL      = ( 1 << 27 ),
    MINIMIZE         = ( 1 << 28 ),
    LEECHSEED        = ( 1 << 29 ),
    DISABLE          = ( 1 << 30 ),
    FORESIGHT        = ( 1LLU << 31 ),
    SNATCH_          = ( 1LLU << 32 ),
    GRUDGE           = ( 1LLU << 33 ),
    IMPRISON         = ( 1LLU << 34 ),
    YAWN             = ( 1LLU << 35 ),
    MAGICCOAT        = ( 1LLU << 36 ),
    INGRAIN          = ( 1LLU << 37 ),
    HELPINGHAND      = ( 1LLU << 38 ),
    TAUNT            = ( 1LLU << 39 ),
    CHARGE_          = ( 1LLU << 40 ),
    FOLLOWME         = ( 1LLU << 41 ),
    TORMENT          = ( 1LLU << 42 ),
    ATTRACT          = ( 1LLU << 43 ),
    ENDURE           = ( 1LLU << 44 ),
    PROTECT_         = ( 1LLU << 45 ),
    DESTINYBOND      = ( 1LLU << 46 ),
    CURSE            = ( 1LLU << 47 ),
    NIGHTMARE        = ( 1LLU << 48 ),
    STOCKPILE        = ( 1LLU << 49 ),
    ENCORE           = ( 1LLU << 50 ),
};

struct moveData {
    u8 m_type        = 9; // ???
    u8 m_contestType = 0; // Clever, Smart, ...
    u8 m_basePower   = 0;
    u8 m_pp          = 1;

    moveHitTypes m_category : 8          = (moveHitTypes) 0;
    moveHitTypes m_defensiveCategory : 8 = (moveHitTypes) 0; // category used for defending pkmn
    u8           m_accuracy              = 0;                // 255: always hit
    s8           m_priority              = 0;

    u32 m_sideCondition = 0; // side introduced by the move (reflect, etc)

    u8 m_weather       = 0; // weather introduced by the move
    u8 m_pseudoWeather = 0; // pseudo weather introduced by the move
    u8 m_terrain       = 0; // terrain introduced by the move
    u8 m_status        = 0;

    u8      m_slotCondition = 0; // stuff introduced on the slot (wish, etc)
    u8      m_fixedDamage   = 0;
    targets m_target : 8    = (targets) 0;
    targets m_pressureTarget : 8
        = (targets) 0; // restrictions are computed based on different target than resulting effect

    u8 m_heal     = 0; // as m_heal / 240
    u8 m_recoil   = 0; // as dealt damage * m_recoil / 240
    u8 m_drain    = 0; // as dealt damage * m_recoil / 240
    u8 m_multiHit = 0; // as ( min << 8 ) | max

    u8 m_critRatio       = 0;
    u8 m_secondaryChance = 0; // chance that the secondary effect triggers
    u8 m_secondaryStatus = 0;
    u8 m_unused          = 0;

    volatileStatus m_volatileStatus          = (volatileStatus) 0; // confusion, etc
    volatileStatus m_secondaryVolatileStatus = (volatileStatus) 0; // confusion, etc

    boosts m_boosts              = boosts( ); // Status ``boosts'' for the target
    boosts m_selfBoosts          = boosts( ); // Status ``boosts'' for the user (if target != user)
    boosts m_secondaryBoosts     = boosts( ); // Stat ``boosts'' for the target
    boosts m_secondarySelfBoosts = boosts( ); // Stat ``boosts'' for the user (if target != user)

    moveFlags m_flags = (moveFlags) 0;
};

struct names {
    char* m_name[ NUM_LANGUAGES ] = { 0 };
};
struct descrs {
    char* m_descr[ NUM_LANGUAGES ] = { 0 };
};
struct data {
    char m_data[ 200 ][ NUM_LANGUAGES ];
};

struct trainerPokemon {
    // 30
    u16 m_speciesId;
    u8  m_forme; // BIT( 6 ): pkmn is female, BIT( 7 ): pkmn is genderless
    u8  m_level;

    u16 m_ability;
    u16 m_heldItem;

    u16 m_moves[ 4 ];

    u8 m_ev[ 6 ];
    u8 m_iv[ 6 ];
    u8 m_shiny;
    u8 m_nature;
};

struct trainerData {
    static constexpr u8 NUM_ITEMS = 5;
    static constexpr u8 NUM_PKMN  = 6;

    u16 m_trainerClass : 8;
    u16 m_trainerBG : 8;

    u16 m_battlePlat1 : 8;
    u16 m_battlePlat2 : 8;

    u16 m_battleBG : 8;
    u16 m_AILevel : 4;
    u16 m_numPokemon : 3;
    u16 m_forceDoubleBattle : 1;

    u16 m_moneyMultiplier;

    u16            m_items[ NUM_ITEMS ];
    trainerPokemon m_pokemon[ NUM_PKMN ];
};

size_t         getLength( u8 p_c );
char           getValue( char* p_text, size_t* p_readIdx );
char*          fixEncoding( char* p_utf8 );
u8             getType( char* p_str );
u8             getFirmness( char* p_str );
u8             getContestType( char* p_str );
u8             getLevelUpType( char* p_str );
u8             getEggGroup( char* p_str );
u8             getExpType( char* p_str );
u8             getGender( char* p_str );
u8             getColor( char* p_str );
u8             getShape( char* p_str );
u8             getItemType( char* p_str );
u8             getMedicineEffect( char* p_str );
volatileStatus getVolatileStatus( char* p_str );
moveFlags      getMoveFlag( char* p_str );
moveFlags      parseMoveFlags( char* p_buffer );
targets        getTarget( char* p_str );
u8             getFixedDamage( char* p_str );
moveHitTypes   getMoveCategory( char* p_str, moveHitTypes p_default = (moveHitTypes) 0 );
u8             getWeather( char* p_str );
u8             getPseudoWeather( char* p_str );
u8             getTerrain( char* p_str );
u16            getSideCondition( char* p_str );
u8             getSlotCondition( char* p_str );
u8             getStatus( char* p_str );
u8             getFrameType( char* p_str );
u8             getNumberOrNone( char* p_str );
u8             parseFraction( char* p_str );
u8             parseMultiHit( char* p_str );
void           parseBoost( char* p_str, boosts& p_out );
boosts         parseBoosts( char* p_buffer );

FILE* getFilePtr( string p_prefix, u16 p_index, u8 p_digits, string p_ext = ".data",
                  u8 p_formeId = 0 );

void printNormalized( char* p_string, FILE* p_f );

void readNames( char* p_path, vector<names>& p_out, int p_maxLen = 29, u8 p_offset = 0 );
void readNames( char* p_path, vector<pair<string, names>>& p_out );
void readNames( char* p_path, map<u16, names>& p_out );
void readDescrs( char* p_path, vector<descrs>& p_out, int p_maxLen = 249 );
