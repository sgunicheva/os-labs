#include <winsock2.h>
#include<iostream>
#include <queue>
#include <stack>


bool check_zero(std::string current) {
    for (int i = 0; i < current.length()-1; i++) {
        if (current[i] == '/' && current[i+1] == '0') {
            if (i+2 <= current.length() && current[i+2] == '.') {

            }else {
                return false;
            }
        }
    }
    return true;
}

int precedence(const char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

std::string calculate(std::string& expr) {
    std::stack<char> operators;
    std::queue<std::string> output;
    std::string number;
    bool lastWasOperator = true;

    for (size_t i = 0; i < expr.length(); i++) {
        char ch = expr[i];

        if (isdigit(ch) || ch == '.') {
            number += ch;
            lastWasOperator = false;
        } else if (ch == '-' && lastWasOperator) {
            number += ch;
        } else {
            if (!number.empty()) {
                output.push(number);
                number.clear();
            }

            if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
                while (!operators.empty() && precedence(operators.top()) >= precedence(ch)) {
                    output.push(std::string(1, operators.top()));
                    operators.pop();
                }
                operators.push(ch);
                lastWasOperator = true;
            }else {
                lastWasOperator = false;
            }
        }
    }

    if (!number.empty()) output.push(number);

    while (!operators.empty()) {
        output.push(std::string(1, operators.top()));
        operators.pop();
    }

    std::stack<double> values;
    while (!output.empty()) {
        std::string token = output.front();
        output.pop();

        if (isdigit(token[0]) || (token.length()>1 && token[0] == '-')) {
            values.push(stod(token));
        } else {
            const double b = values.top(); values.pop();
            const double a = values.top(); values.pop();

            if (token == "+") values.push(a + b);
            if (token == "-") values.push(a - b);
            if (token == "*") values.push(a * b);
            if (token == "/") values.push(a / b);
        }
    }

    return std::to_string(values.top());
}


static bool check_number(std::string number) {
    if (number[0] == '-') {
        number.erase(0, 1);
    }
    int dot_count = 0;
    for (int i = number.size() - 1; i >= 0; i--) {
        if (number[i] == '.') {
            dot_count++;
            if (dot_count > 1) {
                return false;
            }
        } else if (!isdigit(number[i])) {
            return false;
        }
    }
    return true;
}

bool try_to_add(std::string str, std::string &current_number) {
    if (current_number.empty()) {
        if (check_number(str)) {
            current_number = str;
            return true;
        }
        return false;
    }
    if (str[0] == '+' || str[0] == '-' || str[0] == '*' || str[0] == '/') {
        const char type = str[0];
        str.erase(0, 1);
        if (str.length() == 0) {
            return false;
        }
        if (check_number(str)) {
            current_number = current_number + type + str;
            return true;
        }
        return false;
    }
    return false;
}


std::string from_client(SOCKET sock) {
    std::string message;
    char buffer[1024];

    memset(buffer, 0, sizeof(buffer));

    if (recv(sock, buffer, sizeof(buffer) - 1, 0) == -1) {
        std::cerr << "User disconnected" << std::endl;
        if(closesocket(sock) == SOCKET_ERROR){
            std::cerr << "Failed close socket" << std::endl;
        };
        WSACleanup();
        exit(1);
    }
    message = std::string(buffer);
    std::cout << message << std::endl;
    return message;
}

void to_client(std::string message, SOCKET socket) {
    if (send(socket, message.c_str(), strlen(message.c_str()), 0) == SOCKET_ERROR) {
        std::cerr << "User disconnected";
        if(closesocket(socket) == SOCKET_ERROR){
            std::cerr << "Failed close socket" << std::endl;
        };
        WSACleanup();
        exit(1);
    }
    std::cout <<"To client: " <<message << std::endl;
}

void start() {
    const auto run_client = R"(start C:\client.exe)";
    system(run_client);
}

int main() {
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cerr << "Error: failed to link library.\n";
        return 1;
    }

    start();

    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(sock, (SOCKADDR *) &addr, sizeof(addr)) == SOCKET_ERROR) {
        if(closesocket(sock) == SOCKET_ERROR){
            std::cerr << "Failed close socket" << std::endl;
        };
        WSACleanup();
        std::cerr << "Error: failed to bind.\n";
    }



    if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
        if(closesocket(sock) == SOCKET_ERROR){
            std::cerr << "Failed close socket" << std::endl;
        };
        WSACleanup();
        std::cerr << "Error: failed to listen.\n";
    }



    SOCKET newCon;
    int addr_size = sizeof(addr);
    newCon = accept(sock, (SOCKADDR *) &addr, &addr_size);

    if (newCon == SOCKET_ERROR) {
        if(closesocket(sock) == SOCKET_ERROR){
            std::cerr << "Failed close socket" << std::endl;
        };
        WSACleanup();
        std::cerr << "Error: failed to accept connection.\n";
    }

    std::string current_number = "";
    while (true) {
        std::string message = from_client(newCon);
        std::cout <<"From client:" << message << std::endl;

        if (message == "remove") {

            current_number = "";
            std::string message_to_send = "Success delete";
            to_client(message_to_send, newCon);
        } else if (message == "check") {
            if (check_zero(current_number)) {
                std::string message_to_send = "Current expression is correct";
                to_client(message_to_send, newCon);
            }else {
                std::string message_to_send = "Current expression is incorrect";
                to_client(message_to_send, newCon);
            }
        } else if (message == "=") {
            if (check_zero(current_number)) {
                std::string message_to_send = calculate(current_number);
                to_client(message_to_send, newCon);
            }
            else {
                std::string message_to_send = "Current expression is incorrect";
                to_client(message_to_send, newCon);
            }
        } else {
            if (try_to_add(message, current_number)) {
                std::string message_to_send = "Current: " + current_number;
                to_client(message_to_send, newCon);
            } else {
                std::string message_to_send = "Incorrect message,try again";
                to_client(message_to_send, newCon);
            }
        }
    }
}
