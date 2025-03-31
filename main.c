#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

typedef struct BAZA {
    long long int ca_id;
    long long int ca_b_id;
    long long int ca_c_id;
    char ca_name[71];
    int ca_tax_st;
    double ca_bal;
}Baza;

typedef struct CVOR {
    int br;			
    long long int* kljuc;		
    struct CVOR* otac;
    struct CVOR** pok;  
    struct CVOR* desno;
    int nivo;	
    Baza* baza;
}Cvor;


typedef struct queue
{
    void** content;
    int front, rear;
    int max_size;
} Queue;

Queue* createQueue(int initial_size)
{
    Queue* ret = (Queue*)calloc(1, sizeof(Queue));
    if (!ret) return 0;
    ret->content = calloc(initial_size, sizeof(void*));
    if (!ret->content)
    {
        free(ret);
        return 0;
    }
    ret->max_size = initial_size;
    return ret;
}

void eraseQueue(Queue* q)
{
    if (q)
    {
        free(q->content);
        free(q);
    }
}

int insertIntoQueue(Queue* q, void* dat)
{
    if (!q) return 0;
    if ((q->rear + 1) % q->max_size == q->front)
    {
        void** c = (void**)calloc(q->max_size * 2, sizeof(void*));
        int i, j;
        if (!c) return 0;
        for (i = 0, j = q->front; j != q->rear; j = (j + 1) % q->max_size, i++)
            c[i] = q->content[j];
        q->front = 0; q->rear = i;
        free(q->content);
        q->content = c;
        q->max_size *= 2;
    }
    q->content[q->rear++] = dat;
    q->rear = q->rear % q->max_size;
    return 1;
}

void* removeFromQueue(Queue* q)
{
    void* ret = 0;
    if (!q) return 0;
    if (q->front == q->rear) return 0;
    ret = q->content[q->front++];
    q->front = q->front % q->max_size;
    return ret;
}

int isEmptyQueue(Queue* q)
{
    if (!q) return 1;
    return q -> front == q -> rear;
}

Baza* procitaj_red_datoteke(FILE* fp) {
    //CA_ID|CA_B_ID|CA_C_ID|CA_NAME|CA_TAX_ST|CA_BAL
    Baza* baza = (Baza*)malloc(sizeof(Baza));
    if (baza == NULL) {
        printf("Neuspela alokacija memorije! \n");
        exit(1);
    }

    fscanf(fp, "%lld|%lld|%lld|", &baza->ca_id, &baza->ca_b_id, &baza->ca_c_id);

    if (feof(fp)) {
        return NULL;
    }

    //citanje ca_name-a
    char c;
    int i = 0;
    while ((c = fgetc(fp)) != '|' && !feof(fp)) {
        baza->ca_name[i] = c;
        i++;
    }
    baza->ca_name[i] = '\0';

    fscanf(fp, "%d|%f", &baza->ca_tax_st, &baza->ca_bal);

    return baza;
}


void sortiran_niz(long long int* kljucevi, int cnt) {
    int pom;
    for (int i = 0; i < cnt - 1; i++)
    {
        for (int j = i + 1; j < cnt; j++)
        {
            if (kljucevi[i] > kljucevi[j]) {
                pom = kljucevi[i];
                kljucevi[i] = kljucevi[j];
                kljucevi[j] = pom;
            }
        }
    }

}


