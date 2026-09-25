// 01_pila_tad.cpp
// Objetivo: el TAD Pila (LIFO) como contrato, y DOS implementaciones
// intercambiables detras de la misma interfaz.
//
// Idea central de la clase: Pila, Cola, Deque y Cola de Prioridad no son
// estructuras de datos nuevas. Son RESTRICCIONES sobre la Lista del tema
// anterior. Quitamos operaciones, y a cambio ganamos dos cosas:
//    1) todas las que quedan cuestan O(1),
//    2) el codigo cliente se vuelve más fácil de razonar.
//
// El TAD Pila solo permite tocar UN extremo: el que entra el último, sale
// el primero (Last In, First Out).
//
// Compilar:
//   g++ -std=c++17 -Wall -o 01_pila 01_pila_tad.cpp && ./01_pila

#include <iostream>
#include <iomanip>
#include <stdexcept>

// =========================================================================
// LA ESPECIFICACION
// =========================================================================
//
//  apilar(v)    pre: -                 post: v pasa a ser la cima
//  desapilar()  pre: -                 post: si no estaba vacía, elimina la
//                                            cima y devuelve true
//  cima()       pre: !vacia()          post: devuelve la cima; no modifica
//  vacia()      pre: -                 post: devuelve n == 0
//  tamanio()    pre: -                 post: devuelve n
//
// Seguimos el mismo criterio del tema 2:
//   - una operacion que DEVUELVE UN VALOR y puede no tener ninguno que
//     devolver (cima) lleva precondicion, se comprueba, y lanza.
//   - una operacion sobre el extremo que solo modifica (desapilar) no lleva
//     precondicion y devuelve bool: "pila vacía" es un estado legítimo.
//
// Lo que el TAD NO dice: si hay nodos, si hay un array, ni en que orden
// estan guardados los elementos en memoria.

class PilaDeEnteros {
public:
    virtual ~PilaDeEnteros() = default;      // destructor virtual: obligatorio

    virtual void apilar(int valor) = 0;
    virtual bool desapilar() = 0;
    virtual int  cima() const = 0;
    virtual int  tamanio() const = 0;
    virtual const char* nombre() const = 0;

    bool vacia() const { return tamanio() == 0; }   // no virtual: se define
                                                    // una sola vez, en
                                                    // terminos de la interfaz

    // Contador de operaciones elementales, para poder comparar costes.
    virtual long pasos() const = 0;
    virtual void reiniciarContador() = 0;
};

// =========================================================================
// IMPLEMENTACION A: cadena de nodos, apilando por el PRINCIPIO
// =========================================================================
//
// La eleccion del extremo no es caprichosa: en una lista simplemente
// enlazada, insertar y borrar por el principio son O(1) sin necesidad de
// ningun puntero auxiliar. Apilar por el final seria O(n).

class PilaEnlazada : public PilaDeEnteros {
public:
    PilaEnlazada() : cima_(nullptr), n_(0), pasos_(0) {}
    PilaEnlazada(const PilaEnlazada&) = delete;             // regla de los tres:
    PilaEnlazada& operator=(const PilaEnlazada&) = delete;  // aqui las prohibimos
    ~PilaEnlazada() override { while (desapilar()) {} }

    void apilar(int valor) override {                       // O(1)
        cima_ = new Nodo{valor, cima_};
        n_++;
        pasos_++;
    }

    bool desapilar() override {                             // O(1)
        if (cima_ == nullptr) return false;
        Nodo* aBorrar = cima_;
        cima_ = cima_->siguiente;
        delete aBorrar;
        n_--;
        pasos_++;
        return true;
    }

    int cima() const override {                             // O(1)
        if (cima_ == nullptr) throw std::out_of_range("cima() sobre pila vacia");
        return cima_->valor;
    }

    int tamanio() const override { return n_; }             // O(1)
    const char* nombre() const override { return "PilaEnlazada"; }

    long pasos() const override { return pasos_; }
    void reiniciarContador() override { pasos_ = 0; }

private:
    struct Nodo {
        int   valor;
        Nodo* siguiente;
    };
    Nodo* cima_;
    int   n_;
    long  pasos_;

    // --- Invariante de la representacion ---------------------------------
    //  n_ >= 0
    //  n_ == 0  <=>  cima_ == nullptr
    //  n_ es exactamente el numero de nodos alcanzables desde cima_
    // ---------------------------------------------------------------------
};

