# Editor de cod Nassi-Shneiderman

  In programarea computerelor, o diagrama Nassi-Shneiderman (NSD) este o metoda de reprezentare a unui flowchart, o reprezentare grafica de proiectare pentru programarea structurata.

  Proiectul ia drept input un pseudocod simplu, care poate fi citit de la tastatura din program, si afiseaza NSD. 

  Mai mult, pseudocodul poate fi rulat, iar rezultatul executiei (eventual dupa citirea din caseta de input) se afiseaza in caseta de output, ca intr-un limbaj de programare uzual.

  Pseudocodul are urmatoarele instructiuni:
1. var int/string <variabila> - declararea unei variabile de tip int/string
2. set <variabila> <expresie/string> - atribuirea catre o variabila
Daca variabila e int se atribuie o expresie, iar daca e string se atribuie ca string tot cuvantul
3. read <variabila> - citirea unei variabile
4. print <variabila> - afisarea unei variabile
5. if <expresie> … else … endif – instructiunea if
6. while <expresie> … endwhile – instructiunea while
7. repeat … until <expresie> - repeat until
8. pass – instructiune dummy, nu face nimic

  Mentiuni: 
  Variabilele sunt litere mari si mici, expresiile din instructiunea set admit doar variabile, numere si operatori de baza (+, -, *, /, %), iar expresiile din if/while/repeat admit in plus comparatii (!=, <. ==, <=, >, >=), fara spatii intre ele. 
  Expresiile nu pot avea operatii cu string-uri. 
  Toate instructiunile se scriu pe o singura linie (si else, endif, endwhile, etc se numara ca instructiuni). 
  Structura unui if trebuie respectata prin folosirea if, else si endif (daca vrem sa avem o ramura if/else goala se poate folosi instructiunea pass), similar pentru bucle.

  Scurtaturi/butoane:
  
ctrl+r (RUN) - compileaza codul si afiseaza diagrama
  
ctrl+s (SAVE) - salveaza codul si input-ul/output-ul
  
ctrl+l (LOAD) - incarca codul si input-ul/output-ul
  
ctrl+o (CENTER) - aduce diagrama la forma initiala
  
ctrl+d (CLEAR) - sterge tot textul din caseta curenta
  
ctrl+j (SAGEATA DREAPTA) - mutare caseta de text curenta la dreapta

ctrl+k (SAGEATA STANGA) - mutare caseta de text curenta la stanga
  
Exemplu de cod care calculeaza numerele prime pana mai mici decat un n citit:
  

https://user-images.githubusercontent.com/70471765/161297602-7393687c-f4fd-4218-bf98-b2a9a9cf7f1c.mp4


  
Echipa: Aliciuc Alexandru si Laza Gabriel, grupa B2, UAIC
