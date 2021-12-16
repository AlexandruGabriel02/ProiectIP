#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <set>
//#include "diagrams.h"
#include "diagrams.cpp"

using namespace std;
using namespace sf;

ifstream fin("input.txt");

//#define SCREEN_WIDTH 1200
//#define SCREEN_HEIGHT 900
float SCREEN_WIDTH = 1280;
float SCREEN_HEIGHT = 720;
float MINSCR_WIDTH = 1158;
float MINSCR_HEIGHT = 585;
bool sizeScreen = false;
#define BLOCK_WIDTH 300.0
#define BLOCK_HEIGHT 60.0
// interfata diagramei
#define DIAGRAM_MARGIN_WIDTH 75
#define DIAGRAM_MARGIN_HEIGHT 75
//#define DIAGRAM_WIDTH 500
//#define DIAGRAM_HEIGHT 700
float DIAGRAM_WIDTH = 500;
float DIAGRAM_HEIGHT = SCREEN_HEIGHT-DIAGRAM_MARGIN_HEIGHT*2;


enum instructionType {EMPTY_NODE, VAR, SET, IF, WHILE, READ, PRINT, PASS, END, ERROR};
enum errorType {SYNTAX_ERROR_INSTRUCTION, SYNTAX_ERROR_VARTYPE,
    SYNTAX_ERROR_VARIABLE, SYNTAX_ERROR_LINE, ERROR_UNDECLARED, ERROR_MULTIPLE_DECLARATION, ERROR_EXPRESSION}; ///de adaugat pe parcurs
string errorMessage[] =
{
    "Instructiune invalida la linia ",
    "Tip de date declarat incorect la linia ",
    "Variabila declarata incorect la linia ",
    "Linia de cod contine prea putine sau prea multe cuvinte la linia ",
    "Variabila nedeclarata utilizata la linia ",
    "Variabila declarata deja este utilizata la linia ",
    "Expresie incorecta aritmetic la linia " ///de facut
};

// pozita interfatei diagramei
Point originIDiagram = {SCREEN_WIDTH-DIAGRAM_WIDTH-DIAGRAM_MARGIN_WIDTH, DIAGRAM_MARGIN_HEIGHT};

// pozitia initiala a diagramei
Point originDiagramP = {originIDiagram.x+DIAGRAM_WIDTH/2, originIDiagram.y+50};
Point diagramP = originDiagramP;

// variabilele pentru zoom
float zoom = 1, zoomScale = 0.1, zoomMinScale = 0.75;

// variabilele pentru move
bool moveScreen = false;
Point moveP = {0, 0};
Point amoveP = {0, 0};

///verificarea erorilor in arbore, definite de functia checkErrors_DFS()
bool validTree = true;
pair <errorType, int> error;
set <char> declaredGlobal;
set <char> declaredLocal;
///

struct node
{
    int lineOfCode = 0;
    instructionType instruction;
    string line;
    vector <string> words;

    float x, y; ///coordonatele stanga sus ale blocului
    float length;
    float height;

    int verticalNodeCount; ///voi avea nevoie pentru calculul inaltimii

    vector <node*> next;

};
typedef node* tree;

///arborele de sintaxa al pseudocodului
tree Tree;

///sterg arborele din memorie
void clearTree(node* &currentNode)
{
    if (currentNode != NULL)
    {
        for (node* nextNode : currentNode -> next)
            clearTree(nextNode);

        currentNode -> line.clear();
        currentNode -> words.clear();
        currentNode -> next.clear();
        currentNode = NULL;
        delete currentNode;
    }
}


void initTree()
{
    //clearTree();
    Tree = new node;
    Tree -> instruction = EMPTY_NODE;

    Tree -> length = BLOCK_WIDTH * zoom; ///latimea standard a unui bloc
    Tree -> height = BLOCK_HEIGHT * zoom; ///inaltimea standard a unui bloc
    //Tree -> x = diagramP.x; ///coordonata x de inceput (stanga sus)
    Tree -> x = diagramP.x-(BLOCK_WIDTH*zoom)/2; //coorodnata x de inceput (mijloc sus) (ca sa fie zoom ul mai frumos)
    Tree -> y = diagramP.y; ///coordonata y de inceput stanga sus
}