// =========================================================================
// IMPLEMENTACION B: array dinamico, apilando por el FINAL
// =========================================================================
//
// Aqui pasa lo contrario: en un array, el extremo barato es el FINAL, porque
// insertar al principio obligaria a desplazar los n elementos.
//
// La misma palabra ("cima") acaba en sitios opuestos segun la
// representacion. Eso es exactamente lo que el TAD oculta.

class PilaArray : public PilaDeEnteros {
public:
    PilaArray() : datos_(nullptr), n_(0), capacidad_(0), pasos_(0) {}
    PilaArray(const PilaArray&) = delete;
    PilaArray& operator=(const PilaArray&) = delete;
    ~PilaArray() override { delete[] datos_; }

    void apilar(int valor) override {                       // O(1) amortizado
        if (n_ == capacidad_) redimensionar(capacidad_ == 0 ? 1 : capacidad_ * 2);
        datos_[n_++] = valor;
        pasos_++;
    }

    bool desapilar() override {                             // O(1)
        if (n_ == 0) return false;
        n_--;
        pasos_++;
        return true;
    }

    int cima() const override {                             // O(1)
        if (n_ == 0) throw std::out_of_range("cima() sobre pila vacia");
        return datos_[n_ - 1];
    }

    int tamanio() const override { return n_; }
    const char* nombre() const override { return "PilaArray"; }

    long pasos() const override { return pasos_; }
    void reiniciarContador() override { pasos_ = 0; }

    int capacidad() const { return capacidad_; }

private:
    int* datos_;
    int  n_;
    int  capacidad_;
    long pasos_;

    void redimensionar(int nuevaCapacidad) {                // O(n)
        int* nuevo = new int[nuevaCapacidad];
        for (int i = 0; i < n_; i++) { nuevo[i] = datos_[i]; pasos_++; }
        delete[] datos_;
        datos_ = nuevo;
        capacidad_ = nuevaCapacidad;
    }

    // --- Invariante de la representacion ---------------------------------
    //  0 <= n_ <= capacidad_
    //  capacidad_ == 0  <=>  datos_ == nullptr
    //  los elementos validos son datos_[0 .. n_-1]; la cima es datos_[n_-1]
    // ---------------------------------------------------------------------
};

// =========================================================================
// CODIGO CLIENTE: escrito una sola vez, contra la interfaz
// =========================================================================

// Invertir una secuencia es "el" ejemplo canonico de pila: no hace falta
// conocer n de antemano, ni recorrer nada dos veces.
void invertir(PilaDeEnteros& p, const int* v, int n) {
    for (int i = 0; i < n; i++) p.apilar(v[i]);
    std::cout << "  " << std::left << std::setw(14) << p.nombre() << " -> [ ";
    while (!p.vacia()) {
        std::cout << p.cima() << ' ';
        p.desapilar();
    }
    std::cout << "]\n";
}

int main() {
    const int v[] = {1, 4, 9, 16, 25, 36};
    const int n = 6;

    std::cout << "--- El mismo cliente, dos implementaciones ---\n";
    PilaEnlazada a;
    PilaArray    b;
    invertir(a, v, n);
    invertir(b, v, n);

    std::cout << "\n--- A traves de punteros a la interfaz ---\n";
    PilaDeEnteros* pilas[2] = {&a, &b};
    for (PilaDeEnteros* p : pilas) {
        p->reiniciarContador();
        for (int i = 0; i < 1000; i++) p->apilar(i);
        std::cout << "  " << std::left << std::setw(14) << p->nombre()
                  << " 1000 apilar -> cima = " << p->cima()
                  << ", pasos = " << p->pasos() << '\n';
    }
    std::cout << "  (la de array hace mas pasos: los que gasta al copiar\n"
              << "   al redimensionar. Capacidad final = " << b.capacidad() << ")\n";

    std::cout << "\n--- Violando la precondicion de cima() ---\n";
    PilaEnlazada vacia;
    try {
        vacia.cima();
    } catch (const std::out_of_range& e) {
        std::cout << "  Excepcion capturada: " << e.what() << '\n';
    }
    std::cout << "  desapilar() sobre pila vacia devuelve: "
              << std::boolalpha << vacia.desapilar() << " (no lanza)\n";
}
