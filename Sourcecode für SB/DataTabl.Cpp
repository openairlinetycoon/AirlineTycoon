//============================================================================================
//DataTable.cpp - Die allgemeine Struktur für die Tabellendaten
//============================================================================================
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char FileId[] = "Data";

//--------------------------------------------------------------------------------------------
//Löscht eine Tabelle:
//--------------------------------------------------------------------------------------------
void CDataTable::Destroy (void)
{
   AnzColums = 0;
   AnzRows   = 0;
   Values.ReSize (0);
   ValueFlags.ReSize (0);
   ColTitle.ReSize (0);
   LineIndex.ReSize (0);
}

//--------------------------------------------------------------------------------------------
//Sortiert eine Tabelle:
//--------------------------------------------------------------------------------------------
void CDataTable::Sort (void)
{
}

//--------------------------------------------------------------------------------------------
//Füllt die Tabelle mit den Angaben zu den Flugzeugen, die als Parameter beiliegen:
//--------------------------------------------------------------------------------------------
void CDataTable::FillWithPlanes (CPlanes *Planes, BOOL Expert, SLONG FilterType, SLONG Filter1, SLONG Filter2)
{
   SLONG c, d;

   Title=StandardTexte.GetS (TOKEN_SCHED, 1000);

   AnzColums = 4;
   AnzRows   = Planes->GetNumUsed();
   Values.ReSize (0);
   Values.ReSize (AnzColums*AnzRows);
   ValueFlags.ReSize (0);
   ValueFlags.ReSize (AnzColums*AnzRows);
   ColTitle.ReSize (AnzColums);
   LineIndex.ReSize (AnzRows);

   for (c=0; c<AnzColums; c++)
      ColTitle[c]=StandardTexte.GetS (TOKEN_SCHED, 1000+c+1);

   if (Expert==2)  //Experte: Flugzeugkosten
   {
      for (c=0, d=0; c<(SLONG)Planes->AnzEntries(); c++)
         if (Planes->IsInAlbum(c))
         {
            CPlane &qPlane = (*Planes)[c];

            LineIndex[d]  = Planes->GetIdFromIndex(c);
            Values[d*4+0] = qPlane.Name;
            Values[d*4+1] = bitoa (qPlane.NumPannen);
            ValueFlags[d*4+0]=FALSE;

            d++;
         }
   }
   else if (Expert==1)  //Experte: Flugzeugsaldo
   {
      for (c=0, d=0; c<(SLONG)Planes->AnzEntries(); c++)
         if (Planes->IsInAlbum(c))
         {
            LineIndex[d]  = Planes->GetIdFromIndex(c);
            Values[d*4+0] = (*Planes)[c].Name;
            Values[d*4+1] = Einheiten[EINH_DM].bString ((*Planes)[c].GetSaldo());
            if ((*Planes)[c].GetSaldo()<0)
               ValueFlags[d*4+0]=TRUE;
            else
               ValueFlags[d*4+0]=FALSE;

            d++;
         }
   }
   else if (Expert==99)  //Werkstatt: Flugzeugkosten
   {
      for (c=0, d=0; c<(SLONG)Planes->AnzEntries(); c++)
         if (Planes->IsInAlbum(c))
         {
            LineIndex[d]  = Planes->GetIdFromIndex(c);
            Values[d*4+0] = (*Planes)[c].Name;
            Values[d*4+1] = Einheiten[EINH_DM].bString ((*Planes)[c].Wartungskosten);
            Values[d*4+2] = Einheiten[EINH_DM].bString ((*Planes)[c].Wartungskosten);
            ValueFlags[d*4+0]=FALSE;

            d++;
         }
   }
   else
   {
      for (c=0, d=0; c<(SLONG)Planes->AnzEntries(); c++)
         if (Planes->IsInAlbum(c))
         {
            CPlane &qPlane = (*Planes)[c];

            if (FilterType)
            {
               if (!qPlane.Flugplan.ContainsFlight (FilterType, Filter1, Filter2))
                  continue;
            }

            LineIndex[d]  = Planes->GetIdFromIndex(c);
            Values[d*4+0] = qPlane.Name;
            Values[d*4+1] = bprintf ("%li%%", (SLONG)qPlane.Zustand);
            Values[d*4+2] = Einheiten[EINH_DM].bString ((SLONG)qPlane.Wartungskosten);

            if (qPlane.Ort>=0)
               Values[d*4+3] = bprintf ("%s %s", StandardTexte.GetS (TOKEN_SCHED, 1105), Cities[qPlane.Ort].Name);
            else
               Values[d*4+3] = bprintf ("%s %s", StandardTexte.GetS (TOKEN_SCHED, 1102), Cities[qPlane.Flugplan.Flug[qPlane.Flugplan.NextFlight].NachCity].Name);

            //if (qPlane.AnzBegleiter<PlaneTypes[qPlane.TypeId].AnzBegleiter ||
            if (qPlane.AnzBegleiter<qPlane.ptAnzBegleiter ||
                qPlane.AnzPiloten<qPlane.ptAnzPiloten ||
                //qPlane.AnzPiloten<PlaneTypes[qPlane.TypeId].AnzPiloten ||
                qPlane.Problem)
               ValueFlags[d*4+0]=TRUE;
            else
               ValueFlags[d*4+0]=FALSE;

            ValueFlags[d*4+1]=qPlane.TargetZustand;

            d++;
         }
   }

   for (c=0; c<d-1; c++)
   {
      if (ValueFlags[c*4]==FALSE && ValueFlags[c*4+4]==TRUE ||
         (ValueFlags[c*4]==FALSE && ValueFlags[c*4+4]==FALSE && Values[c*4]>Values[c*4+4]))
      {
         SLONG tmp0;
         tmp0=LineIndex[c]; LineIndex[c]=LineIndex[c+1]; LineIndex[c+1]=tmp0;

         CString tmp1;
         tmp1=Values[c*4+0]; Values[c*4+0]=Values[c*4+4]; Values[c*4+4]=tmp1;
         tmp1=Values[c*4+1]; Values[c*4+1]=Values[c*4+5]; Values[c*4+5]=tmp1;
         tmp1=Values[c*4+2]; Values[c*4+2]=Values[c*4+6]; Values[c*4+6]=tmp1;
         tmp1=Values[c*4+3]; Values[c*4+3]=Values[c*4+7]; Values[c*4+7]=tmp1;

         UBYTE tmp2;
         tmp2=ValueFlags[c*4+0]; ValueFlags[c*4+0]=ValueFlags[c*4+4]; ValueFlags[c*4+4]=tmp2;
         tmp2=ValueFlags[c*4+1]; ValueFlags[c*4+1]=ValueFlags[c*4+5]; ValueFlags[c*4+5]=tmp2;

         c-=2; if (c<-1) c=-1;
      }
   }
}

