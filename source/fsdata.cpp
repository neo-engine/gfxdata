#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "fsdata.h"

size_t getLength( u8 p_c ) {
    if( p_c < 0x80 )
        return 1;
    else if( !( p_c & 0x20 ) )
        return 2;
    else if( !( p_c & 0x10 ) )
        return 3;
    else if( !( p_c & 0x08 ) )
        return 4;
    else if( !( p_c & 0x04 ) )
        return 5;
    else
        return 6;
}

char getValue( char* p_text, size_t* p_readIdx ) {
    size_t len = getLength( static_cast<u8>( p_text[ *p_readIdx ] ) );

    if( len == 1 ) {
        char c = p_text[ *p_readIdx ];
        ( *p_readIdx )++;
        if( c == '\\' ) {
            char cc = p_text[ *p_readIdx ];
            if( cc == 'r' ) {
                ( *p_readIdx )++;
                return '\r';
            }
            if( cc == 'n' ) {
                ( *p_readIdx )++;
                return '\n';
            }
            if( cc == '"' ) {
                ( *p_readIdx )++;
                return '"';
            }
        }
        return c;
    }

    unsigned int v = ( p_text[ *p_readIdx ] & ( 0xff >> ( len + 1 ) ) ) << ( ( len - 1 ) * 6 );
    ( *p_readIdx )++;
    for( len--; len > 0; len-- ) {
        v |= ( static_cast<u8>( p_text[ *p_readIdx ] ) - 0x80 ) << ( ( len - 1 ) * 6 );
        ( *p_readIdx )++;
    }
    return ( v > 0xff ) ? 0 : (char) v;
}

char* fixEncoding( char* p_utf8 ) {
    for( size_t readIndex = 0, writeIndex = 0;; writeIndex++ ) {
        if( p_utf8[ readIndex ] == 0 ) {
            p_utf8[ writeIndex ] = 0;
            while( ++writeIndex <= readIndex ) { p_utf8[ writeIndex ] = 0; }
            break;
        }

        char c = getValue( p_utf8, &readIndex );
        if( c == 0 ) {
            fprintf( stderr, "[%s] Invalid character '%c'\n", p_utf8, p_utf8[ readIndex ] );
            c = '_';
        }
        p_utf8[ writeIndex ] = c;
    }

    return p_utf8;
}

u8 getType( char* p_str ) {
    if( !strcmp( p_str, "Normal" ) ) return 0;
    if( !strcmp( p_str, "Fight" ) ) return 1;
    if( !strcmp( p_str, "Fighting" ) ) return 1;
    if( !strcmp( p_str, "Flying" ) ) return 2;
    if( !strcmp( p_str, "Poison" ) ) return 3;
    if( !strcmp( p_str, "Ground" ) ) return 4;
    if( !strcmp( p_str, "Rock" ) ) return 5;
    if( !strcmp( p_str, "Bug" ) ) return 6;
    if( !strcmp( p_str, "Ghost" ) ) return 7;
    if( !strcmp( p_str, "Steel" ) ) return 8;
    if( !strcmp( p_str, "???" ) ) return 9;
    if( !strcmp( p_str, "Water" ) ) return 10;
    if( !strcmp( p_str, "Fire" ) ) return 11;
    if( !strcmp( p_str, "Grass" ) ) return 12;
    if( !strcmp( p_str, "Electric" ) ) return 13;
    if( !strcmp( p_str, "Electr" ) ) return 13;
    if( !strcmp( p_str, "Psychic" ) ) return 14;
    if( !strcmp( p_str, "Ice" ) ) return 15;
    if( !strcmp( p_str, "Dragon" ) ) return 16;
    if( !strcmp( p_str, "Dark" ) ) return 17;
    if( !strcmp( p_str, "Fairy" ) ) return 18;

    if( !strcmp( p_str, "normal" ) ) return 0;
    if( !strcmp( p_str, "fight" ) ) return 1;
    if( !strcmp( p_str, "fighting" ) ) return 1;
    if( !strcmp( p_str, "flying" ) ) return 2;
    if( !strcmp( p_str, "poison" ) ) return 3;
    if( !strcmp( p_str, "ground" ) ) return 4;
    if( !strcmp( p_str, "rock" ) ) return 5;
    if( !strcmp( p_str, "bug" ) ) return 6;
    if( !strcmp( p_str, "ghost" ) ) return 7;
    if( !strcmp( p_str, "steel" ) ) return 8;
    if( !strcmp( p_str, "???" ) ) return 9;
    if( !strcmp( p_str, "water" ) ) return 10;
    if( !strcmp( p_str, "fire" ) ) return 11;
    if( !strcmp( p_str, "grass" ) ) return 12;
    if( !strcmp( p_str, "electric" ) ) return 13;
    if( !strcmp( p_str, "electr" ) ) return 13;
    if( !strcmp( p_str, "psychic" ) ) return 14;
    if( !strcmp( p_str, "ice" ) ) return 15;
    if( !strcmp( p_str, "dragon" ) ) return 16;
    if( !strcmp( p_str, "dark" ) ) return 17;
    if( !strcmp( p_str, "fairy" ) ) return 18;

    fprintf( stderr, "Found bad type %s.\n", p_str );
    return 9;
}

