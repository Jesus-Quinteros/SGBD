#ifndef PLATO_HPP
#define PLATO_HPP

#include <vector>
#include "Superficie.hpp"

class Plato {
private:
  int id;
  std::vector<Superficie> superficies;

public:
  Plato(int id, const std::vector<Superficie>& superficies);
  int getId() const;
  const std::vector<Superficie>& getSuperficies() const;
};

#endif
