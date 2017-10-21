/*Telehoi Alexandru 314CB*/
#include <stdio.h>
#include <stdlib.h>
#include "student.h"
#include "materie.h"
#include "lista.h"
#include <string.h>

typedef struct
{

     size_t M;

     TFHash fh;

     TLG *bucket;

     TFCmp fcmp;//functia de comparare

     AfiKey fkey;//functia de afisare a cheii

     TFE fe;//functia de eliberare 
}TH;

typedef struct
{
    void *value;

    void *key;

    TF afiVal;//functia de afisare a valorii

    TFE fVal;//functia de eliberare a valorii
}CI;//Cell Info

TH *IniTH(size_t range, TFHash f, TFCmp fcmp, AfiKey fkey, TFE fe)
{
    TH *a;

    a = (TH *)malloc(sizeof(TH));//alocarea tabelei hash

    if(!a)//testarea alocarii
        return NULL;
    
    a->bucket = (TLG *)calloc(range,sizeof(TLG));//alocarea vectorului de bucket-uri

    if(!a->bucket)//testarea alocarii
    {

        printf("Eroare de alocare bucket TLG in hash\n");

        free(a);

        return NULL;
    }

    a->M = range;//campurile din structura tabelei hash primesc parametrii dati ai functiei

    a->fh = f;

    a->fcmp = fcmp;

    a->fkey = fkey;

    a->fe = fe;

    return a;
}

int InsTH(TH *a, void *ael, char type)//functia de inserare in tabela hash
{
    TLG l;//declarare lista s.i.g.

    int cod;
    
    if(type == 'd')//calcularea codului in functie de tipul cheii(char sau int)
        cod = hash_f(((CI *)ael)->key, sizeof(int), a->M); 
    else
        cod = hash_f(((CI *)ael)->key, strlen(((CI *)ael)->key) + 1, a->M);
    
    l = a->bucket[cod];//initializare lista

    TLG ant = NULL;//declarare+initializare lista pentru realizare legaturii cu celula anterioara

    for(l = a->bucket[cod]; l != NULL; l = l->urm)//parcurgere bucket[cod corespunzator]
    {

        ant = l;//ant ia ultima valoare a lui l

        if(a->fcmp(((CI*)l->info)->key, ((CI *)ael)->key) == 0)//testare egalitate chei
        {   
            ((CI *)l->info)->fVal(((CI *)l->info)->value);//eliberare valoare precedenta

            a->fe(((CI *)l->info)->key);//eliberare cheie precedenta

            free(l->info);//eliberare informatie

            l->info = ael;//l->info ia valoarea elementului ce se vrea inserat

            return 1;//se realizeaza iesirea din functie
        }
    }
      
//in cazul in care functia de comparare indica non-egalitatea celor 2 chei se realizeaza inserarea la sfarsitul/inceputul bucket-ului
    TLG aux = malloc(sizeof(TCelulaG));//alocare celula auxiliara

    if(!aux)//testare
        return 0;

    aux->info = ael;//aux->info ia valoarea ce se vrea inserata

    aux->urm = NULL;//fiind la sf. listei, campul urm al lui aux ia valoarea NULL

    if(ant == NULL)//in cazul in care ant e NULL a->bucket[cod coresp] devine aux(aux se insereaza la inceput)
        a->bucket[cod] = aux;
    else
        ant->urm = aux;//altfel se insereaza la sf. realizandu-se legaturile corespunzatoare

    return 2;
}

void AfiStud(void *ae, FILE *out)//functia de afisare a unui element de tip TStudent
{
    TStudent *p = (TStudent *)ae;

    fprintf(out,"[Nume: %s, Grupa: %s, Medie: %.2f, Varsta: %d]", p->nume, p->grupa, p->medie, p->varsta);   
}

void AfiMat(void *ae, FILE *out)//functia de afisare a unui element de tip TMaterie
{
    TMaterie *p = (TMaterie *)ae;

    fprintf(out,"[Materie: %s, Ore_curs: %d, Ore_lab: %d, Credit: %d, Teme: %d]", p->materie, p->ore_curs, p->ore_lab, p->credit, p->teme);
}

void afiInt(void *key, FILE *out)//f. de afisare a unei chei de tip int
{
    fprintf(out, " (%d -> ", *(int *)key);
}

void afiChar(void *key, FILE *out)//f. de afisare a unui element de tip char
{
    fprintf(out, " (%s -> ", (char *)key);
}

int compChar(void *p1, void *p2)//f de comparare a 2 chei de timp char
{
    return strcmp((char *)p1, (char *)p2);
}

int compInt(void *p1, void *p2)//f de comparare a 2 chei de tip int
{
    return *(int *)p1 - *(int *)p2;
}

void elibMat(void *p)//f de eliberare a unui element de tip TMaterie
{
    free(((TMaterie *)p)->materie);

    free(p);
}

void elibStud(void *p)//f de eliberare a unui element de tip TStudent
{
    free(((TStudent *)p)->nume);

    free(p);
}

