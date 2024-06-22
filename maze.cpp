#include <iostream>   //Biblioteca para entrada y salida de datos
#include <vector>     //Biblioteca para utilizar el contenedor vector
#include <stack>      //Biblioteca para utilizar el contenedor stack
#include <queue>      //Biblioteca para utilizar el contenedor queue
#include <algorithm>  //Biblioteca para utilizar algoritmos estándar como shuffle
#include <random>     //Biblioteca para generación de números aleatorios
#include <chrono>     //Biblioteca para manipulación del tiempo
#include <thread>     //Biblioteca para usar hilos y dormir el programa

using namespace std;  //Permite el uso de nombres de la biblioteca estándar sin el prefijo std::
using namespace chrono; //Permite el uso de nombres de la biblioteca chrono sin el prefijo chrono::

const int WALL = 1;     //Constante para representar una pared en el laberinto
const int PATH = 0;     //Constante para representar un camino en el laberinto
const int SOLUTION = 2; //Constante para representar la solución en el laberinto

//Estructura que representa una celda del laberinto con coordenadas x e y
struct Cell {
    int x, y;
};

//Función para obtener el tamaño del laberinto del usuario
void getUserInput(int& width, int& height) {
    cout << "Ingrese el número de filas (debe ser impar): "; //Pide el número de filas
    cin >> height; 
    cout << "Ingrese el número de columnas (debe ser impar): "; //Pide el número de columnas
    cin >> width; 
    //Verificar que los tamaños sean impares y ajustar si es necesario
    if (height % 2 == 0) { //Si el número de filas es par
        height++; //Incrementa en 1 para hacerlo impar
        cout << "El número de filas se incrementó a " << height << " para ser impar." << endl; //Informa al usuario
    }
    if (width % 2 == 0) { //Si el número de columnas es par
        width++; //Incrementa en 1 para hacerlo impar
        cout << "El número de columnas se incrementó a " << width << " para ser impar." << endl; //Informa al usuario
    }
}

//Función para generar un laberinto utilizando el algoritmo de backtracking con una pila
vector<vector<int>> generateMaze(int width, int height) {
    vector<vector<int>> maze(height, vector<int>(width, WALL)); //Inicializa el laberinto con paredes
    stack<Cell> cellStack; //Pila para manejar el backtracking
    Cell currentCell = {1, 1}; //Comienza en la celda (1,1)

    maze[currentCell.y][currentCell.x] = PATH; //Marca la celda inicial como camino
    cellStack.push(currentCell); //Añade la celda inicial a la pila

    random_device rd; //Dispositivo de generación de números aleatorios
    mt19937 g(rd());  //Motor de generación de números aleatorios

    int directions[4][2] = { {0, 2}, {2, 0}, {0, -2}, {-2, 0} }; //Array con las direcciones posibles para moverse en el laberinto

    while (!cellStack.empty()) { //Mientras haya celdas en la pila
        currentCell = cellStack.top(); //Toma la celda en la cima de la pila
        cellStack.pop(); //Elimina la celda de la pila

        vector<int> dirOrder = {0, 1, 2, 3}; //Vector con las direcciones
        shuffle(dirOrder.begin(), dirOrder.end(), g); //Mezcla las direcciones aleatoriamente

        for (int i = 0; i < 4; ++i) { //Intenta moverse en cada dirección
            int dx = directions[dirOrder[i]][0]; //Cambio en x
            int dy = directions[dirOrder[i]][1]; //Cambio en y
            int nx = currentCell.x + dx; //Nueva posición x
            int ny = currentCell.y + dy; //Nueva posición y

            if (nx > 0 && nx < width - 1 && ny > 0 && ny < height - 1 && maze[ny][nx] == WALL) { //Verifica si la nueva posición es válida y no ha sido visitada
                maze[ny][nx] = PATH; //Marca la nueva celda como camino
                maze[currentCell.y + dy / 2][currentCell.x + dx / 2] = PATH; //Marca el camino intermedio
                cellStack.push({nx, ny}); //Añade la nueva celda a la pila
            }
        }
    }
    
    return maze; //Devuelve el laberinto generado
}

//Función para imprimir el laberinto en la consola
void printMaze(const vector<vector<int>>& maze) {
    for (const auto& row : maze) { //Itera sobre cada fila del laberinto
        for (int cell : row) { //Itera sobre cada celda de la fila
            cout << (cell == WALL ? '#' : ' ') << ' '; //Imprime '#' para paredes y ' ' para caminos
        }
        cout << endl; //Nueva línea al final de cada fila
    }
}