Cvor* unesi_kljuc(Cvor* koren, int m, long long int key, Baza* baza) {
    

    int donja_klj = m / 2;      //min dozvoljen br kljuceva u listu
    int gornja_klj = m - 1; //maks dozvoljen br kljuceva u listu
    int donjag;             //br kljuceva koji ostaje u starom cvoru tokom prelamanja
    if (m % 2 == 0) {
        donjag = m / 2;
    }
    else
    {
        donjag = (m / 2) + 1;
    }

    Cvor* node = koren;

    //pretraga do lista
    int i = 0;
    while (1) {

        i = 0;
        if (!node->pok[0] || node==NULL) break;

        while (i< (node->br) && key>node->kljuc[i])
        {
            i++;
        }
        node = node->pok[i];
    }//prosla celo stablo i stigla do lista u koji umecem kljuc



    //pretraga kroz list;
    i = 0;
    while (i< (node->br) && key>node->kljuc[i])
    {
        i++;
    }
    if (i < (node->br) && key == node->kljuc[i]) {
        printf("Kljuc je vec umetnut u stablo!\n");
        return 0;
    }
    int j = node->br;
    while (j>i)
    {
        node->kljuc[j] = node->kljuc[j - 1];//reorganizacija kljuceva u listu
        j--;
    }

   
    node->kljuc[i] = key;
    node->br++;

    while (node->br == m) //list je popunjen prelama se
    {
        if (node->pok[0] == NULL) { 
            
            if (node == koren) {    //slucaj koren=list
                //bice novi koren stabla
                Cvor* novik = (Cvor*)malloc(sizeof(Cvor));
                if (novik == NULL) {
                    printf("Neuspela alokacija!\n");
                    exit(1);
                }
                novik->br = 1;

                novik->kljuc = (long long int*)malloc(sizeof(long long int) * (m - 1));
                if (novik->kljuc == NULL) {
                    printf("Neuspela alokacija!\n");
                    exit(1);
                }

                novik->pok = (Cvor**)malloc(m * sizeof(Cvor*));
                if (novik->pok == NULL) {
                    printf("Neuspela alokacija! \n");
                    exit(1);
                }
                novik->pok[0] = node;
                novik->desno = NULL;
                novik->otac = NULL;
                novik->baza = NULL;

                //alocira se mesto za novi cvor gde ce otici m-1-gornja(m/2) kljuceva
                Cvor* novic = (Cvor*)malloc(sizeof(Cvor));
                if (novic == NULL) {
                    printf("Neuspela alokacija!\n");
                    exit(1);
                }
                novic->br = donja_klj;

                novic->kljuc = (long long int*)malloc(sizeof(long long int) * (m - 1));
                if (novic->kljuc == NULL) {
                    printf("Neuspela alokacija!\n");
                    exit(1);
                }

                
                j = 0;
                while (j < donja_klj) {
                    novic->kljuc[j] = node->kljuc[j + donjag];
                    j++;
                }

 
                novic->pok = (Cvor**)malloc(m * sizeof(Cvor*));
                if (novic->pok == NULL) {
                    printf("Neuspela alokacija! \n");
                    exit(1);
                }

                j = 0;
                while (j < m) {   //moze maks m podstabala da ima pa samim tim i maks m pok na decu
                    novic->pok[j] = NULL;
                    j++;
                }

                node->desno = novic;
                novic->desno = NULL;
                novik->pok[1] = novic;

                novik->kljuc[0] = node->kljuc[donjag - 1];
                node->br -= donja_klj;


                koren = novik;

                node->otac = novic->otac = koren;


            }
            else      //slucaj koren!=list, prelamanje
            {
                Cvor* novik = node->otac;
                novik->br++;        //dobice jos jedan kljuc jer ce doci do preloma
                
                
                long long int* niz = (long long int*)malloc(novik->br * sizeof(long long int));
                if (niz == NULL) {
                    exit(1);
                }
                sortiran_niz(niz, novik->br);        

                int ii = 0;
                while (ii<novik->br - 1 && key>novik->kljuc[ii]) ii++;//kretanje kroz cvor radi reorganizacije 
                


                Cvor* novic = (Cvor*)malloc(sizeof(Cvor));
                if (novic == NULL) {
                    printf("Neuspela alokacija!\n");
                    exit(1);
                }

                novic->br = donja_klj;

                novic->kljuc = (long long int*)malloc(sizeof(long long int) * (m - 1));
                if (novic->kljuc == NULL) {
                    printf("Neuspela alokacija!\n");
                    exit(1);
                }

                j = 0;
                while( j < donja_klj)
                {
                    novic->kljuc[j] = node->kljuc[j + donjag];
                    j++;
                }

                
                node->desno = novic;
                novic->desno = NULL;

                novic->pok = (Cvor**)malloc(m * sizeof(Cvor*));
                if (novic->pok == NULL) {
                    printf("Neuspela alokacija! \n");
                    exit(1);
                }

                if (node->pok[0] == NULL) {
                    j = 0;
                    while (j < m + 1) {
                        novic->pok[j] = NULL;
                        j++;
                    }
                    
                }
               
                novik->kljuc[novik->br - 1] = node->kljuc[donjag - 1];
                node->br -= donja_klj;
                
                novik->pok[novik->br] = novic;
                novic->otac = novik;

                j = ii + 2;
                while (j<=novik->br)
                {
                    novik->pok[j]->otac = novik;
                    j++;
                }

                

                node = novik;


            }
        }
        else
        {
            if (node == koren) {    //slucaj koren=list
             //bice novi koren stabla
                Cvor* novik = (Cvor*)malloc(sizeof(Cvor));
                if (novik == NULL) {
                    printf("Neuspela alokacija!\n");
                    exit(1);
                }
                novik->br = 1;

                novik->kljuc = (long long int*)malloc(sizeof(long long int) * (m - 1));
                if (novik->kljuc == NULL) {
                    printf("Neuspela alokacija!\n");
                    exit(1);
                }

                novik->pok = (Cvor**)malloc(m * sizeof(Cvor*));
                if (novik->pok == NULL) {
                    printf("Neuspela alokacija! \n");
                    exit(1);
                }
                novik->pok[0] = node;
                novik->desno = NULL;
                novik->otac = NULL;

                int low = 1;
                int high = node->br;
                int mid = (low + high) / 2;


                //alocira se mesto za novi cvor gde ce otici m-1-gornja(m/2) kljuceva
                Cvor* novic = (Cvor*)malloc(sizeof(Cvor));
                if (novic == NULL) {
                    printf("Neuspela alokacija!\n");
                    exit(1);
                }
                novic->br = high-mid;

                novic->kljuc = (long long int*)malloc(sizeof(long long int) * (m - 1));
                if (novic->kljuc == NULL) {
                    printf("Neuspela alokacija!\n");
                    exit(1);
                }

           
                int cnt = 0;
                j = 0;

                while (j < (high - mid))
                {
                    novic->kljuc[j] = node->kljuc[j + mid];
                    cnt++;
                    j++;
                }
               

                novic->pok = (Cvor**)malloc(m * sizeof(Cvor*));
                if (novic->pok == NULL) {
                    printf("Neuspela alokacija! \n");
                    exit(1);
                } 
                node->br -= cnt+1;

                i = 0;
                while (i<=(high-mid))
                {
                    novic->pok[i] = node->pok[i + gornja_klj];
                    if (novic->pok[i]) node->pok[i + gornja_klj]->otac = novic;
                    i++;
                }

                i = (high-mid)+1;
                while (i<=m)
                {
                    novic->pok[i] = NULL;
                    i++;
                }
            
            


                node->desno = novic;
                novic->desno = NULL;
                novik->pok[1] = novic;

                novik->kljuc[0] = node->kljuc[mid-1];



                koren = novik;

                node->otac = novic->otac = koren;


            }
            else      //slucaj koren!=list, prelamanje, node->pok[0]!= null
            {
                Cvor* novik = node->otac;
                novik->br++;        //dobice jos jedan kljuc jer ce doci do preloma
                

                int ii = 0;
                while (ii<novik->br - 1 && key>novik->kljuc[ii]) ii++;//kretanje kroz cvor radi reorganizacije 
                

                Cvor* novic = (Cvor*)malloc(sizeof(Cvor));
                if (novic == NULL) {
                    printf("Neuspela alokacija!\n");
                    exit(1);
                }

                int low = 1;
                int high = node->br;
                int mid = (low + high) / 2;

                novic->br = high-mid;

                novic->kljuc = (long long int*)malloc(sizeof(long long int) * (m - 1));
                if (novic->kljuc == NULL) {
                    printf("Neuspela alokacija!\n");
                    exit(1);
                }
                int cnt = 0;
                j = 0;
                while (j < (high - mid)) {
                    novic->kljuc[j] = node->kljuc[j + mid];
                    cnt++;
                    j++;
                }


                
                node->desno = novic;
                novic->desno = NULL;

                novic->pok = (Cvor**)malloc(m * sizeof(Cvor*));
                if (novic->pok == NULL) {
                    printf("Neuspela alokacija! \n");
                    exit(1);
                }

                node->br -= cnt + 1;



                //treba da azuriram m  pok na decu za novic
                i = 0;
                while (i <= (high - mid))
                {
                    novic->pok[i] = node->pok[i + gornja_klj];
                    if (novic->pok[i]) node->pok[i + gornja_klj]->otac = novic;
                    i++;
                }

                i = (high - mid) + 1;
                while (i <= m)
                {
                    novic->pok[i] = NULL;
                    i++;
                }



            
                novik->kljuc[novik->br - 1] = node->kljuc[mid - 1];
                
                
                
                novik->pok[novik->br] = novic;
                novic->otac = novik;
                for (int j = ii + 2; j <= novik->br; j++)
                {
                    novik->pok[j]->otac = novik;
                }

                node = novik;


            }
             
        }
 
    }

    return koren;

}

