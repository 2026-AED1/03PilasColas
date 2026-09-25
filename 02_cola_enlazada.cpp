// 02_cola_enlazada.cpp
// Objetivo: el TAD Cola (FIFO) sobre una lista enlazada.
//
// La Cola toca los DOS extremos, pero cada uno con una sola operacion:
// se encola por el final y se desencola por el principio. Primero en entrar,
// primero en salir (First In, First Out).
//
// Aqui se ve de golpe por que en el tema anterior nos molestamos en anadir
// ultimo_ a la lista enlazada: sin ese puntero, encolar seria O(n) y la
// estructura entera no valdria para nada.
//
// Y tambien por que la eleccion de extremos NO es simetrica:
//    encolar por el final   + desencolar por el principio  -> las dos O(1)
//    encolar por el principio + desencolar por el final    -> desencolar O(n)
// porque desde el ultimo nodo no se puede retroceder al anterior. Con un
// solo enlace por nodo, solo una de las dos asignaciones es posible.
//
// Compilar:
//   g++ -std=c++17 -Wall -o 02_cola 02_cola_enlazada.cpp && ./02_cola

#include <iostream>
#include <stdexcept>

// =========================================================================
// LA ESPECIFICACION
// =========================================================================
//
//  encolar(v)     pre: -             post: v pasa a ser el ultimo
//  desencolar()   pre: -             post: si no estaba vacia, elimina el
//                                          primero y devuelve true
//  frente()       pre: !vacia()      post: devuelve el primero; no modifica
//  vacia()        pre: -             post: devuelve n == 0
//  tamanio()      pre: -             post: devuelve n
//
// Mismo criterio que en la Pila: frente() lleva precondicion y lanza;
// desencolar() devuelve bool.

class ColaEnlazada {
public:
    ColaEnlazada() : primero_(nullptr), ultimo_(nullptr), n_(0) {}

    // Regla de los tres: hay memoria dinamica, asi que o se implementan las
    // tres o se prohiben. Aqui las implementamos, para que la clase sea
    // utilizable de verdad.
    ColaEnlazada(const ColaEnlazada& otra)
        : primero_(nullptr), ultimo_(nullptr), n_(0) { copiarDesde(otra); }

    ColaEnlazada& operator=(const ColaEnlazada& otra) {
        if (this != &otra) { liberar(); copiarDesde(otra); }
        return *this;
    }

    ~ColaEnlazada() { liberar(); }

    void encolar(int valor) {                    // O(1)  gracias a ultimo_
        // TODO: implementar
    }

    bool desencolar() {                          // O(1)
        // TODO: implementar
    }

    int frente() const {                         // O(1)
        if (primero_ == nullptr) throw std::out_of_range("frente() sobre cola vacia");
        return primero_->valor;
    }

    int  tamanio() const { return n_; }          // O(1)
    bool vacia() const { return n_ == 0; }

    void imprimir() const {
        std::cout << "  [frente] ";
        for (Nodo* p = primero_; p != nullptr; p = p->siguiente) std::cout << p->valor << ' ';
        std::cout << "[final]  (n=" << n_ << ")\n";
    }

    // Comprobacion explicita del invariante. Es la traduccion literal del
    // bloque de comentarios de mas abajo, y sirve para depurar.
    bool invarianteCorrecto() const {
        if (n_ < 0) return false;
        if ((n_ == 0) != (primero_ == nullptr)) return false;
        if ((primero_ == nullptr) != (ultimo_ == nullptr)) return false;
        if (n_ == 0) return true;
        if (ultimo_->siguiente != nullptr) return false;

        int  contados = 0;
        bool ultimoAlcanzable = false;
        for (Nodo* p = primero_; p != nullptr; p = p->siguiente) {
            contados++;
            if (p == ultimo_) ultimoAlcanzable = true;
            if (contados > n_) return false;        // hay ciclo o n_ miente
        }
        return contados == n_ && ultimoAlcanzable;
    }

private:
    struct Nodo {
        int   valor;
        Nodo* siguiente;
    };

    Nodo* primero_;      // por aqui se SALE
    Nodo* ultimo_;       // por aqui se ENTRA
    int   n_;

    // --- Invariante de la representacion ---------------------------------
    //  n_ >= 0
    //  n_ == 0  <=>  primero_ == nullptr  &&  ultimo_ == nullptr
    //  n_ >  0  =>   ultimo_->siguiente == nullptr
    //                &&  ultimo_ es alcanzable desde primero_
    //                &&  n_ es el numero exacto de nodos
    //
    // Los dos casos que rompen todas las colas mal escritas:
    //    encolar sobre cola VACIA        (hay que fijar tambien primero_)
    //    desencolar dejandola VACIA      (hay que anular tambien ultimo_)
    // Si no se hace lo segundo, ultimo_ queda apuntando a memoria liberada:
    // el siguiente encolar escribe en un puntero colgante. Es un fallo que
    // no se manifiesta hasta mucho despues, y es el clasico de esta clase.
    // ---------------------------------------------------------------------

    void liberar() {
        while (desencolar()) {}
    }

    void copiarDesde(const ColaEnlazada& otra) {
        for (Nodo* p = otra.primero_; p != nullptr; p = p->siguiente) encolar(p->valor);
    }
};

// Cliente: una simulacion minima de atencion por turnos. Ni una linea
// depende de que dentro haya nodos.
void simularVentanilla(ColaEnlazada& cola, int cuantosAtender) {
    for (int i = 0; i < cuantosAtender && !cola.vacia(); i++) {
        std::cout << "    atiendo al " << cola.frente() << '\n';
        cola.desencolar();
    }
}

int main() {
    ColaEnlazada c;
    std::cout << "--- Encolando 1..6 ---\n";
    for (int i = 1; i <= 6; i++) c.encolar(i);
    c.imprimir();
    std::cout << "  invariante correcto: " << std::boolalpha << c.invarianteCorrecto() << '\n';

    std::cout << "\n--- Atendiendo a tres ---\n";
    simularVentanilla(c, 3);
    c.imprimir();

    std::cout << "\n--- Copia (regla de los tres) ---\n";
    ColaEnlazada copia = c;          // constructor de copia
    c.desencolar();                  // se modifica el original...
    std::cout << "  original: "; c.imprimir();
    std::cout << "  copia:    "; copia.imprimir();
    std::cout << "  ...y la copia no se entera: son colas independientes.\n";

    std::cout << "\n--- El caso limite: vaciar y volver a llenar ---\n";
    while (c.desencolar()) {}
    std::cout << "  vacia: " << c.vacia()
              << ", invariante: " << c.invarianteCorrecto() << '\n';
    c.encolar(99);                   // aqui se rompe todo si desencolar no
                                     // anulo ultimo_
    c.imprimir();
    std::cout << "  invariante tras revivir: " << c.invarianteCorrecto() << '\n';

    std::cout << "\n--- Violando la precondicion de frente() ---\n";
    ColaEnlazada vacia;
    try { vacia.frente(); }
    catch (const std::out_of_range& e) { std::cout << "  Excepcion: " << e.what() << '\n'; }
}
