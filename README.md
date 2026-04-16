Información del proyecto

Nombre del proyecto: Messenger Offline con ESP-NOW

Autores:

- [2064431] [Adam Joshua Castillo Rocha]
- [2068787] [Diego Antonio Perez Garza]
- [2071771] [Leonel Espinosa Robledo]

Docente: Ing. Héctor Hugo Flores Moreno

Materia: Laboratorio de Controladores y Microcontroladores Programables

Fecha: 16/04/2026

Messenger Offline con ESP-NOW

Descripción general

El proyecto Messenger Offline con ESP-NOW consiste en la implementación de un sistema de mensajería inalámbrica punto a punto entre dos placas ESP32, sin necesidad de internet ni de un router. La comunicación se realiza mediante el protocolo ESP-NOW, que permite el intercambio directo de paquetes entre dispositivos usando sus direcciones MAC. Cada nodo cuenta con un teclado matricial 4x4 para redactar mensajes y una pantalla OLED de 128x64 para visualizarlos, simulando el funcionamiento de un messenger básico completamente offline con un alcance de hasta 200-400 metros.

Qué problema resuelve

Las soluciones de mensajería convencionales dependen de una conexión a internet o de infraestructura de red Wi-Fi. Este proyecto demuestra cómo dos microcontroladores pueden comunicarse de forma directa, confiable y en tiempo real sin ninguna de esas condiciones, formando una red privada propia.

Propósito

El propósito principal es demostrar el funcionamiento de una red de comunicación local entre dos nodos ESP32 sin acceso a internet. También busca reforzar el aprendizaje de programación embebida, manejo de periféricos y protocolos de comunicación inalámbrica del ecosistema Espressif, integrando múltiples componentes en una arquitectura modular y funcional.

Funcionamiento Interno

El sistema está dividido en cuatro bloques:

- Entrada: el teclado matricial 4x4 permite escribir caracteres mediante lógica de ciclo T9. Pulsaciones repetidas sobre una misma tecla rotan entre los caracteres disponibles en ese grupo. Las teclas A–D insertan macros de preguntas o respuestas predefinidas.
- Procesamiento: el ESP32 gestiona el mensaje en edición, detecta tiempos de inactividad para confirmar caracteres, y controla el flujo de envío y recepción. Se usa una bandera volátil para desacoplar el callback de recepción de la actualización del display.
- Comunicación: se realiza mediante ESP-NOW, un protocolo de capa de enlace de Espressif que no requiere punto de acceso. La dirección MAC del dispositivo destino está configurada de forma estática en cada archivo de código fuente.
- Salida: la pantalla OLED muestra en su mitad superior el último mensaje recibido y en su mitad inferior el mensaje que se está redactando.

Contexto

El proyecto se desarrolla con hardware físico usando placas ESP32 genéricas y componentes electrónicos de bajo costo disponibles comercialmente. Su diseño es simétrico: ambos dispositivos ejecutan el mismo código con la única diferencia de la dirección MAC destino configurada en cada uno, lo que facilita su replicación y escalabilidad a más nodos.

Requisitos de hardware

- Placa: ESP32 (x2, uno por nodo)
- Componentes por nodo:
  - Pantalla OLED 128x64 (controlador SSD1306, comunicación I2C)
  - Teclado matricial de membrana 4x4
  - Cables Dupont Macho-Macho
  - Protoboard

Requisitos de software y librerías

- esp_now.h: Protocolo de comunicación inalámbrica ESP-NOW
- WiFi.h: Inicialización del modo estación del ESP32
- Wire.h: Comunicación I2C con la pantalla OLED
- Adafruit_GFX.h: Motor gráfico base para la pantalla
- Adafruit_SSD1306.h: Control específico del display SSD1306
- Keypad.h: Lectura del teclado matricial 4x4

Cómo usarlo

Obtener la dirección MAC

Antes de cargar el código final, sube un sketch auxiliar a cada ESP32 que imprima su dirección MAC por el Monitor Serie usando WiFi.macAddress(). Anota el valor y colócalo como dirección destino en el código del dispositivo contrario.

Ejecución básica