//--------------------------------------------------------------------------------------------
//Füllt die Tabelle mit den Angaben zu den Flugzeugen:
//--------------------------------------------------------------------------------------------
void CDataTable::FillWithPlaneTypes (void)
{
   SLONG c, d;

   Title=StandardTexte.GetS (TOKEN_SCHED, 1600);

   AnzColums = 2;
   AnzRows   = PlaneTypes.GetNumUsed();
   Values.ReSize (0);
   Values.ReSize (AnzColums*AnzRows);
   ValueFlags.ReSize (0);
   ValueFlags.ReSize (AnzColums*AnzRows);
   ColTitle.ReSize (AnzColums);
   LineIndex.ReSize (AnzRows);

   ColTitle[0]=StandardTexte.GetS (TOKEN_SCHED, 1600+1);
   ColTitle[1]=" ";

   for (c=0, d=0; c<(SLONG)PlaneTypes.AnzEntries(); c++)
      if (PlaneTypes.IsInAlbum(c))
      if (PlaneTypes[c].FirstMissions<Sim.Difficulty || (PlaneTypes[c].FirstMissions==Sim.Difficulty && PlaneTypes[c].FirstDay<=Sim.Date) || (DIFF_FREEGAME==Sim.Difficulty && PlaneTypes[c].FirstDay<=Sim.Date))
      {
         LineIndex[d]  = PlaneTypes.GetIdFromIndex(c);
         Values[d*2+0] = PlaneTypes[c].Name;
         Values[d*2+1] = " ";
         ValueFlags[d*2+0]=FALSE;

         d++;
      }

   AnzRows=d;
}

