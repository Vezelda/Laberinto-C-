#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>

using namespace std;
using namespace chrono;

const int WALL = 1;
const int PATH = 0;
const int SOLUTION = 2;

struct Cell {
    int x, y;
};

// Función para obtener el tamaño del laberinto del usuario
void getUserInput(int& width, int& height) {
    cout << "Ingrese el número de filas (debe ser impar): ";
    cin >> height;
    cout << "Ingrese el número de columnas (debe ser impar): ";
    cin >> width;

    if (height % 2 == 0) {
        height++;
        cout << "El número de filas se incrementó a " << height << " para ser impar." << endl;
    }
    if (width % 2 == 0) {
        width++;
        cout << "El número de columnas se incrementó a " << width << " para ser impar." << endl;
    }
}

// Función para generar un laberinto utilizando el algoritmo de backtracking con una pila
vector<vector<int>> generateMaze(int width, int height) {
    vector<vector<int>> maze(height, vector<int>(width, WALL));
    stack<Cell> cellStack;
    Cell currentCell = {1, 1};

    maze[currentCell.y][currentCell.x] = PATH;
    cellStack.push(currentCell);

    random_device rd;
    mt19937 g(rd());

    int directions[4][2] = { {0, 2}, {2, 0}, {0, -2}, {-2, 0} };

    while (!cellStack.empty()) {
        currentCell = cellStack.top();
        cellStack.pop();

        vector<int> dirOrder = {0, 1, 2, 3};
        shuffle(dirOrder.begin(), dirOrder.end(), g);

        for (int i = 0; i < 4; ++i) {
            int dx = directions[dirOrder[i]][0];
            int dy = directions[dirOrder[i]][1];
            int nx = currentCell.x + dx;
            int ny = currentCell.y + dy;

            if (nx > 0 && nx < width - 1 && ny > 0 && ny < height - 1 && maze[ny][nx] == WALL) {
                maze[ny][nx] = PATH;
                maze[currentCell.y + dy / 2][currentCell.x + dx / 2] = PATH;
                cellStack.push({nx, ny});
            }
        }
    }
    
    return maze;
}

// Función para imprimir el laberinto en la consola
void printMaze(const vector<vector<int>>& maze) {
    for (const auto& row : maze) {
        for (int cell : row) {
            cout << (cell == WALL ? '#' : ' ') << ' ';
        }
        cout << endl;
    }
}

// Función para imprimir un paso de la solución del laberinto
void printSolutionStep(const vector<vector<int>>& maze, const vector<Cell>& path, int step) {
    vector<vector<int>> mazeCopy = maze;
    for (int i = 0; i <= step; ++i) {
        mazeCopy[path[i].y][path[i].x] = SOLUTION;
    }

    mazeCopy[1][1] = 'E';
    mazeCopy[maze.size() - 2][maze[0].size() - 2] = 'S';

    for (const auto& row : mazeCopy) {
        for (int cell : row) {
            if (cell == WALL)
                cout << '#';
            else if (cell == PATH)
                cout << ' ';
            else if (cell == SOLUTION)
                cout << '*';
            else if (cell == 'E')
                cout << 'E';
            else if (cell == 'S')
                cout << 'S';
        }
        cout << endl;
    }
}

// Función para limpiar la pantalla de la consola
void clearScreen() {
    for (int i = 0; i < 50; ++i) {
        cout << endl;
    }
}

// Estructura para representar un nodo en la búsqueda en anchura (BFS)
struct Node {
    int x, y;
    vector<Cell> path;
};

// Función para resolver el laberinto paso a paso utilizando BFS
void solveMazeStepByStep(const vector<vector<int>>& maze) {
    int width = maze[0].size(), height = maze.size();
    vector<vector<bool>> visited(height, vector<bool>(width, false));
    queue<Node> q;

    q.push({1, 1, {{1, 1}}});
    visited[1][1] = true;

    int directions[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };

    while (!q.empty()) {
        Node node = q.front();
        q.pop();

        if (node.x == width - 2 && node.y == height - 2) {
            for (int step = 0; step < node.path.size(); ++step) {
                clearScreen();
                printSolutionStep(maze, node.path, step);
                this_thread::sleep_for(chrono::milliseconds(200));
            }
            return;
        }

        for (int i = 0; i < 4; ++i) {
            int nx = node.x + directions[i][0];
            int ny = node.y + directions[i][1];

            if (nx >= 0 && nx < width && ny >= 0 && ny < height && !visited[ny][nx] && maze[ny][nx] == PATH) {
                visited[ny][nx] = true;
                vector<Cell> newPath = node.path;
                newPath.push_back({nx, ny});
                q.push({nx, ny, newPath});
                
                clearScreen();
                printSolutionStep(maze, newPath, newPath.size() - 1);
                this_thread::sleep_for(chrono::milliseconds(200));
            }
        }
    }

    cout << "No se encontró solución.\n";
}

// Función para imprimir el tiempo de generación y resolución
void printTime(duration<double> genTime, duration<double> solveTime) {
    cout << "Tiempo de generación del laberinto: " << genTime.count() << " segundos." << endl;
    cout << "Tiempo de resolución del laberinto: " << solveTime.count() << " segundos." << endl;
}

// Función principal
int main() {
    int width, height;
    getUserInput(width, height);  // Obtener el tamaño del laberinto del usuario

    auto startGen = high_resolution_clock::now();  // Marca el inicio del tiempo de generación
    vector<vector<int>> maze = generateMaze(width, height);  // Generar laberinto
    auto endGen = high_resolution_clock::now();  // Marca el fin del tiempo de generación
    duration<double> genTime = endGen - startGen;  // Calcula el tiempo de generación

    cout << "Laberinto generado:\n";
    printMaze(maze);  // Imprimir laberinto

    auto startSolve = high_resolution_clock::now();  // Marca el inicio del tiempo de resolución
    cout << "Resolviendo laberinto paso por paso:\n";
    solveMazeStepByStep(maze);  // Llamar a la función de resolución
    auto endSolve = high_resolution_clock::now();  // Marca el fin del tiempo de resolución
    duration<double> solveTime = endSolve - startSolve;  // Calcula el tiempo de resolución

    printTime(genTime, solveTime);  // Imprime el tiempo de generación y resolución

    return 0;
}
