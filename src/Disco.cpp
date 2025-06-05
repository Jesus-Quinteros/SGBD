#include "Disco.hpp"
#include <iostream>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <map>
#include <set>
#include <unordered_set>

Disco::Disco() {}

void Disco::crearDesdeConsola() {
  std::cout << "Nombre del disco: ";
  std::cin >> nombre;
  std::cout << "Cantidad de platos: ";
  std::cin >> numPlatos;
  std::cout << "Superficies: ";
  std::cin >> numSuperficies;
  std::cout << "Pistas: ";
  std::cin >> numPistas;
  std::cout << "Bloques: ";
  std::cin >> numBloques;
  std::cout << "Sectores: ";
  std::cin >> numSectores;
  std::cout << "Tamano de sector(bytes): ";
  std::cin >> tamSectorBytes;

  std::filesystem::create_directory(nombre);
  int sectorIdGlobal = 1;

  for (int i = 1; i <= numPlatos; ++i) {
    std::string platoPath = nombre + "/Plato" + std::to_string(i);
    std::filesystem::create_directory(platoPath);
    std::vector<Superficie> superficies;

    for (int j = 1; j <= numSuperficies; ++j) {
      std::string superficiePath = platoPath + "/Superficie" + std::to_string(j);
      std::filesystem::create_directory(superficiePath);
      std::vector<Pista> pistas;

      for (int k = 1; k <= numPistas; ++k) {
        std::string pistaPath = superficiePath + "/Pista" + std::to_string(k);
        std::filesystem::create_directory(pistaPath);
        std::vector<Bloque> bloques;

        for (int b = 1; b <= numBloques; ++b) {
          std::string bloquePath = pistaPath + "/Bloque" + std::to_string(b);
          std::filesystem::create_directory(bloquePath);
          std::vector<Sector> sectores;

          for (int s = 1; s <= numSectores; ++s) {
            std::string sectorPath = bloquePath + "/Sector" + std::to_string(s) + ".txt";
            std::ofstream archivo(sectorPath);
            archivo.close();

            sectores.emplace_back(sectorIdGlobal++, tamSectorBytes, sectorPath);
          }
          bloques.emplace_back(b, sectores);
        }
        pistas.emplace_back(k, bloques);
      }
      superficies.emplace_back(j, pistas);
    }
    platos.emplace_back(i, superficies);
  }
  std::cout << "\nDisco '" << nombre << "' creado exitosamente.\n";

  // Guardar configuración de disco
  std::ofstream config("discos-config.txt", std::ios::app);
  config << nombre << "," << numPlatos << "," << numSuperficies << "," << numPistas << ","
    << numBloques << "," << numSectores << "," << tamSectorBytes << "\n";
  config.close();
}

void Disco::consultarInformacion() {
  if (platos.empty()) {
    std::cout << "\nNo hay un disco creado.\n";
    return;
  }

  int opcion;
  while (true) {
    std::cout << "\n--- Caracteristicas de Disco ---\n";
    std::cout << "1. Cantidad de Partes\n";
    std::cout << "2. Capacidad total del disco\n";
    std::cout << "3. Espacio ocupado y libre\n";
    std::cout << "4. Volver al menu principal\n";
    std::cout << "Opcion: ";
    std::cin >> opcion;

    if (opcion == 1) {
      std::cout << "\nPlatos: " << platos.size() << "\n";
      std::cout << "Superficies por plato: " << numSuperficies << "\n";
      std::cout << "Pistas por superficie: " << numPistas << "\n";
      std::cout << "Bloques por pista: " << numBloques << "\n";
      std::cout << "Sectores por bloque: " << numSectores << "\n";
    } else if (opcion == 2) {
      int totalSectores = numPlatos * numSuperficies * numPistas * numBloques * numSectores;
      long long totalBytes = static_cast<long long>(totalSectores) * tamSectorBytes;
      std::cout << "\nCapacidad total (bytes): " << totalBytes << "\n";
    } else if (opcion == 3) {
      namespace fs = std::filesystem;
      long long totalOcupado = 0;
      for (const auto& entry : fs::recursive_directory_iterator(nombre)) {
        if (entry.is_regular_file()) {
          std::ifstream archivo(entry.path(), std::ios::binary | std::ios::ate);
          if (archivo) {
            std::streamsize size = archivo.tellg();
            if (size > 0) totalOcupado += size;
          }
        }
      }
      int totalSectores = numPlatos * numSuperficies * numPistas * numBloques * numSectores;
      long long totalBytes = static_cast<long long>(totalSectores) * tamSectorBytes;
      long long espacioLibre = totalBytes - totalOcupado;

      std::cout << "\nEspacio ocupado (bytes): " << totalOcupado << "\n";
      std::cout << "Espacio libre (bytes): " << espacioLibre << "\n";
    } else if (opcion == 4) {
      break;
    } else {
      std::cout << "\nOpcion invalida.\n";
    }
  }
}

