#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <set>
#include <stack>
#include <unordered_map>
#include "diagrams.h"
//#include "diagrams.cpp"

using namespace std;
using namespace sf;

ifstream fin;
ofstream fout;

#define FILENAME "input.txt"
#define TEMPFILE "tempaWFtaGVyZTMK.txt"
//#define SCREEN_WIDTH 1200
//#define SCREEN_HEIGHT 900
float SCREEN_WIDTH = 1280;
float SCREEN_HEIGHT = 720;
float MINSCR_WIDTH = 1158;
float MINSCR_HEIGHT = 585;
bool sizeScreen = false;
bool diagramIprepForPress = false;
bool oneTimeResize = false;
#define BLOCK_WIDTH 300.0
#define BLOCK_HEIGHT 60.0
#define MARGIN 75
#define BLOCK_BUTTON_WIDTH 100
// interface look
// D - diagram interface
// C - code interface
// [       ]
// [ CC DD ]
// [ CC DD ]
// [       ]
// interfata diagramei
#define DIAGRAM_MARGIN_WIDTH 75
#define DIAGRAM_MARGIN_HEIGHT MARGIN
//#define DIAGRAM_WIDTH 500
//#define DIAGRAM_HEIGHT 700
float DIAGRAM_WIDTH = SCREEN_WIDTH/2-DIAGRAM_MARGIN_WIDTH*2;
float DIAGRAM_HEIGHT = SCREEN_HEIGHT-DIAGRAM_MARGIN_HEIGHT*2;
// interfata code
#define CODE_MARGIN_WIDTH 75
#define CODE_MARGIN_HEIGHT MARGIN
float CODE_WIDTH = SCREEN_WIDTH/2-CODE_MARGIN_WIDTH*2;
float CODE_HEIGHT = SCREEN_HEIGHT-CODE_MARGIN_HEIGHT*2;
string str_compiler_info;
#define BLOCK_CODE_WIDTH 16
#define BLOCK_CODE_HEIGHT 20
#define CODEEDIT_MARGIN_WIDTH 20
#define CODEEDIT_MARGIN_HEIGHT 20
int LIMIT_COLUMN_CODE = (CODE_WIDTH-CODEEDIT_MARGIN_WIDTH*2)/BLOCK_CODE_WIDTH-1;
int LIMIT_LINE_CODE = (CODE_HEIGHT-CODEEDIT_MARGIN_HEIGHT*2)/BLOCK_CODE_HEIGHT;

enum instructionType {EMPTY_NODE, VAR, SET, IF, WHILE, REPEAT, READ, PRINT, PASS, END, ERROR};
enum errorType {SYNTAX_ERROR_INSTRUCTION, SYNTAX_ERROR_VARTYPE,
    SYNTAX_ERROR_VARIABLE, SYNTAX_ERROR_INCOMPLETE_LINE, ERROR_UNDECLARED, ERROR_MULTIPLE_DECLARATION, ERROR_EXPRESSION, ERROR_INVALID_STRUCTURE}; ///de adaugat pe parcurs
enum buttonType {RUN, ABOUT, SAVE, LOAD, BACK};
enum windowType {WIN_EDITOR, WIN_ABOUT};
windowType winT = WIN_EDITOR;
string errorMessage[] =
{
    "EROARE: Instructiune invalida la linia ",
    "EROARE: Tip de date declarat incorect la linia ",
    "EROARE: Variabila incorecta la linia ",
    "EROARE: Portiune de cod incompleta la linia ",
    "EROARE: Variabila nedeclarata utilizata la linia ",
    "EROARE: Variabila declarata multiplu la linia ",
    "EROARE: Expresie incorecta aritmetic la linia ", ///de facut
    "EROARE: Structura while/if/repeat incorecta la linia "
};

// pozita interfatei diagramei
Point originIDiagram = {SCREEN_WIDTH-DIAGRAM_WIDTH-DIAGRAM_MARGIN_WIDTH, DIAGRAM_MARGIN_HEIGHT};

// pozitia initiala a diagramei
Point originDiagramP = {originIDiagram.x+DIAGRAM_WIDTH/2, originIDiagram.y+50};
Point diagramP = originDiagramP;

// pozitia interfatei code
Point originICode = {CODE_MARGIN_WIDTH, CODE_MARGIN_HEIGHT};

// pozitia initiala a codului
Point codeP = {0, 0};

// variabilele pentru zoom
float zoom = 1, zoomScale = 0.1, zoomMinScale = 0.3;

// variabilele pentru move
bool moveScreen = false;
Point moveP = {0, 0};
Point amoveP = {0, 0};

// structura butonului
struct Button {
    Point topLeft, bottomRight;
    buttonType type;
    Color colorFill, colorLine, colorOnPressFill;
    bool press, prepForPress;
    string str;

    bool mouseOnButton(float x, float y) {
        return (topLeft.x < x && x < bottomRight.x && topLeft.y < y && y < bottomRight.y);
    }
    void draw(RenderWindow &window, Font &font) {
        if(!press)
            window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
        else
            window.draw(createRect(topLeft, bottomRight, colorOnPressFill, colorLine));
        Box box;
        box.x = topLeft.x+(bottomRight.x-topLeft.x)/8;
        box.y = topLeft.y+(bottomRight.y-topLeft.y)/8;
        box.length = bottomRight.x-topLeft.x-(bottomRight.x-topLeft.x)/4;
        box.height = bottomRight.y-topLeft.y-(bottomRight.y-topLeft.y)/4;
        window.draw(createText(box, str, font));
    }
} backButton;