//--------------------------------------------------------------------------------------------
//Füllt die Tabelle mit den Angaben zu den Flugzeugen:
//--------------------------------------------------------------------------------------------
void CDataTable::FillWithXPlaneTypes (void)
{
   SLONG c, d;

   Title=StandardTexte.GetS (TOKEN_SCHED, 1600);

   AnzColums = 3;
   AnzRows   = 9999;
   Values.ReSize (0);
   Values.ReSize (AnzColums*AnzRows);
   ValueFlags.ReSize (0);
   ValueFlags.ReSize (AnzColums*AnzRows);
   ColTitle.ReSize (AnzColums);
   LineIndex.ReSize (AnzRows);

   ColTitle[0]=StandardTexte.GetS (TOKEN_SCHED, 1600+1);
   ColTitle[1]=" ";
   ColTitle[2]=" ";

   BUFFER<CString> Array;
   GetMatchingFilelist (FullFilename ("*.plane", MyPlanePath), Array);

   for (c=0, d=0; c<(SLONG)Array.AnzEntries() && c<9999; c++)
   {
      CXPlane plane;

      CString fn = FullFilename (Array[c], MyPlanePath);
      if (fn!="") plane.Load (fn);

      if (plane.IsBuildable())
      {
         Values[d*3+0] = plane.Name;
         Values[d*3+1] = " ";
         Values[d*3+2] = fn;
         ValueFlags[d*3+0]=FALSE;

         d++;
      }
   }

   AnzRows=d;
   Values.ReSize (AnzColums*AnzRows);
   ValueFlags.ReSize (AnzColums*AnzRows);
   LineIndex.ReSize (AnzRows);
}

//--------------------------------------------------------------------------------------------
//Füllt die Tabelle mit den Daten der Routen die als Parameter beiliegen:
//--------------------------------------------------------------------------------------------
void CDataTable::FillWithRouten (CRouten *Routen, CRentRouten *RentRouten, BOOL UniqueOnly)
{
   SLONG c, d;

   Title=StandardTexte.GetS (TOKEN_SCHED, 1100);

   for (c=Routen->AnzEntries()-1, d=0; c>=0; c--)
      if (Routen->IsInAlbum(c) && RentRouten->RentRouten[c].Rang) d++;

   AnzColums = 5;
   AnzRows   = d;
   if (UniqueOnly) AnzRows/=2;

   Values.ReSize (0);
   Values.ReSize (AnzColums*AnzRows);
   ValueFlags.ReSize (0);
   ValueFlags.ReSize (AnzColums*AnzRows);
   ColTitle.ReSize (AnzColums);
   LineIndex.ReSize (AnzRows);

   /*for (c=0; c<AnzColums; c++)
      ColTitle[c]=StandardTexte.GetS (TOKEN_SCHED, 1100+c+1); */

   for (c=d=0; c<(SLONG)Routen->AnzEntries(); c++)
      if (Routen->IsInAlbum(c) && RentRouten->RentRouten[c].Rang && (UniqueOnly==0 || (*Routen)[c].VonCity<(*Routen)[c].NachCity))
      {
         LineIndex[d]          = Routen->GetIdFromIndex(c);
         Values[d*AnzColums+0] = Cities[(*Routen)[c].VonCity].Name;
         Values[d*AnzColums+1] = Cities[(*Routen)[c].NachCity].Name;
         Values[d*AnzColums+2] = Cities[(*Routen)[c].VonCity].Kuerzel;
         Values[d*AnzColums+3] = Cities[(*Routen)[c].NachCity].Kuerzel;

         Values[d*AnzColums+4] = CString (bitoa (Sim.Players.Players[(SLONG)Sim.localPlayer].AnzPlanesOnRoute(c))) + StandardTexte.GetS (TOKEN_ROUTE, 1015);

         ValueFlags[d*AnzColums+0] = FALSE;

         d++;
      }

   for (c=0; c<d-1; c++)
   {
      if (Values[c*AnzColums].Compare(Values[c*AnzColums+AnzColums])>0 ||
          (Values[c*AnzColums].Compare(Values[c*AnzColums+AnzColums])==0 && Values[c*AnzColums+1].Compare(Values[c*AnzColums+1+AnzColums])>0))
      {
         SLONG tmp0;
         tmp0=LineIndex[c]; LineIndex[c]=LineIndex[c+1]; LineIndex[c+1]=tmp0;

         CString tmp1;
         tmp1=Values[c*AnzColums+0]; Values[c*AnzColums+0]=Values[c*AnzColums+5];  Values[c*AnzColums+AnzColums]=tmp1;
         tmp1=Values[c*AnzColums+1]; Values[c*AnzColums+1]=Values[c*AnzColums+6];  Values[c*AnzColums+6]=tmp1;
         tmp1=Values[c*AnzColums+2]; Values[c*AnzColums+2]=Values[c*AnzColums+7];  Values[c*AnzColums+7]=tmp1;
         tmp1=Values[c*AnzColums+3]; Values[c*AnzColums+3]=Values[c*AnzColums+8];  Values[c*AnzColums+8]=tmp1;
         tmp1=Values[c*AnzColums+4]; Values[c*AnzColums+4]=Values[c*AnzColums+9];  Values[c*AnzColums+9]=tmp1;

         tmp0=ValueFlags[c*AnzColums+0]; ValueFlags[c*AnzColums+0]=ValueFlags[c*AnzColums+AnzColums]; ValueFlags[c*AnzColums+AnzColums]=UBYTE(tmp0);

         c-=2; if (c<-1) c=-1;
      }
   }
}