u8 getFirmness( char* p_str ) {
    if( !strcmp( p_str, "???" ) ) return 0;
    if( !strcmp( p_str, "very soft" ) ) return 1;
    if( !strcmp( p_str, "soft" ) ) return 2;
    if( !strcmp( p_str, "hard" ) ) return 3;
    if( !strcmp( p_str, "very hard" ) ) return 4;
    if( !strcmp( p_str, "super hard" ) ) return 5;

    fprintf( stderr, "Found bad firmness %s.\n", p_str );
    return 0;
}

u8 getContestType( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return 0;
    if( !strcmp( p_str, "Tough" ) ) return 1;
    if( !strcmp( p_str, "Clever" ) ) return 2;
    if( !strcmp( p_str, "Beautiful" ) ) return 3;
    if( !strcmp( p_str, "Cool" ) ) return 4;
    if( !strcmp( p_str, "Cute" ) ) return 5;

    fprintf( stderr, "Found bad contest type %s.\n", p_str );
    return 0;
}

u8 getLevelUpType( char* p_str ) {
    if( !strcmp( p_str, "Medium Fast" ) ) return 0;
    if( !strcmp( p_str, "Erratic" ) ) return 1;
    if( !strcmp( p_str, "Fluctuating" ) ) return 2;
    if( !strcmp( p_str, "Medium Slow" ) ) return 3;
    if( !strcmp( p_str, "Fast" ) ) return 4;
    if( !strcmp( p_str, "Slow" ) ) return 5;

    fprintf( stderr, "Found bad level up type %s.\n", p_str );
    return 0;
}

u8 getEggGroup( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return 0;
    if( !strcmp( p_str, "Monster" ) ) return 1;
    if( !strcmp( p_str, "Water 1" ) ) return 2;

    if( !strcmp( p_str, "Bug" ) ) return 3;
    if( !strcmp( p_str, "Flying" ) ) return 4;
    if( !strcmp( p_str, "Field" ) ) return 5;
    if( !strcmp( p_str, "Fairy" ) ) return 6;
    if( !strcmp( p_str, "Grass" ) ) return 7;
    if( !strcmp( p_str, "Human-Like" ) ) return 8;
    if( !strcmp( p_str, "Water 3" ) ) return 9;
    if( !strcmp( p_str, "Mineral" ) ) return 10;
    if( !strcmp( p_str, "Amorphous" ) ) return 11;
    if( !strcmp( p_str, "Water 2" ) ) return 12;
    if( !strcmp( p_str, "Ditto" ) ) return 13;
    if( !strcmp( p_str, "Dragon" ) ) return 14;
    if( !strcmp( p_str, "Undiscovered" ) ) return 15;

    fprintf( stderr, "Found bad egg group %s.\n", p_str );
    return 0;
}

u8 getExpType( char* p_str ) {
    if( !strcmp( p_str, "Medium Fast" ) ) return 0;
    if( !strcmp( p_str, "Erratic" ) ) return 1;
    if( !strcmp( p_str, "Fluctuating" ) ) return 2;
    if( !strcmp( p_str, "Medium Slow" ) ) return 3;
    if( !strcmp( p_str, "Fast" ) ) return 4;
    if( !strcmp( p_str, "Slow" ) ) return 5;

    fprintf( stderr, "Found bad exp type %s.\n", p_str );
    return 0;
}

u8 getGender( char* p_str ) {
    if( !strcmp( p_str, "genderless" ) ) return 255;
    if( !strcmp( p_str, "always female" ) ) return 254;
    if( !strcmp( p_str, "1m7f" ) ) return 223;
    if( !strcmp( p_str, "1m3f" ) ) return 191;
    if( !strcmp( p_str, "1m1f" ) ) return 127;
    if( !strcmp( p_str, "3m1f" ) ) return 63;
    if( !strcmp( p_str, "7m1f" ) ) return 31;
    if( !strcmp( p_str, "always male" ) ) return 0;

    fprintf( stderr, "Found bad gender value %s.\n", p_str );
    return 255;
}

u8 getColor( char* p_str ) {
    if( !strcmp( p_str, "Red" ) ) return 0;
    if( !strcmp( p_str, "Blue" ) ) return 1;
    if( !strcmp( p_str, "Yellow" ) ) return 2;
    if( !strcmp( p_str, "Green" ) ) return 3;
    if( !strcmp( p_str, "Black" ) ) return 4;
    if( !strcmp( p_str, "Brown" ) ) return 5;
    if( !strcmp( p_str, "Purple" ) ) return 6;
    if( !strcmp( p_str, "Gray" ) ) return 7;
    if( !strcmp( p_str, "White" ) ) return 8;
    if( !strcmp( p_str, "Pink" ) ) return 9;

    fprintf( stderr, "Found bad color %s.\n", p_str );
    return 0;
}

