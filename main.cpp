#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include "diagrams.h"

using namespace std;
using namespace sf;

ifstream fin("input.txt");

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900
#define SIZE 1000
const int BLOCK_WIDTH = 200, BLOCK_HEIGHT = 60;
enum instructionType {EMPTY_NODE, VAR, SET, IF, WHILE, READ, PRINT, PASS, END, ERROR};

struct node
{
    instructionType instruction;
    string line;

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

instructionType getInstructionType(string str)
{
    ///returneaza primul cuvant
    string word;

    for (unsigned i = 0; i < str.size() && str[i] != ' '; i++)
        word.push_back(str[i]);

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
    if (currentNode -> instruction == EMPTY_NODE)
    {
        bool exitWhile = false;
        while (!exitWhile)
        {
            string lineStr = readLineFromFile();

            if (fin.eof()) ///am ajuns la finalul fisierului
                exitWhile = true;
            else
            {
                node* newNode = new node;
                newNode -> line = lineStr;
                newNode -> instruction = getInstructionType(lineStr);

                ///daca intalnesc else, endif sau endwhile trebuie sa ma intorc in sus in arbore
                if (newNode -> instruction == END)
                {
                    delete newNode; ///nu am avut nevoie de nod
                    exitWhile = true; ///ies din while chiar daca nu termin de citit fisierul
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
        cout << "inaltime: " << height << "\n";
        cout << currentNode -> instruction << " ";
        if (currentNode -> instruction != EMPTY_NODE)
            cout << currentNode -> line << "\n";
        else
            cout << "\n";

        for (node* nextNode : currentNode -> next)
            TreeDFS(nextNode, height + 1);
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

    TreeDFS(Tree, 0); ///afisez arborele
    clearTree(Tree);

    cout << "stergere reusita\n";

    TreeDFS(Tree, 0);
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
