/*
phonecode-de
Kodiert einen übergebenen (Nach)namen in den angegebenen phonetischen Code
Zielcodierungen sind:
Kölner Phonetik		für deutsche Namen nach eigener Erfahrung am optimalsten
			Code Numerisch, variable Länge
Phonem			in versch. dBase Varianten implementiert
			Code Buchstaben, variable Länge
Soundex			Für deutsche Namen nicht ganz geeignet, aber Standard bei Genealogie(programmen)
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
#include "basis_func.h"


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//passend zum Fehlercode wird ein Text ausgegeben
void	zeige_fehler(int fehlercode)
{
	if (fehlercode == TEXT_ZU_KURZ) fprintf(stderr, "Name zu kurz zum sinnvollen dekodieren !\n");
	else if (fehlercode == TEXT_NICHT_OK) fprintf(stderr, "Konvertierung nicht möglich, Name enthält unerlaubte Zeichen !\n");
	else if (fehlercode == TEXT_KEIN_CODE) fprintf(stderr, "erzeugt keinen gültigen phonetischen Code !\n");
	else if (fehlercode == TEXT_UNDEFINIERT)
	{
		fprintf(stderr, "Falsche Aufrufparameter !\n"
			"Aufruf:   phonecode-de -codetyp Name\n"
			"z.B.      phonecode-de -k Müller\n"
			"Rückgabe: phonetischer Code\n"
			"Exitcode: %d bei erfolgreicher phonetischer Codierung\n"
			"          %d wenn unerlaubte Zeichen im Namen enthalten sind. Erlaubt sind nur einzelne Worte,\n"
			"            Buchstaben deutsches Alphabet incl. Umlaute. Keine Leerzeichen oder Satzzeichen.\n"
			"          %d wenn falsche Aufrufparameter\n"
			"          %d wenn Name kürzer als 2 Buchstaben\n"
			"          %d wenn auf Grund des Namens keine kodierung erfolgt z.B. phonecode_de -p Aahe\n"
			"Codetyp:  -k = Kölner Phonetik\n"
			"          -p = Phonem\n"
			"          -s = Soundex\n"
			"          -e = Extended Soundex\n", TEXT_OK, TEXT_NICHT_OK, TEXT_UNDEFINIERT, TEXT_ZU_KURZ, TEXT_KEIN_CODE);
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int 	main(int argc, char* argv[])
{
	int	exitcode;
	char	name[MAX_WORT_LAENGE];
	char	code[MAX_WORT_LAENGE];

	//Übergabeparameter prüfen
	//Falsche Anzahl Argumente
	if (argc != 3)
	{
		exitcode = TEXT_UNDEFINIERT;
		zeige_fehler (exitcode);
		return (exitcode);
	}

	//Namen kleiner als 2 Zeichen erzeugen Fehler
	if (strlen (argv[2]) < 2)
	{
		exitcode=TEXT_ZU_KURZ;
		zeige_fehler(exitcode);
		return (exitcode);
	}

	//Eingabe prüfen ob einzelnes deutsches Wort
	exitcode=pruefe_wort_deutsch(argv[2]);
	if (exitcode == TEXT_NICHT_OK)
	{
		zeige_fehler (exitcode);
		return (exitcode);
	}

	//Name kopieren in Kleinbuchstaben wandeln und Umlaute ersetzten
	strcpy(name, argv[2]);
	conv_kleinbuchstaben (name);
	conv_umlaute (name);

	//Text je nach Codeparameter wandeln, Ende wenn unbekannter Parameter
	if (strcmp (argv[1], "-k") == 0) conv_koelner (name, code);
	else if (strcmp (argv[1], "-p") == 0) conv_phonem (name, code);
	else if (strcmp (argv[1], "-s") == 0) conv_soundex (name, code);
	else if (strcmp (argv[1], "-e") == 0) conv_exsoundex (name, code);
	else
	{
		exitcode=TEXT_UNDEFINIERT;
		zeige_fehler (exitcode);
		return (exitcode);
	}

	//Wenn Code = leer = Ungültiger Code = Fehler
	if (strlen (code) < 1)
	{
		exitcode=TEXT_KEIN_CODE;
		fprintf(stderr, "%s %s ",argv[1] ,argv[2]);
		zeige_fehler(exitcode);
		return (exitcode);
	}

	//Alles OK, Code ausgeben
	printf("%s\n", code);
	return (TEXT_OK);
}


