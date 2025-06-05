#include "Sector.hpp"

Sector::Sector(int id, int tamanoBytes, const std::string& rutaArchivo)
  : id(id), tamanoBytes(tamanoBytes), rutaArchivo(rutaArchivo) {}

int Sector::getId() const {
  return id;
}

int Sector::getTamanoBytes() const {
  return tamanoBytes;
}

const std::string& Sector::getRutaArchivo() const {
  return rutaArchivo;
}
