#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {
    WSADATA wsaData; // Структура для хранения информации о реализации WinSock
    ADDRINFO hints; // Структура для хранения информации о желаемом типе адресов
    ADDRINFO* addrResult; // Указатель на результирующую структуру адресов
    SOCKET ConnectSocket = INVALID_SOCKET; // Переменная для хранения сокета
    char recvBuffer[512]; // Буфер для приема данных

    const char* sendBuffer1 = "Hello from client 1"; // Сообщение для отправки 1
    const char* sendBuffer2 = "Hello from client 2"; // Сообщение для отправки 2

    // Инициализация Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    // Настройка структуры hints
    ZeroMemory(&hints, sizeof(hints)); // Обнуление памяти для структуры
    hints.ai_family = AF_INET; // Использование IPv4
    hints.ai_socktype = SOCK_STREAM; // Установка типа сокета на потоковый (TCP)
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP

    // Получение адреса и порта сервера
    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;
        WSACleanup();
        return 1;
    }

    // Создание сокета для подключения к серверу
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для адресной информации
        WSACleanup(); // Очистка ресурсов Winsock
        return 1;
    }

    // Подключение к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Unable to connect to server" << endl;
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для адресной информации
        WSACleanup(); // Очистка ресурсов Winsock
        return 1;
    }

    // Отправка первого сообщения
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для адресной информации
        WSACleanup(); // Очистка ресурсов Winsock
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    // Отправка второго сообщения
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для адресной информации
        WSACleanup(); // Очистка ресурсов Winsock
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    // Завершение отправляющей стороны сокета, так как больше данных не будет отправлено
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed, error: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для адресной информации
        WSACleanup(); // Очистка ресурсов Winsock
        return 1;
    }

    // Получение данных до тех пор, пока сервер не закроет соединение
    do {
        ZeroMemory(recvBuffer, 512); // Обнуление буфера приема
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "Connection closed" << endl;
        }
        else {
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
        }
    } while (result > 0);

    // Очистка ресурсов
    closesocket(ConnectSocket); // Закрытие сокета
    freeaddrinfo(addrResult); // Освобождение памяти, выделенной для адресной информации
    WSACleanup(); // Очистка ресурсов Winsock
    return 0;
}
