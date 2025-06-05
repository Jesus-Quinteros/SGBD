#ifndef SUPERFICIE_HPP
#define SUPERFICIE_HPP

#include <vector>
#include "Pista.hpp"

class Superficie {
private:
  int id;
  std::vector<Pista> pistas;

public:
  Superficie(int id, const std::vector<Pista>& pistas);
  int getId() const;
  const std::vector<Pista>& getPistas() const;
};

#endif
