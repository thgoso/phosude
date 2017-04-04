/*
phonecomp
Prüft zwei übergebenen Namen auf phonetische Gleichheit
Beide Namen werden wie mit <phonecode-de> entsprechend der gewählten Kodierung verglichen
Vergleichscodierungen sind:
Kölner Phonetik   für deutsche Namen nach eigener Erfahrung am optimalsten
                  Code Numerisch, variable Länge
Phonem            in versch. dBase Varianten implementiert
                  Code Buchstaben, variable Länge
Soundex           Für deutsche Namen nicht ganz geeignet, aber Standard bei Genealogie(programmen)
                  Alphanumerischer Code, fixe Länge, Format Buchstabe gefolgt von 3 Ziffern
Extended Soundex  Erweiterung von Soundex
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
#include "typedefs.h"
#include "string.h"
#include "phonetics.h"

// Rückgaben ans BS
#define PHCOMP_MATCH              0
#define PHCOMP_NO_MATCH           1
#define PHCOMP_NAME_UNDERLENGTH   2
#define PHCOMP_NAME_OVERLENGTH    3
#define PHCOMP_NAME_NOT_GERMAN    4
#define PHCOMP_PARAM_ERROR        5

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// passend zum Fehlercode wird ein Text ausgegeben
// und Programm beendet
static void error_exit (int err_no)
{
  if (err_no == PHCOMP_NAME_UNDERLENGTH) fprintf(stderr, "Name zu kurz zum sinnvollen kodieren !\n");
  else if (err_no == PHCOMP_NAME_OVERLENGTH) fprintf(stderr, "Name zu lang !\n");
  else if (err_no == PHCOMP_NAME_NOT_GERMAN) fprintf(stderr, "Name ungültig !\n");
  else if (err_no == PHCOMP_PARAM_ERROR) {
    fprintf(stderr, "Falsche Aufrufparameter !\n"
      "Aufruf:   phonecomp-de -codetyp Name Name\n"
      "z.B.      phonecomp-de -k Müller Muehler\n"
      "Codetyp: -k = Kölner Phonetik\n"
      "         -p = Phonem\n"
      "         -s = Soundex\n"
      "         -e = Extended Soundex\n"
      "Rückgabe: Erklärung ob phonetisch gleich oder ungleich\n"
      "Exitcode: %i wenn Namen phonetisch gleich\n"
      "          %i wenn Namen phonetisch ungleich\n"
      "          %i wenn Name zu kurz zum sinnvollen codieren\n"
      "          %i wenn Name zu lang\n"
      "          %i wenn unerlaubte Zeichen im Namen enthalten sind. Erlaubt sind nur einzelne Worte,\n"
      "            Buchstaben deutsches Alphabet incl. Umlaute. Keine Leerzeichen oder Satzzeichen.\n"
      "          %i wenn falsche Aufrufparameter\n", PHCOMP_MATCH, PHCOMP_NO_MATCH, PHCOMP_NAME_UNDERLENGTH,
                                        PHCOMP_NAME_OVERLENGTH, PHCOMP_NAME_NOT_GERMAN, PHCOMP_PARAM_ERROR);
  }

  exit (err_no);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main (int argc, char* argv[])
{
  word_t name1, name2;
  code_t code1, code2;
  
  // Übergabeparameter prüfen
  // Falsche Anzahl Argumente
  if (argc != 4) error_exit (PHCOMP_PARAM_ERROR);

  // Namen zu kurz, lang, ungültig
  if (strlen (argv[2]) < 2 || strlen(argv[3]) < 2) error_exit (PHCOMP_NAME_UNDERLENGTH);
  if (str_to_word_type(argv[2], &name1) == false) error_exit (PHCOMP_NAME_OVERLENGTH);
  if (str_to_word_type(argv[2], &name2) == false) error_exit (PHCOMP_NAME_OVERLENGTH);
  if (str_to_ascii_upper_word(name1.s) == false) error_exit (PHCOMP_NAME_NOT_GERMAN);
  if (str_to_ascii_upper_word(name2.s) == false) error_exit (PHCOMP_NAME_NOT_GERMAN);
  
   // Übergabeparameter Codetyp abfragen konvertieren
  if (strcmp (argv[1], "-k") == 0) {
    phoneconvert_cologne(&name1, &code1);
    phoneconvert_cologne(&name2, &code2);
  }
  else if (strcmp (argv[1], "-p") == 0) {
    phoneconvert_phonem(&name1, &code1);
    phoneconvert_phonem(&name2, &code2);
  }
  else if (strcmp (argv[1], "-s") == 0) {
    phoneconvert_soundex(&name1, &code1);
    phoneconvert_soundex(&name2, &code2);
  }
  else if (strcmp (argv[1], "-e") == 0) {
    phoneconvert_exsoundex(&name1, &code1);
    phoneconvert_exsoundex(&name2, &code2);
  }
  else error_exit (PHCOMP_PARAM_ERROR);

    // Ausgabe != oder == und Exitcode
  if (strcmp (code1.s, code2.s) == 0) {
    printf("%s == %s\n", argv[2], argv[3]);
    return PHCOMP_MATCH;
  }
  else {
    printf("%s != %s\n", argv[2], argv[3]);
    return PHCOMP_NO_MATCH;
  }
}
