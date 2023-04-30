//
// Created by anabel on 4/20/2023.
//

#include "help.h"

char *commands[] = { "lsh",
                     "cd",
                     "exit",
                     "help",
                     "redirect",
                     "background",
                     "fg",
                     "jobs",
                     "history",
                     "again",
                     "chain",
                     "if",
                     "multi-if",
                     "true",
                     "false",
                     "get",
                     "set",
                     "unset",
};
char *commands_help[] = { "\nIntegrantes:\nAnabel Benítez González\nAnthuán Montes de Oca Pons\n\nEste es el comando de ayuda de nuestro proyecto\n\nModo de uso: help <keyword>. Si no se especifica, se imprime esta ayuda.\n\nFuncionalidades:\nbasic: funcionalidades básicas (3 puntos)\nmulti-pipe: permite ejecutar multi-pipes (1 punto)\nbackground: permite correr procesos en el background con el operador & (0.5 puntos)\nspaces: los comandos pueden estar separados por cualquier cantidad de espacios (0.5 puntos)\nhistory: permite imprimir y volver a ejecutar los últimos 10 comandos (0.5 puntos)\nchain: permite ejecutar múltiples comandos en una sola línea y de forma condicional con los operadores ; && || (0.5 puntos)\nif: permite ejecutar comandos de forma condicional (1 punto)\nmulti-if: permite anidar condicionales (0.5 puntos)\nhelp: imprime ayuda (1 punto)\nvariables: permite almacenar variables(1 punto)\n\nComandos built-in:\ncd: cambia de directorio\nexit: finaliza la ejecución del shell\nfg: trae hacia el foreground el último un proceso\njobs: lista todos los procesos en el background\nhistory: muestra el historial de comandos\nagain: ejecuta un comando almacenado en el historial\ntrue: representa un comando que siempre se ejecuta con éxito\nfalse: representa un comando que nunca se ejecuta con éxito\nget: muestra el valor de las variables\nset: modifica el valor de una variable\nunset: elimina una variable\n\nTotal: 9.5 puntos\n\n",
                            "El comando cd <dir> cambia el directorio actual del shell al directorio especificado por el usuario, si <dir> no se  especifica se toma por default el home como directorio.\n",
                            "El comando exit finaliza la ejecución del shell.\n",
                            "Help es el comando de ayuda de nuestro proyecto, su modo de uso es help <command>, o bien help para mostrar los comandos con ayuda disponible.\nHelp está implementado como un built-in con redirección e interacción completa con las demás funciones.\n",
                            "Contamos con 4 operadores ( | , < , > , >> ) que nos permiten redirigir la entrada y la salida de los comandos que ejecutemos.\n\nEl comando command1 | command2 ejecuta <command1>, redirecciona la salida de <command1> a la entrada de <command2> y luego ejecuta <command2>.\n\nEl comando command < file redirecciona el contenido del archivo <file> a la entrada de <command> y ejecuta <command>.\n\nEl comando command > file ejecuta <command> redirecciona la salida de <command> hacia el archivo <file>, sobrescribiendo\nel contenido de <file>.\n\nEl comando command >> file hace lo mismo que el comando anterior pero escribe al final de <file>, sin sobrescribir.\n\nAdicionalmente se pueden combinar todas estos comandos en una sola línea.\n",
                            "El operador & al final de un comando ejecuta dicho comando el background esto significa que dicho comando se ejecuta  sin afectar el ciclo del shell, no espera a que el comando termine su ejecución.\n",
                            "El comando fg <pid> trae el proceso enumerado con <pid> desde el background al foreground. Si <pid> no se especifica se toma el último proceso enviado hacia el background.\n",
                            "El comando jobs lista todos los procesos que están ocurriendo en el background.\n",
                            "El comando history muestra los últimos 10 comandos ejecutados en el shell, enumerados desde 1 para el más antiguo hasta 10 para el último comando. Si el comando ejecutado empieza con espacios no será guardado en el historial.",
                            "El comando again <command_number> ejecuta el comando almacenado en el índice <command_number> del historial\n",
                            "Contamos con los operadores ( ; , || , && ).\n\nEl operador ; permite ejecutar varios comandos en la misma línea.\n\nEl comando command1 && command2 ejecuta command1 y si este tiene éxito ejecuta command2\n\nEl comando command1 || command2 ejecuta command1 y si este no tiene éxito ejecuta command2 en caso contrario no se ejecuta más nada.\n",
                            "Los keywords (if, then, else, end) nos permiten realizar una operación condicional.\n\nEl comando if <condition> then <execute1> else <execute2> end, primero ejecuta el comando <condition> y si este tiene éxito entonces se ejecuta <execute1> en caso contrario se ejecuta <execute2>, end se usa para indicar en fin de la operación condicional. El comando else puede no especificarse: if <condition> then <execute> end, si <condition> tiene exito <execute> se ejecutará en caso contrario no pasa nada.\n",
                            "Es posible introducir\nif <condition1> then <command1> else if <condition2> then <command2> end end \ny funciona según lo previsto. También se admite anidar condicinales de cualquier forma.\nA cada if debe corresponderle un then y un end.\n",
                            "El comando true siempre se ejecuta con éxito, puede ver detalles de su uso en chain y conditional.\n",
                            "El comando false siempre falla al ejecutarse, puede ver detalles de su uso en chain y conditional.\n",
                            "El comando get <c> muestra el valor de la variable <c> en el sistema. Si la variable no existe el comando no tendrá éxito.\n",
                            "El comando set <c> <value> permite introducir una nueva variable o modificar el valor de una ya existente.\nSi se desea asignar a la variable la cadena de caracteres <value> entonces debe encontrarse entre comillas set <c> \"value\". Puede contener espacios.\nSi se desea asignar a la variable el resultado de ejecutar un comando entonces debe escribirse entre comillas invertidas set <c> `command`.\nSi se intenta asignar a <c> un valor vacío la ejecución de set no tiene éxito y por tanto no se guarda la variable.\n",
                            "El comando unset <c> elimina la variable <c> del sistema con su respectivo valor. Si <c> no existe el comando no tiene éxito.\n",
};

int lsh_num_commands_help() {
    return sizeof(commands) / sizeof(char *);
}