// butoanele
unordered_map <buttonType, Button> buttons;

// code
Point cursorCP = {0, 0};
vector <vector <char>> codeEdit;

void createAllButtons() {
    // RUN
    buttons[RUN].topLeft = {originIDiagram.x + DIAGRAM_WIDTH - BLOCK_BUTTON_WIDTH, SCREEN_HEIGHT-(MARGIN/4)*3};
    buttons[RUN].bottomRight = {originIDiagram.x + DIAGRAM_WIDTH, SCREEN_HEIGHT-MARGIN/4};
    buttons[RUN].type = RUN;
    buttons[RUN].colorFill = Color(28, 28, 28);
    buttons[RUN].colorLine = Color(255, 0, 0);
    buttons[RUN].colorOnPressFill = Color(0, 255, 0);
    buttons[RUN].press = false;
    buttons[RUN].prepForPress = false;
    buttons[RUN].str = "RUN";

    // SAVE
    buttons[SAVE].topLeft = {originICode.x+50, MARGIN/4};
    buttons[SAVE].bottomRight = {originICode.x+50+BLOCK_BUTTON_WIDTH, (MARGIN/4)*3};
    buttons[SAVE].type = SAVE;
    buttons[SAVE].colorFill = Color(28, 28, 28);
    buttons[SAVE].colorLine = Color(255, 0, 0);
    buttons[SAVE].colorOnPressFill = Color(0, 255, 0);
    buttons[SAVE].press = false;
    buttons[SAVE].prepForPress = false;
    buttons[SAVE].str = "SAVE";

    // LOAD
    buttons[LOAD].topLeft = {originICode.x+buttons[SAVE].bottomRight.x-buttons[SAVE].topLeft.x+100, MARGIN/4};
    buttons[LOAD].bottomRight = {originICode.x+buttons[SAVE].bottomRight.x-buttons[SAVE].topLeft.x+100+BLOCK_BUTTON_WIDTH, (MARGIN/4)*3};
    buttons[LOAD].type = LOAD;
    buttons[LOAD].colorFill = Color(28, 28, 28);
    buttons[LOAD].colorLine = Color(255, 0, 0);
    buttons[LOAD].colorOnPressFill = Color(0, 255, 0);
    buttons[LOAD].press = false;
    buttons[LOAD].prepForPress = false;
    buttons[LOAD].str = "LOAD";

    // ABOUT
    buttons[ABOUT].topLeft = {originICode.x+buttons[LOAD].bottomRight.x-buttons[LOAD].topLeft.x+250, MARGIN/4};
    buttons[ABOUT].bottomRight = {originICode.x+buttons[LOAD].bottomRight.x-buttons[LOAD].topLeft.x+250+BLOCK_BUTTON_WIDTH, (MARGIN/4)*3};
    buttons[ABOUT].type = ABOUT;
    buttons[ABOUT].colorFill = Color(28, 28, 28);
    buttons[ABOUT].colorLine = Color(255, 0, 0);
    buttons[ABOUT].colorOnPressFill = Color(0, 255, 0);
    buttons[ABOUT].press = false;
    buttons[ABOUT].prepForPress = false;
    buttons[ABOUT].str = "ABOUT";

    // BACK
    backButton.topLeft = {originIDiagram.x+DIAGRAM_WIDTH-BLOCK_BUTTON_WIDTH, SCREEN_HEIGHT-(MARGIN/4)*3};
    backButton.bottomRight = {originIDiagram.x+DIAGRAM_WIDTH, SCREEN_HEIGHT-MARGIN/4};
    backButton.type = BACK;
    backButton.colorFill = Color(28, 28, 28);
    backButton.colorLine = Color(255, 0, 0);
    backButton.colorOnPressFill = Color(0, 255, 0);
    backButton.press = false;
    backButton.prepForPress = false;
    backButton.str = "BACK";
}

///verificarea erorilor in arbore, definite de functia checkErrors_DFS()
bool validTree = true;
pair <errorType, int> error;
set <char> declaredGlobal;
stack < set <char> > declaredLocal;
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

// initializarea arborelui
void initTree()
{
    clearTree(Tree);
    Tree = new node;
    Tree -> instruction = EMPTY_NODE;

    Tree -> length = BLOCK_WIDTH * zoom; ///latimea standard a unui bloc
    Tree -> height = BLOCK_HEIGHT * zoom; ///inaltimea standard a unui bloc
    //Tree -> x = diagramP.x; ///coordonata x de inceput (stanga sus)
    Tree -> x = diagramP.x-(BLOCK_WIDTH*zoom)/2; //coorodnata x de inceput (mijloc sus) (ca sa fie zoom ul mai frumos)
    Tree -> y = diagramP.y; ///coordonata y de inceput stanga sus
}

// citirea din fisier line by line
string readLineFromFile()
{
    string str;
    getline(fin, str);
    return str;
}

// imparte un sir in cuvinte
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

// int to string
string intToString(int value) {
    string str = "";
    if(value == 0)
        str = "0";
    int p = 1, auxValue = value;
    while(auxValue) {
        p *= 10;
        auxValue /= 10;
    }
    p /= 10;
    while(p) {
        str += (char)((value/p)%10+'0');
        p /= 10;
    }
    return str;
}

// clear vector<vector<char>>
void clearCodeMemory() {
    for(int i = 0; i < (int)codeEdit.size(); i++)
        codeEdit[i].clear();
    codeEdit.clear();
}

