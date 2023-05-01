## Shell. Proyecto 1 de Sistemas Operativos

### Integrantes
- Anabel Benitez Gonzalez C311
- Anthuán Montes de Oca Pons C311

El objetivo del proyecto es implementar una terminal de comandos en C que simule las del sistema operativo Linux.

### Ejecutar el proyecto
En la carpeta raíz del proyecto ejecutar el siguiente comando:
```
$ make 
```

### Funcionalidades implementadas

* basic: funcionalidades básicas (3 puntos)
* multi-pipe: permite ejecutar multi-pipes (1 punto)
* background: permite correr procesos en el background con el operador & (0.5 puntos)
* spaces: los comandos pueden estar separados por cualquier cantidad de espacios (0.5 puntos) 
* history: permite imprimir y volver a ejecutar los últimos 10 comandos (0.5 puntos)
* chain: permite ejecutar múltiples comandos en una sola línea y de forma condicional con los operadores ; && || (0.5 puntos)
* if: permite ejecutar comandos de forma condicional (1 punto)
* multi-if: permite anidar condicionales (0.5 puntos)
* help: imprime ayuda (1 punto)
* variables: permite almacenar variables(1 punto)

Total: 9.5 puntos

### Implementación

* basic
1. El shell imprime un prompt con estructura `username@hostname:cwd` seguido de un signo de dollar y un espacio, usando el método `lsh_print_prompt` de `utils.c`, el prompt contiene el directorio donde se encuentra el shell, si es home se sustituye por ~. 
2. Utilizando las funciones `fork` y `execvp` de la librería `unistd.h` y `waitpid` de la librería `wait.h` es posible lograr la ejecución de comandos básicos. Luego de hacer fork para duplicar nuestro proceso, con un if preguntamos por el retorno de la funcion fork, si es 0, es que estamos en el proceso hijo, donde procedemos a hacer execvp con los argumentos, en caso contrario hacemos waitpid con el pid que retorna fork, para esperar a que termine el proceso hijo. Esto se realiza en el método `lsh_launch` de `execute.c`.
3. El comando `cd <dir>` es implementado como una función built-in en `builtin.c`, se ejecuta directamente desde el proceso principal mediante la función `chdir`. Si se pasa un argumento se cambia al directorio que se pasa como argumento, en caso contrario se cambia al directorio home.
4. La redireccion de entrada y salida se lleva a cabo en el método `lsh_launch` de `execute.c`, luego del fork, en el proceso hijo antes de hacer execvp, mediante la funcion `dup2`, se cambia la salida o entrada estándar al fichero que se pasa como argumento, el fichero se abre solamente en el proceso hijo con la función `open`.
5. El uso de tuberías se explica con más detalles en el apartado multi-pipe, su funcionamiento está basado en que antes de hacer fork si se detecta un pipe se abren dos file descriptors con la función `pipe`, y se asigna con `dup2`, la salida estándar del primer proceso al pipe, y la entrada estándar del segundo proceso al pipe, de esta manera la salida del primer proceso es la entrada del segundo.
6. Una vez el comando hijo termina, el padre que hacía wait, continua la ejecución, de esta manera el shell siempre es un loop que espera por EOF para terminar, y siempre que termine un comando, mostrará el prompt esperando por la próxima instrucción.
7. Este punto se explica con más detalle en el apartado spaces, en general se permiten cualquier cantidad de espacios entre los tokens, al ser separados por un espacio cada uno, se hace un split por espacios y se separa cada token.
8. Todos los tokens que se encuentren después del token # serán ignorados por el shell. Si se desea terminar la ejecución del shell, el comando exit hará que el shell salga del loop antes mencionado y termine.

* multi-pipe
1. En la función `lsh_execute_redirections_out` se hace un for que recorre los tokens, si el token `|` es encontrado, se procede a crear un array de tamaño 2, sobre el cual se abre el pipe, se ejecutan las instrucciones a la izquierda con la salida redirigida a array[1] y recursivamente se ejecuta la función `lsh_execute_redirections_out` a la derecha con la entrada redirigida a array[0]. Si no se encuentra el token `|` se ejecuta el comando normalmente.

* background
1. Se verifica si `&` es el último token de la línea y en la función `lsh_background`  se procede a crear un proceso hijo, en el cual se ejecuta el comando, y se retorna al proceso padre, este no espera al hijo, sino que añade el pid del hijo a una lista de procesos en background, y continua la ejecución del shell. Si no se encuentra el token `&` se ejecuta el comando normalmente.
2. El comando `jobs` una función built-in en `builtin.c`, nos permite saber que procesos están en background, esa info se guarda en la lista antes mencionada, cuando un proceso termina, este se elimina de la lista mediante el método `lsh_update_background()` de `builtin.c`. 
3. El comando `fg <pid>` saca al foreground el proceso con pid dado, esto se hace mediante waitpid, en realidad no se saca al fg, si no que se simula, el proceso principal empieza a esperar por el proceso en background. Si no se especifica parámetro entonces `fg` se saca al foreground el último proceso que se mandó al background.

