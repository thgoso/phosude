/*
phonecomp-de
Prüft zwei übergebenen Namen auf phonetische Gleichheit
Beide Namen werden wie mit <phonecode-de> entsprechend der gewählten Kodierung verglichen
Vergleichscodierungen sind:
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
#include <string.h>
#include "phonetics.h"

// Statuskonstannten / Fehlercodes
#define PHCOMP_STAT_EQUAL		0		// Namen phonetisch gleich
#define PHCOMP_STAT_NOT_EQUAL	1		// Namen phonetisch ungleich
#define PHCOMP_STAT_TO_SHORT	2		// Name/Wort zur kurz zum sinnvollen Codieren
#define PHCOMP_STAT_TO_LONG		3		// Name/Wort zu Lang
#define PHCOMP_STAT_NO_CODE		4		// Name/Wort erzeugt keinen phonetischen Code
#define PHCOMP_STAT_NO_WORD		5		// Name/Wort kein ungültig (enthält Sonderzeichen usw.)
#define PHCOMP_STAT_PARAM_ERR	6		// ungültige Übergabeparameter



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// passend zum Fehlercode wird ein Text ausgegeben
static void show_error (int err_no)
{
	if (err_no == PHCOMP_STAT_TO_SHORT) fprintf(stderr, "Name zu kurz zum sinnvollen kodieren !\n");
	else if (err_no == PHCOMP_STAT_TO_LONG) fprintf(stderr, "Name zu lang !\n");
	else if (err_no == PHCOMP_STAT_NO_CODE) fprintf(stderr, "Name erzeugt keinen gültigen phonetischen Code !\n");
	else if (err_no == PHCOMP_STAT_NO_WORD) fprintf(stderr, "Name enthält unerlaubte Zeichen !\n");
	else if (err_no == PHCOMP_STAT_PARAM_ERR) {
		fprintf(stderr, "Falsche Aufrufparameter !\n"
			"Aufruf:   phonecomp-de -codetyp Name Name\n"
			"z.B.      phonecomp-de -k Müller Muehler\n"
			"Rückgabe: Erklärung ob phonetisch gleich oder ungleich\n"
			"Exitcode: %d wenn Namen phonetisch gleich\n"
			"          %d wenn Namen phonetisch ungleich\n"
			"          %d wenn Name zu kurz zum sinnvollen codieren\n"
			"          %d wenn Name zu lang\n"
			"          %d wenn auf Grund des Namens keine kodierung erfolgt z.B. phonecomp-de -p Aahe Eehe\n"
			"          %d wenn unerlaubte Zeichen im Namen enthalten sind. Erlaubt sind nur einzelne Worte,\n"
			"            Buchstaben deutsches Alphabet incl. Umlaute. Keine Leerzeichen oder Satzzeichen.\n"
			"          %d wenn falsche Aufrufparameter\n"
			"Codetyp:  -k = Kölner Phonetik\n"
			"          -p = Phonem\n"
			"          -s = Soundex\n"
			"          -e = Extended Soundex\n", PHCOMP_STAT_EQUAL, PHCOMP_STAT_NOT_EQUAL, PHCOMP_STAT_TO_SHORT,
							PHCOMP_STAT_TO_LONG, PHCOMP_STAT_NO_CODE, PHCOMP_STAT_NO_WORD, PHCOMP_STAT_PARAM_ERR);
	}
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main (int argc, char* argv[])
{
	int		convmode;
	int		exitcode;
	int		convstat1;
	int		convstat2;
	char	code1[BUFFER_CODE_SIZE];
	char	code2[BUFFER_CODE_SIZE];

	// Übergabeparameter prüfen
	// Falsche Anzahl Argumente
	if (argc != 4) {
		exitcode = PHCOMP_STAT_PARAM_ERR;
		show_error (exitcode);
		return exitcode;
	}

	// Namen kleiner als 2 Zeichen erzeugen Fehler
	if (strlen (argv[2]) < 2 || strlen(argv[3]) < 2) {
		exitcode = PHCOMP_STAT_TO_SHORT;
		show_error(exitcode);
		return exitcode;
	}
	
	// Übergabeparameter Codetyp abfragen/setzen, Ende wenn unbekannter Parameter
	if (strcmp (argv[1], "-k") == 0) convmode = CONV_MODE_COLOGNE;
	else if (strcmp (argv[1], "-p") == 0) convmode = CONV_MODE_PHONEM;
	else if (strcmp (argv[1], "-s") == 0) convmode = CONV_MODE_SOUNDEX;
	else if (strcmp (argv[1], "-e") == 0) convmode = CONV_MODE_EXSOUNDEX;
	else {
		exitcode = PHCOMP_STAT_PARAM_ERR;
		show_error (exitcode);
		return exitcode;
	}
	
	// Namen in phon. Codes wandeln
	convstat1 = phoneconvert (argv[2], code1, convmode);
	convstat2 = phoneconvert (argv[3], code2, convmode);
	
	// Fehler beim codieren
	if ((convstat1 == CONV_STAT_NO_CODE) || (convstat2 == CONV_STAT_NO_CODE)) {
		exitcode = PHCOMP_STAT_NO_CODE;
		show_error(exitcode);
		return exitcode;
	}
	if ((convstat1 == CONV_STAT_NO_WORD) || (convstat2 == CONV_STAT_NO_WORD)) {
		exitcode = PHCOMP_STAT_NO_WORD;
		show_error(exitcode);
		return exitcode;
	}
	if ((convstat1 == CONV_STAT_TO_LONG) || (convstat2 == CONV_STAT_TO_LONG)) {
		exitcode = PHCOMP_STAT_TO_LONG;
		show_error(exitcode);
		return exitcode;
	}
	
	// Ausgabe != oder == und Exitcode
	if (strcmp (code1, code2) == 0) {
		printf("%s == %s\n", argv[2], argv[3]);
		return PHCOMP_STAT_EQUAL;
	}
	else {
		printf("%s != %s\n", argv[2], argv[3]);
		return PHCOMP_STAT_NOT_EQUAL;
	}
}