// filename to vector<vector<char>>
void getDataFromFile(string filename) {
    clearCodeMemory();
    fin.open(filename);
    while(!fin.eof()) {
        string str = readLineFromFile();

        if (!str.empty())
        {
            codeEdit.push_back(vector<char>());
            for(int i = 0 ; i < (int)str.size(); i++)
                codeEdit[codeEdit.size()-1].push_back(str[i]);
        }
    }

    if (codeEdit.empty())
        codeEdit.push_back(vector<char>());
    fin.close();
}

// vector<vector<char>> to filename
void setDataToFile(string filename) {
    fout.open(filename);

    ///daca nu am o linie libera la final, o adaug (este necesara pt executarea codului)
    ///nu mai sunt nevoit astfel sa dau enter manual din program la finalul codului
    if (codeEdit[codeEdit.size() - 1].size() > 0)
        codeEdit.push_back(vector<char>());

    for(int line = 0; line < (int)codeEdit.size(); line++) {
        for(int column = 0; column < (int)codeEdit[line].size(); column++) {
            fout << codeEdit[line][column];
        }
        if(line != (int)codeEdit.size()-1)
            fout << '\n';
    }
    fout.close();
}

// optine instruction type
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
    else if (word == "repeat")
        return REPEAT;
    else if (word == "pass")
        return PASS;
    else if (word == "read")
        return READ;
    else if (word == "print")
        return PRINT;
    else if (word == "else" || word == "endif" || word == "endwhile" || word == "until")
        return END;

    return ERROR;
}

