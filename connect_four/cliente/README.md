# Instrucciones para ejecutar Cliente(s)
- abrir una terminal y acceder al directorio cliente
  ```bash
  cd cdr-fb/connect_four/cliente
  ```
- ejecutar Makefile
  ```bash
  make
  ```
- una vez ejecutado el Makefile, podrá realizar la conexión con el servidor, especificando la ip desde la cual se conecta y el puerto en el que se encuentra el servidor (se seguirá utilizando el puerto 7623 como en el ejemplo anterior)
  ```bash
  ./cliente <IP> 7623
  ```
- de haber seguido correctamente las instrucciones, en su pantalla se presentará el tablero de juego solicitando que ingrese la posición en la que desea colocar su ficha
- ahora podrá jugar su partida de Conecta 4
> [!TIP]
> Si desea abrir más de una partida en simultáneo, puede abrir una nueva terminal desde el mismo computador indicando la misma ip y puerto, se comenzará otra partida completamente ajena en cada terminal que desee ejecutar

