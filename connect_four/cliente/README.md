# Instrucciones para ejecutar Cliente(s) desde la misma Computadora del Servidar
- abra una nueva terminal y acceda al directorio cliente
  ```bash
  cd cdr-fb/connect_four/cliente
  ```
- ejecute el archivo Makefile
  ```bash
  make
  ```
- una vez ejecutado el Makefile, podrá realizar la conexión con el servidor, especificando la ip desde la cual se conecta (en este caso 127.0.0.1) y el puerto en el que se encuentra el servidor (se seguirá utilizando el puerto 7623 como en el ejemplo anterior)
  ```bash
  ./cliente 127.0.0.1 7623
  ```
- de haber seguido correctamente las instrucciones, en su pantalla se presentará el tablero de juego solicitando que ingrese la posición en la que desea colocar su ficha
- ahora podrá jugar su partida de Conecta 4
> [!TIP]
> Si desea abrir más de una partida en simultáneo, puede abrir una nueva terminal desde el mismo computador indicando la misma ip y puerto, se comenzará otra partida completamente ajena en cada terminal que desee ejecutar

# Instrucciones para ejecutar Cliente(s) desde una Computadora distinta del Servidar
- únicamente será necesario que descargue y ejecute los archivos almacenados dentro del directorio cliente
- abra una terminal y acceda al directorio
- ejecute el archivo makefile utilizando el comando
  ```bash
  make
  ```
- una vez ejecutado el Makefile, podrá realizar la conexión con el servidor, especificando la ip desde la cual se conecta y el puerto en el que se encuentra el servidor (se seguirá utilizando el puerto 7623 como en el ejemplo anterior)
> [!TIP]
> Para conocer la IP de su dispositivo, utilice el comando ifconfig
  ```bash
  ./cliente <IP> 7623
  ```
- de haber seguido correctamente las instrucciones, en su pantalla se presentará el tablero de juego solicitando que ingrese la posición en la que desea colocar su ficha
- ahora podrá jugar su partida de Conecta 4