int visina_stabla(Cvor* koren) {
    int h=1;
    int i = 0;
    Cvor* node = koren;
    while (1) {
        i = 0;
        
        if (!node->pok[0]) break;

        while (i< (node->br))
        {
            i++;
        }
        node = node->pok[i];
        h++;
    }

    return h;
}



void ispis_stablaa(Cvor* koren, int visina) {
    int i = 0;
    if (!koren) return;
    Cvor* node = koren;
    //printf("\t\t");
    int j;
    
    while (i < visina) {
        for (int i = 0; i < node->br; i++)
        {
            printf("|%lld|", node->kljuc[i]);
        }
        if (node->desno == NULL) {
            printf("\n");
            //if (node->pok[0] != NULL) printf("\t");
            node = node->pok[0];
        }
        else
        {
            //if (node->pok[0] != NULL) printf("\t");
            Cvor* cvor = node;
            //node = node->desno;
            
            while (node->desno!=NULL)
            {
                printf("  ");
                for (int i = 0; i < node->desno->br; i++) {
                    printf("|%lld|", node->desno->kljuc[i]);
                }
                node = node->desno;
            }
            printf("\n");
            //cvor = cvor->pok[0];
            node = cvor->pok[0];
        }
        //sredi tabove i razmake za visinu
        
        if (i == 0) {
            for (j = 0; j < visina; j++)
            {
                printf("\t");
            }
        }
        else
        {
            for (j = visina - i; j > 0; j--) {
                printf("\t");
            }
        }
    
        
        //if (i != visina) printf("\t");

        i++;
    }
}
    


