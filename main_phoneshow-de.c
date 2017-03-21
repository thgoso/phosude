/*
phoneshow-de
Führt eine Phonetische Suche in einer Textdatei aus
Suchverfahren sind:
Kölner Phonetik		für deutsche Namen nach eigener Erfahrung am optimalsten
					Code Numerisch, variable Länge
Phonem				in versch. dBase Varianten implementiert
					Code Buchstaben, variable Länge
Soundex				Für deutsche Namen nicht ganz geeignet, aber Standard bei Genealogie(programmen)
					Alphanumerischer Code, fixe Länge, Format Buchstabe gefolgt von 3 Ziffern
Extended Soundex	Erweiterung von Soundex
					Numerischer Code, fixe Länge, 5 Ziffern


Copyright (C) 2015, Thomas Gollmer, th_goso@freenet.de
Dieses Programm ist freie Software. Sie können es unter den Bedingungen der GNU General Public License,
wie von der Free Software Foundation veröffentlicht, weitergeben und/oder modifizieren,
entweder gemäß Version 3 der Lizenz oder (nach Ihrer Option) jeder späteren Version.

Die Veröffentlichung dieses Programms erfolgt in der Hoffnung, daß es Ihnen von Nutzen sein wird,
aber OHNE IRGENDEINE GARANTIE, sogar ohne die implizite Garantie der
MARKTREIFE oder der VERWENDBARKEIT FÜR EINEN BESTIMMTEN ZWECK.
Details finden Sie in der GNU General Public License. Sie sollten ein Exemplar der GNU General Public License
zusammen mit diesem Programm erhalten haben. Falls nicht, siehe <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "phonetics.h"
#include "phoneshow_out.h"


// Statuskonstannten / Fehlercodes
#define PHSHOW_STAT_FOUND		0		// Es wurde Übereinstimmung im Text gefunden
#define PHSHOW_STAT_NO_FOUND	1		// Es wurde keine Übereinstimmung im Text gefunden
#define PHSHOW_STAT_TO_SHORT	2		// Name/Wort zur kurz zum sinnvollen Codieren
#define PHSHOW_STAT_TO_LONG		3		// Name/Wort zu Lang
#define PHSHOW_STAT_NO_CODE		4		// Name/Wort erzeugt keinen phonetischen Code
#define PHSHOW_STAT_NO_WORD		5		// Name/Wort kein ungültig (enthält Sonderzeichen usw.)
#define PHSHOW_STAT_PARAM_ERR	6		// ungültige Übergabeparameter
#define PHSHOW_STAT_STDIN_ERR	7		// Keine Daten über stdin erhalten
#define PHSHOW_STAT_MEM_ERR		8		// Fehler bei Speicheranforderung


//Puffergröße beim lesen von stdin
#define READBUFFER_LEN 			1024

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// passend zum Fehlercode wird ein Text ausgegeben
static void show_error (int err_no)
{
	if (err_no == PHSHOW_STAT_TO_SHORT) fprintf(stderr, "zu kurz zur sinnvollen Suche !\n");
	else if (err_no == PHSHOW_STAT_TO_LONG) fprintf(stderr, "zu lang !\n");
	else if (err_no == PHSHOW_STAT_NO_CODE) fprintf(stderr, "erzeugt keinen gültigen phonetischen Code !\n");
	else if (err_no == PHSHOW_STAT_NO_WORD) fprintf(stderr, "enthält unzulässige Zeichen !\n");
	else if (err_no == PHSHOW_STAT_MEM_ERR) fprintf(stderr, "Fehler bei Speicheranforderung !\n");
	else if (err_no == PHSHOW_STAT_STDIN_ERR) fprintf(stderr, "Keine Daten von stdin erhalten !\n");
	else if (err_no == PHSHOW_STAT_PARAM_ERR) {
		fprintf(stderr, "Falsche Aufrufparameter !\n"
		"Aufruf:    phoneshow-de [Optionen] Name[n] [_Name[n]]\n"
		"Hilfe:     phoneshow-de -h\n"
		"Beispiele: phoneshow-de -b\n\n");
	}
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void show_help (void)
{
	printf(
	"\n\033[1mphoneshow-de     Phonetische Suche von Namen/Wörtern in Text\033[m\n"
	"Aufruf           phoneshow-de [Optionen] Name[n] [_Name[n]]\n"
	"Übergabe         Textdaten von stdin\n"
	"Rückgabe         Textdaten formatiert in stdout\n"
	"Namen            Ein oder mehrere Namen nach denen gesucht werden soll\n"
	"_Namen           Ein oder mehrere Namen die bei der Suche ausgelassen werden sollen\n"
	"Optionen      -k Kölner Phonetik:  für den deutschen Sprachraum das Beste\n"
	"Phonetische   -p Phonem:           in manchen dBase Versionen enthalten auch gut für deutsche Namen\n"
	"Suchverfahren -s Soundex:          ehr für englische Namen, aber Standard im Genealogiebereich\n"
	"              -e Extended Soundex: erweiterte Soundex Variante\n"
	"                 Alle vier Varianten können einzeln oder kombiniert verwendet werden.\n"
	"                 Ohne Parameterangabe zum Suchverfahren ist die phonetische Suche inaktiv.\n"
	"Option        -l Wendet Levenshtein-Filter auf die Worte an, wenn kein phonetisches Verfahren\n"
	"Levenshtein      gewählt ist, oder auf den erzeugten phonetischen Code, wenn angegeben.\n"
	"Distanz          Jedes weitere -l erhöht die zulässige Levenshtein-Distanz um eins.\n"
	"Optionen      -a ganzen Text ausgeben\n"
	"Textausgabe   -z nur Fundzeilen ausgeben\n"
	"              -w nur Fundworte ausgeben\n"
	"keine         -c keine Textausgabe sondern lediglich Fundanzahl ausgeben.\n"
	"Textausgabe   -x keine Textausgabe sondern es wird zu jedem Fund Startposition und Länge\n"
	"                 (beides in char 'gemessen') in Bezug zur Eingabe von stdin ausgegeben.\n"
	"                 Eine der 5 Ausgabevarianten kann gewählt werden.\n"
	"                 Ohne Parameterangabe zur Ausgabe ist -z aktiv.\n"
	"Nummerierung  -n Nummerierung einschalten: Zeilennummer Fundzähler Zeilentext\n"
	"                 Wirkt nur bei -a -z -w nicht bei -c -x\n"
	"Farbe         -f Farbausgabe abschalten: (z.B. zur Weiterleitung in eine Datei)\n"
	"                 Nur nötig bei -a -z -w bei -c -x nicht relevant.\n"
	"Hilfe         -h Zeigt diese Hilfe an\n"
	"Hilfe         -b Zeigt Aufrufbeispiele an\n"
	"Exitcode       %d wenn mindestens ein phonetisch ähnlicher Name gefunden wurde\n"
	"               %d wenn kein phonetisch ähnlicher Name gefunden wurde\n"
	"               %d wenn Name zu kurz zum sinnvollen codieren\n"
	"               %d wenn Name zu lang\n"
	"               %d wenn auf Grund des Namens keine kodierung erfolgt z.B. phoneshow-de -p Aahe\n"
    "               %d wenn unerlaubte Zeichen im Namen enthalten sind. Erlaubt sind nur einzelne Worte,\n"
	"                  Buchstaben deutsches Alphabet incl. Umlaute. Keine Leerzeichen oder Satzzeichen.\n"
	"               %d wenn falsche Aufrufparameter\n"
	"               %d wenn keine Textdaten von stdin empfangen wurden\n"
	"               %d wenn es bei Speicheranfordeung zu Problemen kam\n\n", 
	PHSHOW_STAT_FOUND, PHSHOW_STAT_NO_FOUND, PHSHOW_STAT_TO_SHORT, PHSHOW_STAT_TO_LONG, PHSHOW_STAT_NO_CODE,
	PHSHOW_STAT_NO_WORD, PHSHOW_STAT_PARAM_ERR, PHSHOW_STAT_STDIN_ERR, PHSHOW_STAT_MEM_ERR);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void show_examples (void)
{
	printf(
	"\n\033[1mphoneshow-de Phonetische Suche von Namen/Wörtern in Text\033[m\n"
	"Beispiele Suche:\n"
	"\033[7mphoneshow-de Müller\033[m\n"
	"Findet NUR Müller, MÜLLER und müller Zwischen Groß- und Kleinschreibung wird nicht unterschieden.\n"
	"\033[7mphoneshow-de -l Müller\033[m\n"
	"Findet alle Worte die sich in max. einem Buchstaben unterscheiden, einen Buchstaben mehr oder\n"
	"einen weniger haben. Eine phonetische Suche findet hier nicht statt. Findet Mülker Rüller Kmüller\n"
	"\033[7mphoneshow-de -l -l Müller\033[m\n"
	"Jedes -l erhöht die Anzahl in der sich die Namen unterscheiden dürfen. Mit -l -l werden auch Worte\n"
	"gefunden die sich um zwei unterscheiden, also 2 Buchstaben mehr, weniger oder andere haben.\n"
	"Ebenfalls ohne phonetische Suche. Findet Mücker Küllar Müxxer Müllcker\n"
	"Sinnvoll sind max. 2-3 -l Mehr ist möglich aber nicht sehr zweckdienlich.\n"
	"\033[7mphoneshow-de -k Müller\033[m\n"
	"Findet alle Namen die nach Kölner Phonetik ähnlich zu Müller sind:\n"
	"Müller, Mueller, Myller, Müllar, Mülherr, Nüller, Nyller\n"
	"\033[7mphoneshow-de -p Müller\033[m\n"
	"Das selbe aber mit phonetischem Suchverfahren Phonem:\n"
	"Müller, Mueller, Myller, Müllar, Mülherr werden damit gefunden, Nüller und Nyller aber nicht.\n"
	"\033[7mphoneshow-de -k -p -s -e Müller\033[m\n"
	"Wendet alle phonetischen Verfahren an: Kölner Phonetik, Phonem, Soundex, Extended Soundex.\n"
	"Damit werden so ziemlich alle Varianten gefunden.\n"
	"\033[7mphoneshow-de -k -l Müller\033[m\n"
	"Wendet Suchverfahren Kölner Phonetik an und schiebt den erzeugten Code durch ein Levenshtein Filter.\n"
	"Der phonetische Code für Müller ist in dem Fall 657. Nun werden alle Worte gefunden deren Code sich\n"
	"an einer Stelle unterscheidet oder deren Code ein Zeichen mehr oder weniger hat.\n"
	"Somit werden auch Worte gefunden die den Code 667 oder 6578 oder 57 erzeugen. Auch hier gilt:\n"
	"Jedes zusätzliche -l erhört die Zahl an möglichen Abweichungen. 2-3 -l sind OK. mehr sinnlos.\n\n"
	"Name ausschließen mit _Name:\n"
	"Bei der Suche können EXAKTE Schreibweisen ausgeschlossen werden, wenn man ein _ voranstellt.\n"
	"\033[7mphoneshow-de -k -p Müller_Mueller\033[m\n"
	"Zeigt alle Namen die phonetisch ähnlich zu Müller sind, schließ aber die Schreibweise Mueller aus.\n"
	"Bei den verbotenen Namen wird zwischen Groß- und Kleinschreibung unterschieden. Das heiß:\n"
	"Gefunden werden Müller Möller Myller NICHT Mueller ABER mueller schon (weil Kleingeschrieben).\n"
	"\033[7mphoneshow-de Müller_Müller\033[m\n"
	"Zeigt überhaupt nichts weil die phonetische Suche inaktiv ist.\n"
	"\033[7mphoneshow-de -k Müller_Müller\033[m\n"
	"Zeigt alle phonetisch ähnlichen zu Müller, aber nicht die Schreibweise Müller selbst.\n\n"
	"Praxisbeispiele:\n"
	"\033[7mcat datei.txt | phoneshow-de -k -p Udhe\033[m soll uns alle Udhe und ähnliche zeigen.\n"
	"Allerdings findet man auch hunderte Otto, die stören und nicht benötigt werden.\n"
	"\033[7mcat datei.txt | phoneshow-de -k -p Udhe _Otto\033[m\n"
	"Filtert vorher alle Otto aus und wir bekommen weniger Funde die schon ehr zielführend sind.\n"
	"\033[7mcat datei.txt | phoneshow-de -c -k Schulze\033[m\n"
	"Anzahl Namen die nach Kölner Phonetik ähnlich zu Schulze sind und sich im Text befinden ausgeben.\n"
	"\033[7mcat datei.txt | phoneshow-de -k -p -s -e -n -f Thielemann > fundzeilen.txt\033[m\n"
	"Nutz alle phonetischen Suchverfahren, schaltet Zeilennummerierung ein, Farbe aus und schreibt\n"
	"alle Zeilen in denen sich etwas ähnliches findet nummeriert in fundzeilen.txt.\n"
	"\033[7mlynx -dump www.webseite.html | phoneshow-de -k -p -a Czerny\033[m\n"
	"Dazu muß der Browser lynx installiert sein, welcher einen Textdump der Internetseite erstellt.\n"
	"Es wird der volle Seitentext angezeigt und ähnlich klingende Namen wie Czerny werden markiert.\n"
	"Phonetische Suchverfahren = Kölner Phonetik und Phonem.\n\n");
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Liest Daten von stdin ein und gibt Pointer darauf zurück.
// Bei Problemen mit Speicheranforderung wird NULL zurückgegeben
static char *read_stdin(void)
{
	char	buffer[READBUFFER_LEN];
	size_t	data_len = 1;
	char	*data = malloc(sizeof(char) * READBUFFER_LEN);

	if (data == NULL) return (NULL);

	// Über Puffer Eingabe von stdin lesen und komplett in data speichern
	data[0]='\0';
	while (fgets(buffer, READBUFFER_LEN, stdin)) {
		char *old = data;
		data_len += strlen(buffer);
		data=realloc(data, data_len);
		if(data == NULL) {
			free(old);
			return (NULL);
		}
		strcat(data, buffer);
	}
	
	if (ferror(stdin)) {
		free(data);
		return (NULL);
	}
	
	return (data);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main (int argc, char* argv[])
{
	int			op_cnt;						// Laufzähler für Optionen
	int			letter_cnt;					// Laufzähler für Buchstabe
	int			name_cnt;					// Laufzähler Namen
	int			color_cnt=0;				// Laufzähler Farbe
	int			idxstart;					// Hilfsvars
	int			idxend;						// Index Übergabeparameter
	int			number_of_names;			// Anzahl übergebene Namen
	int			conv_stat;					// Status Code Name wandeln
	bool		only_minusname = true;		// Hilfsvar (Liste besteht NUR aus Minsnamen) true/false
	bool		found = false;				// Status ob ein Name im Eingabetext gefunden wurde oder nicht
	char		*text_data;					// Eingabedaten von stdin
	nameinfo_t	*names_list;				// Namensliste (mit allen Zusatzinfos)
	
	// Übergabeparameter (k,p,s,e,l,n,f)
	options_t	options = {false, false, false, false, 0, false, true};

	// Übergabeparameter (a,z,w,c,x) [Ausgabeformat]
	struct {
		bool	a;
		bool	z;
		bool	w;
		bool	c;
		bool	x;
	} outops = {false, false, false, false, false};
	

	// Wenn keine Übergabeparameter Ende
	if (argc == 1) {
		show_error (PHSHOW_STAT_PARAM_ERR);
		return PHSHOW_STAT_PARAM_ERR;
	}

	// Optionen aus Übergabe lesen und die Vars danach setzten
	for (op_cnt=1; op_cnt < argc; op_cnt++) {
		if (strcmp (argv[op_cnt], "-k") == 0)options.k = true;
		else if (strcmp (argv[op_cnt], "-p") == 0)options.p = true;
		else if (strcmp (argv[op_cnt], "-s") == 0)options.s = true;
		else if (strcmp (argv[op_cnt], "-e") == 0)options.e = true;
		else if (strcmp (argv[op_cnt], "-n") == 0)options.n = true;
		else if (strcmp (argv[op_cnt], "-f") == 0)options.f = false;
		else if (strcmp (argv[op_cnt], "-l") == 0)options.l++;
		else if (strcmp (argv[op_cnt], "-a") == 0)outops.a = true;
		else if (strcmp (argv[op_cnt], "-z") == 0)outops.z = true;
		else if (strcmp (argv[op_cnt], "-w") == 0)outops.w = true;
		else if (strcmp (argv[op_cnt], "-c") == 0)outops.c = true;
		else if (strcmp (argv[op_cnt], "-x") == 0)outops.x = true;
		else if (strcmp (argv[op_cnt], "-h") == 0) {
			show_help();
			return PHSHOW_STAT_PARAM_ERR;
		}
		else if (strcmp (argv[op_cnt], "-b") == 0) {
			show_examples();
			return PHSHOW_STAT_PARAM_ERR;
		}
		else break;
	}

	// Wenn kein Anzeigeparameter übergeben -a -z -w -c -x
	// -z einschalten
	if (outops.a==false && outops.z==false && outops.w==false && outops.c==false && outops.x==false) outops.z = true;

	// Keine Namen übergeben = Fehler
	if (op_cnt == argc) {
		show_error (PHSHOW_STAT_PARAM_ERR);
		return PHSHOW_STAT_PARAM_ERR;
	}

	// Index setzten 1. und letzter Name in Argliste, Anzahl
	idxstart = op_cnt;
	idxend = argc - 1;
	number_of_names = (idxend - idxstart) + 1;

	// Speicher für Namen Liste anfordern, Ende wenn Fehler
	names_list = (nameinfo_t*) malloc( number_of_names * sizeof( nameinfo_t ));
	if (names_list == NULL) {
		show_error (PHSHOW_STAT_MEM_ERR);
		return PHSHOW_STAT_MEM_ERR;
	}
	
	// Alle Namen aus Übergabe durchlaufen
	for (name_cnt = 0; name_cnt < number_of_names; name_cnt++) {
		// Ende wenn übergabe zu Kurz oder zu Lang
		if (strlen(argv[name_cnt+idxstart]) < 2) {
			fprintf (stderr, "%s ", argv[name_cnt+idxstart]);
			show_error (PHSHOW_STAT_TO_SHORT);
			free (names_list);
			return PHSHOW_STAT_TO_SHORT;
		}
		if (strlen(argv[name_cnt+idxstart]) >= (BUFFER_NAME_SIZE-1)) {
			fprintf (stderr, "%s ", argv[name_cnt+idxstart]);
			show_error (PHSHOW_STAT_TO_LONG);
			free (names_list);
			return PHSHOW_STAT_TO_LONG;
		}
		// Länge paßt... Namen in names_list[].Name kopieren
		strcpy(names_list[name_cnt].Name, argv[name_cnt + idxstart]);
		// Bei Minusname "_" entfernen und Ist_Minusname = true
		// Keine phon. Codes erzeugen, keine Farbe erzeugen
		// Normale SuchNamen Ist_Minusname = false setzten
		// phon. Codes erzeugen, Colorstring erzeugen
		if (names_list[name_cnt].Name[0] == '_') {
			letter_cnt=1;
			while (names_list[name_cnt].Name[letter_cnt] != '\0') {
				names_list[name_cnt].Name[letter_cnt-1] = names_list[name_cnt].Name[letter_cnt];
				letter_cnt++;
			} 
			names_list[name_cnt].Name[letter_cnt-1]='\0';
			names_list[name_cnt].Is_Minusname = true;
		}	
		else {
			names_list[name_cnt].Is_Minusname = false;
			if (options.f == true) strcpy (names_list[name_cnt].Colorstring, colors[color_cnt % NUMBER_OF_COLORS]);
			else names_list[name_cnt].Colorstring[0] = '\0';
			color_cnt++;
			only_minusname=false;
		}
		// Codes zum Namen erstellen je nach Optionen Phonetik
		// Fehler wenn ungültiger Code oder ungültiges Wort (Rest wird eh schon vorher geprüft)
		if ((options.k == true) && (names_list[name_cnt].Is_Minusname == false)) {
			conv_stat = phoneconvert (names_list[name_cnt].Name, names_list[name_cnt].Code_k, CONV_MODE_COLOGNE);
			if (conv_stat == CONV_STAT_NO_CODE) {
				fprintf (stderr, "-k %s ", names_list[name_cnt].Name);
				show_error(PHSHOW_STAT_NO_CODE);
				free (names_list);
				return PHSHOW_STAT_NO_CODE;
			}
			if (conv_stat == CONV_STAT_NO_WORD) {
				fprintf (stderr, "%s ", names_list[name_cnt].Name);
				show_error(PHSHOW_STAT_NO_WORD);
				free (names_list);
				return PHSHOW_STAT_NO_WORD;
			}			
		}
		if ((options.p == true) && (names_list[name_cnt].Is_Minusname == false)) {
			conv_stat = phoneconvert (names_list[name_cnt].Name, names_list[name_cnt].Code_p, CONV_MODE_PHONEM);
			if (conv_stat == CONV_STAT_NO_CODE) {
				fprintf (stderr, "-p %s ", names_list[name_cnt].Name);
				show_error(PHSHOW_STAT_NO_CODE);
				free (names_list);
				return PHSHOW_STAT_NO_CODE;
			}
			if (conv_stat == CONV_STAT_NO_WORD) {
				fprintf (stderr, "%s ", names_list[name_cnt].Name);
				show_error(PHSHOW_STAT_NO_WORD);
				free (names_list);
				return PHSHOW_STAT_NO_WORD;
			}			
		}
		if ((options.s == true) && (names_list[name_cnt].Is_Minusname == false)) {
			conv_stat = phoneconvert (names_list[name_cnt].Name, names_list[name_cnt].Code_s, CONV_MODE_SOUNDEX);
			if (conv_stat == CONV_STAT_NO_CODE) {
				fprintf (stderr, "-s %s ", names_list[name_cnt].Name);
				show_error(PHSHOW_STAT_NO_CODE);
				free (names_list);
				return PHSHOW_STAT_NO_CODE;
			}
			if (conv_stat == CONV_STAT_NO_WORD) {
				fprintf (stderr, "%s ", names_list[name_cnt].Name);
				show_error(PHSHOW_STAT_NO_WORD);
				free (names_list);
				return PHSHOW_STAT_NO_WORD;
			}			
		}
		if ((options.e == true) && (names_list[name_cnt].Is_Minusname == false)) {
			conv_stat = phoneconvert (names_list[name_cnt].Name, names_list[name_cnt].Code_e, CONV_MODE_EXSOUNDEX);
			if (conv_stat == CONV_STAT_NO_CODE) {
				fprintf (stderr, "-e %s ", names_list[name_cnt].Name);
				show_error(PHSHOW_STAT_NO_CODE);
				free (names_list);
				return PHSHOW_STAT_NO_CODE;
			}
			if (conv_stat == CONV_STAT_NO_WORD) {
				fprintf (stderr, "%s ", names_list[name_cnt].Name);
				show_error(PHSHOW_STAT_NO_WORD);
				free (names_list);
				return PHSHOW_STAT_NO_WORD;
			}			
		}
	}
	// Wenn Namensliste NUR aus Minusnamen besteht = Fehler
	if (only_minusname == true) {
		free (names_list);
		return PHSHOW_STAT_PARAM_ERR;
	}
	
	// stdin lesen Ende bei MEM ERROR
	text_data=read_stdin();
	if (text_data == NULL) {
		show_error(PHSHOW_STAT_MEM_ERR);
		free (names_list);
		return PHSHOW_STAT_MEM_ERR;
	}

	// Wenn übergebenen Daten leer Ende
	if (text_data[0] == '\0') {
		show_error(PHSHOW_STAT_STDIN_ERR);
		free (names_list);
		free (text_data);
		return PHSHOW_STAT_STDIN_ERR;
	}

	// Datenausgabe je nach Anzeigemodus starten Exitcode je nach Anzahl Funde
	// Wenn mehr als ein Anzeigeparameter übergeben, "gewinnt" der Höhere
	if      (outops.a == true) found = outmode_a (names_list, number_of_names, options, text_data);
	else if (outops.z == true) found = outmode_z (names_list, number_of_names, options, text_data);
	else if (outops.w == true) found = outmode_w (names_list, number_of_names, options, text_data);
	else if (outops.c == true) found = outmode_c (names_list, number_of_names, options, text_data);
	else if (outops.x == true) found = outmode_x (names_list, number_of_names, options, text_data);

	free (names_list);
	free (text_data);
	
	if (found == true) return PHSHOW_STAT_FOUND;
	else return PHSHOW_STAT_NO_FOUND;
}


