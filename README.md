# Tema 3 (i): Pila, Cola y Deque

## Cómo usar esta guía

Igual que en las clases anteriores: cada bloque introduce una idea y termina en un ejercicio con código ejecutable. Lee primero el planteamiento, intenta anticipar el resultado —sobre todo el **coste**— y solo después despliega el código y ejecútalo para comparar tu razonamiento con lo que mide la máquina.

La clase 2 iba sobre **elegir una representación**: dos formas de guardar una secuencia, y el perfil de costes que sale de cada una. Esta clase va sobre algo distinto y, en la práctica, más importante: **elegir qué operaciones NO ofrecer**.

En este bloque vemos tres TADs: **Pila**, **Cola** y **Deque**. No son estructuras nuevas. Son la Lista del tema anterior **con operaciones quitadas**. Y esa mutilación deliberada es exactamente lo que los hace valiosos:

- Todo lo que queda cuesta **O(1)**.
- El código que los usa se vuelve mucho más fácil de razonar, porque hay muchas menos cosas que pueden pasar.
- El compilador pasa a defender el contrato: lo que el TAD prohíbe deja de poder escribirse.

Queda un cuarto TAD, la **Cola de Prioridad**, que se estudia más adelante junto con los árboles (el carpeta `4_con_arboles`), porque su implementación —el montículo binario— es un tipo de árbol. Las aplicaciones prácticas de la Pila y de la Cola (equilibrado de paréntesis, evaluación de expresiones, BFS/DFS…) están en la carpeta `3_practicas`.

Algunos de los ficheros de aquí tienen partes marcadas con `// TODO: implementar`: son el ejercicio. Complétalas tú; las soluciones están en la carpeta `2_profesor` por si te atascas o quieres comparar.

Para compilar y ejecutar cualquier fichero:

```bash
g++ -std=c++17 -Wall -o 01_pila 01_pila_tad.cpp && ./01_pila
```

En el fichero que mide tiempos (`05`) conviene compilar con optimización, que es como se compila el código de verdad:

```bash
g++ -std=c++17 -O2 -Wall -o 05_stl 05_adaptadores_stl.cpp && ./05_stl
```

**Ficheros de esta carpeta:**

| Fichero                        | Contenido                                                                 | ¿Hay que completarlo? |
| ------------------------------- | -------------------------------------------------------------------------- | ----------------------- |
| `01_pila_tad.cpp`            | El TAD Pila: un contrato, dos implementaciones intercambiables            | No, está completo       |
| `02_cola_enlazada.cpp`       | El TAD Cola sobre lista enlazada, y sus dos casos límite                 | **Sí**            |
| `03_cola_circular.cpp`       | El buffer circular: la Cola sobre un array, todo en O(1)                  | **Sí**            |
| `04_deque_circular.cpp`      | El TAD Deque, y la trampa del módulo con números negativos              | No, está completo       |
| `05_adaptadores_stl.cpp`     | `std::stack` y `std::queue`: el TAD hecho tipo del lenguaje            | No, está completo       |

**Lo que damos por sabido de las clases anteriores:** punteros y memoria dinámica, clases con miembros privados, la regla de los tres, plantillas, la noción de invariante de la representación, la notación O(·) y el concepto de **coste amortizado**.

---

## 1. La idea del tema: TADs por restricción

En la clase 2, el TAD Lista ofrecía de todo: insertar por cualquier posición, borrar por cualquier posición, acceder por índice, buscar. El problema de ofrecer de todo es que **algunas de esas operaciones son caras en cualquier representación imaginable**, y el cliente no tiene forma de saber cuál es cuál mirando la interfaz.

Los TADs de hoy dan la vuelta al planteamiento. En vez de preguntarse «¿qué se puede hacer con una secuencia?», se preguntan «**¿qué es lo mínimo que necesita este problema?**». Y resulta que una cantidad enorme de problemas reales necesitan muy poco:

| TAD                         | Restricción                                    | Coste de todo lo que ofrece |
| --------------------------- | ------------------------------------------------- | ---------------------------- |
| **Pila**              | Solo se toca un extremo                           | O(1)                        |
| **Cola**              | Se entra por un extremo y se sale por el otro     | O(1)                        |
| **Deque**             | Se entra y se sale por los dos extremos           | O(1)                        |

(Falta la Cola de Prioridad, cuyo orden de salida lo decide el valor y no el orden de llegada, y por eso necesita O(log n). La vemos con los árboles.)