u8 getShape( char* p_str ) {
    if( !strcmp( p_str, "Pomaceous" ) ) return 1;
    if( !strcmp( p_str, "Caudal" ) ) return 2;
    if( !strcmp( p_str, "Ichthyic" ) ) return 3;
    if( !strcmp( p_str, "Brachial" ) ) return 4;
    if( !strcmp( p_str, "Alvine" ) ) return 5;
    if( !strcmp( p_str, "Sciurine" ) ) return 6;
    if( !strcmp( p_str, "Crural" ) ) return 7;
    if( !strcmp( p_str, "Mensal" ) ) return 8;
    if( !strcmp( p_str, "Alar" ) ) return 9;
    if( !strcmp( p_str, "Cilial" ) ) return 10;
    if( !strcmp( p_str, "Polycephalic" ) ) return 11;
    if( !strcmp( p_str, "Anthropomorphic" ) ) return 12;
    if( !strcmp( p_str, "Lepidopterous" ) ) return 13;
    if( !strcmp( p_str, "Chitinous" ) ) return 14;

    fprintf( stderr, "Found bad shape %s.\n", p_str );
    return 0;
}

u8 getItemType( char* p_str ) {
    if( !strcmp( p_str, "used" ) ) return 0;
    if( !strcmp( p_str, "?" ) ) return 0;
    if( !strcmp( p_str, "pokeball" ) ) return 1; // Pokeball
    if( !strcmp( p_str, "medicine" ) ) return 2; // Item used as medicine
    if( !strcmp( p_str, "medicine-hold" ) )
        return 34;                                   // Item used as medicine with effect when held
    if( !strcmp( p_str, "battleItem" ) ) return 3;   // Item with effect in battle
    if( !strcmp( p_str, "collectibles" ) ) return 4; // Item with no special effect
    if( !strcmp( p_str, "usables" ) ) return 5;      // Consumable item that can be used overworld
    if( !strcmp( p_str, "evolution" ) ) return 6;    // Item that evolves nome Pkmn
    if( !strcmp( p_str, "hold" ) ) return 32;        // Item that only has an effect when held
    if( !strcmp( p_str, "formeChange" ) ) return 8;  // Item used to change the forme of a pkmn
    if( !strcmp( p_str, "keyItems" ) ) return 9;     // Non-Consumable item with special effects
    if( !strcmp( p_str, "TM/HM" ) ) return 12;       // TM/HM/TR
    if( !strcmp( p_str, "Apricorn" ) ) return 13;    // Apricorn
    //    if( !strcmp( p_str, "Z-multi" ) ) return 14;    // Multi-use Z item (unused)
    //    if( !strcmp( p_str, "Z-held" ) ) return 15;     // Held Z item (unused)

    if( !strcmp( p_str, "berry" ) ) return 20;               // Berry
    if( !strcmp( p_str, "berry-medicine" ) ) return 18;      // Berry + Medicine
    if( !strcmp( p_str, "berry-hold" ) ) return 48;          // Berry + hold
    if( !strcmp( p_str, "berry-medicine-hold" ) ) return 50; // Berry + hold + medicine

    fprintf( stderr, "Found bad item type %s.\n", p_str );
    return 0;
}

u8 getMedicineEffect( char* p_str ) {
    if( !strcmp( p_str, "heal hp" ) )
        return 1; // arg0: amount (0 for percentage); arg1: percentage healed; arg3: 1 if works on
                  // fainted pkmn
    if( !strcmp( p_str, "heal hp and status" ) )
        return 2; // heals all status and heals hp extra arg0: amount (0 for percentage); arg1:
                  // percentage healed; arg2: 1 if works on fainted pkmn
    if( !strcmp( p_str, "heal hp bitter" ) )
        return 3; // heals hp but is bitter, arg0: amount (0 for percentage); arg1: percentage
                  // healed; arg3: 1 if works on fainted pkmn
    if( !strcmp( p_str, "heal status" ) )
        return 4; // arg0 (from 0): all, poison, burn, ice, sleep, paralyze,
    if( !strcmp( p_str, "heal status bitter" ) )
        return 5; // arg0 (from 0): all, poison, burn, ice, sleep, paralyze,
    if( !strcmp( p_str, "heal pp" ) )
        return 6; // arg0: amount (0 for percentage); arg1: percentage healed; arg3: number of moves
    if( !strcmp( p_str, "sacred ash" ) ) return 7;
    if( !strcmp( p_str, "ev up" ) )
        return 8; // arg0: stat (0: hp; atk, def, satk, sdef, spd); arg1: amount, arg2: limit
    if( !strcmp( p_str, "level up" ) ) return 9; // arg0: amount, arg1: limit
    if( !strcmp( p_str, "pp up" ) ) return 10;   // arg0: amount; arg1: limit; arg2: numer of moves
    if( !strcmp( p_str, "ev down berry" ) )
        return 11; // arg0: stat (0: hp; atk, def, satk, sdef, spd)
    if( !strcmp( p_str, "ability capsule" ) ) return 12;
    if( !strcmp( p_str, "iv up" ) )
        return 13; // arg0: stat (0: hp; atk, def, satk, sdef, spd); arg1: amount, arg2: target
                   // value
    if( !strcmp( p_str, "exp up" ) ) return 14;        // arg0: amount
    if( !strcmp( p_str, "nature mint" ) ) return 15;   // arg0: new nature
    if( !strcmp( p_str, "ability patch" ) ) return 16; // switch ability to hidden ability

    fprintf( stderr, "Found bad medicine effect %s.\n", p_str );
    return 0;
}