// construirea arborelui
int lineCount = 0;
string untilExpr = ""; ///trebuie salvata expresia din repeat until pe revenirea din recursivitate
void buildTree(node* &currentNode)
{
    //static int lineCount = 0; ///linia de cod la care ma aflu
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
                    ///daca am "until" atunci trebuie sa am exact 2 cuvinte
                    if ((newNode -> words[0] != "until" && newNode -> words.size() != 1)  || (newNode -> words[0] == "until" && newNode -> words.size() != 2))
                    {
                        validTree = false;
                        error = make_pair(SYNTAX_ERROR_INCOMPLETE_LINE, newNode -> lineOfCode);
                        delete newNode; ///nu am avut nevoie de nod
                        return;
                    }

                    if (newNode -> words[0] == "until") ///salvez expresia din until si o retin in nodul repeat (pt repeat ... until expresie)
                        untilExpr = newNode -> words[1];

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
    else if (currentNode -> instruction == REPEAT)
    {
        node* newNode = new node;
        newNode -> instruction = EMPTY_NODE;
        (currentNode -> next).push_back(newNode);

        buildTree(newNode);

        ///salvez expresia din until in nodul repeat
        if (untilExpr != "")
        {
            currentNode -> line += " ";
            currentNode -> line += untilExpr;
            currentNode -> words.push_back(untilExpr);
        }
        untilExpr = "";

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
///mai sunt de tratat cazurile cu expresii gresite
void checkErrors_DFS(node* currentNode)
{
    if (currentNode != NULL && validTree)
    {
        if (currentNode -> instruction == EMPTY_NODE)
            declaredLocal.push(set<char>());
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
                error = make_pair(SYNTAX_ERROR_INCOMPLETE_LINE, currentNode -> lineOfCode);
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
                    declaredLocal.top().insert(ch);
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
                error = make_pair(SYNTAX_ERROR_INCOMPLETE_LINE, currentNode -> lineOfCode);
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
                error = make_pair(SYNTAX_ERROR_INCOMPLETE_LINE, currentNode -> lineOfCode);
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
                error = make_pair(SYNTAX_ERROR_INCOMPLETE_LINE, currentNode -> lineOfCode);
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
                error = make_pair(SYNTAX_ERROR_INCOMPLETE_LINE, currentNode -> lineOfCode);
                return;
            }
        }
        else if (currentNode -> instruction == IF || currentNode -> instruction == WHILE)
        {
            if (currentNode -> words.size() != 2)
            {
                validTree = false;
                error = make_pair(SYNTAX_ERROR_INCOMPLETE_LINE, currentNode -> lineOfCode);
                return;
            }
            else
            {
                /*
                    de verificat daca expresia este valida (currentNode -> words[1])
                */
            }
        }
        else if (currentNode -> instruction == REPEAT)
        {
            if (currentNode -> words.size() > 2)
            {
                validTree = false;
                error = make_pair(SYNTAX_ERROR_INCOMPLETE_LINE, currentNode -> lineOfCode);
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
            for (char ch : declaredLocal.top())
                declaredGlobal.erase(ch);
            declaredLocal.pop();
        }
    }
}

///trateaza cazurile in care pun gresit endif, else, endwhile, etc.
///pt fiecare while trebuie sa am un endwhile, iar pt fiecare if un else si dupa else un endif
///tratez problema exact la fel ca cea a parantezarii unei expresii
void checkStructure()
{
    stack <char> st;
    int line = 0;
    while (!fin.eof())
    {
        line++;
        string str = readLineFromFile();
        if (!str.empty())
        {
            string instruction = splitIntoWords(str)[0];

            if (instruction == "if")
            {
                st.push('[');
                st.push('(');
            }
            else if (instruction == "else")
            {
                if (st.empty() || st.top() != '(')
                {
                    validTree = false;
                    error = make_pair(ERROR_INVALID_STRUCTURE, line);
                    return;
                }
                else
                    st.pop();

            }
            else if (instruction == "endif")
            {
                if (st.empty() || st.top() != '[')
                {
                    validTree = false;
                    error = make_pair(ERROR_INVALID_STRUCTURE, line);
                    return;
                }
                else
                    st.pop();
            }
            else if (instruction == "while")
            {
                st.push('{');
            }
            else if (instruction == "endwhile")
            {
                if (st.empty() || st.top() != '{')
                {
                    validTree = false;
                    error = make_pair(ERROR_INVALID_STRUCTURE, line);
                    return;
                }
                else
                    st.pop();
            }
            else if (instruction == "repeat")
            {
                st.push('+');
            }
            else if (instruction == "until")
            {
                if (st.empty() || st.top() != '+')
                {
                    validTree = false;
                    error = make_pair(ERROR_INVALID_STRUCTURE, line);
                    return;
                }
                else
                    st.pop();
            }
        }
    }

    if (!st.empty())
    {
        validTree = false;
        error = make_pair(ERROR_INVALID_STRUCTURE, line);
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
        else if (currentNode -> instruction == WHILE || currentNode -> instruction == REPEAT)
            currentNode -> verticalNodeCount = 1 + currentNode -> next[0] -> verticalNodeCount;
        else if (currentNode -> instruction != EMPTY_NODE)
            currentNode -> verticalNodeCount = 1;

    }
}

// pregatire pentru desenarea diagramei
void buildDiagram_DFS(node* &currentNode, node* &emptyFather)
{
    if (currentNode != NULL)
    {
        ///creez dimensiunile blocului curent in functie de nodul tata
        if (currentNode -> instruction != EMPTY_NODE)
        {
            currentNode -> length = emptyFather -> length;

            if (currentNode -> instruction != REPEAT)
            {
                currentNode -> x = emptyFather -> x;
                currentNode -> y = emptyFather -> y;

                emptyFather -> y += emptyFather -> height;
            }

            if (currentNode -> instruction != WHILE && currentNode -> instruction != REPEAT) ///bloc de tip dreptunghi
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
        else if (currentNode -> instruction == WHILE || currentNode -> instruction == REPEAT)
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

        if (currentNode -> instruction == REPEAT)
        {
            currentNode -> x = emptyFather -> x;
            currentNode -> y = emptyFather -> y;

            emptyFather -> y += emptyFather -> height;
        }
    }
}

//buildDiagram_DFS is not working if i rerun the function with anothers variables for Tree -> x, y, length, height
//i created a function that can clear the position
void deletePositionDiagram_DFS(node* currentNode) {
    if(currentNode != NULL) {
        currentNode -> x = 0;
        currentNode -> y = 0;
        currentNode -> length = 0;
        currentNode -> height = 0;

        for(node* nextNode:currentNode -> next)
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
            box.y = topLeft.y+5;
            box.length = (bottomRight.x-topLeft.x)/2;
            box.height = (bottomRight.y-topLeft.y)/2;

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
            box.height = (bottomRight.y-topLeft.y)/2;

            window.draw(createText(box, str, font));

            str = "F";
            box.x = bottomRight.x-(bottomRight.x-topLeft.x)/4;
            box.y = topLeft.y+(bottomRight.y-topLeft.y)/2;
            box.length = (bottomRight.x-topLeft.x)/4;
            box.height = (bottomRight.y-topLeft.y)/2;

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
            box.x = topLeft.x+offset;
            box.y = topLeft.y;
            box.length = bottomRight.x-box.x;
            box.height = rectangleHeight;

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
        else if (currentNode -> instruction == REPEAT)
        {
            float offset = currentNode -> length / 5.; ///lungimea pentru bara din stanga
            float rectangleHeight = currentNode -> height / currentNode -> verticalNodeCount; ///inaltimea blocului fara bara din stanga
            Point bottomRight;
            bottomRight.x = currentNode -> x + currentNode -> length;
            bottomRight.y = currentNode -> y + rectangleHeight;
            Point topLeft;
            topLeft.x = currentNode -> x;
            topLeft.y = bottomRight.y - currentNode -> height;

            Box box;
            box.x = topLeft.x+offset;
            box.y = currentNode -> y;
            box.length = bottomRight.x-box.x;
            box.height = rectangleHeight;

            string str = "";
            for(int i = 0; i < (int)currentNode -> words.size(); i++) {
                str += currentNode -> words[i];
                if (i != (int)currentNode -> words.size() - 1)
                    str += ' ';
            }

            // desenare text pentru repeat until
            window.draw(createText(box, str, font));

            // desenarea blocului pentru repeat until
            window.draw(iterationUCreate(topLeft, bottomRight, offset, rectangleHeight));

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
            box.height = bottomRight.y-topLeft.y;

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

// desenarea codului
void printCodeEdit(RenderWindow &window) {
    Box box;
    string str;
    int k = 1, nr = 0, p = 10, aux = codeP.y+1;
    while(aux) {
        nr++;
        aux /= 10;
    }
    aux = nr;
    while(aux) {
        if(aux%2 == 1)
            aux--, k *= p;
        else
            aux /= 2, p = p*p;
    }
    codeP = {0, 0};
    if(cursorCP.x > LIMIT_COLUMN_CODE)
        codeP.x = cursorCP.x-LIMIT_COLUMN_CODE;
    if(cursorCP.y > LIMIT_LINE_CODE-1)
        codeP.y = cursorCP.y-LIMIT_LINE_CODE+1;
    for(int line = 0, aLine = codeP.y; aLine < (int)codeEdit.size() && line < (int)codeEdit.size() && line < LIMIT_LINE_CODE; line++, aLine++) {
        box.x = originICode.x+CODEEDIT_MARGIN_WIDTH-(nr-1)*BLOCK_CODE_WIDTH;
        box.y = originICode.y+line*BLOCK_CODE_HEIGHT+CODEEDIT_MARGIN_HEIGHT;
        box.length = nr*BLOCK_CODE_WIDTH;
        box.height = BLOCK_CODE_HEIGHT;
        str = intToString(aLine+1);
        window.draw(createTextForCode(box, str, font));
        if(aLine == k-2)
            k *= 10, nr++;
        for(int column = 0, aColumn = codeP.x; aColumn < (int)codeEdit[aLine].size() && column < (int)codeEdit[aLine].size() && column < LIMIT_COLUMN_CODE; column++, aColumn++) {
            box.x = originICode.x+(column+1)*BLOCK_CODE_WIDTH+CODEEDIT_MARGIN_WIDTH;
            box.y = originICode.y+line*BLOCK_CODE_HEIGHT+CODEEDIT_MARGIN_HEIGHT;
            box.length = BLOCK_CODE_WIDTH;
            box.height = BLOCK_CODE_HEIGHT;
            str = codeEdit[aLine][aColumn];
            window.draw(createTextForCode(box, str, font));
        }
    }
}

// desenarea margenilor
void interfaceDraw(RenderWindow &window) {
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
    bottomRight = {originICode.x, SCREEN_HEIGHT};
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
    // middle
    topLeft = {originICode.x+CODE_WIDTH, MARGIN+1};
    bottomRight = {originIDiagram.x, SCREEN_HEIGHT-MARGIN};
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
    // jos
    topLeft = {0, originIDiagram.y+DIAGRAM_HEIGHT};
    bottomRight = {SCREEN_WIDTH, SCREEN_HEIGHT};
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
    // border diagram
    topLeft = {originIDiagram.x+2, originIDiagram.y+2};
    bottomRight = {topLeft.x+DIAGRAM_WIDTH-4, topLeft.y+DIAGRAM_HEIGHT-4};
    colorFill = Color(0, 0, 0, 0);
    colorLine = Color(255, 0, 0);
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
    // border code
    topLeft = {originICode.x+2, originICode.y+2};
    bottomRight = {topLeft.x+CODE_WIDTH-4, topLeft.y+CODE_HEIGHT-4};
    colorFill = Color(0, 0, 0, 0);
    colorLine = Color(255, 0, 0);
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
    // border compiler
    topLeft = {originIDiagram.x, SCREEN_HEIGHT-(MARGIN/4)*3};
    bottomRight = {originIDiagram.x+DIAGRAM_WIDTH-BLOCK_BUTTON_WIDTH, SCREEN_HEIGHT-MARGIN/4};
    colorFill = Color(0, 0, 0, 0);
    colorLine = Color(255, 0, 0);
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
}

// background color for diagramCut
void backgroundDiagramDraw(RenderWindow &window) {
    Point topLeft = originIDiagram;
    Point bottomRight = {topLeft.x+DIAGRAM_WIDTH, topLeft.y+DIAGRAM_HEIGHT};
    Color colorFill = Color(14, 10, 10);
    Color colorLine = Color(0, 0, 0, 0);
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
}

// background color for codeCut
void backgroundCodeDraw(RenderWindow &window) {
    Point topLeft = originICode;
    Point bottomRight = {topLeft.x+CODE_WIDTH, topLeft.y+CODE_HEIGHT};
    Color colorFill = Color(14, 10, 10);
    Color colorLine = Color(0, 0, 0, 0);
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
}

// background color for compilerCut
void backgroundCompilerDraw(RenderWindow &window) {
    Point topLeft = {originIDiagram.x, SCREEN_HEIGHT-(MARGIN/4)*3};
    Point bottomRight = {originIDiagram.x+DIAGRAM_WIDTH-BLOCK_BUTTON_WIDTH, SCREEN_HEIGHT-MARGIN/4};
    Color colorFill = Color(14, 10, 10);
    Color colorLine = Color(0, 0, 0, 0);
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
}

// line draw
void lineDraw(Point startP, Point stopP, Color color, RenderWindow &window) {
    VertexArray line(LineStrip, 2);
    line[0].position = Vector2f(startP.x, startP.y);
    line[1].position = Vector2f(stopP.x, stopP.y);
    line[0].color = color;
    line[1].color = color;
    window.draw(line);
}

// cursor draw
void cursorDraw(RenderWindow &window) {
    Point startP, stopP;
    Point aCursorCP = cursorCP;
    if(cursorCP.x > LIMIT_COLUMN_CODE)
        aCursorCP.x = LIMIT_COLUMN_CODE;
    if(cursorCP.y > LIMIT_LINE_CODE-1)
        aCursorCP.y = LIMIT_LINE_CODE-1;
    startP.x = originICode.x+(aCursorCP.x+1)*BLOCK_CODE_WIDTH+CODEEDIT_MARGIN_WIDTH;
    startP.y = originICode.y+aCursorCP.y*BLOCK_CODE_HEIGHT+CODEEDIT_MARGIN_HEIGHT;
    stopP.x = startP.x;
    stopP.y = startP.y+BLOCK_CODE_HEIGHT;
    lineDraw(startP, stopP, Color(255, 0, 0), window);
}

// parte din mecanismul pentru butoane
void activateButton(Button button) {
    if(button.type == RUN) {
        cout << "you pressed RUN\n";
        validTree = true;
        lineCount = 0;
        declaredGlobal.clear();
        while (!declaredLocal.empty())
            declaredLocal.pop();
        initTree();

        setDataToFile(TEMPFILE);
        fin.open(TEMPFILE);
        buildTree(Tree);

        checkErrors_DFS(Tree);

        if (validTree) ///daca arborele e corect mai am de verificat structura programului (erori pt endif, endwhile, else, etc)
        {
            ///merg inapoi la inceputul fisierului
            fin.clear();
            fin.seekg(0);

            checkStructure();
        }

        fin.close();
        remove(TEMPFILE);

        if(validTree) {
            buildDP_DFS(Tree);
            buildDiagram_DFS(Tree, Tree);
            str_compiler_info = "Cod executat cu succes!";
          //  TreeDFS(Tree, 0); ///afisez arborele
          //  clearTree(Tree);

          //  cout << "stergere reusita\n";

          //  TreeDFS(Tree, 0);
        }
        else {
            str_compiler_info = errorMessage[error.first]+intToString(error.second);
            cout << "input invalid\n";
            cout << errorMessage[error.first] << error.second << "\n";

        }
    }
    else if(button.type == ABOUT) {
        cout << "you pressed ABOUT\n";
        winT = WIN_ABOUT;
    }
    else if(button.type == SAVE) {
        cout << "you pressed SAVE\n";
        setDataToFile(FILENAME);
    }
    else if(button.type == LOAD) {
        cout << "you pressed LOAD\n";
        getDataFromFile(FILENAME);
        cursorCP = {0, 0};
      //  codeEdit.push_back(vector<char>()); ///am mutat linia asta in setDataToFile !!!
    }
    else if(button.type == BACK) {
        cout << "you pressed BACK\n";
        winT = WIN_EDITOR;
    }
}

// mecanismul pentru butoane
void buttonsMechanics(RenderWindow &window) {
    Vector2i positionMouse = Mouse::getPosition(window);
    if(winT == WIN_EDITOR) {
        for(auto& it: buttons) {
            if(it.second.mouseOnButton(positionMouse.x, positionMouse.y)) {
                if(!it.second.prepForPress && !Mouse::isButtonPressed(Mouse::Left)) {
                    it.second.prepForPress = true;
                }
                else if(it.second.prepForPress && Mouse::isButtonPressed(Mouse::Left))
                    it.second.press = true;
                else if(it.second.press) {
                    it.second.prepForPress = false;
                    it.second.press = false;
                    activateButton(it.second);
                }
            }
            else {
                it.second.prepForPress = false;
                it.second.press = false;
            }
        }
    }
    if(winT == WIN_ABOUT) {
        if(backButton.mouseOnButton(positionMouse.x, positionMouse.y)) {
            if(!backButton.prepForPress && !Mouse::isButtonPressed(Mouse::Left)) {
                backButton.prepForPress = true;
            }
            else if(backButton.prepForPress && Mouse::isButtonPressed(Mouse::Left))
                backButton.press = true;
            else if(backButton.press) {
                backButton.prepForPress = false;
                backButton.press = false;
                activateButton(backButton);
            }
        }
        else {
            backButton.prepForPress = false;
            backButton.press = false;
        }
    }
}

// mecanismul pentru redimensionare
void resizeMechanics(RenderWindow &window) {
    View view = window.getView();
    bool correctWidth = true;
    bool correctHeight = true;
    if(window.getSize().x < MINSCR_WIDTH)
        correctWidth = false;
    if(window.getSize().y < MINSCR_HEIGHT)
        correctHeight = false;
    if(!oneTimeResize && (sizeScreen || (correctWidth && correctHeight))) {
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
        DIAGRAM_WIDTH = SCREEN_WIDTH/2-DIAGRAM_MARGIN_WIDTH*2;
        DIAGRAM_HEIGHT = SCREEN_HEIGHT-DIAGRAM_MARGIN_HEIGHT*2;
        CODE_WIDTH = SCREEN_WIDTH/2-CODE_MARGIN_WIDTH*2;
        CODE_HEIGHT = SCREEN_HEIGHT-CODE_MARGIN_HEIGHT*2;
        LIMIT_COLUMN_CODE = (CODE_WIDTH-CODEEDIT_MARGIN_WIDTH*2)/BLOCK_CODE_WIDTH-1;
        LIMIT_LINE_CODE = (CODE_HEIGHT-CODEEDIT_MARGIN_HEIGHT*2)/BLOCK_CODE_HEIGHT;
        //if((int)SCREEN_HEIGHT%2 == 1)
        //    SCREEN_HEIGHT += 1;
        createAllButtons();
        view.setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
        view.setCenter(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    }
    window.setView(view);
}

// mecanismul pentru zoom
void zoomMechanics() {
    deletePositionDiagram_DFS(Tree);
    Tree -> x = diagramP.x-(BLOCK_WIDTH*zoom)/2;
    Tree -> y = diagramP.y;
    Tree -> length = BLOCK_WIDTH * zoom;
    Tree -> height = BLOCK_HEIGHT * zoom;
    buildDiagram_DFS(Tree, Tree);
}

// mecanismul pentru miscarea diagramei
void moveMechanics(int direction, RenderWindow &window) {
    Vector2i positionMouse = Mouse::getPosition(window);
    if(originIDiagram.x < positionMouse.x && positionMouse.x < originIDiagram.x+DIAGRAM_WIDTH &&
       originIDiagram.y < positionMouse.y && positionMouse.y < originIDiagram.y+DIAGRAM_HEIGHT && !Mouse::isButtonPressed(Mouse::Left))
        diagramIprepForPress = true;
    if(diagramIprepForPress) {
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
                    diagramIprepForPress = false;
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
                    diagramIprepForPress = false;
                    amoveP = {0, 0};
                    moveP = {0, 0};
                }
            }
            else
                diagramIprepForPress = false;
        }
    }
}

// events
void pollEvents(RenderWindow &window) {
    bool resizedEvent = false;
    Event event;
    while(window.pollEvent(event)) {
        if(event.type == Event::Closed)
            window.close();

        // exit
        if(event.type == Event::KeyPressed) {
            if(event.key.code == Keyboard::Q && Keyboard::isKeyPressed(Keyboard::LControl))
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

        // RUN button action on keyboard
        if(event.type == Event::KeyPressed) {
            if(event.key.code == Keyboard::R && (Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl))) {
                activateButton(buttons[RUN]);
            }
        }
        // SAVE button action on keyboard
        if(event.type == Event::KeyPressed) {
            if(event.key.code == Keyboard::S && (Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl))) {
                activateButton(buttons[SAVE]);
            }
        }
        // LOAD button action on keyboard
        if(event.type == Event::KeyPressed) {
            if(event.key.code == Keyboard::L && (Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl))) {
                activateButton(buttons[LOAD]);
            }
        }

        // pozitia initiala si zoom ul initial
        if(event.type == Event::KeyPressed) {
            if(event.key.code == Keyboard::O && (Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl))) {
                diagramP = originDiagramP;
                zoom = 1;
            }
        }

        //dau clear la editor
        if(event.type == Event::KeyPressed) {
            if(event.key.code == Keyboard::D && (Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl))) {
                clearCodeMemory();
                codeEdit.push_back(vector<char>());
                cursorCP = {0, 0};
            }
        }

        // move cursor with arrow
        if(event.type == Event::KeyPressed) {
            if(event.key.code == Keyboard::Up && cursorCP.y > 0) {
                cursorCP.y -= 1;
                if(codeEdit.size() != 0 && cursorCP.x > codeEdit[cursorCP.y].size())
                    cursorCP.x = codeEdit[cursorCP.y].size();
            }
            if(event.key.code == Keyboard::Right && codeEdit.size() != 0 && cursorCP.x < codeEdit[cursorCP.y].size())
                cursorCP.x += 1;
            if(event.key.code == Keyboard::Left && cursorCP.x > 0)
                cursorCP.x -= 1;
            if(event.key.code == Keyboard::Down && cursorCP.y < (int)codeEdit.size()-1) {
                cursorCP.y += 1;
                if(codeEdit.size() != 0 && cursorCP.x > codeEdit[cursorCP.y].size())
                    cursorCP.x = codeEdit[cursorCP.y].size();
            }
        }

        if(event.type == sf::Event::TextEntered) {
            if(32 <= event.text.unicode && event.text.unicode <= 126) {
                codeEdit[cursorCP.y].insert(codeEdit[cursorCP.y].begin()+cursorCP.x, static_cast<char>(event.text.unicode));
                cursorCP.x += 1;
            }
            else if(event.text.unicode == 13) { // enter
                codeEdit.insert(codeEdit.begin()+cursorCP.y+1, vector<char>());
                codeEdit[cursorCP.y+1].insert(codeEdit[cursorCP.y+1].begin(), codeEdit[cursorCP.y].begin()+cursorCP.x, codeEdit[cursorCP.y].end());
                codeEdit[cursorCP.y].erase(codeEdit[cursorCP.y].begin()+cursorCP.x, codeEdit[cursorCP.y].end());
                cursorCP.x = 0;
                cursorCP.y += 1;
            }
            else if(event.text.unicode == 8) { //backspace
                if(cursorCP.x == 0) {
                    if(cursorCP.y != 0) {
                        cursorCP.x = codeEdit[cursorCP.y-1].size();
                        codeEdit[cursorCP.y-1].insert(codeEdit[cursorCP.y-1].end(), codeEdit[cursorCP.y].begin(), codeEdit[cursorCP.y].end());
                        codeEdit[cursorCP.y].clear();
                        codeEdit.erase(codeEdit.begin()+cursorCP.y);
                        cursorCP.y -= 1;
                    }
                }
                else {
                    codeEdit[cursorCP.y].erase(codeEdit[cursorCP.y].begin()+cursorCP.x-1);
                    cursorCP.x -= 1;
                }
            }
        }

        // resize
        if(event.type == Event::Resized)
            resizedEvent = true;
    }
    if(resizedEvent)
        oneTimeResize = false;
    if(sizeScreen) {
        oneTimeResize = true;
        sizeScreen = false;
    }
    if(!oneTimeResize && !resizedEvent && !Mouse::isButtonPressed(Mouse::Left))
        sizeScreen = true;
}

