//============================================================================================
// Editor.cpp : Der Flugzeugeditor
//============================================================================================
#include "stdafx.h"
#include "Editor.h"
#include "glEditor.h"
#include "atnet.h"
#include <string>
#include <fstream>

#if __cplusplus < 201703L // If the version of C++ is less than 17
#include <experimental/filesystem>
    // It was still in the experimental:: namespace
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SB_CColorFX ColorFX;

#define DSBVOLUME_MIN               -10000
#define DSBVOLUME_MAX               0

//Zum debuggen:
static const char FileId[] = "Edit";

CPlaneParts gPlaneParts;

CPlaneBuild gPlaneBuilds[37]= 
{            // Id    Shortname Cost  Weight  Power, Noise, Wartung, Passag., Verbrauch, BmIdx zPos
   CPlaneBuild (1000, "B1", 10000000, 40000,  0,     0,     -15,     140,    0,         0,    10000),
   CPlaneBuild (1001, "B2", 40000000, 90000,  0,     -5,    0,       600,    0,         1,    10000),
   CPlaneBuild (1002, "B3", 40000000, 30000,  0,     -40,   -15,     130,    0,         2,    10000),
   CPlaneBuild (1003, "B4", 25000000, 70000,  0,     -20,   -15,     320,    0,         3,    10000),
   CPlaneBuild (1004, "B5", 20000000, 60000,  0,     -10,   -15,     280,    0,         4,    10000),
                                                                            
   CPlaneBuild (2000, "C1",   100000,  2000,  0,     5,     10,      0,      0,         5,    10010),
   CPlaneBuild (2001, "C2",  1100000,  2500,  0,     2,     5,       0,      0,         6,    10010),
   CPlaneBuild (2002, "C3",  1200000,  4000,  0,     0,     0,       0,      0,         7,    10010),
   CPlaneBuild (2003, "C4",   200000,   500,  0,     5,     20,      0,      0,         8,    10010),
   CPlaneBuild (2004, "C5",  4000000,  5000,  0,     -15,   0,       0,      0,         9,    10010),
                                                                            
   CPlaneBuild (3000, "H1",   400000,  7000,  0,     10,    0,       0,      0,         10,    9000),
   CPlaneBuild (3001, "H2",   800000,  5000,  0,     0,     0,       0,      0,         11,    9000),
   CPlaneBuild (3002, "H3",  1200000,  3000,  0,     0,     0,       0,      0,         12,    9000),
   CPlaneBuild (3003, "H4",  4500000,  4000,  0,     0,     0,       0,      0,         13,    9000),
   CPlaneBuild (3003, "H5",  2000000,  4000,  -4000, 20,    -20,     0,      0,         14,    9000),
   CPlaneBuild (3003, "H6",  6000000,  9000,  0,     -20,   10,      0,      0,         15,    9000),
   CPlaneBuild (3003, "H7",  5000000,  4000,  0,     -10,   0,       0,      0,         16,    9000),
                                                                            
   CPlaneBuild (4000, "R1",  7000000, 21000,  0,     -10,   0,       0,      -50,       17,   12000),
   CPlaneBuild (4001, "R2",  3000000, 11000,  0,     0,     0,       0,      0,         18,   12000),
   CPlaneBuild (4002, "R3",  1700000,  7000,  0,     2,     0,       0,      0,         19,   12000),
   CPlaneBuild (4003, "R4",  1200000,  5000,  0,     5,     0,       0,      0,         20,   12000),
   CPlaneBuild (4004, "R5",   500000,  2000,  0,     0,     0,       0,      0,         21,   12000),
   CPlaneBuild (4004, "R6",  2000000, 30000,  0,     0,     0,       0,      0,         22,   12000),
                                                                            
   CPlaneBuild (5000, "M1",  2300000,  1100,  6000,  3,     0,       0,      2000,      23,    7500),
   CPlaneBuild (5001, "M2",   100000,   500,  4000,  20,    10,      0,      500,       24,    7500),
   CPlaneBuild (5002, "M3",   250000,   600,  5000,  20,    10,      0,      600,       25,    7500),
   CPlaneBuild (5003, "M4",  4300000,  1600,  7000,  3,     0,       0,      3000,      26,    7500),
   CPlaneBuild (5003, "M5",  2400000,   900,  5000,  0,     0,       0,      4000,      27,    7500),
   CPlaneBuild (5003, "M6",  5000000,  2000,  10000, 14,    5,       0,      12000,     28,    7500),
   CPlaneBuild (5003, "M7",  5500000,  2500,  18000, 20,    10,      0,      8000,      29,    7500),
   CPlaneBuild (5003, "M8",  3000000,  3400,  14000, 15,    12,      0,      10000,     30,    7500),
                                                                            
   CPlaneBuild (6000, "L1",  7000000, 21000,  0,     -10,   0,       0,      -50,       31,    8000),
   CPlaneBuild (6001, "L2",  3000000, 11000,  0,     0,     0,       0,      0,         32,    8000),
   CPlaneBuild (6002, "L3",  1700000,  7000,  0,     2,     0,       0,      0,         33,    8000),
   CPlaneBuild (6003, "L4",  1200000,  5000,  0,     5,     10,      0,      0,         34,    8000),
   CPlaneBuild (6004, "L5",   500000,  2000,  0,     0,     10,      0,      0,         35,    8000),
   CPlaneBuild (6004, "L6",  2000000, 30000,  0,     0,     0,       0,      0,         36,    8000),
};

#undef _C2

static long _B1=GetPlaneBuildIndex("B1");
static long _B2=GetPlaneBuildIndex("B2");
static long _B3=GetPlaneBuildIndex("B3");
static long _B4=GetPlaneBuildIndex("B4");
static long _B5=GetPlaneBuildIndex("B5");
static long _C1=GetPlaneBuildIndex("C1");
static long _C2=GetPlaneBuildIndex("C2");
static long _C3=GetPlaneBuildIndex("C3");
static long _C4=GetPlaneBuildIndex("C4");
static long _C5=GetPlaneBuildIndex("C5");
static long _H1=GetPlaneBuildIndex("H1");
static long _H2=GetPlaneBuildIndex("H2");
static long _H3=GetPlaneBuildIndex("H3");
static long _H4=GetPlaneBuildIndex("H4");
static long _H5=GetPlaneBuildIndex("H5");
static long _H6=GetPlaneBuildIndex("H6");
static long _H7=GetPlaneBuildIndex("H7");
static long _L1=GetPlaneBuildIndex("L1");
static long _L2=GetPlaneBuildIndex("L2");
static long _L3=GetPlaneBuildIndex("L3");
static long _L4=GetPlaneBuildIndex("L4");
static long _L5=GetPlaneBuildIndex("L5");
static long _L6=GetPlaneBuildIndex("L6");
static long _M1=GetPlaneBuildIndex("M1");
static long _M2=GetPlaneBuildIndex("M2");
static long _M3=GetPlaneBuildIndex("M3");
static long _M4=GetPlaneBuildIndex("M4");
static long _M5=GetPlaneBuildIndex("M5");
static long _M6=GetPlaneBuildIndex("M6");
static long _M7=GetPlaneBuildIndex("M7");
static long _M8=GetPlaneBuildIndex("M8");
static long _R1=GetPlaneBuildIndex("R1");
static long _R2=GetPlaneBuildIndex("R2");
static long _R3=GetPlaneBuildIndex("R3");
static long _R4=GetPlaneBuildIndex("R4");
static long _R5=GetPlaneBuildIndex("R5");
static long _R6=GetPlaneBuildIndex("R6");

//Für das Anhängen der Tragflächen:
static XY _rbody2a( 66,79);
static XY _rbody2b(118,91);
static XY _rbody2c(169,105);
static XY _rbody2d(139,138);
static XY _lbody2a( 66+29,79-30);
static XY _lbody2b(118+29,91-30);
static XY _lbody2c(169+29,105-30);
static XY _lbody2d(139+29,138-30);

static XY _rbody3(82,77);
static XY _lbody3(118,44);

static XY _rbody4(114,113);
static XY _lbody4(160,42);

static XY _rbody5(107,94);
static XY _lbody5(120+36,33+15);

static XY _rwing1(182,14);
static XY _lwing1( 80,166);

static XY _rwing2(206,11);
static XY _lwing2( 68,141);

static XY _rwing3(202,20);
static XY _lwing3(110,129);

static XY _rwing4(202,20);
static XY _lwing4( 57,135);

static XY _rwing5(121,10);
static XY _lwing5( 62,86);

static XY _rwing6(200,21);
static XY _lwing6(111,100);

//Für das Anhängen der Triebwerke:
static XY _motor1(45, 6);
static XY _motor2(33, 4);
static XY _motor3(44,13);
static XY _motor4(42, 4);
static XY _motor5(49,12);
static XY _motor6(79,30);
static XY _motor7(87,31);
static XY _motor8(91, 4);

static XY _m_rwing1a(224,41);
static XY _m_rwing1b(150,58);
static XY _m_rwing1c( 80,81);
static XY _m_lwing1a(138,163);
static XY _m_lwing1b(146,112);
static XY _m_lwing1c(154, 60);

static XY _m_rwing2a(221, 29);
static XY _m_rwing2b(156, 45);
static XY _m_rwing2c( 57, 61);
static XY _m_lwing2a(106,129);
static XY _m_lwing2b(108, 90);
static XY _m_lwing2c( 98, 23);

static XY _m_rwing3a(235, 61);
static XY _m_rwing3b(181, 76);
static XY _m_rwing3c( 81, 97);
static XY _m_lwing3a(195,118);
static XY _m_lwing3b(190, 88);
static XY _m_lwing3c(185, 34);

static XY _m_rwing4a(166, 43);
static XY _m_rwing4b(103, 83);
static XY _m_lwing4a(111, 92);
static XY _m_lwing4b(151, 45);

static XY _m_rwing5(100, 54);
static XY _m_lwing5(128, 42);

static XY _m_rwing6a(217, 64);
static XY _m_rwing6b(117,105);
static XY _m_lwing6a(178, 87);
static XY _m_lwing6b(177, 43);

//2d: Zum anhängen des Cockpits:
static XY _2d_cbody1(  0,0);
static XY _2d_cbody2(  0,36);
static XY _2d_cbody3(  0,0);
static XY _2d_cbody4(  0,10);
static XY _2d_cbody5(  0,10);

static XY _2d_cpit1( 48,1);
static XY _2d_cpit2( 62,0);
static XY _2d_cpit3( 76,0);
static XY _2d_cpit4( 37,0);
static XY _2d_cpit5( 99,0);

//2d: Zum anhängen des Hecks:
static XY _2d_hbody1( 89,0);
static XY _2d_hbody2(214,11);
static XY _2d_hbody3(134,0);
static XY _2d_hbody4(178,8);
static XY _2d_hbody5(178,9);

static XY _2d_heck1( 0,71);
static XY _2d_heck2( 0,47);
static XY _2d_heck3( 0,43);
static XY _2d_heck4( 0,46);
static XY _2d_heck5( 0,47);
static XY _2d_heck6( 0,29);
static XY _2d_heck7( 0,59);

//2d: Die Tragflächen anhängen:
static XY _2d_tbody1( 44,39);
static XY _2d_tbody2a(150,48);
static XY _2d_tbody2b(110,48);
static XY _2d_tbody2c(75,48);
static XY _2d_tbody2d(96,81);
static XY _2d_tbody3(66,39);
static XY _2d_tbody4(91,50);
static XY _2d_tbody5(91,50);

static XY _2d_rght1(59,7);
static XY _2d_rght2(52,5);
static XY _2d_rght3(93,6);
static XY _2d_rght4(40,5);
static XY _2d_rght5(43,4);
static XY _2d_rght6(93,6);

static XY _2d_left1(59,78);
static XY _2d_left2(52,64);
static XY _2d_left3(93,58);
static XY _2d_left4(40,69);
static XY _2d_left5(90,44);
static XY _2d_left6(93,31);

//Für das Anhängen der Triebwerke:
static XY _2d_motor1(28,0);
static XY _2d_motor2(42,6);
static XY _2d_motor3(34,14);
static XY _2d_motor4(32,0);
static XY _2d_motor5(27,0);
static XY _2d_motor6(40,0);
static XY _2d_motor7(42,0);
static XY _2d_motor8(56,0);

static XY _2d_m_rwing1a(27,19);
static XY _2d_m_rwing1b(47,27);
static XY _2d_m_rwing1c(66,39);
static XY _2d_m_lwing1a(24,61);
static XY _2d_m_lwing1b(39,49);
static XY _2d_m_lwing1c(53,38);

static XY _2d_m_rwing2a(24,11);
static XY _2d_m_rwing2b(45,19);
static XY _2d_m_rwing2c(87,32);
static XY _2d_m_lwing2a(36,42);
static XY _2d_m_lwing2b(57,29);
static XY _2d_m_lwing2c(93,8);

static XY _2d_m_rwing3a(45,21);
static XY _2d_m_rwing3b(72,34);
static XY _2d_m_rwing3c(105,49);
static XY _2d_m_lwing3a(47,39);
static XY _2d_m_lwing3b(79,22);
static XY _2d_m_lwing3c(105,10);

static XY _2d_m_rwing4a(18,20);
static XY _2d_m_rwing4b(29,43);
static XY _2d_m_lwing4a(18,49);
static XY _2d_m_lwing4b(29,21);

static XY _2d_m_rwing5(19,18);
static XY _2d_m_lwing5(18,27);

static XY _2d_m_rwing6a(43,21);
static XY _2d_m_rwing6b(82,48);
static XY _2d_m_lwing6a(68,17);
static XY _2d_m_lwing6b(97,8);

