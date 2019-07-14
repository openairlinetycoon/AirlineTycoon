//============================================================================================
// Planer.h : Die Basisklasse für Globe und Laptop
//============================================================================================

class CPlaner : public CStdRaum
{
   // Construction
   public:
	   CPlaner(BOOL bHandy, ULONG PlayerNum, UWORD &EarthAlpha, BOOL IsLaptop);

   //Attributes:
   public:
      //Bitmaps:
      SBBM    GlobeBm;
      SBBM    PostItBm;                   //Das UnterCursor PostIt
      SBBMS   MapPlaneBms[4];             //Die kleinen Flugzeuge für den Globus

      TECBM   EarthBm;

      //Sonstiges:
      SLONG   LastPaintedMinute;          //Jede Stunden wegen den Schlössern ein repaint
      BOOL    FensterVisible;             //Wurden die Fenster ausgeblendet?
      BOOL    IsLaptop;                   //0=Globe, 1=Laptop
      UBYTE   UsedToRotate;               //Kennzeichnet ob Routen NACH Rotation notwendig sind

      BOOL    ModeDragAndDrop;            //Armin's Drag&Drop-Modus (TRUE) oder der alte Mode (FALSE)
      BOOL    TookUnderCursorWithThisClick;

      //Cursor, Dragging, PostIt
      SLONG   CurrentDragId;              //Falls ButtonDown, dann wird dieses Fenster gezogen
      XY      DragOffset;                 //Maus Position in der Überschrift
      XY      DragStart;                  //Hier hat der Dreck angefangen.... Hohoho
      SLONG   CurrentPostItType;          //Identifiziert PostIt-Bitmap des Auftrags
      SLONG   CurrentPostItId;            //Identifiziert PostIt-Bitmap des Auftrags
      SLONG   CurrentBlock;               //Über diesem Block ist der Cursor...
      BLOCK  *pBlock;                     //Pointer auf den Block oder NULL

      BOOL    IsInClientArea;             //Ist der Cursor in der Client-Area des Blockes
      XY      ClientPos;                  //Wenn ja, dann ist hier die Position in der Client-Area
      BOOL    IsInClientAreaB;            //Ist der Cursor in der Client-Area des Blockes
      XY      ClientPosB;                 //Wenn ja, dann ist hier die Position in der Client-Area

      XY      CurrentBlockPos;            //...und zwar an dieser Stelle (innerhalb der WorkingArea gesehen)
      SLONG   CurrentIcon;                //Hier ist gerade der Highlight
      BOOL    DragFlightMode;             //Wird gerade ein Flug verschoben?
      SLONG   DragFlightPar0;             //Wird gerade ein Flug verschoben?
      SLONG   DragFlightPar1;             //Wird gerade ein Flug verschoben?
      SLONG   DragFlightPar2;             //Wird gerade ein Flug verschoben?

      //Erdkugel:
      UWORD  &EarthAlpha;                 //Rotationsposition der Erde
      UWORD   EarthTargetAlpha;           //Zielposition der Rotation
      DWORD   LastTime;                   //Für's Sync bei rotieren

   // Operations:
   public:
      void PaintGlobe (void);
      void PaintGlobeRoutes (void);
      void PaintPostIt ();

      void PaintGlobeInScreen (XY TargetPos);
      void DoPollingStuff (void);
      void DoPostPaintPollingStuff (XY FlightPlanPos);
      void PaintBlockContents (TECBM *TargetBm, XY TargetPos);
      void HandleLButtonDouble (void);
      void HandleLButtonDown (void);
      void HandleRButtonDown (void);
      void HandleLButtonUp (void);
      void HandleRButtonUp (void);
      void ButtonNext (void);
      void ButtonPrev (void);
      void ButtonClose (void);
      void ButtonIndex (void);
      void ButtonNextB (void);
      void ButtonPrevB (void);
      void ButtonIndexB (void);
      void TurnGlobe (SLONG Angle);

   // Implementation
   public:
	   virtual ~CPlaner();

};