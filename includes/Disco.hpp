#ifndef DISCO_HPP
#define DISCO_HPP

#include <vector>
#include <string>
#include "Plato.hpp"

class Disco {
private:
  std::string nombre;
  int numPlatos;
  int numSuperficies;
  int numPistas;
  int numBloques;
  int numSectores;
  int tamSectorBytes;
  std::vector<Plato> platos;

public:
  Disco();
  void crearDesdeConsola();
  void consultarInformacion();
  void insertarRegistroDesdeCSV(const std::string&);
  void menuInsercion();
  void crearEstructuraJerarquica();
  void cargarDesdeDirectorio(const std::string&);
  void consultarRelacion(const std::string&);
  void consultarRelacionCondicional(
    const std::string&,
    const std::string&,
    const std::string&,
    const std::string&,
    const std::string&);
  void insertarRegistrosSinCabecera(const std::string&, const std::string&);
};

#endif