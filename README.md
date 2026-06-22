# Programar módulo ESP-01 + Relay 1CH 5V con STC15F104W

Guía completa para entender y hacer funcionar correctamente el módulo relay basado en:

* ESP-01 / ESP-01S
* Relay 1CH 5V
* Microcontrolador STC15F104W

---

# Introducción

Compré un módulo **ESP-01 + Relay 1CH 5V**, esperando controlar el relay directamente desde los pines GPIO del ESP8266.

Sin embargo, después de múltiples pruebas, el relay no respondía correctamente.

Investigando más a fondo descubrí que muchos de estos módulos incorporan un microcontrolador adicional llamado:

```txt
STC15F104W
```

Este chip es el verdadero encargado de controlar el relay.

El ESP-01 no controla el relay directamente; en su lugar, envía comandos seriales (UART) al STC15F104W, y este activa o desactiva el relay.

Ese detalle no suele estar claramente documentado, por eso dejo este instructivo.

---

# Cómo identificar si tu módulo usa STC15F104W

Revisa la placa.

Si encuentras un chip de 8 pines con la referencia:

```txt
STC15F104W
```

Entonces tu módulo probablemente funciona mediante comunicación serial.

<img width="283" height="178" alt="IMG_20260622_092352" src="https://github.com/user-attachments/assets/ccf40f80-ea88-49f6-bbb6-8907e833285f" />
<img width="283" height="178" alt="IMG_20260622_092345" src="https://github.com/user-attachments/assets/30941869-7e2d-4aee-8689-e370acf8c33a" />
<img width="283" height="178" alt="IMG_20260622_092405" src="https://github.com/user-attachments/assets/954acc90-f270-429e-bf7d-2a111c608a29" />


Arquitectura interna:

```txt
ESP-01 ---> UART ---> STC15F104W ---> Relay
```

Flujo:

1. El ESP recibe comandos por WiFi
2. El ESP envía datos por UART
3. El STC interpreta esos datos
4. El STC activa/desactiva el relay

---

# Importante: posible modificación de hardware (R3 / R4)

En algunos módulos, reprogramar el **STC15F104W** puede no ser suficiente para lograr que el relay se active correctamente.

En mi caso fue necesario realizar una modificación adicional en la PCB:

* Intercambiar las resistencias **R3** y **R4**

Es decir:

* La resistencia en **R3** debe moverse a **R4**
* La resistencia en **R4** debe moverse a **R3**

<img width="237" height="178" alt="IMG_20260622_093434" src="https://github.com/user-attachments/assets/6c692517-d3f9-4aeb-a0d4-341b57a7df2c" />

Nota: En esta imagen las resistencias ya están intercambiadas.

---

## Antes de modificar la PCB

Recomiendo primero probar el módulo **sin modificar las resistencias**.

Pasos recomendados:

1. Reprogramar el **STC15F104W**
2. Probar la activación del relay

Si el relay activa correctamente, entonces **no necesitas modificar R3 y R4**.

Solo si el relay no activa, recomiendo hacer este cambio.

---

## ¿Por qué es necesario?

Las resistencias **R3** y **R4** forman un divisor de voltaje que determina la señal que llega al transistor encargado de activar el relay.

Arquitectura simplificada:

```txt
STC15F104W → divisor de voltaje (R3/R4) → transistor → relay
```

Con la configuración original de algunos módulos, el divisor de voltaje genera una señal insuficiente para polarizar correctamente el transistor.

Como consecuencia:

* El transistor no entra completamente en conducción
* La bobina del relay no recibe suficiente corriente
* El relay no activa

En otras palabras, aunque el STC esté enviando la señal correctamente, el relay nunca llega a energizarse.

---

## Solución

Al intercambiar las posiciones de **R3** y **R4**, cambia la relación del divisor de voltaje y se obtiene una señal adecuada para activar correctamente el transistor.

Después de hacer este cambio, el relay comenzó a funcionar normalmente.

---

## Síntomas de este problema

Si tu módulo presenta alguno de estos síntomas, probablemente necesites hacer la modificación:

* El STC parece funcionar correctamente
* La programación fue exitosa
* El relay nunca hace “click”
* El LED puede encender, pero el relay no activa

---

## Mi experiencia

En mi caso, la solución completa requirió dos pasos:

* ✅ Reprogramar el **STC15F104W**
* ✅ Intercambiar **R3** y **R4**

Solo después de realizar ambos cambios, el módulo funcionó correctamente.

---

## Advertencia

Esta modificación requiere experiencia trabajando con componentes SMD.

Herramientas recomendadas:

* Cautín de punta fina
* Flux
* Pinzas de precisión
* Estación de aire caliente (opcional)

Realiza el procedimiento con cuidado para evitar daños en la PCB.

---

# Comunicación serial

## Importante: conexión UART no convencional