string readLineFromFile()
{
    string str;
    getline(fin, str);
    return str;
}

vector<string> splitIntoWords(string str)
{
    vector<string> result;
    string word;
    int strSize = str.size();

    for (int i = 0; i < strSize; i++)
    {
        if (str[i] != ' ')
        {
            word.clear();
            while (str[i] != ' ' && i < strSize)
            {
                word.push_back(str[i]);
                i++;
            }
            i--;
            result.push_back(word);
        }
    }

    return result;
}

instructionType getInstructionType(vector<string> vStr)
{
    ///returneaza primul cuvant
    string word = "";
    if (!vStr.empty())
        word = vStr[0];

    if (word == "var")
        return VAR;
    else if (word == "set")
        return SET;
    else if (word == "if")
        return IF;
    else if (word == "while")
        return WHILE;
    else if (word == "pass")
        return PASS;
    else if (word == "read")
        return READ;
    else if (word == "print")
        return PRINT;
    else if (word == "else" || word == "endif" || word == "endwhile")
        return END;

    return ERROR;
}

void buildTree(node* &currentNode)
{
    static int lineCount = 0; ///linia de cod la care ma aflu
    if (currentNode -> instruction == EMPTY_NODE)
    {
        bool exitWhile = false;
        while (!exitWhile)
        {
            string lineStr = readLineFromFile();
            lineCount++;

            if (fin.eof()) ///am ajuns la finalul fisierului
                exitWhile = true;
            else
            {
                node* newNode = new node;
                newNode -> lineOfCode = lineCount;
                newNode -> line = lineStr;
                newNode -> words = splitIntoWords(lineStr);
                newNode -> instruction = getInstructionType(newNode -> words);

                ///daca intalnesc else, endif sau endwhile trebuie sa ma intorc in sus in arbore
                if (newNode -> instruction == END)
                {
                    exitWhile = true; ///ies din while chiar daca nu termin de citit fisierul

                    ///daca am altceva pe langa acel "else\endif\endwhile" atunci linia nu e valida
                    if (newNode -> words.size() != 1)
                    {
                        validTree = false;
                        error = make_pair(SYNTAX_ERROR_LINE, newNode -> lineOfCode);
                        return;
                    }

                    delete newNode; ///nu am avut nevoie de nod
                }
                else
                {
                    ///am legat nodurile
                   (currentNode -> next).push_back(newNode);
                    buildTree(newNode);
                }
            }
        }
    }
    else if (currentNode -> instruction == IF)
    {
        node* trueNode = new node;
        trueNode -> instruction = EMPTY_NODE;
        (currentNode -> next).push_back(trueNode);
        buildTree(trueNode);

        node* falseNode = new node;
        falseNode -> instruction = EMPTY_NODE;
        (currentNode -> next).push_back(falseNode);
        buildTree(falseNode);
    }
    else if (currentNode -> instruction == WHILE)
    {
        node* newNode = new node;
        newNode -> instruction = EMPTY_NODE;
        (currentNode -> next).push_back(newNode);
        buildTree(newNode);
    }
}

///parcurgerea arborelui; va trebui apelat de mai multe ori pt verificarea corectitudinii
///pseudocodului si calculul coordonatelor si marimii fiecarui bloc
void TreeDFS(node* currentNode, int height)
{
    if (currentNode != NULL)
    {
        cout << "linie: " << currentNode -> lineOfCode << "\n";
        cout << "verticalCount: " << currentNode -> verticalNodeCount << "\n";
        cout << currentNode -> instruction << " ";
        if (currentNode -> instruction != EMPTY_NODE)
            cout << currentNode -> line << "\n";
        else
            cout << "\n";
        cout << "\n";

        for (node* nextNode : currentNode -> next)
            TreeDFS(nextNode, height + 1);
    }
}