Fíjate en que los tres TADs de esta carpeta no dicen nada sobre acceder al elemento de en medio. No es que sea caro: **es que no existe**. Esa es la diferencia entre una operación lenta y una operación ausente.

> **La regla del tema:** cuando escojas una estructura, no busques la que más cosas te deja hacer. Busca **la más restrictiva que todavía resuelva tu problema**. Lo que ganas no es solo velocidad: es que el compilador te impide escribir el error.

---

## 2. El TAD Pila

Una **Pila** (*stack*) es una secuencia en la que solo se puede tocar un extremo, llamado **cima**. El último que entra es el primero que sale: **LIFO**, *Last In, First Out*.

### La especificación

| Operación      | Precondición | Postcondición                                          |
| --------------- | ------------- | -------------------------------------------------------- |
| `apilar(v)`   | —            | `v` pasa a ser la cima. El resto no cambia.            |
| `desapilar()` | —            | Si no estaba vacía, elimina la cima y devuelve`true`. |
| `cima()`      | `!vacia()`  | Devuelve el elemento de la cima. La pila no cambia.      |
| `vacia()`     | —            | Devuelve si no hay elementos.                            |
| `tamanio()`   | —            | Devuelve el número de elementos.                        |

El reparto de precondiciones es el mismo criterio que fijamos en la clase 2, y conviene releerlo porque aquí se ve mejor:

- `cima()` **devuelve un valor**, y sobre una pila vacía no hay ningún valor que devolver. No hay respuesta correcta posible, así que lleva precondición, se comprueba, y violarla lanza `std::out_of_range`.
- `desapilar()` **solo modifica**. Que la pila esté vacía es un estado perfectamente legítimo en tiempo de ejecución (vaciar una pila con un `while` es lo normal), así que no lleva precondición y devuelve `bool`.

Merece la pena mirar cómo resuelve esto la biblioteca estándar, porque hace algo distinto y deliberado: `std::stack::top()` sobre una pila vacía es **comportamiento indefinido**, no una excepción. La STL elige no pagar la comprobación y deja la responsabilidad al cliente. Nosotros comprobamos siempre porque esto es material didáctico y una excepción enseña mucho más que un fallo silencioso.

### Las dos implementaciones, y por qué la cima cambia de sitio

Lo interesante de la Pila es que las dos representaciones del tema anterior sirven, pero **apilan por extremos opuestos**:

| Representación | La cima es…                  | Por qué                                                            |
| --------------- | ----------------------------- | ------------------------------------------------------------------- |
| Lista enlazada  | el**primer** nodo       | Insertar y borrar por el principio es O(1) sin punteros auxiliares. |
| Array dinámico | la**última** posición | Insertar al principio obligaría a desplazar los n elementos.       |

La misma palabra acaba en sitios físicamente opuestos. **Eso es justo lo que el TAD oculta**, y es la mejor ilustración de por qué merece la pena ocultarlo: el código cliente que invierte una secuencia funciona igual con las dos, sin enterarse de nada.

### Ejercicio 1 — Un contrato, dos implementaciones

**Antes de mirar el código:** las dos implementaciones ofrecen `apilar` en O(1). Una de las dos, sin embargo, hace **más del doble** de operaciones elementales al apilar mil elementos. ¿Cuál, y de dónde salen esas operaciones de más? ¿Contradice eso que sea O(1)?

<details>
<summary>Mostrar código (fragmento; completo en <code>01_pila_tad.cpp</code>)</summary>

```cpp
class PilaDeEnteros {                       // la interfaz: solo QUÉ se puede hacer
public:
    virtual ~PilaDeEnteros() = default;     // destructor virtual: obligatorio
    virtual void apilar(int valor) = 0;
    virtual bool desapilar() = 0;
    virtual int  cima() const = 0;
    virtual int  tamanio() const = 0;

    bool vacia() const { return tamanio() == 0; }   // no virtual: se escribe
};                                                  // una vez, para todas

// A: cadena de nodos. La cima es el PRIMER nodo.
class PilaEnlazada : public PilaDeEnteros {
    struct Nodo { int valor; Nodo* siguiente; };
    Nodo* cima_;
    void apilar(int valor) override { cima_ = new Nodo{valor, cima_}; n_++; }
};

// B: array dinámico. La cima es la ÚLTIMA posición.
class PilaArray : public PilaDeEnteros {
    int* datos_; int n_; int capacidad_;
    void apilar(int valor) override {
        if (n_ == capacidad_) redimensionar(capacidad_ == 0 ? 1 : capacidad_ * 2);
        datos_[n_++] = valor;
    }
};
```

