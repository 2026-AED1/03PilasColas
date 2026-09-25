// 04_deque_circular.cpp
// Objetivo: el TAD Deque (double-ended queue, "cola doblemente terminada"):
// insertar y borrar por LOS DOS extremos, todo en O(1).
//
// El Deque es el TAD que generaliza a los otros dos:
//    usa solo un extremo          -> tienes una Pila
//    encola por uno, saca del otro -> tienes una Cola
// Por eso std::stack y std::queue usan, por defecto, un std::deque por debajo.
//
// La implementacion es el mismo buffer circular del fichero 03, con una sola
// idea nueva: para insertar por delante hay que RETROCEDER el indice, y
// retroceder con modulo tiene una trampa.
//
//        (frente_ - 1) % capacidad_     con frente_ == 0  da  -1  en C++
//
// porque el % de C++ NO es el modulo matematico: conserva el signo del
// dividendo. La correccion es sumar la capacidad antes:
//
//        (frente_ - 1 + capacidad_) % capacidad_     -> siempre en [0, cap)
//
// Es un error clasico y silencioso: -1 como indice de array es comportamiento
// indefinido, y no suele fallar hasta mucho despues.
//
// Compilar:
//   g++ -std=c++17 -Wall -o 04_deque 04_deque_circular.cpp && ./04_deque

#include <iostream>
#include <iomanip>
#include <stdexcept>

class Deque {
public:
    explicit Deque(int capacidadInicial = 4)
        : datos_(new int[capacidadInicial]),
          capacidad_(capacidadInicial), frente_(0), n_(0) {}

    Deque(const Deque&) = delete;
    Deque& operator=(const Deque&) = delete;
    ~Deque() { delete[] datos_; }

    // --- Los cuatro extremos -------------------------------------------
    void insertarPrincipio(int valor) {              // O(1) amortizado
        if (n_ == capacidad_) redimensionar(capacidad_ * 2);
        frente_ = anterior(frente_);                 // retroceder: la trampa
        datos_[frente_] = valor;
        n_++;
    }

    void insertarFinal(int valor) {                  // O(1) amortizado
        if (n_ == capacidad_) redimensionar(capacidad_ * 2);
        datos_[indice(n_)] = valor;
        n_++;
    }

    bool borrarPrincipio() {                         // O(1)
        if (n_ == 0) return false;
        frente_ = siguiente(frente_);
        n_--;
        return true;
    }

    bool borrarUltimo() {                            // O(1)
        if (n_ == 0) return false;
        n_--;                     // el ultimo deja de estar en el rango
        return true;              // valido. No hay nada mas que hacer.
    }

    int primero() const {
        if (n_ == 0) throw std::out_of_range("primero() sobre deque vacio");
        return datos_[frente_];
    }

    int ultimo() const {
        if (n_ == 0) throw std::out_of_range("ultimo() sobre deque vacio");
        return datos_[indice(n_ - 1)];
    }

    // Bonus que la cola no daba: acceso por posicion en O(1). El deque
    // conserva el acceso indexado del array, cosa que la lista enlazada no.
    int obtener(int pos) const {                     // O(1)
        if (pos < 0 || pos >= n_) throw std::out_of_range("posicion invalida");
        return datos_[indice(pos)];
    }

    int  tamanio() const { return n_; }
    int  capacidad() const { return capacidad_; }
    bool vacio() const { return n_ == 0; }

    void imprimir(const char* etiqueta) const {
        std::cout << "  " << std::left << std::setw(24) << etiqueta << "[ ";
        for (int k = 0; k < n_; k++) std::cout << std::right << std::setw(3) << obtener(k) << ' ';
        std::cout << "]  (frente_=" << frente_ << " n_=" << n_
                  << " cap=" << capacidad_ << ")\n";
    }

private:
    int* datos_;
    int  capacidad_;
    int  frente_;
    int  n_;

    // --- Invariante de la representacion ---------------------------------
    //  capacidad_ > 0 ;  0 <= frente_ < capacidad_ ;  0 <= n_ <= capacidad_
    //  el elemento logico k (0 <= k < n_) esta en
    //        datos_[(frente_ + k) % capacidad_]
    // ---------------------------------------------------------------------

    int indice(int k) const { return (frente_ + k) % capacidad_; }
    int siguiente(int i) const { return (i + 1) % capacidad_; }
    int anterior(int i) const { return (i - 1 + capacidad_) % capacidad_; }   // <-- ojo

    void redimensionar(int nuevaCapacidad) {         // O(n), solo al crecer
        int* nuevo = new int[nuevaCapacidad];
        for (int k = 0; k < n_; k++) nuevo[k] = datos_[indice(k)];
        delete[] datos_;
        datos_ = nuevo;
        capacidad_ = nuevaCapacidad;
        frente_ = 0;
    }
};

int main() {
    std::cout << "=== Los dos extremos, los dos en O(1) ===\n";
    Deque d(8);
    d.insertarFinal(3); d.insertarFinal(4); d.insertarFinal(5);
    d.imprimir("3 por el final:");
    d.insertarPrincipio(2);
    d.imprimir("insertarPrincipio(2):");
    d.insertarPrincipio(1);
    d.imprimir("insertarPrincipio(1):");
    std::cout << "    frente_ ha ido HACIA ATRAS y ha dado la vuelta a 0.\n"
              << "    Sin el '+ capacidad_' habria salido -1 y el programa\n"
              << "    escribiria fuera del array.\n";

    std::cout << "\n=== Usado como PILA (un solo extremo) ===\n";
    Deque p;
    for (int i = 1; i <= 4; i++) p.insertarFinal(i);
    std::cout << "    salida: ";
    while (!p.vacio()) { std::cout << p.ultimo() << ' '; p.borrarUltimo(); }
    std::cout << " -> LIFO\n";

    std::cout << "\n=== Usado como COLA (un extremo para cada cosa) ===\n";
    Deque q;
    for (int i = 1; i <= 4; i++) q.insertarFinal(i);
    std::cout << "    salida: ";
    while (!q.vacio()) { std::cout << q.primero() << ' '; q.borrarPrincipio(); }
    std::cout << " -> FIFO\n";

    std::cout << "\n=== Y ademas conserva el acceso por posicion ===\n";
    Deque r(4);
    for (int i = 0; i < 6; i++) r.insertarPrincipio(i);     // fuerza a crecer
    r.imprimir("6 por delante:");
    std::cout << "    obtener(0)=" << r.obtener(0)
              << "  obtener(3)=" << r.obtener(3)
              << "  obtener(5)=" << r.obtener(5) << "   (los tres O(1))\n";

    std::cout << "\n=== Como lo hace std::deque, que es distinto ===\n";
    std::cout << "  std::deque NO usa un unico array circular: usa un vector de\n"
              << "  punteros a BLOQUES de tamanio fijo. Al crecer solo reubica la\n"
              << "  tabla de punteros, no los elementos. Consecuencia practica:\n"
              << "  insertar en los extremos de un std::deque NO invalida las\n"
              << "  referencias a los elementos que ya habia (aunque si invalida\n"
              << "  los iteradores). En un std::vector, crecer las invalida todas.\n";
}