volatileStatus getVolatileStatus( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return NONE;
    if( !strcmp( p_str, "confusion" ) ) return CONFUSION;
    if( !strcmp( p_str, "obstruct" ) ) return OBSTRUCT;
    if( !strcmp( p_str, "partiallytrapped" ) ) return PARTIALLYTRAPPED;
    if( !strcmp( p_str, "flinch" ) ) return FLINCH;
    if( !strcmp( p_str, "octolock" ) ) return OCTOLOCK;
    if( !strcmp( p_str, "tarshot" ) ) return TARSHOT;
    if( !strcmp( p_str, "noretreat" ) ) return NORETREAT;
    if( !strcmp( p_str, "laserfocus" ) ) return LASERFOCUS;
    if( !strcmp( p_str, "spotlight" ) ) return SPOTLIGHT;
    if( !strcmp( p_str, "banefulbunker" ) ) return BANEFULBUNKER;
    if( !strcmp( p_str, "smackdown" ) ) return SMACKDOWN;
    if( !strcmp( p_str, "powder" ) ) return POWDERED;
    if( !strcmp( p_str, "spikyshield" ) ) return SPIKYSHIELD;
    if( !strcmp( p_str, "kingsshield" ) ) return KINGSSHIELD;
    if( !strcmp( p_str, "electrify" ) ) return ELECTRIFY;
    if( !strcmp( p_str, "ragepowder" ) ) return RAGEPOWDER;
    if( !strcmp( p_str, "telekinesis" ) ) return TELEKINESIS;
    if( !strcmp( p_str, "magnetrise" ) ) return MAGNETRISE;
    if( !strcmp( p_str, "aquaring" ) ) return AQUARING;
    if( !strcmp( p_str, "gastroacid" ) ) return GASTROACID;
    if( !strcmp( p_str, "powertrick" ) ) return POWERTRICK;
    if( !strcmp( p_str, "healblock" ) ) return HEALBLOCK;
    if( !strcmp( p_str, "embargo" ) ) return EMBARGO;
    if( !strcmp( p_str, "miracleeye" ) ) return MIRACLEEYE;
    if( !strcmp( p_str, "substitute" ) ) return SUBSTITUTE;
    if( !strcmp( p_str, "bide" ) ) return BIDE;
    if( !strcmp( p_str, "focusenergy" ) ) return FOCUSENERGY;
    if( !strcmp( p_str, "defensecurl" ) ) return DEFENSECURL;
    if( !strcmp( p_str, "minimize" ) ) return MINIMIZE;
    if( !strcmp( p_str, "leechseed" ) ) return LEECHSEED;
    if( !strcmp( p_str, "disable" ) ) return DISABLE;
    if( !strcmp( p_str, "foresight" ) ) return FORESIGHT;
    if( !strcmp( p_str, "snatch" ) ) return SNATCH_;
    if( !strcmp( p_str, "grudge" ) ) return GRUDGE;
    if( !strcmp( p_str, "imprison" ) ) return IMPRISON;
    if( !strcmp( p_str, "yawn" ) ) return YAWN;
    if( !strcmp( p_str, "magiccoat" ) ) return MAGICCOAT;
    if( !strcmp( p_str, "ingrain" ) ) return INGRAIN;
    if( !strcmp( p_str, "helpinghand" ) ) return HELPINGHAND;
    if( !strcmp( p_str, "taunt" ) ) return TAUNT;
    if( !strcmp( p_str, "charge" ) ) return CHARGE_;
    if( !strcmp( p_str, "followme" ) ) return FOLLOWME;
    if( !strcmp( p_str, "torment" ) ) return TORMENT;
    if( !strcmp( p_str, "stockpile" ) ) return STOCKPILE;
    if( !strcmp( p_str, "attract" ) ) return ATTRACT;
    if( !strcmp( p_str, "endure" ) ) return ENDURE;
    if( !strcmp( p_str, "protect" ) ) return PROTECT_;
    if( !strcmp( p_str, "destinybond" ) ) return DESTINYBOND;
    if( !strcmp( p_str, "curse" ) ) return CURSE;
    if( !strcmp( p_str, "nightmare" ) ) return NIGHTMARE;
    if( !strcmp( p_str, "encore" ) ) return ENCORE;

    fprintf( stderr, "unknown volatile status %s\n", p_str );
    return NONE;
}

