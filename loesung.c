#include <stdio.h>
#include <stdlib.h>
#include <memory.h>



struct EdgeBlock{
    int toID;
    unsigned int cost;
};
struct GraphBlock {
    struct EdgeBlock* edgelist;
    struct EdgeBlock* fromedgelist;
    int edgelength1,edgelength2;
    int index1,index2;

};

struct NodeDay{
    int ID;
    unsigned int dayleft;
};

struct NodeSuccess{
    int ID;
    unsigned int dayleft;
    int* success;
};

struct Queue{
    void* queue;
    int length;
};

unsigned int STARTID,ENDID, DMAX;
struct GraphBlock** GRAPHSTART;
int GRAPHSIZE,MAXNODE,*HUTLIST, HUTLENGTH;

void initQueue(struct Queue *queue)
{
    queue->queue=NULL;
    queue->length=0;
}
int isQueueEmpty(struct Queue *queue)
{
    if(queue->length==0)
        return 1;
    else
        return 0;
}
int enqueueSuc(struct Queue *queue,struct NodeSuccess node)
{
    struct NodeSuccess* newarray=malloc(sizeof(struct NodeSuccess)*(queue->length+1));
    struct NodeSuccess* oldqueue=newarray+1;
    if(newarray==NULL)
        return -1;

    newarray[0]=node;
    if(queue->queue!=NULL)
        memcpy(oldqueue,queue->queue,sizeof(struct NodeSuccess)*queue->length);
    free(queue->queue);
    queue->queue=newarray;
    queue->length++;

    return 1;
}
int enqueue(struct Queue *queue,struct NodeDay node)
{
    struct NodeDay* newarray=malloc(sizeof(struct NodeDay)*(queue->length+1));
    struct NodeDay* oldqueue=newarray+1;
    if(newarray==NULL)
    return -1;

    newarray[0]=node;
    if(queue->queue!=NULL)
        memcpy(oldqueue,queue->queue,sizeof(struct NodeDay)*queue->length);
    free(queue->queue);
    queue->queue=newarray;
    queue->length++;

    return 1;
}
struct NodeSuccess* dequeueSuc(struct Queue *queue)
{
    struct NodeSuccess *node=malloc(sizeof(struct NodeSuccess)), *allocpoint,*helpointer;
    if(queue->length>0)
    {
        helpointer=(struct NodeSuccess*)queue->queue;
        node->ID=helpointer->ID;
        node->dayleft=helpointer->dayleft;
        queue->length--;
        if(queue->length==0)
            queue->queue=NULL;
        else {
            allocpoint=malloc(sizeof(struct NodeSuccess)*queue->length);
            memcpy(allocpoint,helpointer+1,sizeof(struct NodeSuccess)*queue->length);
            if(allocpoint==NULL)
                return NULL;
            free(helpointer);
            queue->queue=allocpoint;

        }
        return node;
    }
    else
        return NULL;

}
struct NodeDay* dequeue(struct Queue *queue)
{
    struct NodeDay *node=malloc(sizeof(struct NodeDay)), *allocpoint,*helpointer;
    if(queue->length>0)
    {
        helpointer=(struct NodeDay*)queue->queue;
        node->ID=helpointer->ID;
        node->dayleft=helpointer->dayleft;
        queue->length--;
        if(queue->length==0)
            queue->queue=NULL;
        else {
            allocpoint=malloc(sizeof(struct NodeDay)*queue->length);
            memcpy(allocpoint,helpointer+1,sizeof(struct NodeDay)*queue->length);
            if(allocpoint==NULL)
                return NULL;
            free(helpointer);
            queue->queue=allocpoint;

        }
        return node;
    }
    else
        return NULL;

}
void setCheck(int* array,int indicie)
{
    if(array==NULL)
        return;
    array[indicie/(sizeof(int)*8)]|=1<<(indicie%(sizeof(int)*8));
}
int isCheck(int* array,int indicie)
{
    if(array==NULL)
        return 0;
    if(array[indicie/(sizeof(int)*8)]&(1<<(indicie%(sizeof(int)*8))))
        return 1;
    else
        return 0;
}

