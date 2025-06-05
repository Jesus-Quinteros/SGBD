#include "Plato.hpp"

Plato::Plato(int id, const std::vector<Superficie>& superficies) : id(id), superficies(superficies) {}

int Plato::getId() const {
  return id;
}

const std::vector<Superficie>& Plato::getSuperficies() const {
  return superficies;
}
