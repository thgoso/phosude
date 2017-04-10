# phosude
## Phonetische Suche Deutsch
Für Linux Nutzer die auch mit einem Terminal klar kommen zur phonetischen Namens-, Wortsuche in Textdaten bzw. zum phonetischen codieren von Texten.
Getestet mit PC Ubuntu Studio 13.10 64 Bit, Lappi Lubuntu 14.10 32 Bit, RaspberryPi Debian Wheezy 32 Bit... alles jeweils mit UTF-8 deutsches System.

Implementiert sind die phonetischen Verfahren: Kölner Phonetik, Phonem, Soundex, Extended Soundex.
Zusätzlich zu den phonetischen Suchverfahren ist noch ein Levenshtein Filter integriert, sodaß man egal was man sucht, so ziemlich alles findet.
Längenangaben können die Suche ebenfalls eingrenzen. Das Programm funktioniert ähnlich "grep" nur daß eben eine phonetische Suche stattfindet.

Das Programm ist auf Grund der phonetischen Verfahren ausschließlich für den **deutschen Sprachraum** angedacht.

* Aufrufsyntax: phosude
* Aufrufsyntax: phosude Name[n] [_Name[n]] [Optionen]
* Textübergabe: Textdaten werden von stdin gelesen und in stdout ausgegeben

#### Parameterloser Aufruf
Eingabetext von stdin wird weitergeleitet an stdout. Zusätzlich werden alle Wörter extrahiert und diese in alle phonetischen Verfahren "übersetzt" ausgegeben.
#### Aufruf mit Parametern
Eingabetext von stdin wird durchsucht und bearbeitet in stdout ausgegeben. Mindestens ein Name ist Pfilch, Rest optional, Reihenfolge Parameter/Namen egal.

##### Optionsschalter Phonetik
- **-K** *Kölner Phonetik:* Für den deutschen Sprachraum das Beste
- **-P** *Phonem:* In manchen dBase Versionen enthalten auch gut für deutsche Namen
- **-S** *Soundex:* Ehr für englische Namen, aber Standard im Genealogiebereich
- **-E** *Extended Soundex:* Erweiterte Soundex Variante

Alle vier Varianten können einzeln oder kombiniert verwendet werden. Ohne Parameterangabe zum Suchverfahren ist die phonetische Suche inaktiv.
- **-l** *Levenshtein-Filter*

Wendet Levenshtein-Filter auf die Worte an, wenn kein phonetisches Verfahren gewählt ist, oder auf den erzeugten phonetischen Code, wenn phonetische Verfahren gewählt. Jedes weitere -l erhöht die zulässige Levenshtein-Distanz um eins.

- **--min=X** *Mindestbegrenzung*
- **--max=X** *Höchstbegrenzung*

Längenangaben Maximal- und/oder Minimallänge für die Worte können festgelegt werden um die Suche weiter einzugrenzen.

##### Optionsschalter Ausgabe
- **-a** ganzen Text ausgeben
- **-z** nur Fundzeilen ausgeben
- **-w** nur Fundworte ausgeben
- **-c** zu Funden Positionen ausgeben

Option -c funktioniert wie -w jedoch werden statt der Worte die Positionen und Längen innerhalb einer Zeile ausgegeben. Fundzeile(1...) Fundposition(1...) Wortlänge(in 'char' gemessen)

Eine der 4 Ausgabevarianten kann gewählt werden. Ohne Parameterangabe zur Ausgabe ist -z aktiv. Zusätzlich kann der Verbose-Modus eingeschaltet werden.
- **-v** Analysedaten anzeigen

##### Optionsschalter Formatierung
- **-n** Zeilennummerierung einschalten
- **-f** Farbausgabe abschalten (z.B. bei Weiterleitung in eine Datei)
- **-x** Ausgabe der Legende abschalten (z.B. bei Weiterleitung in eine Datei)

Ohne Parameterangabe sind Farbausgabe und Ausgabe der Legende ein- und Zeilennummerierung ausgeschaltet.