void AfiTH(TH *a, FILE *out)//functie de afisare tabele hash
{
    TLG *p, el;

    for(p = a->bucket; p < a->bucket+(a->M); p++)//parcurgere vector de bucket-uri
    {
        if(*p)//in cazul in care p!=NULL
        {
            fprintf(out, "[%d] :", p-a->bucket);//afisare indice bucket

            for(el = *p; el != NULL; el = el->urm)//parcurgere bucket
            {
                a->fkey(((CI *)el->info)->key, out);//afisare cheie

                ((CI *)el->info)->afiVal(((CI *)el->info)->value, out);//afisare valoare

                fprintf(out, ")");
            }

            fprintf(out, "\n");
        }
            else
        {
            fprintf(out, "[%d] :\n", p-a->bucket);//afisare bucket gol
        }
    }
}

void Redim(TH *a, char type)//functia pentru redimensionare
{

    a->bucket = (TLG *)realloc(a->bucket, 2*(a->M)*sizeof(TLG));//realocare vector de bucket-uri

    a->M = (a->M)*2;//dublare range

    int i;

    for(i = ((a->M)/2)+1; i < a->M; i++)//de la jumatatea vectorului de bucket-uri realocat, se initializeaza fiecare bucket cu NULL
        a->bucket[i] = NULL;

    if(!a)//testare realocare
        printf("ERROR: Realocarea nu a avut loc cu succes");
    
    TLG l, *p;

    TLG new = NULL, ultim = NULL;

    for(p = a->bucket; p < a->bucket+(a->M); p++)//parcurgere vector de bucket-uri
    {
        if(*p)
        {
            for(l = *p; l != NULL; l = a->bucket[p-(a->bucket)] )//parcurgere bucket
            {       
//rearanjare elemente prin declararea unei liste sig l(mai sus), prin care toate valorile anterioare for fi puse intr-o lista sig new, apoi inserate prin functia InsTH
                    a->bucket[p-(a->bucket)] = l->urm;

                    if(ultim == NULL)
                    {   
                        
                        l->urm = NULL;

                        new = l;

                        ultim = new;
                        
                    }
                        else
                    {

                        l->urm = NULL;    

                        ultim->urm = l;

                        ultim = l;

                    }
            }
        }
    }

    while(new != NULL)// inserare elemente in bucket-uri 
    {

        InsTH(a, new->info, type);

        new = new->urm;

    }
}

void FindEl(TH *a, char k_type, void* key, FILE *out)//functia de afisare element
{

    int cod;

    if(k_type == 'd')//determinare cod pentru care se vrea gasirea
    {
        cod = hash_f((int *)key, sizeof(int), a->M);
    }
    else
    { 
        cod = hash_f((char*)key, strlen((char*)key)+1, a->M);
    } 

    TLG l = a->bucket[cod];//declar. lista s.i.g. care e init. cu bucket[cod coresp]

    if(l == NULL)//testare disponobilitate element, in cazul in care lista e NULL de la inceput
    {
        fprintf(out, "Nu exista\n");
    }
    else
    {

        for(l = a->bucket[cod]; l != NULL; l = l->urm )//parcuregere bucket
            {

                    if(a->fcmp(((CI *)l->info)->key, key) == 0)//testare compatibilitate chei
                    {
                        ((CI *)l->info)->afiVal(((CI *)l->info)->value, out);//afisarea valorii

                        fprintf(out, "\n");

                        return;
                    }
            }
                fprintf(out, "Nu exista\n");       
    }

}

int DeleteEl(TH *a, char k_type, void *key)//stergerea unui element
{
    TLG ant = NULL, p;

    int cod;

    if(k_type == 'd')//det. cod corespunzator
        cod = hash_f((int *)key, sizeof(int), a->M);
    else
        cod = hash_f((char*)key, strlen((char*)key)+1, a->M);

    if(a->bucket[cod] == NULL)//in cazul in care lista e goala, f. intoarec 0
    {
        return 0;
    }

    for(p = a->bucket[cod]; p != NULL; ant = p, p = p->urm)//parcurgere lista 
    {
            if(a->fcmp(((CI *)p->info)->key, key) == 0)//in cazul in care cheia a fost gasita, se iese din bucla
                break;
    }  

    if(p == NULL)//daca s-a ajuns la sf. listei, f. intoarce 0
    {
        return 0;
    }

    if(ant == NULL)
        a->bucket[cod] = p->urm;//daca elementul sters s-a aflat la inceputul listei, primul element devine p->urm
    else
        ant->urm = p->urm;//altfel, se realizaeaza legaturile coresp.


    if(p!=NULL)//eliberare p
    {
        ((CI *)p->info)->fVal(((CI *)p->info)->value);

        a->fe(((CI *)p->info)->key);

        free(p->info);

        free(p);
    }
            
    return 1;
}

