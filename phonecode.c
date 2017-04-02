/*
phonecode
Kodiert einen übergebenen Namen/Wort in den angegebenen phonetischen Code
Zielcodierungen sind:
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
#include "buffer_sizes.h"
#include "phoneshow_types.h"
#include "phonetics.h"

// Rückgabekonstannten ans BS
#define PHCODE_SUCCESS            0
#define PHCODE_NAME_UNDERSIZE     1
#define PHCODE_NAME_OVERSIZE      2
#define PHCODE_NAME_NOT_GERMAN    3
#define PHCODE_PARAM_ERROR        4

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// passend zum Fehlercode wird ein Text ausgegeben
// und Programm beendet
static void error_exit (int err_no)
{
  if (err_no == PHCODE_NAME_UNDERSIZE) fprintf(stderr, "Name zu kurz zum sinnvollen kodieren !\n");
  else if (err_no == PHCODE_NAME_OVERSIZE) fprintf(stderr, "Name zu lang !\n");
  else if (err_no == PHCODE_NAME_NOT_GERMAN) fprintf(stderr, "Name ungültig !\n");
  else if (err_no == PHCODE_PARAM_ERROR) {
    fprintf(stderr, "Falsche Aufrufparameter !\n"
      "Aufruf:   phonecode-de -codetyp Name\n"
      "z.B.      phonecode-de -k Müller\n"
      "Codetyp: -k = Kölner Phonetik\n"
      "         -p = Phonem\n"
      "         -s = Soundex\n"
      "         -e = Extended Soundex\n"
      "Rückgabe: phonetischer Code\n"
      "Exitcode: %i bei erfolgreicher phonetischer Codierung\n"
      "          %i wenn Name zu kurz zum sinnvollen codieren\n"
      "          %i wenn Name zu lang\n"
      "          %i wenn sich unerlaubte Zeichen im Namen befinden. Erlaubt sind nur einzelne Worte,\n"
      "            Buchstaben deutsches Alphabet incl. Umlaute. Keine Leerzeichen oder Satzzeichen.\n"
      "          %i wenn falsche Aufrufparameter\n", PHCODE_SUCCESS, PHCODE_NAME_UNDERSIZE,
                          PHCODE_NAME_OVERSIZE, PHCODE_NAME_NOT_GERMAN, PHCODE_PARAM_ERROR);
  }
  exit (err_no);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main(int argc, char* argv[])
{
  char code[BUFFER_SIZE_WORD];

  // Übergabeparameter prüfen
  // Falsche Anzahl Argumente
  if (argc != 3) error_exit(PHCODE_PARAM_ERROR);

  // prüfen ob Namen zu kurz oder lang
  if (strlen (argv[2]) < 2) error_exit (PHCODE_NAME_UNDERSIZE);
  if (strlen (argv[2]) >= BUFFER_SIZE_WORD) error_exit (PHCODE_NAME_OVERSIZE);

  // Übergabeparameter Codetyp abfragen phonetischen Code erzeugen, Ende wenn unbekannter Parameter
  if (strcmp (argv[1], "-k") == 0) phoneconvert_cologne(argv[2], code);
  else if (strcmp (argv[1], "-p") == 0) phoneconvert_phonem(argv[2], code);
  else if (strcmp (argv[1], "-s") == 0) phoneconvert_soundex(argv[2], code);
  else if (strcmp (argv[1], "-e") == 0) phoneconvert_exsoundex(argv[2], code);
  else error_exit (PHCODE_PARAM_ERROR);

  // Ende mit Fehler wenn Name ungültig
  if (code[0] == '\0') error_exit (PHCODE_NAME_NOT_GERMAN);

  // Alles OK, Code ausgeben
  printf("%s\n", code);
  return PHCODE_SUCCESS;
}
