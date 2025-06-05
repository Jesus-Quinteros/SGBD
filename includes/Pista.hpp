#ifndef PISTA_HPP
#define PISTA_HPP

#include <vector>
#include "Bloque.hpp"

class Pista {
private:
  int id;
  std::vector<Bloque> bloques;

public:
  Pista(int id, const std::vector<Bloque>& bloques);
  int getId() const;
  const std::vector<Bloque>& getBloques() const;
};

#endif