// actualizarea ferestrei
void updateWindow(RenderWindow &window)
{
    window.clear();

    backgroundDiagramDraw(window);
    if(Tree != NULL && validTree)
        printDiagram_DFS(Tree, window);
    backgroundCodeDraw(window);

    // afisarea codului
    printCodeEdit(window);

    // draw a line
    lineDraw({originICode.x+CODEEDIT_MARGIN_WIDTH+BLOCK_CODE_WIDTH, originICode.y+CODEEDIT_MARGIN_HEIGHT}, \
             {originICode.x+CODEEDIT_MARGIN_WIDTH+BLOCK_CODE_WIDTH, originICode.y+CODE_HEIGHT-CODEEDIT_MARGIN_HEIGHT}, Color(255, 255, 255), window);

    // afisarea cursorului
    cursorDraw(window);

    interfaceDraw(window);

    // afisarea rezultatului sau syntax error
    backgroundCompilerDraw(window);

    // afisare butoane
    for(auto& it: buttons)
        it.second.draw(window, font);

    // afisare compiler info
    Box box;
    box.x = originIDiagram.x;
    box.y = SCREEN_HEIGHT-(MARGIN/4)*3;
    box.length = DIAGRAM_WIDTH-BLOCK_BUTTON_WIDTH;
    box.height = SCREEN_HEIGHT-MARGIN/4-box.y;
    if(str_compiler_info == "")
        window.draw(createText(box, " ", font));
    else
        window.draw(createText(box, str_compiler_info, font));

    window.display();
}