///verificarea erorilor
///mai sunt de tratat cazurile cu expresii gresite si eventual cazuri in care pun gresit endif, else, endwhile, etc.
void checkErrors_DFS(node* currentNode)
{
    if (currentNode != NULL && validTree)
    {
        if (currentNode -> instruction == EMPTY_NODE)
            declaredLocal.clear();
        else if (currentNode -> instruction == ERROR) ///tipul de instructiune nu este recunoscut
        {
            validTree = false;
            error = make_pair(SYNTAX_ERROR_INSTRUCTION, currentNode -> lineOfCode);
            return;
        }
        else if (currentNode -> instruction == VAR)
        {
            if (currentNode -> words.size() != 3) ///o linie de tip var trebuie sa aiba 3 cuvinte; aceeasi idee si mai jos
            {
                validTree = false;
                error = make_pair(SYNTAX_ERROR_LINE, currentNode -> lineOfCode);
                return;
            }
            else
            {
                if (currentNode -> words[1] != "int" && currentNode -> words[1] != "string")
                {
                    validTree = false;
                    error = make_pair(SYNTAX_ERROR_VARTYPE, currentNode -> lineOfCode);
                    return;
                }

                char ch = currentNode -> words[2][0];
                if (currentNode -> words[2].size() != 1 || !isalpha(ch))
                {
                    validTree = false;
                    error = make_pair(SYNTAX_ERROR_VARIABLE, currentNode -> lineOfCode);
                    return;
                }

                ///declararea variabilelor
                if (declaredGlobal.find(ch) == declaredGlobal.end())
                {
                    declaredGlobal.insert(ch);
                    declaredLocal.insert(ch);
                }
                else
                {
                    validTree = false;
                    error = make_pair(ERROR_MULTIPLE_DECLARATION, currentNode -> lineOfCode);
                    return;
                }
            }
        }
        else if (currentNode -> instruction == SET)
        {
            if (currentNode -> words.size() != 3)
            {
                validTree = false;
                error = make_pair(SYNTAX_ERROR_LINE, currentNode -> lineOfCode);
                return;
            }
            else
            {
                char ch = currentNode -> words[1][0];
                if (currentNode -> words[1].size() != 1 || !isalpha(ch))
                {
                    validTree = false;
                    error = make_pair(SYNTAX_ERROR_VARIABLE, currentNode -> lineOfCode);
                    return;
                }

                if (declaredGlobal.find(ch) == declaredGlobal.end()) ///nu am declarat variabila ch
                {
                    validTree = false;
                    error = make_pair(ERROR_UNDECLARED, currentNode -> lineOfCode);
                    return;
                }

                ///de verificat corectitudinea expresiei (currentNode -> words[2])
            }
        }
        else if (currentNode -> instruction == READ)
        {
            if (currentNode -> words.size() != 2)
            {
                validTree = false;
                error = make_pair(SYNTAX_ERROR_LINE, currentNode -> lineOfCode);
                return;
            }
            else
            {
                char ch = currentNode -> words[1][0];
                if (currentNode -> words[1].size() != 1 || !isalpha(ch))
                {
                    validTree = false;
                    error = make_pair(SYNTAX_ERROR_VARIABLE, currentNode -> lineOfCode);
                    return;
                }

                if (declaredGlobal.find(ch) == declaredGlobal.end()) ///nu am declarat variabila ch
                {
                    validTree = false;
                    error = make_pair(ERROR_UNDECLARED, currentNode -> lineOfCode);
                    return;
                }
            }
        }
        else if (currentNode -> instruction == PRINT)
        {
            if (currentNode -> words.size() != 2)
            {
                validTree = false;
                error = make_pair(SYNTAX_ERROR_LINE, currentNode -> lineOfCode);
                return;
            }
            else
            {
                /*
                    eventual de tratat aici cazul "print 'sir de caractere'"
                */

                char ch = currentNode -> words[1][0];
                if (currentNode -> words[1].size() != 1 || !isalpha(ch))
                {
                    validTree = false;
                    error = make_pair(SYNTAX_ERROR_VARIABLE, currentNode -> lineOfCode);
                    return;
                }

                if (declaredGlobal.find(ch) == declaredGlobal.end()) ///nu am declarat variabila ch
                {
                    validTree = false;
                    error = make_pair(ERROR_UNDECLARED, currentNode -> lineOfCode);
                    return;
                }
            }
        }
        else if (currentNode -> instruction == PASS || currentNode -> instruction == END)
        {
            if (currentNode -> words.size() != 1)
            {
                validTree = false;
                error = make_pair(SYNTAX_ERROR_LINE, currentNode -> lineOfCode);
                return;
            }
        }
        else if (currentNode -> instruction == IF || currentNode -> instruction == WHILE)
        {
            if (currentNode -> words.size() != 2)
            {
                validTree = false;
                error = make_pair(SYNTAX_ERROR_LINE, currentNode -> lineOfCode);
                return;
            }
            else
            {
                /*
                    de verificat daca expresia este valida (currentNode -> words[1])
                */
            }
        }

        for (node* nextNode : currentNode -> next)
            checkErrors_DFS(nextNode);

        ///sterg variabilele declarate in interiorul unui if/while deoarece am ajuns la final
        if (currentNode -> instruction == EMPTY_NODE)
        {
            for (char ch : declaredLocal)
                declaredGlobal.erase(ch);
            declaredLocal.clear();
        }
    }
}

