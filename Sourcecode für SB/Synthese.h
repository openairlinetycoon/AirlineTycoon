//============================================================================================
// Synthese.h : Die Sprachsynthese!
//============================================================================================

void SynthesizeNumber (SBFX *TargetFx, CString Talker, SLONG Number, BOOL Waehrung);
void FlushTalkers (void);

//Die Stellung der Laute im NumberString:
#define SAY_D_NULL       0
#define SAY_D_1          1             //"Eins"
#define SAY_D_2          2
#define SAY_D_3          3
#define SAY_D_4          4
#define SAY_D_5          5
#define SAY_D_6          6
#define SAY_D_7          7
#define SAY_D_8          8
#define SAY_D_9          9
#define SAY_D_10        10
#define SAY_D_11        11
#define SAY_D_12        12
#define SAY_D_13        13
#define SAY_D_14        14
#define SAY_D_15        15
#define SAY_D_16        16
#define SAY_D_17        17
#define SAY_D_18        18
#define SAY_D_19        19
#define SAY_D_20        20
#define SAY_D_30        21
#define SAY_D_40        22
#define SAY_D_50        23
#define SAY_D_60        24
#define SAY_D_70        25
#define SAY_D_80        26
#define SAY_D_90        27
#define SAY_D_Eine      28             //"Eine."
#define SAY_D_1_        29             //"Ein.."
#define SAY_D_2_        30
#define SAY_D_3_        31
#define SAY_D_4_        32
#define SAY_D_5_        33
#define SAY_D_6_        34
#define SAY_D_7_        35
#define SAY_D_8_        36
#define SAY_D_9_        37
#define SAY_D_10_       38
#define SAY_D_11_       39
#define SAY_D_12_       40
#define SAY_D_13_       41
#define SAY_D_14_       42
#define SAY_D_15_       43
#define SAY_D_16_       44
#define SAY_D_17_       45
#define SAY_D_18_       46
#define SAY_D_19_       47
#define SAY_D_20_       48
#define SAY_D_30_       49
#define SAY_D_40_       50
#define SAY_D_50_       51
#define SAY_D_60_       52
#define SAY_D_70_       53
#define SAY_D_80_       54
#define SAY_D_90_       55
#define SAY_D_1UND_     56             //"Ein.."
#define SAY_D_2UND_     57
#define SAY_D_3UND_     58
#define SAY_D_4UND_     59
#define SAY_D_5UND_     60
#define SAY_D_6UND_     61
#define SAY_D_7UND_     62
#define SAY_D_8UND_     63
#define SAY_D_9UND_     64
#define SAY_D_UND       65             //"..und.."
#define SAY_D_MINUS     66

#define SAY_D_100       67             //"..hundert"
#define SAY_D_100_      68             //"..hundert.."
#define SAY_D_1000      69             //"..tausend"
#define SAY_D_1000_     70             //"..tausend.."
#define SAY_D_1000000   71             //"..Millionen"
#define SAY_D_1000000_  72             //"..Millionen.."

#define SAY_D_MARK      73             //"Mark"
