/*
phosude -> PHOnetische SUche DEutsch
Führt eine Phonetische Suche in Text durch bzw. konvertiert Text in phonetischen Code

Implementierte Phonetische Verfahren sind:
Kölner Phonetik   für deutsche Namen nach eigener Erfahrung am optimalsten
                  Code Numerisch, variable Länge
Phonem            in versch. dBase Varianten implementiert
                  Code Buchstaben, variable Länge
Soundex           Für deutsche Namen nicht ganz geeignet, aber Standard bei Genealogie(programmen)
                  Alphanumerischer Code, fixe Länge, Format Buchstabe gefolgt von 3 Ziffern
Extended Soundex  Erweiterung von Soundex
                  Numerischer Code, fixe Länge, 5 Ziffern
Caverphone v2.0   Alphabetischer Code mit fixer Länge von 10. Füllzeichen wenn unter 10 = "1"
                  Sehr auf Englisch ausgelegt, macht aber mit deutschen Namen auch keinen schlechten Eindruck
Daitch-Mokotoff   Soundex-Variante, Numerischer Code fixer Länge von 6 Ziffern
                  kann allerdings mehrere Codevarianten erstellen
                  Gut für osteuropäische, baltische, schlesische, jüdische Namen geeignet

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

#include <stdlib.h>
#include <stdio.h>
#include "phosude.h"
#include "src_io/cmdparse.h"
#include "src_io/textprint.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Private Funktionen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void show_help (void)
{
  printf(
    "phosude:         Phonetische Codierung und Suche von Namen/Wörtern in Text\n"
    "Aufruf           phosude [Name[n]] [_Name[n]] [Optionen]\n"
    "Übergabe         Textdaten von stdin\n"
    "Rückgabe         Textdaten formatiert in stdout\n"
    "Ohne Parameter   Es werden aus dem Übergabetext alle Wörter extrahiert und diese in\n"
    "                 alle phonetischen Verfahren \"übersetzt\" ausgegeben.\n"
    "Mit Parameter    Es wird im Übergabetext nach phonetisch ähnlichen Namen/Wörtern\n"
    "                 gesuchsucht und diese formatiert ausgegeben.\n"
    "                 Ein Suchname -wort Pflicht, Rest optional, Reihenfolge egal\n"
    "Name[n]          Ein oder mehrere Namen nach denen gesucht werden soll\n"
    "_Name[n]         Ein oder mehrere Namen die bei der Suche ausgelassen werden sollen\n"
    "Phonetische   -K Kölner Phonetik: Für deutsche Namen am Besten.\n"
    "Suchverfahren -P Phonem: Ebenfalls gut für deutsche Namen geeignet.\n"
    "              -S Soundex: Ehr für englische Namen geeignet.\n"
    "              -E Extended Soundex: Erweiterte Soundex Variante.\n"
    "              -C Caverphone: Ebenfalls mehr für englische Namen.\n"
    "              -D Daitch–Mokotoff: Gut für osteuropäische Namen.\n"
    "                 Alle 6 Varianten können einzeln oder kombiniert verwendet werden.\n"
    "                 Ohne Parameterangabe zum Suchverfahren ist die phonetische Suche inaktiv.\n"
    "Längenangaben    Maximal- und/oder Minimallänge für die Worte können festgelegt werden\n"
    "         --min=X Nur Funde ausgeben die Mindestens X Buchstaben Lang sind.\n"
    "         --max=X Nur Funde ausgeben die Höchstens X Buchstaben Lang sind.\n"
    "Levenshtein   -l Wendet Levenshtein-Filter auf die Worte an, wenn kein phonetisches Verfahren\n"
    "Distanz          gewählt ist, oder auf den erzeugten phonetischen Code, wenn angegeben.\n"
    "                 Jedes weitere -l erhöht die zulässige Levenshtein-Distanz um eins.\n"
    "Ausgabe       -a ganzen Text ausgeben\n"
    "              -z nur Fundzeilen ausgeben\n"
    "              -w nur Fundworte ausgeben\n"
    "              -c zu Funden die Positionen innerhalb der Zeilen und Längen ausgeben.\n"
    "                 Fundzeile(1...) Fundposition(1...) Wortlänge(in 'char' gemessen)\n"
    "                 Eine der 4 Ausgabevarianten kann gewählt werden.\n"
    "                 Ohne Parameterangabe zur Ausgabe ist -z aktiv.\n"
    "Verbose Modus -v Textausgabe erfolgt incl. Analyseansicht\n"
    "Nummerierung  -n Zeilennummerierung einschalten\n"
    "Farbe         -f Farbausgabe abschalten         (z.B. bei Weiterleitung in eine Datei)\n"
    "Legende       -x Ausgabe der Legende abschalten (z.B. bei Weiterleitung in eine Datei)\n"
    "Hilfe         -h Zeigt diese Hilfe an\n"
  );
  
  printf("Exitcode       %i keine Fehler\n", STAT_SUCCESS);
  printf("               %i wenn Name zu kurz zum sinnvollen codieren\n", STAT_ERR_NAME_UNDERLEN);
  printf("               %i wenn Name zu lang\n", STAT_ERR_NAME_OVERLEN);
  printf("               %i wenn unerlaubte Zeichen im Namen enthalten sind\n", STAT_ERR_NAME_NO_WORD);
  printf("               %i wenn falsche Aufrufparameter\n", STAT_ERR_PARAM);
  printf("               %i wenn von stdin Textzeile in Überlänge empfangen wurde\n", STAT_ERR_REC_OVERLEN_LINE);
  printf("               %i wenn von stdin Wort in Überlänge empfangen wurde\n", STAT_ERR_REC_OVERLEN_WORD);
  printf("               %i wenn es bei Speicheranfordeung zu Problemen kam\n", STAT_ERR_MEM);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void show_err_msg (const int err_no)
{
  if (err_no == STAT_ERR_NAME_UNDERLEN) 
    fprintf(stderr, "phosude: Name zu kurz zur sinnvollen Suche !\n");
  else if (err_no == STAT_ERR_NAME_OVERLEN) 
    fprintf(stderr, "phosude: Name zu lang !\n");
  else if (err_no == STAT_ERR_NAME_NO_WORD) 
    fprintf(stderr, "phosude: Name enthält Sonderzeichen !\n");
  else if (err_no == STAT_ERR_REC_OVERLEN_LINE) 
    fprintf(stderr, "phosude: Suche Abgebrochen: Zeile von stdin hat Überlänge !\n");
  else if (err_no == STAT_ERR_REC_OVERLEN_WORD) 
    fprintf(stderr, "phosude: Suche Abgebrochen: Wort von stdin hat Überlänge !\n");
  else if (err_no == STAT_ERR_MEM) 
    fprintf(stderr, "phosude: Fehler bei Speicheranforderung !\n");
  else if (err_no == STAT_ERR_PARAM) 
    fprintf(stderr,  "phosude: Falscher Aufrufparameter !\n"
                     "Aufruf:  phosude [Name[n]] [_Name[n]] [Optionen]\n"
                     "Hilfe:   phosude -h\n");
  else if (err_no == STAT_ERR_HELP) show_help();
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main (int argc, char* argv[])
{
  nameslist_t list = {0, NULL};     // Namensliste mit allen Zusatzinfos
  phops_t     phops;                // Optionen Phonetik
  frmops_t    frmops;               // Optionen Formatierung
  outops_t    outops;               // Optionen Ausgabemodus
  int         stat;                 // Stausnummer... wird ans BS durchgereicht

  // Falls keine Argumente übergeben, Konvertierroutine aufrufen
  // Gibt STAT_SUCCESS oder Fehlerkonstannte zurück
  if (argc == 1) {
    stat = printout_convert();
    if (stat != STAT_SUCCESS) show_err_msg (stat);
    return stat;
  }
  
  // Optionen aus Übergabe erstellen, gibt STAT_SUCCESS oder Fehlerkonstannte zurück
  stat = parse_options (argc, argv, &phops, &frmops, &outops);
  if (stat != STAT_SUCCESS) {
    show_err_msg (stat);
    return stat;
  }
  
  // Namensliste (list) füllen, Funktion gibt STAT_SUCCESS oder Fehlerkonstannte zurück 
  stat = parse_nameslist (argc, argv, &list);
  if (stat != STAT_SUCCESS) {
    if (list.items != NULL) free (list.items);
    show_err_msg (stat);
    return stat;
  }
  
  // Anzeigeroutine anspringen, wenn mehrere gewählt
  // gewinnt die, mit mehr Textausgabe... Funktionen liefern STAT_SUCCESS oder Fehlerkonstannte zurück
  if (outops.a == true) stat = printout_a (&list, &phops, &frmops);
  else if (outops.z == true) stat = printout_z (&list, &phops, &frmops);
  else if (outops.w == true) stat = printout_w (&list, &phops, &frmops);
  else stat = printout_c (&list, & phops, &frmops);
  
  // Ende mit oder ohne Fehler
  if (stat != STAT_SUCCESS) show_err_msg(stat);
  if (list.items != NULL) free (list.items);
  return stat;
  
}
