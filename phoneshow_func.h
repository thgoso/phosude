#ifndef PHONESHOW_FUNC
#define PHONESHOW_FUNC

// Option Ein/Aus (-k -p -s -e usw.)
#define OP_ON 				1
#define OP_OFF 				0
// Max. Länge Colorstring
#define COLORSTRING_LEN 	16

// Für je einen übergebenen Namen
typedef struct {
	char	Name[MAX_WORD_LEN];
	int		Is_Minusname;
	char	Code_k[MAX_WORD_LEN];
	char	Code_p[MAX_WORD_LEN];
	char	Code_s[MAX_WORD_LEN];
	char	Code_e[MAX_WORD_LEN];
	char	Colorstring[COLORSTRING_LEN];
} nameinfo_t;

// Für Optionen
typedef struct {
	int	k;
	int	p;
	int	s;
	int	e;
	int	l;
	int	n;
	int	f;
} options_t;



// Funktion zum Einlesen der Daten von stdin
char *read_stdin		(void);
// Prüffunktion für Namensliste
int	check_names_list	(nameinfo_t names_list[], int number_of_names, options_t options);
// Ausgabefunktionen
int	outmode_a			(nameinfo_t names_list[], int number_of_names, options_t options, char *text_data);
int	outmode_z			(nameinfo_t names_list[], int number_of_names, options_t options, char *text_data);
int	outmode_w			(nameinfo_t names_list[], int number_of_names, options_t options, char *text_data);
int	outmode_c			(nameinfo_t names_list[], int number_of_names, options_t options, char *text_data);
int	outmode_x			(nameinfo_t names_list[], int number_of_names, options_t options, char *text_data);


#endif //PHONESHOW_FUNC



