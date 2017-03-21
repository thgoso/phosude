/*
Ausgabefunktionen zu main_phoneshow-de

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


//Levenshtein Dist.
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
// Max Zeilenlänge
#define BUFFER_LINE_SIZE		4096
// Für Umlaute
typedef char uml_t[3];



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Private Funktionen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Zeigt die Legende zur Suche an: Namenliste & Parameter aus Programmaufruf zur Suche sowie Fundanzahl
// Wird von outmode_a, outmode_z, outmode_w aufgerufen
static void show_caption (const nameinfo_t names_list[], const int number_of_names, const options_t options, const long foundcount)
{
	int	cnt;
	int	nolist=0;

	if (options.f == true) printf("\033[7m<<<<<< LEGENDE >>>>>>\033[m\n");
	else printf("<<<<<< LEGENDE >>>>>>\n");

	//Suchnamen
	printf("Suche: ");
	for (cnt = 0; cnt < number_of_names; cnt++) {
		if (names_list[cnt].Is_Minusname == false) {
			if (options.f == true) printf("%s%s\033[m ", names_list[cnt].Colorstring, names_list[cnt].Name);
			else printf("%s ", names_list[cnt].Name);
		}
		else nolist=1;
	}

	//Verbotene Namen
	if (nolist == 1) {
		printf("\nnicht: ");
		for (cnt = 0; cnt < number_of_names; cnt++) {
			if (names_list[cnt].Is_Minusname == true) printf("%s ", names_list[cnt].Name);
		}
	}

	//Optionen
	if (options.k == false && options.p == false && options.s == false && options.e == false && options.l == 0) {
		printf ("\nSuche: Einfache Textsuche");
	}
	else if (options.k == false && options.p == false && options.s == false && options.e == false && options.l != 0) {
		printf ("\nTyp:   Textsuche mit Levenshtein-Distanz von nicht mehr als %d", options.l);
	}
	else {
		printf("\nTyp:   Phonetische Suche ");
		if (options.k == true) printf ("(Kölner Phonetik) ");
		if (options.p == true) printf ("(Phonem) ");
		if (options.s == true) printf ("(Soundex) ");
		if (options.e == true) printf ("(Extended Soundex) ");
		if (options.l != 0) printf ("\n       mit Levenshtein-Distanz von nicht mehr als %d im phonetischen Code", options.l);
	}

	//Funde
	printf("\nFunde: %ld\n", foundcount);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Wandelt alle Großbuchstaben im String in Kleinbuchstaben (A-Z), Umlaute
static void to_lower (char *text)
{
	int			cnt=0;
	char		letter[3];
	int			ulcnt;
	const uml_t	ul_list[]     = {{"Ä"},{"Ö"},{"Ü"}};
	const uml_t	ul_rep_list[] = {{"ä"},{"ö"},{"ü"}};

	while (text[cnt] != '\0') {
		// A-Z
		if (text[cnt] >= 'A' && text[cnt] <= 'Z') {
			text[cnt] += 32;
			cnt++;
			continue;
		}
		// Große Umlaute 0-2
		for (ulcnt = 0; ulcnt <= 2; ulcnt++) {
			strcpy(letter, ul_list[ulcnt]);
			if (text[cnt] == letter[0] && text[cnt + 1] == letter[1]) {
				strcpy(letter, ul_rep_list[ulcnt]);
				text[cnt] = letter[0];
				text[cnt + 1] = letter[1];
				cnt++;
				break;
			}
		}
		cnt++;
	}
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Levenshtein-Distanz aus http://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
// Berechnet die Levenshtein-Distanz zweier übergebener Strings
// Was in dem Fall auf das Wort direkt oder den erzeugten phonetischen Code angewandt werden kann
// Übergabe:	Wort1 und Wort2
// Rückgabe:	Distanz als Integer
// Es wird KEINE Groß- Kleinschreibung beachtet (NAME == name)
static int levenshtein(const char *word1, const char *word2)
{
	char s1[BUFFER_NAME_SIZE];
	char s2[BUFFER_NAME_SIZE];
	strcpy(s1, word1);
	strcpy(s2, word2);
	to_lower(s1);
	to_lower(s2);
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

	return (matrix[s2len][s1len]);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Prüft ob übergebener Name mit übergebenem Wort identisch ist (je nach phon. Opt)
// Übergabe:	Name
//				Wort
//				Optionen zur Phonetik
// Rückgabe:	true wenn mind. eine Übereinstimmung in einer der phon. Optionen
static bool check_phone_equal (const nameinfo_t name, const char *word, const options_t options)
{
	int		lvst_dist;
	char	code[BUFFER_CODE_SIZE];

	// Wenn keine phonetischen Optionen gewählt
	// Nur Lev Dist auf Name & Wort anwenden
	if (options.k == false && options.p == false && options.s == false && options.e == false) {
		lvst_dist = levenshtein(name.Name, word);
		if (lvst_dist <= options.l) return true;
		else return false;
	}

	// Phon Opt gewählt, also Wort in phone Code wandeln
	// mit jeweiligen phon. Code des Namens vergleichen
	if (options.k == true) {
		phoneconvert(word, code, CONV_MODE_COLOGNE);
		if (options.l != 0) {
			lvst_dist = levenshtein (code, name.Code_k);
			if (lvst_dist <= options.l) return true;
		}
		else if (strcmp (code, name.Code_k) == 0) return true;
	}
	
	if (options.p == true) {
		phoneconvert(word, code, CONV_MODE_PHONEM);
		if (options.l != 0) {
			lvst_dist = levenshtein (code, name.Code_p);
			if (lvst_dist <= options.l) return true;
		}
		else if (strcmp (code, name.Code_p) == 0) return true;
	}
	
	if (options.s == true) {
		phoneconvert(word, code, CONV_MODE_SOUNDEX);
		if (options.l != 0) {
			lvst_dist = levenshtein (code, name.Code_s);
			if (lvst_dist <= options.l) return true;
		}
		else if (strcmp (code, name.Code_s) == 0) return true;
	}
	
	if (options.e == true) {
		phoneconvert(word, code, CONV_MODE_EXSOUNDEX);
		if (options.l != 0) {
			lvst_dist = levenshtein (code, name.Code_e);
			if (lvst_dist <= options.l) return true;
		}
		else if (strcmp (code, name.Code_e) == 0) return true;
	}

	return false;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Holt das nächste gültige Wort aus "text_data" und gibt true zurück
// Falls als nächstes kein Wort sondern ein "verbotenes" / nicht deutsches Zeichen kommt
// Wird das Zeichen allein zurückgegeben (plus Nullbyte) und Rückgabe = false
// Ist das Datenende mal erreicht wird immer wieder '\0' in word zurückgegeben
// Übergabe:	Pointer auf Textdaten
// Rückgabe:	Wort
//				true oder false
//				aktualisiert auch txtposcnt wenn kein NULL-Zeiger übergeben
static bool get_word (const char *text_data, char *word, long *txtposcnt)
{
	const uml_t		ul_list[] = {{"Ä"}, {"Ö"}, {"Ü"}, {"ä"}, {"ö"}, {"ü"}, {"ß"}};
	static long		cnt=0;
	int				gd;
	char			tmp[4];
	int				ulcnt;

	word[0]='\0';
	// text_data Ende erreicht
	if (text_data[cnt] == '\0') return false;

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
		return true;
	}
	
	// einzelnes Zeichen
	else {
		word[0] = text_data[cnt];
		word[1] = '\0';
		if (txtposcnt != NULL) *txtposcnt = cnt;
		cnt++;
		return false;
	}
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	
	











// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Öffentliche Funktionen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabe Option -a = ganzen Text ausgeben
// Übergabe:	Namenliste
//				Anzahl Namen in Liste
//				Optionen
//				Pointer auf Textdaten
// Rückgabe:	true wenn mind. ein Fund, wenn gar kein Fund false
bool outmode_a (const nameinfo_t names_list[], const int number_of_names, const options_t options, const char *text_data)
{
	char	word[BUFFER_NAME_SIZE];
	char	line[BUFFER_LINE_SIZE]="\0";
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
			if (options.n == true) printf ("%ld %ld %s\n", linecount, foundcount, line);
			else printf ("%s\n", line);
			line[0] = '\0';
			linecount++;
		}
		// anderes Sonderzeichen
		else if (retval == false) strcat (line, word);

		// Wenn Wort
		else if (retval == true) {
			// Wenn Wort eines der verbotenen nicht weiter prüfen aber ausgeben
			forbidden=0;
			for (namecount=0; namecount < number_of_names; namecount++) {
				if (names_list[namecount].Is_Minusname == true) {
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
				if (names_list[namecount].Is_Minusname == true) continue;
				//Prüfen ob phongleich
				if (check_phone_equal (names_list[namecount], word, options) == true) {
					//Colorstring in line schieben
					strcat (line, names_list[namecount].Colorstring);
					foundcount++;
					break;
				}
			}
			//Wort an line anfügen, Farbausgabe aus
			strcat (line, word);
			if (options.f == true) strcat (line, "\033[m");
			continue;
		}
	}

	//Legende ausgeben
	show_caption (names_list, number_of_names, options, foundcount);

	if (foundcount != 0) return true;
	else return false;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabe Option -z = nur Zeilen ausgeben
// Übergabe:	Namenliste
//				Anzahl Namen in Liste
//				Optionen
//				Pointer auf Textdaten
// Rückgabe:	true wenn mind. ein Fund, wenn gar kein Fund TEXT_NOT_EQUAL
bool outmode_z (const nameinfo_t names_list[], const int number_of_names, const options_t options, const char *text_data)
{
	char	word[BUFFER_NAME_SIZE];
	char	line[BUFFER_LINE_SIZE]="\0";
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
				if (options.n == true) printf ("%ld %ld %s\n", linecount, foundcount, line);
				else printf ("%s\n", line);
				isinline = 0;
			}
			// Zeile leeren
			line[0] = '\0';
			linecount++;
		}
		// anderes Sonderzeichen
		else if (retval == false) strcat (line, word);

		// Wenn Wort
		else if (retval == true) {
			// Wenn Wort eines der verbotenen nicht weiter prüfen aber ausgeben
			forbidden=0;
			for (namecount=0; namecount < number_of_names; namecount++) {
				if (names_list[namecount].Is_Minusname == true) {
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
				if (names_list[namecount].Is_Minusname == true) continue;
				// Prüfen ob phongleich
				if (check_phone_equal (names_list[namecount], word, options) == true) {
					// Colorstring in line schieben
					strcat (line, names_list[namecount].Colorstring);
					foundcount++;
					isinline=1;
					break;
				}
			}
			// Wort an line anfügen, Farbausgabe aus
			strcat (line, word);
			if (options.f == true) strcat (line, "\033[m");
			continue;
		}
	}

	// Legende ausgeben
	show_caption (names_list, number_of_names, options, foundcount);

	if (foundcount != 0) return true;
	else return false;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabe Option -w = nur Worte ausgeben
// Übergabe:	Namenliste
//				Anzahl Namen in Liste
//				Optionen
//				Pointer auf Textdaten
// Rückgabe:	true wenn mind. ein Fund, wenn gar kein Fund TEXT_NOT_EQUAL
bool outmode_w (const nameinfo_t names_list[], const int number_of_names, const options_t options, const char *text_data)
{
	char	word[BUFFER_NAME_SIZE];
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
		else if (retval == true) {
			// Wenn Wort eines der verbotenen nicht weiter prüfen
			forbidden=0;
			for (namecount=0; namecount < number_of_names; namecount++) {
				if (names_list[namecount].Is_Minusname == true) {
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
				if (names_list[namecount].Is_Minusname == true) continue;
				// Prüfen ob phongleich
				if (check_phone_equal (names_list[namecount], word, options) == true) {
					// Farbig ausgeben / oder nicht
					foundcount++;
					if (options.n == true) {
						if (options.f == true) printf ("%ld %ld %s%s\033[m\n",
										linecount, foundcount, names_list[namecount].Colorstring, word);
						else printf ("%ld %ld %s\n", linecount, foundcount, word);
					}
					else {
						if (options.f == true) printf ("%s%s\033[m\n",
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

	if (foundcount != 0) return true;
	else return false;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabe Option -c = keine Textausgabe, nur Zählen und Anzahl Funde ausgeben
// Übergabe:	Namenliste
// 				Anzahl Namen in Liste
// 				Optionen
// 				Pointer auf Textdaten
// Rückgabe:	true wenn mind. ein Fund, wenn gar kein Fund TEXT_NOT_EQUAL
bool outmode_c (const nameinfo_t names_list[], const int number_of_names, const options_t options, const char *text_data)
{
	char	word[BUFFER_NAME_SIZE];
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
		else if (retval == true) {
			// Wenn Wort eines der verbotenen nicht weiter prüfen
			forbidden=0;
			for (namecount=0; namecount < number_of_names; namecount++) {
				if (names_list[namecount].Is_Minusname == true) {
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
				if (names_list[namecount].Is_Minusname == true) continue;
				// Prüfen ob phongleich
				if (check_phone_equal (names_list[namecount], word, options) == true) {
					// nur zählen
					foundcount++;
					break;
				}
			}
		}
	}
	
	// nur Anzahl ausgeben
	printf ("%ld\n", foundcount);
	if (foundcount != 0) return true;
	else return false;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabe Option -x = Ausgegeben wird für jeden Fund die Startposition im String und die Stringlänge
// (beides in char "gemessen") aus den von stdin gelieferten Daten
// Übergabe:	Namenliste
// 				Anzahl Namen in Liste
// 				Optionen
// 				Pointer auf Textdaten
// Rückgabe:	true wenn mind. ein Fund, wenn gar kein Fund TEXT_NOT_EQUAL
bool outmode_x (const nameinfo_t names_list[], const int number_of_names, const options_t options, const char *text_data)
{
	char	word[BUFFER_NAME_SIZE];
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
		else if (retval == true) {
			// Wenn Wort eines der verbotenen nicht weiter prüfen
			forbidden=0;
			for (namecount=0; namecount < number_of_names; namecount++) {
				if (names_list[namecount].Is_Minusname == true) {
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
				if (names_list[namecount].Is_Minusname == true) continue;
				// Prüfen ob phongleich
				if (check_phone_equal (names_list[namecount], word, options) == true) {
					// nur zählen
					foundcount++;
					len = strlen(word);
					printf ("%ld %d\n", txtpos - len, len);
					break;
				}
			}
		}
	}

	if (foundcount != 0) return true;
	else return false;
}
