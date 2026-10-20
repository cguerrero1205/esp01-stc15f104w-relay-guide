# Programar módulo ESP-01 + Relay 1CH 5V con STC15F104W

Guía completa para entender y hacer funcionar correctamente el módulo relay basado en:

* ESP-01 / ESP-01S
* Relay 1CH 5V
* Microcontrolador STC15F104W

## Contenido

1. [Introducción](#introducción)
2. [Estructura del repositorio](#estructura-del-repositorio)
3. [Cómo identificar si tu módulo usa STC15F104W](#cómo-identificar-si-tu-módulo-usa-stc15f104w)
4. [Comunicación serial (TX → TX)](#comunicación-serial)
5. [Programación del STC15F104W](#programación-del-stc15f104w)
6. [Procedimiento de programación](#procedimiento-de-programación)
7. [Modificación de hardware (R3 / R4)](#modificación-de-hardware-r3--r4)
8. [Comandos para controlar el relay](#comandos-para-controlar-el-relay)
9. [Cableado](#cableado)
10. [Problemas comunes](#problemas-comunes)
11. [Créditos y referencias](#créditos-y-referencias)

---

# Introducción

Compré un módulo **ESP-01 + Relay 1CH 5V**, esperando controlar el relay directamente desde los pines GPIO del ESP8266.

Sin embargo, después de múltiples pruebas, el relay no respondía correctamente.

Investigando más a fondo descubrí que muchos de estos módulos incorporan un microcontrolador adicional llamado **STC15F104W**. Este chip es el verdadero encargado de controlar el relay.

El ESP-01 no controla el relay directamente; en su lugar, envía comandos seriales (UART) al STC15F104W, y este activa o desactiva el relay.

Ese detalle no suele estar claramente documentado, por eso dejo este instructivo.

---

# Estructura del repositorio

```txt
.
├── README.md
├── firmware/
│   └── firmware.hex              # Firmware para el STC15F104W
├── examples/
│   └── relay_toggle/
│       └── relay_toggle.ino      # Ejemplo para el ESP-01 (Arduino IDE)
└── images/
    ├── hardware/                 # Fotos del módulo, chip, R3/R4 y conexión UART
    └── programacion/             # USB-TTL y capturas de STC-ISP
```

---

# Cómo identificar si tu módulo usa STC15F104W

Revisa la placa.

Si encuentras un chip de 8 pines con la referencia `STC15F104W`, entonces tu módulo probablemente funciona mediante comunicación serial.

<p>
  <img src="images/hardware/chip-stc15f104w-1.jpg" width="283" alt="Módulo ESP-01 Relay con chip STC15F104W">
  <img src="images/hardware/chip-stc15f104w-2.jpg" width="283" alt="Vista del módulo relay">
  <img src="images/hardware/chip-stc15f104w-3.jpg" width="283" alt="Detalle del chip STC15F104W">
</p>

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

# Comunicación serial

## Importante: conexión UART no convencional

Un detalle importante de este módulo es que la conexión serial **no sigue la convención UART tradicional**.

Normalmente, al comunicar dos dispositivos por UART, se conectan los pines cruzados:

```txt
ESP-01 TX → STC RX
ESP-01 RX → STC TX
```

## En este módulo funciona diferente

En este módulo específico, la comunicación funciona con conexión directa:

```txt
ESP-01 TX → STC TX
ESP-01 RX → STC RX
```

## ¿Por qué?

Esto sucede debido al diseño interno de la PCB y al enrutamiento de señales del módulo.

Internamente, las pistas ya están diseñadas de una forma que hace que la conexión correcta sea directa, no cruzada.

Por eso, si intentas conectarlo usando la convención UART estándar, es muy probable que:

* La programación falle
* No haya comunicación serial
* El relay no responda

## Recomendación

No asumir que este módulo usa el esquema UART tradicional.

Antes de diagnosticar problemas de firmware o hardware, verifica que las conexiones seriales estén hechas así (`TX → TX`, `RX → RX`):

<img src="images/hardware/conexion-uart-tx-tx.jpg" width="320" alt="Conexión UART directa TX a TX y RX a RX">

Este fue uno de los detalles más importantes para lograr que el módulo funcionara correctamente.

La velocidad de comunicación es **9600 baud**.

---

# Programación del STC15F104W

Para programar el microcontrolador **STC15F104W**, utilicé el software oficial de STC.

## Software necesario

Descargar el programa desde la página oficial:

* https://www.stcmicro.com/rjxz.html

En mi caso utilicé la versión **STC-ISP 6.96S**.

## Firmware necesario

Descargar el archivo [`firmware/firmware.hex`](firmware/firmware.hex) de este repositorio.

Este archivo contiene el firmware necesario para que el **STC15F104W** controle correctamente el relay.

## Hardware necesario

Para programar el STC, se necesita un convertidor **USB a TTL**.

En mi caso utilicé el siguiente:

<img src="images/programacion/usb-ttl.jpg" width="608" alt="Convertidor USB a TTL utilizado">

Ejemplos comunes:

* CH340 USB-TTL
* FTDI USB-TTL
* CP2102 USB-TTL

## Conexión del USB-TTL al STC

```txt
USB-TTL TX  → STC TX
USB-TTL RX  → STC RX
USB-TTL GND → GND
USB-TTL 5V  → VCC
```

> [!WARNING]
> A diferencia de una conexión UART tradicional, en este módulo la conexión es `TX → TX` y `RX → RX`. **No cruzar TX/RX.**

## Configuración del software STC-ISP

Abrir el programa **STC-ISP** y configurar:

<img src="images/programacion/stc-isp-configuracion.png" width="700" alt="Configuración de STC-ISP">

### 1. Seleccionar puerto COM

Elegir el puerto COM correspondiente al convertidor USB-TTL (por ejemplo `COM3`, `COM4`, `COM5`).

### 2. Cargar el firmware

Cargar el archivo `firmware/firmware.hex`.

### 3. Verificar frecuencia IRC

Verificar que la frecuencia IRC detectada sea correcta para el STC.

Este parámetro es importante para que la programación sea exitosa.

---

# Procedimiento de programación

El STC15F104W entra en modo de programación al energizarse.

Por esta razón, el orden de conexión es muy importante.

### Paso 1

Conectar los cables **TX**, **RX** y **GND** del USB-TTL al STC, pero **NO conectar todavía el cable de 5V**.

### Paso 2

Abrir **STC-ISP** y configurar:

* Puerto COM
* Frecuencia IRC
* Firmware HEX

### Paso 3

Dar clic en **`Download/Program`**.

El programa quedará esperando al microcontrolador.

### Paso 4

Ahora sí conectar el cable de **5V**.

En el momento en que el STC recibe alimentación, entra en modo bootloader y el software comienza a cargar el firmware automáticamente.

### Paso 5

Esperar a que finalice la programación.

Si todo sale correctamente, el software mostrará un mensaje de éxito:

<img src="images/programacion/stc-isp-exito.png" width="700" alt="STC-ISP mostrando programación exitosa">

---

# Modificación de hardware (R3 / R4)

En algunos módulos, reprogramar el **STC15F104W** puede no ser suficiente para lograr que el relay se active correctamente.

En mi caso fue necesario realizar una modificación adicional en la PCB: **intercambiar las resistencias R3 y R4**.

* La resistencia en **R3** debe moverse a **R4**
* La resistencia en **R4** debe moverse a **R3**

<img src="images/hardware/resistencias-r3-r4.jpg" width="320" alt="Resistencias R3 y R4 intercambiadas">

*Nota: en esta imagen las resistencias ya están intercambiadas.*

## Antes de modificar la PCB

Recomiendo primero probar el módulo **sin modificar las resistencias**:

1. Reprogramar el **STC15F104W**
2. Probar la activación del relay

Si el relay activa correctamente, entonces **no necesitas modificar R3 y R4**. Solo si el relay no activa, recomiendo hacer este cambio.

## ¿Por qué es necesario?

Las resistencias **R3** y **R4** forman un divisor de voltaje que determina la señal que llega al transistor encargado de activar el relay.

```txt
STC15F104W → divisor de voltaje (R3/R4) → transistor → relay
```

Con la configuración original de algunos módulos, el divisor de voltaje genera una señal insuficiente para polarizar correctamente el transistor.

Como consecuencia:

* El transistor no entra completamente en conducción
* La bobina del relay no recibe suficiente corriente
* El relay no activa

En otras palabras, aunque el STC esté enviando la señal correctamente, el relay nunca llega a energizarse.

## Solución

Al intercambiar las posiciones de **R3** y **R4**, cambia la relación del divisor de voltaje y se obtiene una señal adecuada para activar correctamente el transistor.

Después de hacer este cambio, el relay comenzó a funcionar normalmente.

## Síntomas de este problema

Si tu módulo presenta alguno de estos síntomas, probablemente necesites hacer la modificación:

* El STC parece funcionar correctamente
* La programación fue exitosa
* El relay nunca hace "click"
* El LED puede encender, pero el relay no activa

## Mi experiencia

En mi caso, la solución completa requirió dos pasos:

* ✅ Reprogramar el **STC15F104W**
* ✅ Intercambiar **R3** y **R4**

Solo después de realizar ambos cambios, el módulo funcionó correctamente.

## Advertencia

> [!CAUTION]
> Esta modificación requiere experiencia trabajando con componentes SMD. Realiza el procedimiento con cuidado para evitar daños en la PCB.

Herramientas recomendadas:

* Cautín de punta fina
* Flux
* Pinzas de precisión
* Estación de aire caliente (opcional)

---

# Comandos para controlar el relay

El ESP-01 envía los comandos por UART a **9600 baud**:

```cpp
Serial.begin(9600);
```

| Acción    | Comando hexadecimal |
|-----------|---------------------|
| Relay ON  | `A0 01 01 A2`       |
| Relay OFF | `A0 01 00 A1`       |

## Relay ON

```cpp
byte relayOn[] = {0xA0, 0x01, 0x01, 0xA2};
Serial.write(relayOn, sizeof(relayOn));
```

## Relay OFF

```cpp
byte relayOff[] = {0xA0, 0x01, 0x00, 0xA1};
Serial.write(relayOff, sizeof(relayOff));
```

## Ejemplo completo (Arduino IDE)

Disponible en [`examples/relay_toggle/relay_toggle.ino`](examples/relay_toggle/relay_toggle.ino):

```cpp
void setup() {
  Serial.begin(9600);
}

void encenderRelay() {
  byte cmd[] = {0xA0, 0x01, 0x01, 0xA2};
  Serial.write(cmd, sizeof(cmd));
}

void apagarRelay() {
  byte cmd[] = {0xA0, 0x01, 0x00, 0xA1};
  Serial.write(cmd, sizeof(cmd));
}

void loop() {
  encenderRelay();
  delay(3000);

  apagarRelay();
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

> [!IMPORTANT]
> * No alimentar directamente el ESP-01 con 5V
> * El ESP8266 trabaja a 3.3V
> * Muchos módulos relay ya incluyen regulación de voltaje
>
> Verifica tu placa antes de energizar.

---

# Problemas comunes

## No detecta el STC

Verificar:

* Puerto COM correcto
* Drivers del USB-TTL instalados
* TX y RX bien conectados (`TX → TX`, `RX → RX`)
* GND conectado
* Orden correcto de programación

## Falla al programar

Verificar especialmente este paso:

> [!WARNING]
> El cable de **5V debe conectarse únicamente después de presionar `Download/Program`**.
>
> Si el STC ya está energizado antes de dar clic, normalmente no entrará en modo de programación.

## El firmware carga pero el relay no funciona

Revisar:

* Sección de [modificación de hardware R3 / R4](#modificación-de-hardware-r3--r4)
* [Conexiones UART](#comunicación-serial)
* Firmware correcto

## El relay no responde

Validar:

* Baud rate = 9600
* Serial correctamente inicializado
* ESP correctamente energizado
* STC funcional
* Relay alimentado con 5V estable

## El relay parpadea al iniciar

Esto suele ocurrir porque durante el boot:

* GPIO0 cambia de estado
* UART transmite basura
* El STC interpreta datos inválidos

## El ESP funciona pero el relay no

En muchos casos ocurre porque el usuario intenta controlar:

```cpp
digitalWrite(GPIO0, HIGH);
```

Pero este módulo no funciona así. Debe controlarse mediante [UART](#comandos-para-controlar-el-relay).

---

# Créditos y referencias

Durante el proceso de investigación y pruebas, me apoyé bastante en información compartida por la comunidad, especialmente en este hilo del foro ESP8266, donde varios usuarios documentaron el funcionamiento del módulo LC Technology con el microcontrolador STC15F104W:

* [ESP8266 Forum – Trouble with ESP-01 + 1Ch 5V relay from LC Technology](https://esp8266.com/viewtopic.php?f=160&t=13164&start=108)

Ese hilo fue clave para entender varios puntos importantes:

* El relay no se controla directamente desde los GPIO del ESP-01.
* El relay realmente es controlado por el microcontrolador **STC15F104W**.
* La comunicación entre ESP-01 y STC se realiza mediante **UART serial**.
* La velocidad de comunicación utilizada normalmente es **9600 baud**.

A partir de esa información, realicé pruebas adicionales con mi propio hardware hasta lograr una configuración funcional, que es la que documento en este repositorio.

El objetivo de esta guía es ahorrar tiempo a otros desarrolladores que tengan este mismo módulo y se encuentren con la falta de documentación clara.