void interfaceDrawABOUT(RenderWindow &window) {
    Point topLeft, bottomRight;
    Color colorFill(20, 20,20), colorLine(255, 255, 255);

    topLeft = {0, 0};
    bottomRight = {SCREEN_WIDTH, SCREEN_HEIGHT};
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));

    // border ABOUT interface
    topLeft.x = CODE_MARGIN_WIDTH;
    topLeft.y = CODE_MARGIN_HEIGHT;
    bottomRight.x = SCREEN_WIDTH-DIAGRAM_MARGIN_WIDTH;
    bottomRight.y = SCREEN_HEIGHT-MARGIN;
    colorFill = Color(0, 0, 0, 0);
    colorLine = Color(255, 0, 0);
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
}

// background color for ABOUT interface
void backgroundABOUTDraw(RenderWindow &window) {
    Point topLeft, bottomRight;
    topLeft.x = CODE_MARGIN_WIDTH;
    topLeft.y = CODE_MARGIN_HEIGHT;
    bottomRight.x = SCREEN_WIDTH-DIAGRAM_MARGIN_WIDTH;
    bottomRight.y = SCREEN_HEIGHT-MARGIN;
    Color colorFill = Color(14, 10, 10);
    Color colorLine = Color(0, 0, 0, 0);
    window.draw(createRect(topLeft, bottomRight, colorFill, colorLine));
}

