/*
phonecomp-de
Prüft zwei übergebenen Namen auf phonetische Gleichheit
Beide Namen werden wie mit <phonecode-de> entsprechend der gewählten Kodierung verglichen
Vergleichscodierungen sind:
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
//Übergabeparameter TEXT_NICHT_OK (1) oder TEXT_UNDEFINIERT (2) oder TEXT_ZU_KURZ (3) oder TEXT_KEIN_CODE (4)
//passend zum Fehlercode wird ein Text ausgegeben
void	zeige_fehler(int fehlercode)
{
	if (fehlercode == TEXT_ZU_KURZ) fprintf(stderr, "Name zu kurz zum sinnvollen dekodieren !\n");
	else if (fehlercode == TEXT_NICHT_OK) fprintf(stderr, "Konvertierung nicht möglich, Name enthält unerlaubte Zeichen !\n");
	else if (fehlercode == TEXT_KEIN_CODE) fprintf(stderr, "erzeugt keinen gültigen phonetischen Code !\n");
	else if (fehlercode == TEXT_UNDEFINIERT)
	{
		fprintf(stderr, "Falsche Aufrufparameter !\n"
			"Aufruf:   phonecode-de -codetyp Name Name\n"
			"z.B.      phonecode-de -k Müller Muehler\n"
			"Rückgabe: Erklärung ob phonetisch gleich oder ungleich\n"
			"Exitcode: %d wenn Namen phonetisch gleich\n"
			"          %d wenn unerlaubte Zeichen im Namen enthalten sind. Erlaubt sind nur einzelne Worte,\n"
			"            Buchstaben deutsches Alphabet incl. Umlaute. Keine Leerzeichen oder Satzzeichen.\n"
			"          %d wenn falsche Aufrufparameter\n"
			"          %d wenn Name kürzer als 2 Buchstaben\n"
			"          %d wenn auf Grund des Namens keine kodierung erfolgt z.B. phonecode_de -p Aahe\n"
		        "          %d wenn Namen phonetisch ungleich\n"
			"Codetyp:  -k = Kölner Phonetik\n"
			"          -p = Phonem\n"
			"          -s = Soundex\n"
			"          -e = Extended Soundex\n", TEXT_OK, TEXT_NICHT_OK, TEXT_UNDEFINIERT,
							TEXT_ZU_KURZ, TEXT_KEIN_CODE, TEXT_UNGLEICH);
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int 	main(int argc, char* argv[])
{

	int	exitcode;
	char	name1[MAX_WORT_LAENGE];
	char	name2[MAX_WORT_LAENGE];
	char	code1[MAX_WORT_LAENGE];
	char	code2[MAX_WORT_LAENGE];

	//Übergabeparameter prüfen
	//Falsche Anzahl Argumente
	if (argc != 4)
	{
		exitcode = TEXT_UNDEFINIERT;
		zeige_fehler (exitcode);
		return (exitcode);
	}

	//Namen kleiner als 2 Zeichen erzeugen Fehler
	if (strlen (argv[2]) < 2 || strlen(argv[3]) < 2)
	{
		exitcode=TEXT_ZU_KURZ;
		zeige_fehler(exitcode);
		return (exitcode);
	}

	//Eingaben prüfen ob einzelne deutsche Worte Ende wenn Probleme
	exitcode=pruefe_wort_deutsch(argv[2]);
	if (exitcode == TEXT_NICHT_OK)
	{
		zeige_fehler (exitcode);
		return (exitcode);
	}
	exitcode=pruefe_wort_deutsch(argv[3]);
	if (exitcode == TEXT_NICHT_OK)
	{
		zeige_fehler (exitcode);
		return (exitcode);
	}

	//Namen kopieren in Kleinbuchstaben wandeln und Umlaute ersetzten
	strcpy(name1, argv[2]);
	strcpy(name2, argv[3]);
	conv_kleinbuchstaben (name1);
	conv_kleinbuchstaben (name2);
	conv_umlaute (name1);
	conv_umlaute (name2);

	//Text je nach Codeparameter wandeln, Ende wenn unbekannter Parameter
	if (strcmp (argv[1], "-k") == 0)
	{
		conv_koelner (name1, code1);
		conv_koelner (name2, code2);
	}
	else if (strcmp (argv[1], "-p") == 0)
	{
		conv_koelner (name1, code1);
		conv_koelner (name2, code2);
	}
	else if (strcmp (argv[1], "-s") == 0)
	{
		conv_koelner (name1, code1);
		conv_koelner (name2, code2);
	}
	else if (strcmp (argv[1], "-e") == 0)
	{
		conv_koelner (name1, code1);
		conv_koelner (name2, code2);
	}
	else
	{
		exitcode=TEXT_UNDEFINIERT;
		zeige_fehler (exitcode);
  		return (exitcode);
	}

	//Wenn Code = leer = Ungültiger Code = Fehler
	if (strlen (code1) < 1)
	{
		exitcode=TEXT_KEIN_CODE;
		fprintf(stderr, "%s %s ",argv[1] ,argv[2]);
		zeige_fehler(exitcode);
		return (exitcode);
	}
	if (strlen (code2) < 1)
	{
		exitcode=TEXT_KEIN_CODE;
		fprintf(stderr, "%s %s ",argv[1] ,argv[3]);
		zeige_fehler(exitcode);
		return (exitcode);
	}

	//Ausgabe != oder == und Exitcode
	if (strcmp (code1, code2) == 0)
	{
		printf("%s == %s\n", argv[2], argv[3]);
		return (TEXT_OK);
	}
	else
	{
		printf("%s != %s\n", argv[2], argv[3]);
		return (TEXT_UNGLEICH);
	}
}



