#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include "diagrams.h"

using namespace std;
using namespace sf;

ifstream fin("input.txt");

#define WIDTH 1200
#define HEIGHT 900
#define SIZE 1000
const int BLOCK_WIDTH = 200, BLOCK_HEIGHT = 60;
enum instructionType {EMPTY_NODE, VAR, SET, IF, WHILE, READ, PRINT, PASS, ERROR};

struct node
{
    instructionType instruction;
    string line;

    float x, y;
    float length;
    float height;

    vector <node*> next;

};
typedef node* tree;

tree Tree;

void clearTree()
{

}

void initTree()
{
    clearTree();
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

    return ERROR;
}

void buildTree(node* currentNode)
{
    if (currentNode -> instruction == EMPTY_NODE)
    {
        node* newNode = new node;
        newNode -> line = readLineFromFile();
        newNode -> instruction = getInstructionType(newNode -> line);

        ///am legat nodurile
        (currentNode -> next).push_back(newNode);
        buildTree(newNode);
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
    RectangleShape rect(Vector2f(BLOCK_WIDTH, BLOCK_HEIGHT));
    rect.setPosition(10, 10);
    window.clear();
    window.draw(rect);

    window.display();
}

int main() {
   // RenderWindow window(VideoMode(WIDTH, HEIGHT), "NS Diagram");

    /*
    if(!font.loadFromFile("./font.ttf")) {
        cout << "Not found file";
        exit(0);
    }*/

    /*
    while(window.isOpen()) {

        pollEvents(window);
        updateWindow(window);
    }*/
    fin.close();

    return 0;
}
