#ifndef _LISTA_GENERICA_
#define _LISTA_GENERICA_

typedef struct celulag
{
  void *info;
  struct celulag *urm;
} TCelulaG, *TLG, **ALG;

typedef unsigned int (*TFHash)(const void*, size_t, size_t);

typedef int (*TFCmp)(void *, void *);//functie de comparare a doua chei

typedef void (*AfiKey)(void *, FILE *);//functie de afisare a unei chei

typedef void (*TF)(void *, FILE *);//functie de afisare/eliberare a unei valori

typedef void (*TFE)(void *);

unsigned int hash_f(const void *data, size_t len, size_t range);

#endif
