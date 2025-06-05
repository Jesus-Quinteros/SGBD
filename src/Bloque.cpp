#include "Bloque.hpp"

Bloque::Bloque(int id, const std::vector<Sector>& sectores) : id(id), sectores(sectores) {}

int Bloque::getId() const {
  return id;
}

const std::vector<Sector>& Bloque::getSectores() const {
  return sectores;
}

