// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

template<bool>
struct static_assert;

template<>
struct static_assert<true> {};

#include <afxwin.h>
#include <ddraw.h>
#include <dplay.h>
#include <math.h>
#include <mmsystem.h>

#include "Defines.h"

#include "TeakLibW.H"
#include "sbl.h"
#include "sbbm.h"
#include "sse.h"
#include "ColorFx.h"

#include "glstd.h"
#include "glbasis.h"

//#include "res\Resource.h"

#include "class.h"         //Eigene Klassen
#include "Proto.h"
#include "TakeOff.h"
#include "GameFrame.h"     //Rahmenfenster mit PrimarySurface

//Räume:
#include "SmackPrs.h"
#include "StdRaum.h"
#include "Planer.h"        //Die Basisklasse für Laptop/Globe
#include "AirportView.h"   //Die Isometrische Sicht auf den Flughafen
#include "Globe.h"         //Der Nachfolger des Schedulers
#include "Laptop.h"        //Der Schleppable
#include "LastMin.h"       //Das LastMinute Reisebüro
#include "Options.h"       //Das Optionsfenster
#include "Personal.h"      //Das Personalbüro
#include "Schedule.h"      //Der Scheduler Bildschirm
#include "Werkstat.h"      //Die Werkstatthalle

#include "global.h"        //Deklarationen der globalen Variablen
#pragma hdrstop