//Input: Ein Pointer zur Datei
//Output: Ein Array mit den Werten der Knoten, und Uberganggebuehr
unsigned int** getValidLine(FILE* fp)
{
    

    char c,lastdigit=0; //Eingabe Char
    unsigned int count=0;
    unsigned int *num=malloc(sizeof(unsigned int)); //Indicie des Chars, und unsigned int den wir anehmen von der Datei
    unsigned int **list=malloc(3* sizeof(unsigned int*)); // Bekommen wir hier 3 oder 1 wert zurueck;
    //Lesen ein Char
    c=(char)fgetc(fp);
    list[0]=NULL;list[1]=NULL;list[2]=NULL;
    *num=0;

    //Wenn Pointer zeigt auf keine Datei
    if(fp==NULL) {
        printf("The File is invalid");
        goto ERROR;
    }

    //Wir lesen bis:
    // wir zum Ende der Datei sind
    // die Zahl ist zu gross
    // eine neue Linie kommt
    // ein Space ist betroffen

    while(c!=EOF && count<10 && c!='\n' && c!=' ' ) {
        if (c > '9' || c < '0') {
            printf("No1: Invalid character. \n");
            goto ERROR;
        }
        (*num) = (*num) * 10 + (c - '0');
        //das N MAX fuer 1GB speicherplatz ist 2 hoch 27, weil wir 4 byte fuer Knoten und 4 byte fuer gebuehr per Kante speichern
        if (*num > 1 << 27)
        {
            printf("No1: Value is too big. \n");
            goto ERROR;
        }
        if(count==0)
            count++;

        c=(char)fgetc(fp);
    }
	
	//Das Ende der Datei
    if(count==0 && c==EOF) {
        free(list);
        free(num);
        return NULL;
    }

    if( count==0 || count==10 )
    {
        printf("No1: Value is too big. \n");
        goto ERROR;
    }

    list[0] = num;

    //Nur eine Zahl ist gelesen, dh eine Zeile wo nur ein Knoten vorkommt.
    if(c=='\n'  || c==EOF) {
        return list;
    }

    count=0;
    num=malloc(sizeof(*num));
    *num=0;
    c=(char)fgetc(fp);

    // Falls es ein WhiteSpace gibt, mussen noch genau zwei Zahlen vorkommen
    while(c!=EOF && count<10 && c!='\n' && c!=' ' )
    {
        if(c> '9' || c<'0')
        {
            printf("No2: Invalid character. \n");
            goto ERROR;
        }
        (*num) = (*num) * 10 + (c - '0');
        //das N MAX fuer 1GB speicherplatz ist 2 hoch 27, weil wir 4 byte fuer Knoten und 4 byte fuer gebuehr per Kante speichern
        if (*num > 1 << 27)
        {
            printf("No2: Value is too Big \n");
            goto ERROR;
        }

        if(count==0)
            count++;
        c=(char)fgetc(fp);
    }

    if(count==0 || count==10 || c=='\n' || c==EOF ) {
        printf("No2: invalid format. \n");
        goto ERROR;
    }

    list[1] = num;

    count=0;
    num=malloc(sizeof(*num));
    *num=0;

    c=(char)fgetc(fp);
    //Jetzt wir die dritte Zahl einlesen
    while(c!=EOF && count<10 && c!='\n')
    {
	if(c==' ')
	{
		printf("Exceeded the number of inputs. \n");
		goto ERROR;
	}

        if(c> '9' || c<'0')
        {
            printf("No3: Invalid character. \n");

            goto ERROR;
        }

	        if(count==0)
            lastdigit=c;

        if(count==9)
            if(lastdigit>'3')
            {
                printf("No3: Value too Big. \n");
                goto ERROR;
            }
        (*num)=(*num)*10+(c-'0');
        count++;

        c=(char)fgetc(fp);
    }

    if(count==0 || count==11 ) {
        printf("No3: Value is too Big. \n");
        goto ERROR;
    }

    list[2] = num;
    return list;

    ERROR:
        free(list[0]);
	free(list[1]);
	free(list[2]);
	free(num);
        list[0]=NULL;
	return list;
}