std::string deducirTipo(const std::string& valor) {
  std::string v = valor;
  std::transform(v.begin(), v.end(), v.begin(), ::tolower); // manejar true/false

  if (v == "true" || v == "false")
    return "bool";

  bool tienePunto = false;
  int i = 0;
  if (v[0] == '-' || v[0] == '+') i = 1;

  for (; i < v.size(); ++i) {
    char c = v[i];
    if (c == '.') {
      if (tienePunto) return "string";
      tienePunto = true;
    } else if (!isdigit(c)) {
      return "string";
    }
  }
  return tienePunto ? "float" : "int";
}

std::vector<std::string> parseCSVLinea(const std::string& linea) {
  std::vector<std::string> resultado;
  std::string campo;
  bool dentroDeComillas = false;

  for (size_t i = 0; i < linea.size(); ++i) {
    char c = linea[i];
    if (c == '"') {
      if (dentroDeComillas && i + 1 < linea.size() && linea[i + 1] == '"') {
        campo += '"';
        ++i;
      } else {
        dentroDeComillas = !dentroDeComillas;
      }
    } else if (c == ',' && !dentroDeComillas) {
      resultado.push_back(campo);
      campo.clear();
    } else {
      campo += c;
    }
  }
  resultado.push_back(campo);
  return resultado;
}

std::map<std::string, std::vector<std::string>> leerMetadata() {
  std::ifstream meta("metadata.txt");
  std::map<std::string, std::vector<std::string>> metadata;
  std::string linea;

  while (std::getline(meta, linea)) {
    size_t sep = linea.find(':');
    if (sep == std::string::npos) continue;

    std::string nombreCSV = linea.substr(0, sep);
    std::string sectoresStr = linea.substr(sep + 1);

    std::vector<std::string> sectores;
    std::stringstream ss(sectoresStr);
    std::string sector;

    while (std::getline(ss, sector, ',')) {
      sectores.push_back(sector);
    }

    metadata[nombreCSV] = sectores;
  }

  return metadata;
}

void actualizarMetadata(const std::string& nombreCSV, const std::vector<std::string>& sectoresUsados) {
  std::map<std::string, std::vector<std::string>> metadata = leerMetadata();
  metadata[nombreCSV] = sectoresUsados;

  std::ofstream meta("metadata.txt");
  for (const auto& par : metadata) {
    meta << par.first << ":";
    for (size_t i = 0; i < par.second.size(); ++i) {
      meta << par.second[i];
      if (i != par.second.size() - 1) meta << ",";
    }
    meta << "\n";
  }
}

