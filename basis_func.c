/*
Basisfunktionen Textkonvertierung und phonetische Verfahren

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
#include <string.h>
#include "basis_func.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Prüft ob Text ein deutsches Wort ist. Das ist es wenn ausschließlich deutsche Buchstaben im String
// enthalten sind NICHTS anderes (Auch kein Space) !!! Falls OK wird "TEXT_OK" zurückgegeben, sonst  "TEXT_ERROR"
int check_word_german (char *text)
{
	int			cnt=0;
	char		letter[3];
	int			ulcnt;
	int			gd;
	const uml_t	ul_list[]={{"Ä"},{"Ö"},{"Ü"},{"ä"},{"ö"},{"ü"},{"ß"}};

	while (text[cnt] != '\0') {
		// A-Z, a-z
		if ((text[cnt] >= 'A' && text[cnt] <= 'Z') || (text[cnt] >= 'a' && text[cnt] <= 'z')) {
			cnt++;
			continue;
		}
		// Umlaute 0-6
		gd=0;
		for (ulcnt=0; ulcnt <=6; ulcnt++) {
			strcpy(letter, ul_list[ulcnt]);
			if (text[cnt] == letter[0] && text[cnt+1] == letter[1]) {
				cnt+=2;
				gd=1;
				break;
			}
		}
		if (gd == 1) continue;
		// Verbotenes Zeichen
		return (TEXT_ERROR);
	}
	return (TEXT_OK);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Konvertiert alle Umlaute in Einzelbuchstaben
void conv_uml (char *text)
{
	int			cnt=0;
	char		letter[3];
	int			ulcnt;
	const uml_t	ul_list[]     = {{"Ä"}, {"Ö"}, {"Ü"}, {"ä"}, {"ö"}, {"ü"}, {"ß"}};
	const uml_t ul_rep_list[] = {{"AE"},{"OE"},{"UE"},{"ae"},{"oe"},{"ue"},{"ss"}};

	while (text[cnt] != '\0') {
		for (ulcnt = 0; ulcnt <= 6; ulcnt++) {
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

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Wandelt alle Kleinbuchstaben im String in Großbuchstaben (a-z), Umlaute, ß wird zu SS
void conv_to_upper (char *text)
{
	int			cnt=0;
	char		letter[3];
	int			ulcnt;
	const uml_t	ul_list[]     = {{"ä"},{"ö"},{"ü"},{"ß"}};
	const uml_t ul_rep_list[] = {{"Ä"},{"Ö"},{"Ü"},{"SS"}};

	while (text[cnt] != '\0') {
		// a-z
		if (text[cnt] >= 'a' && text[cnt] <= 'z') {
			text[cnt] -= 32;
			cnt++;
			continue;
		}
		// kleine Umlaute 0-3
		for (ulcnt = 0; ulcnt <= 3; ulcnt++) {
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

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Wandelt alle Großbuchstaben im String in Kleinbuchstaben (A-Z), Umlaute
void conv_to_lower (char *text)
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

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// entfernt einzelne Zeichen im Text die dem übergebenen Zeichen entsprechen, sodaß dieses Zeichen gelöscht wird
// Text darf KEINE Mehrbytezeichen enthalten
void kill_onebytechars (char *text, int character)
{
	int		cnt=0;
	char	tmp[MAX_WORD_LEN];
	char	letter[2];

	letter[1]='\0';
	tmp[0]='\0';

	do {
		if (text[cnt] != character) {
			letter[0] = text[cnt];
			strcat (tmp, letter);
		}
		cnt++;
	} while (text[cnt] != '\0');
	
	strcpy(text, tmp);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// entfernt alle mehrfach hintereinander auftretenden Zeichen im Text, sodaß nur noch je eines vorhanden ist
// Text darf KEINE Mehrbytezeichen enthalten
void kill_double_onebytechars (char *text)
{
	int		cnt=0;
	int		last=0;
	char	tmp[MAX_WORD_LEN];
	char	letter[2];

	letter[1]='\0';
	tmp[0]='\0';

	do {
		if (text[cnt] != last) {
			letter[0] = text[cnt];
			strcat (tmp, letter);
			last = text[cnt];
		}
		cnt++;
	} while (text[cnt] != '\0');
	
	strcpy(text, tmp);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Kölner Phonetik Verfahren
// Wort wird in Gruppen von 3 Buchstaben gelesen, da es Regeln für "NACH" und "VOR" Buchstabe gibt
// Der Referenzbuchstabe ist immer der in der Mitte der 3er Gruppe
void conv_to_cologne(char *target, char *dest)
{
	char	text[MAX_WORD_LEN]="\0";
	char	code[MAX_WORD_LEN]="\0";
	char	group[3];
	int		len;
	int		cnt;

	// Wegen lesen in 3er Gruppen erweitern wir den übergebenen String
	// nachdem er in text kopiert ist hinten um vorn um ein Zeichen
	strcat (text, " ");
	strcat (text, target);
	strcat (text, " ");
	len = strlen(text);

	for (cnt=1; cnt<=len - 2; cnt++) {
		// In 3er Gruppen lesen
		group[0]=text[cnt - 1];
		group[1]=text[cnt];
		group[2]=text[cnt + 1];

		// Sonderfälle Wortanfang
		if (cnt==1) {
			if (group[1] == 'c') {
				if (group[2] == 'a' || group[2] == 'h' || group[2] == 'k' ||
					group[2] == 'l' || group[2] == 'o' || group[2] == 'q' ||
					group[2] == 'r' || group[2] == 'u' || group[2] == 'x') {
						strcat(code, "4");
						continue;
				}
				else {
					strcat(code, "8");
					continue;
				}
			}
		}

		// Normale codierung KEIN Wortanfang
		if (group[1] == 'p' && group[2] == 'h') {
			strcat (code, "3");
			continue;
		}
		if ((group[1] == 'd' && group[2] == 's') || (group[1] == 'd' && group[2] == 'c') ||
			(group[1] == 'd' && group[2] == 'z') || (group[1] == 't' && group[2] == 's') ||
			(group[1] == 't' && group[2] == 'c') || (group[1] == 't' && group[2] == 'z')) {
				strcat(code, "8");
				continue;
		}
		if (group[1] == 'd' || group[1] == 't') {
			strcat(code, "2");
			continue;
		}
		if ((group[0] == 'c' && group[1] == 'x') || (group[0] == 'k' && group[1] == 'x') ||
			(group[0] == 'q' && group[1] == 'x')) {
				strcat(code, "8");
				continue;
		}
		if (group[1] == 'x') {
			strcat(code, "48");
			continue;
		}
		if ((group[0] == 's' && group[1] == 'c') || (group[0] == 's' && group[1] == 'z')) {
			strcat(code, "8");
			continue;
		}
		if (group[1] == 'c') {
			if (group[2] == 'a' || group[2] == 'o' || group[2] == 'u' || group[2] == 'h' ||
				group[2] == 'k' || group[2] == 'x' || group[2] == 'q') {
					strcat(code, "4");
					continue;
			}
			else {
				strcat(code, "8");
				continue;
			}
		}
		if (group[1] == 'h') {
			strcat(code, "h");
			continue;
		}
		if (group[1] == 'a' || group[1] == 'e' || group[1] == 'i' || group[1] == 'j' ||
			group[1] == 'y' || group[1] == 'o' || group[1] == 'u') {
				strcat(code, "0");
				continue;
		}
		if (group[1] == 'b' || group[1] == 'p') {
			strcat(code, "1");
			continue;
		}
		if (group[1] == 'f' || group[1] == 'v' || group[1] == 'w') {
			strcat(code, "3");
			continue;
		}
		if (group[1] == 'g' || group[1] == 'k' || group[1] == 'q') {
			strcat(code, "3");
			continue;
		}
		if (group[1] == 'l') {
			strcat(code, "5");
			continue;
		}
		if (group[1] == 'r') {
			strcat(code, "7");
			continue;
		}
		if (group[1] == 'm' || group[1] == 'n') {
			strcat(code, "6");
			continue;
		}
		if (group[1] == 's' || group[1] == 'z') {
			strcat(code, "8");
			continue;
		}
	}
	
	// doppelte entfernen, h entfernen
	kill_double_onebytechars (code);
	kill_onebytechars (code, 'h');
	// 0 außer am Anfang entfernen Rückgabe füllen
	dest[0]='\0';
	if (code[0] == '0') {
		dest[0]='0';
		dest[1]='\0';
	}
	kill_onebytechars (code, '0');
	strcat (dest, code);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Phonem Verfahren
void conv_to_phonem(char *target, char *dest)
{
	int		cnt=0;
	char	txt[MAX_WORD_LEN];
	char	tmp[5];

	// 1. Durchgang = Buchstabenpaare kopieren/nach Regeln codieren
	//                Da es mit dem ö zu Problemen kommt, wird es vorerst als 0 kodiert
	txt[0]='\0';
	while (target[cnt] != '\0') {
		// ae
		if (target[cnt] == 'a' && target[cnt + 1] == 'e') {
			strcat(txt, "e");
			cnt = cnt + 2;
		}
		// pf
		else if (target[cnt] == 'p' && target[cnt + 1] == 'f') {
			strcat(txt, "v");
			cnt = cnt + 2;
		}
		// ks
		else if (target[cnt] == 'k' && target[cnt + 1] == 's') {
			strcat(txt, "x");
			cnt = cnt + 2;
		}
		// qu
		else if (target[cnt] == 'q' && target[cnt + 1] == 'u') {
			strcat(txt, "kw");
			cnt = cnt + 2;
		}
		// oe
		else if (target[cnt] == 'o' && target[cnt + 1] == 'e') {
			strcat(txt, "0");
			cnt = cnt + 2;
		}
		// eu
		else if (target[cnt] == 'e' && target[cnt + 1] == 'u') {
			strcat(txt, "oy");
			cnt = cnt + 2;
		}
		// ou
		else if (target[cnt] == 'o' && target[cnt + 1] == 'u') {
			strcat(txt, "u");
			cnt = cnt + 2;
		}
		// ei,ey
		else if ((target[cnt] == 'e' && target[cnt + 1] == 'i') ||
			(target[cnt] == 'e' && target[cnt + 1] == 'y')) {
				strcat(txt, "ay");
				cnt = cnt + 2;
		}
		// sc,sz,cz,tz,ts
		else if ((target[cnt] == 's' && target[cnt + 1] == 'c') ||
			(target[cnt] == 's' && target[cnt + 1] == 'z') ||
			(target[cnt] == 'c' && target[cnt + 1] == 'z') ||
			(target[cnt] == 't' && target[cnt + 1] == 'z') ||
			(target[cnt] == 't' && target[cnt + 1] == 's')) {
				strcat(txt, "c");
				cnt = cnt + 2;
		}

		else {
			// Wenn Buchstabenpaar nicht nach Regeln kodiert werden konnte
			// nur einen Buchstaben kopieren und weiter ab dem nächsten mit Paarsuche
			tmp[0]=target[cnt];
			tmp[1]='\0';
			strcat (txt, tmp);
			cnt = cnt + 1;
		}
	}

	// 2. Durchgang einzelne Buchstaben ersetzten/kodieren
	cnt=0;
	while (txt[cnt] != '\0') {
		//z,k,g,q
		if (txt[cnt] == 'z' || txt[cnt] == 'k' || txt[cnt] == 'g' || txt[cnt] == 'q') txt[cnt] = 'c';
		//u,i,j
		else if (txt[cnt] == 'u' || txt[cnt] == 'i' || txt[cnt] == 'j') txt[cnt] = 'y';
		//f,w,v
		else if (txt[cnt] == 'f' || txt[cnt] == 'w') txt[cnt] = 'v';
		//a
		else if (txt[cnt] == 'a') txt[cnt] = 'e';
		//p
		else if (txt[cnt] == 'p') txt[cnt] = 'b';
		//t
		else if (txt[cnt] == 't') txt[cnt] = 'd';
		cnt++;
	}

	// 3. Durchgang alle doppelten Zeichen entfernen
	kill_double_onebytechars (txt);

	// 4. Durchgang alles in Rückgabestring kopieren, unerlaubte Zeichen dabei außlassen & 0 zu Ö wandeln, Großschreiben
	cnt=0;
	dest[0]='\0';
	while (txt[cnt] != '\0') {
		if (txt[cnt] == 'a' || txt[cnt] == 'b' || txt[cnt] == 'c' || txt[cnt] == 'd' || txt[cnt] == 'l' ||
			txt[cnt] == 'm' || txt[cnt] == 'n' || txt[cnt] == 'o' || txt[cnt] == 'r' || txt[cnt] == 's' ||
			txt[cnt] == 'u' || txt[cnt] == 'v' || txt[cnt] == 'w' || txt[cnt] == 'x' || txt[cnt] == 'y') {
				tmp[0] = txt[cnt];
				tmp[1] = '\0';
				strcat (dest, tmp);
		}
		else if (txt[cnt] == '0') strcat (dest, "Ö\0");
		cnt++;
	}
	conv_to_upper (dest);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Soundex Verfahren
void conv_to_soundex(char *target, char *dest)
{
	int		cnt=1;
	char	txt[MAX_WORD_LEN];
	int		len;

	// 1. Zeichen einfach kopieren in Rückgabe, Großschreiben
	dest[0] = target[0];
	dest[1] = '\0';
	conv_to_upper (dest);

	// ab 2. Zeichen alles rüberkopieren nach txt
	while (target[cnt] != '\0') {
		txt[cnt - 1] = target[cnt];
		cnt++;
	}
	txt[cnt - 1]='\0';

	// Ersetzungsregeln auf txt anwenden
	cnt=0;
	while (txt[cnt] != '\0') {
		if (txt[cnt] == 'a' || txt[cnt] == 'e' || txt[cnt] == 'h' || txt[cnt] == 'i' ||
		    txt[cnt] == 'o' || txt[cnt] == 'u' || txt[cnt] == 'w' || txt[cnt] == 'y') txt[cnt] = '0';
		else if (txt[cnt] == 'b' || txt[cnt] == 'f' || txt[cnt] == 'p' || txt[cnt] == 'v') txt[cnt] = '1';
		else if (txt[cnt] == 'c' || txt[cnt] == 'g' || txt[cnt] == 'j' || txt[cnt] == 'k' ||
			 txt[cnt] == 'q' || txt[cnt] == 's' || txt[cnt] == 'x' || txt[cnt] == 'z') txt[cnt] = '2';
		else if (txt[cnt] == 'd' || txt[cnt] == 't') txt[cnt] = '3';
		else if (txt[cnt] == 'l') txt[cnt] = '4';
		else if (txt[cnt] == 'm' || txt[cnt] == 'n') txt[cnt] = '5';
		else if (txt[cnt] == 'r') txt[cnt] = '6';
		cnt++;
	}

	// Doppelte entfernen, dann 0en entfernen, an Rückgabe hängen, auf 4 Stellen kürzen/0en anhängen
	kill_double_onebytechars (txt);
	kill_onebytechars (txt, '0');
	strcat (dest, txt);
	len=strlen(dest);

	if (len == 1) strcat (dest, "000");
	else if (len == 2) strcat (dest, "00");
	else if (len == 3) strcat (dest, "0");
	else if (len > 4) dest[4]='\0';
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Extended Soundex Verfahren
void conv_to_exsoundex(char *target, char *dest)
{
	int		cnt=0;
	char	txt[MAX_WORD_LEN];

	// Quelle kopieren und Zeichen kodieren/ersetzten
	strcpy (txt, target);
	while (txt[cnt] != '\0') {
		if (txt[cnt] == 'a' || txt[cnt] == 'e' || txt[cnt] == 'h' || txt[cnt]== 'i' ||
		    txt[cnt] == 'o' || txt[cnt] == 'u' || txt[cnt] == 'w' || txt[cnt]== 'y') txt[cnt] = '0';
		else if (txt[cnt] == 'b' || txt[cnt] == 'p') txt[cnt] = '1';
		else if (txt[cnt] == 'v' || txt[cnt] == 'f') txt[cnt] = '2';
		else if (txt[cnt] == 'c' || txt[cnt] == 'k' || txt[cnt] == 's') txt[cnt] = '3';
		else if (txt[cnt] == 'g' || txt[cnt] == 'j') txt[cnt] = '4';
		else if (txt[cnt] == 'q' || txt[cnt] == 'x' || txt[cnt] == 'z') txt[cnt] = '5';
		else if (txt[cnt] == 'd' || txt[cnt] == 't') txt[cnt] = '6';
		else if (txt[cnt] == 'l') txt[cnt] = '7';
		else if (txt[cnt] == 'm' || txt[cnt] == 'n') txt[cnt] = '8';
		else if (txt[cnt] == 'r') txt[cnt] = '9';
		cnt++;
	}

	// Doppelte entfernen, 0en entfernen
	kill_double_onebytechars (txt);
	kill_onebytechars (txt, '0');

	// Ausgabe mit 5 Stellen 0 füllen, txt rüberkopieren auf 5 Stellen kürzen
	strcpy (dest, "00000");
	cnt=0;
	while (txt[cnt] != '\0') {
		dest[cnt] = txt[cnt];
		cnt++;
	}
	dest[5] = '\0';
}


