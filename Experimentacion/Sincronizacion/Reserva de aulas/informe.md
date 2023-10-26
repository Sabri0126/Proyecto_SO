# 1.2. Sincronización
## Reserva de Aulas
### (I) Diseñe e implemente un modelo que se adapte al enunciado
Para resolver el problema utilizaremos un recurso compartido que serán las horas a reservar.
Estas horas se representarán como un arreglo de "booleanos" de longitud acorde a la cantidad de horas disponibles.

Los `alumnos` se ejecutan de manera concurrente realizando las operaciones aleatoriamente utilizando las probabilidades dadas.

El alumno almacenara la hora reservada (en caso de que tuviera una).

- Para las reservas se eligirá la hora de manera aleatoria y se intentará registrarlo en la tabla de horas y se guardará su pocición. 
En caso de que no se pueda realizar la reserva. (El alumno deberia esperar o continuar?)
- Para la cancelacion se utilizara el horario almacenado.
- Para la consulta tambien se elijirá el horario de manera aleatoria.


    Para registrar y cancelar reservas se garantizará la exclusión mutua al momento de modificar la tabla de reservas.

### (II) Explique el comportamiento que presenta su solución.
### (III)Resuelva el mismo problema utilizando procesos y memoria compartida.