//--------------------------------------------------------------------------------------------
//Füllt die Tabelle mit den Daten aller Routen die als Parameter beiliegen:
//--------------------------------------------------------------------------------------------
void CDataTable::FillWithAllRouten (CRouten *Routen, CRentRouten *RentRouten, BOOL UniqueOnly)
{
   SLONG c, d;

   Title=StandardTexte.GetS (TOKEN_SCHED, 1100);

   for (c=Routen->AnzEntries()-1, d=0; c>=0; c--)
      if (Routen->IsInAlbum(c) && (UniqueOnly==0 || (*Routen)[c].VonCity<(*Routen)[c].NachCity)) d++;

   AnzColums = 5;
   AnzRows   = d;
   Values.ReSize (0);
   Values.ReSize (AnzColums*AnzRows);
   ValueFlags.ReSize (0);
   ValueFlags.ReSize (AnzColums*AnzRows);
   ColTitle.ReSize (AnzColums);
   LineIndex.ReSize (AnzRows);

   for (c=d=0; c<(SLONG)Routen->AnzEntries(); c++)
      if (Routen->IsInAlbum(c) && (UniqueOnly==0 || (*Routen)[c].VonCity<(*Routen)[c].NachCity))
      {
         LineIndex[d]          = Routen->GetIdFromIndex(c);
         Values[d*AnzColums+0] = Cities[(*Routen)[c].VonCity].Name;
         Values[d*AnzColums+1] = Cities[(*Routen)[c].NachCity].Name;
         Values[d*AnzColums+2] = Cities[(*Routen)[c].VonCity].Kuerzel;
         Values[d*AnzColums+3] = Cities[(*Routen)[c].NachCity].Kuerzel;

         //Values[d*AnzColums+4] = CString (bitoa (Sim.Players.Players[(SLONG)Sim.localPlayer].AnzPlanesOnRoute(c))) + StandardTexte.GetS (TOKEN_ROUTE, 1015);

         ValueFlags[d*AnzColums+0] = (RentRouten->RentRouten[c].Rang>0);

         d++;
      }

   for (c=0; c<d-1; c++)
   {
      if (Values[c*AnzColums].Compare(Values[c*AnzColums+AnzColums])>0 ||
          (Values[c*AnzColums].Compare(Values[c*AnzColums+AnzColums])==0 && Values[c*AnzColums+1].Compare(Values[c*AnzColums+1+AnzColums])>0))
      {
         SLONG tmp0;
         tmp0=LineIndex[c]; LineIndex[c]=LineIndex[c+1]; LineIndex[c+1]=tmp0;

         CString tmp1;
         tmp1=Values[c*AnzColums+0]; Values[c*AnzColums+0]=Values[c*AnzColums+5];  Values[c*AnzColums+AnzColums]=tmp1;
         tmp1=Values[c*AnzColums+1]; Values[c*AnzColums+1]=Values[c*AnzColums+6];  Values[c*AnzColums+6]=tmp1;
         tmp1=Values[c*AnzColums+2]; Values[c*AnzColums+2]=Values[c*AnzColums+7];  Values[c*AnzColums+7]=tmp1;
         tmp1=Values[c*AnzColums+3]; Values[c*AnzColums+3]=Values[c*AnzColums+8];  Values[c*AnzColums+8]=tmp1;
         tmp1=Values[c*AnzColums+4]; Values[c*AnzColums+4]=Values[c*AnzColums+9];  Values[c*AnzColums+9]=tmp1;

         tmp0=ValueFlags[c*AnzColums+0]; ValueFlags[c*AnzColums+0]=ValueFlags[c*AnzColums+AnzColums]; ValueFlags[c*AnzColums+AnzColums]=UBYTE(tmp0);

         c-=2; if (c<-1) c=-1;
      }
   }
}

