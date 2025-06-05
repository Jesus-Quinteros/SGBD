#include <iostream>
#include <filesystem>
#include "Disco.hpp"

int main() {
  int opcion;
  Disco disco;

  while (true) {
    std::cout << "===== MENU =====\n";
    std::cout << "1. Crear Disco\n";
    std::cout << "2. Consultar Informacion del Disco\n";
    std::cout << "3. Consultar Registros\n";
    std::cout << "0. Salir\n";
    std::cout << "Opcion: ";
    std::cin >> opcion;

    if (opcion == 1) {
      Disco nuevoDisco;
      nuevoDisco.crearDesdeConsola();
    } else if (opcion == 2 || opcion == 3) {
      std::string nombreDisco;
      Disco disco;
      bool cargado = false;

      while (!cargado) {
        std::cout << "\nIngrese el nombre del disco a usar: ";
        std::cin >> nombreDisco;
        if (std::filesystem::exists(nombreDisco)) {
          disco.cargarDesdeDirectorio(nombreDisco);
          cargado = true;
        } else {
          std::cout << "\nEl disco no existe. Intente de nuevo.\n";
        }
      }

      if (opcion == 2) {
        disco.consultarInformacion();
      } else {
        disco.menuInsercion();
      }
    } else if (opcion == 0) {
      break;
    } else {
      std::cout << "\nOpcion invalida.\n";
    }
  }

  return 0;
}
