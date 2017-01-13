
/* Cambiar el archivo a incluir para probar las distintas políticas de planificación.
   Los posibles archivos son:
   * coremapclock.h
   * coremapfifo.h
   * coremaprandom.h
   * coremapLRU.h    --> Nota: Para probar el algoritmo LRU, hay que hacer una pequeña
   *                           modificación en 'machine'. Hay que descomentar las lineas
   *                           256-258 de translate.cc
*/

#include "coremapfifo.h"

