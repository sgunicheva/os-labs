#include <cstring>
#include <iostream>
#include <cmath>
#include <unistd.h>
#include <valarray>
#include <vector>
#include <fstream>

using namespace std;

int pipe_in[2];
int pipe_out[2];
pid_t pid;
string infile;
string outfile;

void help_message() {
    cout << "Help:"<< endl;
    cout << "--help: Show this help" << endl;
    cout << "<input_file> <output_file>: Read and Write from files" << endl;
}

double determinant(const vector<vector<double>> &matrix) {
    int n = matrix.size();

    // Базовый случай: матрица 1x1
    if (n == 1) {
        return matrix[0][0];
    }

    // Базовый случай: матрица 2x2
    if (n == 2) {
        return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
    }

    double det = 0;

    // Разложение по первой строке
    for (int col = 0; col < n; col++) {
        // Создаем подматрицу (минор)
        vector<vector<double>> submatrix(n - 1, vector<double>(n - 1));

        for (int i = 1; i < n; i++) {
            int sub_col = 0;
            for (int j = 0; j < n; j++) {
                if (j == col) continue;
                submatrix[i - 1][sub_col] = matrix[i][j];
                sub_col++;
            }
        }

        // Рекурсивно вычисляем определитель подматрицы
        double sub_det = determinant(submatrix);

        // Добавляем к общему определителю с учетом знака
        det += (col % 2 == 0 ? 1 : -1) * matrix[0][col] * sub_det;
    }

    return det;
}

bool correct_number_n(const string& number) {
    try {
        stoi(number);
    }catch (invalid_argument e) {
        return true;
    }
    return false;
}

bool correct_number(const string& number) {
    try {
        stod(number);
    }catch (invalid_argument e) {
        return true;
    }
    return false;
}


void server() {
    int n;
    read(pipe_in[0],&n,sizeof(n));

    vector<vector<double>> matrix(n, vector<double>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double temp;
            read(pipe_in[0],&temp,sizeof(temp));
            matrix[i][j] = temp;
        }
    }

    const double det = determinant(matrix);

    write(pipe_out[1],&det,sizeof(det));
}

void client(const bool file_flag) {
    if (!file_flag) {
        cout << "Введите размер квадратной матрицы (n x n): ";
        bool correct = true;
        string n_temp;
        while (correct) {
            cin >> n_temp;
            correct = correct_number_n(n_temp);
            if (correct) {
                cout << "Неверное число" << endl;
            }
        }
        int n = stoi(n_temp);

        write(pipe_in[1],&n,sizeof(n));
        for (int i = 0; i < n; i++) {
            cout << "Строка " << i+1 << ": ";
            for (int j = 0; j < n; j++) {
                string temp;
                bool correct_matrix = true;
                while (correct_matrix) {
                    cin >> temp;
                    correct_matrix = correct_number(temp);
                    if (correct_matrix) {
                        cout << "Неверное число" << endl;
                    }
                }
                double result = stod(temp);
                write(pipe_in[1],&result,sizeof(result));
            }
        }

        double det;
        read(pipe_out[0],&det,sizeof(det));
        cout << det << endl;
    }else {
        ifstream in(infile);
        ofstream out(outfile);
        if (!in.is_open()) {
            cerr << "Unable to open file " << infile << endl;
            exit(1);
        }
        int n;
        in >> n;
        write(pipe_in[1],&n,sizeof(n));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                double temp;
                in >> temp;
                write(pipe_in[1],&temp,sizeof(temp));
            }
        }
        in.close();

        double det;
        read(pipe_out[0],&det,sizeof(det));
        out << det << endl;
        out.close();
    }

 }

void process(const bool file_flag) {
    pipe(pipe_in);
    pipe(pipe_out);
    pid = fork();
    if (pid < 0) {
        cerr << "Fork failed" << endl;
        exit(1);
    }

    if (pid > 0) {
        client(file_flag);
    }else {
        server();
    }
    for (int i = 0; i < 2; i++) {
        close(pipe_in[i]);
        close(pipe_out[i]);
    }
}

int main(const int argc, char const *argv[]) {
    if (argc == 2 && !strcmp(argv[1], "--help")) {
        help_message();
        return 0;
    }
    if (argc == 1) {
        process(false);
        return 0;
    }
    if (argc == 3) {
        infile = argv[1];
        outfile = argv[2];
        process(true);
        return 0;
    }
    return 1;
}