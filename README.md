# phonepack-de

Programmpack für Linux Nutzer die auch mit einem Terminal klar kommen zur phonetischen Namens-, Wortsuche in Textdaten.
Getestet mit PC Ubuntu Studio 13.10 64 Bit, Lappi Lubuntu 14.10 32 Bit, RaspberryPi Debian Wheezy 32 Bit... alles jeweils mit UTF-8 deutsches System.

#### Enthalten sind 3 Programme:
- phonecode-de zum Erzeugen von phonetischen Codes zu Namen/Worten
- phonecomp-de zum Vergleichen von zwei Namen/Worten auf phonetische Gleichheit
- phoneshow-de zum Durchsuchen von Text auf phonetisch ähnliche Namen/Worte

Implementiert sind die phonetischen Verfahren: Kölner Phonetik, Phonem, Soundex, Extended Soundex.
Mittelpunkt ist das Programm phoneshow-de mit dem man eine phonetische Wort/Namenssuche in einem Text durchführen kann.
Zusätzlich zu den phonetischen Suchverfahren ist noch ein Levenshtein Filter integriert, sodaß man egal was man sucht, so ziemlich alles findet.
Das Programm funktioniert ähnlich "grep" nur daß eben eine phonetische Suche stattfindet.

Die Programme sind auf Grund der phonetischen Verfahren ausschließlich für den **deutschen Sprachraum** angedacht.

*Da die Programme phonecode-de und phonecomp-de ehr simpel gehalten sind, wird auf eine Erklärung verzichtet.
Dafür gehe ich jetzt etwas näher auf das Programm phoneshow-de ein. Zum ersten Test kann die Datei **adressbuch.txt** genutzt werden.*

* Aufrufsyntax: phoneshow-de [Optionen] Name[n] [_Name[n]]
* Textübergabe: Textdaten werden von stdin gelesen und in stdout ausgegeben

##### Optionsschalter Phonetik
- **-k** *Kölner Phonetik:* Für den deutschen Sprachraum das Beste
- **-p** *Phonem:* In manchen dBase Versionen enthalten auch gut für deutsche Namen
- **-s** *Soundex:* Ehr für englische Namen, aber Standard im Genealogiebereich
- **-e** *Extended Soundex:* Erweiterte Soundex Variante

Alle vier Varianten können einzeln oder kombiniert verwendet werden. Ohne Parameterangabe zum Suchverfahren ist die phonetische Suche inaktiv.
- **-l** *Levenshtein-Filter*

Wendet Levenshtein-Filter auf die Worte an, wenn kein phonetisches Verfahren gewählt ist, oder auf den erzeugten phonetischen Code, wenn phonetische Verfahren gewählt. Jedes weitere -l erhöht die zulässige Levenshtein-Distanz um eins.

##### Optionsschalter Ausgabe
- **-a** ganzen Text ausgeben
- **-z** nur Fundzeilen ausgeben
- **-w** nur Fundworte ausgeben

Eine der 3 Ausgabevarianten kann gewählt werden. Ohne Parameterangabe zur Ausgabe ist -z aktiv.

##### Optionsschalter Formatierung
- **-n** Zeilennummerierung einschalten
- **-f** Farbausgabe abschalten (z.B. bei Weiterleitung in eine Datei)
- **-x** Ausgabe der Legende abschalten (z.B. bei Weiterleitung in eine Datei)

Ohne Parameterangabe sind Farbausgabe und Ausgabe der Legende ein- und Zeilennummerierung ausgeschaltet.

## Verwendung
- phoneshow-de Müller

Findet NUR Müller, MÜLLER und müller Zwischen Groß- und Kleinschreibung wird nicht unterschieden.
- phoneshow-de -l Müller

Findet alle Worte die sich in max. einem Buchstaben unterscheiden, einen Buchstaben mehr oder
einen weniger haben. Eine phonetische Suche findet hier nicht statt. Findet Mülker Rüller Kmüller
- phoneshow-de -l -l Müller