//--------------------------------------------------------------------------------------------
//Füllt die Tabelle mit den Daten der Aufträge, die als Paramter beiliegen:
//--------------------------------------------------------------------------------------------
void CDataTable::FillWithAuftraege (CAuftraege *Auftraege)
{
   SLONG c, d;

   Title=StandardTexte.GetS (TOKEN_SCHED, 1200);

   AnzColums = 7;
   AnzRows   = Auftraege->GetNumUsed();
   Values.ReSize (0);
   Values.ReSize (AnzColums*AnzRows);
   ValueFlags.ReSize (0);
   ValueFlags.ReSize (AnzColums*AnzRows);
   ColTitle.ReSize (AnzColums);
   LineIndex.ReSize (AnzRows);

   for (c=0; c<AnzColums; c++)
      ColTitle[c]="";

   for (c=Auftraege->AnzEntries()-1, d=0; c>=0; c--)
      if (Auftraege->IsInAlbum(c) && (*Auftraege)[c].InPlan!=-1 && (*Auftraege)[c].BisDate>=Sim.Date)
      {
         LineIndex[d]          = Auftraege->GetIdFromIndex(c);
         Values[d*AnzColums+0] = Cities[(*Auftraege)[c].VonCity].Name;
         Values[d*AnzColums+1] = Cities[(*Auftraege)[c].NachCity].Name;
         Values[d*AnzColums+2] = Cities[(*Auftraege)[c].VonCity].Kuerzel;
         Values[d*AnzColums+3] = Cities[(*Auftraege)[c].NachCity].Kuerzel;

         if ((*Auftraege)[c].Date==(*Auftraege)[c].BisDate)
            Values[d*AnzColums+4] = StandardTexte.GetS (TOKEN_SCHED, 3010+((*Auftraege)[c].Date+Sim.StartWeekday)%7);
         else
            Values[d*AnzColums+4] = CString(StandardTexte.GetS (TOKEN_SCHED, 3009)) + " " + CString(StandardTexte.GetS (TOKEN_SCHED, 3010+((*Auftraege)[c].BisDate+Sim.StartWeekday)%7));

         Values[d*AnzColums+5] = Einheiten[EINH_DM].bString ((*Auftraege)[c].Praemie);
         Values[d*AnzColums+6] = bprintf("(%li)", (*Auftraege)[c].Personen);

         ValueFlags[d*AnzColums+0] = (*Auftraege)[c].InPlan;
         ValueFlags[d*AnzColums+1] = (*Auftraege)[c].Okay;
         ValueFlags[d*AnzColums+2] = ((*Auftraege)[c].Okay==0)*2 + ((*Auftraege)[c].InPlan==0);
         d++;
      }

   for (c=0; c<d-1; c++)
   {
      if (ValueFlags[c*7+2]<ValueFlags[c*7+2+7] ||
          (ValueFlags[c*7+2]==ValueFlags[c*7+2+7] && (*Auftraege)[LineIndex[c]].BisDate>(*Auftraege)[LineIndex[c+1]].BisDate) ||
          (ValueFlags[c*7+2]==ValueFlags[c*7+2+7] && (*Auftraege)[LineIndex[c]].BisDate==(*Auftraege)[LineIndex[c+1]].BisDate && Values[c*7]>Values[c*7+7]))
      {
         SLONG tmp0;
         tmp0=LineIndex[c]; LineIndex[c]=LineIndex[c+1]; LineIndex[c+1]=tmp0;

         CString tmp1;
         tmp1=Values[c*7+0]; Values[c*7+0]=Values[c*7+7];  Values[c*7+7]=tmp1;
         tmp1=Values[c*7+1]; Values[c*7+1]=Values[c*7+8];  Values[c*7+8]=tmp1;
         tmp1=Values[c*7+2]; Values[c*7+2]=Values[c*7+9];  Values[c*7+9]=tmp1;
         tmp1=Values[c*7+3]; Values[c*7+3]=Values[c*7+10]; Values[c*7+10]=tmp1;
         tmp1=Values[c*7+4]; Values[c*7+4]=Values[c*7+11]; Values[c*7+11]=tmp1;
         tmp1=Values[c*7+5]; Values[c*7+5]=Values[c*7+12]; Values[c*7+12]=tmp1;
         tmp1=Values[c*7+6]; Values[c*7+6]=Values[c*7+13]; Values[c*7+13]=tmp1;

         tmp0=ValueFlags[c*7+0]; ValueFlags[c*7+0]=ValueFlags[c*7+7]; ValueFlags[c*7+7]=UBYTE(tmp0);
         tmp0=ValueFlags[c*7+1]; ValueFlags[c*7+1]=ValueFlags[c*7+8]; ValueFlags[c*7+8]=UBYTE(tmp0);
         tmp0=ValueFlags[c*7+2]; ValueFlags[c*7+2]=ValueFlags[c*7+9]; ValueFlags[c*7+9]=UBYTE(tmp0);

         c-=2; if (c<-1) c=-1;
      }
   }

   AnzRows=d;
}