CPlanePartRelation gPlanePartRelations[307]=
{
   //Bug             // Id,  From, To, Offset2d,     Offset3d,     Note1,            Note1,           Note1,    zAdd, Slot, RulesOutSlots
   CPlanePartRelation ( 100,  -1, _B1, XY(-44, -75), XY(320, 220-30), NOTE_BEGLEITER4,  NOTE_STD,        NOTE_STD, 0,    0,     "B*", "B*" ),
   CPlanePartRelation ( 101,  -1, _B2, XY(-107,-120),XY(320, 220-30), NOTE_BEGLEITER8,  NOTE_BEGLEITER4, NOTE_STD, 0,    0,     "B*", "B*" ),
   CPlanePartRelation ( 102,  -1, _B3, XY(-67, -76), XY(320, 220-30), NOTE_BEGLEITER6,  NOTE_STD,        NOTE_STD, 0,    0,     "B*", "B*" ),
   CPlanePartRelation ( 103,  -1, _B4, XY(-80, -99), XY(320, 220-30), NOTE_BEGLEITER6,  NOTE_BEGLEITER4, NOTE_STD, 0,    0,     "B*", "B*" ),
   CPlanePartRelation ( 104,  -1, _B5, XY(-80, -94), XY(320, 220-30), NOTE_BEGLEITER8,  NOTE_STD,        NOTE_STD, 0,    0,     "B*", "B*" ),

   //Bug->Cockpit    // Id,  From, To, Offset2d,             Offset3d,     Note1,       Note1,       Note1,         zAdd, Noise, Slot, RulesOutSlots
   CPlanePartRelation ( 200, _B1, _C1, _2d_cbody1-_2d_cpit1, XY(103,  30), NOTE_PILOT3, NOTE_STD,    NOTE_SPEED400, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 201, _B1, _C2, _2d_cbody1-_2d_cpit2, XY(104,  30), NOTE_PILOT3, NOTE_STD,    NOTE_SPEED500, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 202, _B1, _C3, _2d_cbody1-_2d_cpit3, XY(104,  30), NOTE_PILOT4, NOTE_STD,    NOTE_SPEED800, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 203, _B1, _C4, _2d_cbody1-_2d_cpit4, XY(104,  31), NOTE_PILOT2, NOTE_STD,    NOTE_SPEED300, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 204, _B1, _C5, _2d_cbody1-_2d_cpit5, XY(104,  31), NOTE_PILOT4, NOTE_PILOT3, NOTE_STD,      0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 210, _B2, _C1, _2d_cbody2-_2d_cpit1, XY(249, 105), NOTE_PILOT3, NOTE_STD,    NOTE_SPEED400, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 211, _B2, _C2, _2d_cbody2-_2d_cpit2, XY(250, 105), NOTE_PILOT3, NOTE_STD,    NOTE_SPEED500, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 212, _B2, _C3, _2d_cbody2-_2d_cpit3, XY(250, 105), NOTE_PILOT4, NOTE_STD,    NOTE_SPEED800, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 213, _B2, _C4, _2d_cbody2-_2d_cpit4, XY(250, 106), NOTE_PILOT2, NOTE_STD,    NOTE_SPEED300, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 214, _B2, _C5, _2d_cbody2-_2d_cpit5, XY(250, 106), NOTE_PILOT4, NOTE_PILOT3, NOTE_STD,      0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 220, _B3, _C1, _2d_cbody3-_2d_cpit1, XY(157,  46), NOTE_PILOT3, NOTE_STD,    NOTE_SPEED400, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 221, _B3, _C2, _2d_cbody3-_2d_cpit2, XY(157,  46), NOTE_PILOT3, NOTE_STD,    NOTE_SPEED500, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 222, _B3, _C3, _2d_cbody3-_2d_cpit3, XY(158,  46), NOTE_PILOT4, NOTE_STD,    NOTE_SPEED800, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 223, _B3, _C4, _2d_cbody3-_2d_cpit4, XY(158,  47), NOTE_PILOT2, NOTE_STD,    NOTE_SPEED300, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 224, _B3, _C5, _2d_cbody3-_2d_cpit5, XY(158,  47), NOTE_PILOT4, NOTE_PILOT3, NOTE_STD,      0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 230, _B4, _C1, _2d_cbody4-_2d_cpit1, XY(208,  60), NOTE_PILOT3, NOTE_STD,    NOTE_SPEED400, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 231, _B4, _C2, _2d_cbody4-_2d_cpit2, XY(209,  60), NOTE_PILOT3, NOTE_STD,    NOTE_SPEED500, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 232, _B4, _C3, _2d_cbody4-_2d_cpit3, XY(209,  60), NOTE_PILOT4, NOTE_STD,    NOTE_SPEED800, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 233, _B4, _C4, _2d_cbody4-_2d_cpit4, XY(209,  61), NOTE_PILOT2, NOTE_STD,    NOTE_SPEED300, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 234, _B4, _C5, _2d_cbody4-_2d_cpit5, XY(209,  61), NOTE_PILOT4, NOTE_PILOT3, NOTE_STD,      0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 240, _B5, _C1, _2d_cbody5-_2d_cpit1, XY(207,  60), NOTE_PILOT3, NOTE_STD,    NOTE_SPEED400, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 241, _B5, _C2, _2d_cbody5-_2d_cpit2, XY(208,  60), NOTE_PILOT3, NOTE_STD,    NOTE_SPEED500, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 242, _B5, _C3, _2d_cbody5-_2d_cpit3, XY(208,  60), NOTE_PILOT4, NOTE_STD,    NOTE_SPEED800, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 243, _B5, _C4, _2d_cbody5-_2d_cpit4, XY(208,  61), NOTE_PILOT2, NOTE_STD,    NOTE_SPEED300, 0,    0,     "C0", "C0" ),
   CPlanePartRelation ( 244, _B5, _C5, _2d_cbody5-_2d_cpit5, XY(208,  61), NOTE_PILOT4, NOTE_PILOT3, NOTE_STD,      0,    0,     "C0", "C0" ),
                                                 
   //Bug->Heck       // Id,  From, To, Offset2d,     Offset3d,        Note1,    Note1,    Note1,         zAdd, Noise, Slot, RulesOutSlots
   CPlanePartRelation ( 300, _B1, _H1, _2d_hbody1-_2d_heck1, XY(-130, -121),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 301, _B1, _H2, _2d_hbody1-_2d_heck2, XY(-126,  -91),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 302, _B1, _H3, _2d_hbody1-_2d_heck3, XY(-110,  -90),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 303, _B1, _H4, _2d_hbody1-_2d_heck4, XY( -78,  -85),  NOTE_STD, NOTE_STD, NOTE_SPEED500, 0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 304, _B1, _H5, _2d_hbody1-_2d_heck5, XY(-111,  -87),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 305, _B1, _H6, _2d_hbody1-_2d_heck6, XY( -98,  -59),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 306, _B1, _H7, _2d_hbody1-_2d_heck7, XY(-136, -113),  NOTE_STD, NOTE_STD, NOTE_SPEED800, 0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 310, _B2, _H1, _2d_hbody2-_2d_heck1, XY(-130, -121),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 311, _B2, _H2, _2d_hbody2-_2d_heck2, XY(-126,  -91),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 312, _B2, _H3, _2d_hbody2-_2d_heck3, XY(-110,  -90),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 313, _B2, _H4, _2d_hbody2-_2d_heck4, XY( -78,  -85),  NOTE_STD, NOTE_KAPUTTXL, NOTE_SPEED500, 0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 314, _B2, _H5, _2d_hbody2-_2d_heck5, XY(-111,  -87),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 315, _B2, _H6, _2d_hbody2-_2d_heck6, XY( -98,  -59),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 316, _B2, _H7, _2d_hbody2-_2d_heck7, XY(-136, -113),  NOTE_STD, NOTE_STD, NOTE_SPEED800, 0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 320, _B3, _H1, _2d_hbody3-_2d_heck1, XY(-130, -121),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 321, _B3, _H2, _2d_hbody3-_2d_heck2, XY(-126,  -91),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 322, _B3, _H3, _2d_hbody3-_2d_heck3, XY(-110,  -90),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 323, _B3, _H4, _2d_hbody3-_2d_heck4, XY( -78,  -85),  NOTE_STD, NOTE_STD, NOTE_SPEED500, 0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 324, _B3, _H5, _2d_hbody3-_2d_heck5, XY(-111,  -87),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 325, _B3, _H6, _2d_hbody3-_2d_heck6, XY( -98,  -59),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 326, _B3, _H7, _2d_hbody3-_2d_heck7, XY(-136, -113),  NOTE_STD, NOTE_STD, NOTE_SPEED800, 0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 330, _B4, _H1, _2d_hbody4-_2d_heck1, XY(-130, -121),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 331, _B4, _H2, _2d_hbody4-_2d_heck2, XY(-126,  -91),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 332, _B4, _H3, _2d_hbody4-_2d_heck3, XY(-110,  -90),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 333, _B4, _H4, _2d_hbody4-_2d_heck4, XY( -78,  -85),  NOTE_STD, NOTE_STD, NOTE_SPEED500, 0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 334, _B4, _H5, _2d_hbody4-_2d_heck5, XY(-111,  -87),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 335, _B4, _H6, _2d_hbody4-_2d_heck6, XY( -98,  -59),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 336, _B4, _H7, _2d_hbody4-_2d_heck7, XY(-136, -113),  NOTE_STD, NOTE_STD, NOTE_SPEED800, 0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 340, _B5, _H1, _2d_hbody5-_2d_heck1, XY(-130, -121),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 341, _B5, _H2, _2d_hbody5-_2d_heck2, XY(-126,  -91),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 342, _B5, _H3, _2d_hbody5-_2d_heck3, XY(-110,  -90),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 343, _B5, _H4, _2d_hbody5-_2d_heck4, XY( -78,  -85),  NOTE_STD, NOTE_STD, NOTE_SPEED500, 0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 344, _B5, _H5, _2d_hbody5-_2d_heck5, XY(-111,  -87),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 345, _B5, _H6, _2d_hbody5-_2d_heck6, XY( -98,  -59),  NOTE_STD, NOTE_STD, NOTE_STD,      0,    0,     "H0", "H0" ),
   CPlanePartRelation ( 346, _B5, _H7, _2d_hbody5-_2d_heck7, XY(-136, -113),  NOTE_STD, NOTE_STD, NOTE_SPEED800, 0,    0,     "H0", "H0" ),

   //Bug->Flügel     // Id,  From, To, Offset2d,     Offset3d,        Note1,    Note1,    Note1,    zAdd, Noise, Slot, RulesOutSlots
   CPlanePartRelation ( 400, _B1, _R4, _2d_tbody1-_2d_rght4, XY(-128,   59),  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "RxR1R2R3" ),
   CPlanePartRelation ( 600, _B1, _L4, _2d_tbody1-_2d_left4, XY(  42,  -93),  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "LxL1L2L3" ),
   CPlanePartRelation ( 401, _B1, _R5, _2d_tbody1-_2d_rght5, XY( -66,   58),  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "RxR1R2R3" ),
   CPlanePartRelation ( 601, _B1, _L5, _2d_tbody1-_2d_left5, XY(  35,  -46),  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "LxL1L2L3" ),

   CPlanePartRelation ( 411, _B2, _R1, _2d_tbody2b-_2d_rght1, _rbody2b-_rwing1, NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "RxR1R2R3" ),
   CPlanePartRelation ( 611, _B2, _L1, _2d_tbody2b-_2d_left1, _lbody2b-_lwing1, NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "LxL1L2R5" ),
   CPlanePartRelation ( 413, _B2, _R1, _2d_tbody2d-_2d_rght1, _rbody2d-_rwing1, NOTE_STD, NOTE_STD, NOTE_STD, -400, 0,     "Rx", "RxR4" ),
   CPlanePartRelation ( 613, _B2, _L1, _2d_tbody2d-_2d_left1, _lbody2d-_lwing1, NOTE_STD, NOTE_STD, NOTE_STD, -400, 0,     "Lx", "LxL4" ),

   CPlanePartRelation ( 420, _B2, _R2, _2d_tbody2a-_2d_rght2, _rbody2a-_rwing2, NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "RxR1R2" ),
   CPlanePartRelation ( 620, _B2, _L2, _2d_tbody2a-_2d_left2, _lbody2a-_lwing2, NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "LxL1L2" ),
   CPlanePartRelation ( 421, _B2, _R2, _2d_tbody2b-_2d_rght2, _rbody2b-_rwing2, NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "RxR1R2R3" ),
   CPlanePartRelation ( 621, _B2, _L2, _2d_tbody2b-_2d_left2, _lbody2b-_lwing2, NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "LxL1L2R5" ),
   CPlanePartRelation ( 422, _B2, _R2, _2d_tbody2c-_2d_rght2, _rbody2c-_rwing2, NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "RxR2R3" ),
   CPlanePartRelation ( 622, _B2, _L2, _2d_tbody2c-_2d_left2, _lbody2c-_lwing2, NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "LxL2L3" ),
   CPlanePartRelation ( 423, _B2, _R2, _2d_tbody2d-_2d_rght2, _rbody2d-_rwing2, NOTE_STD, NOTE_STD, NOTE_STD, -400, 0,     "Rx", "RxR4" ),
   CPlanePartRelation ( 623, _B2, _L2, _2d_tbody2d-_2d_left2, _lbody2d-_lwing2, NOTE_STD, NOTE_STD, NOTE_STD, -400, 0,     "Lx", "LxL4" ),

   CPlanePartRelation ( 424, _B2, _R3, _2d_tbody2b-_2d_rght3, _rbody2b-_rwing3, NOTE_STD, NOTE_STD, NOTE_STD, -400, 0,     "Rx", "RxR1R2R3" ),
   CPlanePartRelation ( 624, _B2, _L3, _2d_tbody2b-_2d_left3, _lbody2b-_lwing3, NOTE_STD, NOTE_STD, NOTE_STD, -400, 0,     "Lx", "LxL1L2R5" ),

   CPlanePartRelation ( 430, _B2, _R4, _2d_tbody2a-_2d_rght4, _rbody2a-_rwing4, NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "RxR1R2" ),
   CPlanePartRelation ( 630, _B2, _L4, _2d_tbody2a-_2d_left4, _lbody2a-_lwing4, NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "LxL1L2" ),
   CPlanePartRelation ( 431, _B2, _R4, _2d_tbody2b-_2d_rght4, _rbody2b-_rwing4, NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "RxR1R2R3" ),
   CPlanePartRelation ( 631, _B2, _L4, _2d_tbody2b-_2d_left4, _lbody2b-_lwing4, NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "LxL1L2R5" ),
   CPlanePartRelation ( 432, _B2, _R4, _2d_tbody2c-_2d_rght4, _rbody2c-_rwing4, NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "RxR2R3" ),
   CPlanePartRelation ( 632, _B2, _L4, _2d_tbody2c-_2d_left2, _lbody2c-_lwing4, NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "LxL2L3" ),
   CPlanePartRelation ( 433, _B2, _R4, _2d_tbody2d-_2d_rght4, _rbody2d-_rwing4, NOTE_STD, NOTE_STD, NOTE_STD, -400, 0,     "Rx", "RxR4" ),
   CPlanePartRelation ( 633, _B2, _L4, _2d_tbody2d-_2d_left4, _lbody2d-_lwing4, NOTE_STD, NOTE_STD, NOTE_STD, -400, 0,     "Lx", "LxL4" ),

   CPlanePartRelation ( 444, _B2, _R6, _2d_tbody2b-_2d_rght6, _rbody2b-_rwing6, NOTE_STD, NOTE_STD, NOTE_STD, -400, 0,     "Rx", "RxR1R2R3" ),
   CPlanePartRelation ( 644, _B2, _L6, _2d_tbody2b-_2d_left6, _lbody2b-_lwing6, NOTE_STD, NOTE_STD, NOTE_STD, -400, 0,     "Lx", "LxL1L2R5" ),

   CPlanePartRelation ( 450, _B3, _R1, _2d_tbody3-_2d_rght1, _rbody3-_rwing1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "Rx" ),
   CPlanePartRelation ( 650, _B3, _L1, _2d_tbody3-_2d_left1, _lbody3-_lwing1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "Lx" ),
   CPlanePartRelation ( 451, _B3, _R2, _2d_tbody3-_2d_rght2, _rbody3-_rwing2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "Rx" ),
   CPlanePartRelation ( 651, _B3, _L2, _2d_tbody3-_2d_left2, _lbody3-_lwing2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "Lx" ),
   CPlanePartRelation ( 452, _B3, _R4, _2d_tbody3-_2d_rght4, _rbody3-_rwing4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "Rx" ),
   CPlanePartRelation ( 652, _B3, _L4, _2d_tbody3-_2d_left4, _lbody3-_lwing4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "Lx" ),
   CPlanePartRelation ( 453, _B3, _R5, _2d_tbody3-_2d_rght5, _rbody3-_rwing5,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "Rx" ),
   CPlanePartRelation ( 653, _B3, _L5, _2d_tbody3-_2d_left5, _lbody3-_lwing5,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "Lx" ),

   CPlanePartRelation ( 460, _B4, _R2, _2d_tbody4-_2d_rght2, _rbody4-_rwing2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "Rx" ),
   CPlanePartRelation ( 660, _B4, _L2, _2d_tbody4-_2d_left2, _lbody4-_lwing2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "Lx" ),
   CPlanePartRelation ( 461, _B4, _R4, _2d_tbody4-_2d_rght4, _rbody4-_rwing4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "Rx" ),
   CPlanePartRelation ( 661, _B4, _L4, _2d_tbody4-_2d_left4, _lbody4-_lwing4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "Lx" ),
   CPlanePartRelation ( 462, _B4, _R5, _2d_tbody4-_2d_rght5, _rbody4-_rwing5-XY(14,5),  NOTE_STD, NOTE_STD, NOTE_STD, 0,  0, "Rx", "Rx" ),
   CPlanePartRelation ( 662, _B4, _L5, _2d_tbody4-_2d_left5, _lbody4-_lwing5,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "Lx" ),

   CPlanePartRelation ( 470, _B5, _R1, _2d_tbody5-_2d_rght1, _rbody5-_rwing1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "Rx" ),
   CPlanePartRelation ( 670, _B5, _L1, _2d_tbody5-_2d_left1, _lbody5-_lwing1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "Lx" ),
   CPlanePartRelation ( 471, _B5, _R2, _2d_tbody5-_2d_rght2, _rbody5-_rwing2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "Rx" ),
   CPlanePartRelation ( 671, _B5, _L2, _2d_tbody5-_2d_left2, _lbody5-_lwing2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "Lx" ),
   CPlanePartRelation ( 472, _B5, _R3, _2d_tbody5-_2d_rght3, _rbody5-_rwing3,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "Rx" ),
   CPlanePartRelation ( 672, _B5, _L3, _2d_tbody5-_2d_left3, _lbody5-_lwing3,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "Lx" ),
   CPlanePartRelation ( 473, _B5, _R4, _2d_tbody5-_2d_rght4, _rbody5-_rwing4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "Rx" ),
   CPlanePartRelation ( 673, _B5, _L4, _2d_tbody5-_2d_left4, _lbody5-_lwing4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "Lx" ),
   CPlanePartRelation ( 474, _B5, _R5, _2d_tbody5-_2d_rght5, _rbody5-_rwing5,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Rx", "Rx" ),
   CPlanePartRelation ( 674, _B5, _L5, _2d_tbody5-_2d_left5, _lbody5-_lwing5+XY(45,15),  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "Lx", "Lx" ),

   //Flügel->Motor   // Id,  From, To, Offset2d,                  Offset3d,            Note1,    Note1,    Note1,    zAdd, Noise, Slot, RulesOutSlots
   CPlanePartRelation ( 700, _R1, _M1, _2d_m_rwing1a-_2d_motor1, _m_rwing1a-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 2,     "M1", "M1" ),
   CPlanePartRelation ( 710, _L1, _M1, _2d_m_lwing1a-_2d_motor1, _m_lwing1a-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    2,     "M4", "M4" ),
   CPlanePartRelation ( 701, _R1, _M2, _2d_m_rwing1a-_2d_motor2, _m_rwing1a-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 15,    "M1", "M1" ),
   CPlanePartRelation ( 711, _L1, _M2, _2d_m_lwing1a-_2d_motor2, _m_lwing1a-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    15,    "M4", "M4" ),
   CPlanePartRelation ( 704, _R1, _M5, XY(90, 22),               XY(-33, 40),         NOTE_STD, NOTE_STD, NOTE_STD, 5000, 0,     "MR", "MR" ),
   CPlanePartRelation ( 714, _L1, _M5, XY(64, -14),              XY(102, -27),        NOTE_STD, NOTE_STD, NOTE_STD, 1000, 0,     "ML", "ML" ),
   CPlanePartRelation ( 707, _R1, _M8, _2d_m_rwing1a-_2d_motor8, _m_rwing1a-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 8,     "M1", "M1M2" ),
   CPlanePartRelation ( 717, _L1, _M8, _2d_m_lwing1a-_2d_motor8, _m_lwing1a-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    8,     "M4", "M4M5" ),
                                                         
   CPlanePartRelation ( 720, _R1, _M1, _2d_m_rwing1b-_2d_motor1, _m_rwing1b-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation ( 730, _L1, _M1, _2d_m_lwing1b-_2d_motor1, _m_lwing1b-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation ( 721, _R1, _M2, _2d_m_rwing1b-_2d_motor2, _m_rwing1b-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation ( 731, _L1, _M2, _2d_m_lwing1b-_2d_motor2, _m_lwing1b-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation ( 722, _R1, _M3, _2d_m_rwing1b-_2d_motor3, _m_rwing1b-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation ( 732, _L1, _M3, _2d_m_lwing1b-_2d_motor3, _m_lwing1b-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation ( 723, _R1, _M4, _2d_m_rwing1b-_2d_motor4, _m_rwing1b-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation ( 733, _L1, _M4, _2d_m_lwing1b-_2d_motor4, _m_lwing1b-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation ( 725, _R1, _M6, _2d_m_rwing1b-_2d_motor6, _m_rwing1b-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M1M2M3" ),
   CPlanePartRelation ( 735, _L1, _M6, _2d_m_lwing1b-_2d_motor6, _m_lwing1b-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M4M5M6" ),
   CPlanePartRelation ( 726, _R1, _M7, _2d_m_rwing1b-_2d_motor7, _m_rwing1b-_motor7,  NOTE_STD, NOTE_PILOT1, NOTE_STD, 3000, 0,     "M2", "M1M2M3" ),
   CPlanePartRelation ( 736, _L1, _M7, _2d_m_lwing1b-_2d_motor7, _m_lwing1b-_motor7,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M4M5M6" ),
   CPlanePartRelation ( 727, _R1, _M8, _2d_m_rwing1b-_2d_motor8, _m_rwing1b-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M1M2M3" ),
   CPlanePartRelation ( 737, _L1, _M8, _2d_m_lwing1b-_2d_motor8, _m_lwing1b-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M4M5M6" ),
                                                         
   CPlanePartRelation ( 740, _R1, _M1, _2d_m_rwing1c-_2d_motor1, _m_rwing1c-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M3" ),
   CPlanePartRelation ( 750, _L1, _M1, _2d_m_lwing1c-_2d_motor1, _m_lwing1c-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M6" ),
   CPlanePartRelation ( 741, _R1, _M2, _2d_m_rwing1c-_2d_motor2, _m_rwing1c-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M3" ),
   CPlanePartRelation ( 751, _L1, _M2, _2d_m_lwing1c-_2d_motor2, _m_lwing1c-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M6" ),
   CPlanePartRelation ( 742, _R1, _M3, _2d_m_rwing1c-_2d_motor3, _m_rwing1c-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M3" ),
   CPlanePartRelation ( 752, _L1, _M3, _2d_m_lwing1c-_2d_motor3, _m_lwing1c-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M6" ),
   CPlanePartRelation ( 743, _R1, _M4, _2d_m_rwing1c-_2d_motor4, _m_rwing1c-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M3" ),
   CPlanePartRelation ( 753, _L1, _M4, _2d_m_lwing1c-_2d_motor4, _m_lwing1c-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M6" ),
   CPlanePartRelation ( 745, _R1, _M6, _2d_m_rwing1c-_2d_motor6, _m_rwing1c-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M2M3" ),
   CPlanePartRelation ( 755, _L1, _M6, _2d_m_lwing1c-_2d_motor6, _m_lwing1c-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M5M6" ),
   CPlanePartRelation ( 746, _R1, _M7, _2d_m_rwing1c-_2d_motor7, _m_rwing1c-_motor7,  NOTE_STD, NOTE_PILOT1, NOTE_STD, 3000, 0,     "M3", "M2M3" ),
   CPlanePartRelation ( 756, _L1, _M7, _2d_m_lwing1c-_2d_motor7, _m_lwing1c-_motor7,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M5M6" ),
   CPlanePartRelation ( 747, _R1, _M8, _2d_m_rwing1c-_2d_motor8, _m_rwing1c-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M2M3" ),
   CPlanePartRelation ( 757, _L1, _M8, _2d_m_lwing1c-_2d_motor8, _m_lwing1c-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M5M6" ),

   //Flügel2->Motor  // Id,  From, To, Offset2d,                 Offset3d,            Note1,    Note1,    Note1,    zAdd, Slot, RulesOutSlots
   CPlanePartRelation ( 800, _R2, _M1, _2d_m_rwing2a-_2d_motor1, _m_rwing2a-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 2,     "M1", "M1" ),
   CPlanePartRelation ( 810, _L2, _M1, _2d_m_lwing2a-_2d_motor1, _m_lwing2a-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    2,     "M4", "M4" ),
   CPlanePartRelation ( 801, _R2, _M2, _2d_m_rwing2a-_2d_motor2, _m_rwing2a-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 15,    "M1", "M1" ),
   CPlanePartRelation ( 811, _L2, _M2, _2d_m_lwing2a-_2d_motor2, _m_lwing2a-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    15,    "M4", "M4" ),
   CPlanePartRelation ( 807, _R2, _M8, _2d_m_rwing2a-_2d_motor8, _m_rwing2a-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 8,     "M1", "M1M2" ),
   CPlanePartRelation ( 817, _L2, _M8, _2d_m_lwing2a-_2d_motor8, _m_lwing2a-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    8,     "M4", "M4M5" ),
                                                         
   CPlanePartRelation ( 820, _R2, _M1, _2d_m_rwing2b-_2d_motor1, _m_rwing2b-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation ( 830, _L2, _M1, _2d_m_lwing2b-_2d_motor1, _m_lwing2b-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation ( 821, _R2, _M2, _2d_m_rwing2b-_2d_motor2, _m_rwing2b-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation ( 831, _L2, _M2, _2d_m_lwing2b-_2d_motor2, _m_lwing2b-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation ( 822, _R2, _M3, _2d_m_rwing2b-_2d_motor3, _m_rwing2b-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation ( 832, _L2, _M3, _2d_m_lwing2b-_2d_motor3, _m_lwing2b-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation ( 823, _R2, _M4, _2d_m_rwing2b-_2d_motor4, _m_rwing2b-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation ( 833, _L2, _M4, _2d_m_lwing2b-_2d_motor4, _m_lwing2b-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation ( 824, _R2, _M5, XY(87, 33),               XY(-37, 43),         NOTE_STD, NOTE_STD, NOTE_STD, 5000, 0,     "MR", "MR" ),
   CPlanePartRelation ( 834, _L2, _M5, XY(86, -14),              XY(52, -23),         NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "ML", "ML" ),
   CPlanePartRelation ( 825, _R2, _M6, _2d_m_rwing2b-_2d_motor6, _m_rwing2b-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M1M2M3" ),
   CPlanePartRelation ( 835, _L2, _M6, _2d_m_lwing2b-_2d_motor6, _m_lwing2b-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M4M5M6" ),
   CPlanePartRelation ( 826, _R2, _M7, _2d_m_rwing2b-_2d_motor7, _m_rwing2b-_motor7,  NOTE_STD, NOTE_PILOT1, NOTE_STD, 3000, 0,     "M2", "M1M2M3" ),
   CPlanePartRelation ( 836, _L2, _M7, _2d_m_lwing2b-_2d_motor7, _m_lwing2b-_motor7,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M4M5M6" ),
   CPlanePartRelation ( 827, _R2, _M8, _2d_m_rwing2b-_2d_motor8, _m_rwing2b-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M1M2M3" ),
   CPlanePartRelation ( 837, _L2, _M8, _2d_m_lwing2b-_2d_motor8, _m_lwing2b-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M4M5M6" ),
                                                         
   CPlanePartRelation ( 840, _R2, _M1, _2d_m_rwing2c-_2d_motor1, _m_rwing2c-_motor1,  NOTE_KAPUTT, NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M3" ),
   CPlanePartRelation ( 850, _L2, _M1, _2d_m_lwing2c-_2d_motor1, _m_lwing2c-_motor1,  NOTE_KAPUTT, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M6" ),
   CPlanePartRelation ( 841, _R2, _M2, _2d_m_rwing2c-_2d_motor2, _m_rwing2c-_motor2,  NOTE_STD,    NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M3" ),
   CPlanePartRelation ( 851, _L2, _M2, _2d_m_lwing2c-_2d_motor2, _m_lwing2c-_motor2,  NOTE_STD,    NOTE_STD, NOTE_STD, 0,    0,     "M6", "M6" ),
   CPlanePartRelation ( 842, _R2, _M3, _2d_m_rwing2c-_2d_motor3, _m_rwing2c-_motor3,  NOTE_STD,    NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M3" ),
   CPlanePartRelation ( 852, _L2, _M3, _2d_m_lwing2c-_2d_motor3, _m_lwing2c-_motor3,  NOTE_STD,    NOTE_STD, NOTE_STD, 0,    0,     "M6", "M6" ),
   CPlanePartRelation ( 843, _R2, _M4, _2d_m_rwing2c-_2d_motor4, _m_rwing2c-_motor4,  NOTE_KAPUTT, NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M3" ),
   CPlanePartRelation ( 853, _L2, _M4, _2d_m_lwing2c-_2d_motor4, _m_lwing2c-_motor4,  NOTE_KAPUTT, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M6" ),
   CPlanePartRelation ( 845, _R2, _M6, _2d_m_rwing2c-_2d_motor6, _m_rwing2c-_motor6,  NOTE_KAPUTTXL, NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M2M3" ),
   CPlanePartRelation ( 855, _L2, _M6, _2d_m_lwing2c-_2d_motor6, _m_lwing2c-_motor6,  NOTE_KAPUTTXL, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M5M6" ),
   CPlanePartRelation ( 846, _R2, _M7, _2d_m_rwing2c-_2d_motor7, _m_rwing2c-_motor7,  NOTE_KAPUTTXL, NOTE_PILOT1, NOTE_STD, 3000, 0,     "M3", "M2M3" ),
   CPlanePartRelation ( 856, _L2, _M7, _2d_m_lwing2c-_2d_motor7, _m_lwing2c-_motor7,  NOTE_KAPUTTXL, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M5M6" ),
   CPlanePartRelation ( 847, _R2, _M8, _2d_m_rwing2c-_2d_motor8, _m_rwing2c-_motor8,  NOTE_KAPUTTXL, NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M2M3" ),
   CPlanePartRelation ( 857, _L2, _M8, _2d_m_lwing2c-_2d_motor8, _m_lwing2c-_motor8,  NOTE_KAPUTTXL, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M5M6" ),

   //Flügel2->Motor  // Id,  From, To, Offset2d,                 Offset3d,            Note1,    Note1,    Note1,    zAdd, Noise, Slot, RulesOutSlots
   CPlanePartRelation ( 900, _R3, _M1, _2d_m_rwing3a-_2d_motor1, _m_rwing3a-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 2,     "M1", "M1" ),
   CPlanePartRelation ( 910, _L3, _M1, _2d_m_lwing3a-_2d_motor1, _m_lwing3a-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    2,     "M4", "M4" ),
   CPlanePartRelation ( 901, _R3, _M2, _2d_m_rwing3a-_2d_motor2, _m_rwing3a-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 20,    "M1", "M1" ),
   CPlanePartRelation ( 911, _L3, _M2, _2d_m_lwing3a-_2d_motor2, _m_lwing3a-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    20,    "M4", "M4" ),
   CPlanePartRelation ( 904, _R3, _M5, XY(115, 54),              XY(-19, 81),         NOTE_STD, NOTE_STD, NOTE_STD, 5000, 0,     "MR", "MR" ),
   CPlanePartRelation ( 914, _L3, _M5, XY(111, -13),             XY(126, -18),        NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "ML", "ML" ),
   CPlanePartRelation ( 907, _R3, _M8, _2d_m_rwing3a-_2d_motor8, _m_rwing3a-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 15,    "M1", "M1M2" ),
   CPlanePartRelation ( 917, _L3, _M8, _2d_m_lwing3a-_2d_motor8, _m_lwing3a-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    15,    "M4", "M4M5" ),
                                                         
   CPlanePartRelation ( 920, _R3, _M1, _2d_m_rwing3b-_2d_motor1, _m_rwing3b-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation ( 930, _L3, _M1, _2d_m_lwing3b-_2d_motor1, _m_lwing3b-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation ( 921, _R3, _M2, _2d_m_rwing3b-_2d_motor2, _m_rwing3b-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation ( 931, _L3, _M2, _2d_m_lwing3b-_2d_motor2, _m_lwing3b-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation ( 922, _R3, _M3, _2d_m_rwing3b-_2d_motor3, _m_rwing3b-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation ( 932, _L3, _M3, _2d_m_lwing3b-_2d_motor3, _m_lwing3b-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation ( 923, _R3, _M4, _2d_m_rwing3b-_2d_motor4, _m_rwing3b-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation ( 933, _L3, _M4, _2d_m_lwing3b-_2d_motor4, _m_lwing3b-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation ( 925, _R3, _M6, _2d_m_rwing3b-_2d_motor6, _m_rwing3b-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M1M2M3" ),
   CPlanePartRelation ( 935, _L3, _M6, _2d_m_lwing3b-_2d_motor6, _m_lwing3b-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M4M5M6" ),
   CPlanePartRelation ( 926, _R3, _M7, _2d_m_rwing3b-_2d_motor7, _m_rwing3b-_motor7,  NOTE_STD, NOTE_PILOT1, NOTE_STD, 3000, 0,     "M2", "M1M2M3" ),
   CPlanePartRelation ( 936, _L3, _M7, _2d_m_lwing3b-_2d_motor7, _m_lwing3b-_motor7,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M4M5M6" ),
   CPlanePartRelation ( 927, _R3, _M8, _2d_m_rwing3b-_2d_motor8, _m_rwing3b-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M1M2M3" ),
   CPlanePartRelation ( 937, _L3, _M8, _2d_m_lwing3b-_2d_motor8, _m_lwing3b-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M4M5M6" ),
                                                         
   CPlanePartRelation ( 940, _R3, _M1, _2d_m_rwing3c-_2d_motor1, _m_rwing3c-_motor1,  NOTE_KAPUTT, NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M3" ),
   CPlanePartRelation ( 950, _L3, _M1, _2d_m_lwing3c-_2d_motor1, _m_lwing3c-_motor1,  NOTE_KAPUTT, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M6" ),
   CPlanePartRelation ( 941, _R3, _M2, _2d_m_rwing3c-_2d_motor2, _m_rwing3c-_motor2,  NOTE_STD,    NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M3" ),
   CPlanePartRelation ( 951, _L3, _M2, _2d_m_lwing3c-_2d_motor2, _m_lwing3c-_motor2,  NOTE_STD,    NOTE_STD, NOTE_STD, 0,    0,     "M6", "M6" ),
   CPlanePartRelation ( 942, _R3, _M3, _2d_m_rwing3c-_2d_motor3, _m_rwing3c-_motor3,  NOTE_STD,    NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M3" ),
   CPlanePartRelation ( 952, _L3, _M3, _2d_m_lwing3c-_2d_motor3, _m_lwing3c-_motor3,  NOTE_STD,    NOTE_STD, NOTE_STD, 0,    0,     "M6", "M6" ),
   CPlanePartRelation ( 943, _R3, _M4, _2d_m_rwing3c-_2d_motor4, _m_rwing3c-_motor4,  NOTE_KAPUTT, NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M3" ),
   CPlanePartRelation ( 953, _L3, _M4, _2d_m_lwing3c-_2d_motor4, _m_lwing3c-_motor4,  NOTE_KAPUTT, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M6" ),
   CPlanePartRelation ( 945, _R3, _M6, _2d_m_rwing3c-_2d_motor6, _m_rwing3c-_motor6,  NOTE_KAPUTTXL, NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M2M3" ),
   CPlanePartRelation ( 955, _L3, _M6, _2d_m_lwing3c-_2d_motor6, _m_lwing3c-_motor6,  NOTE_KAPUTTXL, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M5M6" ),
   CPlanePartRelation ( 946, _R3, _M7, _2d_m_rwing3c-_2d_motor7, _m_rwing3c-_motor7,  NOTE_KAPUTTXL, NOTE_PILOT1, NOTE_STD, 3000, 0,     "M3", "M2M3" ),
   CPlanePartRelation ( 956, _L3, _M7, _2d_m_lwing3c-_2d_motor7, _m_lwing3c-_motor7,  NOTE_KAPUTTXL, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M5M6" ),
   CPlanePartRelation ( 947, _R3, _M8, _2d_m_rwing3c-_2d_motor8, _m_rwing3c-_motor8,  NOTE_KAPUTTXL, NOTE_STD, NOTE_STD, 3000, 0,     "M3", "M2M3" ),
   CPlanePartRelation ( 957, _L3, _M8, _2d_m_lwing3c-_2d_motor8, _m_lwing3c-_motor8,  NOTE_KAPUTTXL, NOTE_STD, NOTE_STD, 0,    0,     "M6", "M5M6" ),

   //Flügel4->Motor  // Id,  From, To, Offset2d,                 Offset3d,            Note1,    Note1,    Note1,    zAdd, Slot, RulesOutSlots
   CPlanePartRelation (1000, _R4, _M1, _2d_m_rwing4a-_2d_motor1, _m_rwing4a-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 2,     "M3", "M3" ),
   CPlanePartRelation (1010, _L4, _M1, _2d_m_lwing4a-_2d_motor1, _m_lwing4a-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    2,     "M6", "M6" ),
   CPlanePartRelation (1001, _R4, _M2, _2d_m_rwing4a-_2d_motor2, _m_rwing4a-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 20,    "M3", "M3" ),
   CPlanePartRelation (1011, _L4, _M2, _2d_m_lwing4a-_2d_motor2, _m_lwing4a-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    20,    "M6", "M6" ),
   CPlanePartRelation (1002, _R4, _M3, _2d_m_rwing4a-_2d_motor3, _m_rwing4a-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 20,    "M3", "M3" ),
   CPlanePartRelation (1012, _L4, _M3, _2d_m_lwing4a-_2d_motor3, _m_lwing4a-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    20,    "M6", "M6" ),
   CPlanePartRelation (1003, _R4, _M4, _2d_m_rwing4a-_2d_motor4, _m_rwing4a-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 6,     "M3", "M3" ),
   CPlanePartRelation (1013, _L4, _M4, _2d_m_lwing4a-_2d_motor4, _m_lwing4a-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    6,     "M6", "M6" ),
   CPlanePartRelation (1004, _R4, _M5, XY(22, 61),               XY(-24, 107),        NOTE_STD, NOTE_STD, NOTE_STD, 5000, 0,     "MR", "MR" ),
   CPlanePartRelation (1014, _L4, _M5, XY(18, -13),              XY(137, -24),        NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "ML", "ML" ),
   CPlanePartRelation (1005, _R4, _M6, _2d_m_rwing4a-_2d_motor6, _m_rwing4a-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 14,    "ML", "M3" ),
   CPlanePartRelation (1015, _L4, _M6, _2d_m_lwing4a-_2d_motor6, _m_lwing4a-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    14,    "M6", "M6" ),
   CPlanePartRelation (1006, _R4, _M7, _2d_m_rwing4a-_2d_motor7, _m_rwing4a-_motor7,  NOTE_STD, NOTE_PILOT1, NOTE_STD, 3000, 20,    "M3", "M3" ),
   CPlanePartRelation (1016, _L4, _M7, _2d_m_lwing4a-_2d_motor7, _m_lwing4a-_motor7,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    20,    "M6", "M6" ),
   CPlanePartRelation (1007, _R4, _M8, _2d_m_rwing4a-_2d_motor8, _m_rwing4a-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 15,    "M3", "M3" ),
   CPlanePartRelation (1017, _L4, _M8, _2d_m_lwing4a-_2d_motor8, _m_lwing4a-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    15,    "M6", "M6" ),
                                                        
   CPlanePartRelation (1020, _R4, _M1, _2d_m_rwing4b-_2d_motor1, _m_rwing4b-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation (1030, _L4, _M1, _2d_m_lwing4b-_2d_motor1, _m_lwing4b-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation (1021, _R4, _M2, _2d_m_rwing4b-_2d_motor2, _m_rwing4b-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation (1031, _L4, _M2, _2d_m_lwing4b-_2d_motor2, _m_lwing4b-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation (1022, _R4, _M3, _2d_m_rwing4b-_2d_motor3, _m_rwing4b-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation (1032, _L4, _M3, _2d_m_lwing4b-_2d_motor3, _m_lwing4b-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation (1023, _R4, _M4, _2d_m_rwing4b-_2d_motor4, _m_rwing4b-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation (1033, _L4, _M4, _2d_m_lwing4b-_2d_motor4, _m_lwing4b-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation (1025, _R4, _M6, _2d_m_rwing4b-_2d_motor6, _m_rwing4b-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation (1035, _L4, _M6, _2d_m_lwing4b-_2d_motor6, _m_lwing4b-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation (1026, _R4, _M7, _2d_m_rwing4b-_2d_motor7, _m_rwing4b-_motor7,  NOTE_STD, NOTE_PILOT1, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation (1036, _L4, _M7, _2d_m_lwing4b-_2d_motor7, _m_lwing4b-_motor7,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation (1027, _R4, _M8, _2d_m_rwing4b-_2d_motor8, _m_rwing4b-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation (1037, _L4, _M8, _2d_m_lwing4b-_2d_motor8, _m_lwing4b-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),

   //Flügel4->Motor  // Id,  From, To, Offset2d,                 Offset3d,            Note1,    Note1,    Note1,    zAdd, Slot, RulesOutSlots
   CPlanePartRelation (1100, _R5, _M1, _2d_m_rwing5-_2d_motor1, _m_rwing5-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 3000,  2,    "M3", "M3" ),
   CPlanePartRelation (1110, _L5, _M1, _2d_m_lwing5-_2d_motor1, _m_lwing5-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,     2,    "M6", "M6" ),
   CPlanePartRelation (1101, _R5, _M2, _2d_m_rwing5-_2d_motor2, _m_rwing5-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 3000,  20,   "M3", "M3" ),
   CPlanePartRelation (1111, _L5, _M2, _2d_m_lwing5-_2d_motor2, _m_lwing5-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,     20,   "M6", "M6" ),
   CPlanePartRelation (1102, _R5, _M3, _2d_m_rwing5-_2d_motor3, _m_rwing5-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 3000,  20,   "M3", "M3" ),
   CPlanePartRelation (1112, _L5, _M3, _2d_m_lwing5-_2d_motor3, _m_lwing5-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 0,     20,   "M6", "M6" ),
   CPlanePartRelation (1103, _R5, _M4, _2d_m_rwing5-_2d_motor4, _m_rwing5-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 3000,  6,    "M3", "M3" ),
   CPlanePartRelation (1113, _L5, _M4, _2d_m_lwing5-_2d_motor4, _m_lwing5-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,     6,    "M6", "M6" ),
   CPlanePartRelation (1104, _R5, _M5, XY(13,37),               XY(-16, 67),        NOTE_STD, NOTE_STD, NOTE_STD, 5000,  0,    "MR", "MR" ),
   CPlanePartRelation (1114, _L5, _M5, XY(13, -13),             XY(103, -22),       NOTE_STD, NOTE_STD, NOTE_STD, 0,     0,    "ML", "ML" ),
   CPlanePartRelation (1105, _R5, _M6, _2d_m_rwing5-_2d_motor6, _m_rwing5-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 3000,  14,   "M3", "M2M3" ),
   CPlanePartRelation (1115, _L5, _M6, _2d_m_lwing5-_2d_motor6, _m_lwing5-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 0,     14,   "M6", "M5M6" ),
   CPlanePartRelation (1106, _R5, _M7, _2d_m_rwing5-_2d_motor7, _m_rwing5-_motor7,  NOTE_STD, NOTE_PILOT1, NOTE_STD, 3000,  20,   "M3", "M2M3" ),
   CPlanePartRelation (1116, _L5, _M7, _2d_m_lwing5-_2d_motor7, _m_lwing5-_motor7,  NOTE_STD, NOTE_STD, NOTE_STD, 0,     20,   "M6", "M5M6" ),
   CPlanePartRelation (1107, _R5, _M8, _2d_m_rwing5-_2d_motor8, _m_rwing5-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 3000,  15,   "M3", "M2M3" ),
   CPlanePartRelation (1117, _L5, _M8, _2d_m_lwing5-_2d_motor8, _m_lwing5-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 0,     15,   "M6", "M5M6" ),

   //Flügel6->Motor  // Id,  From, To, Offset2d,                 Offset3d,            Note1,    Note1,    Note1,    zAdd, Noise, Slot, RulesOutSlots
   CPlanePartRelation (1200, _R6, _M1, _2d_m_rwing6a-_2d_motor1, _m_rwing6a-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 2,     "M3", "M3" ),
   CPlanePartRelation (1210, _L6, _M1, _2d_m_lwing6a-_2d_motor1, _m_lwing6a-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    2,     "M6", "M6" ),
   CPlanePartRelation (1201, _R6, _M2, _2d_m_rwing6a-_2d_motor2, _m_rwing6a-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 20,    "M3", "M3" ),
   CPlanePartRelation (1211, _L6, _M2, _2d_m_lwing6a-_2d_motor2, _m_lwing6a-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    20,    "M6", "M6" ),
   CPlanePartRelation (1202, _R6, _M3, _2d_m_rwing6a-_2d_motor3, _m_rwing6a-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 20,    "M3", "M3" ),
   CPlanePartRelation (1212, _L6, _M3, _2d_m_lwing6a-_2d_motor3, _m_lwing6a-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    20,    "M6", "M6" ),
   CPlanePartRelation (1203, _R6, _M4, _2d_m_rwing6a-_2d_motor4, _m_rwing6a-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 6,     "M3", "M3" ),
   CPlanePartRelation (1213, _L6, _M4, _2d_m_lwing6a-_2d_motor4, _m_lwing6a-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    6,     "M6", "M6" ),
   CPlanePartRelation (1204, _R6, _M5, XY(116, 76),              XY(-19, 112),        NOTE_STD, NOTE_STD, NOTE_STD, 5000, 0,     "MR", "MR" ),
   CPlanePartRelation (1214, _L6, _M5, XY(104, -15),             XY(124, -19),        NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "ML", "ML" ),
   CPlanePartRelation (1205, _R6, _M6, _2d_m_rwing6a-_2d_motor6, _m_rwing6a-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 14,    "M3", "M3" ),
   CPlanePartRelation (1215, _L6, _M6, _2d_m_lwing6a-_2d_motor6, _m_lwing6a-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    14,    "M6", "M6" ),
   CPlanePartRelation (1206, _R6, _M7, _2d_m_rwing6a-_2d_motor7, _m_rwing6a-_motor7,  NOTE_STD, NOTE_PILOT1, NOTE_STD, 3000, 20,    "M3", "M3" ),
   CPlanePartRelation (1216, _L6, _M7, _2d_m_lwing6a-_2d_motor7, _m_lwing6a-_motor7,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    20,    "M6", "M6" ),
   CPlanePartRelation (1207, _R6, _M8, _2d_m_rwing6a-_2d_motor8, _m_rwing6a-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 15,    "M3", "M3" ),
   CPlanePartRelation (1217, _L6, _M8, _2d_m_lwing6a-_2d_motor8, _m_lwing6a-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    15,    "M6", "M6" ),
                                                         
   CPlanePartRelation (1220, _R6, _M1, _2d_m_rwing6b-_2d_motor1, _m_rwing6b-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation (1230, _L6, _M1, _2d_m_lwing6b-_2d_motor1, _m_lwing6b-_motor1,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation (1221, _R6, _M2, _2d_m_rwing6b-_2d_motor2, _m_rwing6b-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation (1231, _L6, _M2, _2d_m_lwing6b-_2d_motor2, _m_lwing6b-_motor2,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation (1222, _R6, _M3, _2d_m_rwing6b-_2d_motor3, _m_rwing6b-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation (1232, _L6, _M3, _2d_m_lwing6b-_2d_motor3, _m_lwing6b-_motor3,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation (1223, _R6, _M4, _2d_m_rwing6b-_2d_motor4, _m_rwing6b-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation (1233, _L6, _M4, _2d_m_lwing6b-_2d_motor4, _m_lwing6b-_motor4,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation (1225, _R6, _M6, _2d_m_rwing6b-_2d_motor6, _m_rwing6b-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation (1235, _L6, _M6, _2d_m_lwing6b-_2d_motor6, _m_lwing6b-_motor6,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation (1226, _R6, _M7, _2d_m_rwing6b-_2d_motor7, _m_rwing6b-_motor7,  NOTE_STD, NOTE_PILOT1, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation (1236, _L6, _M7, _2d_m_lwing6b-_2d_motor7, _m_lwing6b-_motor7,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),
   CPlanePartRelation (1227, _R6, _M8, _2d_m_rwing6b-_2d_motor8, _m_rwing6b-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 3000, 0,     "M2", "M2" ),
   CPlanePartRelation (1237, _L6, _M8, _2d_m_lwing6b-_2d_motor8, _m_lwing6b-_motor8,  NOTE_STD, NOTE_STD, NOTE_STD, 0,    0,     "M5", "M5" ),

   //Heck => Triebwerk  Id,  From, To, Offset2d,                 Offset3d,            Note1,       Note1,    Note1,    zAdd, Noise, Slot, RulesOutSlots
   CPlanePartRelation (1304, _H2, _M5, XY(40, 68),               XY(-47, 99),         NOTE_PILOT1, NOTE_STD, NOTE_STD, 1550, 0,     "Mr", "Mr" ),
   CPlanePartRelation (1314, _H2, _M5, XY(40, 25),               XY(69, 11),          NOTE_STD,    NOTE_STD, NOTE_STD, 1450, 0,     "Ml", "Ml" ),
   CPlanePartRelation (1305, _H5, _M5, XY(24, 71),               XY(-34, 106),        NOTE_PILOT1, NOTE_STD, NOTE_STD, 1550, 0,     "Mr", "Mr" ),
   CPlanePartRelation (1315, _H5, _M5, XY(24, 40),               XY(76, 13),          NOTE_STD,    NOTE_STD, NOTE_STD, 1450, 0,     "Ml", "Ml" ),
   CPlanePartRelation (1306, _H7, _M5, XY(72, 21),               XY(-33, 33),         NOTE_PILOT1, NOTE_STD, NOTE_STD, 1550, 0,     "Mr", "Mr" ),
   CPlanePartRelation (1316, _H7, _M5, XY(72, -11),              XY(34, -24),         NOTE_STD,    NOTE_STD, NOTE_STD, 1450, 0,     "Ml", "Ml" ),
};

//--------------------------------------------------------------------------------------------
//Die Schalter wird eröffnet:
//--------------------------------------------------------------------------------------------
CEditor::CEditor(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "Editor.gli", GFX_EDITOR)
{
   SetRoomVisited (PlayerNum, ROOM_EDITOR);
   HandyOffset = 320;
   Sim.FocusPerson=-1;

   //Tabellen exportieren:
   if (0)
   {
      std::string str;

      //Relations:
      {
         std::ofstream file(FullFilename ("relation.csv", ExcelPath), std::ios_base::trunc|std::ios_base::out);

         str="Id;From;To;Offset2dX;Offset2dY;Offset3dX;Offset3dY;Note1;Note2;Note3;Noise";
         file << str << std::endl;

         for (long c=0; c<sizeof(gPlanePartRelations)/sizeof(gPlanePartRelations[0]); c++)
         {
            str = gPlanePartRelations[c].ToString ();
            file << str << std::endl;
         }
      }

      //Planebuilds:
      {
         std::ofstream file(FullFilename ("builds.csv", ExcelPath), std::ios_base::trunc | std::ios_base::out);

         str="Id;ShortName;Cost;Weight;Power;Noise;Wartung;Passagiere;Verbrauch";
         file << str << std::endl;

         for (long c=0; c<sizeof(gPlaneBuilds)/sizeof(gPlaneBuilds[0]); c++)
         {
            str = gPlaneBuilds[c].ToString ();
            file << str << std::endl;
         }
      }
   }

   //Tabellen importieren:
   if (1)
   {
      std::string str;

      //Relations:
      {
         std::ifstream file(FullFilename ("relation.csv", ExcelPath), std::ios_base::in);

         file >> str;

         for (long c=0; c<sizeof(gPlanePartRelations)/sizeof(gPlanePartRelations[0]); c++)
         {
            file >> str;
            long id=atol(str.c_str());

            if (gPlanePartRelations[c].Id!=id) hprintf (0, "Id mismatch: %li vs %li!", gPlanePartRelations[c].Id, id);
            gPlanePartRelations[c].FromString (str);
         }
      }

      //Planebuilds:
      {
         std::ifstream file(FullFilename ("builds.csv", ExcelPath), std::ios_base::in);

         file >> str;

         for (long c=0; c<sizeof(gPlaneBuilds)/sizeof(gPlaneBuilds[0]); c++)
         {
            file >> str;
            long id = atol(str.c_str());

            if (gPlaneBuilds[c].Id!=id) hprintf (0, "Id mismatch: %li vs %li!", gPlaneBuilds[c].Id, id);
            gPlaneBuilds[c].FromString (str);
         }
      }
   }

   bAllowB=bAllowC=bAllowH=bAllowW=bAllowM=false;

   GripRelation=-1;
   Plane.Parts.PlaneParts.ReSize (100);

   if (!bHandy) AmbientManager.SetGlobalVolume (60);

   GripRelation=-1;
   PartUnderCursor="";
   sel_b=sel_c=sel_h=sel_w=sel_m=0;
   DragDropMode=false;

   Plane.Name = StandardTexte.GetS (TOKEN_MISC, 8210);

   PlaneFilename = FullFilename ("data.plane", MyPlanePath);
   if (DoesFileExist (PlaneFilename)) Plane.Load(PlaneFilename);

   UpdateButtonState ();
   
   PartBms.ReSize (pRoomLib, "BODY_A01 BODY_A02 BODY_A03 BODY_A04 BODY_A05 "
                             "CPIT_A01 CPIT_A02 CPIT_A03 CPIT_A04 CPIT_A05 "
                             "HECK_A01 HECK_A02 HECK_A03 HECK_A04 HECK_A05 HECK_A06 HECK_A07 "
                             "RWNG_A01 RWNG_A02 RWNG_A03 RWNG_A04 RWNG_A05 RWNG_A06 "
                             "MOT_A01  MOT_A02  MOT_A03  MOT_A04  MOT_A05  MOT_A06  MOT_A07  MOT_A08 "
                             "LWNG_A01 LWNG_A02 LWNG_A03 LWNG_A04 LWNG_A05 LWNG_A06 ");

   SelPartBms.ReSize (pRoomLib, "S_B_01 S_B_02 S_B_03 S_B_04 S_B_05 "
                                "S_C_01 S_C_02 S_C_03 S_C_04 S_C_05 "
                                "S_H_01 S_H_02 S_H_03 S_H_04 S_H_05 S_H_06 S_H_07 "
                                "S_W_01 S_W_02 S_W_03 S_W_04 S_W_05 S_W_06 "
                                "S_M_01 S_M_02 S_M_03 S_M_04 S_M_05 S_M_06 S_M_07  S_M_08 ");

   ButtonPartLRBms.ReSize (pRoomLib, "BUT_TL0 BUT_TL1 BUT_TL2 BUT_TR0 BUT_TR1 BUT_TR2");
   ButtonPlaneLRBms.ReSize (pRoomLib, "BUT_DL0 BUT_DL1 BUT_DL2 BUT_DR0 BUT_DR1 BUT_DR2");
   OtherButtonBms.ReSize (pRoomLib, "CANCEL0 CANCEL1 CANCEL2 DELETE0 DELETE1 DELETE2 NEW0 NEW1 NEW2 OK0 OK1 OK2");
   MaskenBms.ReSize (pRoomLib, "MASKE_O MASKE_U");

   CheckUnusablePart (1);
   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);

   Hdu.HercPrintf (0, "stat_3.mcf");
   FontNormalRed.Load (lpDD, (char*)(LPCTSTR)FullFilename ("stat_3.mcf", MiscPath));
   FontYellow.Load (lpDD, (char*)(LPCTSTR)FullFilename ("stat_4.mcf", MiscPath));
   
   //Hintergrundsounds:
   if (Sim.Options.OptionEffekte)
   {
      BackFx.ReInit("secure.raw");
      BackFx.Play(DSBPLAY_NOSTOP|DSBPLAY_LOOPING, Sim.Options.OptionEffekte*100/7);
   }
}

//--------------------------------------------------------------------------------------------
//Sehr destruktiv!
//--------------------------------------------------------------------------------------------
CEditor::~CEditor()
{
   BackFx.SetVolume(DSBVOLUME_MIN);
   BackFx.Stop();
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void CEditor::UpdateButtonState(void)
{
   long d;

   bAllowB=true;
   bAllowC=bAllowH=bAllowW=bAllowM=false;

   for (d=0; d<(long)Plane.Parts.AnzEntries(); d++)
      if (Plane.Parts.IsInAlbum(d))
      {
         if (Plane.Parts[d].Shortname[0]=='B') bAllowC=bAllowH=bAllowW=true;
         if (Plane.Parts[d].Shortname[0]=='L' || Plane.Parts[d].Shortname[0]=='H') bAllowM=true;
      }

   for (d=0; d<(long)Plane.Parts.AnzEntries(); d++)
      if (Plane.Parts.IsInAlbum(d))
      {
         if (Plane.Parts[d].Shortname[0]=='B') bAllowB=false;
         if (Plane.Parts[d].Shortname[0]=='C') bAllowC=false;
         if (Plane.Parts[d].Shortname[0]=='H') bAllowH=false;
         if (Plane.Parts[d].Shortname[0]=='L') bAllowW=false;
      }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CEditor message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CEditor::OnPaint()
//--------------------------------------------------------------------------------------------
void CEditor::OnPaint()
{
   long c, d;

   if (!bHandy) SetMouseLook (CURSOR_NORMAL, 0, ROOM_EDITOR, 0);

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   GripRelation=-1;
   bool bAlsoOutline = false;
   if (PartUnderCursor!="")
   {
      double BestDist     = 100;

      GripRelation     = -1;
      GripRelationB    = -1;
      GripRelationPart = -1;
      GripAtPos        = gMousePosition-PartBms[GetPlaneBuild(PartUnderCursor).BitmapIndex].Size/SLONG(2);

      if (PartUnderCursorB!="")
         GripAtPosB = GripAtPos+XY(PartBms[GetPlaneBuild(PartUnderCursor).BitmapIndex].Size.x*3/4, -PartBms[GetPlaneBuild(PartUnderCursorB).BitmapIndex].Size.y);

      //Für alle Relations:
      for (c=0; c<sizeof(gPlanePartRelations)/sizeof(gPlanePartRelations[0]); c++)
         if (gPlanePartRelations[c].ToBuildIndex==GetPlaneBuildIndex(PartUnderCursor))
            if (Plane.Parts.IsSlotFree (gPlanePartRelations[c].Slot))
            {
               XY   GripToSpot   (-9999,-9999);
               XY   GripToSpot2d (-9999,-9999);

               //Ist die Von-Seite ein Part oder der Desktop?
               if (gPlanePartRelations[c].FromBuildIndex==-1)
               {
                  GripToSpot   =gPlanePartRelations[c].Offset3d-PartBms[GetPlaneBuild(PartUnderCursor).BitmapIndex].Size/SLONG(2);
                  GripToSpot2d =gPlanePartRelations[c].Offset2d;

                  if ((gMousePosition-GripToSpot).abs()<BestDist*2)
                  {
                     BestDist         = (gMousePosition-GripToSpot).abs();
                     GripAtPos        = GripToSpot;
                     GripAtPos2d      = GripToSpot2d;
                     GripRelation     = c;
                     GripRelationPart = -1;

                     bAlsoOutline = true;
                  }
               }
               else
               {
                  //long OtherParent = gPlanePartRelations[c].FromBuildIndex;

                  //     if (gPlaneBuilds[OtherParent].Shortname[0]=='L') OtherParent-=(5+8);
                  //else if (gPlaneBuilds[OtherParent].Shortname[0]=='R') OtherParent+=(5+8);

                  //Für alle eingebauten Planeparts:
                  
                  for (d=0; d<(long)Plane.Parts.AnzEntries(); d++)
                     if (Plane.Parts.IsInAlbum(d))
                        if (gPlanePartRelations[c].FromBuildIndex==GetPlaneBuildIndex(Plane.Parts[d].Shortname))
                        {
                           GripToSpot   = Plane.Parts[d].Pos3d+gPlanePartRelations[c].Offset3d;
                           GripToSpot2d = Plane.Parts[d].Pos2d+gPlanePartRelations[c].Offset2d;

                           if ((gMousePosition-GripToSpot-PartBms[GetPlaneBuild(PartUnderCursor).BitmapIndex].Size/SLONG(2)).abs()<BestDist)
                           {
                              BestDist         = (gMousePosition-GripToSpot-PartBms[GetPlaneBuild(PartUnderCursor).BitmapIndex].Size/SLONG(2)).abs();
                              GripAtPos        = GripToSpot;
                              GripAtPos2d      = GripToSpot2d;
                              GripRelation     = c;
                              GripRelationPart = d;

                              if (PartUnderCursorB!="")
                              {
                                 long e=d;
                                 if (Plane.Parts[e].Shortname[0]=='L' || Plane.Parts[e].Shortname[0]=='R')
                                 {
                                    for (e=0; e<(long)Plane.Parts.AnzEntries(); e++)
                                       if (Plane.Parts.IsInAlbum(e))
                                          if (d!=e && (Plane.Parts[e].Shortname[0]=='L' || Plane.Parts[e].Shortname[0]=='R'))
                                             break;
                                 }

                                 long add=1;
                                 if (Plane.Parts[d].Shortname[0]=='L' || (Plane.Parts[d].Shortname[0]=='H' && gPlanePartRelations[c].Slot[1]=='L')) add=-1;

                                 GripAtPosB    = Plane.Parts[e].Pos3d+gPlanePartRelations[c+add].Offset3d;
                                 GripAtPosB2d  = Plane.Parts[e].Pos2d+gPlanePartRelations[c+add].Offset2d;
                                 GripRelationB = c+add;
                              }

                              bAlsoOutline     = true;
                           }
                        }
               }
            }
   }

   //Die Online-Statistik:
   long weight    = Plane.CalcWeight();
   long passa     = Plane.CalcPassagiere();
   long verbrauch = Plane.CalcVerbrauch();
   long noise     = Plane.CalcNoise();
   long wartung   = Plane.CalcWartung();
   long cost      = Plane.CalcCost();
   long speed     = Plane.CalcSpeed();
   long tank      = Plane.CalcTank(true);
   long reichw    = Plane.CalcReichweite();

   //NUR TEMPORÄR:
   long verbrauch2=0;
   if (verbrauch && speed && passa)
      verbrauch2 = verbrauch*100/speed*100/passa;
   else
      verbrauch2 = 0;

   long index_b = GetPlaneBuild(bprintf("B%li", 1+sel_b)).BitmapIndex;
   long index_c = GetPlaneBuild(bprintf("C%li", 1+sel_c)).BitmapIndex;
   long index_h = GetPlaneBuild(bprintf("H%li", 1+sel_h)).BitmapIndex;
   long index_w = GetPlaneBuild(bprintf("R%li", 1+sel_w)).BitmapIndex;
   long index_m = GetPlaneBuild(bprintf("M%li", 1+sel_m)).BitmapIndex;

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      //Part tips:
      for (c=0; c<5; c++)
      {
         if (gMousePosition.IfIsWithin (  4+c*127,363,124+c*127,436))
         {
            CString part;

            if ((c==0 && bAllowB==false) || (c==1 && bAllowC==false) || (c==2 && bAllowH==false) || (c==3 && bAllowW==false) || (c==4 && bAllowM==false)) continue;

            if (c==0) part=bprintf("B%li", 1+sel_b);
            if (c==1) part=bprintf("C%li", 1+sel_c);
            if (c==2) part=bprintf("H%li", 1+sel_h);
            if (c==3) part=bprintf("R%li", 1+sel_w);
            if (c==4) part=bprintf("M%li", 1+sel_m);

            CPlaneBuild &qb = GetPlaneBuild(part);
            weight    = qb.Weight;
            passa     = qb.Passagiere;
            verbrauch = 0;
            noise     = qb.Noise;
            wartung   = qb.Wartung;
            cost      = qb.Cost;
            speed     = 0;
            tank      = 0;
            reichw    = 0;
         }
      }
   }

   CString        loudnesstext = StandardTexte.GetS (TOKEN_MISC, 8400); //sehr leise
   if (noise>0)   loudnesstext = StandardTexte.GetS (TOKEN_MISC, 8401); //leise
   if (noise>20)  loudnesstext = StandardTexte.GetS (TOKEN_MISC, 8402); //okay
   if (noise>40)  loudnesstext = StandardTexte.GetS (TOKEN_MISC, 8403); //noch okay
   if (noise>60)  loudnesstext = StandardTexte.GetS (TOKEN_MISC, 8404); //halbwegs laut
   if (noise>80)  loudnesstext = StandardTexte.GetS (TOKEN_MISC, 8405); //laut
   if (noise>100) loudnesstext = StandardTexte.GetS (TOKEN_MISC, 8406); //sehr laut

   CString wartungtext          = StandardTexte.GetS (TOKEN_MISC, 8508); //sehr gut
   if (wartung>-30) wartungtext = StandardTexte.GetS (TOKEN_MISC, 8507); //recht gut
   if (wartung>-20) wartungtext = StandardTexte.GetS (TOKEN_MISC, 8506); //gut
   if (wartung>-10) wartungtext = StandardTexte.GetS (TOKEN_MISC, 8505); //über normal
   if (wartung>=0)  wartungtext = StandardTexte.GetS (TOKEN_MISC, 8504); //normal
   if (wartung>20)  wartungtext = StandardTexte.GetS (TOKEN_MISC, 8503); //mäßig
   if (wartung>50)  wartungtext = StandardTexte.GetS (TOKEN_MISC, 8502); //schlecht
   if (wartung>80)  wartungtext = StandardTexte.GetS (TOKEN_MISC, 8501); //sehr schlecht
   if (wartung>110)  wartungtext = StandardTexte.GetS (TOKEN_MISC, 8500); //katastrophal

/*   //Die Online-Statistik:
   RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8200), weight), FontSmallBlack, TEC_FONT_LEFT, 480, 20, 640, 200);
   if (passa>0)     RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8201), passa, passa/10), FontSmallBlack, TEC_FONT_LEFT, 480, 20+15, 640, 200);
   if (verbrauch>0) RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8202), verbrauch), FontSmallBlack, TEC_FONT_LEFT, 480, 20+30, 640, 200);
   if (noise!=0)    RoomBm.PrintAt (bprintf(CString(StandardTexte.GetS (TOKEN_MISC, 8204)), loudnesstext, abs(noise)), FontSmallBlack, TEC_FONT_LEFT, 480, 20+45, 640, 200);
   RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8206), wartungtext), FontSmallBlack, TEC_FONT_LEFT, 480, 20+60, 640, 200);
   if (speed>0)     RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8207), (CString)Einheiten[EINH_KMH].bString (speed)), FontSmallBlack, TEC_FONT_LEFT, 480, 20+75, 640, 200);
   if (tank>0)      RoomBm.PrintAt (CString(StandardTexte.GetS (TOKEN_PLANE, 1008))+": "+(CString)Einheiten[EINH_L].bString (tank), FontSmallBlack, TEC_FONT_LEFT, 480, 20+75+15, 640, 200);
   if (reichw>0)    RoomBm.PrintAt (CString(StandardTexte.GetS (TOKEN_PLANE, 1001))+": "+(CString)Einheiten[EINH_KM].bString (reichw), FontSmallBlack, TEC_FONT_LEFT, 480, 20+75+30, 640, 200);
   //if (verbrauch2>0) RoomBm.PrintAt (bprintf("Verbrauch: %li l/100/100", verbrauch2), FontSmallBlack, TEC_FONT_LEFT, 480, 20+75+15, 640, 200);
   RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8203), (CString)Einheiten[EINH_DM].bString (cost)), FontSmallBlack, TEC_FONT_LEFT, 480, 20+100+15+15, 640, 200);

   CString error = Plane.GetError();
   if (error!="") RoomBm.PrintAt (error, FontNormalRed, TEC_FONT_LEFT, 480, 20+125+15+15, 580, 300);
 */
   //Das Flugzeug blitten:
   bool bCursorBlitted=false;
   bool bCursorBlittedB=false;
   for (d=0; d<(long)Plane.Parts.AnzEntries(); d++)
      if (Plane.Parts.IsInAlbum(d))
      {
         BOOL bShift = 0; //(GetAsyncKeyState (VK_SHIFT)/256)!=0;
         
         SBBM &qBm = bShift?(gEditorPlane2dBms[GetPlaneBuild(Plane.Parts[d].Shortname).BitmapIndex]):(PartBms[GetPlaneBuild(Plane.Parts[d].Shortname).BitmapIndex]);
         XY    p   = bShift?(Plane.Parts[d].Pos2d+XY(320,200)):(Plane.Parts[d].Pos3d);

         if (PartUnderCursor!="" && bCursorBlitted==false && GetPlaneBuild(PartUnderCursor).zPos+GripAtPos.y+gPlanePartRelations[GripRelation].zAdd<=GetPlaneBuild(Plane.Parts[d].Shortname).zPos+Plane.Parts[d].Pos3d.y)
         {
            RoomBm.BlitFromT (PartBms[GetPlaneBuild(PartUnderCursor).BitmapIndex], GripAtPos);
            bCursorBlitted=true;
         }
         /*if (PartUnderCursorB!="" && bCursorBlittedB==false && GetPlaneBuild(PartUnderCursorB).zPos+GripAtPosB.y+gPlanePartRelations[GripRelation].zAdd<=GetPlaneBuild(Plane.Parts[d].Shortname).zPos+Plane.Parts[d].Pos3d.y)
         {
            RoomBm.BlitFromT (PartBms[GetPlaneBuild(PartUnderCursorB).BitmapIndex], GripAtPosB);
            bCursorBlittedB=true;
         }*/

         RoomBm.BlitFromT (qBm, p);
      }

   if (Plane.Parts.GetNumUsed()>0)
   {
      for (long cx=-1; cx<=1; cx++)
         for (long cy=-1; cy<=1; cy++)
         {
            //Die Online-Statistik:
            RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8200), weight), FontSmallBlack, TEC_FONT_LEFT, 480+cx, 20+cy, 640+cx, 200+cy);
            if (passa>0)     RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8201), passa, passa/10), FontSmallBlack, TEC_FONT_LEFT, 480+cx, 20+15+cy, 640+cx, 200+cy);
            if (verbrauch>0) RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8202), verbrauch), FontSmallBlack, TEC_FONT_LEFT, 480+cx, 20+30+cy, 640+cx, 200+cy);
            if (noise!=0)    RoomBm.PrintAt (bprintf(CString(StandardTexte.GetS (TOKEN_MISC, 8204)), loudnesstext.c_str(), abs(noise)), FontSmallBlack, TEC_FONT_LEFT, 480+cx, 20+45+cy, 640+cx, 200+cy);
            RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8206), wartungtext.c_str()), FontSmallBlack, TEC_FONT_LEFT, 480+cx, 20+60+cy, 640+cx, 200+cy);
            if (speed>0)     RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8207), CString(Einheiten[EINH_KMH].bString(speed)).c_str()), FontSmallBlack, TEC_FONT_LEFT, 480+cx, 20+75+cy, 640+cx, 200+cy);
            if (tank>0)      RoomBm.PrintAt (CString(StandardTexte.GetS (TOKEN_PLANE, 1008))+": "+ CString(Einheiten[EINH_L].bString (tank)).c_str(), FontSmallBlack, TEC_FONT_LEFT, 480+cx, 20+75+15+cy, 640+cx, 200+cy);
            if (reichw>0)    RoomBm.PrintAt (CString(StandardTexte.GetS (TOKEN_PLANE, 1001))+": "+ CString(Einheiten[EINH_KM].bString (reichw)).c_str(), FontSmallBlack, TEC_FONT_LEFT, 480+cx, 20+75+30+cy, 640+cx, 200+cy);
            //if (verbrauch2>0) RoomBm.PrintAt (bprintf("Verbrauch: %li l/100/100", verbrauch2), FontSmallBlack, TEC_FONT_LEFT, 480, 20+75+15, 640, 200);
            RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8203), CString(Einheiten[EINH_DM].bString (cost)).c_str()), FontSmallBlack, TEC_FONT_LEFT, 480+cx, 20+100+15+15+cy, 640+cx, 200+cy);
         }
   }
   #define FontSmallBlack FontYellow

   //Die Online-Statistik:
   if (Plane.Parts.GetNumUsed()>0)
   {
      RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8200), weight), FontSmallBlack, TEC_FONT_LEFT, 480, 20, 640, 200);
      if (passa>0)     RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8201), passa, passa/10), FontSmallBlack, TEC_FONT_LEFT, 480, 20+15, 640, 200);
      if (verbrauch>0) RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8202), verbrauch), FontSmallBlack, TEC_FONT_LEFT, 480, 20+30, 640, 200);
      if (noise!=0)    RoomBm.PrintAt (bprintf(CString(StandardTexte.GetS (TOKEN_MISC, 8204)), loudnesstext.c_str(), abs(noise)), FontSmallBlack, TEC_FONT_LEFT, 480, 20+45, 640, 200);
      RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8206), wartungtext.c_str()), FontSmallBlack, TEC_FONT_LEFT, 480, 20+60, 640, 200);
      if (speed>0)     RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8207), CString(Einheiten[EINH_KMH].bString (speed)).c_str()), FontSmallBlack, TEC_FONT_LEFT, 480, 20+75, 640, 200);
      if (tank>0)      RoomBm.PrintAt (CString(StandardTexte.GetS (TOKEN_PLANE, 1008))+": "+ CString(Einheiten[EINH_L].bString (tank)).c_str(), FontSmallBlack, TEC_FONT_LEFT, 480, 20+75+15, 640, 200);
      if (reichw>0)    RoomBm.PrintAt (CString(StandardTexte.GetS (TOKEN_PLANE, 1001))+": "+ CString(Einheiten[EINH_KM].bString (reichw)).c_str(), FontSmallBlack, TEC_FONT_LEFT, 480, 20+75+30, 640, 200);
      //if (verbrauch2>0) RoomBm.PrintAt (bprintf("Verbrauch: %li l/100/100", verbrauch2), FontSmallBlack, TEC_FONT_LEFT, 480, 20+75+15, 640, 200);
      RoomBm.PrintAt (bprintf(StandardTexte.GetS (TOKEN_MISC, 8203), CString(Einheiten[EINH_DM].bString (cost)).c_str()), FontSmallBlack, TEC_FONT_LEFT, 480, 20+100+15+15, 640, 200);

      CString error = Plane.GetError();
      if (error!="") RoomBm.PrintAt (error, FontNormalRed, TEC_FONT_LEFT, 480, 20+125+15+15, 580, 300);
   }
   
   //BROKEN:
   if (PartUnderCursor!="" && bCursorBlitted==false)   RoomBm.BlitFromT (PartBms[GetPlaneBuild(PartUnderCursor).BitmapIndex], GripAtPos);
   //if (PartUnderCursorB!="" && bCursorBlittedB==false) RoomBm.BlitFromT (PartBms[GetPlaneBuild(PartUnderCursorB).BitmapIndex], GripAtPosB);

   if (bAlsoOutline && !IsDialogOpen() && !MenuIsOpen())
   {
      ColorFX.BlitOutline (PartBms[GetPlaneBuild(PartUnderCursor).BitmapIndex].pBitmap, RoomBm.pBitmap, GripAtPos, 0xffffff);
      if (PartUnderCursorB!="") ColorFX.BlitOutline (PartBms[GetPlaneBuild(PartUnderCursorB).BitmapIndex].pBitmap, RoomBm.pBitmap, GripAtPosB, 0xffffff);
   }

   //Die Maske um Überhänge zu verdecken:
   RoomBm.BlitFromT (MaskenBms[0], 0, 0);
   RoomBm.BlitFromT (MaskenBms[1], 0, 343);

   //Flugzeugname:
   RoomBm.PrintAt (Plane.Name, FontNormalGreen, TEC_FONT_CENTERED, 193, 4+3, 471, 25+3);
   
   //Die aktuell gewählten Parts:
   if (bAllowB) RoomBm.BlitFromT (SelPartBms[index_b],  66-SelPartBms[index_b].Size.x/2, 399-SelPartBms[index_b].Size.y/2);
   else         ColorFX.BlitTrans (SelPartBms[index_b].pBitmap, RoomBm.pBitmap, XY(66-SelPartBms[index_b].Size.x/2, 399-SelPartBms[index_b].Size.y/2), NULL, 5);
   if (bAllowC) RoomBm.BlitFromT (SelPartBms[index_c], 193-SelPartBms[index_c].Size.x/2, 399-SelPartBms[index_c].Size.y/2);
   else         ColorFX.BlitTrans (SelPartBms[index_c].pBitmap, RoomBm.pBitmap, XY(193-SelPartBms[index_c].Size.x/2, 399-SelPartBms[index_c].Size.y/2), NULL, 5);
   if (bAllowH) RoomBm.BlitFromT (SelPartBms[index_h], 320-SelPartBms[index_h].Size.x/2, 399-SelPartBms[index_h].Size.y/2);
   else         ColorFX.BlitTrans (SelPartBms[index_h].pBitmap, RoomBm.pBitmap, XY(320-SelPartBms[index_h].Size.x/2, 399-SelPartBms[index_h].Size.y/2), NULL, 5);
   if (bAllowW) RoomBm.BlitFromT (SelPartBms[index_w], 447-SelPartBms[index_w].Size.x/2, 399-SelPartBms[index_w].Size.y/2);
   else         ColorFX.BlitTrans (SelPartBms[index_w].pBitmap, RoomBm.pBitmap, XY(447-SelPartBms[index_w].Size.x/2, 399-SelPartBms[index_w].Size.y/2), NULL, 5);
   if (bAllowM) RoomBm.BlitFromT (SelPartBms[index_m], 574-SelPartBms[index_m].Size.x/2, 399-SelPartBms[index_m].Size.y/2);
   else         ColorFX.BlitTrans (SelPartBms[index_m].pBitmap, RoomBm.pBitmap, XY(574-SelPartBms[index_m].Size.x/2, 399-SelPartBms[index_m].Size.y/2), NULL, 5);
   
   if (!IsDialogOpen() && !MenuIsOpen())
   {
      //Ok, Cancel:
      if (gMousePosition.IfIsWithin (602, 192, 640, 224)) SetMouseLook (CURSOR_EXIT, 0, ROOM_EDITOR, 998);
      if (gMousePosition.IfIsWithin (602, 158, 640, 188)) SetMouseLook (CURSOR_EXIT, 0, ROOM_EDITOR, 999);

      //Delete, new:
      if (gMousePosition.IfIsWithin (0, 192, 38, 224)) SetMouseLook (CURSOR_HOT, 0, ROOM_EDITOR, 900);
      if (gMousePosition.IfIsWithin (0, 158, 38, 188)) SetMouseLook (CURSOR_HOT, 0, ROOM_EDITOR, 901);

      //Prev, Next:
      if (gMousePosition.IfIsWithin (185, 0, 212, 23)) SetMouseLook (CURSOR_HOT, 0, ROOM_EDITOR, 800);
      if (gMousePosition.IfIsWithin (438, 0, 465, 23)) SetMouseLook (CURSOR_HOT, 0, ROOM_EDITOR, 801);

      //Rename:
      if (gMousePosition.IfIsWithin (212, 0, 438, 15)) SetMouseLook (CURSOR_HOT, 0, ROOM_EDITOR, 10);

      //Select Parts:
      for (c=0; c<5; c++)
      {
         if ((c==0 && bAllowB==false) || (c==1 && bAllowC==false) || (c==2 && bAllowH==false) || (c==3 && bAllowW==false) || (c==4 && bAllowM==false)) continue;

         if (gMousePosition.IfIsWithin (  4+c*127,370, 27+c*127,426)) SetMouseLook (CURSOR_HOT, 0, ROOM_EDITOR, c*100+100);
         if (gMousePosition.IfIsWithin (101+c*127,370,124+c*127,426)) SetMouseLook (CURSOR_HOT, 0, ROOM_EDITOR, c*100+101);
         if (gMousePosition.IfIsWithin ( 27+c*127,363,101+c*127,436)) SetMouseLook (CURSOR_HOT, 0, ROOM_EDITOR, c*100+105);
         
         if (gMousePosition.IfIsWithin (  4+c*127,363,124+c*127,436))
         {
            CString part;

            if (c==0) part=bprintf("B%li", 1+sel_b);
            if (c==1) part=bprintf("C%li", 1+sel_c);
            if (c==2) part=bprintf("H%li", 1+sel_h);
            if (c==3) part=bprintf("R%li", 1+sel_w);
            if (c==4) part=bprintf("M%li", 1+sel_m);

            CPlaneBuild &qb = GetPlaneBuild(part);
            weight    = qb.Weight;
            passa     = qb.Passagiere;
            verbrauch = 0;
            noise     = qb.Noise;
            wartung   = qb.Wartung;
            cost      = qb.Cost;
            speed     = 0;
         }
      }
   }
   
   test:

   //Select Part Buttons:
   for (c=0; c<5; c++)
   {
      if ((c==0 && bAllowB==false) || (c==1 && bAllowC==false) || (c==2 && bAllowH==false) || (c==3 && bAllowW==false) || (c==4 && bAllowM==false)) continue;

      RoomBm.BlitFromT (ButtonPartLRBms[0+(MouseClickId==c*100+100)+(MouseClickId==c*100+100 && gMouseLButton)], 4+c*127, 370);
      RoomBm.BlitFromT (ButtonPartLRBms[3+(MouseClickId==c*100+101)+(MouseClickId==c*100+101 && gMouseLButton)], 101+c*127, 370);
   }

   //Cancel, Delete, New, Ok:
   RoomBm.BlitFromT (OtherButtonBms[0+(MouseClickId==998)+(MouseClickId==998 && gMouseLButton)], 602, 192);
   RoomBm.BlitFromT (OtherButtonBms[3+(MouseClickId==900)+(MouseClickId==900 && gMouseLButton)],   0, 192);
   RoomBm.BlitFromT (OtherButtonBms[6+(MouseClickId==901)+(MouseClickId==901 && gMouseLButton)],   0, 158);
   RoomBm.BlitFromT (OtherButtonBms[9+(MouseClickId==999)+(MouseClickId==999 && gMouseLButton)], 602, 158);

   //Prev, Next:
   RoomBm.BlitFromT (ButtonPlaneLRBms[0+(MouseClickId==800)+(MouseClickId==800 && gMouseLButton)], 185, 0);
   RoomBm.BlitFromT (ButtonPlaneLRBms[3+(MouseClickId==801)+(MouseClickId==801 && gMouseLButton)], 438, 0);
   
   if (!IsDialogOpen() && !MenuIsOpen())
   {
      bool bHotPartFound=false;
      for (long pass=1; pass<=2; pass++)
         for (d=(long)Plane.Parts.AnzEntries()-1; d>=0; d--)
            if (Plane.Parts.IsInAlbum(d) && (Plane.Parts[d].Shortname[0]=='M')==(pass==1))
            {
               SBBM &qBm = PartBms[GetPlaneBuild(Plane.Parts[d].Shortname).BitmapIndex];
               XY    p   = Plane.Parts[d].Pos3d;

               if (PartUnderCursor=="" && bHotPartFound==false)
                  if (gMousePosition.x>=p.x && gMousePosition.y>=p.y && gMousePosition.x<p.x+qBm.Size.x && gMousePosition.y<p.y+qBm.Size.y)
                     if (qBm.GetPixel (gMousePosition.x-p.x, gMousePosition.y-p.y)!=0)
                     {
                        ColorFX.BlitOutline (qBm.pBitmap, RoomBm.pBitmap, p, 0xffffff);
                        SetMouseLook (CURSOR_HOT, 0, ROOM_EDITOR, 10000+d);
                        bHotPartFound=true;
                     }
            }
   }
   
   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//void CEditor::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
   XY RoomPos;

   DefaultOnLButtonDown ();

   if (!ConvertMousePosition (point, &RoomPos))
   {
	   CStdRaum::OnLButtonDown(nFlags, point);
      return;
   }

   if (!PreLButtonDown (point))
   {
      if (MouseClickArea==ROOM_EDITOR)
      {
         DoLButtonWork (nFlags, point);
      }
      else CStdRaum::OnLButtonDown(nFlags, point);
   }

        if (PartUnderCursor!="" && PartUnderCursor[0]=='R') PartUnderCursorB = CString("L") + PartUnderCursor[1];
   else if (PartUnderCursor!="" && PartUnderCursor[0]=='M') PartUnderCursorB = PartUnderCursor;
   else                                                     PartUnderCursorB = "";

   UpdateButtonState ();
}

//--------------------------------------------------------------------------------------------
//void CEditor::OnLButtonDblClk(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CEditor::OnLButtonDblClk(UINT nFlags, CPoint point)
{
   XY RoomPos;

   DefaultOnLButtonDown ();

   if (!ConvertMousePosition (point, &RoomPos))
   {
	   CStdRaum::OnLButtonDblClk(nFlags, point);
      return;
   }

   if (!PreLButtonDown (point))
   {
      if (MouseClickArea==ROOM_EDITOR)
      {
         DoLButtonWork (nFlags, point);
      }
      else CStdRaum::OnLButtonDblClk(nFlags, point);
   }

        if (PartUnderCursor!="" && PartUnderCursor[0]=='R') PartUnderCursorB = CString("L") + PartUnderCursor[1];
   else if (PartUnderCursor!="" && PartUnderCursor[0]=='M') PartUnderCursorB = PartUnderCursor;
   else                                                     PartUnderCursorB = "";

   UpdateButtonState ();
}

//--------------------------------------------------------------------------------------------
// Erledigt die eigentliche Arbeit bei einem L-Click
//--------------------------------------------------------------------------------------------
void CEditor::DoLButtonWork (UINT nFlags, CPoint point)
{
   if (MouseClickId==998) Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
   if (MouseClickId==999)
   {
      //Plane.Save (FullFilename (Plane.Name+".plane", MyPlanePath));
      Plane.Save (PlaneFilename);

      Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
   }
   if (MouseClickId==10)  MenuStart (MENU_RENAMEEDITPLANE);

   if (MouseClickId==100) sel_b = (sel_b-1+NUM_PLANE_BODY)%NUM_PLANE_BODY;
   if (MouseClickId==101) sel_b = (sel_b+1+NUM_PLANE_BODY)%NUM_PLANE_BODY;
   if (MouseClickId==105) PartUnderCursor = bprintf("B%li", 1+sel_b);

   if (MouseClickId==200) sel_c = (sel_c-1+NUM_PLANE_COCKPIT)%NUM_PLANE_COCKPIT;
   if (MouseClickId==201) sel_c = (sel_c+1+NUM_PLANE_COCKPIT)%NUM_PLANE_COCKPIT;
   if (MouseClickId==205) PartUnderCursor = bprintf("C%li", 1+sel_c);

   if (MouseClickId==300) sel_h = (sel_h-1+NUM_PLANE_HECK)%NUM_PLANE_HECK;
   if (MouseClickId==301) sel_h = (sel_h+1+NUM_PLANE_HECK)%NUM_PLANE_HECK;
   if (MouseClickId==305) PartUnderCursor = bprintf("H%li", 1+sel_h);

   if (MouseClickId==400) sel_w = (sel_w-1+NUM_PLANE_LWING)%NUM_PLANE_LWING;
   if (MouseClickId==401) sel_w = (sel_w+1+NUM_PLANE_LWING)%NUM_PLANE_LWING;
   if (MouseClickId==405) PartUnderCursor = bprintf("R%li", 1+sel_w);

   if (MouseClickId==500) sel_m = (sel_m-1+NUM_PLANE_MOT)%NUM_PLANE_MOT;
   if (MouseClickId==501) sel_m = (sel_m+1+NUM_PLANE_MOT)%NUM_PLANE_MOT;
   if (MouseClickId==505) PartUnderCursor = bprintf("M%li", 1+sel_m);

   if (MouseClickId==105 || MouseClickId==205 || MouseClickId==305 || MouseClickId==405 || MouseClickId==505) 
      DragDropMode=true;

   //Delete, new:
   if (MouseClickId==900)
   {
      if (Plane.Parts.GetNumUsed()>1)
      {
         PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];
         (*(CStdRaum*)qPlayer.LocationWin).MenuStart (MENU_REQUEST, MENU_REQUEST_KILLPLANE, 77);
         MouseClickId=0;
      }
      else DeleteCurrent();
   }
   if (MouseClickId==901)
   {
      //Plane.Save (FullFilename (Plane.Name+".plane", MyPlanePath));
      Plane.Save (PlaneFilename);

      Plane.Clear();
      PlaneFilename = CreateNumeratedFreeFilename (FullFilename ("data%s.plane", MyPlanePath));
      Plane.Name    = GetFilenameFromFullFilename (PlaneFilename);
      Plane.Name    = CString(StandardTexte.GetS (TOKEN_MISC, 8210))+Plane.Name.Mid (5, Plane.Name.GetLength()-6-5);
   }

   //Prev, Next:
   if (MouseClickId==800)
   {
      Plane.Save (PlaneFilename);
      //Plane.Save (FullFilename (Plane.Name+".plane", MyPlanePath));
      Plane.Clear();

      //PlaneFilename = FullFilename (GetMatchingNext (FullFilename ("*.plane", MyPlanePath), Plane.Name+".plane", -1), MyPlanePath);
      PlaneFilename = FullFilename (GetMatchingNext (FullFilename ("*.plane", MyPlanePath), GetFilenameFromFullFilename(PlaneFilename), -1), MyPlanePath);
      Plane.Load (PlaneFilename);
   }
   if (MouseClickId==801)
   {
      Plane.Save (PlaneFilename);
      //Plane.Save (FullFilename (Plane.Name+".plane", MyPlanePath));
      Plane.Clear();

      //PlaneFilename = FullFilename (GetMatchingNext (FullFilename ("*.plane", MyPlanePath), Plane.Name+".plane", 1), MyPlanePath);
      PlaneFilename = FullFilename (GetMatchingNext (FullFilename ("*.plane", MyPlanePath), GetFilenameFromFullFilename(PlaneFilename), 1), MyPlanePath);
      Plane.Load (PlaneFilename);
   }

   if (MouseClickId>=10000)
   {
      long relnr = Plane.Parts[long(MouseClickId-10000)].ParentRelationId;
      long rel = gPlanePartRelations[relnr].Id;
      PartUnderCursor = Plane.Parts[long(MouseClickId-10000)].Shortname;
      if (PartUnderCursor[0]=='L') PartUnderCursor.SetAt (0, 'R');

      DragDropMode=-1;

      if (PartUnderCursor[0]=='B' && Plane.Parts.GetNumUsed()>1)
      {
         PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];
         (*(CStdRaum*)qPlayer.LocationWin).MenuStart (MENU_REQUEST, MENU_REQUEST_KILLPLANE);
         PartUnderCursor="";
      }
      else
      {
         Plane.Parts-=(MouseClickId-10000);

         while (1)
         {
            long c;
            for (c=0; c<(long)Plane.Parts.AnzEntries(); c++)
               if (Plane.Parts.IsInAlbum(c))
                  //if ((Plane.Parts[c].ParentShortname!="" && !Plane.Parts.IsShortnameInAlbum(Plane.Parts[c].ParentShortname)) || (Plane.Parts[c].ParentShortname!="" && PartUnderCursor[0]=='R' && ((gPlanePartRelations[Plane.Parts[c].ParentRelationId].Id==rel+200 && rel>=400 && rel<600) || (gPlanePartRelations[Plane.Parts[c].ParentRelationId].Id==rel-200 && rel>=600 && rel<800))))
                  if ((Plane.Parts[c].ParentShortname!="" && !Plane.Parts.IsShortnameInAlbum(Plane.Parts[c].ParentShortname)) || (Plane.Parts[c].ParentShortname!="" && PartUnderCursor[0]=='R' && (gPlanePartRelations[Plane.Parts[c].ParentRelationId].Id==rel+200 && rel>=400 && rel<600) || (gPlanePartRelations[Plane.Parts[c].ParentRelationId].Id==rel-200 && rel>=600 && rel<800) || (PartUnderCursor[0]=='M' && rel>=700 && rel<=1400 && abs(gPlanePartRelations[Plane.Parts[c].ParentRelationId].Id-rel)==10 && abs(relnr-Plane.Parts[c].ParentRelationId)==1)))
                  {
                     Plane.Parts-=c;
                     break;
                  }

            if (c>=(long)Plane.Parts.AnzEntries())
               break;
         }
      }
   }

   CheckUnusablePart ((MouseClickId%100)==0?(-1):(1));

   if ((MouseClickId==100 || MouseClickId==101) && PartUnderCursor.Left(1)=="B") PartUnderCursor = bprintf("B%li", 1+sel_b);
   if ((MouseClickId==200 || MouseClickId==201) && PartUnderCursor.Left(1)=="C") PartUnderCursor = bprintf("C%li", 1+sel_c);
   if ((MouseClickId==300 || MouseClickId==301) && PartUnderCursor.Left(1)=="H") PartUnderCursor = bprintf("H%li", 1+sel_h);
   if ((MouseClickId==400 || MouseClickId==401) && PartUnderCursor.Left(1)=="R") PartUnderCursor = bprintf("R%li", 1+sel_w);
   if ((MouseClickId==500 || MouseClickId==501) && PartUnderCursor.Left(1)=="M") PartUnderCursor = bprintf("M%li", 1+sel_m);
}

//--------------------------------------------------------------------------------------------
//OnLButtonUp(UINT nFlags, CPoint point) 
//--------------------------------------------------------------------------------------------
void CEditor::OnLButtonUp(UINT, CPoint point) 
{
   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (GripRelation!=-1 && DragDropMode!=-1)
      {
         ULONG Id = Plane.Parts.GetUniqueId();

         Plane.Parts += Id;
         Plane.Parts[Id].Pos2d            = GripAtPos2d;
         Plane.Parts[Id].Pos3d            = GripAtPos;
         Plane.Parts[Id].Shortname        = PartUnderCursor;
         Plane.Parts[Id].ParentShortname  = (GripRelationPart==-1)?"":Plane.Parts[GripRelationPart].Shortname;
         Plane.Parts[Id].ParentRelationId = GripRelation;

         if (PartUnderCursor.Left(1)=="B" || PartUnderCursor.Left(1)=="C" || PartUnderCursor.Left(1)=="H" || PartUnderCursor.Left(1)=="L" || PartUnderCursor.Left(1)=="R" || PartUnderCursor.Left(1)=="W") PartUnderCursor="";

         if (GripRelationB!=-1)
         {
            ULONG Id = Plane.Parts.GetUniqueId();

            Plane.Parts += Id;
            Plane.Parts[Id].Pos2d            = GripAtPosB2d;
            Plane.Parts[Id].Pos3d            = GripAtPosB;
            Plane.Parts[Id].Shortname        = PartUnderCursorB;
            Plane.Parts[Id].ParentShortname  = Plane.Parts[GripRelationPart].Shortname;
            Plane.Parts[Id].ParentRelationId = GripRelationB;

            Plane.Parts.Sort();

            if (PartUnderCursor.Left(1)=="B" || PartUnderCursor.Left(1)=="C" || PartUnderCursor.Left(1)=="H") PartUnderCursor="";
         }

         Plane.Parts.Sort();
         UpdateButtonState ();

         if (DragDropMode) PartUnderCursor=PartUnderCursorB="";
      }
   }

   DragDropMode=false;

   CheckUnusablePart (1);

   CStdRaum::OnLButtonUp(1, CPoint(1,1));
}

//--------------------------------------------------------------------------------------------
// Testet ob ein aktuell gewähltes Teil da gar nicht dran paßt:
//--------------------------------------------------------------------------------------------
void CEditor::CheckUnusablePart(long iDirection)
{
   if (bAllowW)
   {
again_w:
      bool bad=false;

      if (Plane.IstPartVorhanden ("B1") && (sel_w==0 || sel_w==1 || sel_w==2 || sel_w==5)) bad=true;
      if (Plane.IstPartVorhanden ("B2") && (sel_w==4)) bad=true;
      if (Plane.IstPartVorhanden ("B3") && (sel_w==2 || sel_w==5)) bad=true;
      if (Plane.IstPartVorhanden ("B4") && (sel_w==0 || sel_w==2 || sel_w==5)) bad=true;
      if (Plane.IstPartVorhanden ("B5") && (sel_w==5)) bad=true;

      if (bad)
      {
         sel_w=(sel_w+iDirection+NUM_PLANE_LWING)%NUM_PLANE_LWING;
         goto again_w;
      }
   }
}

//--------------------------------------------------------------------------------------------
// Löscht das aktuelle Flugzeug:
//--------------------------------------------------------------------------------------------
void CEditor::DeleteCurrent(void)
{
   //try { std::remove (FullFilename (Plane.Name+".plane", MyPlanePath)); }
   try { std::remove (PlaneFilename); }
   catch (...) {}

   Plane.Clear();
   CString fn = FullFilename (GetMatchingNext (FullFilename ("*.plane", MyPlanePath), GetFilenameFromFullFilename(PlaneFilename), -1), MyPlanePath);
   if (fn!="" && fn.Right(1)!="\\")
   {
      Plane.Load (fn);
      PlaneFilename = fn;
   }
   else
      Plane.Name = StandardTexte.GetS (TOKEN_MISC, 8210);
}

//--------------------------------------------------------------------------------------------
//void CEditor::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CEditor::OnRButtonDown(UINT nFlags, CPoint point)
{
   DefaultOnRButtonDown ();

   //Außerhalb geklickt? Dann Default-Handler!
   if (point.x<WinP1.x || point.y<WinP1.y || point.x>WinP2.x || point.y>WinP2.y)
   {
      return;
   }
   else
   {
      if (MenuIsOpen())
      {
         MenuRightClick (point);
      }
      else
      {
         if (PartUnderCursor!="")
         {
            PartUnderCursor  = "";
            PartUnderCursorB = "";
            return;
         }
         else if (MouseClickId>=10000)
         {
            long relnr = Plane.Parts[long(MouseClickId-10000)].ParentRelationId;
            long rel   = gPlanePartRelations[relnr].Id;
            PartUnderCursor = Plane.Parts[long(MouseClickId-10000)].Shortname;
            if (PartUnderCursor[0]=='L') PartUnderCursor.SetAt (0, 'R');

            if (PartUnderCursor[0]=='B' && Plane.Parts.GetNumUsed()>1)
            {
               PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];
               (*(CStdRaum*)qPlayer.LocationWin).MenuStart (MENU_REQUEST, MENU_REQUEST_KILLPLANE);
               PartUnderCursor="";
               return;
            }
            else
            {
               Plane.Parts-=(MouseClickId-10000);

               while (1)
               {
                  long c;
                  for (c=0; c<(long)Plane.Parts.AnzEntries(); c++)
                     if (Plane.Parts.IsInAlbum(c))
                        if ((Plane.Parts[c].ParentShortname!="" && !Plane.Parts.IsShortnameInAlbum(Plane.Parts[c].ParentShortname)) || (Plane.Parts[c].ParentShortname!="" && PartUnderCursor[0]=='R' && (gPlanePartRelations[Plane.Parts[c].ParentRelationId].Id==rel+200 && rel>=400 && rel<600) || (gPlanePartRelations[Plane.Parts[c].ParentRelationId].Id==rel-200 && rel>=600 && rel<800) || (PartUnderCursor[0]=='M' && rel>=700 && rel<=1400 && abs(gPlanePartRelations[Plane.Parts[c].ParentRelationId].Id-rel)==10 && abs(relnr-Plane.Parts[c].ParentRelationId)==1)))
                        {
                           Plane.Parts-=c;
                           break;
                        }

                  if (c>=(long)Plane.Parts.AnzEntries())
                     break;
               }
            }

            PartUnderCursor="";
            UpdateButtonState();
         }
         else
         {
            long c, MouseClickId=0;

            for (c=0; c<5; c++)
               if (gMousePosition.IfIsWithin ( 27+c*127,363,101+c*127,436)) 
                  MouseClickId=c*100+105;

            if (MouseClickId==105 && Plane.Parts.GetNumUsed()>1)
            {
               PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];
               (*(CStdRaum*)qPlayer.LocationWin).MenuStart (MENU_REQUEST, MENU_REQUEST_KILLPLANE);
               return;
            }
            
            while (1)
            {
               for (long c=0; c<(long)Plane.Parts.AnzEntries(); c++)
                  if (Plane.Parts.IsInAlbum(c))
                     if (Plane.Parts[c].Shortname!="")
                     {
                        char typ=Plane.Parts[c].Shortname[0];

                        if ((MouseClickId==105 && typ=='B') || (MouseClickId==205 && typ=='C') || (MouseClickId==305 && typ=='H') || (MouseClickId==405 && (typ=='R' || typ=='L')) || (MouseClickId==505 && typ=='M'))
                        {
                           Plane.Parts-=c;
                           break;
                        }
                        else if (Plane.Parts[c].ParentShortname!="" && !Plane.Parts.IsShortnameInAlbum(Plane.Parts[c].ParentShortname))
                        {
                           Plane.Parts-=c;
                           break;
                        }
                     }

               if (c>=(long)Plane.Parts.AnzEntries())
                  break;
            }

            UpdateButtonState();
         }


         /*if (!IsDialogOpen() && point.y<440)
            Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();*/

         CStdRaum::OnRButtonDown(nFlags, point);
      }
   }
}

//--------------------------------------------------------------------------------------------
// CPlaneParts
//--------------------------------------------------------------------------------------------
// Sucht einen Id eines Planeparts anhand seines Shortnames raus:
//--------------------------------------------------------------------------------------------
ULONG CPlaneParts::IdFrom (CString ShortName)
{
   SLONG c;

   for (c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum(c) && stricmp (ShortName, PlaneParts[c].Shortname)==0)
         return (GetIdFromIndex(c));

   TeakLibW_Exception (FNL, ExcNever);
   return (0);
}

//--------------------------------------------------------------------------------------------
// Gibt an, ob dieses Part im Flugzeug ist:
//--------------------------------------------------------------------------------------------
bool CPlaneParts::IsShortnameInAlbum (CString ShortName)
{
   SLONG c;

   for (c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum(c) && stricmp (ShortName, PlaneParts[c].Shortname)==0)
         return (true);

   return (false);
}

//--------------------------------------------------------------------------------------------
// Gibt true zurück, falls der Slot noch von keinem Part belegt ist:
//--------------------------------------------------------------------------------------------
bool CPlaneParts::IsSlotFree (CString Slotname)
{
   SLONG c, d;

   for (c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum(c))
      {
         CString SlotsUsed = gPlanePartRelations[(*this)[c].ParentRelationId].RulesOutSlots;

         for (d=0; d<SlotsUsed.GetLength(); d+=2)
            if (*(WORD*)(((LPCTSTR)SlotsUsed)+d)==*(WORD*)(LPCTSTR)Slotname)
               return (false);
      }

   return (true);
}

//--------------------------------------------------------------------------------------------
// Sortiert die Parts nach der Z-Position
//--------------------------------------------------------------------------------------------
void CPlaneParts::Sort (void)
{
   SLONG c;

   for (c=0; c<long(AnzEntries()-1); c++)
      if ((!IsInAlbum(c) && IsInAlbum(c+1)) || (IsInAlbum(c) && IsInAlbum(c+1) && GetPlaneBuild((*this)[c].Shortname).zPos+(*this)[c].Pos3d.y+gPlanePartRelations[(*this)[c].ParentRelationId].zAdd > GetPlaneBuild((*this)[SLONG(c+1)].Shortname).zPos+(*this)[SLONG(c+1)].Pos3d.y+gPlanePartRelations[(*this)[SLONG(c+1)].ParentRelationId].zAdd))
      {
         Swap (c, c+1);
         c-=2; if (c<-1) c=-1;
      }
}

//--------------------------------------------------------------------------------------------
// Speichert ein CPlaneParts-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CPlaneParts &pp)
{
   File << pp.PlaneParts;
   File << *((ALBUM<CPlaneParts>*)&pp);

   return (File);
}

//--------------------------------------------------------------------------------------------
// Lädt ein CPlaneParts-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CPlaneParts &pp)
{
   File >> pp.PlaneParts;
   File >> *((ALBUM<CPlaneParts>*)&pp);

   return (File);
}

//--------------------------------------------------------------------------------------------
// CPlanePart
//--------------------------------------------------------------------------------------------
// Gibt die Bitmap zurück (via das PlaneBuild Array) was dieses Part repräsentiert
//--------------------------------------------------------------------------------------------
SBBM &CPlanePart::GetBm (SBBMS &PartBms)
{
   for (long c=0; c<(sizeof(gPlaneBuilds)/sizeof(gPlaneBuilds[0])); c++)
      if (gPlaneBuilds[c].Shortname==Shortname)
         return (PartBms[gPlaneBuilds[c].BitmapIndex]);

   TeakLibW_Exception (FNL, ExcNever);
   return (*(SBBM*)NULL);
}

//--------------------------------------------------------------------------------------------
// Speichert ein CPlanePart-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CPlanePart &pp)
{
   File << pp.Pos2d << pp.Pos3d << pp.Shortname << pp.ParentShortname << pp.ParentRelationId;

   return (File);
}

//--------------------------------------------------------------------------------------------
// Lädt ein CPlanePart-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CPlanePart &pp)
{
   File >> pp.Pos2d >> pp.Pos3d >> pp.Shortname >> pp.ParentShortname >> pp.ParentRelationId;

   return (File);
}

//--------------------------------------------------------------------------------------------
// CXPlane::
//--------------------------------------------------------------------------------------------
// Löscht ein altes Flugzeug
//--------------------------------------------------------------------------------------------
void CXPlane::Clear (void)
{
   Parts.PlaneParts.ReSize (0);
   Parts.IsInAlbum(SLONG(0x01000000));
   Parts.PlaneParts.ReSize (100);
   Parts.IsInAlbum(SLONG(0x01000000));
}

//--------------------------------------------------------------------------------------------
// Berechnet die Kosten für ein Flugzeug:
//--------------------------------------------------------------------------------------------
long CXPlane::CalcCost (void)
{
   long cost=0;

   for (long c=0; c<(SLONG)Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c) && Parts[c].Shortname!="")
         cost+=GetPlaneBuild(Parts[c].Shortname).Cost;

   return (cost);
}

//--------------------------------------------------------------------------------------------
// Berechnet die Anzahl der Passagiere die reinpassen:
//--------------------------------------------------------------------------------------------
long CXPlane::CalcPassagiere (void)
{
   long passagiere=0;

   for (long c=0; c<(SLONG)Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c))
         passagiere+=GetPlaneBuild(Parts[c].Shortname).Passagiere;

   return (passagiere);
}

//--------------------------------------------------------------------------------------------
// Berechnet die Reichweite des Flugzeuges:
//--------------------------------------------------------------------------------------------
long CXPlane::CalcReichweite (void)
{
   long Reichweite=0;

   if (CalcVerbrauch()>0)
      Reichweite = CalcTank()/CalcVerbrauch()*CalcSpeed();

   return (Reichweite);
}

//--------------------------------------------------------------------------------------------
// Berechnet das benötigte Flugpersonal:
//--------------------------------------------------------------------------------------------
long CXPlane::CalcPiloten (void)
{
   long c, piloten=0;

   for (c=0; c<Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c))
      {
         for (long pass=1; pass<=3; pass++)
         {
            long note=0;
            if (pass==1) note=gPlanePartRelations[Parts[c].ParentRelationId].Note1;
            if (pass==2) note=gPlanePartRelations[Parts[c].ParentRelationId].Note2;
            if (pass==3) note=gPlanePartRelations[Parts[c].ParentRelationId].Note3;

            switch (note)
            {
               case NOTE_PILOT1: piloten+=1; break;
               case NOTE_PILOT2: piloten+=2; break;
               case NOTE_PILOT3: piloten+=3; break;
               case NOTE_PILOT4: piloten+=4; break;
            }
         }
      }

   return (std::max(1l,piloten));
}

//--------------------------------------------------------------------------------------------
// Berechnet das benötigte Flugpersonal:
//--------------------------------------------------------------------------------------------
long CXPlane::CalcBegleiter (void)
{
   long c, begleiter=0;

   for (c=0; c<Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c))
      {
         for (long pass=1; pass<=3; pass++)
         {
            long note=0;
            if (pass==1) note=gPlanePartRelations[Parts[c].ParentRelationId].Note1;
            if (pass==2) note=gPlanePartRelations[Parts[c].ParentRelationId].Note2;
            if (pass==3) note=gPlanePartRelations[Parts[c].ParentRelationId].Note3;

            switch (note)
            {
               case NOTE_BEGLEITER4:  begleiter+=4; break;
               case NOTE_BEGLEITER6:  begleiter+=6; break;
               case NOTE_BEGLEITER8:  begleiter+=8; break;
               case NOTE_BEGLEITER10: begleiter+=10; break;
            }
         }
      }

   return (std::max(1l,begleiter));
}

//--------------------------------------------------------------------------------------------
// Berechnet die Tankgröße des Flugzeuges:
//--------------------------------------------------------------------------------------------
long CXPlane::CalcTank (bool bFaked)
{
   long tank=0;

   for (long c=0; c<(SLONG)Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c) && (Parts[c].Shortname[0]=='L' || Parts[c].Shortname[0]=='R'))
         tank+=GetPlaneBuild(Parts[c].Shortname).Weight;
   tank*=7;

   if (bFaked) return (tank);

   //Länger als 22 Stunden unterwegs?
   long Verbrauch=CalcVerbrauch();
   if (Verbrauch>0 && tank>0 && tank/Verbrauch>22)
      tank=22*Verbrauch;

   return (tank);
}

//--------------------------------------------------------------------------------------------
// Berechnet den Verbrauch des Flugzeugs:
//--------------------------------------------------------------------------------------------
long CXPlane::CalcVerbrauch (void)
{
   long c, verbrauch=0;

   for (c=0; c<(SLONG)Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c))
         verbrauch+=GetPlaneBuild(Parts[c].Shortname).Verbrauch;

   for (c=0; c<(SLONG)Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c))
      {
         for (long pass=1; pass<=3; pass++)
         {
            long note=0;
            if (pass==1) note=gPlanePartRelations[Parts[c].ParentRelationId].Note1;
            if (pass==2) note=gPlanePartRelations[Parts[c].ParentRelationId].Note2;
            if (pass==3) note=gPlanePartRelations[Parts[c].ParentRelationId].Note3;

            switch (note)
            {
               case NOTE_VERBRAUCH:   verbrauch+=2500; break;
               case NOTE_VERBRAUCHXL: verbrauch+=5000; break;
            }
         }
      }

   /*if (verbrauch>0)
   {
      //Länger als 22 Stunden unterwegs?
      if (CalcTank()/verbrauch>22)
         verbrauch=CalcTank()/22;
   }*/

   return (verbrauch);
}

//--------------------------------------------------------------------------------------------
// Berechnet das Gewicht für ein Flugzeug:
//--------------------------------------------------------------------------------------------
long CXPlane::CalcWeight (void)
{
   long weight=0;

   for (long c=0; c<(SLONG)Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c))
         weight+=GetPlaneBuild(Parts[c].Shortname).Weight;

   return (weight);
}

//--------------------------------------------------------------------------------------------
// Berechnet die Kraft der Triebwerke für ein Flugzeug:
//--------------------------------------------------------------------------------------------
long CXPlane::CalcPower (void)
{
   long power=0;

   for (long c=0; c<(SLONG)Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c))
         power+=GetPlaneBuild(Parts[c].Shortname).Power;

   return (power);
}

//--------------------------------------------------------------------------------------------
// Berechnet die Kraft der Triebwerke für ein Flugzeug:
//--------------------------------------------------------------------------------------------
long CXPlane::CalcNoise (void)
{
   long c, noise=0;

   for (c=0; c<(SLONG)Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c))
         noise+=GetPlaneBuild(Parts[c].Shortname).Noise;

   for (c=0; c<(SLONG)Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c))
         noise+=gPlanePartRelations[Parts[c].ParentRelationId].Noise;

   return (noise);
}

//--------------------------------------------------------------------------------------------
// Berechnet die Wartungsintensität:
//--------------------------------------------------------------------------------------------
long CXPlane::CalcWartung (void)
{
   long c, wartung=0;

   for (c=0; c<(SLONG)Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c))
         wartung+=GetPlaneBuild(Parts[c].Shortname).Wartung;

   for (c=0; c<(SLONG)Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c))
      {
         for (long pass=1; pass<=3; pass++)
         {
            long note=0;
            if (pass==1) note=gPlanePartRelations[Parts[c].ParentRelationId].Note1;
            if (pass==2) note=gPlanePartRelations[Parts[c].ParentRelationId].Note2;
            if (pass==3) note=gPlanePartRelations[Parts[c].ParentRelationId].Note3;

            switch (note)
            {
               case NOTE_KAPUTT:   wartung+=10; break;
               case NOTE_KAPUTTXL: wartung+=20; break;
            }
         }
      }

   return (wartung);
}

//--------------------------------------------------------------------------------------------
// Berechnet die Wartungsintensität:
//--------------------------------------------------------------------------------------------
long CXPlane::CalcSpeed (void)
{
   long c, speed=0;

   //Power 6000...50000 wird zu kmh 270..1000
   speed=(CalcPower()-6000)*(1000-270)/(50000-6000)+270;
   if (CalcPower()==0) speed=0;

   for (c=0; c<(SLONG)Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c))
      {
         for (long pass=1; pass<=3; pass++)
         {
            long note=0;
            if (pass==1) note=gPlanePartRelations[Parts[c].ParentRelationId].Note1;
            if (pass==2) note=gPlanePartRelations[Parts[c].ParentRelationId].Note2;
            if (pass==3) note=gPlanePartRelations[Parts[c].ParentRelationId].Note3;

            switch (note)
            {
               case NOTE_SPEED300: if (speed>300) speed=(speed*2+300)/3; break;
               case NOTE_SPEED400: if (speed>400) speed=(speed*2+400)/3; break;
               case NOTE_SPEED500: if (speed>500) speed=(speed*2+500)/3; break;
               case NOTE_SPEED600: if (speed>600) speed=(speed*2+600)/3; break;
               case NOTE_SPEED700: if (speed>700) speed=(speed*2+700)/3; break;
               case NOTE_SPEED800: if (speed>800) speed=(speed*2+800)/3; break;
            }
         }
      }

   return (speed);
}

//--------------------------------------------------------------------------------------------
// Gibtr ggf. true zurück
//--------------------------------------------------------------------------------------------
bool CXPlane::IstPartVorhanden (CString Shortname, bool bOnlyThisType)
{
   long c;

   if (bOnlyThisType==false)
   {
      for (c=0; c<(SLONG)Parts.AnzEntries(); c++)
         if (Parts.IsInAlbum(c) && (Parts[c].Shortname==Shortname || (Parts[c].Shortname[0]==Shortname[0] && Shortname[1]=='*')))
            return (true);

      return (false);
   }
   else
   {
      for (c=0; c<(SLONG)Parts.AnzEntries(); c++)
         if (Parts.IsInAlbum(c) && Parts[c].Shortname!=Shortname && Parts[c].Shortname[0]==Shortname[0])
            return (false);

      return (IstPartVorhanden (Shortname));
   }
}

//--------------------------------------------------------------------------------------------
// Kann dieses Flugzeug gebaut werden?
//--------------------------------------------------------------------------------------------
bool CXPlane::IsBuildable (void)
{
   if (!IstPartVorhanden ("B*")) return (false);
   if (!IstPartVorhanden ("C*")) return (false);
   if (!IstPartVorhanden ("H*")) return (false);
   if (!IstPartVorhanden ("R*")) return (false);
   if (!IstPartVorhanden ("M*")) return (false);

   if (GetError()!="") return(false);

   return (true);
}

//--------------------------------------------------------------------------------------------
// Gibt eine Fehlerbeschreibung zurück:
//--------------------------------------------------------------------------------------------
CString CXPlane::GetError (void)
{
   long c, d;

   if (!IstPartVorhanden ("B*")) return ("");
   if (!IstPartVorhanden ("C*")) return ("");
   if (!IstPartVorhanden ("H*")) return ("");
   if (!IstPartVorhanden ("R*")) return ("");
   if (!IstPartVorhanden ("M*")) return ("");

   //Symetrisch 1/2?
   for (c=0; c<(SLONG)Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c) && Parts[c].Shortname[0]=='M' && gPlaneBuilds[gPlanePartRelations[Parts[c].ParentRelationId].FromBuildIndex].Shortname[0]=='R')
      {
         for (d=0; d<(SLONG)Parts.AnzEntries(); d++)
            if (Parts.IsInAlbum(d) && Parts[d].Shortname[0]=='M' && gPlaneBuilds[gPlanePartRelations[Parts[d].ParentRelationId].FromBuildIndex].Shortname[0]=='L')
               if (gPlanePartRelations[Parts[c].ParentRelationId].Id+10 == gPlanePartRelations[Parts[d].ParentRelationId].Id)
                  break;

         if (d==(SLONG)Parts.AnzEntries())
            return (StandardTexte.GetS (TOKEN_MISC, 8300));
      }

   //Symetrisch 2/2?
   for (c=0; c<(SLONG)Parts.AnzEntries(); c++)
      if (Parts.IsInAlbum(c) && Parts[c].Shortname[0]=='M' && gPlaneBuilds[gPlanePartRelations[Parts[c].ParentRelationId].FromBuildIndex].Shortname[0]=='L')
      {
         for (d=0; d<(SLONG)Parts.AnzEntries(); d++)
            if (Parts.IsInAlbum(d) && Parts[d].Shortname[0]=='M' && gPlaneBuilds[gPlanePartRelations[Parts[d].ParentRelationId].FromBuildIndex].Shortname[0]=='R')
               if (gPlanePartRelations[Parts[c].ParentRelationId].Id-10 == gPlanePartRelations[Parts[d].ParentRelationId].Id)
                  break;

         if (d==(SLONG)Parts.AnzEntries())
            return (StandardTexte.GetS (TOKEN_MISC, 8300));
      }

   //Triebwerke kräftig genug?
   if (CalcPower()*4<CalcWeight())
      return (StandardTexte.GetS (TOKEN_MISC, 8301));

   //Tragflächen groß genug?
   /*if (IstPartVorhanden ("R5"))
      if (IstPartVorhanden ("B2") || IstPartVorhanden ("B4") || IstPartVorhanden ("B5"))
         return (StandardTexte.GetS (TOKEN_MISC, 8302));

   if (IstPartVorhanden ("R4") && IstPartVorhanden ("B2"))
      return (StandardTexte.GetS (TOKEN_MISC, 8302));*/

   return ("");
}
 
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void CXPlane::Load (CString Filename)
{
   TEAKFILE InputFile (Filename, TEAKFILE_READ);

   if (InputFile.GetFileLength()==0)
      Clear();
   else
      InputFile >> (*this);
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void CXPlane::Save (CString Filename)
{
   TEAKFILE OutputFile (Filename, TEAKFILE_WRITE);

   OutputFile << (*this);
}

//--------------------------------------------------------------------------------------------
// Speichert ein CXPlane-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CXPlane &p)
{
   CXPlane *pp = (CXPlane *)&p;

   DWORD   dwSize = sizeof(DWORD)+sizeof(long)*6 + strlen(p.Name);
   long    lCost        = pp->CalcCost();
   long    lWeight      = pp->CalcWeight();
   long    lConsumption = pp->CalcVerbrauch();
   long    lNoise       = pp->CalcNoise();
   long    lReliability = pp->CalcVerbrauch();
   long    lSpeed       = pp->CalcSpeed();
   long    lReichweite  = pp->CalcReichweite();

   File << dwSize << lCost << lWeight << lConsumption << lNoise << lReliability << lSpeed << lReichweite;
   File.Write ((UBYTE*)(LPCTSTR)(p.Name), strlen(p.Name)+1);

   File << p.Name << p.Cost << p.Parts;

   return (File);
}

//--------------------------------------------------------------------------------------------
// Lädt ein CXPlane-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CXPlane &p)
{
   DWORD   dwSize;
   long    lCost;
   long    lWeight;
   long    lConsumption;
   long    lNoise;
   long    lReliability;
   long    lSpeed;

   char Dummy[8192];

   File >> dwSize >> lCost >> lWeight >> lConsumption >> lNoise >> lReliability >> lSpeed;
   dwSize -= sizeof(DWORD)+sizeof(long)*5;
   File.Read ((UBYTE*)Dummy, dwSize+1);

   File >> p.Name;
   
   File >> p.Cost >> p.Parts;

   return (File);
}

//--------------------------------------------------------------------------------------------
// ::
//--------------------------------------------------------------------------------------------
// Gibt das passende Build zum Shortname zurück:
//--------------------------------------------------------------------------------------------
long GetPlaneBuildIndex (CString Shortname)
{
   for (long c=0; c<(sizeof(gPlaneBuilds)/sizeof(gPlaneBuilds[0])); c++)
      if (gPlaneBuilds[c].Shortname==Shortname)
         return (c);

   TeakLibW_Exception (FNL, ExcNever);
   return (-1);
}

//--------------------------------------------------------------------------------------------
// Gibt das passende Build zum Shortname zurück:
//--------------------------------------------------------------------------------------------
CPlaneBuild &GetPlaneBuild (CString Shortname)
{
   for (long c=0; c<(sizeof(gPlaneBuilds)/sizeof(gPlaneBuilds[0])); c++)
      if (gPlaneBuilds[c].Shortname==Shortname)
         return (gPlaneBuilds[c]);

   TeakLibW_Exception (FNL, ExcNever);
   return (*(CPlaneBuild*)NULL);
}

//--------------------------------------------------------------------------------------------
//Blittet das Flugzeug (Footpos):
//--------------------------------------------------------------------------------------------
void CXPlane::BlitPlaneAt (SBPRIMARYBM &TargetBm, SLONG Size, XY Pos, SLONG OwningPlayer)
{
   Parts.Repair (Parts.PlaneParts);

   switch (Size)
   {
      //Auf Runway:
      case 1:
         TargetBm.BlitFromT (gUniversalPlaneBms[OwningPlayer], Pos.x-gUniversalPlaneBms[OwningPlayer].Size.x/2, Pos.y-gUniversalPlaneBms[OwningPlayer].Size.y);
         break;

      //Hinter Glas:
      case 2:
         {
            Parts.IsInAlbum(SLONG(0x01000000));
            for (long d=0; d<(long)Parts.AnzEntries(); d++)
               if (Parts.IsInAlbum(d))
               {
                  SBBM &qBm = gEditorPlane2dBms[GetPlaneBuild(Parts[d].Shortname).BitmapIndex];
                  XY    p   = Parts[d].Pos2d;

                  TargetBm.BlitFromT (qBm, Pos+p);
               }
         }
         break;
   }
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void CXPlane::operator = (const CXPlane &plane)
{
   TEAKFILE f;

   if (plane.Parts.PlaneParts.AnzEntries()>0)
   {
      f.Announce(30000);

      f << plane;
      f.MemPointer = 0;

      if(this->Cost != -1){
          Clear();

          Parts.Repair (Parts.PlaneParts);
      }

      Parts.PlaneParts.ReSize (100);
      f >> (*this);
   }
}

//--------------------------------------------------------------------------------------------
// Zerlegt das Ganze anhand von Semikolons:
//--------------------------------------------------------------------------------------------
void ParseTokens (char *String, char *tokens[], long nTokens)
{
   long c, n=0;

   for (c=0; c<nTokens; c++) tokens[c]=NULL;

   tokens[n]=String;
   while (*String!=0)
   {
      if (*String==';' && n<nTokens-1)
      {
         *String=0;
         n++;

         tokens[n]=String+1;
      }

      String++;
   }
}

//--------------------------------------------------------------------------------------------
// Konvertiert die Daten aus dem String
//--------------------------------------------------------------------------------------------
void CPlaneBuild::FromString (CString str)
{
   char *tokens[9];

   ParseTokens ((char*)(LPCTSTR)str, tokens, 9);

   Cost       = atol(tokens[2]);
   Weight     = atol(tokens[3]);
   Power      = atol(tokens[4]);
   Noise      = atol(tokens[5]);
   Wartung    = atol(tokens[6]);
   Passagiere = atol(tokens[7]);
   Verbrauch  = atol(tokens[8]);
}

//--------------------------------------------------------------------------------------------
// Konvertiert die Daten in einen String
//--------------------------------------------------------------------------------------------
CString CPlaneBuild::ToString (void)
{
   return (bprintf ("%li;%s;%li;%li;%li;%li;%li;%li;%li", Id, Shortname, Cost, Weight, Power, Noise, Wartung, Passagiere, Verbrauch));
}

//--------------------------------------------------------------------------------------------
// Konvertiert die Daten aus dem String
//--------------------------------------------------------------------------------------------
void CPlanePartRelation::FromString (CString str)
{
   char *tokens[11];

   ParseTokens ((char*)(LPCTSTR)str, tokens, 11);

   Offset2d.x = atol(tokens[3]);
   Offset2d.y = atol(tokens[4]);
   Offset3d.x = atol(tokens[5]);
   Offset3d.y = atol(tokens[6]);
   Note1      = atol(tokens[7]);
   Note2      = atol(tokens[8]);
   Note3      = atol(tokens[9]);
   Noise      = atol(tokens[10]);
}

//--------------------------------------------------------------------------------------------
// Konvertiert die Daten in einen String
//--------------------------------------------------------------------------------------------
CString CPlanePartRelation::ToString (void)
{
   LPCTSTR n1="-";
   LPCTSTR n2="-";

   if (FromBuildIndex!=-1) n1=gPlaneBuilds[FromBuildIndex].Shortname;
   if (FromBuildIndex!=-1) n2=gPlaneBuilds[ToBuildIndex].Shortname;

   return (bprintf ("%li;%s;%s;%li;%li;%li;%li;%li;%li;%li;%li", Id, n1, n2, Offset2d.x, Offset2d.y, Offset3d.x, Offset3d.y, Note1, Note2, Note3, Noise));
}