/**
 * Zeichne den Graph und alle seiner Knoten
 */
void printGraph(int maxnode)
{
    struct EdgeBlock *edgepointer;
    for (int i=0;i<maxnode;i++)
    {
        //Wenn der Knote existiert
       if(GRAPHSTART[i]!=NULL)
       {
           printf("ID: %d\n",i);
           edgepointer=GRAPHSTART[i]->fromedgelist;
           //Wenn die Kante existiert.
           if(edgepointer!=NULL) {
               for (int j = 0; j < GRAPHSTART[i]->index2; j++)
                   printf("    -> %d cost: %d \n", edgepointer[j].toID, edgepointer[j].cost);
           }
       }

    }
}
/**
 * Die Funktion befreit den Graph
 */
void freeGraph()
{
    struct GraphBlock *nodepointer;


    for (int i=0;i<=MAXNODE;i++)
    {
        //zeige auf den Knoten am i
        nodepointer=GRAPHSTART[i];
        //wenn der Knoten existiert
        if(nodepointer!=NULL){
            //befreie die seine Liste von Kanten
            free(nodepointer->edgelist);
            free(nodepointer->fromedgelist);
            //befreie den Knoten
            free(nodepointer);
        }
    }


}

/**
 * Die Funktion vergroessert die Kanten liste von einem Knoten zwaifach
 * @param nodepointer pointer auf dem Knoten
 * @return 1 ERFOLG -1 MISSGLUECK
 */
//TODO: finish the function to enlarge the list of edges.
int enlargeEdgeList(struct GraphBlock* nodepointer)
{
    struct EdgeBlock *newlist;
    int edgelength1=nodepointer->edgelength1,edgelength2=nodepointer->edgelength2;
    //vergrossere die Liste
    newlist=realloc(nodepointer->edgelist,sizeof(struct EdgeBlock)*edgelength1*2);
    if(newlist==NULL)
    {
        printf("Failed to allocate new Edgelist (enlargeEdgelist)\n");
        return -1;
    }
    nodepointer->edgelist=newlist;
    //erzeuge Nullen
    newlist=calloc((size_t)edgelength1,sizeof(struct EdgeBlock));
    if(newlist==NULL)
    {
        printf("Failed to allocate new Edgelist (enlargeEdgelist)\n");
        return -1;
    }
    //fulle die neuen Platz mit Nullen aus
    memcpy(nodepointer->edgelist,newlist,(size_t)edgelength1);
    free(newlist);
    nodepointer->edgelength1=edgelength1*2;

    //Dieselbe fuer die zweite Kanten Liste

    newlist=realloc(nodepointer->fromedgelist,sizeof(struct EdgeBlock)*edgelength2*2);
    if(newlist==NULL)
    {
        printf("Failed to allocate new Edgelist (enlargeEdgelist)\n");
        return -1;
    }
    nodepointer->fromedgelist=newlist;
    //erzeuge Nullen
    newlist=calloc((size_t)edgelength2,sizeof(struct EdgeBlock));
    if(newlist==NULL)
    {
        printf("Failed to allocate new Edgelist (enlargeEdgelist)\n");
        return -1;
    }
    //fulle die neuen Platz mit Nullen aus
    memcpy(nodepointer->fromedgelist,newlist,(size_t)edgelength2);
    free(newlist);
    nodepointer->edgelength2=edgelength2*2;
    return 1;
}
/**
 * Die Funktion addiert einen Knoten und Kanten zu dem Graph per Zeile
 * @param values Die gegeben gelesene Zeile von Datei
 * @return -1 FEHLER; 1 ERFOLG
 */