void textDrawABOUT(RenderWindow &window) {
    Box box;
    box.x = CODE_MARGIN_WIDTH;
    box.y = CODE_MARGIN_HEIGHT;
    box.length = SCREEN_WIDTH-DIAGRAM_MARGIN_WIDTH-box.x;
    box.height = SCREEN_HEIGHT-MARGIN-box.y;
    string str = "Team: Laza Gabriel si Aliciuc Alexandru";
    window.draw(createText(box, str, font));
}

void pollEventsABOUT(RenderWindow &window) {
    bool resizedEvent = false;
    Event event;
    while(window.pollEvent(event)) {
        if(event.type == Event::Closed)
            window.close();
        if(event.type == Event::KeyPressed) {
            if(event.key.code == Keyboard::B && (Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl)))
                winT = WIN_EDITOR;
        }

        // resize
        if(event.type == Event::Resized)
            resizedEvent = true;
    }
    if(resizedEvent)
        oneTimeResize = false;
    if(sizeScreen) {
        oneTimeResize = true;
        sizeScreen = false;
    }
    if(!oneTimeResize && !resizedEvent && !Mouse::isButtonPressed(Mouse::Left))
        sizeScreen = true;
}

void updateWindowABOUT(RenderWindow &window) {
    window.clear();
    interfaceDrawABOUT(window);
    backgroundABOUTDraw(window);
    textDrawABOUT(window);
    backButton.draw(window, font);
    window.display();
}

