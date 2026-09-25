// 03_cola_circular.cpp
// Objetivo: la Cola sobre un ARRAY. Es la implementacion que de verdad se
// usa, y contiene la idea mas bonita del tema: el buffer circular.
//
// El problema. Si guardamos la cola en datos_[0..n-1] y desencolamos por el
// principio, hay que desplazar los n-1 elementos restantes: O(n). Inaceptable.
//
// La solucion ingenua. No desplazar: llevar un indice frente_ que avanza.
// Pero entonces la cola "repta" hacia la derecha y agota el array aunque
// sobren huecos por la izquierda.
//
//      encolar 5, desencolar 3:
//      [ . . . D E ? ? ? ]      <- capacidad 8, n = 2, y solo caben 3 mas
//
// La solucion buena. Que el array se comporte como un CIRCULO: cuando un
// indice llega al final, vuelve al 0. Eso es todo lo que hace el "% cap".
//
//      [ E ? ? . . . B C D ]    <- la cola ocupa 5,6,7,0 ; da igual
//        ^final     ^frente
//
// Todas las operaciones quedan en O(1) sin desplazar nada nunca.
//
// Compilar:
//   g++ -std=c++17 -Wall -o 03_circ 03_cola_circular.cpp && ./03_circ

#include <iostream>
#include <iomanip>
#include <stdexcept>

class ColaCircular {
public:
    explicit ColaCircular(int capacidadInicial = 4)
        : datos_(new int[capacidadInicial]),
          capacidad_(capacidadInicial), frente_(0), n_(0) {}

    ColaCircular(const ColaCircular&) = delete;
    ColaCircular& operator=(const ColaCircular&) = delete;
    ~ColaCircular() { delete[] datos_; }

    void encolar(int valor) {                        // O(1) amortizado
        // TODO: implementar
    }

    bool desencolar() {                              // O(1) SIEMPRE
        // TODO: no se borra ni se desplaza nada: solo avanza el indice
        
    }

    int frente() const {                             // O(1)
        if (n_ == 0) throw std::out_of_range("frente() sobre cola vacia");
        return datos_[frente_];
    }

    int  tamanio() const { return n_; }
    int  capacidad() const { return capacidad_; }
    bool vacia() const { return n_ == 0; }
    bool llena() const { return n_ == capacidad_; }

    // Dibuja el array FISICO, no la cola logica. Es la unica forma de ver
    // que esta pasando de verdad.
    void dibujar(const char* etiqueta) const {
        std::cout << "  " << std::left << std::setw(18) << etiqueta << "[ ";
        for (int i = 0; i < capacidad_; i++) {
            if (ocupada(i)) std::cout << std::right << std::setw(3) << datos_[i] << ' ';
            else            std::cout << "  . ";
        }
        std::cout << "]  frente_=" << frente_ << " n_=" << n_
                  << " cap=" << capacidad_ << '\n';
    }

private:
    int* datos_;
    int  capacidad_;
    int  frente_;       // indice del primero que saldra
    int  n_;            // cuantos hay. NO guardamos un indice "final":
                        // se deduce, y asi no hay dos verdades que mantener

    // --- Invariante de la representacion ---------------------------------
    //  capacidad_ > 0
    //  0 <= frente_ < capacidad_
    //  0 <= n_ <= capacidad_
    //  los elementos son datos_[(frente_ + k) % capacidad_] para k en [0, n_)
    //  el resto del array es basura, y da igual lo que contenga
    // ---------------------------------------------------------------------
    //
    // POR QUE GUARDAR n_ Y NO UN INDICE final_:
    // Con frente_ y final_ solamente, "cola vacia" y "cola llena" dan
    // exactamente la misma configuracion (frente_ == final_) y no hay forma
    // de distinguirlas. Las tres salidas clasicas son:
    //    a) guardar n_                    <- lo que hacemos aqui: lo mas claro
    //    b) dejar siempre una celda libre <- se desperdicia una, y la
    //                                        capacidad util es capacidad_-1
    //    c) guardar un bool "llena"
    // Es otra vez el compromiso del tema anterior: informacion redundante a
    // cambio de mantenerla coherente.

    int siguiente(int i) const { return (i + 1) % capacidad_; }

    int posicionLibre() const { return (frente_ + n_) % capacidad_; }

    bool ocupada(int i) const {
        if (n_ == 0) return false;
        int fin = (frente_ + n_ - 1) % capacidad_;
        if (frente_ <= fin) return i >= frente_ && i <= fin;   // tramo recto
        return i >= frente_ || i <= fin;                       // tramo partido
    }

    // Al redimensionar hay que DESENROLLAR el circulo: el array nuevo
    // empieza siempre en 0. Es la unica operacion O(n) de la clase.
    void redimensionar(int nuevaCapacidad) {
        int* nuevo = new int[nuevaCapacidad];
        for (int k = 0; k < n_; k++) nuevo[k] = datos_[(frente_ + k) % capacidad_];
        delete[] datos_;
        datos_ = nuevo;
        capacidad_ = nuevaCapacidad;
        frente_ = 0;                  // <-- imprescindible
    }
};

int main() {
    std::cout << "=== El array como circulo ===\n";
    ColaCircular c(8);
    for (int i = 1; i <= 5; i++) c.encolar(i * 10);
    c.dibujar("5 encolados:");

    for (int i = 0; i < 3; i++) c.desencolar();
    c.dibujar("3 desencolados:");
    std::cout << "    (nadie se ha movido: solo ha avanzado frente_)\n";

    for (int i = 6; i <= 10; i++) c.encolar(i * 10);
    c.dibujar("5 mas:");
    std::cout << "    (han dado la vuelta y ocupan el hueco del principio)\n";

    std::cout << "\n=== Cuando se llena: desenrollar ===\n";
    ColaCircular d(4);
    for (int i = 1; i <= 4; i++) d.encolar(i);
    d.desencolar(); d.desencolar();
    d.encolar(5); d.encolar(6);
    d.dibujar("llena y partida:");
    std::cout << "    llena = " << std::boolalpha << d.llena() << '\n';//bool
    d.encolar(7);
    d.dibujar("tras encolar 7:");
    std::cout << "    al crecer, el circulo se desenrolla y frente_ vuelve a 0\n";

    std::cout << "\n=== El contrato se sigue cumpliendo ===\n";
    std::cout << "  orden de salida: ";
    while (!d.vacia()) { std::cout << d.frente() << ' '; d.desencolar(); }
    std::cout << "\n  (FIFO exacto, con toda la aritmetica modular por debajo)\n";

    std::cout << "\n=== Cola ACOTADA: la variante que no crece ===\n";
    std::cout << "  Si en vez de redimensionar se rechaza el encolar, tenemos\n"
              << "  un buffer de tamanio fijo: lo que usan un driver de red, un\n"
              << "  buffer de audio o un productor/consumidor. Ahi el limite de\n"
              << "  memoria es un REQUISITO, no un defecto.\n";
}