moveFlags getMoveFlag( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return (moveFlags) 0;
    if( !strcmp( p_str, "authentic" ) ) return AUTHENTIC;
    if( !strcmp( p_str, "bite" ) ) return BITE;
    if( !strcmp( p_str, "bullet" ) ) return BULLET;
    if( !strcmp( p_str, "charge" ) ) return CHARGE;
    if( !strcmp( p_str, "contact" ) ) return CONTACT;
    if( !strcmp( p_str, "dance" ) ) return DANCE;
    if( !strcmp( p_str, "defrost" ) ) return DEFROST;
    if( !strcmp( p_str, "distance" ) ) return DISTANCE;
    if( !strcmp( p_str, "gravity" ) ) return GRAVITY;
    if( !strcmp( p_str, "heal" ) ) return HEAL;
    if( !strcmp( p_str, "mirror" ) ) return MIRROR;
    if( !strcmp( p_str, "mindblownrecoil" ) ) return MINDBLOWNRECOIL;
    if( !strcmp( p_str, "nonsky" ) ) return NONSKY;
    if( !strcmp( p_str, "powder" ) ) return POWDER;
    if( !strcmp( p_str, "protect" ) ) return PROTECT;
    if( !strcmp( p_str, "pulse" ) ) return PULSE;
    if( !strcmp( p_str, "protect" ) ) return PROTECT;
    if( !strcmp( p_str, "pulse" ) ) return PULSE;
    if( !strcmp( p_str, "punch" ) ) return PUNCH;
    if( !strcmp( p_str, "recharge" ) ) return RECHARGE;
    if( !strcmp( p_str, "reflectable" ) ) return REFLECTABLE;
    if( !strcmp( p_str, "snatch" ) ) return SNATCH;
    if( !strcmp( p_str, "sound" ) ) return SOUND;
    if( !strcmp( p_str, "lockedmove" ) ) return LOCKEDMOVE;
    if( !strcmp( p_str, "rage" ) ) return RAGE;
    if( !strcmp( p_str, "roost" ) ) return ROOST;
    if( !strcmp( p_str, "uproar" ) ) return UPROAR;
    if( !strcmp( p_str, "selfswitch" ) ) return SELFSWITCH;
    if( !strcmp( p_str, "defasoff" ) ) return DEFASOFF;
    if( !strcmp( p_str, "snatch" ) ) return SNATCH;
    if( !strcmp( p_str, "crashdamage" ) ) return CRASHDAMAGE;
    if( !strcmp( p_str, "ohko" ) ) return OHKO;
    if( !strcmp( p_str, "ohkoice" ) ) return moveFlags( OHKO | OHKOICE );
    if( !strcmp( p_str, "selfdestruct" ) ) return SELFDESTRUCT;
    if( !strcmp( p_str, "selfdestructhit" ) ) return SELFDESTRUCTHIT;
    if( !strcmp( p_str, "nosketch" ) ) return NOSKETCH;
    if( !strcmp( p_str, "noppboost" ) ) return NOPPBOOST;
    if( !strcmp( p_str, "sleepusable" ) ) return SLEEPUSABLE;
    if( !strcmp( p_str, "nofaint" ) ) return NOFAINT;
    if( !strcmp( p_str, "breaksprotect" ) ) return BREAKSPROTECT;
    if( !strcmp( p_str, "ignoreimmunity" ) ) return IGNOREIMMUNITY;
    if( !strcmp( p_str, "ignoreimmunitygnd" ) ) return IGNOREIMMUNITYGROUND;
    if( !strcmp( p_str, "ignoreability" ) ) return IGNOREABILITY;
    if( !strcmp( p_str, "ignoredefs" ) ) return IGNOREDEFS;
    if( !strcmp( p_str, "ignoreevasion" ) ) return IGNOREEVASION;
    if( !strcmp( p_str, "defrosttarget" ) ) return DEFROSTTARGET;
    if( !strcmp( p_str, "forceswitch" ) ) return FORCESWITCH;
    if( !strcmp( p_str, "willcrit" ) ) return WILLCRIT;
    if( !strcmp( p_str, "targetoffensives" ) ) return TARGETOFFENSIVES;
    if( !strcmp( p_str, "futuremove" ) ) return FUTUREMOVE;
    if( !strcmp( p_str, "batonpass" ) ) return BATONPASS;

    fprintf( stderr, "Found bad move flag %s.\n", p_str );
    return (moveFlags) 0;
}

moveFlags parseMoveFlags( char* p_buffer ) {
    char*     p   = strtok( p_buffer, "|" );
    moveFlags res = getMoveFlag( p );

    while( ( p = strtok( NULL, "|" ) ) ) { res = moveFlags( res | getMoveFlag( p ) ); }

    return res;
}