</details>

<details>
<summary>Mostrar resultado</summary>

```text
--- El mismo cliente, dos implementaciones ---
  PilaEnlazada   -> [ 36 25 16 9 4 1 ]
  PilaArray      -> [ 36 25 16 9 4 1 ]

--- A traves de punteros a la interfaz ---
  PilaEnlazada   1000 apilar -> cima = 999, pasos = 1000
  PilaArray      1000 apilar -> cima = 999, pasos = 2016
  (la de array hace mas pasos: los que gasta al copiar
   al redimensionar. Capacidad final = 1024)

--- Violando la precondicion de cima() ---
  Excepcion capturada: cima() sobre pila vacia
  desapilar() sobre pila vacia devuelve: false (no lanza)
```

</details>

**La respuesta:** los 1016 pasos de más son las copias de los redimensionamientos (1 + 2 + 4 + … + 512 = 1023, de los cuales 1016 se reparten como se ve). No contradice el O(1): es **O(1) amortizado**, exactamente lo que demostramos en la clase 2. El total sigue siendo lineal, y el coste medio por operación, constante.

**Lo que hay que llevarse:** las dos cumplen el contrato y dan el mismo resultado. No cuestan lo mismo. Y en la práctica la de array gana casi siempre, aunque haga más «pasos», porque esos pasos son copias de memoria contigua y los de la enlazada son un `new` por elemento. Volveremos a esto con números en el fichero 05.

### Por qué la Pila aparece en todas partes

Una pila es la respuesta siempre que aparece esta frase:

> «Tengo que aparcar algo **ahora** para atenderlo **luego**, y lo último que aparqué es lo primero que hay que atender.»

Dicho de otro modo: **siempre que algo anida**. Paréntesis dentro de paréntesis, llamadas dentro de llamadas, operadores dentro de operadores, etiquetas HTML dentro de etiquetas HTML. El anidamiento *es* la estructura LIFO.

El ejemplo que no se suele contar es el más importante: **la pila de llamadas de tu programa es, literalmente, una pila**. Cada llamada apila un registro de activación con los parámetros, las variables locales y la dirección de retorno; cada `return` lo desapila. Cuando una recursión infinita provoca un *stack overflow*, lo que ha pasado es que un TAD Pila se ha quedado sin memoria. Y de ahí sale un corolario práctico: **toda recursión se puede convertir en un bucle con una pila explícita**, porque es lo que el procesador estaba haciendo por ti.

Las tres aplicaciones clásicas de esta idea —equilibrado de delimitadores, evaluación en notación postfija y el algoritmo *shunting-yard* de Dijkstra— se trabajan en la carpeta `3_practicas` (fichero `01_pila_aplicaciones.cpp`).

---

## 3. El TAD Cola

Una **Cola** (*queue*) toca los dos extremos, pero cada uno con una sola operación: se **encola** por el final y se **desencola** por el principio. El primero en entrar es el primero en salir: **FIFO**, *First In, First Out*.

| Operación       | Precondición | Postcondición                                             |
| ---------------- | ------------- | ---------------------------------------------------------- |
| `encolar(v)`   | —            | `v` pasa a ser el último.                               |
| `desencolar()` | —            | Si no estaba vacía, elimina el primero y devuelve`true`. |
| `frente()`     | `!vacia()`  | Devuelve el primero. La cola no cambia.                    |
| `tamanio()`    | —            | Devuelve el número de elementos.                          |

### La asimetría que no se puede elegir

Aquí se cobra el trabajo del tema anterior. Si implementamos la cola sobre una lista simplemente enlazada, hay dos formas aparentes de repartir los extremos, y **solo una funciona**:

| Reparto                                     | Encolar | Desencolar     | ¿Por qué?                                                |
| ------------------------------------------- | ------- | -------------- | ---------------------------------------------------------- |
| Entrar por el final, salir por el principio | O(1)    | O(1)           | ✔ Con`ultimo_`, las dos son asignaciones de punteros.   |
| Entrar por el principio, salir por el final | O(1)    | **O(n)** | ✘ Para borrar el último hace falta el**anterior**. |

La causa es la misma que nos llevó a la lista doblemente enlazada: **desde un nodo no se puede retroceder**. Con un solo enlace por nodo, de las dos asignaciones posibles solo una es barata, y eso decide el diseño entero. No es una preferencia: es una consecuencia.

