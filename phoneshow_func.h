#ifndef PHONESHOW_FUNC
#define PHONESHOW_FUNC

//Option Ein/Aus (-k -p -s -e usw.)
#define OP_EIN 1
#define OP_AUS 0
//Max. länge Farbstring
#define FARB_STRING_LAENGE 16
//Puffergröße beim lesen von stdin
#define PUFFER_GROESSE 512
//Levenshtein Dist.
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

//Für je einen übergebenen Namen
typedef struct
{
	char	Name[MAX_WORT_LAENGE];
	int	Ist_Minusname;
	char	Code_k[MAX_WORT_LAENGE];
	char	Code_p[MAX_WORT_LAENGE];
	char	Code_s[MAX_WORT_LAENGE];
	char	Code_e[MAX_WORT_LAENGE];
	char	Farbstring[FARB_STRING_LAENGE];
} NAME_FORM;

//Für Optionen
typedef struct
{
	int	k;
	int	p;
	int	s;
	int	e;
	int	l;
	int	n;
	int	f;
} OPT_FORM;

//Prototypen
char	*lies_eingabe		(void);
int	levenshtein		(char *s1, char *s2);
int	hole_wort		(char *textdaten, char *wort, long *txtposcnt);
int	pruefe_phongleich	(NAME_FORM name, char *wort, OPT_FORM optionen);
int	pruefe_namen_liste	(NAME_FORM namen_liste[], int anz_namen, OPT_FORM optionen);
int	ausgabe_a		(NAME_FORM namen_liste[], int anz_namen, OPT_FORM optionen, char *textdaten);
int	ausgabe_z		(NAME_FORM namen_liste[], int anz_namen, OPT_FORM optionen, char *textdaten);
int	ausgabe_w		(NAME_FORM namen_liste[], int anz_namen, OPT_FORM optionen, char *textdaten);
int	ausgabe_c		(NAME_FORM namen_liste[], int anz_namen, OPT_FORM optionen, char *textdaten);
int	ausgabe_x		(NAME_FORM namen_liste[], int anz_namen, OPT_FORM optionen, char *textdaten);
void	zeige_legende		(NAME_FORM namen_liste[], int anz_namen, OPT_FORM optionen, long funde);

#endif //PHONESHOW_FUNC