Cvor* napravi_stablo(int m, char* ime_datoteke) {   //pravi stablo i stavlja prvi ikada pocetni cvor, koji je prazan, jer jos nema kljuceve i ucitava bazu podataka iz datoteke
    FILE* fp = fopen(ime_datoteke, "r");
    if (fp == NULL) {
        printf("Ne postoji datoteka %s!\n", ime_datoteke);
        exit(1);
    }

    Cvor* koren = (Cvor*)malloc(sizeof(Cvor));
    if (koren == NULL) {
        printf("Neuspela alokacija memorije!\n");
        exit(1);
    }

    koren->br = 0;
    koren->otac = NULL;
    //koren->nivo = 0;
    koren->baza = NULL;

    koren->kljuc = (long long int*)malloc((m - 1) * sizeof(long long int));
    if (koren->kljuc == NULL) {
        printf("Neuspela alokacija memorije!\n");
        exit(1);
    }

    koren->pok = (Cvor**)malloc(m * sizeof(Cvor*));



    if (koren->pok == NULL) {
        printf("Neuspela alokacija memorije!\n");
        exit(1);
    }

    for (int i = 0; i < m; i++) {
        koren->pok[i] = NULL;
    }



    koren->desno = NULL;
    int cnt = 0;

    while (1) {
        Baza* baza = procitaj_red_datoteke(fp);

        

        if (feof(fp)) {
            break;
        }

        cnt++;

        long long int key = baza->ca_id;

        koren = unesi_kljuc(koren, m, key, baza);
        

        //ispis_stabla(koren, m);

        /*printf("Unet je kljuc %d\n", key);
        
        for (int i = 0; i < koren->br; i++)
        {
            printf("%d|", koren->kljuc[i]);
        }
        printf("\n");   



        if (koren->pok[0] != NULL) {
            for (int i = 0; i < koren->br + 1; i++)
            {
                for (int j = 0; j < koren->pok[i]->br; j++) {
                    printf("%d,", koren->pok[i]->kljuc[j]);
                }

                printf(" -> ");             
            }
        }       
        printf("\n\n");*/
    }
    int visina = visina_stabla(koren);
    //printf("broj kljuceva u stablu je %d\n", cnt);

    //ispis_stablaa(koren, visina);


    return koren;
    fclose(fp);
}