1. Instala las librerías necesarias desde el gestor de librerías del Arduino IDE.
2. Abre el archivo dispositivoA.ino y reemplaza la dirección MAC con la del Dispositivo B.
3. Abre el archivo dispositivoB.ino y reemplaza la dirección MAC con la del Dispositivo A.
4. Conecta el Dispositivo A al puerto USB y selecciona la placa ESP32 correspondiente en el IDE.
5. Sube el código. Si el IDE queda esperando conexión, mantén presionado el botón BOOT del ESP32 hasta que aparezcan los porcentajes de carga en la consola y suéltalo.
6. Repite los pasos 4 y 5 con el Dispositivo B.
7. Abre el Monitor Serie a 115200 baudios para verificar el estado del envío.

Ejemplo de funcionamiento

- Presionar 2 una vez escribe "a"; dos veces "b"; tres veces "c".
- Presionar 0 dos veces inserta un espacio.
- Presionar A escribe la macro "¿Como vas?"; presionar A de nuevo la reemplaza por "Todo bien".
- Presionar # dos veces envía el mensaje al otro dispositivo.
- El mensaje recibido aparece en la parte superior de la pantalla OLED del otro nodo.

Estructura del proyecto

    Messenger-ESP-NOW/
    ├── README.md
    ├── dispositivoA.ino
    ├── dispositivoB.ino
    ├── /diagramas
    │   ├── Diagrama de Bloques.png
    │   ├── Diagrama Esquematico.png
    │   └── Diagrama Pictorico.png
    ├── /docs
    │   ├── Actividad 6.pdf
    │   ├── Actividad 7.pdf
    │   └── Actividad 8.pdf
    └── /librerias
        ├── Adafruit_GFX.h
        ├── Adafruit_SSD1306.h
        ├── esp_now.h
        ├── Keypad.h
        ├── WiFi.h
        └── Wire.h
        
Diagramas del proyecto

Diagrama Pictórico: muestra la disposición física de todos los componentes en la protoboard.
![Diagrama pictórico del proyecto](diagramas/Diagrama_Pictorico.png)

Diagrama de Bloques: explica la interacción entre los módulos de entrada, procesamiento, comunicación y salida.
![Diagrama de bloques del proyecto](diagramas/Diagram_de_Bloques.png)

Diagrama Esquemático: representa las conexiones eléctricas del sistema.
![Diagrama esquemático del proyecto](diagramas/Diagrama_Esquematico.png)

Resumen Técnico

El proyecto Messenger Offline implementa una arquitectura Entrada–Procesamiento–Comunicación–Salida sobre dos nodos ESP32 simétricos. Utiliza ESP-NOW como capa de transporte sin necesidad de router, con direccionamiento por MAC estática. La lógica de entrada emula el sistema T9 de teléfonos de teclas, permitiendo escribir texto a partir de un teclado numérico. El código emplea una bandera volátil para desacoplar el callback de recepción de la actualización del display, evitando conflictos de acceso al bus I2C desde contextos de interrupción. El par de archivos dispositivoA.ino y dispositivoB.ino contienen el mismo código fuente, diferenciándose únicamente en la dirección MAC destino configurada en cada uno.

FAQ (Preguntas frecuentes)

- ¿Por qué no se muestra el mensaje recibido?
  Verifica que la dirección MAC configurada en cada archivo corresponda exactamente al dispositivo contrario. Una sola diferencia en cualquier byte impide la comunicación.
- ¿El teclado no responde correctamente?
  Revisa que los pines de filas estén configurados como INPUT_PULLUP. Esta configuración se refuerza en el setup() después de inicializar Wi-Fi, ya que la radio del ESP32 puede alterar el estado de los GPIO durante su inicialización.
- ¿La pantalla OLED no enciende?
  Confirma que la dirección I2C sea 0x3C, que es la más común en módulos SSD1306 de 0.96 pulgadas. Algunos módulos usan 0x3D.
- ¿El IDE no encuentra el puerto del ESP32?
  Asegúrate de tener instalado el driver CP210x o CH340 según el chip USB que use tu placa. También verifica que el cable sea de datos y no solo de carga.