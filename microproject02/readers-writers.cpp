/**
 * Еремина Дарья Валерьевна
 * Микропроект №2
 * Вариант 8
 *
 * Задача о читателях и писателях-2 («грязное чтение»).
 *
 * Базу данных разделяют два типа потоков – читатели и писатели.
 * Читатели выполняют транзакции, которые просматривают записи базы данных,
 * транзакции писателей и просматривают и изменяют записи.
 * Предполагается, что в начале БД находится в непротиворечивом состоянии
 * (т.е. отношения между данными имеют смысл).
 * Транзакции выполняются в режиме «грязного чтения»,
 * то есть процесс-писатель не может получить доступ к БД только в том случае,
 * если ее занял другой процесс-писатель, а процессы-читатели ему не мешают.
 * Создать многопоточное приложение с потоками-писателями и потоками-читателями.
 * Реализовать решение, используя семафоры, и не используя блокировки чтения-записи.
 */

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <random>
#include <csignal>
#include <string>
#include <ctime>

// Размер "базы данных"
const int dataSize = 3;
// Общий массив данных (база данных), к которому обращаются читатели/писатели
int data[dataSize];

// Двоичные семафоры
sem_t mutex = 1;
sem_t writing = 1;
sem_t reading = 1;

// Количество блоков итераций программы
unsigned int iter;

clock_t start;

// Глобальные переменне, отвечающие за число активных писателей/читателей
// и аналогично ожидающих писателей/читателей
int active_readers = 0;
int active_writers = 0;
int waiting_readers = 0;
int waiting_writers = 0;

// Генератор случайных чисел для изменений и получения значений из "базы данных"
std::mt19937 gen(6274674);
std::uniform_int_distribution<int> distIndex(0, dataSize - 1);
std::uniform_int_distribution<int> distValue(1, 100);

// Метод для чтения данных из базы
void *readData(void *param) {
    int num = *((int *) param);
    for (int i = 0; i < iter; ++i) {
        // Начало критической секции
        sem_wait(&mutex);

        // Если существуют какие-либо писатели (активные или ожидающие),
        // то данный читатель должен подождать окончания их работы,
        // поэтому увеличиваем соответствующий счетчик
        if (active_writers + waiting_writers > 0) {
            ++waiting_readers;
        } else { // Иначе читатель может начать чтение
            sem_post(&reading);
            // Отражаем это также в соответствующем счетчике
            ++active_readers;
        }
        // Конец критической секции
        sem_post(&mutex);
        sem_wait(&reading);

        // Имитация процесса чтения - получение случайного значения из массива
        int index = distIndex(gen);
        printf("%f: Reader №%d gets value %d from data[%d]\n", (float) (clock() - start) / CLOCKS_PER_SEC, num,
               data[index], index);

        // Начало критической секции
        sem_wait(&mutex);

        // В данный момент читатель окончил чтение, отражаем это в счетчике
        --active_readers;

        // Тогда если все активные читатели отработали,
        // запускаем поток записи, если сейчас есть существующие писатели
        if (active_readers == 0 && waiting_writers > 0) {
            sem_post(&writing);
            ++active_writers;
            --waiting_writers;
        }

        // Конец критической секции
        sem_post(&mutex);
    }
    return nullptr;
}

// Метод для записи данных в базу
void *writeData(void *param) {
    int num = *((int *) param);
    for (int i = 0; i < iter; ++i) {
        // Начало критической секции
        sem_wait(&mutex);

        // Если в данный момент происходит чтение/запись,
        // то нужно подождать, чтобы начать записывать
        if (active_writers + active_readers > 0) {
            ++waiting_writers;
        } else { // Иначе можно просто начать запись
            sem_post(&writing);
            ++active_writers;
        }

        // Конец критической секции
        sem_post(&mutex);
        sem_wait(&writing);

        // Имитация процесса записи
        int index = distIndex(gen);
        int value = distValue(gen);
        data[index] = value;
        printf("%f: Writer №%d writes value %d to data[%d]\n", (float) (clock() - start) / CLOCKS_PER_SEC, num, value,
               index);
        fflush(stdout);

        // Начало критической секции
        sem_wait(&mutex);
        // Писатель закончил запись
        --active_writers;

        // Если существуют ожидающие писатели,
        // нужно начать запись
        // (писатели в приоритете, так как наличие читателей не влияет на их доступ к базе)
        if (waiting_writers > 0) {
            sem_post(&writing);
            ++active_writers;
            --waiting_writers;
        } else if (waiting_readers > 0) { // Иначе если очереди ожидает читатель, дать ему получить данныые
            sem_post(&reading);
            ++active_readers;
            --waiting_readers;
        }

        // Конец критической секции
        sem_post(&mutex);
    }
    return nullptr;
}

int main() {
    int n_read;
    int n_write;

    std::cout << "Введите количество писателей: " << std::endl;
    std::cin >> n_write;
    while (n_write < 1 || n_write > 10) {
        std::cout << "Количество должно быть в диапазоне [1; 10]: " << std::endl;
        std::cin >> n_write;
    }

    std::cout << "Введите количество читателей: " << std::endl;
    std::cin >> n_read;
    while (n_read < 1 || n_read > 100) {
        std::cout << "Количество должно быть в диапазоне [1; 100]: " << std::endl;
        std::cin >> n_read;
    }

    std::cout << "Введите количество производимых итераций: " << std::endl;
    std::cin >> iter;
    while (iter < 1 || iter > 100) {
        std::cout << "Количество должно быть в диапазоне [1; 100]: " << std::endl;
        std::cin >> iter;
    }

    // Соответствующие потоки
    pthread_t threadRE[n_read];
    pthread_t threadWR[n_write];

    // Создание потоков
    for (int i = 0; i < n_write; i++) {
        pthread_create(&(threadWR[i]), nullptr, writeData, (void *) &i);
    }
    for (int i = 0; i < n_read; i++) {
        pthread_create(&(threadRE[i]), nullptr, readData, (void *) &i);
    }

    start = clock();
    for (auto &i : threadRE) {
        pthread_join(i, nullptr);
    }
    for (auto &i : threadWR) {
        pthread_join(i, nullptr);
    }
    return 0;
}