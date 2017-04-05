/*
phoneshow
Führt eine Phonetische Suche in Text aus
Suchverfahren sind:
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
#include "bool.h"
#include "string.h"
#include "phonetics.h"
#include "phoneshow_types.h"
#include "phoneshow_write.h"

// Farbstringtabelle zur bunten Ausgabe
#define SIZE_COLORSTR             16
#define NUMBER_OF_COLORS          28
static const char Color_Table[NUMBER_OF_COLORS][SIZE_COLORSTR] = {
  {"\033[1;37m\033[41m"}, {"\033[1;37m\033[42m"}, {"\033[1;37m\033[43m"}, {"\033[1;37m\033[44m"},
  {"\033[1;37m\033[45m"}, {"\033[1;37m\033[46m"}, {"\033[1;37m\033[47m"}, {"\033[1;33m\033[41m"},
  {"\033[1;33m\033[42m"}, {"\033[1;33m\033[43m"}, {"\033[1;33m\033[44m"}, {"\033[1;33m\033[45m"},
  {"\033[1;33m\033[46m"}, {"\033[1;33m\033[47m"}, {"\033[1;36m\033[41m"}, {"\033[1;36m\033[42m"},
  {"\033[1;36m\033[43m"}, {"\033[1;36m\033[44m"}, {"\033[1;36m\033[45m"}, {"\033[1;36m\033[46m"},
  {"\033[1;36m\033[47m"}, {"\033[1;32m\033[41m"}, {"\033[1;32m\033[42m"}, {"\033[1;32m\033[43m"},
  {"\033[1;32m\033[44m"}, {"\033[1;32m\033[45m"}, {"\033[1;32m\033[46m"}, {"\033[1;32m\033[47m"}
};
// Farbstring bei unbunter Ausgabe
static const char Color_Empty[]="";



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void show_help (void)
{
  printf(
    "\n\033[1mphoneshow-de:    Phonetische Suche von Namen/Wörtern in Text\033[m\n"
    "Aufruf           phoneshow-de\n"
    "                 Eingabetext von stdin wird weitergeleitet an stdout. Zusätzlich werden alle Wörter\n"
    "                 extrahiert und diese in alle phonetischen Verfahren \"übersetzt\" ausgegeben.\n"
    "---------------------------------------------------------------------------------------------------\n"
    "Aufruf           phoneshow-de [Optionen] Name[n] [_Name[n]]\n"
    "Übergabe         Textdaten von stdin\n"
    "Rückgabe         Textdaten formatiert in stdout\n"
    "Namen            Ein oder mehrere Namen nach denen gesucht werden soll\n"
    "_Namen           Ein oder mehrere Namen die bei der Suche ausgelassen werden sollen\n"
    "Optionen      -k Kölner Phonetik: für den deutschen Sprachraum das Beste\n"
    "Phonetische   -p Phonem: in manchen dBase Versionen enthalten auch gut für deutsche Namen\n"
    "Suchverfahren -s Soundex: ehr für englische Namen, aber Standard im Genealogiebereich\n"
    "              -e Extended Soundex: erweiterte Soundex Variante\n"
    "                 Alle vier Varianten können einzeln oder kombiniert verwendet werden.\n"
    "                 Ohne Parameterangabe zum Suchverfahren ist die phonetische Suche inaktiv.\n"
    "Option        -l Wendet Levenshtein-Filter auf die Worte an, wenn kein phonetisches Verfahren\n"
    "Levenshtein      gewählt ist, oder auf den erzeugten phonetischen Code, wenn angegeben.\n"
    "Distanz          Jedes weitere -l erhöht die zulässige Levenshtein-Distanz um eins.\n"
    "Optionen      -a ganzen Text ausgeben\n"
    "zur Ausgabe   -z nur Fundzeilen ausgeben\n"
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
    "Hilfe         -b Zeigt Aufrufbeispiele an\n"
    "---------------------------------------------------------------------------------------------------\n" );

  printf("Exitcode       %i wenn mindestens ein phonetisch ähnlicher Name gefunden wurde\n",
                         PHSHOW_MATCH);
  printf("               %i wenn kein phonetisch ähnlicher Name gefunden wurde\n",
                         PHSHOW_NO_MATCH);
  printf("               %i wenn Name zu kurz zum sinnvollen codieren\n",
                         PHSHOW_ERR_NAME_UNDERLEN);
  printf("               %i wenn Name zu lang\n",
                         PHSHOW_ERR_NAME_OVERLEN);
  printf("               %i wenn unerlaubte Zeichen im Namen enthalten sind. Erlaubt sind nur einzelne Worte,\n"
         "                 Buchstaben deutsches Alphabet incl. Umlaute. Keine Leerzeichen oder Satzzeichen.\n",
                         PHSHOW_ERR_NAME_NOT_GERMAN);
  printf("               %i wenn falsche Aufrufparameter\n",
                         PHSHOW_ERR_PARAM);
  printf("               %i wenn von stdin Textzeile in Überlänge empfangen wurde\n",
                         PHSHOW_ERR_LINE_OVERLEN);
  printf("               %i wenn von stdin Wort in Überlänge empfangen wurde\n",
                         PHSHOW_ERR_WORD_OVERLEN);
  printf("               %i wenn es bei Speicheranfordeung zu Problemen kam\n",
                         PHSHOW_ERR_MEM);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void show_examples (void)
{
  printf(
    "\n\033[1mphoneshow-de: Phonetische Suche von Namen/Wörtern in Text\033[m\n"
    "Beispiele Suche:\n"
    "\033[7mphoneshow-de Müller\033[m\n"
    "Findet NUR Müller, MÜLLER und müller Zwischen Groß- und Kleinschreibung wird nicht unterschieden.\n"
    "\033[7mphoneshow-de -l Müller\033[m\n"
    "Findet alle Worte die sich in max. einem Buchstaben unterscheiden, einen Buchstaben mehr oder\n"
    "einen weniger haben. Eine phonetische Suche findet hier nicht statt. Findet Mülker Rüller Kmüller\n"
    "\033[7mphoneshow-de -l -l Müller\033[m\n"
    "Jedes -l erhöht die Anzahl in der sich die Namen unterscheiden dürfen. Mit -l -l werden auch Worte\n"
    "gefunden die sich um zwei unterscheiden, also 2 Buchstaben mehr, weniger oder andere haben.\n"
    "Ebenfalls ohne phonetische Suche. Findet Mücker Küllar Müxxer Müllcker\n"
    "Sinnvoll sind max. 2-3 -l Mehr ist möglich aber nicht sehr zweckdienlich.\n"
    "\033[7mphoneshow-de -k Müller\033[m\n"
    "Findet alle Namen die nach Kölner Phonetik ähnlich zu Müller sind:\n"
    "Müller, Mueller, Myller, Müllar, Mülherr, Nüller, Nyller\n"
    "\033[7mphoneshow-de -p Müller\033[m\n"
    "Das selbe aber mit phonetischem Suchverfahren Phonem:\n"
    "Müller, Mueller, Myller, Müllar, Mülherr werden damit gefunden, Nüller und Nyller aber nicht.\n"
    "\033[7mphoneshow-de -k -p -s -e Müller\033[m\n"
    "Wendet alle phonetischen Verfahren an: Kölner Phonetik, Phonem, Soundex, Extended Soundex.\n"
    "Damit werden so ziemlich alle Varianten gefunden.\n"
    "\033[7mphoneshow-de -k -l Müller\033[m\n"
    "Wendet Suchverfahren Kölner Phonetik an und schiebt den erzeugten Code durch ein Levenshtein Filter.\n"
    "Der phonetische Code für Müller ist in dem Fall 657. Nun werden alle Worte gefunden deren Code sich\n"
    "an einer Stelle unterscheidet oder deren Code ein Zeichen mehr oder weniger hat.\n"
    "Somit werden auch Worte gefunden die den Code 667 oder 6578 oder 57 erzeugen. Auch hier gilt:\n"
    "Jedes zusätzliche -l erhört die Zahl an möglichen Abweichungen. 2-3 -l sind OK. mehr sinnlos.\n\n"
    "Name ausschließen mit _Name:\n"
    "Bei der Suche können EXAKTE Schreibweisen ausgeschlossen werden, wenn man ein _ voranstellt.\n"
    "\033[7mphoneshow-de -k -p Müller_Mueller\033[m\n"
    "Zeigt alle Namen die phonetisch ähnlich zu Müller sind, schließ aber die Schreibweise Mueller aus.\n"
    "Bei den verbotenen Namen wird zwischen Groß- und Kleinschreibung unterschieden. Das heiß:\n"
    "Gefunden werden Müller Möller Myller NICHT Mueller ABER mueller schon (weil Kleingeschrieben).\n"
    "\033[7mphoneshow-de Müller_Müller\033[m\n"
    "Zeigt überhaupt nichts weil die phonetische Suche inaktiv ist.\n"
    "\033[7mphoneshow-de -k Müller_Müller\033[m\n"
    "Zeigt alle phonetisch ähnlichen zu Müller, aber nicht die Schreibweise Müller selbst.\n\n"
    "Praxisbeispiele:\n"
    "\033[7mcat datei.txt | phoneshow-de -k -p Udhe\033[m soll uns alle Udhe und ähnliche zeigen.\n"
    "Allerdings findet man auch hunderte Otto, die stören und nicht benötigt werden.\n"
    "\033[7mcat datei.txt | phoneshow-de -k -p Udhe _Otto\033[m\n"
    "Filtert alle Otto aus und wir bekommen weniger Funde die schon ehr zielführend sind.\n"
    "\033[7mcat datei.txt | phoneshow-de -k -p -s -e -n -f Thielemann > fundzeilen.txt\033[m\n"
    "Nutz alle phonetischen Suchverfahren, schaltet Zeilennummerierung ein, Farbe aus und schreibt\n"
    "alle Zeilen in denen sich etwas ähnliches findet nummeriert in fundzeilen.txt.\n"
    "\033[7mcat datei.txt | phoneshow-de -k -p -s -e -w -f -x Meier Müller Schulz | sort | uniq -c\033[m\n"
    "Findet alle phonetisch ähnlichen zu Meier Müller Schulz.\n"
    "Schaltet Ausgabe von Farbe und Legende ab, sodaß lediglich die Funde ausgegeben werden.\n"
    "Leitet durch sort und läßt von uniq zählen, sodaß eine Häufigkeitsliste ausgegeben wird.\n\n");
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Zentrale Exitfunktion
// Übergabe Statuskonstannte... diese wird ans BS zurückgegeben
// Gibt bei Fehlerstatus Fehlermeldung aus
static void exit_func (const int status_no)
{
  if (status_no == PHSHOW_ERR_NAME_UNDERLEN) fprintf(stderr, "phoneshow-de: Name zu kurz zur sinnvollen Suche !\n");
  else if (status_no == PHSHOW_ERR_NAME_OVERLEN) fprintf(stderr, "phoneshow-de: Name zu lang !\n");
  else if (status_no == PHSHOW_ERR_NAME_NOT_GERMAN) fprintf(stderr, "phoneshow-de: Name ungültig !\n");
  else if (status_no == PHSHOW_ERR_MEM) fprintf(stderr, "phoneshow-de: Fehler bei Speicheranforderung !\n");
  else if (status_no == PHSHOW_ERR_LINE_OVERLEN) fprintf(stderr, "phoneshow-de: Suche Abgebrochen !\n"
                                                               "Zeile von stdin hat Überlänge !\n");
  else if (status_no == PHSHOW_ERR_WORD_OVERLEN) fprintf(stderr, "phoneshow-de: Suche Abgebrochen !\n"
                                                               "Wort von stdin hat Überlänge !\n");
  else if (status_no == PHSHOW_ERR_PARAM) {
    fprintf(stderr, "phoneshow-de: Falsche Aufrufparameter !\n"
      "Aufruf:    phoneshow-de\n"
      "Aufruf:    phoneshow-de [Optionen] Name[n] [_Name[n]]\n"
      "Hilfe:     phoneshow-de -h\n"
      "Beispiele: phoneshow-de -b\n\n");
  }
    
  exit (status_no);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Reserviert Speicher für Namensliste und füllt mit default
// Übergabe:  Anzahl Namen
// Rückgabe:  Namensliste
// Bei Fehler Speicherreservierung Sprung zu exit_func und Ende
static nameslist_t create_names_list (const int number_of_names)
{
  int           cnt;
  name_t        *items;
  nameslist_t   list;

  items = (name_t*) malloc( number_of_names * sizeof( name_t ));
  if (items == NULL) exit_func (PHSHOW_ERR_MEM);

  for (cnt = 0; cnt < number_of_names; cnt++) {
    items[cnt].is_minusname=false;
    items[cnt].name_norm.s[0]='\0';
    items[cnt].name_upper.s[0]='\0';
    items[cnt].code_k.s[0]='\0';
    items[cnt].code_p.s[0]='\0';
    items[cnt].code_s.s[0]='\0';
    items[cnt].code_e.s[0]='\0';
    items[cnt].color=Color_Empty;
  }

  list.number_of_names = number_of_names;
  list.items = items;

  return list;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main (int argc, char* argv[])
{
  char        *param;
  int         op_cnt, name_cnt, number_of_names;
  int         idxstart, idxend;
  int         write_retval;
  int         color_cnt=0;
  bool        only_minusname=true;
  nameslist_t list;
  
  // Startwerte Parameter Funktionsintern
  bool        op_f=true;      // Farbausgabe
  bool        op_a=false;     // Ganzen Text ausgeben
  bool        op_z=false;     // Zeilen ausgeben
  bool        op_w=false;     // Worte ausgeben
  bool        op_c=false;     // Positionen ausgeben
  
  // Startwerte Parameter die weitergeleitet werden
  frmops_t    frmops = {
                false,        // Zeilennummerierung
                true,         // Ausgabe Legende
                false         // Verbose Mode
              };
  phops_t     phops = {
                false,        // Kölner Phonetik
                false,        // Phonem Phonetik
                false,        // Soundex Phonetic
                false,        // Exsoundex Phonetik
                0             // erlaubte Lev-Dist
              };

  // Wenn keine Übergabeparameter Text kodieren und schon fertig
  // Funktion liefert IMMER PH_SUCCESS_NO_MATCH oder Fehlercode
  if (argc == 1) {
    write_retval = write_out_convert();
    exit_func (write_retval);
  }

  // Optionen aus Übergabe lesen und die Vars danach setzten
  for (op_cnt=1; op_cnt < argc; op_cnt++) {
    param=argv[op_cnt];
    if (strcmp (param, "-k") == 0) phops.k = true;
    else if (strcmp (param, "-p") == 0) phops.p = true;
    else if (strcmp (param, "-s") == 0) phops.s = true;
    else if (strcmp (param, "-e") == 0) phops.e = true;
    else if (strcmp (param, "-l") == 0) phops.l++;
    else if (strcmp (param, "-n") == 0) frmops.n = true;
    else if (strcmp (param, "-x") == 0) frmops.x = false;
    else if (strcmp (param, "-v") == 0) frmops.v = true;
    else if (strcmp (param, "-a") == 0) op_a = true;
    else if (strcmp (param, "-z") == 0) op_z = true;
    else if (strcmp (param, "-w") == 0) op_w = true;
    else if (strcmp (param, "-c") == 0) op_c = true;
    else if (strcmp (param, "-f") == 0) op_f = false;
    else if (strcmp (param, "-h") == 0) {
      show_help();
      exit_func(PHSHOW_SUCCESS);
    }
    else if (strcmp (param, "-b") == 0) {
      show_examples();
      exit_func(PHSHOW_SUCCESS);
    }
    else break;
  }

  // Wenn kein Anzeigeparameter übergeben (-a -z -w -c) ... -z einschalten
  if (op_a == false && op_z == false && op_w == false && op_c == false) op_z = true;

  // Keine Namen übergeben = Fehler
  if (op_cnt == argc) exit_func (PHSHOW_ERR_PARAM);

  // Index setzten 1. und letzter Name in Argliste, Anzahl
  idxstart = op_cnt;
  idxend = argc - 1;
  number_of_names = (idxend - idxstart) + 1;

  // Speicher für Namen Liste anfordern, (Beendet Prog bei Fehler)
  list = create_names_list (number_of_names);

  // Namensliste füllen
  // Option zur Farbausgabe wird nicht weitergereicht. Elemente in liste haben einen Pointer auf Colorstring
  // Der zeigt auf einen String aus obiger Farbtabelle oder auf obigen Leerstring
  // Der Leerstring kann in den Ausgaberoutinen problemlos angezeit werden, als sei es ein Farbstring
  for (name_cnt = 0; name_cnt < number_of_names; name_cnt++) {
    // Name aus Übergabe holen
    param = argv[name_cnt + idxstart];
    // Ende wenn Name zu kurz oder lang
    if (strlen(param) < 2) {
      free(list.items);
      exit_func (PHSHOW_ERR_NAME_UNDERLEN);
    }
    if (strlen(param) >= BUFFSIZE_WORD) {
      free(list.items);
      exit_func(PHSHOW_ERR_NAME_OVERLEN);
    }
    // Minusnamen... .is_minusname, .name_norm, .name_upper setzten, Rest default lassen
    // Ende wenn kein deutsches Wort
    if (param[0] == '_') {
      list.items[name_cnt].is_minusname = true;
      param++;
      strcpy(list.items[name_cnt].name_norm.s, param);
      strcpy(list.items[name_cnt].name_upper.s, param);
      if (str_to_ascii_upper_word(list.items[name_cnt].name_upper.s) == false) {
        free(list.items);
        exit_func(PHSHOW_ERR_NAME_NOT_GERMAN);
      }
    }
    // Suchnamen, alles setzten, Hilfsvar only_minusname clear
    else {
      only_minusname=false;
      list.items[name_cnt].is_minusname = false;
      strcpy(list.items[name_cnt].name_norm.s, param);
      strcpy(list.items[name_cnt].name_upper.s, param);
      if (str_to_ascii_upper_word(list.items[name_cnt].name_upper.s) == false) {
        free(list.items);
        exit_func (PHSHOW_ERR_NAME_NOT_GERMAN);
      }
      if (op_f == true) {
        list.items[name_cnt].color = Color_Table[color_cnt % NUMBER_OF_COLORS];
        color_cnt++;
      }
      // Immer alle Codes erzeugen, egal welche per Parameter gewählt
      phoneconvert_cologne(&list.items[name_cnt].name_norm, &list.items[name_cnt].code_k);
      phoneconvert_phonem(&list.items[name_cnt].name_norm, &list.items[name_cnt].code_p);
      phoneconvert_soundex(&list.items[name_cnt].name_norm, &list.items[name_cnt].code_s);
      phoneconvert_exsoundex(&list.items[name_cnt].name_norm, &list.items[name_cnt].code_e);
    }
  }

  // Wenn Namensliste NUR aus Minusnamen besteht = Fehler
  if (only_minusname == true) {
    free(list.items);
    exit_func(PHSHOW_ERR_PARAM);
  }

  // Ausgabe starten, wenn mehrere gewählt, gewinnt die mit mehr Textausgabe
  // Funktionen liefern passende Statuskonstannte für exit_func 
  if (op_a == true) write_retval = write_out_a (&list, &phops, &frmops);
  else if (op_z == true) write_retval = write_out_z (&list, &phops, &frmops);
  else if (op_w == true) write_retval = write_out_w (&list, &phops, &frmops);
  else write_retval = write_out_c (&list, &phops, &frmops);

  // Aufäumen und Ende
  free(list.items);
  exit_func(write_retval);
  
  // Wir nie erreicht
  return 255;
}