void Disco::insertarRegistroDesdeCSV(const std::string& nombreArchivoCSV) {
  std::ifstream archivoCSV(nombreArchivoCSV);
  if (!archivoCSV.is_open()) {
    std::cout << "\nNo se pudo abrir el archivo CSV.\n";
    return;
  }
  std::string cabecera, primeraLinea;
  if (!std::getline(archivoCSV, cabecera)) {
    std::cout << "\nEl archivo CSV está vacío.\n";
    return;
  }
  if (!std::getline(archivoCSV, primeraLinea)) {
    std::cout << "\nEl archivo solo tiene cabecera, no hay registros.\n";
    return;
  }

  // Parsear cabecera y primera línea para obtener atributos y tipos
  std::vector<std::string> atributos = parseCSVLinea(cabecera);
  std::vector<std::string> primeraFila = parseCSVLinea(primeraLinea);

  if (atributos.size() != primeraFila.size()) {
    std::cout << "\nCantidad de atributos y valores en la primera línea no coinciden.\n";
    return;
  }

  std::vector<std::string> tipos;
  for (const auto& valor : primeraFila) {
    tipos.push_back(deducirTipo(valor));
  }

  // Escribir esquema con tipos
  std::ofstream esquema("esquema.txt", std::ios::app);
  std::string nombreBase = nombreArchivoCSV.substr(0, nombreArchivoCSV.find('.'));
  esquema << nombreBase;
  for (size_t i = 0; i < atributos.size(); ++i) {
    esquema << "#" << atributos[i] << "#" << tipos[i];
  }
  esquema << "\n";
  esquema.close();

  // Insertar registros
  std::vector<std::string> registros;

  // Insertar la primera línea ya leída
  std::string procesado;
  for (size_t i = 0; i < primeraFila.size(); ++i) {
    if (i > 0) procesado += "#";
    procesado += primeraFila[i];
  }
  procesado += "\n";
  registros.push_back(procesado);

  // Procesar el resto de las líneas del CSV
  std::string linea;
  while (std::getline(archivoCSV, linea)) {
    std::vector<std::string> campos = parseCSVLinea(linea);
    std::string registroProcesado;
    for (size_t i = 0; i < campos.size(); ++i) {
      if (i > 0) registroProcesado += "#";
      registroProcesado += campos[i];
    }
    registroProcesado += "\n";
    registros.push_back(registroProcesado);
  }

  // Leer metadata existente
  auto metadata = leerMetadata();
  std::set<std::string> sectoresOcupados;
  for (const auto& par : metadata) {
    sectoresOcupados.insert(par.second.begin(), par.second.end());
  }

  std::vector<std::string> sectoresAsignados;
  size_t registroIndex = 0;

  // Insertar registros en sectores libres (no mezclados)
  for (auto& plato : platos) {
    for (auto& superficie : plato.getSuperficies()) {
      for (auto& pista : superficie.getPistas()) {
        for (auto& bloque : pista.getBloques()) {
          for (auto& sector : bloque.getSectores()) {
            std::string idSector = std::to_string(sector.getId());

            // Saltar sectores ya ocupados por otros CSV
            if (sectoresOcupados.count(idSector) > 0) continue;

            std::fstream arch(sector.getRutaArchivo(), std::ios::in | std::ios::out);
            if (!arch) continue;

            arch.seekg(0, std::ios::end);
            size_t usado = arch.tellg();
            arch.seekp(usado);
            size_t disponible = sector.getTamanoBytes() - usado;

            bool sectorUsado = false;
            while (registroIndex < registros.size() && registros[registroIndex].size() <= disponible) {
              arch << registros[registroIndex];
              disponible -= registros[registroIndex].size();
              ++registroIndex;
              sectorUsado = true;
            }

            arch.close();
            if (sectorUsado) {
              sectoresAsignados.push_back(idSector);
            }

            if (registroIndex >= registros.size()) {
              actualizarMetadata(nombreBase, sectoresAsignados);
              std::cout << "Todos los registros han sido insertados.\n";
              return;
            }
          }
        }
      }
    }
  }
  // Registro incompleto
  actualizarMetadata(nombreBase, sectoresAsignados);
  std::cout << "Se insertaron " << registroIndex << " registros. Algunos registros no fueron insertados por falta de espacio.\n";
}

void Disco::insertarRegistrosSinCabecera(const std::string& nombreArchivoSinCabecera, const std::string& nombreRelacionBase) {
  std::ifstream archivo(nombreArchivoSinCabecera);
  if (!archivo.is_open()) {
    std::cout << "No se pudo abrir el archivo '" << nombreArchivoSinCabecera << "' para insertar registros.\n";
    return;
  }

  std::vector<std::string> registros;
  std::string linea;
  while (std::getline(archivo, linea)) {
    std::replace(linea.begin(), linea.end(), ',', '#'); // en caso de que tenga comas
    registros.push_back(linea + "\n");
  }

  archivo.close();

  // Leer metadata existente para evitar sobrescribir sectores ya usados
  auto metadata = leerMetadata();
  std::set<std::string> sectoresOcupados;
  for (const auto& par : metadata) {
    sectoresOcupados.insert(par.second.begin(), par.second.end());
  }

  std::vector<std::string> sectoresAsignados;
  size_t registroIndex = 0;
  std::string nombreBase = nombreArchivoSinCabecera;

  // Insertar registros
  for (auto& plato : platos) {
    for (auto& superficie : plato.getSuperficies()) {
      for (auto& pista : superficie.getPistas()) {
        for (auto& bloque : pista.getBloques()) {
          for (auto& sector : bloque.getSectores()) {
            std::string idSector = std::to_string(sector.getId());

            if (sectoresOcupados.count(idSector) > 0) continue;

            std::fstream arch(sector.getRutaArchivo(), std::ios::in | std::ios::out);
            if (!arch) continue;

            arch.seekg(0, std::ios::end);
            size_t usado = arch.tellg();
            arch.seekp(usado);
            size_t disponible = sector.getTamanoBytes() - usado;

            bool sectorUsado = false;
            while (registroIndex < registros.size() && registros[registroIndex].size() <= disponible) {
              arch << registros[registroIndex];
              disponible -= registros[registroIndex].size();
              ++registroIndex;
              sectorUsado = true;
            }

            arch.close();
            if (sectorUsado) {
              sectoresAsignados.push_back(idSector);
            }

            if (registroIndex >= registros.size()) {
              actualizarMetadata(nombreBase, sectoresAsignados);
              std::cout << "Todos los registros del archivo '" << nombreArchivoSinCabecera << "' fueron insertados correctamente.\n";
              return;
            }
          }
        }
      }
    }
  }

  // Si llegamos aquí, no hubo espacio suficiente para todos
  actualizarMetadata(nombreBase, sectoresAsignados);
  std::cout << "Se insertaron " << registroIndex << " registros del archivo '" << nombreArchivoSinCabecera << "'. Algunos no fueron insertados por falta de espacio.\n";
}

