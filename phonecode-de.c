/*
phonecode-de
Kodiert einen übergebenen Namen/Wort in den angegebenen phonetischen Code
Zielcodierungen sind:
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
#include <string.h>
#include "phonetics.h"

// Statuskonstannten / Fehlercodes
#define STAT_OK				0
#define STAT_TO_SHORT		1		// Name/Wort zur kurz zum sinnvollen Codieren
#define STAT_TO_LONG		2		// Name/Wort zu Lang
#define STAT_NO_CODE		3		// Name/Wort ungültig
#define STAT_PARAM_ERR		4		// ungültige Übergabeparameter



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// passend zum Fehlercode wird ein Text ausgegeben
static void show_error (int err_no)
{
	if (err_no == STAT_TO_SHORT) fprintf(stderr, "Name zu kurz zum sinnvollen kodieren !\n");
	else if (err_no == STAT_TO_LONG) fprintf(stderr, "Name zu lang !\n");
	else if (err_no == STAT_NO_CODE) fprintf(stderr, "Name erzeugt keinen gültigen phonetischen Code !\n"
													"oder enthält unerlaubte Zeichen !\n");
	else if (err_no == STAT_PARAM_ERR) {
		fprintf(stderr, "Falsche Aufrufparameter !\n"
			"Aufruf:   phonecode-de -codetyp Name\n"
			"z.B.      phonecode-de -k Müller\n"
			"Rückgabe: phonetischer Code\n"
			"Exitcode: %d bei erfolgreicher phonetischer Codierung\n"
			"          %d wenn Name zu kurz zum sinnvollen codieren\n"
			"          %d wenn Name zu lang\n"
			"          %d wenn auf Grund des Namens keine kodierung erfolgt z.B. phonecode-de -p Aahe\n"
			"           oder sich unerlaubte Zeichen im Namen befinden. Erlaubt sind nur einzelne Worte,\n"
			"            Buchstaben deutsches Alphabet incl. Umlaute. Keine Leerzeichen oder Satzzeichen.\n"
			"          %d wenn falsche Aufrufparameter\n"
			"Codetyp:  -k = Kölner Phonetik\n"
			"          -p = Phonem\n"
			"          -s = Soundex\n"
			"          -e = Extended Soundex\n",
								STAT_OK, STAT_TO_SHORT, STAT_TO_LONG, STAT_NO_CODE,STAT_PARAM_ERR);
	}
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main(int argc, char* argv[])
{
	char code[BUFFER_CODE_SIZE];

	// Übergabeparameter prüfen
	// Falsche Anzahl Argumente
	if (argc != 3) {
		show_error (STAT_PARAM_ERR);
		return STAT_PARAM_ERR;
	}

	// prüfen ob Namen zu kurz oder lang
	if (strlen (argv[2]) < 2) {
		show_error(STAT_TO_SHORT);
		return STAT_TO_SHORT;
	}
	if (strlen (argv[2]) >= BUFFER_WORD_SIZE) {
		show_error(STAT_TO_LONG);
		return STAT_TO_LONG;
	}
	
	
	// Übergabeparameter Codetyp abfragen phonetischen Code erzeugen, Ende wenn unbekannter Parameter
	if (strcmp (argv[1], "-k") == 0) phoneconvert_cologne(argv[2], code);
	else if (strcmp (argv[1], "-p") == 0) phoneconvert_phonem(argv[2], code);
	else if (strcmp (argv[1], "-s") == 0) phoneconvert_soundex(argv[2], code);
	else if (strcmp (argv[1], "-e") == 0) phoneconvert_exsoundex(argv[2], code);
	else {
		show_error (STAT_PARAM_ERR);
		return STAT_PARAM_ERR;
	}
	
	// Ende mit Fehler wenn Name ungültig
	if (code[0] == '\0') {
		show_error(STAT_NO_CODE);
		return STAT_NO_CODE;
	}
	
	// Alles OK, Code ausgeben
	printf("%s\n", code);
	return STAT_OK;
}
