#include "Superficie.hpp"

Superficie::Superficie(int id, const std::vector<Pista>& pistas) : id(id), pistas(pistas) {}

int Superficie::getId() const {
  return id;
}

const std::vector<Pista>& Superficie::getPistas() const {
  return pistas;
}