int addNode(unsigned int* values){
    struct GraphBlock *newNode;
    struct EdgeBlock *newEdge, *secEdge;

    if(values==NULL)
    {
        printf("Invalid read from File (addNode)\n");
        return -1;
    }
    //Wenn der Knoten nicht im Graph existiert, erzeuge ich den Knoten.
    if(GRAPHSTART[values[0]]==NULL)
    {
        //erzeuge neuen Knoten
        newNode=malloc(sizeof(struct GraphBlock));
        //erzeuge eine Liste fuer 5 Kanten
        newEdge=calloc((size_t)5,sizeof(struct EdgeBlock));
        secEdge=calloc((size_t)5,sizeof(struct EdgeBlock));

        if(newNode==NULL || newEdge==NULL || secEdge==NULL)
        {
            printf("Failed to allocate memory (addNode)\n");
            return -1;
        }
        //erzeuge die neue Kante an der ersten Stelle der Kanten Liste
        newEdge[0].toID=values[1];
        newEdge[0].cost=values[2];
        //setze die Laenge der Kantenliste als 1 und verknupfe den Knoten damit
        newNode->edgelength1=5;
        newNode->index1=1;
        newNode->edgelength2=5;
        newNode->index2=0;
        newNode->edgelist=newEdge;
        newNode->fromedgelist=secEdge;
        //setze den neuen Knoten in dem Graph
        GRAPHSTART[values[0]]=newNode;
    }
    else
    {
        //fuegen die Kante zum aktuellen Knoten
        newNode=GRAPHSTART[values[0]];

        //pruefe ob die Kanten Liste voll ist
        if(newNode->index1>=newNode->edgelength1)
        {
            if(enlargeEdgeList(newNode)==-1)
                return -1;
        }

        newNode->edgelist[newNode->index1].toID=values[1];
        newNode->edgelist[newNode->index1++].cost=values[2];

    }

    if(GRAPHSTART[values[1]]==NULL)
    {
        //erzeuge neuen Knoten
        newNode=malloc(sizeof(struct GraphBlock));
        //erzeuge eine Liste fuer 5 Kanten
        newEdge=calloc((size_t)5,sizeof(struct EdgeBlock));
        secEdge=calloc((size_t)5,sizeof(struct EdgeBlock));

        if(newNode==NULL || newEdge==NULL || secEdge==NULL)
        {
            printf("Failed to allocate memory (addNode)\n");
            return -1;
        }
        //erzeuge die neue Kante an der ersten Stelle der Kanten Liste
        newEdge[0].toID=values[0];
        newEdge[0].cost=values[2];
        //setze die Laenge der Kantenliste als 1 und verknupfe den Knoten damit
        newNode->edgelength1=5;
        newNode->index1=0;
        newNode->edgelength2=5;
        newNode->index2=1;
        newNode->edgelist=secEdge;
        newNode->fromedgelist=newEdge;
        //setze den neuen Knoten in dem Graph
        GRAPHSTART[values[1]]=newNode;
    }
    else
    {
        //fuegen die Kante zum aktuellen Knoten
        newNode=GRAPHSTART[values[1]];

        //pruefe ob die Kanten Liste voll ist
        if(newNode->index2>=newNode->edgelength2)
        {
            if(enlargeEdgeList(newNode)==-1)
                return -1;
        }

        newNode->fromedgelist[newNode->index2].toID=values[0];
        newNode->fromedgelist[newNode->index2++].cost=values[2];

    }


    return 1;
}

/**
 * Die Funktion vergrosst die globale Liste der Knoten
 * @return 1 - wenn erfolgreich; -1 - wenn es eine Fehler gibt
 */
int enlargeLists(int maxnode)
{

    int newGSize; //neue Grosse der Listen
    struct GraphBlock **newGraph;//neue Knotenliste

    if(GRAPHSTART==NULL || HUTLIST==NULL)
    {
        printf("The Graph or Hut list is not initiated (enlargeLists)\n");
    }
    //************vergroesse die Knoten Liste
    newGSize=(maxnode+1)*2;
    newGraph=realloc(GRAPHSTART,sizeof(struct GraphBlock*)*newGSize);

    if(newGraph==NULL)
    {
        printf("FAILED to allocate memmory (enlargeLists)\n");
        return -1;
    }

    GRAPHSTART=newGraph;
    newGraph=calloc((size_t)newGSize-GRAPHSIZE,sizeof(struct GraphBlock*));

    if(newGraph==NULL)
    {
        printf("FAILED to allocate memmory (enlargeLists)\n");
        return -1;
    }
    //fulle den neuen Platz mit Nullen
    memcpy(GRAPHSTART+GRAPHSIZE,newGraph,sizeof(struct GraphBlock*)*(newGSize-GRAPHSIZE));
    GRAPHSIZE=newGSize;
    free(newGraph);



    return 1;

}

