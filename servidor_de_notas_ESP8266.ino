#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h> // Para usar SPIFFS

#define WIFI_SSID "tu_SSID"
#define WIFI_PASSWORD "tu_contraseña"

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Conectando a WiFi...");

  // Conectar a WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConexión establecida.");
  Serial.print("Dirección IP asignada: ");
  Serial.println(WiFi.localIP());

  // Inicializar SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Error al montar SPIFFS");
    return;
  }

  // Ruta principal para el formulario y visualización
  server.on("/", HTTP_GET, []() {
    String html = R"rawliteral(
      <!DOCTYPE html>
      <html lang="en">
      <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Bloc de Notas</title>
        <style>
          body { font-family: Arial, sans-serif; background-color: #f0f0f5; display: flex; justify-content: center; align-items: center; height: 100vh; flex-direction: column; }
          .container { text-align: center; background: #ffffff; border-radius: 10px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); padding: 20px; width: 300px; margin-bottom: 20px; }
          h1 { color: #333; margin-bottom: 20px; }
          textarea { width: 100%; height: 150px; margin-bottom: 20px; }
          button { padding: 10px 20px; font-size: 16px; color: white; background-color: #007bff; border: none; border-radius: 5px; cursor: pointer; }
          button:hover { background-color: #0056b3; }
          .notes { margin-top: 20px; }
          table { width: 100%; border-collapse: collapse; margin-top: 20px; }
          th, td { padding: 10px; text-align: left; border-bottom: 1px solid #ddd; }
          th { background-color: #f2f2f2; }
          .delete-btn { padding: 5px 10px; color: white; background-color: #dc3545; border: none; border-radius: 5px; cursor: pointer; }
          .delete-btn:hover { background-color: #c82333; }
        </style>
      </head>
      <body>
        <div class="container">
          <h1>Bloc de Notas</h1>
          <form action="/guardar" method="POST">
            <textarea name="nota" placeholder="Escribe tu nota aquí..."></textarea><br>
            <button type="submit">Guardar Nota</button>
          </form>
        </div>
        <div class="container notes">
          <h2>Notas Guardadas</h2>
          <div id="notas">
            <!-- Las notas se cargarán aquí -->
          </div>
        </div>
        <script>
          function eliminarNota(id) {
            fetch('/eliminar?id=' + id, { method: 'GET' })
              .then(response => response.text())
              .then(data => {
                alert(data);
                location.reload(); // Recargar la página para actualizar las notas
              });
          }

          fetch('/notas').then(response => response.text()).then(data => {
            document.getElementById('notas').innerHTML = data;
          });
        </script>
      </body>
      </html>
    )rawliteral";
    server.send(200, "text/html", html);
  });

  // Ruta para guardar la nota
  server.on("/guardar", HTTP_POST, []() {
    String nota = server.arg("nota");

    if (nota.length() > 0) {
      File file = SPIFFS.open("/notas.csv", "a");
      if (file) {
        file.print(nota);
        file.println(); // Añade una nueva línea después de cada nota
        file.close();
        server.sendHeader("Location", "/");
        server.send(303);
      } else {
        server.send(500, "text/html", "<h1>Error al guardar la nota</h1>");
      }
    } else {
      server.send(400, "text/html", "<h1>No se ha ingresado texto</h1>");
    }
  });

  // Ruta para visualizar las notas
  server.on("/notas", HTTP_GET, []() {
    File file = SPIFFS.open("/notas.csv", "r");
    if (!file) {
      server.send(500, "text/plain", "Error al leer las notas");
      return;
    }

    String html = "<table><tr><th>Nota</th><th>Acción</th></tr>";
    int id = 0; // Identificador único para cada nota
    while (file.available()) {
      String linea = file.readStringUntil('\n');
      if (linea.length() > 0) {
        html += "<tr><td>" + linea + "</td><td><button class='delete-btn' onclick='eliminarNota(" + String(id) + ")'>Eliminar</button></td></tr>";
        id++;
      }
    }
    html += "</table>";
    file.close();
    server.send(200, "text/html", html);
  });

  // Ruta para eliminar una nota
  server.on("/eliminar", HTTP_GET, []() {
    int id = server.arg("id").toInt();
    File file = SPIFFS.open("/notas.csv", "r");
    if (!file) {
      server.send(500, "text/plain", "Error al abrir el archivo de notas");
      return;
    }

    String nuevaData = "";
    int lineaActual = 0;
    while (file.available()) {
      String linea = file.readStringUntil('\n');
      if (lineaActual != id) {
        nuevaData += linea + "\n";
      }
      lineaActual++;
    }
    file.close();

    // Reescribir el archivo con las notas restantes
    file = SPIFFS.open("/notas.csv", "w");
    if (file) {
      file.print(nuevaData);
      file.close();
      server.send(200, "text/plain", "Nota eliminada correctamente");
    } else {
      server.send(500, "text/plain", "Error al escribir el archivo de notas");
    }
  });

  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  server.handleClient();
}
