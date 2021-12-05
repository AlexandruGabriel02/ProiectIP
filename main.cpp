#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <set>
#include "diagrams.h"

using namespace std;
using namespace sf;

ifstream fin("input.txt");

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900
#define SIZE 1000
const int BLOCK_WIDTH = 200, BLOCK_HEIGHT = 60;
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
        cout << "inaltime: " << height << "\n";
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

void checkErrors_DFS(node* currentNode)
{
    if (currentNode != NULL && validTree)
    {
        if (currentNode -> instruction == EMPTY_NODE)
            declaredLocal.clear();
        ///mai trebuie adaugate cazuri de erori!
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

    window.display();
}

void Debugger()
{
    initTree();
    buildTree(Tree);
    checkErrors_DFS(Tree);

    if (validTree)
    {
        TreeDFS(Tree, 0); ///afisez arborele
        clearTree(Tree);

        cout << "stergere reusita\n";

        TreeDFS(Tree, 0);
    }
    else
    {
        cout << "input invalid\n";
        cout << errorMessage[error.first] << error.second << "\n";
    }
}

int main() {
   // RenderWindow window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "NS Diagram");

    /*
    if(!font.loadFromFile("./font.ttf")) {
        cout << "Not found file";
        exit(0);
    }*/

    Debugger();

    /*
    while(window.isOpen()) {

        pollEvents(window);
        updateWindow(window);
    }*/
    fin.close();

    return 0;
}