///dp[empty_node] = noduri_simple + (1 + dp[while_empty]) + (1 + max(dp[if_T], dp[if_F]))
///dp - tinut in verticalNodeCount
void buildDP_DFS(node* &currentNode)
{
    if (currentNode != NULL)
    {
        if (currentNode -> instruction == EMPTY_NODE)
            currentNode -> verticalNodeCount = 0;

        for (node* nextNode : currentNode -> next)
        {
            buildDP_DFS(nextNode);
            if (currentNode -> instruction == EMPTY_NODE)
                currentNode -> verticalNodeCount += nextNode -> verticalNodeCount;
        }

        if (currentNode -> instruction == IF)
            currentNode -> verticalNodeCount = 1 + max(currentNode -> next[0] -> verticalNodeCount, currentNode -> next[1] -> verticalNodeCount);
        else if (currentNode -> instruction == WHILE)
            currentNode -> verticalNodeCount = 1 + currentNode -> next[0] -> verticalNodeCount;
        else if (currentNode -> instruction != EMPTY_NODE)
            currentNode -> verticalNodeCount = 1;

    }
}

void buildDiagram_DFS(node* &currentNode, node* &emptyFather)
{
    if (currentNode != NULL)
    {
        ///creez dimensiunile blocului curent in functie de nodul tata
        if (currentNode -> instruction != EMPTY_NODE)
        {
            currentNode -> length = emptyFather -> length;
            currentNode -> x = emptyFather -> x;
            currentNode -> y = emptyFather -> y;

            emptyFather -> y += emptyFather -> height;

            if (currentNode -> instruction != WHILE) ///bloc de tip dreptunghi
                currentNode -> height = emptyFather -> height;
            else ///bloc de tip bucla, trebuie sa acopere toate blocurile din interior
                currentNode -> height = currentNode -> verticalNodeCount * emptyFather -> height;

        }

        ///initializez urmatoarele noduri goale care vor deveni tati
        if (currentNode -> instruction == IF)
        {
            currentNode -> next[0] -> length = emptyFather -> length / 2.; ///nodul gol de tip true
            currentNode -> next[1] -> length = emptyFather -> length / 2.; ///nodul gol de tip false
            currentNode -> next[0] -> x = emptyFather -> x;
            currentNode -> next[1] -> x = emptyFather -> x + emptyFather -> length / 2.;

            currentNode -> next[0] -> height = 1.f *
            (currentNode -> verticalNodeCount - 1) * emptyFather -> height / currentNode -> next[0] -> verticalNodeCount;
            currentNode -> next[1] -> height = 1.f *
            (currentNode -> verticalNodeCount - 1) * emptyFather -> height / currentNode -> next[1] -> verticalNodeCount;
            currentNode -> next[0] -> y = currentNode -> next[1] -> y = emptyFather -> y;

            emptyFather -> y += (currentNode -> verticalNodeCount - 1) * emptyFather -> height;

        }
        else if (currentNode -> instruction == WHILE)
        {
            float offset = emptyFather -> length / 5.; ///lungimea pentru bara din stanga
            currentNode -> next[0] -> length = emptyFather -> length - offset;
            currentNode -> next[0] -> x = emptyFather -> x + offset;

            currentNode -> next[0] -> height = emptyFather -> height;
            currentNode -> next[0] -> y = emptyFather -> y;

            emptyFather -> y += (currentNode -> verticalNodeCount - 1) * emptyFather -> height;
        }

        for (node* nextNode : currentNode -> next)
        {
            if (currentNode -> instruction == EMPTY_NODE)
                buildDiagram_DFS(nextNode, currentNode);
            else
                buildDiagram_DFS(nextNode, emptyFather);
        }
    }
}