//--------------------------------------------------------------------------------------------
//Füllt die Tabelle mit den Daten der Aufträge, die als Paramter beiliegen:
//--------------------------------------------------------------------------------------------
void CDataTable::FillWithFracht (CFrachten *Frachten)
{
   SLONG c, d;

   Title=StandardTexte.GetS (TOKEN_SCHED, 1200);

   AnzColums = 7;
   AnzRows   = Frachten->GetNumUsed();
   Values.ReSize (0);
   Values.ReSize (AnzColums*AnzRows);
   ValueFlags.ReSize (0);
   ValueFlags.ReSize (AnzColums*AnzRows);
   ColTitle.ReSize (AnzColums);
   LineIndex.ReSize (AnzRows);

   for (c=0; c<AnzColums; c++)
      ColTitle[c]="";

   for (c=Frachten->AnzEntries()-1, d=0; c>=0; c--)
      if (Frachten->IsInAlbum(c) && (*Frachten)[c].InPlan!=-1 && (*Frachten)[c].BisDate>=Sim.Date)
      {
         LineIndex[d]          = Frachten->GetIdFromIndex(c);
         Values[d*AnzColums+0] = Cities[(*Frachten)[c].VonCity].Name;
         Values[d*AnzColums+1] = Cities[(*Frachten)[c].NachCity].Name;
         Values[d*AnzColums+2] = Cities[(*Frachten)[c].VonCity].Kuerzel;
         Values[d*AnzColums+3] = Cities[(*Frachten)[c].NachCity].Kuerzel;

         if ((*Frachten)[c].Date==(*Frachten)[c].BisDate)
            Values[d*AnzColums+4] = StandardTexte.GetS (TOKEN_SCHED, 3010+((*Frachten)[c].Date+Sim.StartWeekday)%7);
         else
            Values[d*AnzColums+4] = CString(StandardTexte.GetS (TOKEN_SCHED, 3009)) + " " + CString(StandardTexte.GetS (TOKEN_SCHED, 3010+((*Frachten)[c].BisDate+Sim.StartWeekday)%7));

         Values[d*AnzColums+5] = Einheiten[EINH_DM].bString ((*Frachten)[c].Praemie);
         Values[d*AnzColums+6] = bprintf("(%li)", (*Frachten)[c].Tons);

         ValueFlags[d*AnzColums+0] = (*Frachten)[c].InPlan;
         ValueFlags[d*AnzColums+1] = (*Frachten)[c].Okay;
         ValueFlags[d*AnzColums+2] = ((*Frachten)[c].Okay==0)*2 + ((*Frachten)[c].InPlan==0);
         d++;
      }

   for (c=0; c<d-1; c++)
   {
      if (ValueFlags[c*7+2]<ValueFlags[c*7+2+7] ||
          (ValueFlags[c*7+2]==ValueFlags[c*7+2+7] && (*Frachten)[LineIndex[c]].BisDate>(*Frachten)[LineIndex[c+1]].BisDate) ||
          (ValueFlags[c*7+2]==ValueFlags[c*7+2+7] && (*Frachten)[LineIndex[c]].BisDate==(*Frachten)[LineIndex[c+1]].BisDate && Values[c*7]>Values[c*7+7]))
      {
         SLONG tmp0;
         tmp0=LineIndex[c]; LineIndex[c]=LineIndex[c+1]; LineIndex[c+1]=tmp0;

         CString tmp1;
         tmp1=Values[c*7+0]; Values[c*7+0]=Values[c*7+7];  Values[c*7+7]=tmp1;
         tmp1=Values[c*7+1]; Values[c*7+1]=Values[c*7+8];  Values[c*7+8]=tmp1;
         tmp1=Values[c*7+2]; Values[c*7+2]=Values[c*7+9];  Values[c*7+9]=tmp1;
         tmp1=Values[c*7+3]; Values[c*7+3]=Values[c*7+10]; Values[c*7+10]=tmp1;
         tmp1=Values[c*7+4]; Values[c*7+4]=Values[c*7+11]; Values[c*7+11]=tmp1;
         tmp1=Values[c*7+5]; Values[c*7+5]=Values[c*7+12]; Values[c*7+12]=tmp1;
         tmp1=Values[c*7+6]; Values[c*7+6]=Values[c*7+13]; Values[c*7+13]=tmp1;

         tmp0=ValueFlags[c*7+0]; ValueFlags[c*7+0]=ValueFlags[c*7+7]; ValueFlags[c*7+7]=UBYTE(tmp0);
         tmp0=ValueFlags[c*7+1]; ValueFlags[c*7+1]=ValueFlags[c*7+8]; ValueFlags[c*7+8]=UBYTE(tmp0);
         tmp0=ValueFlags[c*7+2]; ValueFlags[c*7+2]=ValueFlags[c*7+9]; ValueFlags[c*7+9]=UBYTE(tmp0);

         c-=2; if (c<-1) c=-1;
      }
   }

   AnzRows=d;
}

