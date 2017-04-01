#ifndef PHONE_BUFFSIZES
#define PHONE_BUFFSIZES

// Puffergröße ggf. einfach erhöhen
// Sollte aber für normale Textdateien reichen
#define BUFFER_SIZE_LINE        4096
#define BUFFER_SIZE_WORD        64
// Faktor nicht ändern
#define BUFFER_SIZE_CODE        (BUFFER_SIZE_WORD * 4)


#endif // PHONE_BUFFSIZES
