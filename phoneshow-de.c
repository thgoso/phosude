/*
phoneshow-de
Führt eine Phonetische Suche in Text aus
Suchverfahren sind:
Kölner Phonetik		für deutsche Namen nach eigener Erfahrung am optimalsten
					Code Numerisch, variable Länge
Phonem				in versch. dBase Varianten implementiert
					Code Buchstaben, variable Länge
Soundex				Für deutsche Namen nicht ganz geeignet, aber Standard bei Genealogie(programmen)
					Alphanumerischer Code, fixe Länge, Format Buchstabe gefolgt von 3 Ziffern
Extended Soundex	Erweiterung von Soundex
					Numerischer Code, fixe Länge, 5 Ziffern


Copyright (C) 2015-2017, Thomas Gollmer, th_goso@freenet.de
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
#include <stdint.h>
#include "phonetics.h"
#include "phoneshow.h"



static name_t			*Names_List=NULL;						// Pointer auf Namensliste im RAM
static int				Number_Of_Names=0;						// Anzahl Namen in Liste
static char				Cur_Line_Text[LINE_BUFFER_SIZE];		// Aktuelle Zeile von stdin
static unsigned long	Cur_Line_Number=0;						// Aktuelle Zeilennummer
static char				Cur_Word[BUFFER_WORD_SIZE];				// Aktuelles abzuarbeitendes Wort/Zeichen
static phoneops_t		Phone_Ops = {false, false, false, false, 0, false, true, true};



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void show_help (void)
{
	printf(
	"\n\033[1mphoneshow-de:    Phonetische Suche von Namen/Wörtern in Text\033[m\n"
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
	"                 Eine der 3 Ausgabevarianten kann gewählt werden.\n"
	"                 Ohne Parameterangabe zur Ausgabe ist -z aktiv.\n"
	"Nummerierung  -n Zeilennummerierung einschalten\n"
	"Farbe         -f Farbausgabe abschalten         (z.B. bei Weiterleitung in eine Datei)\n"
	"Legende       -x Ausgabe der Legende abschalten (z.B. bei Weiterleitung in eine Datei)\n"
	"Hilfe         -h Zeigt diese Hilfe an\n"
	"Hilfe         -b Zeigt Aufrufbeispiele an\n"
	"Exitcode       %d wenn mindestens ein phonetisch ähnlicher Name gefunden wurde\n"
	"               %d wenn kein phonetisch ähnlicher Name gefunden wurde\n"
	"               %d wenn Name zu kurz zum sinnvollen codieren\n"
	"               %d wenn Name zu lang\n"
	"               %d wenn auf Grund des Namens keine kodierung erfolgt z.B. phoneshow-de -p Aahe\n"
    "                 oder unerlaubte Zeichen im Namen enthalten sind. Erlaubt sind nur einzelne Worte,\n"
	"                 Buchstaben deutsches Alphabet incl. Umlaute. Keine Leerzeichen oder Satzzeichen.\n"
	"               %d wenn falsche Aufrufparameter\n"
	"               %d wenn von stdin Textzeilen oder Worte mit Überlänge empfangen wurden\n"
	"               %d wenn es bei Speicheranfordeung zu Problemen kam\n\n", STAT_FOUND, STAT_NO_FOUND, STAT_TO_SHORT,
												STAT_TO_LONG, STAT_NO_CODE, STAT_PARAM_ERR, STAT_STDIN_ERR, STAT_MEM_ERR);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void show_examples (void)
{
	printf(
	"\n\033[1mphoneshow-de: Phonetische Suche von Namen/Wörtern in Text\033[m\n"
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
	"Filtert alle Otto aus und wir bekommen weniger Funde die schon ehr zielführend sind.\n"
	"\033[7mcat datei.txt | phoneshow-de -k -p -s -e -n -f Thielemann > fundzeilen.txt\033[m\n"
	"Nutz alle phonetischen Suchverfahren, schaltet Zeilennummerierung ein, Farbe aus und schreibt\n"
	"alle Zeilen in denen sich etwas ähnliches findet nummeriert in fundzeilen.txt.\n"
	"\033[7mcat datei.txt | phoneshow-de -k -p -s -e -w -f -x Meier Müller Schulz | sort | uniq -c\033[m\n"
	"Findet alle phonetisch ähnlichen zu Meier Müller Schulz.\n"
	"Schaltet Ausgabe von Farbe und Legende ab, sodaß lediglich die Funde ausgegeben werden.\n"
	"Leit durch sort und läßt von uniq zählen, sodaß eine Häufigkeitsliste ausgegeben wird.\n\n");
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Gibt genutzten Speicher für Namesnliste wieder frei
// Beendet Programm und gibt übergebene StatusNummer ans BS zurück
// Sowie Text aus bei Fehler
void clean_exit (const int status)
{
	if (Names_List != NULL) free(Names_List);
		
	if (status == STAT_TO_SHORT) fprintf(stderr, "phoneshow-de: Name zu kurz zur sinnvollen Suche !\n");
	else if (status == STAT_TO_LONG) fprintf(stderr, "phoneshow-de: Name zu lang !\n");
	else if (status == STAT_NO_CODE) fprintf(stderr, "phoneshow-de: Name erzeugt keinen gültigen phonetischen Code"
													"oder enthält unzulässige Zeichen !\n");
	else if (status == STAT_MEM_ERR) fprintf(stderr, "phoneshow-de: Fehler bei Speicheranforderung !\n");
	else if (status == STAT_STDIN_ERR) fprintf(stderr, "phoneshow-de: Suche Abgebrochen !\n"
														"Zeile oder Wort in Übergabedaten hat Überlänge !\n");
	else if (status == STAT_PARAM_ERR) {
		fprintf(stderr, "phoneshow-de: Falsche Aufrufparameter !\n"
		"Aufruf:    phoneshow-de [Optionen] Name[n] [_Name[n]]\n"
		"Hilfe:     phoneshow-de -h\n"
		"Beispiele: phoneshow-de -b\n\n");
	}
	else if (status == STAT_SHOW_EXAMPLES) show_examples();
	else if (status == STAT_SHOW_HELP) show_help();
		
	exit(status);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Reserviert Speicher für Namensliste und füllt mit default
// Übergabe: 	Anzahl Namen
// Rückgabe:	true bei Erfolg
// 				false bei Fehler Speicherreservierung
bool create_names_list (const int number_of_names)
{	
	int cnt;
	
	Names_List = (name_t*) malloc( number_of_names * sizeof( name_t ));
	if (Names_List == NULL) return false;
				
	for (cnt = 0; cnt < number_of_names; cnt++) {
		Names_List[cnt].is_minusname=false;
		Names_List[cnt].name_norm[0]='\0';
		Names_List[cnt].name_lower[0]='\0';
		Names_List[cnt].code_k[0]='\0';
		Names_List[cnt].code_p[0]='\0';
		Names_List[cnt].code_s[0]='\0';
		Names_List[cnt].code_e[0]='\0';
		Names_List[cnt].p_color=color_empty;
	}
	
	return true;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Wandelt alle Großbuchstaben im String in Kleinbuchstaben (A-Z), Umlaute
static void to_lower (char *s)
{
	const char	uml[3][3] = {{"Ä"},{"Ö"},{"Ü"}};
	const char	rep[3][3] = {{"ä"},{"ö"},{"ü"}};
	uint8_t		cnt;
	size_t		pos=0;

	while (s[pos] != '\0') {
		// A-Z
		if (s[pos] >= 'A' && s[pos] <= 'Z') {
			s[pos] += 32;
			pos++;										// Großbuchstabe pos+=1
			continue;									// Hier zurück
		}
		// Große Umlaute 0-2
		for (cnt = 0; cnt <= 2; cnt++) {
			if ((s[pos] == uml[cnt][0]) && (s[pos+1] == uml[cnt][1])) {
				s[pos] = rep[cnt][0];
				s[pos+1] = rep[cnt][1];
				pos++;									// Bei Umlaut pos NUR +1
				break;
			}
		}
		pos++;											// Wenn Umlaut war ist jetzt pos+=2, alles Andere pos+=1;
	}
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Levenshtein-Distanz aus http://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
// Berechnet die Levenshtein-Distanz zweier übergebener Strings
// Was in dem Fall auf das Wort direkt oder den erzeugten phonetischen Code angewandt werden kann
// Übergabe:	Pointer auf Wort1 und Wort2
// Rückgabe:	Distanz als int
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

static int levenshtein(const char *s1, const char *s2)
{
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = strlen(s1);
	s2len = strlen(s2);
	unsigned int column[s1len+1];
    
	for (y = 1; y <= s1len; y++)
		column[y] = y;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x-1; y <= s1len; y++) {
			olddiag = column[y];
			column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}
	return (column[s1len]);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Liest eine Zeile von stdin ein und speichert in Cur_Line_Text
// Aktuelle Zeilennummer wird in Cur_Line_Number gespeichert
// Rückgabe:	true wenn Zeile gelesen
//				false wenn Ende erreicht und nichts mehr von stdin kommt
// Funktion ruft die "clean_exit" routine auf
// Und beendet das Programm, wenn eine überlange Zeile eintrifft
static bool read_line (void)
{
	size_t	size;
		
	Cur_Line_Text[0]='\0';													// Ziel leeren
	
	if(fgets(Cur_Line_Text, LINE_BUFFER_SIZE, stdin) != NULL) {				// Block lesen
		size=strlen(Cur_Line_Text);
		if (size == LINE_BUFFER_SIZE - 1) clean_exit(STAT_STDIN_ERR);		// Wenn Block zu lang Ende
		else {																// Wenn Block keine Überlänge hat gültige Zeile
			if (Cur_Line_Text[size-1] == '\n') Cur_Line_Text[size-1]='\0';	// Zeilenendzeichen entfernen
			Cur_Line_Number++;												// Alles klar
			return true;
		}
	}
	
	if (ferror(stdin)) clean_exit(STAT_STDIN_ERR);							// Probleme beim Lesen von stdin... Ende
		
	return false;															// Wenn fgets NULL-Pointer liefert
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Holt das nächste gültige deutsche Wort aus Cur_Line und speichert es in Cur_Word
// Funktion so oft aufrufen bis Cur_Word '\0' ist... dann ist der String komplett durchlaufen 
// Funktion gibt true zurück wenn es sich um ein gültiges Wort handelt, welches dann in Cur_Word steht
// oder false wenn es ein Sonderzeichen ist
// In dem Fall wird lediglich das Sonderzeichen allein (plus Null-Byte) in Cur_Word gespeichert
// Übergabe: 	false zum Initialisieren (Interne "readpos" wird zurückgesetzt
//				true um das erste/nächste wort zu holen
// Funktion ruft clean_exit auf und beendet das Programm
// Wenn ein Wort mit Überlänge gefunden wird
static bool read_word (const bool read)
{	
	const char		uml[7][3] = {{"Ä"}, {"Ö"}, {"Ü"}, {"ä"}, {"ö"}, {"ü"}, {"ß"}};
	static size_t	readpos=0;
	size_t			writepos=0;
	bool			word_mode=false;
	uint8_t			cnt;
	
	// Bei Aufruf mit Param read == false
	// Zeichencounter zum lesen auf Start setzten
	// Wenn man die Zeile mehrmals lesen will
	if (read == false) {
		readpos = 0;
		return false;
	}
	
	// Ausgabe leeren
	Cur_Word[0]='\0';
	// Text komplett durch
	if (Cur_Line_Text[readpos] == '\0') return false;
	
	// 1. Zeichen bestimmen Buchstabe oder Sonderzeichen
	if ((Cur_Line_Text[readpos] >= 'a' && Cur_Line_Text[readpos] <= 'z') || (Cur_Line_Text[readpos] >= 'A' && Cur_Line_Text[readpos] <= 'Z')) {
		word_mode = true;
	}
	else {
		for (cnt=0; cnt<=6; cnt++) {
			if ((Cur_Line_Text[readpos] == uml[cnt][0]) && (Cur_Line_Text[readpos+1] == uml[cnt][1])) {
				word_mode = true;
				break;
			}
		}
	}
	
	// War Sonderzeichen, diese in Cur_Word schieben und raus
	if (word_mode == false) {
		Cur_Word[0]=Cur_Line_Text[readpos];
		Cur_Word[1]='\0';
		readpos++;
		return false;
	}

NextChar:
	if (writepos >= BUFFER_WORD_SIZE) clean_exit(STAT_STDIN_ERR);
	// So lange Zeichen rüberkopieren wie Buchstaben kommen
	if ((Cur_Line_Text[readpos] >= 'a' && Cur_Line_Text[readpos] <= 'z') || (Cur_Line_Text[readpos] >= 'A' && Cur_Line_Text[readpos] <= 'Z')) {
		Cur_Word[writepos] = Cur_Line_Text[readpos];
		Cur_Word[writepos+1] = '\0';
		readpos++;
		writepos++;
		goto NextChar;
	}
	for (cnt=0; cnt<=6; cnt++) {
		if ((Cur_Line_Text[readpos] == uml[cnt][0]) && (Cur_Line_Text[readpos+1] == uml[cnt][1])) {
			Cur_Word[writepos] = Cur_Line_Text[readpos];
			Cur_Word[writepos+1] = Cur_Line_Text[readpos+1];
			Cur_Word[writepos+2] = '\0';
			readpos+=2;
			writepos+=2;
			goto NextChar;
		}
	}
	return true;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Prüft ob ein Name aus Namensliste mit momentanem Wort in Cur_Word identisch ist (je nach phon. Opt)
// Übergabe:	Nummer des Namens in Names_List
// Rückgabe:	true wenn mind. eine Übereinstimmung in einer der phon. Optionen, sonst false
static bool check_phone_equal (const int name_number)
{
	int		lvst_dist;
	char	code[BUFFER_CODE_SIZE];
	char	word_lower[BUFFER_WORD_SIZE];

	// Wenn keine phonetischen Optionen gewählt
	// Nur Lev Dist auf Name & Wort anwenden
	// Groß, Kleinschreibung ignorieren
	if (Phone_Ops.k == false && Phone_Ops.p == false && Phone_Ops.s == false && Phone_Ops.e == false) {
		strcpy(word_lower, Cur_Word);
		to_lower(word_lower);
		lvst_dist = levenshtein(Names_List[name_number].name_lower, word_lower);
		if (lvst_dist <= Phone_Ops.l) return true;
		else return false;
	}

	// Phon Opt gewählt, also Wort in phone Code wandeln
	// mit jeweiligen phon. Code des Namens vergleichen
	if (Phone_Ops.k == true) {
		phoneconvert_cologne(Cur_Word, code);
		if (Phone_Ops.l != 0) {
			lvst_dist = levenshtein (code, Names_List[name_number].code_k);
			if (lvst_dist <= Phone_Ops.l) return true;
		}
		else if (strcmp (code, Names_List[name_number].code_k) == 0) return true;
	}
	
	if (Phone_Ops.p == true) {
		phoneconvert_phonem(Cur_Word, code);
		if (Phone_Ops.l != 0) {
			lvst_dist = levenshtein (code, Names_List[name_number].code_p);
			if (lvst_dist <= Phone_Ops.l) return true;
		}
		else if (strcmp (code, Names_List[name_number].code_p) == 0) return true;
	}

	if (Phone_Ops.s == true) {
		phoneconvert_soundex(Cur_Word, code);
		if (Phone_Ops.l != 0) {
			lvst_dist = levenshtein (code, Names_List[name_number].code_s);
			if (lvst_dist <= Phone_Ops.l) return true;
		}
		else if (strcmp (code, Names_List[name_number].code_s) == 0) return true;
	}

	if (Phone_Ops.e == true) {
		phoneconvert_exsoundex(Cur_Word, code);
		if (Phone_Ops.l != 0) {
			lvst_dist = levenshtein (code, Names_List[name_number].code_e);
			if (lvst_dist <= Phone_Ops.l) return true;
		}
		else if (strcmp (code, Names_List[name_number].code_e) == 0) return true;
	}

	return false;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Prüft das momentanem Wort in Cur_Word auf phonetische Gleichheit mit der Namensliste Names_List
// Rückgabe:	true wenn mind. eine Übereinstimmung in einer der phon. Optionen, sonst false
// Rückgabe:	Index in Namensliste passend zum Fund
static bool check_names_list_phone_equal (int *name_number)
{
	int 	cnt;
	
	// Wort gegen Minusname prüfen, Ende bei EXAKTER Übereinstimmung
	for (cnt = 0; cnt < Number_Of_Names; cnt++) {
		if (Names_List[cnt].is_minusname == true) {
			if (strcmp(Names_List[cnt].name_norm, Cur_Word) == 0) return false;
		}
	}
	
	// Wort gegen normale Namen prüfen, raus bei phonetischer Übereinstimmung
	for (cnt = 0; cnt < Number_Of_Names; cnt++) {
		if (Names_List[cnt].is_minusname == false) {
			if (check_phone_equal(cnt) == true) {
				*name_number = cnt;
				return true;
			}
		}
	}
	
	// Keine Übereinstimmung
	return false;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Zeigt die Legende zur Suche an
// Wird von Ausgaberoutinen aufgerufen
// Übergeben werden muß Anzahl der Funde
static void show_footer (const unsigned long foundcount)
{
	int	cnt;
	int	nolist=0;

	// Gleich raus wenn Modus Legende Aus aktiv ist
	if (Phone_Ops.x == false) return;

	if (Phone_Ops.f == true) printf("\033[7m<<<<<< LEGENDE >>>>>>\033[m\n");
	else printf("<<<<<< LEGENDE >>>>>>\n");

	//Suchnamen
	printf("Suche: ");
	for (cnt = 0; cnt < Number_Of_Names; cnt++) {
		if (Names_List[cnt].is_minusname == false) {
			if (Phone_Ops.f == true) printf("%s%s\033[m ", Names_List[cnt].p_color, Names_List[cnt].name_norm);
			else printf("%s ", Names_List[cnt].name_norm);
		}
		else nolist=1;
	}

	//Verbotene Namen
	if (nolist == 1) {
		printf("\nnicht: ");
		for (cnt = 0; cnt < Number_Of_Names; cnt++) {
			if (Names_List[cnt].is_minusname == true) printf("%s ", Names_List[cnt].name_norm);
		}
	}

	//Optionen
	if (Phone_Ops.k == false && Phone_Ops.p == false && Phone_Ops.s == false && Phone_Ops.e == false && Phone_Ops.l == 0) {
		printf ("\nSuche: Einfache Textsuche");
	}
	else if (Phone_Ops.k == false && Phone_Ops.p == false && Phone_Ops.s == false && Phone_Ops.e == false && Phone_Ops.l != 0) {
		printf ("\nTyp:   Textsuche mit Levenshtein-Distanz von nicht mehr als %d", Phone_Ops.l);
	}
	else {
		printf("\nTyp:   Phonetische Suche ");
		if (Phone_Ops.k == true) printf ("(Kölner Phonetik) ");
		if (Phone_Ops.p == true) printf ("(Phonem) ");
		if (Phone_Ops.s == true) printf ("(Soundex) ");
		if (Phone_Ops.e == true) printf ("(Extended Soundex) ");
		if (Phone_Ops.l != 0) printf ("\n       mit Levenshtein-Distanz von nicht mehr als %d im phonetischen Code", Phone_Ops.l);
	}

	//Funde
	printf("\nFunde: %ld\n", foundcount);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus a
// gesammten Text anzeigen
static bool print_mode_a (void)
{
	bool			word_stat;
	bool			phone_stat;
	int				name_number;
	unsigned long 	foundcount=0;
	
	while (1) {
		if (read_line() == false) break;												// Nächste Zeile lesen, Ende wenn alle durch
		read_word(false);																// Wortlesefunktion init 
		if (Phone_Ops.n == true) printf("%ld ", Cur_Line_Number);						// Zeilennummer ausgeben falls erwünscht
		while (1) {
			word_stat = read_word(true);												// Nächstes Wort aus Zeile holen
			if (Cur_Word[0] == '\0') break;												// Alle durch
			if (word_stat == false) printf("%s", Cur_Word);								// War kein Wort sondern Sonderzeichen
			else {																		// War gültiges deutsche Wort
				phone_stat = check_names_list_phone_equal(&name_number);				// Phonetisch Vergleichen mit Namensliste
				if (phone_stat == true) {												// Wir haben einen Fund
					foundcount++;
					printf("%s%s\033[m", Names_List[name_number].p_color, Cur_Word);	// Ausgabe farbig
				}
				else printf("%s", Cur_Word);											// Ausgabe unbunt
			}
		}
		printf("\n");																	// Zeile ist durch
	}
		
	show_footer(foundcount);															// Legende anzeigen
	if (foundcount > 0) return true;													// Ende mit true wenn Fund(e) waren
	else return false;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus z
// Nur Fundzeilen anzeigen
static bool print_mode_z (void)
{
	bool			word_stat;
	bool			phone_stat;
	bool			show_line;
	int				name_number;
	unsigned long 	foundcount=0;
	
	while (1) {
		if (read_line() == false) break;												// Nächste Zeile lesen, Ende wenn alle durch
		read_word(false);																// Wortlesefunktion init 
		show_line=false;
		while (1) {																		// Zeile komplett prüfen
			word_stat = read_word(true);												// ob sich ein passendes Wort
			if (Cur_Word[0] == '\0') break;												// darinnen befinden
			if (word_stat == true) {													// falls ja, show_line = true
				phone_stat = check_names_list_phone_equal(&name_number);				// sonst false
				if (phone_stat == true) {
					show_line = true;
					break;
				}
			}
		}
		if (show_line == true) {														// Zeile muß ausgegeben werden, da Fund(e)
			read_word(false);															// Wortlesefunktion init 
			if (Phone_Ops.n == true) printf("%ld ", Cur_Line_Number);					// Zeilennummer ausgeben falls erwünscht
			while (1) {
				word_stat = read_word(true);											// Nächstes Wort aus Zeile holen
				if (Cur_Word[0] == '\0') break;											// Alle durch
				if (word_stat == false) printf("%s", Cur_Word);							// War kein Wort sondern Sonderzeichen
				else {																	// War gültiges deutsche Wort
					phone_stat = check_names_list_phone_equal(&name_number);			// Phonetisch Vergleichen mit Namensliste
					if (phone_stat == true) {											// Wir haben einen Fund
						foundcount++;
						printf("%s%s\033[m", Names_List[name_number].p_color, Cur_Word);// Ausgabe farbig
					}
					else printf("%s", Cur_Word);										// Ausgabe unbunt
				}
			}
			printf("\n");																// Zeile ist durch
		}
	}
		
	show_footer(foundcount);															// Legende anzeigen
	if (foundcount > 0) return true;													// Ende mit true wenn Fund(e) waren
	else return false;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus w
// nur Fundworte anzeigen
static bool print_mode_w (void)
{
	bool			word_stat;
	bool			phone_stat;
	int				name_number;
	unsigned long 	foundcount=0;
	
	while (1) {
		if (read_line() == false) break;													// Nächste Zeile lesen, Ende wenn alle durch
		read_word(false);																	// Wortlesefunktion init 
		while (1) {
			word_stat = read_word(true);													// Nächstes Wort aus Zeile holen
			if (Cur_Word[0] == '\0') break;													// Alle durch
			if (word_stat == true) {														// War gültiges deutsche Wort
				phone_stat = check_names_list_phone_equal(&name_number);					// Phonetisch Vergleichen mit Namensliste
				if (phone_stat == true) {													// Wir haben einen Fund
					foundcount++;
					if (Phone_Ops.n == true) printf("%ld ", Cur_Line_Number);
					if (Phone_Ops.f == true) {
						printf("%s%s\033[m\n", Names_List[name_number].p_color, Cur_Word);	// Ausgabe farbig
					}
					else printf("%s\n", Cur_Word);											// Ausgabe unbunt
				}
			}
		}
	}
		
	show_footer(foundcount);																// Legende anzeigen
	if (foundcount > 0) return true;														// Ende mit true wenn Fund(e) waren
	else return false;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main (int argc, char* argv[])
{
	int				letter_cnt;					// Hilfszähler Buchsaben
	int				op_cnt;						// Laufzähler für Optionen
	int				idxstart;					// Hilfsvars
	int				idxend;						// Index Übergabeparameter
	int				name_cnt;					// Laufzähler Namen
	int				color_cnt=0;				// Laufzähler Farbe
	bool			only_minusname=true;		// Hilfsvar (Liste besteht NUR aus Minsnamen) true/false
	struct outops_t {							// Struct für Ausgabeoptionen
		bool	a;
		bool	z;
		bool	w;
	} outops = {false, false, false};
	
	
	
	// Wenn keine Übergabeparameter Ende
	if (argc == 1) clean_exit(STAT_PARAM_ERR);
	
	// Optionen aus Übergabe lesen und die Vars danach setzten
	for (op_cnt=1; op_cnt < argc; op_cnt++) {
		if (strcmp (argv[op_cnt], "-k") == 0) Phone_Ops.k = true;
		else if (strcmp (argv[op_cnt], "-p") == 0) Phone_Ops.p = true;
		else if (strcmp (argv[op_cnt], "-s") == 0) Phone_Ops.s = true;
		else if (strcmp (argv[op_cnt], "-e") == 0) Phone_Ops.e = true;
		else if (strcmp (argv[op_cnt], "-n") == 0) Phone_Ops.n = true;
		else if (strcmp (argv[op_cnt], "-f") == 0) Phone_Ops.f = false;
		else if (strcmp (argv[op_cnt], "-x") == 0) Phone_Ops.x = false;
		else if (strcmp (argv[op_cnt], "-l") == 0) Phone_Ops.l++;
		else if (strcmp (argv[op_cnt], "-a") == 0) outops.a = true;
		else if (strcmp (argv[op_cnt], "-z") == 0) outops.z = true;
		else if (strcmp (argv[op_cnt], "-w") == 0) outops.w = true;
		else if (strcmp (argv[op_cnt], "-h") == 0) clean_exit (STAT_SHOW_HELP);
		else if (strcmp (argv[op_cnt], "-b") == 0) clean_exit (STAT_SHOW_EXAMPLES);
		else break;
	}

	// Wenn kein Anzeigeparameter übergeben -a -z -w ... -z einschalten
	if (outops.a==false && outops.z==false && outops.w==false) outops.z = true;

	// Keine Namen übergeben = Fehler
	if (op_cnt == argc) clean_exit (STAT_PARAM_ERR);

	// Index setzten 1. und letzter Name in Argliste, Anzahl
	idxstart = op_cnt;
	idxend = argc - 1;
	Number_Of_Names = (idxend - idxstart) + 1;

	// Speicher für Namen Liste anfordern, Ende wenn Fehler
	if (create_names_list(Number_Of_Names) == false) clean_exit (STAT_MEM_ERR);
	
	// Namensliste füllen
	for (name_cnt = 0; name_cnt < Number_Of_Names; name_cnt++) {
		// Ende wenn Name zu kurz oder lang
		if (strlen(argv[name_cnt+idxstart]) < 2) clean_exit (STAT_TO_SHORT);
		if (strlen(argv[name_cnt+idxstart]) >= BUFFER_WORD_SIZE) clean_exit (STAT_TO_LONG);
		// Länge paßt... Namen in Names_List[].name_norm kopieren
		strcpy(Names_List[name_cnt].name_norm, argv[name_cnt + idxstart]);
		// Minusname name, is_minusname setzten, name_low setzten, Rest leer
		if (Names_List[name_cnt].name_norm[0] == '_') {
			letter_cnt=1;
			while (Names_List[name_cnt].name_norm[letter_cnt] != '\0') {
				Names_List[name_cnt].name_norm[letter_cnt-1] = Names_List[name_cnt].name_norm[letter_cnt];
				letter_cnt++;
			} 
			Names_List[name_cnt].name_norm[letter_cnt-1]='\0';
			Names_List[name_cnt].is_minusname = true;
			strcpy(Names_List[name_cnt].name_lower, Names_List[name_cnt].name_norm);
			to_lower(Names_List[name_cnt].name_lower);
		} 
		// Bei Normalnamen alles füllen
		else {
			strcpy(Names_List[name_cnt].name_lower, Names_List[name_cnt].name_norm);
			to_lower(Names_List[name_cnt].name_lower);
			Names_List[name_cnt].is_minusname = false;
			if (Phone_Ops.f == true) 
			Names_List[name_cnt].p_color = color_table[color_cnt % NUMBER_OF_COLORS];
			color_cnt++;
			only_minusname=false;
			// Immer alle Codes erzeugen, egal welche per Parameter gewählt
			phoneconvert_cologne(Names_List[name_cnt].name_norm, Names_List[name_cnt].code_k);
			phoneconvert_phonem(Names_List[name_cnt].name_norm, Names_List[name_cnt].code_p);
			phoneconvert_soundex(Names_List[name_cnt].name_norm, Names_List[name_cnt].code_s);
			phoneconvert_exsoundex(Names_List[name_cnt].name_norm, Names_List[name_cnt].code_e);
			// Je nach gewählten Optionen Ende wenn Name ungültig/keinen Code erzeugt
			if (((Phone_Ops.k == true) && (Names_List[name_cnt].code_k[0] == '\0')) ||
				((Phone_Ops.p == true) && (Names_List[name_cnt].code_p[0] == '\0')) ||
				((Phone_Ops.s == true) && (Names_List[name_cnt].code_s[0] == '\0')) ||
				((Phone_Ops.e == true) && (Names_List[name_cnt].code_e[0] == '\0'))) {
					clean_exit (STAT_NO_CODE);
			}
		}
	}
			
	// Wenn Namensliste NUR aus Minusnamen besteht = Fehler
	if (only_minusname == true) clean_exit (STAT_PARAM_ERR);
	
	// Datenausgabe je nach gewähltem Anzeigemodus
	// Wenn mehrere übergeben, gewinnt der mit mehr Textausgabe
	// Funktionen liefern true bei Fund, false wenn Keiner
	bool stat;
	if      (outops.a == true) stat = print_mode_a();
	else if (outops.z == true) stat = print_mode_z();
	else  stat = print_mode_w();


	if (stat == true) clean_exit(STAT_FOUND);
	else clean_exit(STAT_NO_FOUND);

	// Wird nie erreicht
	return STAT_MEM_ERR;
}