* spaces
1. El shell permite el uso de cualquier cantidad de espacios entre comandos y operadores, en la función `lsh_clean_line` de `utils.c` se hace un for que recorre la línea, si se encuentra un operador, se añade un espacio a cada lado de este, de esta manera se separan los tokens por espacios, luego en el métode `lsh_split_line` de `utils.c` se hace un split por espacios para separar los tokens. Se da la posibilidad de unir tokens separados por espacios si se usan comillas dobles `" "`.

* history 
1. Se guarda en un archivo ubicado en `/home/user` llamado `.shell_history` las últimas 10 entradas que se ejecutan, excepto aquellas que comiencen con un espacio, esta forma de almacenarlo garantiza persistencia.
2. El comando `history` imprime las últimas 10 entradas almacenadas en el archivo mencionado, enumeradas del 1 al 10 de más antigua a más reciente.
3. El comando `again <number>` ejecuta la entrada en history con el número dado. Esto se realiza luego de leerse la línea en la función `replace_again`, que reemplaza las apariciones de `again <number>` válidas, en cualquier lugar de la línea. Si again entre comillas no se reemplaza. Si la línea debe guardarse, se guarda modificada.

* chain
1. Se hace un for buscando estos operadores en la función `lsh_execute_chain` de `execute.c` al encontrarse uno se ejecuta lo que está a la izquierda y recursivamente se siguen buscando operadores chain a la derecha.
2. El operador `;` siempre continúa ejecutando lo que hay a la derecha.
3. el operador `&&` continúa ejecutando lo que hay a la derecha solo si el `exit status` de la izquierda fue 0.
4. El operador `||` continúa ejecutando lo que hay a la derecha solo si el `exit status` de la izquierda fue distinto que 0.

* if
1. El comando `if <condition> then <command1> else <command2> end`, primero se ejecuta el comando <condition> y en dependencia de su exit status entonces procede a ejecutar then o else con el mismo método.
2. else es opcional.
3. if, then, end son obligatorio para que se reconozca la condicional. 
4. El exit status del if en general está dado por el exit status del then o else, en dependencia, de si el exit status de la condición fue 0 o no respectivamente.
5. Para reconocer este comando y ejecutar sus partes se utiliza el método `lsh_execute_conditional` de `execute.c`, cada parte es ejecutada utilizando el método `lsh_execute` de `execute.c`.

* multi-if
1. Con lo explicado en el apartado if, queda bastante claro que los multi-if son naturalemnte permitidos, ya que if ejecuta sus statements como si fueran una "línea completa", no hay problema para que ejecute un if dentro de si mismo. El cambio sería a lo hora de parsear, donde se busca el balanceo entre if y ends para saber cuando nos encontramos dentro de los statements de un if o de otro.

* help
1. El comando `help` sin parámetros imprime una lista de los comandos built-in y una breve descripción de cada uno.
2. El comando `help <command>` imprime la ayuda del comando correspondiente si existe, un mensaje de error si no.
3. `help` está implementado como un built-in con redirección e interacción completa con las demás funciones.

*variables
1. Las variables en nuestro proyecto son guardadas en dos listas simulando un diccionario, con relación <key, value> entre elementos con el mismo índice en las dos lístas.
2. El comando `set` sin parámetro nos permite ver los valores asignados a cada llave.
3. El comando `set <key> <value>` es usado para asignarle un valor a una variable. Si la llave no existe se crea, y en caso de existir ya su valor es sustituido por el nuevo valor.
4. En pos de la simplicidad, se decidió que si se desea asignar la cadena de caracteres de value en la variable esta debe encontrarse entre comillas `" "`, en nuestro shell las comillas cumplen la función de tomar como un solo token lo que está contenido entre ellas. Es importante señalar que en caso de no poner comillas se tomará solo el primer caracter correspondiente al valor. Esta decisión fue tomada para que fuera más simple la implementación de `set` como un built-in.
```  
set a b c d e            #asigna a la variable a el valor b, y da error porque luego del comando set debe haber o el final de linea o un operador chain
set a "b c d e"          #si se desea asignar todo debe escribirse
```
5. Otro uso viene dado por las comillas invertidas, el comando set \<key\> \`command\`, guarda como valor de la llave la salida del comando, para esto se hace un pipe, luego de que termina de ejecutars el comando se lee el pipe y se copia lo que se lee al valor de la llave. Con esto queda claro que se pueden anidar set, el parseo de estos ocurre en la función `parse_set` de `execute.c` para llevar un balance de a qué set pertenecen las \`.
6. Si se intenta asignar un valor vacío a una variable dará error y no se asignará ej: `set o ""` dará error.
```
set x `set b hola`            #se intenta asignar a x la salida de set b hola que es vacía, por tanto este comando asigna a b y luego da error
```
6. La función `unset <key>`, elimina la llave y su valor de las listas.
7. La función `get <key>` podrá usarse en cualquier contexto e imprime el valor de la llave que se pide. En caso de que no esté la llave se imprime que la llave no fue encontrada.