//void Debugger()
//{
//    initTree();
//    buildTree(Tree);
//    checkErrors_DFS(Tree);
//
//    if (validTree)
//    {
//        buildDP_DFS(Tree);
//        buildDiagram_DFS(Tree, Tree);
//      //  TreeDFS(Tree, 0); ///afisez arborele
//      //  clearTree(Tree);
//
//      //  cout << "stergere reusita\n";
//
//      //  TreeDFS(Tree, 0);
//    }
//    else
//    {
//        cout << "input invalid\n";
//        cout << errorMessage[error.first] << error.second << "\n";
//    }
//}

int main() {
    RenderWindow window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "NS Diagram");
    window.setFramerateLimit(90);


    if(!font.loadFromFile("./font.ttf")) {
        cout << "Not found file";
        exit(0);
    }
    createAllButtons();
    codeEdit.push_back(vector<char>());

    //Debugger();

    while(window.isOpen()) {
        if(winT == WIN_EDITOR) {
            pollEvents(window);
            buttonsMechanics(window);
            resizeMechanics(window);
            if(Tree != NULL && validTree) {
                moveMechanics(1, window);
                zoomMechanics();
            }
            updateWindow(window);
            if(Tree != NULL && validTree) {
                moveMechanics(-1, window);
            }
        }
        else if(winT == WIN_ABOUT) {
            pollEventsABOUT(window);
            buttonsMechanics(window);
            resizeMechanics(window);
            updateWindowABOUT(window);
        }
    }

    return 0;
}