//buildDiagram_DFS is not working if i rerun the function with anothers variables for Tree -> x, y, length, height
//i created a function that can clear the position
void deletePositionDiagram_DFS(node* currentNode)
{
    if (currentNode != NULL)
    {
        currentNode -> x = 0;
        currentNode -> y = 0;
        currentNode -> length = 0;
        currentNode -> height = 0;

        for (node* nextNode : currentNode -> next)
            deletePositionDiagram_DFS(nextNode);
    }
}

// desenarea diagramei
Font font;
void printDiagram_DFS(node* currentNode, RenderWindow &window)
{
    if (currentNode != NULL)
    {
        if (currentNode -> instruction == IF)
        {
            Point topLeft;
            topLeft.x = currentNode -> x;
            topLeft.y = currentNode -> y;
            Point bottomRight;
            bottomRight.x = topLeft.x + currentNode -> length;
            bottomRight.y = topLeft.y + currentNode -> height;

            Box box;
            box.x = topLeft.x + (bottomRight.x-topLeft.x)/4;
            box.y = topLeft.y;
            box.length = (bottomRight.x-topLeft.x)/2;
            box.height = (bottomRight.y-topLeft.y)/4;

            string str = "";
            for(int i = 1; i < (int)currentNode -> words.size(); i++) {
                if(i > 1)
                    str += ' ';
                str += currentNode -> words[i];
            }
            str += '?';

            // desenarea conditiei pentru if
            window.draw(createText(box, str, font));

            // desenare True si False
            str = "T";
            box.x = topLeft.x;
            box.y = topLeft.y+(bottomRight.y-topLeft.y)/2;
            box.length = (bottomRight.x-topLeft.x)/4;
            box.height = (bottomRight.y-topLeft.y)/4;

            window.draw(createText(box, str, font));

            str = "F";
            box.x = bottomRight.x-(bottomRight.x-topLeft.x)/4;
            box.y = topLeft.y+(bottomRight.y-topLeft.y)/2;
            box.length = (bottomRight.x-topLeft.x)/4;
            box.height = (bottomRight.y-topLeft.y)/4;

            window.draw(createText(box, str, font));

            // desenarea blocului pentru if
            window.draw(decisionCreate(topLeft, bottomRight));
        }
        else if (currentNode -> instruction == WHILE)
        {
            Point topLeft;
            topLeft.x = currentNode -> x;
            topLeft.y = currentNode -> y;
            Point bottomRight;
            bottomRight.x = topLeft.x + currentNode -> length;
            bottomRight.y = topLeft.y + currentNode -> height;
            float offset = currentNode -> length / 5.; ///lungimea pentru bara din stanga
            float rectangleHeight = currentNode -> height / currentNode -> verticalNodeCount; ///inaltimea blocului fara bara din stanga

            Box box;
            box.x = topLeft.x;
            box.y = topLeft.y;
            box.length = bottomRight.x-topLeft.x;
            box.height = rectangleHeight/2;

            string str = "";
            for(int i = 0; i < (int)currentNode -> words.size(); i++) {
                str += currentNode -> words[i];
                if (i != (int)currentNode -> words.size() - 1)
                    str += ' ';
            }

            // desenare text pentru while
            window.draw(createText(box, str, font));

            // desenarea blocului pentru while
            window.draw(iterationWCreate(topLeft, bottomRight, offset, rectangleHeight));
        }
        else if (currentNode -> instruction != EMPTY_NODE)
        {
            Point topLeft;
            topLeft.x = currentNode -> x;
            topLeft.y = currentNode -> y;
            Point bottomRight;
            bottomRight.x = topLeft.x + currentNode -> length;
            bottomRight.y = topLeft.y + currentNode -> height;

            Box box;
            box.x = topLeft.x;
            box.y = topLeft.y;
            box.length = bottomRight.x-topLeft.x;
            box.height = (bottomRight.y-topLeft.y)/2;

            string str = "";
            for(int i = 0; i < (int)currentNode -> words.size(); i++) {
                str += currentNode -> words[i];
                if (i != (int)currentNode -> words.size() - 1)
                    str += ' ';
            }

            // desenare text pentru singleStep
            window.draw(createText(box, str, font));

            // desenarea blocului pentru singleStep
            window.draw(singleStepCreate(topLeft, bottomRight));
        }

        for (node* nextNode : currentNode -> next)
            printDiagram_DFS(nextNode, window);
    }
}

