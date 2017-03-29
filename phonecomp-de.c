/*
phonecomp-de
Prüft zwei übergebenen Namen auf phonetische Gleichheit
Beide Namen werden wie mit <phonecode-de> entsprechend der gewählten Kodierung verglichen
Vergleichscodierungen sind:
Kölner Phonetik   für deutsche Namen nach eigener Erfahrung am optimalsten
          Code Numerisch, variable Länge
Phonem        in versch. dBase Varianten implementiert
          Code Buchstaben, variable Länge
Soundex       Für deutsche Namen nicht ganz geeignet, aber Standard bei Genealogie(programmen)
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
#include <string.h>
#include "phonetics.h"

// Statuskonstannten / Fehlercodes
#define STAT_EQUAL      0   // Namen phonetisch gleich
#define STAT_NOT_EQUAL  1   // Namen phonetisch ungleich
#define STAT_TO_SHORT   2   // Name/Wort zur kurz zum sinnvollen Codieren
#define STAT_TO_LONG    3   // Name/Wort zu Lang
#define STAT_NO_CODE    4   // Name/Wort erzeugt keinen phonetischen Code
                            // oder enthält Sonderzeichen usw.)
#define STAT_PARAM_ERR  5   // ungültige Übergabeparameter



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
      "          %i wenn auf Grund des Namens keine kodierung erfolgt z.B. phonecomp-de -p Aahe Eehe\n"
      "            oder wenn unerlaubte Zeichen im Namen enthalten sind. Erlaubt sind nur einzelne Worte,\n"
      "            Buchstaben deutsches Alphabet incl. Umlaute. Keine Leerzeichen oder Satzzeichen.\n"
      "          %i wenn falsche Aufrufparameter\n", STAT_EQUAL, STAT_NOT_EQUAL, STAT_TO_SHORT,
                          STAT_TO_LONG, STAT_NO_CODE, STAT_PARAM_ERR);
  }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main (int argc, char* argv[])
{
  char code1[BUFFER_CODE_SIZE];
  char code2[BUFFER_CODE_SIZE];

  // Übergabeparameter prüfen
  // Falsche Anzahl Argumente
  if (argc != 4) {
    show_error (STAT_PARAM_ERR);
    return STAT_PARAM_ERR;
  }

  // Namen zu kurz oder zu lang
  if (strlen (argv[2]) < 2 || strlen(argv[3]) < 2) {
    show_error(STAT_TO_SHORT);
    return STAT_TO_SHORT;
  }
    if (strlen (argv[2]) >= BUFFER_WORD_SIZE || strlen(argv[3]) >= BUFFER_WORD_SIZE) {
    show_error(STAT_TO_LONG);
    return STAT_TO_LONG;
  }
    
  // Übergabeparameter Codetyp abfragen konvertieren, Ende bei Fehler
  if (strcmp (argv[1], "-k") == 0) {
    phoneconvert_cologne(argv[2], code1);
    phoneconvert_cologne(argv[3], code2);
  }
  else if (strcmp (argv[1], "-p") == 0) {
    phoneconvert_phonem(argv[2], code1);
    phoneconvert_phonem(argv[3], code2);
  }
  else if (strcmp (argv[1], "-s") == 0) {
    phoneconvert_soundex(argv[2], code1);
    phoneconvert_soundex(argv[3], code2);
  }
  else if (strcmp (argv[1], "-e") == 0) {
    phoneconvert_exsoundex(argv[2], code1);
    phoneconvert_exsoundex(argv[3], code2);
  }
  else {
    show_error (STAT_PARAM_ERR);
    return STAT_PARAM_ERR;
  }
  
  // Ende wenn Fehler bei Codeerstellung
  if ((code1[0] == '\0') || (code2[0] == '\0')) {
    show_error(STAT_NO_CODE);
    return STAT_NO_CODE;
  }
    
  // Ausgabe != oder == und Exitcode
  if (strcmp (code1, code2) == 0) {
    printf("%s == %s\n", argv[2], argv[3]);
    return STAT_EQUAL;
  }
  else {
    printf("%s != %s\n", argv[2], argv[3]);
    return STAT_NOT_EQUAL;
  }
}