targets getTarget( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return NO_TARGET;
    if( !strcmp( p_str, "normal" ) ) return NORMAL;
    if( !strcmp( p_str, "adjacentAlly" ) ) return ADJACENT_ALLY;
    if( !strcmp( p_str, "adjacentFoe" ) ) return ADJACENT_FOE;
    if( !strcmp( p_str, "any" ) ) return ANY;
    if( !strcmp( p_str, "allies" ) ) return ALLIES;
    if( !strcmp( p_str, "allyTeam" ) ) return ALLY_TEAM;
    if( !strcmp( p_str, "scripted" ) ) return SCRIPTED; // Metal burst, counter, etc
    if( !strcmp( p_str, "adjacentAllyOrSelf" ) ) return ADJACENT_ALLY_OR_SELF;
    if( !strcmp( p_str, "self" ) ) return SELF;
    if( !strcmp( p_str, "randomNormal" ) ) return RANDOM;
    if( !strcmp( p_str, "allAdjacent" ) ) return ALL_ADJACENT;
    if( !strcmp( p_str, "allAdjacentFoes" ) ) return ALL_ADJACENT_FOES;
    if( !strcmp( p_str, "allySide" ) ) return ALLY_SIDE;
    if( !strcmp( p_str, "foeSide" ) ) return FOE_SIDE;
    if( !strcmp( p_str, "all" ) ) return ALL;

    fprintf( stderr, "unknown target %s\n", p_str );
    return (targets) 0;
}

u8 getFixedDamage( char* p_str ) {
    if( !strcmp( p_str, "level" ) ) return 255;
    if( !strcmp( p_str, "none" ) ) return 0;
    u8 res = 0;
    if( !sscanf( p_str, "%hhu", &res ) ) {
        fprintf( stderr, "Unknown fixed damage type %s.\n", p_str );
    }
    return res;
}

moveHitTypes getMoveCategory( char* p_str, moveHitTypes p_default ) {
    if( !strcmp( p_str, "none" ) ) return p_default;
    if( !strcmp( p_str, "Physical" ) ) return PHYSICAL;
    if( !strcmp( p_str, "Special" ) ) return SPECIAL;
    if( !strcmp( p_str, "Status" ) ) return STATUS;

    fprintf( stderr, "unknown move category %s\n", p_str );
    return p_default;
}

u8 getWeather( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return 0;

    if( !strcmp( p_str, "RainDance" ) ) return 1;
    if( !strcmp( p_str, "hail" ) ) return 2;
    if( !strcmp( p_str, "fog" ) ) return 3;
    if( !strcmp( p_str, "Sandstorm" ) ) return 4;
    if( !strcmp( p_str, "sunnyday" ) ) return 5;
    if( !strcmp( p_str, "heavyrain" ) ) return 6;
    if( !strcmp( p_str, "heavysun" ) ) return 7;
    if( !strcmp( p_str, "strongwinds" ) ) return 8;

    fprintf( stderr, "unknown weather %s\n", p_str );
    return 0;
}

u8 getPseudoWeather( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return 0;
    if( !strcmp( p_str, "iondeluge" ) ) return 1 << 0;
    if( !strcmp( p_str, "magicroom" ) ) return 1 << 1;
    if( !strcmp( p_str, "wonderroom" ) ) return 1 << 2;
    if( !strcmp( p_str, "trickroom" ) ) return 1 << 3;
    if( !strcmp( p_str, "gravity" ) ) return 1 << 4;
    if( !strcmp( p_str, "watersport" ) ) return 1 << 5;
    if( !strcmp( p_str, "mudsport" ) ) return 1 << 6;
    if( !strcmp( p_str, "fairylock" ) ) return 1 << 7;

    fprintf( stderr, "unknown pseudo weather %s\n", p_str );
    return 0;
}

u8 getTerrain( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return 0;
    if( !strcmp( p_str, "psychicterrain" ) ) return 1;
    if( !strcmp( p_str, "electricterrain" ) ) return 2;
    if( !strcmp( p_str, "mistyterrain" ) ) return 3;
    if( !strcmp( p_str, "grassyterrain" ) ) return 4;

    fprintf( stderr, "unknown terrain %s\n", p_str );
    return 0;
}

u16 getSideCondition( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return 0;
    if( !strcmp( p_str, "craftyshield" ) ) return ( 1 << 0 );
    if( !strcmp( p_str, "stickyweb" ) ) return ( 1 << 1 );
    if( !strcmp( p_str, "matblock" ) ) return ( 1 << 2 );
    if( !strcmp( p_str, "quickguard" ) ) return ( 1 << 3 );
    if( !strcmp( p_str, "wideguard" ) ) return ( 1 << 4 );
    if( !strcmp( p_str, "stealthrock" ) ) return ( 1 << 5 );
    if( !strcmp( p_str, "toxicspikes" ) ) return ( 1 << 6 );
    if( !strcmp( p_str, "luckychant" ) ) return ( 1 << 7 );
    if( !strcmp( p_str, "tailwind" ) ) return ( 1 << 8 );
    if( !strcmp( p_str, "safeguard" ) ) return ( 1 << 9 );
    if( !strcmp( p_str, "spikes" ) ) return ( 1 << 10 );
    if( !strcmp( p_str, "reflect" ) ) return ( 1 << 11 );
    if( !strcmp( p_str, "lightscreen" ) ) return ( 1 << 12 );
    if( !strcmp( p_str, "mist" ) ) return ( 1 << 13 );
    if( !strcmp( p_str, "auroraveil" ) ) return ( 1 << 14 );

    fprintf( stderr, "unknown side condition %s\n", p_str );
    return 0;
}