void Disco::menuInsercion() {
  if (platos.empty()) {
    std::cout << "Primero debe crear un disco.\n";
    return;
  }

  int opcion;
  while (true) {
    std::cout << "\n--- Submenu de Insercion ---\n";
    std::cout << "1. Agregar registro completo\n";
    std::cout << "2. Realizar consulta simple (SELECT)\n";
    std::cout << "3. Realizar consulta con condicion (WHERE)\n";
    std::cout << "4. Volver al menu principal\n";
    std::cout << "Opcion: ";
    std::cin >> opcion;

    if (opcion == 1) {
      std::string nombreArchivo;
      std::cout << "\nIngrese el nombre del archivo CSV: ";
      std::cin >> nombreArchivo;
      insertarRegistroDesdeCSV(nombreArchivo);
    } else if (opcion == 2) {
      std::string relacion;
      std::cout << "Ingrese el nombre de la relacion: ";
      std::cin >> relacion;
      consultarRelacion(relacion);
    } else if (opcion == 3) {
      std::string nombreRelacion, atributoCondicion, operador, valorComparar, archivoSalida;
      std::cout << "\nNombre de la relacion: ";
      std::cin >> nombreRelacion;
      std::cout << "Atributo a comparar: ";
      std::cin >> atributoCondicion;
      std::cout << "Operador de condicion: ";
      std::cin >> operador;
      std::cout << "Valor para comparar: ";
      std::cin >> valorComparar;
      std::cout << "Guardar en archivo (nombre): ";
      std::cin >> archivoSalida;
      consultarRelacionCondicional(nombreRelacion, atributoCondicion, operador, valorComparar, archivoSalida);
      break;
    } else if (opcion == 4) {
      break;
    } else {
      std::cout << "\nOpcion invalida.\n";
    }
  }
}

void Disco::crearEstructuraJerarquica() {
  platos.clear();  // Por si ya hay algo cargado
  int sectorIdGlobal = 1;

  for (int i = 1; i <= numPlatos; ++i) {
    std::vector<Superficie> superficies;
    for (int j = 1; j <= numSuperficies; ++j) {
      std::vector<Pista> pistas;
      for (int k = 1; k <= numPistas; ++k) {
        std::vector<Bloque> bloques;
        for (int b = 1; b <= numBloques; ++b) {
          std::vector<Sector> sectores;
          for (int s = 1; s <= numSectores; ++s) {
            std::string sectorPath = nombre + "/Plato" + std::to_string(i) +
                                      "/Superficie" + std::to_string(j) +
                                      "/Pista" + std::to_string(k) +
                                      "/Bloque" + std::to_string(b) +
                                      "/Sector" + std::to_string(s) + ".txt";
            sectores.emplace_back(sectorIdGlobal++, tamSectorBytes, sectorPath);
          }
          bloques.emplace_back(b, sectores);
        }
        pistas.emplace_back(k, bloques);
      }
      superficies.emplace_back(j, pistas);
    }
    platos.emplace_back(i, superficies);
  }
}