Y aquí se ve por qué en la clase 2 nos molestamos en añadir `ultimo_` a la lista enlazada. Sin ese puntero, `encolar` sería O(n) y la estructura no valdría absolutamente para nada.

### Los dos casos límite que rompen todas las colas mal escritas

Son siempre los mismos dos, y merece la pena memorizarlos:

1. **Encolar sobre una cola vacía.** Hay que fijar *también* `primero_`, no solo `ultimo_`.
2. **Desencolar dejándola vacía.** Hay que anular *también* `ultimo_`, no solo avanzar `primero_`.

El segundo es el peligroso. Si se olvida, `ultimo_` queda apuntando a memoria ya liberada, y el programa **no falla ahí**: falla en el siguiente `encolar`, que escribe a través de un puntero colgante. Un fallo que se manifiesta lejos de su causa es el más caro de depurar que existe, y por eso el fichero 02 incluye una función `invarianteCorrecto()` que lo detecta en el acto.

### Ejercicio 2 — La cola enlazada y su invariante

**Tu tarea:** en `02_cola_enlazada.cpp`, `encolar` y `desencolar` están marcados con `// TODO: implementar`. Complétalos teniendo en cuenta los dos casos límite de arriba.

**Antes de escribir código:** ¿qué debe cumplir el invariante?

El invariante, escrito como código ejecutable (esto ya está en el fichero, y te sirve de ayuda: cuando `encolar`/`desencolar` estén bien, `invarianteCorrecto()` debe devolver siempre `true`):

```cpp
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
```

<details>
<summary>Mostrar resultado esperado (con la solución correcta)</summary>

```text
--- Encolando 1..6 ---
  [frente] 1 2 3 4 5 6 [final]  (n=6)
  invariante correcto: true

--- Copia (regla de los tres) ---
  original:   [frente] 5 6 [final]  (n=2)
  copia:      [frente] 4 5 6 [final]  (n=3)
  ...y la copia no se entera: son colas independientes.

--- El caso limite: vaciar y volver a llenar ---
  vacia: true, invariante: true
  [frente] 99 [final]  (n=1)
  invariante tras revivir: true
```

</details>

**Si te atascas:** la solución completa está en `2_profesor/02_cola_enlazada_resuelta.cpp`.

**La pregunta que debes poder responder tras el ejercicio:** ¿qué pasaría si al desencolar el último elemento no anularas `ultimo_`? Sería **algo peor** que un error. `ultimo_->siguiente = nuevo` escribiría cuatro u ocho bytes en un bloque de memoria que ya se ha liberado. Es comportamiento indefinido: lo más probable es que no pase nada visible en ese momento, que el programa siga funcionando, y que reviente mucho más tarde en un sitio sin ninguna relación aparente. Un `if` de una línea es lo que separa un programa correcto de uno que falla los martes.

---

## 4. La Cola sobre un array: el buffer circular

Es la idea más bonita del tema, y la implementación que de verdad se usa.

**El problema.** Si guardamos la cola en `datos_[0..n-1]` y desencolamos por el principio, hay que desplazar los n−1 elementos restantes. Desencolar sería O(n) y la cola no serviría.

**La solución ingenua: no desplazar.** Llevamos un índice `frente_` que simplemente avanza. Desencolar vuelve a ser O(1)… pero la cola **repta** hacia la derecha y agota el array aunque le sobren huecos por la izquierda:

```text
capacidad 8, tras encolar 5 y desencolar 3:

    [ .  .  .  D  E  ?  ?  ? ]     n = 2, y solo caben 3 más
               ^frente_
```

**La solución buena: que el array sea un círculo.** Cuando un índice llega al final, vuelve al 0. Eso es todo lo que hace el `% capacidad_`:

```text
    [ E  ?  ?  .  .  .  B  C  D ]   la cola ocupa 6,7,8,0 — y da igual
      ^final        ^frente_
```

Todas las operaciones quedan en O(1) y **no se desplaza nada nunca**.

### La decisión de diseño: `n_` o un índice `final_`

Aquí hay un detalle que casi siempre se explica mal. Con `frente_` y `final_` **solamente**, la cola vacía y la cola llena dan exactamente la misma configuración (`frente_ == final_`), y no hay forma de distinguirlas. Las tres salidas clásicas:

| Estrategia                     | Coste                                                       |
| ------------------------------ | ----------------------------------------------------------- |
| Guardar`n_` (lo que hacemos) | Un entero más, y hay que mantenerlo coherente.             |
| Dejar siempre una celda libre  | Se desperdicia una celda: la capacidad útil es`cap - 1`. |
| Guardar un`bool llena`       | Un caso especial más que recordar en cada operación.      |

Es otra vez el compromiso de la clase 2: **información redundante a cambio de mantenerla**. Nosotros guardamos `n_` porque es la que menos casos especiales genera, y deducimos la posición libre con `(frente_ + n_) % capacidad_`.

### Ejercicio 3 — Ver el array físico, no la cola lógica

**Tu tarea:** en `03_cola_circular.cpp`, `encolar` y `desencolar` están marcados con `// TODO: implementar`. Complétalos.

**Antes de escribir código:** una cola circular de capacidad 4 contiene los elementos `[5, 6, 3, 4]` con `frente_ = 2`. Está llena y **partida** en dos tramos. Encolas un 7, lo que obliga a duplicar la capacidad. ¿Puedes copiar `datos_` tal cual al array nuevo? ¿Qué le pasa a `frente_`?

Dos funciones auxiliares que ya están en el fichero, y que necesitarás:

```cpp
int siguiente(int i) const { return (i + 1) % capacidad_; }
int posicionLibre() const  { return (frente_ + n_) % capacidad_; }
```

<details>
<summary>Mostrar resultado esperado (con la solución correcta)</summary>

```text
=== El array como circulo ===
  5 encolados:      [  10  20  30  40  50   .   .   . ]  frente_=0 n_=5 cap=8
  3 desencolados:   [   .   .   .  40  50   .   .   . ]  frente_=3 n_=2 cap=8
    (nadie se ha movido: solo ha avanzado frente_)
  5 mas:            [  90 100   .  40  50  60  70  80 ]  frente_=3 n_=7 cap=8
    (han dado la vuelta y ocupan el hueco del principio)

=== Cuando se llena: desenrollar ===
  llena y partida:  [   5   6   3   4 ]  frente_=2 n_=4 cap=4
  tras encolar 7:   [   3   4   5   6   7   .   .   . ]  frente_=0 n_=5 cap=8
    al crecer, el circulo se desenrolla y frente_ vuelve a 0

=== El contrato se sigue cumpliendo ===
  orden de salida: 3 4 5 6 7
  (FIFO exacto, con toda la aritmetica modular por debajo)
```

</details>

**Si te atascas:** la solución completa está en `2_profesor/03_cola_circular_resuelta.cpp`.

**La respuesta a la pregunta de arriba:** **no** se puede copiar tal cual. En memoria, el contenido es `[5 6 3 4]`: una copia literal dejaría la cola en el orden equivocado. Hay que recorrer **lógicamente** (`datos_[(frente_ + k) % capacidad_]` para k de 0 a n−1) y escribir desde la posición 0 del array nuevo. Y entonces `frente_` **debe volver a 0**, porque el círculo se ha desenrollado. Olvidar esa línea es el error clásico de esta estructura: la cola sigue funcionando un rato y luego devuelve los elementos en un orden imposible.

### La variante acotada, que es la que más se usa

Si en vez de redimensionar se **rechaza** el `encolar` cuando está llena, tenemos un buffer de tamaño fijo. Es lo que hay dentro de un driver de red, un buffer de audio o un esquema productor/consumidor. Ahí el límite de memoria **es un requisito, no un defecto**: se prefiere descartar un paquete a que un pico de tráfico agote la RAM del sistema.

---

## 5. El TAD Deque

Un **Deque** (*double-ended queue*) permite insertar y borrar por **los dos extremos**, todo en O(1). Es el TAD que generaliza a los otros dos:

- Usa un solo extremo → tienes una **Pila**.
- Entra por uno, sale por el otro → tienes una **Cola**.

Por eso `std::stack` y `std::queue` usan, por defecto, un `std::deque` por debajo.

La implementación es el mismo buffer circular del fichero 03, con una sola idea nueva y una trampa que hay que ver una vez en la vida.

### La trampa del módulo

Para insertar por delante hay que **retroceder** el índice. Y retroceder con `%` en C++ tiene un problema:

```cpp
(frente_ - 1) % capacidad_        // con frente_ == 0  da  -1
```

Porque **el `%` de C++ no es el módulo matemático**: conserva el signo del dividendo. La corrección es sumar la capacidad antes de tomar el módulo:

```cpp
(frente_ - 1 + capacidad_) % capacidad_     // siempre en [0, capacidad_)
```

Es un error clásico y silencioso: `datos_[-1]` es comportamiento indefinido, escribe justo *antes* del bloque reservado, y normalmente no revienta hasta que ese vecino de memoria se usa para otra cosa.

### Ejercicio 4 — Los cuatro extremos

`04_deque_circular.cpp` está completo: aquí no hay que implementar nada, léelo y ejecútalo para entenderlo bien antes de seguir.

**Antes de mirar el código:** el deque mantiene el acceso por posición `obtener(pos)` en O(1), cosa que la lista enlazada nunca pudo dar. ¿Cómo se calcula la dirección del elemento lógico k? ¿Y por qué eso **no** convierte al deque en una lista (es decir, por qué `insertar(pos, v)` en medio sigue siendo O(n))?

<details>
<summary>Mostrar código (fragmento; completo en <code>04_deque_circular.cpp</code>)</summary>

```cpp
int indice(int k) const    { return (frente_ + k) % capacidad_; }
int siguiente(int i) const { return (i + 1) % capacidad_; }
int anterior(int i) const  { return (i - 1 + capacidad_) % capacidad_; }   // <-- ojo

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

bool borrarUltimo() {                            // O(1)
    if (n_ == 0) return false;
    n_--;                     // el último deja de estar en el rango válido.
    return true;              // No hay nada más que hacer.
}
```

</details>

<details>
<summary>Mostrar resultado</summary>

```text
=== Los dos extremos, los dos en O(1) ===
  3 por el final:         [   3   4   5 ]  (frente_=0 n_=3 cap=8)
  insertarPrincipio(2):   [   2   3   4   5 ]  (frente_=7 n_=4 cap=8)
  insertarPrincipio(1):   [   1   2   3   4   5 ]  (frente_=6 n_=5 cap=8)
    frente_ ha ido HACIA ATRAS y ha dado la vuelta a 0.

=== Usado como PILA (un solo extremo) ===
    salida: 4 3 2 1  -> LIFO
=== Usado como COLA (un extremo para cada cosa) ===
    salida: 1 2 3 4  -> FIFO
```

</details>

**La respuesta:** el elemento lógico k está en `datos_[(frente_ + k) % capacidad_]`: una suma, un módulo y un acceso, todo O(1). Pero **insertar en medio sigue siendo O(n)** porque hay que hacer sitio, y hacer sitio significa desplazar físicamente la mitad de los elementos. El acceso indexado barato y la inserción barata en medio son cosas independientes: el deque te da lo primero, nunca lo segundo. (Para eso está la lista enlazada, que da lo segundo y nunca lo primero.)

### Cómo lo hace `std::deque`, que es distinto

`std::deque` **no** usa un único array circular: usa un vector de punteros a **bloques** de tamaño fijo. Al crecer solo reubica la tabla de punteros, no los elementos. La consecuencia práctica es importante y suele sorprender:

> Insertar en los extremos de un `std::deque` **no invalida las referencias** a los elementos que ya había (aunque sí invalida los iteradores). En un `std::vector`, crecer las invalida todas.

Ese es justo el motivo por el que `std::stack` usa `deque` y no `vector` por defecto, aunque `vector` suela ser más rápido: la STL elige por defecto **lo predecible**, no lo más rápido en promedio.

Si quieres ver *cómo* está hecho ese vector de bloques por dentro, ese es exactamente el material de `2_profesor/11_deque_bloques.cpp`.

---

## 6. Los adaptadores de la STL: el TAD hecho tipo del lenguaje

`std::stack` y `std::queue` **no son estructuras de datos**. Son **adaptadores**: interfaces recortadas sobre otro contenedor, que se pasa como parámetro de plantilla.

```cpp
template <class T, class Contenedor = std::deque<T>>
class stack;
```

Léelo despacio, porque es el tema 2 entero convertido en un mecanismo del lenguaje: **la especificación es lo único que se ve, y la implementación es literalmente un argumento que puedes cambiar sin tocar tu código.**

### Lo que el adaptador quita

Lo más instructivo de un adaptador es lo que **prohíbe**:

```cpp
std::deque<int> d;   d[3];         // compila
std::stack<int> p;   p.top();      // compila
                     p[3];         // NO COMPILA
                     p.begin();    // NO COMPILA
```