Un detalle importante de este módulo es que la conexión serial **no sigue la convención UART tradicional**.

Normalmente, al comunicar dos dispositivos por UART, se conectan los pines cruzados:

```txt
TX → RX
RX → TX
```

Ejemplo tradicional:

```txt
ESP-01 TX → STC RX
ESP-01 RX → STC TX
```

---

## En este módulo funciona diferente

En este módulo específico, la comunicación funciona con conexión directa:

```txt
TX → TX
RX → RX
```

Es decir:

```txt
ESP-01 TX → STC TX
ESP-01 RX → STC RX
```

---

## ¿Por qué?

Esto sucede debido al diseño interno de la PCB y al enrutamiento de señales del módulo.

Internamente, las pistas ya están diseñadas de una forma que hace que la conexión correcta sea directa, no cruzada.

Por eso, si intentas conectarlo usando la convención UART estándar, es muy probable que:

* La programación falle
* No haya comunicación serial
* El relay no responda

---

## Recomendación

No asumir que este módulo usa el esquema UART tradicional.

Antes de diagnosticar problemas de firmware o hardware, verifica que las conexiones seriales estén hechas así:

<img width="237" height="178" alt="IMG_20260622_094402" src="https://github.com/user-attachments/assets/df19d748-dddc-4753-b85a-ddc9b43ef51a" />

```txt
TX → TX
RX → RX
```

Este fue uno de los detalles más importantes para lograr que el módulo funcionara correctamente.

# Programación del STC15F104W

Para programar el microcontrolador **STC15F104W**, utilicé el software oficial de STC.

## Software necesario

Descargar el programa desde la página oficial:

* https://www.stcmicro.com/rjxz.html

En mi caso utilicé la versión:

```txt id="i9wjlwm"
STC-ISP 6.96S
```

---

## Firmware necesario

También deben descargar el archivo:

```txt id="u7ndn2"
firmware.hex
```

Este archivo contiene el firmware necesario para que el **STC15F104W** controle correctamente el relay.

El archivo está disponible en este repositorio.

---

## Hardware necesario

Para programar el STC, se necesita un convertidor:

* USB a TTL

En mi caso utilicé el siguiente:

<img width="608" height="178" alt="IMG_20260622_101017" src="https://github.com/user-attachments/assets/9c5c3655-3b3e-4cb3-a8c8-ceb5fe309dbc" />

Ejemplos comunes:

* CH340 USB-TTL
* FTDI USB-TTL
* CP2102 USB-TTL

---

## Conexión del USB-TTL al STC

Conectar el convertidor USB-TTL al módulo.

Conexiones:

```txt id="a8q16h"
USB-TTL TX → STC TX
USB-TTL RX → STC RX
GND → GND
5V → VCC
```

⚠️ Importante:

A diferencia de una conexión UART tradicional, en este módulo la conexión funciona así:

```txt id="w63nku"
TX → TX
RX → RX
```

No cruzar TX/RX.

---

## Configuración del software STC-ISP

Abrir el programa **STC-ISP** y configurar:

<img width="939" height="658" alt="image" src="https://github.com/user-attachments/assets/08dcccae-65da-4da0-8f71-39a831679f3c" />

### 1. Seleccionar puerto COM

Elegir el puerto COM correspondiente al convertidor USB-TTL.

Ejemplo:

```txt id="vrrvh8"
COM3
COM4
COM5
```

---

### 2. Cargar el firmware

Cargar el archivo:

```txt id="4ywj8r"
firmware.hex
```

---

### 3. Verificar frecuencia IRC

Verificar que la frecuencia IRC detectada sea correcta para el STC.

Este parámetro es importante para que la programación sea exitosa.

---

# Procedimiento de programación

El STC15F104W entra en modo de programación al energizarse.

Por esta razón, el orden de conexión es muy importante.

## Paso 1

Conectar todos los cables del USB-TTL al STC:

* TX
* RX
* GND

Pero **NO conectar todavía el cable de 5V**.

---

## Paso 2

Abrir **STC-ISP** y configurar:

* Puerto COM
* Frecuencia IRC
* Firmware HEX

---

## Paso 3

Dar clic en:

```txt id="w63cjq"
Download/Program
```

El programa quedará esperando al microcontrolador.

---

## Paso 4

Ahora sí conectar el cable de:

```txt id="zcnd5e"
5V
```

En el momento en que el STC recibe alimentación, entra en modo bootloader y el software comienza a cargar el firmware automáticamente.

---

## Paso 5

Esperar a que finalice la programación.

Si todo sale correctamente, el software mostrará un mensaje de éxito.

<img width="935" height="653" alt="image" src="https://github.com/user-attachments/assets/04ed2991-5c42-4b9b-bd7d-057cb9c29444" />

---

# Troubleshooting

## No detecta el STC

Verificar:

* Puerto COM correcto
* Drivers del USB-TTL instalados
* TX y RX bien conectados
* GND conectado
* Orden correcto de programación

---

## Falla al programar

Verificar especialmente este paso:

⚠️ El cable de **5V debe conectarse únicamente después de presionar `Download/Program`**.

Si el STC ya está energizado antes de dar clic, normalmente no entrará en modo de programación.

---

## El firmware carga pero el relay no funciona

Revisar:

* Sección de modificación de hardware **R3 / R4**
* Conexiones UART
* Firmware correcto

---

En Arduino IDE:

```cpp
Serial.begin(9600);
```

---

# Comandos para controlar el relay

## Relay ON

Comando hexadecimal:

```txt
A0 01 01 A2
```

Código:

```cpp
byte relayOn[] = {0xA0, 0x01, 0x01, 0xA2};
Serial.write(relayOn, sizeof(relayOn));
```

---

## Relay OFF

Comando hexadecimal:

```txt
A0 01 00 A1
```

Código:

```cpp
byte relayOff[] = {0xA0, 0x01, 0x00, 0xA1};
Serial.write(relayOff, sizeof(relayOff));
```

---

# Ejemplo completo (Arduino IDE)

```cpp
void setup() {
  Serial.begin(9600);
}

void loop() {
  byte relayOn[] = {0xA0, 0x01, 0x01, 0xA2};
  byte relayOff[] = {0xA0, 0x01, 0x00, 0xA1};

  Serial.write(relayOn, sizeof(relayOn));
  delay(3000);

  Serial.write(relayOff, sizeof(relayOff));
  delay(3000);
}
```

---

# Cableado

Normalmente el módulo ya conecta internamente el ESP-01 con el STC.

Solo necesitas:

* Alimentación de 5V para el módulo relay
* ESP-01 correctamente insertado
* Firmware cargado en el ESP

Importante:

* No alimentar directamente el ESP-01 con 5V
* El ESP8266 trabaja a 3.3V
* Muchos módulos relay ya incluyen regulación de voltaje

Verifica tu placa antes de energizar.

---

# Problemas comunes

## El relay no responde

Validar:

* Baud rate = 9600
* Serial correctamente inicializado
* ESP correctamente energizado
* STC funcional
* Relay alimentado con 5V estable

---

## El relay parpadea al iniciar

Esto suele ocurrir porque durante el boot:

* GPIO0 cambia de estado
* UART transmite basura
* El STC interpreta datos inválidos

---

## El ESP funciona pero el relay no

En muchos casos ocurre porque el usuario intenta controlar:

```cpp
digitalWrite(GPIO0, HIGH);
```

Pero este módulo no funciona así.

Debe controlarse mediante UART.

---

# Mi configuración funcional

Aquí puedes documentar exactamente cómo lo hiciste funcionar.

Ejemplo:

* ESP-01S de 1MB
* Programado con Arduino IDE
* Alimentación 5V / 2A
* Control vía MQTT
* Relay funcionando correctamente con comandos UART

---

# Cómo lo descubrí

Mi proceso fue el siguiente:

1. Inicialmente intenté controlar el relay desde GPIO
2. No funcionó
3. Revisé el hardware y encontré el chip STC15F104W
4. Investigando descubrí que el relay no se controla directamente
5. Probé comandos UART
6. Finalmente logré activar y desactivar el relay

Este comportamiento no está bien documentado, por eso dejo esta guía para futuras referencias.

---

# Recomendaciones

Si vas a usar este módulo en proyectos reales:

* Domótica
* MQTT
* Home Assistant
* Automatización industrial

Recomiendo:

* Documentar bien el hardware
* Confirmar si tu módulo usa STC o GPIO directo
* Probar primero con comandos seriales simples

---

## Créditos y referencias

Durante el proceso de investigación y pruebas, me apoyé bastante en información compartida por la comunidad, especialmente en este hilo del foro ESP8266, donde varios usuarios documentaron el funcionamiento del módulo LC Technology con el microcontrolador STC15F104W:

* [ESP8266 Forum – Trouble with ESP-01 + 1Ch 5V relay from LC Technology](https://esp8266.com/viewtopic.php?f=160&t=13164&start=108)

Ese hilo fue clave para entender varios puntos importantes:

* El relay no se controla directamente desde los GPIO del ESP-01.
* El relay realmente es controlado por el microcontrolador **STC15F104W**.
* La comunicación entre ESP-01 y STC se realiza mediante **UART serial**.
* La velocidad de comunicación utilizada normalmente es **9600 baud**.

A partir de esa información, realicé pruebas adicionales con mi propio hardware hasta lograr una configuración funcional, que es la que documento en este repositorio.

El objetivo de esta guía es ahorrar tiempo a otros desarrolladores que tengan este mismo módulo y se encuentren con la falta de documentación clara.

---

# Licencia

MIT License
