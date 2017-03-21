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
#define PHCODE_STAT_OK			0
#define PHCODE_STAT_TO_SHORT	1		// Name/Wort zur kurz zum sinnvollen Codieren
#define PHCODE_STAT_TO_LONG		2		// Name/Wort zu Lang
#define PHCODE_STAT_NO_CODE		3		// Name/Wort erzeugt keinen phonetischen Code
#define PHCODE_STAT_NO_WORD		4		// Name/Wort kein ungültig (enthält Sonderzeichen usw.)
#define PHCODE_STAT_PARAM_ERR	5		// ungültige Übergabeparameter



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// passend zum Fehlercode wird ein Text ausgegeben
static void show_error (int err_no)
{
	if (err_no == PHCODE_STAT_TO_SHORT) fprintf(stderr, "Name zu kurz zum sinnvollen kodieren !\n");
	else if (err_no == PHCODE_STAT_TO_LONG) fprintf(stderr, "Name zu lang !\n");
	else if (err_no == PHCODE_STAT_NO_CODE) fprintf(stderr, "Name erzeugt keinen gültigen phonetischen Code !\n");
	else if (err_no == PHCODE_STAT_NO_WORD) fprintf(stderr, "Name enthält unerlaubte Zeichen !\n");
	else if (err_no == PHCODE_STAT_PARAM_ERR) {
		fprintf(stderr, "Falsche Aufrufparameter !\n"
			"Aufruf:   phonecode-de -codetyp Name\n"
			"z.B.      phonecode-de -k Müller\n"
			"Rückgabe: phonetischer Code\n"
			"Exitcode: %d bei erfolgreicher phonetischer Codierung\n"
			"          %d wenn Name zu kurz zum sinnvollen codieren\n"
			"          %d wenn Name zu lang\n"
			"          %d wenn auf Grund des Namens keine kodierung erfolgt z.B. phonecode-de -p Aahe\n"
			"          %d wenn unerlaubte Zeichen im Namen enthalten sind. Erlaubt sind nur einzelne Worte,\n"
			"            Buchstaben deutsches Alphabet incl. Umlaute. Keine Leerzeichen oder Satzzeichen.\n"
			"          %d wenn falsche Aufrufparameter\n"
			"Codetyp:  -k = Kölner Phonetik\n"
			"          -p = Phonem\n"
			"          -s = Soundex\n"
			"          -e = Extended Soundex\n",PHCODE_STAT_OK, PHCODE_STAT_TO_SHORT, PHCODE_STAT_TO_LONG,
												PHCODE_STAT_NO_CODE, PHCODE_STAT_NO_WORD, PHCODE_STAT_PARAM_ERR );
	}
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main(int argc, char* argv[])
{
	int		convstat;
	int		convmode;
	int		exitcode;
	char	code[BUFFER_CODE_SIZE];

	// Übergabeparameter prüfen
	// Falsche Anzahl Argumente
	if (argc != 3) {
		exitcode = PHCODE_STAT_PARAM_ERR;
		show_error (exitcode);
		return exitcode;
	}

	// Namen kleiner als 2 Zeichen erzeugen Fehler
	if (strlen (argv[2]) < 2) {
		exitcode = PHCODE_STAT_TO_SHORT;
		show_error(exitcode);
		return exitcode;
	}

	// Übergabeparameter Codetyp abfragen/setzen, Ende wenn unbekannter Parameter
	if (strcmp (argv[1], "-k") == 0) convmode = CONV_MODE_COLOGNE;
	else if (strcmp (argv[1], "-p") == 0) convmode = CONV_MODE_PHONEM;
	else if (strcmp (argv[1], "-s") == 0) convmode = CONV_MODE_SOUNDEX;
	else if (strcmp (argv[1], "-e") == 0) convmode = CONV_MODE_EXSOUNDEX;
	else {
		exitcode = PHCODE_STAT_PARAM_ERR;
		show_error (exitcode);
		return exitcode;
	}
	
	// Name in phon. Code wandeln
	convstat = phoneconvert (argv[2], code, convmode);
	
	// Fehler beim codieren
	if (convstat == CONV_STAT_NO_CODE) {
		exitcode = PHCODE_STAT_NO_CODE;
		show_error(exitcode);
		return exitcode;
	}
	else if (convstat == CONV_STAT_NO_WORD) {
		exitcode = PHCODE_STAT_NO_WORD;
		show_error(exitcode);
		return exitcode;
	}
	else if (convstat == CONV_STAT_TO_LONG) {
		exitcode = PHCODE_STAT_TO_LONG;
		show_error(exitcode);
		return exitcode;
	}

	// Alles OK, Code ausgeben
	printf("%s\n", code);
	return PHCODE_STAT_OK;
}