/**
 * Die Funktion liest und prueft die Datei, und baut die Knoten Liste und Hutte Liste
 * @param fp pointer zu der Datei (ich gebe STDIN ueber); firstmax der grossere Knoten von Start- und End- Knoten
 * @return -1 FEHLER oder int max die Anzahl der Knoten
 */
int initiateGraph(FILE* fp)
{
    unsigned int ** values,*valuesArray,getvalue,getvalue1; // eingelesene Werte
    int* newhutlist;
    //int maxhut=GRAPHSIZE;
	    //int maxnode=firstmax; // die Grosse des Graphes und der Wert maximaler KnotenID
    int isValid;// Wert von addNode funktion
    int newlength,isNodesdone=0;
    int linesread=0;
    int errorFlag=0;

    if(GRAPHSTART==NULL || HUTLIST==NULL) {
        printf("GRAPH or HUT list are not initiated (readGraph)\n");
       	goto ERROR;
    }

    //liest eine Zeile von der Datei
    values = getValidLine(fp);
    if(values!=NULL && values[0]==NULL)
	    errorFlag=1;
    else
    	    linesread++;

    while(values!=NULL && errorFlag!=1)
    {    
	if(values[0]==NULL)
	{
		errorFlag=1;
		free(values);
		continue;
	}	
	   
        //wenn Es eine Zeile mit Huette Nummer
        if(values[1]==NULL)
        {
            if(isNodesdone==0)
            {
                newlength=(GRAPHSIZE/(sizeof(int)*8)+1);
                if(newlength>HUTLENGTH)
                {
                    newhutlist=calloc(newlength,sizeof(int));
                    if(newhutlist==NULL)
                    {
                        printf("Failed to enlarge HUTLIST (addNode)\n");
			errorFlag=1;
			free(values[0]);
            		free(values);
			continue;
                    }
		    free(HUTLIST);
                    HUTLIST=newhutlist;
                    HUTLENGTH=newlength;
		 
                }
                isNodesdone=1;
            }
            getvalue=*values[0];
            //Wenn die Listen zu klein sind, vergroessere ich die HueteListe
            if(getvalue>=HUTLENGTH) {

                newhutlist=realloc(HUTLIST,(size_t)HUTLENGTH*2);
                if(newhutlist==NULL)
                {
                    printf("Failed to enlarge HUTLIST (addNode)\n");
                    errorFlag=1;
		    free(values[0]);
            	    free(values);
		    continue;
                }
                HUTLIST=newhutlist;
                HUTLENGTH*=2;

            }
            //Merke dass dies eine Huette ist
            setCheck(HUTLIST,getvalue);

            free(values[0]);
            free(values);

        }
        else
        {
            //Pruefe ob ich mehr Platz fuer die neue Knoten brauche
            getvalue=*values[0];
            getvalue1=*values[1];

            if(getvalue1>getvalue)
                getvalue=getvalue1;
            // speichere die maximale Knoten ID
            if(getvalue>MAXNODE)
                MAXNODE=getvalue;

            //Wenn die Listen zu klein sind, vergroessere ich die Listen
            if(getvalue>=GRAPHSIZE) {
                if(enlargeLists(getvalue)==-1)
		{
		         printf("Error while enlarging Nodeslist. \n");
			 errorFlag=1;
			 free(values[0]);
                         free(values[1]);
                         free(values[2]);
                         free(values);
			 continue;
		}
                //maxnode = GRAPHSIZE;
            }

            //***Add neue Kante und Knoten zum Graph

            //Mache ein normaler Array von ints mit Werten von Array von Pointers values
            valuesArray=malloc(sizeof(int)*3);
            valuesArray[0]=*values[0];
            valuesArray[1]=*values[1];
            valuesArray[2]=*values[2];

            //Freischalte originale values Array von Pointer
            free(values[0]);
            free(values[1]);
            free(values[2]);
            free(values);

            isValid=addNode(valuesArray);
            free(valuesArray);

            if(isValid==-1)
	    {
		printf("Error while adding a new Node. \n");
	    	errorFlag=1;
		continue;
	    }
        }
        values = getValidLine(fp);
	linesread++;
    }

if(errorFlag)
	goto ERROR;

    return 1;

ERROR:
	printf("line: %d \n",linesread);
        return -1;

}



