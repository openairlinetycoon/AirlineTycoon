
//============================================================================================
// Statistk.h : Der Statistik-Screen
//============================================================================================

class CStatButton
{
   public:
      SLONG  Id;                       //Frei wählbar, aber bitte immer eindeutig
      SLONG  HelpId;                   //ToolTipID
      CRect  HotArea;                  //MouseArea, wo der Button gehighlighted wird
      XY     BitmapOffset;             //Hierhin wird die Button-Bitmap geblittet
      XY    *BitmapOffset2;            //Referenzmöglichkeit für bewegte Buttons
      SBBM  *BitmapNormal;             //Normale Bitmap oder NULL
      SBBM  *BitmapHi;                 //Bitmap fürs Highlight oder NULL
      SBBM  *BitmapClicked;            //Bitmap fürs Geclickte oder NULL
      SBBM  *BitmapSuperHi;            //Nur für Toggle-Buttons: Highlight, wenn geklickt
      BOOL   IsToggle;                 //Ist es ein Toggle-Button?
      DWORD  LastClicked;              //Wann wurde er zum letzten Mal geklickt (intern)

   public:
      void ReSize (SLONG Id, SLONG  HelpId, CRect HotArea, XY BitmapOffset, XY *BitmapOffset2, SBBM *BitmapNormal, SBBM *BitmapHi, SBBM *BitmapClicked, SBBM *BitmapSuperHi, BOOL IsToggle);
};

#define	MAX_GROUP			3				// Anzahl der Gruppen
#define	MAX_ITEMS			16				// Anzahl der Einträge je Gruppe
#define	LINE_DISTANCE		20				// Abstand der Zeilen

#define	TYP_LINEFEED		0				// Leerzeile
#define	TYP_GROUP			1				// Gruppe
#define	TYP_SUM_CURR		2				// Summe einer Gruppe
#define	TYP_SUM_CURR_N		3				// Summe einer Gruppe (-)
#define	TYP_SUM_DIFF		4				// Differenz der Summen
#define  TYP_SHAREVALUE		5				// Wert aller Aktien zusammen

#define	TYP_VALUE			20				// Normale Zahl
#define	TYP_CURRENCY		21				// Währung
#define	TYP_PERCENT    	22				// Prozent
#define	TYP_SINGLE_PERCENT 23		   // Prozent, aber nicht in Abhängigkeit von der Vorvariable


typedef struct tagItem
{
	bool	visible;
	short	textId;
	short	define;
	short	typOfItem;

} ITEM;


class CStatistik : public CStdRaum
{
// Construction
public:
	CStatistik(BOOL bHandy, ULONG PlayerNum);

   BUFFER<CStatButton> StatButtons;    //Die Statistik Buttons

   SBBM     HighlightBar;              //Der helle Balken für die optische Auswahl
   SBBM     TopSaver;                  //Verhindert, daß der obere Rand von den Zoomicons überschrien wird

   SBBM     DropDownBackgroundBm;      //Die Hintergrundbitmap
   SBBM     DropDownBm;                //Die Hintergrundbitmap mit Linien drauf
   SBBMS    UpDownArrows;              //Die Buttons zum runter/raufscrollen
                                      
   SBBM     TextTableBm;               //Die Bitmap mit den Texten (kann von der Drop-Down Liste überdeckt werden)
                                      
   SBBMS    LupeLogoBms;               //Die Lupe mit dem Firmenlogo
   SBBMS    LupeZoomBms;               //Die Lupe mit +/s
   SBBMS    ButtonGeldBms;             //Die Buttons mit dem Geld
   SBBMS    ButtonRouteBms;            //Die Buttons mit der Route
                                      
   SBBMS    ExitBms;                   //Das Exit-Schild
   SBBMS    LogoBms;                   //Die Vier großen Spielerlogos in 3 varianten
	SBBMS		PobelBms;

   XY       DropDownPos;
   SLONG    DropDownSpeed;

	SBBM		Haeckchen;

   SB_CFont StatFonts[5];             //0=Schwarz, 1-4=Farben der Spieler


	// Diese Werte am besten in der Registry
	// speichern.
	bool		_fGraphVisible;							// true -> Der Graph ist sichtbar, ansonsten die schnöden Zahlen
	bool		_playerMask[4];							// Diese Spieler wurden zur Ansicht ausgewählt
	BYTE		_group;										// Die angewählte Gruppe (*0=Finanzen, 1=?, 2=?)
	long		_days;										// Anzahl der darzustellenden Tage
	long		_newDays;									// Für eine Animation
	ITEM		_iArray[MAX_GROUP][MAX_ITEMS];		// Merkt sich für jede Gruppe welche Einträge selektiert sind.

	// Temporäre Werte
	short		_selectedItem;
	short		_oldSelectedItem;
	double	_yAxis;
	double	_xGraph;
	double	_yGraph;

	bool		_fRepaint;

// Attributes
public:

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatistik)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStatistik();

	// Generated message map functions
protected:
   void	 RepaintGraphWindow (void);
	void	 CalcGraph (void);

   void RepaintTextWindow (void);

	//{{AFX_MSG(CStatistik)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};
