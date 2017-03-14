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
#include "basis_func.h"
#include "phoneshow_func.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// passend zum Fehlercode wird ein Text ausgegeben
static void show_error (int err_no)
{
	if (err_no == TEXT_TO_SHORT) fprintf(stderr, "zu kurz zur sinnvollen Suche !\n");
	else if (err_no == TEXT_ERROR) fprintf(stderr, "enthält unzulässige Zeichen !\n");
	else if (err_no == TEXT_NO_CODE) fprintf(stderr, "erzeugt keinen gültigen phonetischen Code !\n");
	else if (err_no == TEXT_UNDEF) {
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
	"               %d wenn unerlaubte Zeichen im Namen enthalten sind. Erlaubt sind nur einzelne Worte,\n"
	"                  Buchstaben deutsches Alphabet incl. Umlaute. Keine Leerzeichen oder Satzzeichen.\n"
	"               %d wenn falsche Aufrufparameter\n"
	"               %d wenn Name kürzer als 2 Buchstaben\n"
	"               %d wenn auf Grund des Namens keine kodierung erfolgt z.B. phoneshow-de -p Aahe\n"
        "               %d wenn kein phonetisch ähnlicher Name im Text gefunden wurde\n"
	"               %d wenn es bei Speicheranfordeung zu Problemen kam\n\n",
	TEXT_OK, TEXT_ERROR, TEXT_UNDEF, TEXT_TO_SHORT, TEXT_NO_CODE, TEXT_NOT_EQUAL, MEM_ERR);
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
int main (int argc, char* argv[])
{
	int			cnt;
	int			retval=0;
	int			idxstart;
	int			idxend;
	int			number_of_names;
	char		*text_data;
	nameinfo_t	*names_list;
	
	// Übergabeparameter (k,p,s,e,l,n,f)
	options_t	options = {OP_OFF, OP_OFF, OP_OFF, OP_OFF, 0, OP_OFF, OP_ON};

	// Übergabeparameter (a,z,w,c,x) [Ausgabeformat]
	struct {
		int a;
		int	z;
		int	w;
		int	c;
		int	x;
	} outops = {OP_OFF, OP_OFF, OP_OFF, OP_OFF, OP_OFF};
	

	
	


	// Wenn keine Übergabeparameter Ende
	if (argc == 1) {
		show_error (TEXT_UNDEF);
		return (TEXT_UNDEF);
	}

	// Optionen aus Übergabe lesen und die Vars danach setzten
	for (cnt=1; cnt < argc; cnt++) {
		if (strcmp (argv[cnt], "-k") == 0)options.k = OP_ON;
		else if (strcmp (argv[cnt], "-p") == 0)options.p = OP_ON;
		else if (strcmp (argv[cnt], "-s") == 0)options.s = OP_ON;
		else if (strcmp (argv[cnt], "-e") == 0)options.e = OP_ON;
		else if (strcmp (argv[cnt], "-n") == 0)options.n = OP_ON;
		else if (strcmp (argv[cnt], "-f") == 0)options.f = OP_OFF;
		else if (strcmp (argv[cnt], "-l") == 0)options.l++;
		else if (strcmp (argv[cnt], "-a") == 0)outops.a = OP_ON;
		else if (strcmp (argv[cnt], "-z") == 0)outops.z = OP_ON;
		else if (strcmp (argv[cnt], "-w") == 0)outops.w = OP_ON;
		else if (strcmp (argv[cnt], "-c") == 0)outops.c = OP_ON;
		else if (strcmp (argv[cnt], "-x") == 0)outops.x = OP_ON;
		else if (strcmp (argv[cnt], "-h") == 0) {
			show_help();
			return 0;
		}
		else if (strcmp (argv[cnt], "-b") == 0) {
			show_examples();
			return 0;
		}
		else break;
	}

	// Wenn kein Anzeigeparameter übergeben -a -z -w -c -x
	// -z einschalten
	if (outops.a==OP_OFF && outops.z==OP_OFF && outops.w==OP_OFF && outops.c==OP_OFF && outops.x==OP_OFF) outops.z = OP_ON;

	// Keine Namen übergeben = Fehler
	if (cnt == argc) {
		show_error (TEXT_UNDEF);
		return (TEXT_UNDEF);
	}

	// Index setzten 1. und letzter Name in Argliste, Anzahl
	idxstart = cnt;
	idxend = argc - 1;
	number_of_names = (idxend - idxstart) + 1;

	// Speicher für Namen Liste anfordern und übergebene Namen kopieren
	names_list = (nameinfo_t*) malloc( number_of_names * sizeof( nameinfo_t ));
	if (names_list == NULL) {
		fprintf(stderr, "Fehler bei Speichanforderung !\n");
		return (MEM_ERR);
	}
	for (cnt = 0; cnt < number_of_names; cnt++) strcpy(names_list[cnt].Name, argv[cnt + idxstart]);

	// Namen prüfen und Rest in names_list ergänzen lassen, wenn Fehler Ende
	retval=check_names_list(names_list, number_of_names, options);
	if (retval != TEXT_OK) {
		show_error (retval);
		free (names_list);
		return (retval);
	}

	// stdin lesen Ende bei MEM ERROR
	text_data=read_stdin();
	if (text_data == NULL) {
		fprintf(stderr, "Fehler bei Speichanforderung !\n");
		free (names_list);
		return (MEM_ERR);
	}

	// Wenn übergebenen Daten leer Ende
	if (text_data[0] == '\0') {
		fprintf (stderr, "Keine Textdaten übergeben !\n");
		free (names_list);
		free (text_data);
		return (TEXT_UNDEF);
	}

	// Datenausgabe je nach Anzeigemodus starten Exitcode je nach Anzahl Funde
	// Wenn mehr als ein Anzeigeparameter übergeben, "gewinnt" der Höhere
	if      (outops.a == OP_ON) retval=outmode_a (names_list, number_of_names, options, text_data);
	else if (outops.z == OP_ON) retval=outmode_z (names_list, number_of_names, options, text_data);
	else if (outops.w == OP_ON) retval=outmode_w (names_list, number_of_names, options, text_data);
	else if (outops.c == OP_ON) retval=outmode_c (names_list, number_of_names, options, text_data);
	else if (outops.x == OP_ON) retval=outmode_x (names_list, number_of_names, options, text_data);

	free (names_list);
	free (text_data);
	return (retval);
}


