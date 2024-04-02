# Estudio Web Server con RGB

[Web Server/ RGB control](https://www.youtube.com/watch?v=YlPXwCDcI1o&list=PL-Hb9zZP9qC65SpXHnTAO0-qV6x5JxCMJ&index=16)

## Temas estudiados

Siguiendo el tutorial al hacerlo en platformio he tenido que hacer algunos ajustes

Estos archivos #include los he tenido que buscar en la instalación .platformio  

#include "protocol_examples_common.h"   
#include "esp_https_server.h"  

Para el archivo html, además del CMakeLists.txt he tenido que modificar el platformio.ini, pero al final todo funciona. 

- [x] Crear un Web server para controlar unos led RGB. 

<div style="text-align: center;">

![](docs/result.png)

</div>
