#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <set>
//#include "diagrams.h"
#include "diagrams.cpp"

using namespace std;
using namespace sf;

ifstream fin("input.txt");

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900
#define SIZE 1000
#define BLOCK_WIDTH 300.0
#define BLOCK_HEIGHT 60.0
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

    Tree -> length = BLOCK_WIDTH; ///latimea standard a unui bloc
    Tree -> height = BLOCK_HEIGHT; ///inaltimea standard a unui bloc
    Tree -> x = 300; ///coordonata x de inceput (stanga sus)
    Tree -> y = 100; ///coordonata y de inceput stanga sus
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

// desenarea diagramei
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
            string str;

            // desenare text pentru if
            for(int i = 1; i < (int)currentNode -> words.size(); i++)
                str += currentNode -> words[i];
            //window.draw(createText(xUp+(xDown-xUp)/2, yUp, (char*)str.c_str(), font));
            
            // desenare blocul pentru if
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

            window.draw(singleStepCreate(topLeft, bottomRight));
        }

        for (node* nextNode : currentNode -> next)
            printDiagram_DFS(nextNode, window);
    }
}

void pollEvents(RenderWindow &window)
{
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed)
            window.close();
        if (event.type == Event::KeyPressed)
        {
            if (event.key.code == Keyboard::Escape)
                window.close();
        }
    }
}

void updateWindow(RenderWindow &window)
{
    window.clear();

    printDiagram_DFS(Tree, window);

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
        updateWindow(window);
    }
    fin.close();

    return 0;
}