void ispis(long long int key, int cnt, char* ime_datoteke) {
    FILE* fp = fopen(ime_datoteke, "r");
    if (fp == NULL) {
        printf("Ne postoji datoteka %s!\n", ime_datoteke);
        exit(1);
    }

    while (1)
    {
        Baza* baza = procitaj_red_datoteke(fp);

        if (feof(fp)) break;

        if (cnt != 0 && key==baza->ca_id) {
            printf("%lld|%lld|%lld|%s|%d|%f\n", baza->ca_id, baza->ca_b_id, baza->ca_c_id, baza->ca_name, baza->ca_tax_st, baza->ca_bal);
            
        }
    }

    fclose(fp);
}

void ispis2(char* ime_datoteke,long long int* niz,long long int k) {
    FILE* fp = fopen(ime_datoteke, "r");
    if (fp == NULL) {
        printf("Ne postoji datoteka pod tim nazivom!\n");
        exit(1);
    }


    FILE* f = fopen("izlaz.txt", "w");

    while (1)
    {
        Baza* baza = procitaj_red_datoteke(fp);

        if (feof(fp)) break;

        for (int i = 0; i < k; i++)
        {
            if (niz[i] == baza->ca_id) {
                fprintf(f, "%lld|%lld|%lld|%s|%d|%f\n", baza->ca_id, baza->ca_b_id, baza->ca_c_id, baza->ca_name, baza->ca_tax_st, baza->ca_bal);
            }
        }
    }





    fclose(f);
    fclose(fp);
}

void ispis3(char* ime_datoteke,long long int* niz,long long int k) {
    FILE* fp = fopen(ime_datoteke, "r");
    if (fp == NULL) {
        printf("Ne postoji datoteka pod tim nazivom!\n");
        exit(1);
    }


    FILE* f = fopen("izlazz.txt", "w");

    while (1)
    {
        Baza* baza = procitaj_red_datoteke(fp);

        if (feof(fp)) break;

        for (int i = 0; i < k; i++)
        {
            if (niz[i] == baza->ca_id) {
                fprintf(f, "%lld|%lld|%lld|%s|%d|%f\n", baza->ca_id, baza->ca_b_id, baza->ca_c_id, baza->ca_name, baza->ca_tax_st, baza->ca_bal);
            }
        }
    }





    fclose(f);
    fclose(fp);
}

int pretraga_kjluca(Cvor* koren,long long int key, char* ime_datoteke, int flag) {
    if (!koren) return 0;
    else
    {
        int cnt = 0;
        Cvor* node = koren;
        
        int i = 0;
        while (node->pok[0]) {

            i = 0;

            while (i < node->br) {
                if (key <= node->kljuc[i]) {
                    cnt++;
                    break;
                }
                else
                {
                    i++;
                    cnt++;
                }
            }
            
            node = node->pok[i];
        }//prosli celo stablo i stigli do lista gde umecem

        //pretraga kroz list;
        i = 0;
        while (i< (node->br) && key>node->kljuc[i])
        {         
            cnt++;
            i++;           
        }
        if (i == node->br) return 0;
        if (key == node->kljuc[i]) {
            cnt++;
            //printf("\nbr poredjenja je %d\n", cnt);
            if (flag == 1) {
                ispis(key, cnt, ime_datoteke);
            }                      
        }
        else
        {
            cnt = 0;
            printf("Ne postoji taj kljuc u stablu!\n");            
        }
        return cnt;
    }   
}