void Disco::cargarDesdeDirectorio(const std::string& nombreDisco) {
  std::ifstream config("discos-config.txt");
  if (!config.is_open()) {
    std::cerr << "No se pudo abrir discos-config.txt\n";
    return;
  }

  std::string linea;
  bool encontrado = false;
  while (std::getline(config, linea)) {
    std::stringstream ss(linea);
    std::string token;
    std::getline(ss, token, ',');
    if (token == nombreDisco) {
      nombre = token;
      std::getline(ss, token, ','); numPlatos = std::stoi(token);
      std::getline(ss, token, ','); numSuperficies = std::stoi(token);
      std::getline(ss, token, ','); numPistas = std::stoi(token);
      std::getline(ss, token, ','); numBloques = std::stoi(token);
      std::getline(ss, token, ','); numSectores = std::stoi(token);
      std::getline(ss, token, ','); tamSectorBytes = std::stoi(token);
      encontrado = true;
      break;
    }
  }

  if (!encontrado) {
    std::cerr << "No se encontro configuracion para el disco '" << nombreDisco << "'\n";
  } else {
    std::cout << "Disco '" << nombre << "' cargado con exito.\n";
  }

  crearEstructuraJerarquica();
}

void Disco::consultarRelacion(const std::string& nombreRelacion) {
  // Verificar si la relación existe en esquema.txt
  std::ifstream esquema("esquema.txt");
  std::string lineaEsquema;
  bool encontrada = false;

  while (std::getline(esquema, lineaEsquema)) {
    if (lineaEsquema.find(nombreRelacion + "#") == 0) {
      encontrada = true;
      break;
    }
  }
  esquema.close();

  if (!encontrada) {
    std::cout << "La relacion '" << nombreRelacion << "' no existe en el esquema.\n";
    return;
  }

  // Buscar los IDs de sectores en metadata.txt
  std::ifstream metadata("metadata.txt");
  std::string lineaMetadata;
  std::unordered_set<int> idsRelacion;

  while (std::getline(metadata, lineaMetadata)) {
    size_t posDosPuntos = lineaMetadata.find(':');
    if (posDosPuntos == std::string::npos) continue;

    std::string nombreRel = lineaMetadata.substr(0, posDosPuntos);
    if (nombreRel != nombreRelacion) continue;

    std::string idsStr = lineaMetadata.substr(posDosPuntos + 1);
    std::stringstream ss(idsStr);
    std::string id;
    while (std::getline(ss, id, ',')) {
      if (!id.empty())
        idsRelacion.insert(std::stoi(id));
    }
    break;
  }
  metadata.close();

  if (idsRelacion.empty()) {
    std::cout << "\nNo se encontraron sectores asociados a la relación '" << nombreRelacion << "'.\n";
    return;
  }

  std::cout << "\n--- Registros de la relación '" << nombreRelacion << "' ---\n";

  for (const auto& plato : platos) {
    for (const auto& superficie : plato.getSuperficies()) {
      for (const auto& pista : superficie.getPistas()) {
        for (const auto& bloque : pista.getBloques()) {
          for (const auto& sector : bloque.getSectores()) {
            if (idsRelacion.count(sector.getId())) {
              std::ifstream archivo(sector.getRutaArchivo());
              std::string linea;
              while (std::getline(archivo, linea)) {
                std::replace(linea.begin(), linea.end(), '#', '|');
                std::cout << linea << '\n';
              }
              archivo.close();
            }
          }
        }
      }
    }
  }
}