Jedes -l erhöht die Anzahl in der sich die Namen unterscheiden dürfen. Mit -l -l werden auch Worte
gefunden die sich um zwei unterscheiden, also 2 Buchstaben mehr, weniger oder andere haben.
Ebenfalls ohne phonetische Suche. Findet Mücker Küllar Müxxer Müllcker
Sinnvoll sind max. 2-3 -l Mehr ist möglich aber nicht sehr zweckdienlich.
- phoneshow-de -k Müller

Findet alle Namen die nach Kölner Phonetik ähnlich zu Müller sind:
Müller, Mueller, Myller, Müllar, Mülherr, Nüller, Nyller
- phoneshow-de -p Müller

Das selbe aber mit phonetischem Suchverfahren Phonem:
Müller, Mueller, Myller, Müllar, Mülherr werden damit gefunden, Nüller und Nyller aber nicht.
- phoneshow-de -k -p -s -e Müller

Wendet alle phonetischen Verfahren an: Kölner Phonetik, Phonem, Soundex, Extended Soundex.
Damit werden so ziemlich alle Varianten gefunden.
- phoneshow-de -k -l Müller

Wendet Suchverfahren Kölner Phonetik an und schiebt den erzeugten Code durch ein Levenshtein Filter.
Der phonetische Code für Müller ist in dem Fall 657. Nun werden alle Worte gefunden deren Code sich
an einer Stelle unterscheidet oder deren Code ein Zeichen mehr oder weniger hat.
Somit werden auch Worte gefunden die den Code 667 oder 6578 oder 57 erzeugen. Auch hier gilt:
Jedes zusätzliche -l erhört die Zahl an möglichen Abweichungen. 2-3 -l sind OK. mehr sinnlos.

#### Name ausschließen mit _Name:
Bei der Suche können EXAKTE Schreibweisen ausgeschlossen werden, wenn man ein _ voranstellt.
- phoneshow-de -k -p Müller_Mueller

Zeigt alle Namen die phonetisch ähnlich zu Müller sind, schließ aber die Schreibweise Mueller aus.
Bei den verbotenen Namen wird zwischen Groß- und Kleinschreibung unterschieden. Das heißt:
Gefunden werden Müller Möller Myller NICHT Mueller ABER mueller schon (weil Kleingeschrieben).
- phoneshow-de Müller_Müller

Zeigt überhaupt nichts weil die phonetische Suche inaktiv ist.
- phoneshow-de -k Müller_Müller

Zeigt alle phonetisch ähnlichen zu Müller, aber nicht die Schreibweise Müller selbst.

### Praxisbeispiele:
- cat adressbuch.txt | phoneshow-de -k -p -s -e Udhe

Soll uns alle Udhe und ähnliche zeigen. Allerdings findet man auch hunderte Otto, die stören und nicht benötigt werden.
- cat adressbuch.txt | phoneshow-de -k -p Udhe _Otto

Filtert alle Otto aus und wir bekommen weniger Funde die schon ehr zielführend sind.
- cat adressbuch.txt | phoneshow-de -k -p -s -e -n -f Thielemann > fundzeilen.txt

Nutz alle phonetischen Suchverfahren, schaltet Zeilennummerierung ein, Farbe aus und schreibt
alle Zeilen in denen sich etwas ähnliches findet nummeriert in fundzeilen.txt.
- cat adressbuch.txt | phoneshow-de -k -p -s -e -w -f -x Meier Müller Schulz | sort | uniq -c

Findet alle phonetisch ähnlichen zu Meier Müller Schulz.
Schaltet Ausgabe von Farbe und Legende ab, sodaß lediglich die Funde ausgegeben werden.
Leitet durch sort und läßt von uniq zählen, sodaß eine Häufigkeitsliste ausgegeben wird.