//--------------------------------------------------------------------------------------------
//Füllt die Datentabelle mit allen verfügbaren Städten:
//--------------------------------------------------------------------------------------------
void CDataTable::FillWithCities (CRentCities *RentCities)
{
   SLONG c, d;

   Title=StandardTexte.GetS (TOKEN_SCHED, 1400);

   AnzColums = 3;
   AnzRows   = Cities.GetNumUsed();
   Values.ReSize (0);
   Values.ReSize (AnzColums*AnzRows);
   ValueFlags.ReSize (0);
   ValueFlags.ReSize (AnzColums*AnzRows);
   ColTitle.ReSize (AnzColums);
   LineIndex.ReSize (AnzRows);

   for (c=0; c<AnzColums; c++)
      ColTitle[c]=StandardTexte.GetS (TOKEN_SCHED, 1400+c+1);

   for (c=d=0; c<(SLONG)Cities.AnzEntries(); c++)
      if (Cities.IsInAlbum(c))
      {
         if (RentCities && RentCities->RentCities[c].Rang)
            ValueFlags[d*3+0]=TRUE;
         else
            ValueFlags[d*3+0]=FALSE;

         LineIndex[d]  = Cities.GetIdFromIndex(c);
         Values[d*3+0] = Cities[c].Name;
         Values[d*3+1] = Cities[c].Lage;
         d++;
      }
}

//--------------------------------------------------------------------------------------------
//Füllt die Datentabelle mit allen verfügbaren Expertenmeinungen:
//--------------------------------------------------------------------------------------------
void CDataTable::FillWithExperts (SLONG /*PlayerNum*/)
{
   SLONG c, d;

   Title = StandardTexte.GetS (TOKEN_EXPERT, 1000);

   AnzColums = 1;
   AnzRows   = 4;
   Values.ReSize (0);
   Values.ReSize (AnzColums*AnzRows);
   ValueFlags.ReSize (0);
   ValueFlags.ReSize (AnzColums*AnzRows);
   ColTitle.ReSize (AnzColums);
   LineIndex.ReSize (AnzRows);

   ColTitle[0] = StandardTexte.GetS (TOKEN_EXPERT, 1000);

   for (c=d=0; c<4; c++)
   {
      LineIndex[c] = c;
      Values[d+0]  = StandardTexte.GetS (TOKEN_EXPERT, 2000+c);
      d++;
   }
}

//--------------------------------------------------------------------------------------------
//Speichert ein DataTable-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CDataTable &d)
{
   File << d.Title      << d.AnzRows   << d.AnzColums;
   File << d.ColTitle   << d.Values;
   File << d.ValueFlags << d.LineIndex;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein DataTable-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CDataTable &d)
{
   File >> d.Title      >> d.AnzRows   >> d.AnzColums;
   File >> d.ColTitle   >> d.Values;
   File >> d.ValueFlags >> d.LineIndex;

   return (File);
}