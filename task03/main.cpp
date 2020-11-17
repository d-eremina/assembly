/**
 * Еремина Дарья Валерьевна
 * БПИ196
 * Вариант 8
 * Найти решение СЛАУ (4х4), используя формулы Крамера.
 * Предусмотреть возможность деления на ноль. Входные данные: коэффициенты системы.
 */

#include <cstdio>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <vector>

pthread_mutex_t mutex; // двоичный семафор
const int rank = 4; // размерность матрицы
double x[rank]; // массив результирующих значений
int **matrix = new int *[rank];
int *b = new int[rank];
int *num = new int[rank];
int det = 0;

// Считает определитель матрицы 4х4
int determinant4(int **m) {
    return m[0][0] * (m[1][1] * m[2][2] * m[3][3] + m[2][1] * m[3][2] * m[1][3] + m[1][2] * m[2][3] * m[3][1] -
                      m[1][3] * m[2][2] * m[3][1] - m[2][3] * m[3][2] * m[1][1] - m[1][2] * m[2][1] * m[3][3]) -
           m[0][1] * (m[1][0] * m[2][2] * m[3][3] + m[2][0] * m[3][2] * m[1][3] + m[1][2] * m[2][3] * m[3][0] -
                      m[1][3] * m[2][2] * m[3][0] - m[2][3] * m[3][2] * m[1][0] - m[1][2] * m[2][0] * m[3][3]) +
           m[0][2] * (m[1][0] * m[2][1] * m[3][3] + m[2][0] * m[3][1] * m[1][3] + m[1][1] * m[2][3] * m[3][0] -
                      m[1][3] * m[2][1] * m[3][0] - m[2][3] * m[3][1] * m[1][0] - m[1][1] * m[2][0] * m[3][3]) -
           m[0][3] * (m[1][0] * m[2][1] * m[3][2] + m[2][0] * m[3][1] * m[1][2] + m[1][1] * m[2][2] * m[3][0] -
                      m[1][2] * m[2][1] * m[3][0] - m[2][2] * m[3][1] * m[1][0] - m[1][1] * m[2][0] * m[3][2]);
}

//стартовая функция для дочерних потоков
void *find_x(void *param) {
    int column = *(int *) param;  // номер столбца, вместо которого подставляем b (= соответствующий x)
    int **submatrix = new int *[rank]; // модифицированная матрица
    pthread_mutex_lock(&mutex);
    // начало критической секциио
    // только один поток должен работать в текущий момент для безопасности кода и отсутствия непредвиденных значений
    for (int i = 0; i < rank; i++) {
        submatrix[i] = new int[rank];
    }
    for (int i = 0; i < rank; ++i) {
        for (int j = 0; j < rank; ++j) {
            if (j == column) {
                submatrix[i][j] = b[i];
            } else {
                submatrix[i][j] = matrix[i][j];
            }
        }
    }
    int det1 = determinant4(submatrix);
    x[column] = (double) det1 / det;
    // конец критической секции
    pthread_mutex_unlock(&mutex);
    //открыть двоичный семафор
    return nullptr;
}

int read_test() {
    std::cout << "enter test number in range [1, 5]:\n";
    int32_t test;
    std::cin >> test;
    if (test < 1 || test > 5) {
        std::cout << "wrong number";
        exit(1);
    }

    std::string path = "../input/test" + std::to_string(test) + ".txt";
    std::ifstream in;
    in.open(path);

    try {
        for (int i = 0; i < rank; ++i) {
            for (int j = 0; j < rank + 1; ++j) {
                std::string c;
                in >> c;
                if (j < rank) {
                    matrix[i][j] = std::stoi(c);
                } else {
                    b[i] = std::stoi(c);
                }
            }
        }
    }
    catch (...) {
        std::cout << "wrong file format";
        exit(1);
    }

    if (!in.eof()) {
        std::cout << "wrong file format";
        exit(1);
    }
}

int read_console() {
    for (int i = 0; i < rank; ++i) {
        for (int j = 0; j < rank + 1; ++j) {
            if (j < rank) {
                std::cin >> matrix[i][j];
            } else {
                std::cin >> b[i];
            }
        }
    }
}

int choose_option() {
    int a;
    std::cout << "choose input option:\n1 - from console\n2 - from test file\n";
    std::cin >> a;
    if (!(a == 1 || a == 2)) {
        std::cout << "wrong option\n";
        exit(1);
    }
    return a;
}

void check_det() {
    if (det == 0) {
        std::cout << "determinant of matrix A equals 0. can't solve this way" << std::endl;
        exit(1);
    }
}

void create_threads(std::vector<pthread_t> &threads) {
    for (int i = 0; i < threads.size(); ++i) {
        pthread_create(&threads[i], nullptr, find_x, (void *) (num + i));
    }
}

void join_threads(std::vector<pthread_t> &threads) {
    for (auto &thread : threads) {
        pthread_join(thread, nullptr);
    }
}

int main() {
    for (int i = 0; i < rank; i++) {
        matrix[i] = new int[rank];
    }

    if (choose_option() == 1) {
        std::cout << "you can now enter coefficients:\n";
        read_console();
    } else {
        read_test();
    }

    det = determinant4(matrix);
    check_det();

    // инициализация двоичного семафора
    pthread_mutex_init(&mutex, nullptr);
    // вектор используемых потоков
    std::vector<pthread_t> threads = std::vector<pthread_t>(rank);
    for (int i = 0; i < rank; i++) {
        num[i] = i; // номера столбцов для потоков
    }
    create_threads(threads);
    join_threads(threads);

    // вывод результата вычислений всех потоков
    printf("\nОТВЕТ:\n");
    for (int i = 0; i < rank; i++) {
        std::cout << "x" + std::to_string(i) + ": " << x[i] << "\n";
    }
    return 0;
}