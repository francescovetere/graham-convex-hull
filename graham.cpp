/*
    Compilare con: g++ -Wall -Wextra graham.cpp -o graham
    Vetere Francesco

    Algoritmo di Graham

    Il seguente programma C++ riceve in ingresso una sequenza di punti Q e ne restituisce l'involucro convesso
    CH(Q), ossia quel poligono P convesso e di area minimale tale che ogni punto della sequenza Q si trova 
    sul perimetro di P o nella sua parte interna.
    CH(Q) viene calcolato tramite l'algoritmo di Graham, che ha complessità O(nlogn) dove n = |Q| è il numero 
    di punti in input.
*/

#include <iostream>
#include <stack>
#include <stdlib.h> //per la funzione qsort
using namespace std;

struct punto
{
    int x;
    int y;
};

//p0, il punto con ordinata minima (e con ascissa minima se più punti hanno questa ordinata in comune)
//è memorizzato in una variabile globale poichè la subroutine compare di qsort deve riferirsi ad esso
//p0 farà sicuramente parte dell'involucro convesso
punto p0;

//funzione di utilità che scambia due punti
void scambia(punto& p1, punto& p2)
{
    punto temp;
    temp = p1;
    p1 = p2;
    p2 = temp;
}

//funzione che calcola la distanza al quadrato tra due punti
//il calcolo della radice quadrata non è necessario per lo scopo della funzione
int distanza(punto p1, punto p2)
{
    return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y);
}

//funzione che calcola il prodotto vettoriale (p1 - p0)x(p2 - p0)
int curva(punto p0, punto p1, punto p2)
{
    //restituisce un intero > 0 se la curva formata da p0,p1,p2 volge a destra, < 0 se volge a sinistra, = 0 se sono
    //allineati
    return (p2.x - p0.x)*(p1.y - p0.y) - (p1.x - p0.x)*(p2.y - p0.y);
}

//funzione utilizzata da qsort come criterio di ordinamento tra due punti
//ritorna -1 se _p1 viene prima di _p2, 1 se _p1 viene dopo _p2
int compare(const void *_p1, const void *_p2)
{
   punto p1 = *(punto*) _p1;
   punto p2 = *(punto*) _p2;

   int _curva = curva(p0, p1, p2);

   //i tre punti sono allineati: il primo nell'ordinamento è quello più vicino a p0
   if (_curva == 0)
   {
       if(distanza(p0, p2) >= distanza(p0, p1)) return -1;
       else return 1;
   }

   else if(_curva > 0) //curva a destra
       return 1;
   else //curva a sinistra
       return -1;
}

//funzione che restituisce il punto p sotto la cima della pila, senza però modificare la pila
punto next_to_top(stack<punto>& S)
{
    punto _top = S.top(); S.pop();
    punto p = S.top(); S.push(_top);
    return p;
}

//funzione che calcola e stampa la sequenza di punti che forma l'involucro convesso CH(V)
void graham(punto V[], int n)
{
   //min_y tiene traccia dell'ordinata più bassa trovata fino ad un determinato istante
   //a parità di ordinata, viene scelto il punto con ascissa minore
   int min_y = V[0].y;
   int min_index = 0; //Tiene traccia dell'indice in V del punto la cui ascissa è min_y
   for (int i = 1; i < n; i++)
   {
       if ((V[i].y < min_y) || (V[i].y == min_y && V[i].x < V[min_index].x))
       {
           min_y = V[i].y;
           min_index = i;
       }
   }

   //il punto di indice min_index in V viene messo in posizione 0
   scambia(V[0], V[min_index]);

   //tale punto è assegnato a p0, variabile globale
   p0 = V[0];

   //si ordinano a partire da 1 i restanti n-1 punti in senso antiorario rispetto a p0
   //il confronto tra due punti non viene effettuato calcolando direttamente l'angolo che essi formano con p0, 
   //bensì tramite il prodotto vettoriale
   //utilizzo la funzione di libreria qsort, presente in stdlib.h, che ha costo O(nlogn) nel caso medio
   qsort(&V[1], n-1, sizeof(punto), compare);

   int new_dim = 1;
   //se più punti formano lo stesso angolo con p0, li rimuovo tutti tranne quello più distante da p0 stesso, in quanto
   //sono tutti combinazioni convesse di p0 e del punto più lontano, e si possono quindi escludere dall'involucro
   //convesso
   //ricostruisco quindi V eliminando tali punti superflui 
   //new_dim sarà la nuova dimensione significativa di V
   for (int i = 1; i < n; i++)
   {
       while (i < n-1 && curva(p0, V[i], V[i+1]) == 0) i++;
       V[new_dim] = V[i];
       new_dim++;
   }

   //se la nuova dimensione significativa è minore di 3, non posso calcolare alcun involucro convesso
   if (new_dim < 3) {cout << "Impossibile calcolare involucro convesso\n"; return;}

   //dichiaro una pila S che alla fine conterrà esattamente i vertici dell'involucro convesso
   stack<punto> S;

   //inserisco in pila i primi 3 punti
   S.push(V[0]);
   S.push(V[1]);
   S.push(V[2]);

   //esamino i restanti (new_dim - 3) punti
   for (int i = 3; i < new_dim; i++)
   {
        //elimino la cima della pila finchè i punti next_to_top(S), S.top() e V[i] effettuano una svolta non a 
        //sinistra
        while (curva(next_to_top(S), S.top(), V[i]) >= 0) S.pop();

        //una volta teminato il ciclo while, V[i] appartiene sicuramente a CH(V), e dunque viene messo in pila
        S.push(V[i]);
   }

   cout << "L'involucro convesso è formato dai seguenti punti:\n";

   //estraendoli dalla pila, i punti di CH(V) vengono stampati in senso orario
   while(!S.empty())
   {
        punto p = S.top();
        cout << '(' << p.x << ", " << p.y << ')';
        S.pop();
        cout << '\n';
   }

}

int main()
{
    int n;

    do
    {
        cout << "Inserire il numero di punti (almeno 3): ";
        cin >> n;
    }while(n < 3);

    punto Q[n];

    cout << "Inserire " << n << " punti\n";

    for(int i = 0; i < n; i++)
    {
        cout << "Q[" << i << "].x = ";
        cin >> Q[i].x;
        cout << "Q[" << i << "].y = ";
        cin >> Q[i].y;
    }

    graham(Q, n);

    return 0;
}