void pollEvents(RenderWindow &window) {
    Event event;
    while(window.pollEvent(event)) {
        if(event.type == Event::Closed)
            window.close();

        // exit
        if(event.type == Event::KeyPressed) {
            if(event.key.code == Keyboard::Escape)
                window.close();
        }
        Vector2i positionMouse = Mouse::getPosition(window);
        if(originIDiagram.x < positionMouse.x && positionMouse.x < originIDiagram.x+DIAGRAM_WIDTH &&
           originIDiagram.y < positionMouse.y && positionMouse.y < originIDiagram.y+DIAGRAM_HEIGHT) {
            if(event.type == Event::MouseWheelMoved) {
                if(event.mouseWheel.delta == -1 && zoomMinScale < zoom)
                    zoom -= zoomScale;
                else if(event.mouseWheel.delta == 1)
                    zoom += zoomScale;
            }
        }

        // pozitia initiala si zoom ul initial
        if(event.type == Event::KeyPressed) {
            if(event.key.code == Keyboard::O) {
                diagramP = originDiagramP;
                zoom = 1;
            }
        }

        // rezise
        View view = window.getView();
        if((abs(window.getSize().x-SCREEN_WIDTH) > 1 || abs(window.getSize().y-SCREEN_HEIGHT) > 1) && event.type != Event::Resized && !sizeScreen) {
            cout<<"fcklsjdflk\n";
            sizeScreen = true;
        }
    }
}

// desenarea margenilor
void interfaceDiagramDraw(RenderWindow &window) {
    Point topLeft, bottomRight;
    Color colorFill(20, 20,20), colorLine(255, 255, 255);

    // sus
    topLeft = {0, 0};
    bottomRight = {SCREEN_WIDTH, originIDiagram.y};
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
    // dreapta
    topLeft = {originIDiagram.x+DIAGRAM_WIDTH, 0};
    bottomRight = {SCREEN_WIDTH, SCREEN_HEIGHT};
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
    // stanga
    topLeft = {0, 0};
    bottomRight = {originIDiagram.x, SCREEN_HEIGHT};
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
    // jos
    topLeft = {0, originIDiagram.y+DIAGRAM_HEIGHT};
    bottomRight = {SCREEN_WIDTH, SCREEN_HEIGHT};
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
}

