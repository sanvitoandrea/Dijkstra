#include <iostream>
#include <random>
#include <iomanip>



const int size = 30, infinity = 99 * size + 1;   /* size = total numnber of nodes, while parameter "infinity" is used for calculations */
const double density = 0.35;   /* parameter used for randomization */

std::default_random_engine eng(time(NULL));   /* engine and distributions to create a random graph */
std::uniform_real_distribution<> doubleDis(0, 1);
std::uniform_int_distribution<> integerDis(1, 99);

double getDoubleNumber() { return doubleDis(eng); }
int getIntegerNumber() { return integerDis(eng); }




class graph {   /* graph that keeps track of the connections between nodes. if graph[i][j] = 10, it means that between nodes i and j there's an edge with a cost equal to 10 */
    public:
        int** structure;
        graph(int size) {
            structure = new int*[size];

            for(int i = 0; i < size; i++)
                structure[i] = new int[size];

            for(int i = 0; i < size; i++)
                for(int j = i; j < size; j++)
                    structure[i][j] = structure[j][i] = 0;    /* setting loopbacks as edges with value zero */
        }

        void initialize() {
            for(int i = 0; i < size; i++)
                for(int j = i; j < size; j++)
                    if(i != j) {
                        if(getDoubleNumber() < density) 
                            structure[i][j] = structure[j][i] = getIntegerNumber();
                        else
                            structure[i][j] = structure[j][i] = infinity;   /* every edge will have either a random 1-99 number (if the edge exists) or infinity (if the edge doesn't exist) */
                    }

            return;
        }

        void print() {   /* prints the graph as a matrix */
            std::cout << "       ";
            for(int i = 0; i < size; i++) {
                std::cout << "(";
                if(i < 10) std::cout << 0;
                std::cout << i << ")  ";
            }
            std::cout << std::endl;
            
            for(int i = 0; i < size; i++) {
                std::cout << "(";
                if(i < 10) std::cout << 0; 
                std::cout << i << ") ";

                for(int j = 0; j < size; j++) {
                    if(structure[i][j] != infinity)
                        std::cout << std::setw(5) << structure[i][j] << " ";
                    else std::cout << "   -- ";
                }

                std::cout << std::endl;
            }

            std::cout << std::endl;
            return;
        }

        ~graph() {
            for(int i = 0; i < size; i++)
                delete structure[i];

            delete structure;
        }
};

struct node {   /* representation of a single node */
    int id;
    node* next;
};

class nodeList { /* group of nodes that is assured are part of the MST (minimum spanning tree) */
    public:
        node* head;
        node* cursor;

        nodeList(int start) {   /* constructor can only be called with a beginning value (the starting point of the algorithm) */
            head = new node;
            head -> id = start;
            head -> next = nullptr;

            cursor = head;
        }

        int count() {   /* counts the elements of the list */
            if(!head) return 0;

            int counter = 0;

            for(cursor = head; cursor != nullptr; cursor = cursor -> next)
                counter++;
            
            cursor = head;
            return counter;
        }

        void append(int value) {   /* appends a given value to the end of the list */
            if(!head) {   /* actually this is never going to be used, just for safety */
                head = new node;
                head -> id = value;
                head -> next = nullptr;

                cursor = head;
            }
            else {    /* runs to the end of the list and creates a node */
                for(cursor = head; cursor -> next != nullptr; cursor = cursor -> next);

                cursor -> next = new node;
                cursor -> next -> id = value;
                cursor -> next -> next = nullptr;

                cursor = head;
            }

            return;
        }

        void print() {   /* prints the list */
            if(!head) {   /* this is never going to be used. again for safety reasons */
                std::cout << std::endl << "warning: set is currently empty" << std::endl;
                return;
            }

            node* previous = cursor;
            std::cout << "closed set: ";
            for(cursor = head; cursor != nullptr; cursor = cursor -> next)
                std::cout << cursor -> id << ", ";

            cursor = previous;
            return;
        }

        ~nodeList() {
            node* nextNode = head;

            while(head != nullptr) {
                nextNode = nextNode -> next;
                delete head;
                head = nextNode;
            }
        }

};

class distances {   /* class representing the distances between the startgin point and certain node, and the last node used to reach a certain destination */
    public:
        int cost[size];
        int previousNode[size];
        int startingNode;

        distances(int start) {  /* all the costs are set to infinity and all the positions to -1, except the starting point which will have a cost equal to zero */
            startingNode = start;
            for(int i = 0; i < size; i++) {
                previousNode[i] = -1;
                cost[i] = (i == start ? 0 : infinity);
            }
        }