void getHuts(int size)
{
    struct Queue nodequeue,hutqueue,finalqueue;
    struct NodeDay *node=NULL,*newnode=NULL;
    struct GraphBlock *startBlock;
    unsigned int **costlist=calloc((size_t)size,sizeof(int*)),*newlist,**oldpointer,**newcostlist;
    int* hutchecklist= calloc((size_t)HUTLENGTH,sizeof(int)),*hutchecklist2= calloc((size_t)HUTLENGTH,sizeof(int));

    if(costlist==NULL||hutchecklist==NULL)
    {
        printf("Failed to allocate hutchecklist or costlist (getHuts)\n");
        return;
    }
    node=malloc(sizeof(struct NodeDay));

    node->ID=STARTID;
    node->dayleft=DMAX;

    initQueue(&finalqueue);
    initQueue(&nodequeue);
    initQueue(&hutqueue);


    enqueue(&nodequeue,*node);

    while(!isQueueEmpty(&nodequeue))
    {

        free(node);
        node=dequeue(&nodequeue);

        if(node!=NULL) {
            if(isCheck(HUTLIST,node->ID))
                setCheck(hutchecklist,node->ID);
            if (costlist[node->ID] == NULL){
                newlist=malloc(sizeof(int));
                if(newlist==NULL)
                {
                    printf("Failed to allocate memory costlist 1(getHuts)\n");
                    return;
                }
                costlist[node->ID]=newlist;
                *costlist[node->ID] = node->dayleft;
            }
            if(*costlist[node->ID] <= node->dayleft) {
                *costlist[node->ID] = node->dayleft;
                startBlock = GRAPHSTART[node->ID];
                if (startBlock != NULL) {
                    for (int i = 0; i < startBlock->index1; i++) {
                        if (node->dayleft >= startBlock->edgelist[i].cost ) {
                            newnode = malloc(sizeof(struct NodeDay));
                            if(newnode==NULL)
                            {
                                printf("Failed to allocate memory newnode (getHuts)\n");
                                return;
                            }
                            newnode->ID = startBlock->edgelist[i].toID;
                            newnode->dayleft = node->dayleft - startBlock->edgelist[i].cost;
                            enqueue(&nodequeue, *newnode);
                            free(newnode);
                        }
                    }

                }

            }
        }

    }

        oldpointer=costlist;
        newcostlist=calloc((size_t)size,sizeof(int*));

        if(newcostlist==NULL)
        {
            printf("Error to allocate memoryewcostlist (getHuts)\n ");
            return ;
        }
        costlist=newcostlist;
        node=malloc(sizeof(struct NodeDay));
        if (node == NULL) {
        printf("Failed to allocate memory node 1(getHuts)\n");
        return;}
        node->ID=ENDID;
        node->dayleft=DMAX;

    enqueue(&nodequeue,*node);

    while(!isQueueEmpty(&nodequeue)) {

        free(node);
        node = dequeue(&nodequeue);

        if (node != NULL) {
            if (isCheck(HUTLIST, node->ID))
                setCheck(hutchecklist2, node->ID);
            if (costlist[node->ID] == NULL) {
                newlist = malloc(sizeof(int));
                if (newlist == NULL) {
                    printf("Failed to allocate memory costlist 1(getHuts)\n");
                    return;
                }
                costlist[node->ID] = newlist;
                *costlist[node->ID] = node->dayleft;
            }
            if (*costlist[node->ID] <= node->dayleft) {
                *costlist[node->ID] = node->dayleft;
                startBlock = GRAPHSTART[node->ID];
                if (startBlock != NULL) {
                    for (int i = 0; i < startBlock->index2; i++) {
                        if (node->dayleft >= startBlock->fromedgelist[i].cost) {
                            newnode = malloc(sizeof(struct NodeDay));
                            if (newnode == NULL) {
                                printf("Failed to allocate memory newnode (getHuts)\n");
                                return;
                            }
                            newnode->ID = startBlock->fromedgelist[i].toID;
                            newnode->dayleft = node->dayleft - startBlock->fromedgelist[i].cost;
                            enqueue(&nodequeue, *newnode);
                            free(newnode);
                        }
                    }
                }
            }
        }
    }

    for(int i=size-1;i>=0;i--)
    {
        if(isCheck(hutchecklist,i)==isCheck(hutchecklist2,i)&& isCheck(hutchecklist2,i)==1)
        {
            newnode = malloc(sizeof(struct NodeDay));
            if (newnode == NULL) {
                printf("Failed to allocate memory newnode (getHuts)\n");
                return;
            }
            newnode->ID=i;
            newnode->dayleft=0;
            enqueue(&finalqueue,*newnode);
            free(newnode);
        }
    }
        while(!isQueueEmpty(&finalqueue))
        {
            node=dequeue(&finalqueue);
            printf("%d\n",node->ID);
            if(node!=NULL)
                free(node);
        }
        for(int i=0;i<size;i++) {
            free(costlist[i]);
            free(oldpointer[i]);
        }
        free(costlist);
        free(oldpointer);
        costlist=NULL;
        oldpointer=NULL;
}

