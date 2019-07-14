// stdafx.cpp : source file that includes just the standard includes
//	TakeOff.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

//MSC ist zu blöd für tiefer Verschachtelte Dependecies. Daher werden hier die Entsprechenden
//Includes vorgegauckelt. Tatsächlich ausgeführt werden sie aber nie.
#ifdef this_will_never_happen
   #include "e:\TeaklibW\TeakLibW.h"
   #include "e:\projekt\sbl\inc\sbl.h"
   #include "sbbm.h"
   #include "sse.h"
   #include "ColorFx.h"
   #include "glstd.h"
   #include "glbasis.h"

   #include "class.h"
   #include "Defines.h"
   #include "GameFrame.h"
   #include "Global.h"
   #include "Proto.h"
   #include "res\Resource.h"

   //Räume
   #include "SmackPrs.h"
   #include "StdRaum.h"
   #include "Planer.h"
   #include "AirportView.h"
   #include "Globe.h"
   #include "Laptop.h"
   #include "LastMin.h"
   #include "Options.h"
   #include "Personal.h"
   #include "TakeOff.h"
   #include "Schedule.h"
   #include "Werkstat.h"      //Die Werkstatthalle
   #include "World.h"         //Telefonieren mit den anderen Leuten
#endif