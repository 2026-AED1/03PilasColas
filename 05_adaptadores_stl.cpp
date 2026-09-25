// 05_adaptadores_stl.cpp
// Objetivo: cerrar el bloque de Pila/Cola/Deque viendo como la STL implementa
// estos TADs, y por que su forma de hacerlo es la definicion misma de TAD.
//
// std::stack y std::queue NO son estructuras de datos. Son ADAPTADORES:
// interfaces recortadas sobre OTRO contenedor, que se pasa como parametro de
// plantilla.
//
//     template <class T, class Contenedor = std::deque<T>>
//     class stack;
//
// Es decir: la especificacion es lo unico que se ve, y la implementacion es
// literalmente un argumento que puedes cambiar sin tocar tu codigo. El TAD
// del tema 2, convertido en un mecanismo del lenguaje.
//
// Lo mas instructivo de un adaptador es lo que QUITA. Sobre un std::deque
// puedes hacer v[3]; sobre un std::stack construido encima del mismo deque,
// no. No porque no se pueda: porque el TAD Pila dice que no se debe, y el
// adaptador lo hace imposible de escribir. El compilador pasa a defender el
// contrato.
//
// (Cuando veamos arboles, retomaremos esta misma idea con std::priority_queue,
// el tercer adaptador de la STL, que ademas permite elegir el ORDEN como
// parametro de plantilla.)
//
// Compilar:
//   g++ -std=c++17 -O2 -Wall -o 05_stl 05_adaptadores_stl.cpp && ./05_stl

#include <iostream>
#include <iomanip>
#include <stack>
#include <queue>
#include <vector>
#include <deque>
#include <list>
#include <chrono>

template <typename F>
double milisegundos(F f) {
    auto t0 = std::chrono::steady_clock::now();
    f();
    auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

int main() {
    const int N = 2000000;

    std::cout << "=== 1) El contenedor subyacente es un parametro ===\n";
    std::cout << "  La misma pila, tres representaciones distintas por debajo.\n";
    std::cout << "  El codigo que las usa es identico; solo cambia el tipo.\n\n";
    std::cout << "  " << std::left << std::setw(34) << "tipo" << "tiempo (ms)\n";

    double t1 = milisegundos([&] {
        std::stack<int, std::deque<int>> p;              // el que trae por defecto
        for (int i = 0; i < N; i++) p.push(i);
        while (!p.empty()) p.pop();
    });
    std::cout << "  " << std::left << std::setw(34) << "stack<int, deque<int>>  (def.)"
              << std::fixed << std::setprecision(1) << t1 << '\n';

    double t2 = milisegundos([&] {
        std::stack<int, std::vector<int>> p;              // suele ser el mas rapido
        for (int i = 0; i < N; i++) p.push(i);
        while (!p.empty()) p.pop();
    });
    std::cout << "  " << std::left << std::setw(34) << "stack<int, vector<int>>"
              << t2 << '\n';

    double t3 = milisegundos([&] {
        std::stack<int, std::list<int>> p;                // un new por elemento
        for (int i = 0; i < N; i++) p.push(i);
        while (!p.empty()) p.pop();
    });
    std::cout << "  " << std::left << std::setw(34) << "stack<int, list<int>>"
              << t3 << '\n';

    std::cout << "\n  Los tres son O(1) amortizado por operacion. Los tres cumplen\n"
              << "  el mismo contrato y dan el mismo resultado. Y sin embargo hay\n"
              << "  un factor " << std::setprecision(1) << t3 / t2 << " entre el mejor y el peor:\n"
              << "  el TAD garantiza el RESULTADO, nunca el COSTE.\n";

    std::cout << "\n  Por que vector no es el defecto, si suele ganar: porque al\n"
              << "  crecer reasigna y COPIA, y eso invalida referencias y da picos\n"
              << "  de latencia. deque crece por bloques: nunca copia lo que ya\n"
              << "  habia. La STL elige por defecto lo predecible, no lo mas rapido\n"
              << "  en promedio.\n";

    std::cout << "\n=== 2) Lo que el adaptador PROHIBE ===\n";
    std::cout << "  std::deque<int> d;   d[3]        -> compila\n"
              << "  std::stack<int> p;   p.top()     -> compila\n"
              << "                       p[3]        -> NO COMPILA\n"
              << "                       p.begin()   -> NO COMPILA\n\n"
              << "  Un std::stack no tiene iteradores: no se puede recorrer. Eso\n"
              << "  no es una carencia, es el TAD. Si necesitas recorrerlo, lo que\n"
              << "  necesitas no era una pila.\n";

    std::cout << "\n=== 3) Tabla del bloque ===\n";
    std::cout << "  TAD              STL                  contenedor por defecto\n"
              << "  ---------------------------------------------------------\n"
              << "  Pila             std::stack           std::deque\n"
              << "  Cola             std::queue           std::deque\n"
              << "  Deque            std::deque           (es una estructura)\n\n"
              << "  Falta la Cola de Prioridad y su std::priority_queue: los veremos\n"
              << "  mas adelante, junto con los arboles, porque el monticulo binario\n"
              << "  que hay detras es un tipo de arbol.\n";
}