Un `std::stack` **no tiene iteradores**: no se puede recorrer. Eso no es una carencia, es el TAD. Sobre el `deque` que hay debajo sí podrías hacer `d[3]`; el adaptador lo hace imposible de escribir. **El compilador pasa a defender el contrato.** Y de ahí una regla práctica: si necesitas recorrer tu pila, lo que necesitabas no era una pila.

### Ejercicio 5 — El mismo TAD, tres representaciones

**Antes de mirar el código:** las tres pilas son O(1) amortizado por operación y cumplen el mismo contrato. ¿Cuánta diferencia de tiempo esperas entre la mejor y la peor? Y la pregunta buena: si `vector` es la más rápida, **¿por qué el defecto de la STL es `deque`?**

<details>
<summary>Mostrar código (fragmento; completo en <code>05_adaptadores_stl.cpp</code>)</summary>

```cpp
std::stack<int, std::deque<int>>  p;   // el que trae por defecto
std::stack<int, std::vector<int>> p;   // suele ser el más rápido
std::stack<int, std::list<int>>   p;   // un new por elemento
```

</details>

<details>
<summary>Mostrar resultado</summary>

```text
=== 1) El contenedor subyacente es un parametro ===
  tipo                              tiempo (ms)
  stack<int, deque<int>>  (def.)    8.6
  stack<int, vector<int>>           6.8
  stack<int, list<int>>             96.5

  ...hay un factor 14.2 entre el mejor y el peor:
  el TAD garantiza el RESULTADO, nunca el COSTE.
```

</details>

**Las respuestas:** un factor **14×** entre `vector` y `list`, las dos O(1) amortizado. Y `deque` es el defecto, a pesar de ser algo más lenta que `vector`, porque **al crecer no copia lo que ya había**: crece por bloques, así que no invalida referencias ni provoca picos de latencia. La STL elige por defecto lo predecible, no lo más rápido en promedio. Es una decisión de ingeniería, y está tomada por ti.

Fíjate por último en la tabla de defectos de este bloque, porque cada uno responde a algo concreto:

| TAD   | En la STL      | Contenedor por defecto | Por qué                                             |
| ----- | -------------- | ------------------------ | ----------------------------------------------------- |
| Pila  | `std::stack` | `std::deque`          | No copia al crecer; latencia predecible.             |
| Cola  | `std::queue` | `std::deque`          | O(1) por los dos extremos, que es justo lo que pide. |
| Deque | `std::deque` | *(es una estructura)*  | Bloques + tabla de punteros.                          |

Falta una fila, la de la Cola de Prioridad y `std::priority_queue`: la veremos junto con los árboles.

---

## 7. Tabla de costes y cómo elegir (de momento)

| Operación           | Pila enlazada | Pila array | Cola enlazada | Cola circular | Deque circular |
| -------------------- | ------------- | ---------- | ------------- | ------------- | -------------- |
| insertar (extremo)   | O(1)          | O(1)*      | O(1)          | O(1)*         | O(1)*          |
| extraer (extremo)    | O(1)          | O(1)       | O(1)          | O(1)          | O(1)           |
| consultar (extremo)  | O(1)          | O(1)       | O(1)          | O(1)          | O(1)           |
| acceso por posición | —            | —         | —            | —            | **O(1)** |
| peor caso de una op. | O(1)          | O(n)       | O(1)          | O(n)          | O(n)           |
| memoria por`int`   | 16 B          | 4–8 B     | 16 B          | 4–8 B        | 4–8 B         |

\* O(1) amortizado: el redimensionamiento puntual cuesta O(n).

La fila que casi nadie mira y casi siempre importa es la del **peor caso de una operación**. Las implementaciones sobre array tienen todas un pico O(n) escondido. Si estás escribiendo un servidor web, da igual. Si estás escribiendo el control de un airbag, es lo único que importa.

### Cómo elegir, en orden

1. **¿Necesito los dos extremos?** → Deque (`std::deque`).
2. **¿Entra por un lado y sale por el otro?** → Cola (`std::queue`).
3. **¿Solo toco un extremo?** → Pila (`std::stack`).
4. **¿Necesito además acceder por posición, o recorrer?** → Entonces no era ninguno de estos: vuelve a la clase 2 y usa `std::vector`.
5. **¿Tengo un límite duro de latencia?** → Evita las implementaciones con pico O(n): usa buffers circulares de capacidad fija y reserva la memoria por adelantado.

