# Bloc de Notas con ESP8266

Este proyecto permite utilizar un ESP8266 como servidor web para crear, visualizar y eliminar notas. Las notas se almacenan en el sistema de archivos SPIFFS del ESP8266 y son accesibles a través de una interfaz web interactiva.

## Características

- **Guardar notas:** Escribe y almacena notas en el ESP8266.
- **Visualizar notas:** Consulta las notas guardadas en una tabla interactiva.
- **Eliminar notas:** Elimina notas específicas mediante un botón asociado a cada una.
- **Interfaz web intuitiva:** Diseño sencillo y funcional accesible desde cualquier navegador web.

## Requisitos

### Hardware
- ESP8266 (NodeMCU, Wemos D1 Mini, etc.).
- Conexión Wi-Fi activa.

### Software
- Arduino IDE.
- Librerías necesarias:
  - `ESP8266WiFi.h` (incluida en el Core de ESP8266).
  - `ESP8266WebServer.h`.
  - `FS.h` para el sistema de archivos SPIFFS.

