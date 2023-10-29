# 1.2. Sincronización
## Reserva de Aulas
### (I) Diseñe e implemente un modelo que se adapte al enunciado
Para resolver el problema utilizaremos un recurso compartido que serán las horas a reservar.
Estas horas se representarán como un arreglo de "booleanos" de longitud acorde a la cantidad de horas disponibles.

Los `alumnos` se ejecutan de manera concurrente realizando las operaciones aleatoriamente utilizando las probabilidades dadas.

El alumno almacenara la hora reservada (en caso de que tuviera una).

- Para las reservas se eligirá la hora de manera aleatoria y se intentará registrarlo en la tabla de horas y se guardará su posición. 
En caso de que no se pueda realizar la reserva el alumno el alumno tendrá que volver a elegir una operación. 
- Para la cancelacion se utilizara el horario almacenado.
- Para la consulta tambien se elijirá el horario de manera aleatoria.


Para registrar y cancelar reservas se garantizará la exclusión mutua al momento de modificar la tabla de reservas.

### (II) Explique el comportamiento que presenta su solución.

La solución retorna mensajes con los eventos ocurridos durante la ejecucion del programa. Los eventos que podrian ocurrir son los siguientes:

- El alumno reserva un turno. 
  - Ya contaba con un turno previo.
  - La hora que intenta reservar ya fue reservada.
  - Se realiza la reserva correctamente.
- El alumno cancela la reserva.
  - Intenta cancelar una reserva pero no tiene ninguna.
  - Se cancela la reserva correctamente.
- El alumno consulta el estado de un horario.
  - El horario se encuentra reservado.
  - El horario no se encuentra reseervado.

 Se consideró la posible inanicion de los procesos que quieren realizar reservas/cancelaciones. En esta solucion se dio prioridad a las consultas frente a las reservas/cancelaciones debido a que las consultas ocurren con menor frecuencia reduciendo la probabilidad de inanicion y simplificando la implementacion de la solución.

### (III)Resuelva el mismo problema utilizando procesos y memoria compartida.