        void print() {
            std::cout << "distances from starting point" << std::endl;

            for(int i = 0; i < size; i++) {
                std::cout << "node (";
                if(i < 10) std::cout << 0;

                if(i == startingNode) std::cout << i << ")  -->  start" << std::endl;
                else {
                    std::cout << i << ")  -->  ";
                    previousNode[i] != -1 ? std::cout << "cost = " << cost[i] << ", previous node = " << previousNode[i] << std::endl : std::cout << "unreachable" << std::endl;
                }
            } 

            std::cout << std::endl;
            return;
        }

        void update(graph& g, nodeList& closedSet) {   /* method that updates the distances for every iteration of the algorithm */
            int singleCost;

            for(int currentNode = 0; currentNode < size; currentNode++)
                for(closedSet.cursor = closedSet.head; closedSet.cursor != nullptr; closedSet.cursor = closedSet.cursor -> next) {
                    int nodeInClosedSet = closedSet.cursor -> id;

                    if(g.structure[nodeInClosedSet][currentNode])   /* checks if that specific edge exists (then if it's different from zero) */
                        if((singleCost = g.structure[nodeInClosedSet][currentNode] + cost[nodeInClosedSet]) < cost[currentNode]) {   /* if the new cost (singleCost) is lower than the previous, update and keep track of the previous node*/
                            cost[currentNode] = singleCost;
                            previousNode[currentNode] = nodeInClosedSet;
                        } 
                }

            closedSet.cursor = closedSet.head;
            return;
        }

        ~distances() { }
};




inline void getStartingNode(int& start) {   /* simply asks for a node in input, which will be used as the base for the distances */
    bool valid;

    std::cout << "what node would you like the algorithm to start from?\n> ";

    do {
        valid = true;
        std::cin >> start;

        if(start > size - 1) {
            std::cout << std::endl << "please enter a valid input (from 0 to " << size - 1 << ")\n> ";
            valid = false;
        }
    } while(!valid);

    return;
}

inline bool inClosedSet(int value, nodeList& closedSet) {  /* checks if a given value is inside a node of the closedSet */
    bool found = false; 

    for(node* helper = closedSet.head; helper != nullptr && !found; helper = helper -> next)
        if(value == helper -> id) found = true;

    return found;
}

inline int next(graph& g, nodeList& closedSet, distances& distance) {  /* given a closed set, calculates which is the next node to be included inside of it */
    if(!closedSet.head) return -2;  /* should never happen */

    int nextNode = -1, minimum = infinity;

    for(closedSet.cursor = closedSet.head; closedSet.cursor != nullptr; closedSet.cursor = closedSet.cursor -> next) {  /* for each node in the MST, checks which is the closes node to the startingNode */
        int currentInsideNode = closedSet.cursor -> id;

        for(int currentOutsideNode = 0; currentOutsideNode < size; currentOutsideNode++)
            if(!inClosedSet(currentOutsideNode, closedSet) && g.structure[currentInsideNode][currentOutsideNode] + distance.cost[currentInsideNode] < minimum){
                minimum = g.structure[currentInsideNode][currentOutsideNode] + distance.cost[currentInsideNode];
                nextNode = currentOutsideNode;
            }
    }

    closedSet.cursor = closedSet.head;
    return nextNode;  /* returns the closest node*/
}

bool dijkstra(graph& g, distances& distance, nodeList& closedSet) {
    bool converged = false;   /* variable to keep track if algorithm has converged or not (it's demonstrated that it converges in maximum "size - 1" iterations)*/
    for(int iteration = 0; iteration < size - 1 && !converged; iteration++) {
        distance.update(g, closedSet);

        int nextNode = next(g, closedSet, distance);  /* get the next node to append to the MST group */

        if(nextNode == -2) {   /* only happens if somehow closedSet is empty (should never happen) */
            std::cout << "\nfatal error\n";
            return true;
        }
        else if(nextNode == -1) converged = true;   /* early exit if algorithm has converged before (size - 1) iterations */
        else closedSet.append(nextNode);
    }

    return false;
}





int main() {
    graph g(size);   /* creates graph object */
    g.initialize();   /* fills with values depending on density parameter */
    std::cout << "\ngraph generated:\n";
    g.print();

    int startingNode;
    getStartingNode(startingNode);  /* asks for a starting node in input */
    
    nodeList closedSet(startingNode);   /* closed set = group of nodes in the minium spanning tree (MST) set */
    distances distance(startingNode);   /* all the distances from startingNode */

    bool good = !dijkstra(g, distance, closedSet);

    if(!good) {  /* dijkstra function returns whether everything went smooth or not */
        std::cout << "fatal error: something went wrong. terminating.\n\n";
        return true;
    }

    std::cout << std::endl;
    distance.print();  
    return 0;
}