int pretrazi_kljuceve_neuredjene(long long int* niz, int k, Cvor* koren, char* ime_datoteke) {
    if (!koren) return -1;
    else
    {
        
        Cvor* node = koren;
        int cnt = 0;
        for (int i = 0; i < k; i++)
        {
            cnt += pretraga_kjluca(node, niz[i],ime_datoteke, 0);
        }

        ispis2(ime_datoteke, niz, k);


        return cnt;
    }
}


int pretrazi_kljuceve_uredjeno(int K,long long int klj, Cvor* koren, char* ime_datoteke) {
    if (!koren) return -1;
    else
    {
        Cvor* node = koren;
        int cnt = 0;

        long long int* niz = (long long int*)malloc(K * sizeof(long long int));
        if (niz == NULL) {
            printf("Neuspela alokacija!\n");
            exit(1);
        }
        for (int i = 0; i < K; i++) {
            niz[i] = 0;
        }

        int i = 0;
        while (node->pok[0]) {

            i = 0;

            while (i < node->br) {
                if (klj <= node->kljuc[i]) {
                    cnt++;
                    break;
                }
                else
                {
                    i++;
                    cnt++;
                }
            }

            node = node->pok[i];
        }//nasla u kom je listu


        i = 0;
        while (i< (node->br) && klj>node->kljuc[i])
        {
            cnt++;
            i++;
        }
        if (i == node->br) return 0;
        if (klj == node->kljuc[i]) {
            //cnt++;
            //printf("\nbr poredjenja je %d\n", cnt);
            /*for (int j = 0; j < K; j++)
            {
                cnt++;
                node = node->desno;

            }*/
            //int z = 0;
            int j = 0;
            
            while (j<K)
            {
                while (i<node->br)
                {
                    niz[j] = node->kljuc[i];

                    j++;
                    cnt++;
                    i++;
                }
                if (node->desno != NULL) {
                    node = node->desno;
                    i = 0;
                }
               
            }

            ispis3(ime_datoteke, niz, K);
        }
        else
        {
            cnt = 0;
            printf("Ne postoji taj kljuc u stablu!\n");
        }



        return cnt;
            
    }
}

void obrisi_stablo(Cvor* koren, int m) {
    if (!koren) {
        free(koren);
        return;
    }
    Queue* q = createQueue(m * (m - 1));
    Cvor* node = koren;
    insertIntoQueue(q, node);
    while (!isEmptyQueue(q)) {
        removeFromQueue(q);
        for (int i = 0; i < node->br; i++)
        {
            if (node->pok[i]) insertIntoQueue(q, node->pok[i]);

        }
        for (int i = node->br; i <=m; i++)
        {
            node->pok[i] = NULL;
        }
        node->otac = NULL;
        free(node);
    }
}