void resizeMechanics(RenderWindow &window) {
    View view = window.getView();
    bool correctWidth = true;
    bool correctHeight = true;
    if(window.getSize().x < MINSCR_WIDTH)
        correctWidth = false;
    if(window.getSize().y < MINSCR_HEIGHT)
        correctHeight = false;
    if((abs(window.getSize().x-SCREEN_WIDTH) > 1 || abs(window.getSize().y-SCREEN_HEIGHT) > 1) && (sizeScreen || (correctWidth && correctHeight))) {
        if(!correctWidth && !correctHeight) {
            window.setSize(Vector2u(MINSCR_WIDTH, MINSCR_HEIGHT));
            sizeScreen = false;
        }
        else if(!correctWidth) {
            window.setSize(Vector2u(MINSCR_WIDTH, window.getSize().y));
            sizeScreen = false;
        }
        else if(!correctHeight) {
            window.setSize(Vector2u(window.getSize().x, MINSCR_HEIGHT));
            sizeScreen = false;
        }
        view = window.getView();
        SCREEN_WIDTH = window.getSize().x;
        SCREEN_HEIGHT = window.getSize().y;
        originIDiagram = {SCREEN_WIDTH-DIAGRAM_WIDTH-DIAGRAM_MARGIN_WIDTH, DIAGRAM_MARGIN_HEIGHT};
        originDiagramP = {originIDiagram.x+DIAGRAM_WIDTH/2, originIDiagram.y+50};
        diagramP = originDiagramP;
        DIAGRAM_HEIGHT = SCREEN_HEIGHT-DIAGRAM_MARGIN_HEIGHT*2;
        if((int)SCREEN_HEIGHT%2 == 1)
            SCREEN_HEIGHT += 1;
        view.setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
        view.setCenter(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    }
    window.setView(view);
}

void zoomMechanics() {
    deletePositionDiagram_DFS(Tree);
    Tree -> x = diagramP.x-(BLOCK_WIDTH*zoom)/2;
    Tree -> y = diagramP.y;
    Tree -> length = BLOCK_WIDTH * zoom;
    Tree -> height = BLOCK_HEIGHT * zoom;
    buildDiagram_DFS(Tree, Tree);
}

void moveMechanics(int direction, RenderWindow &window) {
    Vector2i positionMouse = Mouse::getPosition(window);
    if(originIDiagram.x < positionMouse.x && positionMouse.x < originIDiagram.x+DIAGRAM_WIDTH &&
       originIDiagram.y < positionMouse.y && positionMouse.y < originIDiagram.y+DIAGRAM_HEIGHT) {
        if(direction > 0) {
            if(Mouse::isButtonPressed(Mouse::Left) && !moveScreen) {
                moveScreen = true;
                amoveP.x = positionMouse.x;
                amoveP.y = positionMouse.y;
            }
            else if(!Mouse::isButtonPressed(Mouse::Left) && moveScreen) {
                diagramP.x += moveP.x;
                diagramP.y += moveP.y;
                moveScreen = false;
                amoveP = {0, 0};
                moveP = {0, 0};
            }
        }
        if(moveScreen) {
            if(direction > 0)
                moveP = {positionMouse.x-amoveP.x, positionMouse.y-amoveP.y};

            diagramP.x += direction*moveP.x;
            diagramP.y += direction*moveP.y;
        }
    }
    else {
        if(moveScreen) {
            if(direction > 0) {
                diagramP.x += direction*moveP.x;
                diagramP.y += direction*moveP.y;
            }
            else {
                moveScreen = false;
                amoveP = {0, 0};
                moveP = {0, 0};
            }
        }
    }
}

void updateWindow(RenderWindow &window)
{
    window.clear();

    printDiagram_DFS(Tree, window);
    interfaceDiagramDraw(window);

    window.display();
}

void Debugger()
{
    initTree();
    buildTree(Tree);
    checkErrors_DFS(Tree);

    if (validTree)
    {
        buildDP_DFS(Tree);
        buildDiagram_DFS(Tree, Tree);
      //  TreeDFS(Tree, 0); ///afisez arborele
      //  clearTree(Tree);

      //  cout << "stergere reusita\n";

      //  TreeDFS(Tree, 0);
    }
    else
    {
        cout << "input invalid\n";
        cout << errorMessage[error.first] << error.second << "\n";
    }
}


int main() {
    RenderWindow window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "NS Diagram");

    if(!font.loadFromFile("./font.ttf")) {
        cout << "Not found file";
        exit(0);
    }

    Debugger();

    while(window.isOpen()) {
        pollEvents(window);
        resizeMechanics(window);
        moveMechanics(1, window);
        zoomMechanics();
        updateWindow(window);
        moveMechanics(-1, window);
        sizeScreen = false;
    }
    fin.close();

    return 0;
}