int main()
{

    FILE *in = fopen("input.in", "rt");//deschidere fisier de citire

    if(in == NULL)//testare
    {
        fprintf(stderr, "EROOR: Can't open file input.in");

        return -1;
    }

    FILE *out = fopen("output.out", "wt");//deschidere fisier de iesire

    if(out == NULL)//testare
    {
        fprintf(stderr, "ERROR: Can't open file output.out");

        return -1;
    }


    char op[10];//operatie

    char k_type;//tipul cheii

    int nr_bucket, loop; float k = 0;//variabila pentru numarul de intrari in tabela hash

    char inithash[9];

    char value[5];

    void *key;

    float lf = 0.75;//load-factor
//citire valori in ordinea corespunzatoare
    fscanf(in,"%s", inithash);

    fscanf(in, " %c", &k_type);

    fscanf(in, "%d", &nr_bucket);

    fscanf(in, "%d", &loop);

    TH *a = NULL;//initializare tabela hash

    if(k_type == 'd')//creare tabela hash
    {
        a = IniTH(nr_bucket, hash_f, compInt, afiInt, free);
    }
        else
    {
        a = IniTH(nr_bucket, hash_f, compChar, afiChar, free); 
    }
 
    int i;

    for(i = 0; i < loop; i++)
    {
        fscanf(in, "%s", op);

        if(strcmp(op, "insert") == 0)//in cazul in care operatia citita este insert
        {

            if(k_type == 's')//se aloca cheia in functie de tipul acesteia
            {
                key = malloc(50*sizeof(char));

                fscanf(in, "%s", (char *)key);
            }
                else
            {
                key = malloc(sizeof(int));

                fscanf(in, "%d", (int *)key);
            }    

            CI *cell = (CI *)malloc(sizeof(CI));//declar o variabila de tipul CI(Cell Info)

            cell->key = key;

            fscanf(in, "%s", value);

            if(strcmp(value, "mat") == 0)//in cazul in care se doreste implemnetarea unei structuri de tip TMaterie
            {
//declarare + alocare varibila de tip TMaterie aux
                TMaterie *aux;

                aux = malloc(sizeof(TMaterie));

                aux->materie = (char *)malloc(30);//alocare campul materie
//citire valori corespunzatoare din structura
                fscanf(in, "%s", aux->materie);

                fscanf(in, "%d", &aux->ore_curs);

                fscanf(in, "%d", &aux->ore_lab);

                fscanf(in, "%d", &aux->credit);

                fscanf(in, "%d", &aux->teme);
//variabila cell declarata anterior primeste valorile citite din fisier
                cell->value = aux;

                cell->fVal = elibMat;

                cell->afiVal = AfiMat;

                int k_increment_mat = InsTH(a, cell, k_type);//declarare variabila ce ia valoare de return a functiei InsTH si inserarea elementului un tabela hash

                if(k_increment_mat == 2)
                    k++;//incrementare k in cazul in care elementul a fost inserat la sfarsit(nu a fost deja existent in tabela) 
            }
                else//repetare operatii anterioare pentru cazul in care se doreste inserarea unui element de tip TStudent
            {
                TStudent *aux1;

                aux1 = malloc(sizeof(TStudent));

                aux1->nume = (char *)malloc(20);

                fscanf(in, "%s", aux1->nume);

                fscanf(in, "%f", &aux1->medie);

                fscanf(in, "%s", aux1->grupa);

                fscanf(in, "%d", &aux1->varsta);

                cell->value = aux1;

                cell->fVal = elibStud;

                cell->afiVal = AfiStud;

                int k_increment_stud = InsTH(a, cell, k_type);

                if(k_increment_stud == 2)
                    k++;
            }

            if((k/nr_bucket) >= lf)//in cazul in care se depaseste load factor-ul se realizeaza redimensionarea vectorului de bucket-uri
            {
                Redim(a, k_type); 

                nr_bucket = nr_bucket*2;//dublarea nr-ului de bucket-uri
            }    
        }
            else if(strcmp(op, "print") == 0)//in cazul in care operatia citita e print, se af. tabela
        {
                AfiTH(a, out);
        }
            else if(strcmp(op, "find") == 0)//in cazul in care operatia citita e find, se ap. f. de gasire a elementului cu cheia aferenta
        {

            void *KeyToFind;

            if(k_type == 's')//alocare+citire cheie pentru care se vrea gasirea elementului(cheii) in functie de tipul acesteia
            {
                KeyToFind = malloc(50*sizeof(char));

                fscanf(in, "%s", (char *)KeyToFind);
            }
            else if(k_type == 'd')
            {

                KeyToFind = malloc(sizeof(int));

                fscanf(in, "%d", (int *)KeyToFind);
            }

                FindEl(a, k_type, KeyToFind, out);//apel la functia de find
            
        }
            else if(strcmp(op, "delete") == 0)//in cazul in care operatia citita e delete, se ap. f. de stegere  elem. cu cheia coresp.
        {
            void *KeyToDel;

            if(k_type == 's')
            {
                KeyToDel = malloc(50*sizeof(char));

                fscanf(in, "%s", (char*)KeyToDel);
            }
            else if(k_type == 'd')
            {
                KeyToDel = malloc(sizeof(int));

                fscanf(in, "%d", (int *)KeyToDel);
            }

            int delete = DeleteEl(a, k_type, KeyToDel);

            if(delete == 1)//daca functia de stergere returneaza 1, nr-ul de intrari in tabela este decrementat
                k--;
        }
    }

    fclose(in);//inchidere fisier input

    fclose(out);//inchidere fisier output

    return 0;

}