int main() {
    int m;
    Cvor* koren;
    Baza* baza = NULL;

    int visina;
    long long int* niz;
    //printf("\n visina je %d\n", visina_stabla(koren));
    printf("Unesite red stabla: ");
    scanf("%d", &m);
    printf("\n");
    koren = napravi_stablo(m, "proba.txt");
    printf("Stablo je napravljeno!\n");
   

    printf("\n----------------------------------------\n");
    printf("[1] Ispis indeksne strukture\n");
    printf("[2] Dodavanje novog zapisa\n");
    printf("[3] Pretrazivanje jednog podatka\n");
    printf("[4] Pretrazivanje k podataka\n");
    printf("[5] Pretrazivanje k sukcesivnih podataka\n");
    printf("[6] Unisti indeksnu strukturu\n");
    printf("[7] Kraj programa\n");
    printf("\n----------------------------------------\n");

    
    
    
    while (1)
    {
        int opcija;
        printf("Izaberite jednu od stavki: ");
        scanf("%d", &opcija);
        printf("\n");
        switch (opcija) {

        case 1:
            visina = visina_stabla(koren);
            ispis_stablaa(koren, visina);
            break;


        case 2:
            printf("\nUnesite kljuc koji zelite da dodate: ");
            long long int kljuc;
            scanf("%lld", &kljuc);
            koren = unesi_kljuc(koren, m, kljuc, baza);
            break;

        case 3:
            printf("\nUnesite kljuc koji zelite da pretrazite: ");
            long long int key;
            scanf("%lld", &key);
            printf("\nBroj poredjenja do nalazenja kljuca %lld je %d\n", key, pretraga_kjluca(koren, key, "proba.txt", 1));
            break;


        case 4:
            printf("\nUnesite koliko kljuceva zelite da pretrazite: ");
             int k;
            scanf("%d", &k);
            niz = (long long int*)malloc(k * sizeof(long long int));
            if (niz == NULL) {
                printf("Neuspela alokacija!\n");
                exit(1);
            }

            printf("Unesite vrednosti kljuceva koje pretrazujete: ");
            for (int i = 0; i < k; i++)
            {
                scanf("%lld", &niz[i]);
            }
            printf("\nBroj ukupnih poredjenja tokom pretraga kljuceva je %d\n", pretrazi_kljuceve_neuredjene(niz, k, koren, "proba.txt"));
            break;

        case 5:
            printf("\nUnesite koliko koliko kljuceva pretrazujete: ");
            int K;
            scanf("%d", &K);
            printf("\nUnesite pocetni primarni kljuc koji se pretrazuje: ");
            long long int klj;
            scanf("%lld", &klj);
            printf("\nUkupan broj poredjenja je %d\n", pretrazi_kljuceve_uredjeno(K, klj, koren, "proba.txt"));
            break;

        case 6:
            printf("Indeksna struktura je obrisana");
            obrisi_stablo(koren, m);

        case 7:
            printf("Kraj programa!\n");
            exit(1);
            break;
        default:
            printf("Izabrali ste pogresan broj, unesite ponovo\n");
            break;
        }
    }
    return 0;
}

/*43000000001|4300000001|4300000001|Joshua Fowle Savings Account|1|6334235.05
43000000002|4300000003|4300000001|Joshua Fowle Play Money|2|2731646.43
43000000003|4300000004|4300000001|Joshua Fowle Emergency Expenses|2|7625683.59
43000000004|4300000005|4300000001|Joshua Fowle Vacation Account|2|79823.44
43000000005|4300000010|4300000001|Joshua Fowle Healthcare Fund|2|1104436.03
43000000006|4300000008|4300000001|Joshua Fowle New Car Account|1|1594842.08
43000000011|4300000004|4300000002|Willie Swigert Joint Account|1|-9093725.95
43000000012|4300000003|4300000002|Willie Swigert Business Account|1|9288893.44
43000000013|4300000007|4300000002|Willie Swigert College Fund|1|8096128.80
43000000014|4300000006|4300000002|Willie Swigert Flexible Spending|0|9638415.62
43000000015|4300000001|4300000002|Willie Swigert Play Money|1|-1877283.56
43000000016|4300000002|4300000002|Willie Swigert Emergency Expenses|2|7588354.79
43000000017|4300000005|4300000002|Willie Swigert Vacation Account|2|7102893.94
43000000018|4300000008|4300000002|Willie Swigert Healthcare Fund|2|7079673.10
43000000021|4300000001|4300000003|Amos Labree Individual Account|1|-8591065.12
43000000022|4300000006|4300000003|Amos Labree Family Trust|1|2859736.31
43000000023|4300000007|4300000003|Amos Labree Retirement Fund|0|6271643.53
43000000024|4300000002|4300000003|Amos Labree Joint Account|2|-7002828.09
43000000025|4300000009|4300000003|Amos Labree Business Account|1|4997031.51
43000000031|4300000005|4300000004|Charlotte Ciciora Healthcare Fund|1|1787356.81

*/