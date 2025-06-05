#ifndef SECTOR_HPP
#define SECTOR_HPP

#include <string>

class Sector {
private:
  int id;
  int tamanoBytes;
  std::string rutaArchivo;

public:
  Sector(int id, int tamanoBytes, const std::string& rutaArchivo);
  int getId() const;
  int getTamanoBytes() const;
  const std::string& getRutaArchivo() const;
};

#endif