//Función para imprimir un paso de la solución del laberinto
void printSolutionStep(const vector<vector<int>>& maze, const vector<Cell>& path, int step) {
    vector<vector<int>> mazeCopy = maze; //Copia del laberinto original
    for (int i = 0; i <= step; ++i) {
        mazeCopy[path[i].y][path[i].x] = SOLUTION; //Marca el camino de la solución hasta el paso actual
    }

    mazeCopy[1][1] = 'E'; //Marca la entrada
    mazeCopy[maze.size() - 2][maze[0].size() - 2] = 'S'; //Marca la salida

    for (const auto& row : mazeCopy) { //Imprime el laberinto con la solución hasta el paso actual
        for (int cell : row) {
            if (cell == WALL)
                cout << '#'; //Imprime '#'
            else if (cell == PATH)
                cout << ' '; //Imprime ' '
            else if (cell == SOLUTION)
                cout << '*'; //Marca el camino de la solución
            else if (cell == 'E')
                cout << 'E'; //Marca la entrada
            else if (cell == 'S')
                cout << 'S'; //Marca la salida
        }
        cout << endl; //Nueva línea al final de cada fila
    }
}

//Función para limpiar la pantalla de la consola
void clearScreen() {
    for (int i = 0; i < 50; ++i) { //Imprime múltiples líneas en blanco
        cout << endl;
    }
}

//Estructura para representar un nodo en la búsqueda en anchura (BFS)
struct Node {
    int x, y; //Coordenadas del nodo
    vector<Cell> path; //Camino recorrido hasta este nodo
};

//Función para resolver el laberinto paso a paso utilizando BFS
void solveMazeStepByStep(const vector<vector<int>>& maze) {
    int width = maze[0].size(), height = maze.size(); //Obtiene las dimensiones del laberinto
    vector<vector<bool>> visited(height, vector<bool>(width, false)); //Matriz para marcar celdas visitadas
    queue<Node> q; //Cola para manejar los nodos a explorar

    q.push({1, 1, {{1, 1}}}); //Añade la celda inicial a la cola
    visited[1][1] = true; //Marca la celda inicial como visitada

    int directions[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} }; //Direcciones posibles para moverse

    while (!q.empty()) { //Mientras haya nodos en la cola
        Node node = q.front(); //Toma el nodo al frente de la cola
        q.pop(); //Elimina el nodo de la cola

        if (node.x == width - 2 && node.y == height - 2) { //Si se llega a la salida
            for (int step = 0; step < node.path.size(); ++step) { //Imprime la solución paso a paso
                clearScreen(); //Limpia la pantalla
                printSolutionStep(maze, node.path, step); //Imprime la solución hasta el paso actual
                this_thread::sleep_for(chrono::milliseconds(200)); //Pausa de 200ms entre pasos
            }
            return; //Termina la función al encontrar la solución
        }

        for (int i = 0; i < 4; ++i) { //Explora las direcciones posibles
            int nx = node.x + directions[i][0]; //Nueva posición x
            int ny = node.y + directions[i][1]; //Nueva posición y

            if (nx >= 0 && nx < width && ny >= 0 && ny < height && !visited[ny][nx] && maze[ny][nx] == PATH) { //Verifica si la nueva posición es válida y no ha sido visitada
                visited[ny][nx] = true; //Marca la nueva celda como visitada
                vector<Cell> newPath = node.path; //Crea un nuevo camino
                newPath.push_back({nx, ny}); //Añade la nueva celda al camino
                q.push({nx, ny, newPath}); //Añade la nueva celda a la cola
                
                clearScreen(); //Limpia la pantalla
                printSolutionStep(maze, newPath, newPath.size() - 1); //Imprime la solución hasta el paso actual
                this_thread::sleep_for(chrono::milliseconds(200)); //Pausa de 200ms entre pasos
            }
        }
    }

    cout << "No se encontró solución.\n"; //Mensaje si no se encuentra solución
}

//Función para imprimir el tiempo de generación y resolución
void printTime(duration<double> genTime, duration<double> solveTime) {
    cout << "Tiempo de generación del laberinto: " << genTime.count() << " segundos." << endl;
    cout << "Tiempo de resolución del laberinto: " << solveTime.count() << " segundos." << endl;
}

//Función principal
int main() {
    int width, height; //Variables para almacenar el tamaño del laberinto
    getUserInput(width, height); //Obtener el tamaño del laberinto del usuario

    auto startGen = high_resolution_clock::now(); //Marca el inicio del tiempo de generación
    vector<vector<int>> maze = generateMaze(width, height); //Generar laberinto
    auto endGen = high_resolution_clock::now(); //Marca el fin del tiempo de generación
    duration<double> genTime = endGen - startGen; //Calcula el tiempo de generación

    cout << "Laberinto generado:\n"; //Mostrar laberinto generado
    printMaze(maze); //Imprimir laberinto

    auto startSolve = high_resolution_clock::now(); //Marca el inicio del tiempo de resolución
    cout << "Resolviendo laberinto paso por paso:\n"; //Resolver laberinto paso por paso
    solveMazeStepByStep(maze); //Llamar a la función de resolución
    auto endSolve = high_resolution_clock::now(); //Marca el fin del tiempo de resolución
    duration<double> solveTime = endSolve - startSolve; //Calcula el tiempo de resolución

    printTime(genTime, solveTime); //Imprime el tiempo de generación y resolución

    return 0; //Finaliza el programa
}