void Disco::consultarRelacionCondicional(
    const std::string& nombreRelacion,
    const std::string& atributoCondicion,
    const std::string& operador,
    const std::string& valorComparar,
    const std::string& archivoSalida
) {
  // Leer esquema.txt y extraer cabecera y tipos
  std::ifstream esquema("esquema.txt");
  std::string lineaEsquema;
  bool existe = false;
  std::vector<std::string> atributos, tipos;
  while (std::getline(esquema, lineaEsquema)) {
    if (lineaEsquema.find(nombreRelacion + "#") == 0) {
      existe = true;
      std::stringstream ss(lineaEsquema);
      std::string token;
      std::getline(ss, token, '#');  // nombreRelacion
      while (std::getline(ss, token, '#')) {
        atributos.push_back(token);      // nombre atributo
        std::getline(ss, token, '#');
        tipos.push_back(token);          // tipo
      }
      break;
    }
  }
  esquema.close();

  if (!existe) {
    std::cout << "La relación '" << nombreRelacion << "' no existe.\n";
    return;
  }

  // Buscar posición del atributo
  auto it = std::find(atributos.begin(), atributos.end(), atributoCondicion);
  if (it == atributos.end()) {
    std::cout << "El atributo '" << atributoCondicion << "' no existe en la relación.\n";
    return;
  }
  int pos = std::distance(atributos.begin(), it);
  std::string tipo = tipos[pos];

  // Leer metadata.txt
  std::ifstream metadata("metadata.txt");
  std::string lineaMetadata;
  std::unordered_set<int> idsRelacion;

  while (std::getline(metadata, lineaMetadata)) {
    size_t posDosPuntos = lineaMetadata.find(':');
    if (posDosPuntos == std::string::npos) continue;

    std::string nombreRel = lineaMetadata.substr(0, posDosPuntos);
    if (nombreRel != nombreRelacion) continue;

    std::string idsStr = lineaMetadata.substr(posDosPuntos + 1);
    std::stringstream ss(idsStr);
    std::string id;
    while (std::getline(ss, id, ',')) {
      if (!id.empty())
        idsRelacion.insert(std::stoi(id));
    }
    break;
  }
  metadata.close();

  if (idsRelacion.empty()) {
    std::cout << "No se encontraron sectores asociados a la relación.\n";
    return;
  }
  // Abrir archivo de salida
  std::ofstream out(archivoSalida);
  if (!out.is_open()) {
    std::cout << "No se pudo abrir el archivo de salida.\n";
    return;
  }

  // Filtrar registros por condición
  for (const auto& plato : platos) {
    for (const auto& superficie : plato.getSuperficies()) {
      for (const auto& pista : superficie.getPistas()) {
        for (const auto& bloque : pista.getBloques()) {
          for (const auto& sector : bloque.getSectores()) {
            if (idsRelacion.count(sector.getId())) {
              std::ifstream archivo(sector.getRutaArchivo());
              std::string linea;
              while (std::getline(archivo, linea)) {
                std::vector<std::string> campos = parseCSVLinea(linea);
                if (campos.size() <= pos) continue;

                std::string valor = campos[pos];
                bool cumple = false;

                if (tipo == "int") {
                  int a = std::stoi(valor), b = std::stoi(valorComparar);
                  if      (operador == "==") cumple = (a == b);
                  else if (operador == "!=") cumple = (a != b);
                  else if (operador == "<")  cumple = (a < b);
                  else if (operador == "<=") cumple = (a <= b);
                  else if (operador == ">")  cumple = (a > b);
                  else if (operador == ">=") cumple = (a >= b);
                }
                else if (tipo == "float") {
                  float a = std::stof(valor), b = std::stof(valorComparar);
                  if      (operador == "==") cumple = (a == b);
                  else if (operador == "!=") cumple = (a != b);
                  else if (operador == "<")  cumple = (a < b);
                  else if (operador == "<=") cumple = (a <= b);
                  else if (operador == ">")  cumple = (a > b);
                  else if (operador == ">=") cumple = (a >= b);
                }
                else if (tipo == "string") {
                  if      (operador == "==") cumple = (valor == valorComparar);
                  else if (operador == "!=") cumple = (valor != valorComparar);
                }

                if (cumple) {
                  std::replace(linea.begin(), linea.end(), '#', ',');  // formato CSV
                  out << linea << "\n";
                }
              }
              archivo.close();
            }
          }
        }
      }
    }
  }
  out.close();
  std::cout << "\nConsulta condicional completada. Resultados guardados en '" << archivoSalida << "'.\n";

  // Agregar nueva relación a esquema.txt con los mismos atributos y tipos
  std::ofstream esquemaOut("esquema.txt", std::ios::app);
  if (esquemaOut.is_open()) {
    esquemaOut << archivoSalida;
    for (size_t i = 0; i < atributos.size(); ++i) {
      esquemaOut << "#" << atributos[i] << "#" << tipos[i];
    }
    esquemaOut << "\n";
    esquemaOut.close();
    std::cout << "Nueva relacion '" << archivoSalida << "' agregada al esquema.\n";
  } else {
    std::cerr << "Error al abrir esquema.txt para agregar la nueva relacion.\n";
    return;
  }
  insertarRegistrosSinCabecera(archivoSalida, nombreRelacion);
}

