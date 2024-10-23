**Tudor Robert-Fabian**
**312CAa**
**Anul 2023-2024**

## Segregated Free Lists - Tema 1

### Descriere:

* Tema își propune dezvoltarea unui alocator de memorie folosind o structură de date denumită "Segregated Free Lists". Această structură e reprezentată de un vector de liste dublu înlănțuite, fiecare listă gestionând blocuri de memorie de aceeași dimensiune. Scopul alocatorului de memorie este să ofere diverse funcționalități precum: alocare, eliberare, citire, scriere a memoriei în funcție de comenzile primite.

* Pentru alocarea memoriei, comanda MALLOC este urmată de numărul de octeți pe care dorim să îi alocăm. Alocatorul caută în vectorul de liste un bloc de memorie disponibil de dimensiune corespunzătoare sau, dacă nu găsește, poate fragmenta blocuri mai mari pentru a îndeplini comanda.

* Eliberarea memoriei este realizată prin comanda FREE, care primește adresa de început a blocului de memorie ce trebuie eliberat. Acest bloc este marcat ca fiind liber și reintrodus în vectorul de liste.

* Operațiile disponibile sunt cele de citire (READ) și scriere (WRITE) și permit accesarea și modificarea conținutului memoriei de la o anumită adresă și pe o anumită lungime.

* O altă funcție ce trebuie implementată este afișarea stării actuale a memoriei (DUMP_MEMORY)/

* Tema presupune totodată și gestionarea atentă a erorilor, cum ar fi lipsa de memorie (Out of memory) sau accesul la zone de memorie nealocate (Segmentation fault).


## Explicații:

### init_heap
* Funcția alocă memorie pentru vectorul de liste dublu înlănțuite denumită în enunțul temei "Segregated Free Lists". Ea primește adresa de început a zonei de memorie, numărul de liste din vector și dimensiunea fiecărei liste. De asemenea, aceasta inițializează vectorul de liste, precum și blocurile de memorie corespunzătoare pentru fiecare listă, utilizând apeluri către funcția "dll_allocate_nth_node". La final, funcția returnează vectorul de liste.


### dump_memory

* Funcția afișează informații despre starea actuală a memoriei alocate și nealocate, precum și despre blocurile de memorie utilizate și disponibile.


### my_malloc

* Funcția începe prin a verifica dacă avem sufiecient spațiu pentru alocarea cerută în vectorul de liste. Această verificare presupune parcurgerea fiecărei liste din vector pentru a determina dacă există o listă disponibilă de dimensiune suficient de mare.

* Dacă un spațiu corespunzător este găsit, funcția înlătură un nod din lista corespunzătoare și utilizează acest bloc de memorie pentru a crea un nou bloc alocat cu informațiile celui vechi actualizate pentru dimensiunea lui.

* În situația în care în urma alocării cerute rămâne spațiu nealocat în acel bloc, funcția îl reintroduce în vectorul de liste.

* Pentru a realiza aceste operațiuni funcția utilizează alte două funcții auxiliare: "insert_node_ordonated_vector" și "insert_element_ordonated_allocated_blocks". Aceste funcții auxiliare sunt responsabile de inserarea ordonată a nodurilor și a blocurilor de memorie în structurile de date corespunzătoare.


### my_free - *(tip reconstituire 0)*

* Funcția parcurge lista de blocuri alocate pentru a localiza blocul care conține adresa specificată. Dacă adresa reprezintă începutul unui nod din vectorul de liste, ea înlătură blocul din lista de blocuri alocate, iar spațiul de memorie pe care l-a ocupat este marcat ca fiind eliberat. De asemenea, creează un nou bloc de dimensiunea celui eliberat, iar dacă blocul acesta are o dimensiune deja existentă în vector, funcția introduce blocul în lista corespunzătoare dimensiunii lui. În caz contrar, vectorul este redimensionea și se creează o listă nouă de dimensiunea blocului, în care îl adăugăm.


### my_free_1 - *(tip reconstituire 1)*

* Funcția parcurge lista de blocuri alocate pentru a localiza blocul care conține adresa specificată. Dacă adresa reprezintă începutul unui nod din vectorul de liste, ea înlătură blocul din lista de blocuri alocate, iar spațiul de memorie pe care l-a ocupat este marcat ca fiind eliberat. De asemenea, funcția creează un nou bloc de dimensiunea celui eliminat(new_node). Apoi, funcția realizează o căutare în vector a unor blocuri ce au același index ca al blocului eliberat și totodată adrese consecutive acestuia(la stânga, la dreapta sau ambele). Dacă nu găsește niciun bloc lateral, inserează new_node în listă după dimensiune și adresă. Dacă în schimb găsește un unul sau două blocuri laterale inițializează un nod nou(merged_node) ce însumează new_node împreună cu cel/cele lateral/laterale. În final new_node este dezalocat, iar merged_node este introdus în vector pe baza noii sale dimensiuni și adrese.


### destroy_heap

* Funcția este responsabilă pentru eliberarea tuturor resurselor alocate și încheierea programului. Ea parcurge listele de noduri din vector și lista de blocuri alocate și eliberează memoria alocată pentru acestea de-a lungul rulării programului.


### write

* Funcția se ocupă de scrierea unui număr specificat de bytes la o anumită adresă în lista de noduri alocate. Ea parcurge blocurile de memorie alocate pentru a verifica dacă adresa specificată se află într-unul dintre acestea. În cazul în care nu există suficienți bytes alocați de la adresa dată în continuare, funcția afișează un mesaj de eroare și apealează dump_memory.Dacă găsește în schimb un bloc care conține adresa și memorie suficientă, funcția efectuează scrierea la această adresă.


### read

* Funcția se ocupă de citirea unui număr specificat de bytes la o anumită adresă în lista de noduri alocate. Ea parcurge blocurile de memorie alocate pentru a verifica dacă adresa specificată se află într-unul dintre acestea. Dacă mesajul pe care vrem să îl scriem are dimensiunea mai mică decât nr_bytes, scriem doar atâția bytes cât are mesajul. În cazul în care nu există suficienți bytes alocați de la adresa dată în continuare, funcția afișează un mesaj de eroare și apelează dump_memory. Dacă găsește în schimb un bloc care conține adresa și memorie suficientă, funcția efectuează citirea a nr_bytes de la această adresă.


### main

* Funcția primește diverse comenzi pe care le parsează sau le transformă pentru a apela funcțiile principale ale programului, în urma cărora se realizează operații precum inițializarea memoriei(INIT_HEAP), diagnosticarea memoriei(DUMP_MEMORY), alocarea de memorie(MALLOC), eliberarea memoriei(FREE), scrierea în memorie(WRITE) sau citirea din memorie(READ).




