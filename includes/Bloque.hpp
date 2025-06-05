#ifndef BLOQUE_HPP
#define BLOQUE_HPP

#include <vector>
#include "Sector.hpp"

class Bloque {
private:
    int id;
    std::vector<Sector> sectores;

public:
    Bloque(int id, const std::vector<Sector>& sectores);
    int getId() const;
    const std::vector<Sector>& getSectores() const;
};

#endif