## Verwendung
```
phosude Müller
```
Findet NUR Müller, MÜLLER und müller Zwischen Groß- und Kleinschreibung wird nicht unterschieden.
```
phosude -l Müller
```
Findet alle Worte die sich in max. einem Buchstaben unterscheiden, einen Buchstaben mehr oder
einen weniger haben. Eine phonetische Suche findet hier nicht statt. Findet Mülker Rüller Kmüller
```
phosude -l -l Müller
```
Jedes -l erhöht die Anzahl in der sich die Namen unterscheiden dürfen. Mit -l -l werden auch Worte
gefunden die sich um zwei unterscheiden, also 2 Buchstaben mehr, weniger oder andere haben.
Ebenfalls ohne phonetische Suche. Findet Mücker Küllar Müxxer Müllcker
Sinnvoll sind max. 2-3 -l Mehr ist möglich aber nicht sehr zweckdienlich.
```
phosude -K Müller
```
Findet alle Namen die nach Kölner Phonetik ähnlich zu Müller sind:
Müller, Mueller, Myller, Müllar, Mülherr, Nüller, Nyller
```
phosude -P Müller
```
Das selbe aber mit phonetischem Suchverfahren Phonem:
Müller, Mueller, Myller, Müllar, Mülherr werden damit gefunden, Nüller und Nyller aber nicht.
```
phosude -K -P -S -E Müller --min=5 --max=10
```
Wendet alle phonetischen Verfahren an: Kölner Phonetik, Phonem, Soundex, Extended Soundex.
Damit werden so ziemlich alle Varianten gefunden. Allerdings nur diese mit 5 bis 10 Buchstaben.
```
phosude -K -l Müller
```
Wendet Suchverfahren Kölner Phonetik an und schiebt den erzeugten Code durch ein Levenshtein Filter.
Der phonetische Code für Müller ist in dem Fall 657. Nun werden alle Worte gefunden deren Code sich
an einer Stelle unterscheidet oder deren Code ein Zeichen mehr oder weniger hat.
Somit werden auch Worte gefunden die den Code 667 oder 6578 oder 57 erzeugen. Auch hier gilt:
Jedes zusätzliche -l erhört die Zahl an möglichen Abweichungen. 2-3 -l sind OK. mehr sinnlos.

#### Name ausschließen mit _Name:
Bei der Suche können **EXAKTE** Schreibweisen ausgeschlossen werden, wenn man ein _ voranstellt.
```
phosude -K -P Müller_Mueller
```
Zeigt alle Namen die phonetisch ähnlich zu Müller sind, schließ aber die Schreibweise Mueller aus.
Bei den verbotenen Namen wird zwischen Groß- und Kleinschreibung unterschieden. Das heißt:
Gefunden werden Müller Möller Myller NICHT ~~Mueller~~ ABER mueller schon (weil Kleingeschrieben).
```
phosude Müller_Müller
```
Zeigt überhaupt nichts weil die phonetische Suche inaktiv ist.
```
phosude -K Müller_Müller
```
Zeigt alle phonetisch ähnlichen zu Müller, aber nicht die Schreibweise Müller selbst.

### Praxisbeispiele:
```
cat adressbuch.txt | phosude -K -P -S -E Udhe
```
Soll uns alle Udhe und ähnliche zeigen. Allerdings findet man auch hunderte Otto, die stören und nicht benötigt werden.
```
cat adressbuch.txt | phosude -K -P -S -E Udhe _Otto
```
Filtert alle Otto aus und wir bekommen weniger Funde die schon ehr zielführend sind.
```
cat adressbuch.txt | phosude -K -P -S -E -n -f Thielemann > fundzeilen.txt
```
Nutz alle phonetischen Suchverfahren, schaltet Zeilennummerierung ein, Farbe aus und schreibt
alle Zeilen in denen sich etwas ähnliches findet nummeriert in fundzeilen.txt.
```
cat adressbuch.txt | phosude -K -P -S -E -w -x Meier Müller Schulz | sort | uniq -c
```
Findet alle phonetisch ähnlichen zu Meier Müller Schulz.
Schaltet Ausgabe von Legende ab, sodaß lediglich die Funde ausgegeben werden.
Leitet durch sort und läßt von uniq zählen, sodaß eine Häufigkeitsliste ausgegeben wird.
```
cat adressbuch.txt | phosude | egrep "^Worte:|^CodeP:"
```
Codiert den eingehenden Text, leitet durch egrep und man erhält eine Ausgabe aller im Eingabetext enthaltenen Worte, sowie ihren dazugehörigen Codes des Verahrens Phonem.
```
phosude
```
Programm wartet auf Eingabe von stdin. Über die Tastatur gibt man jetzt ein paar Namen ein:
> czerny czernie tschernie scherni scerny schärnü

Es werden die Codes der Namen aller phonetischer Verfahren ausgegeben. Gereade hier sieht man, daß für die deutsche Sprache Kölner Phonetik und Phonem klar überlegen sind, denn sie codieren alle diese Namen gleich.

