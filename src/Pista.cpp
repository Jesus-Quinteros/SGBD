#include "Pista.hpp"

Pista::Pista(int id, const std::vector<Bloque>& bloques) : id(id), bloques(bloques) {}

int Pista::getId() const {
  return id;
}

const std::vector<Bloque>& Pista::getBloques() const {
  return bloques;
}