u8 getSlotCondition( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return 0;
    if( !strcmp( p_str, "Wish" ) ) return 1;
    if( !strcmp( p_str, "healingwish" ) ) return 2;
    if( !strcmp( p_str, "lunardance" ) ) return 3;

    fprintf( stderr, "unknown slot condition %s\n", p_str );
    return 0;
}

u8 getStatus( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return 0;
    if( !strcmp( p_str, "brn" ) ) return 1;
    if( !strcmp( p_str, "par" ) ) return 2;
    if( !strcmp( p_str, "frz" ) ) return 3;
    if( !strcmp( p_str, "slp" ) ) return 4;
    if( !strcmp( p_str, "psn" ) ) return 5;
    if( !strcmp( p_str, "tox" ) ) return 6;

    fprintf( stderr, "unknown status %s\n", p_str );
    return 0;
}

u8 getFrameType( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return 0;
    if( !strcmp( p_str, "route" ) ) return 0;

    // water
    if( !strcmp( p_str, "water" ) ) return 2;
    if( !strcmp( p_str, "beach" ) ) return 3;
    if( !strcmp( p_str, "coastroute" ) ) return 11;

    // special
    if( !strcmp( p_str, "cave" ) ) return 4;
    if( !strcmp( p_str, "forest" ) ) return 5;
    if( !strcmp( p_str, "special" ) ) return 10;
    if( !strcmp( p_str, "coast" ) ) return 7;
    if( !strcmp( p_str, "inside" ) ) return 1;

    // city
    if( !strcmp( p_str, "forestcity" ) ) return 12;
    if( !strcmp( p_str, "city" ) ) return 6;
    if( !strcmp( p_str, "town" ) ) return 8;
    if( !strcmp( p_str, "field" ) ) return 9;

    fprintf( stderr, "unknown frame type \"%s\"\n", p_str );
    return 0;
}

u8 getNumberOrNone( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return 0;

    u8 num;
    sscanf( p_str, "%hhu", &num );
    return num;
}

u8 parseFraction( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return 0;

    u8 num, denom;
    sscanf( p_str, "%hhu|%hhu", &num, &denom );
    return u8( num * 240.0 / denom );
}

u8 parseMultiHit( char* p_str ) {
    if( !strcmp( p_str, "none" ) ) return 0;

    u8 mn = 0, mx = 0;
    if( sscanf( p_str, "%hhu|%hhu", &mn, &mx ) ) {
        if( mx ) {
            return ( mn << 4 ) | mx;
        } else {
            return ( mn << 4 ) | mn;
        }
    }

    if( sscanf( p_str, "%hhu", &mn ) ) return ( mn << 4 ) | mn;

    return 0;
}

void parseBoost( char* p_str, boosts& p_out ) {
    char buf[ 50 ] = { 0 };
    s8   val;
    sscanf( p_str, "%[^:]:%hhd", buf, &val );

    if( !strcmp( buf, "atk" ) ) return p_out.setBoost( ATK, val );
    if( !strcmp( buf, "def" ) ) return p_out.setBoost( DEF, val );
    if( !strcmp( buf, "spa" ) ) return p_out.setBoost( SATK, val );
    if( !strcmp( buf, "spd" ) ) return p_out.setBoost( SDEF, val );
    if( !strcmp( buf, "spe" ) ) return p_out.setBoost( SPEED, val );
    if( !strcmp( buf, "accuracy" ) ) return p_out.setBoost( ACCURACY, val );
    if( !strcmp( buf, "evasion" ) ) return p_out.setBoost( EVASION, val );

    fprintf( stderr, "unknown boost type %s\n", buf );
}

boosts parseBoosts( char* p_buffer ) {
    boosts res = boosts( );
    if( !strcmp( p_buffer, "none" ) ) return res;

    char* p = strtok( p_buffer, "|" );
    if( !p ) {
        parseBoost( p_buffer, res );
    } else {
        do { parseBoost( p, res ); } while( ( p = strtok( NULL, "|" ) ) );
    }
    return res;
}

FILE* getFilePtr( string p_prefix, u16 p_index, u8 p_digits, string p_ext, u8 p_formeId ) {
    char buffer[ 50 ]   = { 0 };
    char buffer2[ 100 ] = { 0 };
    if( p_digits ) {
        snprintf( buffer2, 20, "%%0%hhuhhu/", p_digits );
        snprintf( buffer, 40, buffer2, p_index / MAX_ITEMS_PER_DIR );
    }

    fs::create_directories( p_prefix + buffer );
    if( !p_formeId )
        snprintf( buffer2, 80, "%s/%s%hu%s", p_prefix.c_str( ), buffer, p_index, p_ext.c_str( ) );
    else
        snprintf( buffer2, 80, "%s/%s%hu_%hhu%s", p_prefix.c_str( ), buffer, p_index, p_formeId,
                  p_ext.c_str( ) );

    return fopen( buffer2, "wb" );
}