int main(int argc, char* argv[])
{
        unsigned int** values; // Die Zeile die wir lesen.
        int initialsize; // Die upspruengliche Grosse 
	int statusvalue; // Der zuruekgegebene Wert der Funktion

        //Lese die Datei
        FILE* readfile= stdin;

        //Lese die erste Zeile
        values=getValidLine(readfile);

        if(values==NULL)
        {
            printf("Cannot read the first line in the file.\n");
            return 1;
        }

        //Speichere die erste Zeile, Startknote Endeknote und DMAX
        STARTID=*values[0];
        ENDID=*values[1];
        DMAX=*values[2];

        //Freischalte den Lesbuffer
        free(values[0]);
        free(values[1]);
        free(values[2]);
        free(values);

        //Max KnoteID am anfang is entweder Start- oder End- Knote
        initialsize=STARTID>ENDID?STARTID:ENDID;
	MAXNODE=initialsize;
        //Die erste Grosse der Graph ist zweimal den maximalen Knoten
        GRAPHSIZE=(initialsize+1)*2;
	
        //Die Huetten sind in ein BIT muster gespeichert, jeder Bit ist ein Index. so es ist GRAPHSIZE/32(normalerweise)bit + 1
        HUTLENGTH=GRAPHSIZE/(sizeof(int)*4)+1;

        //Wir reservieren so viel Platz wie oben fuer Huetteliste und Knotenliste
        HUTLIST=calloc((size_t)HUTLENGTH,sizeof(int));
        GRAPHSTART=calloc((size_t)GRAPHSIZE,sizeof(struct GraphBlock*));

        if(HUTLIST==NULL || GRAPHSTART==NULL)
        {
            printf("Failed to allocate memory\n");
            return 1;
        }
        //Wir lesen die Datei und speichern den grossesten KnotenID
        statusvalue=initiateGraph(readfile);

        if(statusvalue==-1)
        {
            printf("Error initiating Graph. \n");
	    printf("maxnode=%d graphsize=%d \n",MAXNODE,GRAPHSIZE);
	    freeGraph();
	    free(GRAPHSTART);
	    free(HUTLIST);
	    fclose(readfile);

            return 1;
        }
        //getHuts(maxnode);

   /* for(int i=0;i<=maxnode;i++) {
        if (isCheck(HUTLIST, i) == 1)
            printf("%d is Hut\n", i);
    }*/
        //printGraph(maxnode);
        printf("maxnode=%d graphsize=%d \n",MAXNODE,GRAPHSIZE);

	freeGraph();
        //befreie die Liste von Knoten
        free(GRAPHSTART);
        //befreie die Liste von Huetten
        free(HUTLIST);
        fclose(readfile);
        return 0;

}