(Falta la primera pregunta de la lista completa —«¿el orden de salida lo decide el valor?»—, que lleva a la Cola de Prioridad. Se añade en `4_con_arboles`.)

---

## Resumen

- Los tres TADs de esta carpeta son la Lista **con operaciones quitadas**. La restricción no es una limitación: es lo que permite que todo lo que queda sea O(1), y lo que hace que el compilador defienda el contrato.
- La regla práctica: elige **la estructura más restrictiva que resuelva tu problema**, no la que más cosas te deje hacer.
- La **Pila** es la respuesta siempre que algo **anida**. La pila de llamadas de tu programa es una pila literal, y por eso toda recursión se puede convertir en un bucle con una pila explícita.
- En la **Cola** sobre lista enlazada, el reparto de extremos no se elige: solo una de las dos opciones es O(1), porque desde un nodo no se puede retroceder. Los dos errores clásicos son encolar sobre vacía y **desencolar dejándola vacía** (hay que anular `ultimo_`).
- El **buffer circular** convierte el array en un círculo con `% capacidad_` y deja todas las operaciones en O(1) sin desplazar nada. Al redimensionar hay que **desenrollarlo** y poner `frente_ = 0`.
- El **Deque** generaliza a Pila y Cola, y mantiene el acceso por posición en O(1). Cuidado con `(i - 1 + capacidad_) % capacidad_`: el `%` de C++ no es el módulo matemático.
- Los **adaptadores de la STL** son el TAD convertido en mecanismo del lenguaje: la implementación es un parámetro de plantilla, y la interfaz recortada hace que lo prohibido **no compile**.
- El TAD garantiza el **resultado**, nunca el **coste**. Entre dos pilas O(1) amortizado hay un factor 14. Al final, hay que medir.

---

## Anexo — Cómo están implementados de verdad `std::stack` y `std::queue`

Si abres `<stack>` te vas a llevar una decepción productiva: no hay nada. La clase entera es un contenedor miembro y una docena de funciones que delegan:

```cpp
template <class T, class Container = deque<T>>
class stack {
protected:
    Container c;                        // el contenedor de verdad
public:
    bool empty() const   { return c.empty(); }
    size_t size() const  { return c.size(); }
    reference top()      { return c.back(); }
    void push(const T& x){ c.push_back(x); }
    void pop()           { c.pop_back(); }
};
```

Eso es, esencialmente, todo. Y es justo lo que hay que entender: **el adaptador no aporta estructura, aporta restricción**. Lo único que hace es no exponer `operator[]`, no exponer `begin()` y no exponer `insert()`. Su valor es negativo, en el sentido literal.

Fíjate además en que `stack` usa `back()`/`push_back()`/`pop_back()` y `queue` usa `front()`/`push_back()`/`pop_front()`. De ahí salen los requisitos sobre el contenedor: `std::vector` vale para `stack` pero **no** para `queue`, porque `vector` no tiene `pop_front()` (y no lo tiene porque sería O(n): la STL, otra vez, no ofrece lo que no puede dar barato).

### Ejercicio final — Inspecciona tu propia biblioteca

```cpp
#include <iostream>
#include <stack>
#include <queue>
#include <deque>
#include <vector>

int main() {
    std::cout << "sizeof(std::deque<int>) = " << sizeof(std::deque<int>) << '\n';
    std::cout << "sizeof(std::stack<int>) = " << sizeof(std::stack<int>) << '\n';

    // ¿Se mueven los elementos de un deque al crecer por los extremos?
    std::deque<int> d = {1, 2, 3};
    int* antes = &d[1];
    for (int i = 0; i < 10000; i++) { d.push_back(i); d.push_front(i); }
    std::cout << "la referencia al elemento central sigue valiendo: "
              << (antes == &d[10001] ? "SI" : "NO") << '\n';
}
```

Preguntas a responder con la salida:

1. ¿Cuánto ocupa un `std::stack<int>` comparado con el `std::deque<int>` que lleva dentro? ¿Qué te dice eso sobre lo que añade el adaptador?
2. ¿Sobrevive la referencia al crecer el `deque` por los dos extremos? Repite el experimento con un `std::vector` y `push_back`. ¿Qué cambia?
3. Sustituye el `int` por una clase tuya con contadores de copias y movimientos. Al hacer `push` mil veces sobre un `stack<T, vector<T>>` y sobre un `stack<T, deque<T>>`, ¿cuántas copias hace cada uno? Ahí está, medida, la razón del defecto de la STL.
