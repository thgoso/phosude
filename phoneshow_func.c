/*
Spezialfunktionen phonetische Suche in Text

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



//Levenshtein Dist.
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
//Puffergröße beim lesen von stdin
#define READBUFFER_LEN 1024



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Private Funktionen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Zeigt die Legende zur Suche an: Namenliste & Parameter aus Programmaufruf zur Suche sowie Fundanzahl
// Wird von outmode_a, outmode_z, outmode_w aufgerufen
static void show_caption (nameinfo_t names_list[], int number_of_names, options_t options, long foundcount)
{
	int	cnt;
	int	nolist=0;

	if (options.f == OP_ON) printf("\033[7m<<<<<< LEGENDE >>>>>>\033[m\n");
	else printf("<<<<<< LEGENDE >>>>>>\n");

	//Suchnamen
	printf("Suche: ");
	for (cnt = 0; cnt < number_of_names; cnt++) {
		if (names_list[cnt].Is_Minusname == OP_OFF) {
			if (options.f == OP_ON) printf("%s%s\033[m ", names_list[cnt].Colorstring, names_list[cnt].Name);
			else printf("%s ", names_list[cnt].Name);
		}
		else nolist=1;
	}

	//Verbotene Namen
	if (nolist == 1) {
		printf("\nnicht: ");
		for (cnt = 0; cnt < number_of_names; cnt++) {
			if (names_list[cnt].Is_Minusname == OP_ON) printf("%s ", names_list[cnt].Name);
		}
	}

	//Optionen
	if (options.k == OP_OFF && options.p == OP_OFF && options.s == OP_OFF && options.e == OP_OFF && options.l == 0) {
		printf ("\nSuche: Einfache Textsuche");
	}
	else if (options.k == OP_OFF && options.p == OP_OFF && options.s == OP_OFF && options.e == OP_OFF && options.l != 0) {
		printf ("\nTyp:   Textsuche mit Levenshtein-Distanz von nicht mehr als %d", options.l);
	}
	else {
		printf("\nTyp:   Phonetische Suche ");
		if (options.k == OP_ON) printf ("(Kölner Phonetik) ");
		if (options.p == OP_ON) printf ("(Phonem) ");
		if (options.s == OP_ON) printf ("(Soundex) ");
		if (options.e == OP_ON) printf ("(Extended Soundex) ");
		if (options.l != 0) printf ("\n       mit Levenshtein-Distanz von nicht mehr als %d im phonetischen Code", options.l);
	}

	//Funde
	printf("\nFunde: %ld\n", foundcount);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Levenshtein-Distanz aus http://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
// Berechnet die Levenshtein-Distanz zweier übergebener Strings
// Was in dem Fall auf das Wort direkt oder den erzeugten phonetischen Code angewandt werden kann
// Übergabe:	Wort1 und Wort2
// Rückgabe:	Distanz als Integer
static int levenshtein(char *s1, char *s2)
{
	unsigned int x, y, s1len, s2len;
	s1len = strlen(s1);
	s2len = strlen(s2);
	unsigned int matrix[s2len+1][s1len+1];
	matrix[0][0] = 0;
	for (x = 1; x <= s2len; x++)
		matrix[x][0] = matrix[x-1][0] + 1;
	for (y = 1; y <= s1len; y++)
		matrix[0][y] = matrix[0][y-1] + 1;
	for (x = 1; x <= s2len; x++)
		for (y = 1; y <= s1len; y++)
			matrix[x][y] = MIN3(matrix[x-1][y] + 1, matrix[x][y-1] + 1, matrix[x-1][y-1] + (s1[y-1] == s2[x-1] ? 0 : 1));

	return(matrix[s2len][s1len]);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Prüft ob übergebener Name mit übergebenem Wort identisch ist (je nach phon. Opt)
// Übergabe:	Name
//				Wort
//				Optionen zur Phonetik
// Rückgabe:	TEXT_OK wenn mind. eine Übereinstimmung in einer der phon. Optionen
static int check_phone_equal (nameinfo_t name, char *word, options_t options)
{
	char	sw[MAX_WORD_LEN];
	char	sc[MAX_WORD_LEN];
	char	sww[MAX_WORD_LEN];
	int		lvst_dist;

	// Wenn keine phonetischen Optionen gewählt
	// Nur Lev Dist auf Name & Wort anwenden
	if (options.k == OP_OFF && options.p == OP_OFF && options.s == OP_OFF && options.e == OP_OFF) {
		strcpy (sw, name.Name);
		strcpy (sww, word);
		conv_to_lower (sw);
		conv_to_lower (sww);
		lvst_dist = levenshtein (sw, sww);
		if (lvst_dist <= options.l) return (TEXT_OK);
		else return (TEXT_ERROR);
	}

	// Phon Opt gewählt, also Wort in phone Code wandeln
	// mit jeweiligen phon. Code des Namens vergleichen
	strcpy (sw, word);
	conv_to_lower (sw);
	conv_uml (sw);

	if (options.k == OP_ON) {
		conv_to_cologne (sw, sc);
		if (options.l != 0) {
			lvst_dist = levenshtein (sc, name.Code_k);
			if (lvst_dist <= options.l) return (TEXT_OK);
		}
		else if (strcmp (sc, name.Code_k) == 0) return (TEXT_OK);
	}
	
	if (options.p == OP_ON) {
		conv_to_phonem (sw, sc);
		if (options.l != 0) {
			lvst_dist = levenshtein (sc, name.Code_p);
			if (lvst_dist <= options.l) return (TEXT_OK);
		}
		else if (strcmp (sc, name.Code_p) == 0) return (TEXT_OK);
	}
	
	if (options.s == OP_ON) {
		conv_to_soundex (sw, sc);
		if (options.l != 0) {
			lvst_dist = levenshtein (sc, name.Code_s);
			if (lvst_dist <= options.l) return (TEXT_OK);
		}
		else if (strcmp (sc, name.Code_s) == 0) return (TEXT_OK);
	}
	
	if (options.e == OP_ON) {
		conv_to_exsoundex (sw, sc);
		if (options.l != 0) {
			lvst_dist = levenshtein (sc, name.Code_e);
			if (lvst_dist <= options.l) return (TEXT_OK);
		}
		else if (strcmp (sc, name.Code_e) == 0) return (TEXT_OK);
	}

	return (TEXT_ERROR);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++













// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Öffentliche Funktionen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Liest Daten von stdin ein und gibt Pointer darauf zurück.
// Bei Problemen mit Speicheranforderung wird NULL zurückgegeben
char *read_stdin(void)
{
	char	buffer[READBUFFER_LEN];
	size_t	data_len = 1;
	char	*data = malloc(sizeof(char) * READBUFFER_LEN);

	if (data == NULL) return (NULL);

	// Über Puffer Eingabe von stdin lesen und komplett in daten speichern
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
// Holt das nächste gültige Wort aus "text_data" und gibt TEXT_OK zurück
// Falls als nächstes kein Wort sondern ein "verbotenes" / nicht deutsches Zeichen kommt
// Wird das Zeichen allein zurückgegeben (plus Nullbyte) und Rückgabe = TEXT_ERROR
// Ist das Datenende mal erreicht wird immer wieder '\0' in word zurückgegeben
// Übergabe:	Pointer auf Textdaten
// Rückgabe:	Wort
//				TEXT_OK oder TEXT_ERROR
//				aktualisiert auch txtposcnt wenn kein NULL-Zeiger übergeben
int get_word (char *text_data, char *word, long *txtposcnt)
{
	const uml_t		ul_list[] = {{"Ä"}, {"Ö"}, {"Ü"}, {"ä"}, {"ö"}, {"ü"}, {"ß"}};
	static long		cnt=0;
	int				gd;
	char			tmp[4];
	int				ulcnt;

	word[0]='\0';
	// text_data Ende erreicht
	if (text_data[cnt] == '\0') return (TEXT_ERROR);

	while (1) {
		// Erlaubtes Zeichen an "word" anfügen (A-Z, a-z)
		if ((text_data[cnt] >= 'a' && text_data[cnt] <= 'z') || (text_data[cnt] >= 'A' && text_data[cnt] <= 'Z')) {
			tmp[0]=text_data[cnt];
			tmp[1]='\0';
			strcat(word, tmp);
			cnt++;
			continue;
		}
		// Erlaubtes DoppelbyteZeichen an "word" anfügen (Umlaute)
		gd=0;
		for (ulcnt=0; ulcnt<= 6; ulcnt++) {
			strcpy (tmp, ul_list[ulcnt]);
			if (text_data[cnt] == tmp[0] && text_data[cnt + 1] == tmp[1]) {
				strcat(word, tmp);
				cnt+=2;
				gd=1;
				break;
			}
		}
		// Weiter wenn gültiges Zeichen war
		if (gd == 1) continue;
		else break;
	}
	
	// Verbotenes Zeichen angetroffen
	// aber Wort noch nicht zurückgegeben
	if (word[0] != '\0') {
		if (txtposcnt != NULL) *txtposcnt = cnt;
		return (TEXT_OK);
	}
	
	// einzelnes Zeichen
	else {
		word[0] = text_data[cnt];
		word[1] = '\0';
		if (txtposcnt != NULL) *txtposcnt = cnt;
		cnt++;
		return (TEXT_ERROR);
	}
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Namen überprüfen ob gültige deutsche Worte,
// Ob sie nach den gewählten Optionen phon. Code generieren
// Ob sie Länger als 1 Buchstabe sind
// Bei Problemen wird Name/Option in stderr ausgegeben
// In Ergänzung zu "zeige_fehler" ist die Ausgabe dann passend
// Gibt Fehlercode zurück oder "TEXT_OK"

// Übergabe:	Namen Liste (Namen müssen ausgefüllt sein)
//				Anzahl Namen in Liste
//				Phonetik Optionen
// Rückgabe:	TEXT_OK oder entsprechender Fehlercode
//				In der Namen Liste werden die phon. Codes zum Namen "eingetragen"
//				und der Colorstring für den Namen (wenn opt.f = OP_OFF) Colorstring = '\0'

int check_names_list (nameinfo_t names_list[], int number_of_names, options_t options)
{
	int		cnt;
	int		colcnt=0;
	int		only_minusname=1;
	char	txt[MAX_WORD_LEN];

	// Farbstrings
	typedef char color_t[COLORSTRING_LEN];
	const int number_of_colors = 28;
	const color_t colors[] =	{{"\033[1;37m\033[41m"}, {"\033[1;37m\033[42m"},
								{"\033[1;37m\033[43m"}, {"\033[1;37m\033[44m"}, {"\033[1;37m\033[45m"},
								{"\033[1;37m\033[46m"}, {"\033[1;37m\033[47m"}, {"\033[1;33m\033[41m"},
								{"\033[1;33m\033[42m"}, {"\033[1;33m\033[43m"}, {"\033[1;33m\033[44m"},
								{"\033[1;33m\033[45m"}, {"\033[1;33m\033[46m"}, {"\033[1;33m\033[47m"},
								{"\033[1;36m\033[41m"}, {"\033[1;36m\033[42m"}, {"\033[1;36m\033[43m"},
								{"\033[1;36m\033[44m"}, {"\033[1;36m\033[45m"}, {"\033[1;36m\033[46m"},
								{"\033[1;36m\033[47m"}, {"\033[1;32m\033[41m"}, {"\033[1;32m\033[42m"},
								{"\033[1;32m\033[43m"}, {"\033[1;32m\033[44m"}, {"\033[1;32m\033[45m"},
								{"\033[1;32m\033[46m"}, {"\033[1;32m\033[47m"}};



	for (cnt = 0; cnt < number_of_names; cnt++) {
		strcpy(txt, names_list[cnt].Name);
		
		// Bei Minusname "_" entfernen und Ist_Minusname = OP_ON
		// Keine phon. Codes erzeugen, keine Farbe erzeugen
		// Normale SuchNamen Ist_Minusname = OP_OFF setzten
		// phon. Codes erzeugen, Colorstring erzeugen
		if (txt[0] == '_') {
			kill_onebytechars (txt, '_');
			strcpy(names_list[cnt].Name, txt);
			names_list[cnt].Is_Minusname = OP_ON;
		}
		else {
			names_list[cnt].Is_Minusname = OP_OFF;
			if (options.f == OP_ON) strcpy (names_list[cnt].Colorstring, colors[colcnt % number_of_colors]);
			else names_list[cnt].Colorstring[0] = '\0';
			colcnt++;
			only_minusname=0;
		}
		
		// Länge < 2 = Fehler
		conv_to_lower (txt);
		conv_uml (txt);
		if (strlen (txt) < 2) {
			fprintf (stderr, "%s ", txt);
			return (TEXT_TO_SHORT);
		}
		
		// Nicht deutsches Wort = Fehler
		if (check_word_german (txt) == TEXT_ERROR) {
			fprintf (stderr, "%s ", txt);
			return (TEXT_ERROR);
		}
		
		// Codes zum Namen erstellen je nach Optionen Phonetik
		// Fehler wenn ungültiger Code
		if ((options.k == OP_ON) && (names_list[cnt].Is_Minusname == OP_OFF)) {
			conv_to_cologne (txt, names_list[cnt].Code_k);
			if (strlen (names_list[cnt].Code_k) < 1) {
				fprintf (stderr, "-k %s ", names_list[cnt].Name);
				return (TEXT_NO_CODE);
			}
		}
		
		if ((options.p == OP_ON) && (names_list[cnt].Is_Minusname == OP_OFF)) {
			conv_to_phonem (txt, names_list[cnt].Code_p);
			if (strlen (names_list[cnt].Code_p) < 1) {
				fprintf (stderr, "-p %s ", names_list[cnt].Name);
				return (TEXT_NO_CODE);
			}
		}
		
		if ((options.s == OP_ON) && (names_list[cnt].Is_Minusname == OP_OFF)) {
			conv_to_soundex (txt, names_list[cnt].Code_s);
			if (strlen (names_list[cnt].Code_s) < 1) {
				fprintf (stderr, "-s %s ", names_list[cnt].Name);
				return (TEXT_NO_CODE);
			}
		}
		
		if ((options.e == OP_ON) && (names_list[cnt].Is_Minusname == OP_OFF)) {
			conv_to_exsoundex (txt, names_list[cnt].Code_e);
			if (strlen (names_list[cnt].Code_e) < 1) {
				fprintf (stderr, "-e %s ", names_list[cnt].Name);
				return (TEXT_NO_CODE);
			}
		}
	}
	
	// Wenn Namensliste NUR aus Minusnamen besteht
	if (only_minusname == 1) return (TEXT_UNDEF);
	else return (TEXT_OK);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabe Option -a = ganzen Text ausgeben
// Übergabe:	Namenliste
//				Anzahl Namen in Liste
//				Optionen
//				Pointer auf Textdaten
// Rückgabe:	TEXT_OK wenn mind. ein Fund, wenn gar kein Fund TEXT_NOT_EQUAL
int outmode_a (nameinfo_t names_list[], int number_of_names, options_t options, char *text_data)
{
	char	word[MAX_WORD_LEN];
	char	line[MAX_LINE_LEN]="\0";
	int		namecount;
	int		forbidden;
	int		retval;
	long	foundcount=0;
	long	linecount=1;

	while (1) {
		// Nächstes Wort abholen
		retval = get_word (text_data, word, NULL);
		// Wenn Textende erreichet Ende
		if (word[0] == '\0') break;
		// Wenn Zeilenende erreicht line ausgeben
		else if (word[0] == '\n') {
			if (options.n == OP_ON) printf ("%ld %ld %s\n", linecount, foundcount, line);
			else printf ("%s\n", line);
			line[0] = '\0';
			linecount++;
		}
		// anderes Sonderzeichen
		else if (retval == TEXT_ERROR) strcat (line, word);

		// Wenn Wort
		else if (retval == TEXT_OK) {
			// Wenn Wort eines der verbotenen nicht weiter prüfen aber ausgeben
			forbidden=0;
			for (namecount=0; namecount < number_of_names; namecount++) {
				if (names_list[namecount].Is_Minusname == OP_ON) {
					if (strcmp (word, names_list[namecount].Name) == 0) {
						forbidden = 1;
						break;
					}
				}
			}
			if (forbidden == 1) {
				strcat (line, word);
				continue;
			}
			// Kein verbotenes Wort
			for (namecount=0; namecount < number_of_names; namecount++) {
				//Name = verbotener Name, nicht prüfen
				if (names_list[namecount].Is_Minusname == OP_ON) continue;
				//Prüfen ob phongleich
				if (check_phone_equal (names_list[namecount], word, options) == TEXT_OK) {
					//Colorstring in line schieben
					strcat (line, names_list[namecount].Colorstring);
					foundcount++;
					break;
				}
			}
			//Wort an line anfügen, Farbausgabe aus
			strcat (line, word);
			if (options.f == OP_ON) strcat (line, "\033[m");
			continue;
		}
	}

	//Legende ausgeben
	show_caption (names_list, number_of_names, options, foundcount);

	if (foundcount != 0) return (TEXT_OK);
	else return (TEXT_NOT_EQUAL);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabe Option -z = nur Zeilen ausgeben
// Übergabe:	Namenliste
//				Anzahl Namen in Liste
//				Optionen
//				Pointer auf Textdaten
// Rückgabe:	TEXT_OK wenn mind. ein Fund, wenn gar kein Fund TEXT_NOT_EQUAL
int outmode_z (nameinfo_t names_list[], int number_of_names, options_t options, char *text_data)
{
	char	word[MAX_WORD_LEN];
	char	line[MAX_LINE_LEN]="\0";
	int		namecount;
	int		forbidden;
	int		retval;
	long	foundcount=0;
	long	linecount=1;
	int		isinline=0;

	while (1) {
		// Nächstes Wort abholen
		retval = get_word (text_data, word, NULL);
		// Wenn Textende erreichet Ende
		if (word[0] == '\0') break;
		// Wenn Zeilenende erreicht line ausgeben
		else if (word[0] == '\n') {
			// Aber nur wenn Fund in Zeile
			if (isinline == 1) {
				if (options.n == OP_ON) printf ("%ld %ld %s\n", linecount, foundcount, line);
				else printf ("%s\n", line);
				isinline = 0;
			}
			// Zeile leeren
			line[0] = '\0';
			linecount++;
		}
		// anderes Sonderzeichen
		else if (retval == TEXT_ERROR) strcat (line, word);

		// Wenn Wort
		else if (retval == TEXT_OK) {
			// Wenn Wort eines der verbotenen nicht weiter prüfen aber ausgeben
			forbidden=0;
			for (namecount=0; namecount < number_of_names; namecount++) {
				if (names_list[namecount].Is_Minusname == OP_ON) {
					if (strcmp (word, names_list[namecount].Name) == 0) {
						forbidden = 1;
						break;
					}
				}
			}
			if (forbidden == 1) {
				strcat (line, word);
				continue;
			}
			// Kein verbotenes Wort
			for (namecount=0; namecount < number_of_names; namecount++) {
				// Name = verbotener Name, nicht prüfen
				if (names_list[namecount].Is_Minusname == OP_ON) continue;
				// Prüfen ob phongleich
				if (check_phone_equal (names_list[namecount], word, options) == TEXT_OK) {
					// Colorstring in line schieben
					strcat (line, names_list[namecount].Colorstring);
					foundcount++;
					isinline=1;
					break;
				}
			}
			// Wort an line anfügen, Farbausgabe aus
			strcat (line, word);
			if (options.f == OP_ON) strcat (line, "\033[m");
			continue;
		}
	}

	// Legende ausgeben
	show_caption (names_list, number_of_names, options, foundcount);

	if (foundcount != 0) return (TEXT_OK);
	else return (TEXT_NOT_EQUAL);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabe Option -w = nur Worte ausgeben
// Übergabe:	Namenliste
//				Anzahl Namen in Liste
//				Optionen
//				Pointer auf Textdaten
// Rückgabe:	TEXT_OK wenn mind. ein Fund, wenn gar kein Fund TEXT_NOT_EQUAL
int outmode_w (nameinfo_t names_list[], int number_of_names, options_t options, char *text_data)
{
	char	word[MAX_WORD_LEN];
	int		namecount;
	int		retval;
	int		forbidden;
	long	foundcount=0;
	long	linecount=1;

	while (1) {
		// Nächstes Wort abholen
		retval = get_word (text_data, word, NULL);
		// Wenn Textende erreichet Ende
		if (word[0] == '\0') break;
		// Wenn Zeilenende
		if (word[0] == '\n') linecount++;
		// Wenn Wort
		else if (retval == TEXT_OK) {
			// Wenn Wort eines der verbotenen nicht weiter prüfen
			forbidden=0;
			for (namecount=0; namecount < number_of_names; namecount++) {
				if (names_list[namecount].Is_Minusname == OP_ON) {
					if (strcmp (word, names_list[namecount].Name) == 0) {
						forbidden = 1;
						break;
					}
				}
			}
			if (forbidden == 1) continue;
			// Kein verbotenes Wort
			for (namecount=0; namecount < number_of_names; namecount++) {
				// Name = verbotener Name, nicht prüfen
				if (names_list[namecount].Is_Minusname == OP_ON) continue;
				// Prüfen ob phongleich
				if (check_phone_equal (names_list[namecount], word, options) == TEXT_OK) {
					// Farbig ausgeben / oder nicht
					foundcount++;
					if (options.n == OP_ON) {
						if (options.f == OP_ON) printf ("%ld %ld %s%s\033[m\n",
										linecount, foundcount, names_list[namecount].Colorstring, word);
						else printf ("%ld %ld %s\n", linecount, foundcount, word);
					}
					else {
						if (options.f == OP_ON) printf ("%s%s\033[m\n",
										names_list[namecount].Colorstring, word);
						else printf("%s\n", word);
					}
					break;
				}
			}
		}
	}
	// Legende ausgeben
	show_caption (names_list, number_of_names, options, foundcount);

	if (foundcount != 0) return (TEXT_OK);
	else return (TEXT_NOT_EQUAL);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabe Option -c = keine Textausgabe, nur Zählen und Anzahl Funde ausgeben
// Übergabe:	Namenliste
// 				Anzahl Namen in Liste
// 				Optionen
// 				Pointer auf Textdaten
// Rückgabe:	TEXT_OK wenn mind. ein Fund, wenn gar kein Fund TEXT_NOT_EQUAL
int outmode_c (nameinfo_t names_list[], int number_of_names, options_t options, char *text_data)
{
	char	word[MAX_WORD_LEN];
	int		namecount;
	int		forbidden;
	int		retval;
	long	foundcount=0;

	while (1) {
		// Nächstes Wort abholen
		retval = get_word (text_data, word, NULL);
		// Wenn Textende erreichet Ende
		if (word[0] == '\0') break;
		// Wenn Wort
		else if (retval == TEXT_OK) {
			// Wenn Wort eines der verbotenen nicht weiter prüfen
			forbidden=0;
			for (namecount=0; namecount < number_of_names; namecount++) {
				if (names_list[namecount].Is_Minusname == OP_ON) {
					if (strcmp (word, names_list[namecount].Name) == 0) {
						forbidden = 1;
						break;
					}
				}
			}
			if (forbidden == 1) continue;
			// Kein verbotenes Wort
			for (namecount=0; namecount < number_of_names; namecount++) {
				// Name = verbotener Name, nicht prüfen
				if (names_list[namecount].Is_Minusname == OP_ON) continue;
				// Prüfen ob phongleich
				if (check_phone_equal (names_list[namecount], word, options) == TEXT_OK) {
					// nur zählen
					foundcount++;
					break;
				}
			}
		}
	}
	
	// nur Anzahl ausgeben
	printf ("%ld\n", foundcount);
	if (foundcount != 0) return (TEXT_OK);
	else return (TEXT_NOT_EQUAL);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabe Option -x = Ausgegeben wird für jeden Fund die Startposition im String und die Stringlänge
// (beides in char "gemessen") aus den von stdin gelieferten Daten
// Übergabe:	Namenliste
// 				Anzahl Namen in Liste
// 				Optionen
// 				Pointer auf Textdaten
// Rückgabe:	TEXT_OK wenn mind. ein Fund, wenn gar kein Fund TEXT_NOT_EQUAL
int outmode_x (nameinfo_t names_list[], int number_of_names, options_t options, char *text_data)
{
	char	word[MAX_WORD_LEN];
	int		namecount;
	int		retval;
	int		forbidden;
	int		len;
	long	foundcount=0;
	long	txtpos;

	while (1) {
		// Nächstes Wort abholen
		retval = get_word (text_data, word, &txtpos);
		// Wenn Textende erreichet Ende
		if (word[0] == '\0') break;
		// Wenn Wort
		else if (retval == TEXT_OK) {
			// Wenn Wort eines der verbotenen nicht weiter prüfen
			forbidden=0;
			for (namecount=0; namecount < number_of_names; namecount++) {
				if (names_list[namecount].Is_Minusname == OP_ON) {
					if (strcmp (word, names_list[namecount].Name) == 0) {
						forbidden = 1;
						break;
					}
				}
			}
			if (forbidden == 1) continue;
			// Kein verbotenes Wort
			for (namecount=0; namecount < number_of_names; namecount++) {
				// Name = verbotener Name, nicht prüfen
				if (names_list[namecount].Is_Minusname == OP_ON) continue;
				// Prüfen ob phongleich
				if (check_phone_equal (names_list[namecount], word, options) == TEXT_OK) {
					// nur zählen
					foundcount++;
					len = strlen(word);
					printf ("%ld %d\n", txtpos - len, len);
					break;
				}
			}
		}
	}

	if (foundcount != 0) return (TEXT_OK);
	else return (TEXT_NOT_EQUAL);
}