void printNormalized( char* p_string, FILE* p_f ) {
    while( p_string && *p_string ) {
        if( *p_string == '(' ) {
            ++p_string;
            continue;
        }
        if( *p_string == ')' ) {
            ++p_string;
            continue;
        }
        if( *p_string == '\xe9' || *p_string == '\xc9' ) {
            fputc( 'E', p_f );
            ++p_string;
            continue;
        }
        if( *p_string == '{' ) {
            fputc( '_', p_f );
            fputc( 'M', p_f );
            ++p_string;
            continue;
        }
        if( *p_string == '}' ) {
            fputc( '_', p_f );
            fputc( 'F', p_f );
            ++p_string;
            continue;
        }
        if( *p_string == '.' ) {
            ++p_string;
            continue;
        }
        if( *p_string == '&' ) {
            ++p_string;
            if( *p_string == ' ' ) { ++p_string; }
            continue;
        }
        if( *p_string == '\'' || *p_string == ' ' || *p_string == ':' || *p_string == '-'
            || *p_string == '?' ) {
            fputc( '_', p_f );
            ++p_string;
            continue;
        }
        fputc( toupper( *( p_string++ ) ), p_f );
    }
}

void readNames( char* p_path, vector<names>& p_out, int p_maxLen, u8 p_offset ) {
    FILE* f             = fopen( p_path, "r" );
    char  buffer[ 500 ] = { 0 };
    char* t1;
    while( fgets( buffer, sizeof( buffer ), f ) ) {
        t1 = strtok( buffer, "," );
        names n;
        for( int i = 0; i < NUM_LANGUAGES; ++i ) {
            n.m_name[ i ] = new char[ 2 * p_maxLen ];
            std::memset( n.m_name[ i ], 0, 2 * p_maxLen );
        }
        int cnt = 0;
        for( u8 i = 0; i < p_offset; ++i, ( t1 = strtok( NULL, "," ) ) )
            ;
        while( cnt < NUM_LANGUAGES && ( t1 = strtok( NULL, "," ) ) ) {
            strncpy( n.m_name[ cnt++ ], fixEncoding( t1 ), p_maxLen );
        }
        p_out.push_back( n );
    }
    // fprintf( stderr, "read %lu objects from %s\n", p_out.size( ), p_path );
    fclose( f );
}

void readNames( char* p_path, map<u16, names>& p_out ) {
    FILE* f             = fopen( p_path, "r" );
    char  buffer[ 500 ] = { 0 };
    char* t1;
    while( fgets( buffer, sizeof( buffer ), f ) ) {
        t1 = strtok( buffer, "," );
        u16 id;
        sscanf( t1, "%hu", &id );
        names n;
        for( int i = 0; i < NUM_LANGUAGES; ++i ) {
            n.m_name[ i ] = new char[ 40 ];
            std::memset( n.m_name[ i ], 0, 40 );
        }
        int cnt = 0;
        while( cnt < NUM_LANGUAGES && ( t1 = strtok( NULL, "," ) ) ) {
            strncpy( n.m_name[ cnt++ ], fixEncoding( t1 ), 29 );
        }
        p_out[ id ] = n;
    }
    // fprintf( stderr, "read %lu objects from %s\n", p_out.size( ), p_path );
    fclose( f );
}

void readNames( char* p_path, vector<pair<string, names>>& p_out ) {
    FILE* f = fopen( p_path, "r" );
    assert( f );
    char  buffer[ 500 ] = { 0 };
    char* t1;
    while( fgets( buffer, sizeof( buffer ), f ) ) {
        t1        = strtok( buffer, ";" );
        string id = string( t1 );
        names  n;
        for( int i = 0; i < NUM_LANGUAGES; ++i ) {
            n.m_name[ i ] = new char[ 40 ];
            std::memset( n.m_name[ i ], 0, 40 );
        }
        int cnt = 0;
        while( cnt < NUM_LANGUAGES && ( t1 = strtok( NULL, ";" ) ) ) {
            strncpy( n.m_name[ cnt++ ], fixEncoding( t1 ), 29 );
        }
        p_out.push_back( { id, n } );
    }
    // fprintf( stderr, "read %lu objects from %s\n", p_out.size( ), p_path );
    fclose( f );
}

void readDescrs( char* p_path, vector<descrs>& p_out, int p_maxLen ) {
    FILE* f             = fopen( p_path, "r" );
    char  buffer[ 500 ] = { 0 };
    char* t1;
    while( fgets( buffer, sizeof( buffer ), f ) ) {
        t1 = strtok( buffer, ";" );
        descrs n;
        for( int i = 0; i < NUM_LANGUAGES; ++i ) { n.m_descr[ i ] = new char[ 250 ]; }
        int cnt = 0;
        while( cnt < NUM_LANGUAGES && ( t1 = strtok( NULL, ";" ) ) ) {
            strncpy( n.m_descr[ cnt++ ], fixEncoding( t1 ), std::min( 249, p_maxLen ) );
        }
        p_out.push_back( n );
    }
    // fprintf( stderr, "read %lu objects from %s\n", p_out.size( ), p_path );
    fclose( f );
}
