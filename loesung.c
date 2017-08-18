#include <stdio.h>
#include <stdlib.h>
#include <memory.h>



typedef struct{
    int toID;
    unsigned int cost;
}EdgeBlock;

typedef struct {
    EdgeBlock* edgelist;
    EdgeBlock* fromedgelist;
    int edgelength1,edgelength2;
    int index1,index2;

}GraphBlock;

typedef struct{
    int ID;
    unsigned int dayleft;
}NodeDay;


typedef struct{
    NodeDay* stack;
    int length;
    int current;
}Stack;

typedef struct{
    NodeDay* stack;
    int length;
    int current;
}PQ;

PQ MAINPQ;
unsigned int STARTID,ENDID, DMAX;
GraphBlock** GRAPHSTART;
int GRAPHSIZE,MAXNODE,*HUTLIST, HUTLENGTH,*HITLIST,*HITLIST2,*DONELIST,*DONELIST2;

int initPQ(PQ *inqueue)
{
    NodeDay* newstack=malloc(sizeof(NodeDay)*(MAXNODE+1));
    if(inqueue==NULL || newstack==NULL)
        return 1;
    inqueue->stack=newstack;
    inqueue->length=MAXNODE+1;
    inqueue->current=0;
    return 0;
}
void siftUP(PQ *inqueue, int index)
{
    NodeDay swap;
    int parentindx=(index-1)/2;
    while(parentindx>=0 && inqueue->stack[index].dayleft < inqueue->stack[parentindx].dayleft)
    {
        swap=inqueue->stack[parentindx];
        inqueue->stack[parentindx]=inqueue->stack[index];
        inqueue->stack[index]=swap;
        index=parentindx;
        parentindx=(index-1)/2;
    }

}
int enqueue(PQ *inqueue, NodeDay node)
{
    NodeDay* newstack;
    if(inqueue->current==inqueue->length)
    {
        newstack=realloc(inqueue->stack,(size_t)inqueue->length*2);
        if(newstack==NULL)
            return 1;
        inqueue->length*=2;
        inqueue->stack=newstack;
    }
    inqueue->stack[inqueue->current]=node;
    siftUP(inqueue,inqueue->current);
    inqueue->current++;
    return 0;
}

void siftDOWN(PQ *inqueue, int index)
{
    NodeDay swap;
    int childindx=index*2+1;
    while(childindx<inqueue->current)
    {

        if(childindx+1 < inqueue->current )
           {
               if(inqueue->stack[index].dayleft > inqueue->stack[childindx].dayleft &&
                  inqueue->stack[index].dayleft > inqueue->stack[childindx+1].dayleft  )
               {
                   if(inqueue->stack[childindx].dayleft > inqueue->stack[childindx+1].dayleft)
                   {
                       swap=inqueue->stack[++childindx];
                   }
                   else
                   {
                       swap=inqueue->stack[childindx];
                   }
               }
               else
               {
                   if(inqueue->stack[index].dayleft > inqueue->stack[childindx].dayleft)
                       swap=inqueue->stack[childindx];
                   else
                       if(inqueue->stack[index].dayleft > inqueue->stack[childindx+1].dayleft)
                           swap=inqueue->stack[++childindx];
                       else
                            return;
               }


            inqueue->stack[childindx]=inqueue->stack[index];
            inqueue->stack[index]=swap;
        }
        else
        {

            if(inqueue->stack[index].dayleft <= inqueue->stack[childindx].dayleft)
                return;
            swap=inqueue->stack[childindx];
            inqueue->stack[childindx]=inqueue->stack[index];
            inqueue->stack[index]=swap;
        }


        index=childindx;
        childindx=index*2+1;
    }

}

NodeDay* dequeue(PQ *inqueue)
{
    NodeDay* ret= malloc(sizeof(NodeDay));
    if(ret==NULL || inqueue->current==0)
    {
	free(ret);
        return NULL;
    }
    ret->dayleft=inqueue->stack[0].dayleft;
    ret->ID=inqueue->stack[0].ID;
    inqueue->stack[0]=inqueue->stack[--inqueue->current];
    siftDOWN(inqueue,0);

    return ret;
}



void setCheck(int* array,int indicie)
{
    if(array==NULL)
        return;
    array[(int)(indicie/(sizeof(int)*8))]|=1<<(indicie%(sizeof(int)*8));
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
    EdgeBlock *edgepointer;
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
    GraphBlock *nodepointer;


    for (int i=0;i<GRAPHSIZE;i++)
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
 * Die Funktion vergroessert die Kantenliste von einem Knoten zwaifach
 * @param nodepointer-pointer auf dem Knoten. index- hin oder her kommende Kantenliste
 * @return 0 ERFOLG 1 MISSGLUECK
 */
//TODO: finish the function to enlarge the list of edges.
int enlargeEdgeList(GraphBlock* nodepointer,int index)
{
    EdgeBlock *newlist;
    int edgelength1=nodepointer->edgelength1,edgelength2=nodepointer->edgelength2;
    
    if(index==0)
    {
    //vergrossere die Liste
    newlist=realloc(nodepointer->edgelist,sizeof(EdgeBlock)*edgelength1*2);
    if(newlist==NULL)
    {
        printf("Failed to allocate new Edgelist (enlargeEdgelist)\n");
        return 1;
    }
    nodepointer->edgelist=newlist;
    //erzeuge Nullen
    newlist=calloc((size_t)edgelength1,sizeof(EdgeBlock));
    if(newlist==NULL)
    {
        printf("Failed to allocate new Edgelist (enlargeEdgelist)\n");
        return 1;
    }
    //fulle die neuen Platz mit Nullen aus
    memcpy(nodepointer->edgelist+edgelength1,newlist,(size_t)edgelength1);
    free(newlist);
    nodepointer->edgelength1=edgelength1*2;
    }
    else
    {
    //Dieselbe fuer die zweite Kanten Liste

    newlist=realloc(nodepointer->fromedgelist,sizeof(EdgeBlock)*edgelength2*2);
    if(newlist==NULL)
    {
        printf("Failed to allocate new Edgelist (enlargeEdgelist)\n");
        return 1;
    }
    nodepointer->fromedgelist=newlist;
    //erzeuge Nullen
    newlist=calloc((size_t)edgelength2,sizeof(EdgeBlock));
    if(newlist==NULL)
    {
        printf("Failed to allocate new Edgelist (enlargeEdgelist)\n");
        return 1;
    }
    //fulle die neuen Platz mit Nullen aus
    memcpy(nodepointer->fromedgelist+edgelength2,newlist,(size_t)edgelength2);
    free(newlist);
    nodepointer->edgelength2=edgelength2*2;
    }
    return 0;
}
/**
 * Die Funktion addiert einen Knoten und Kanten zu dem Graph per Zeile
 * @param values Die gegeben gelesene Zeile von Datei
 * @return 0 ERFOLG 1 MISSGLUECK
 */

int addNode(unsigned int* values){
    GraphBlock *newNode;
    EdgeBlock *newEdge, *secEdge;

    if(values==NULL)
    {
        printf("Invalid read from File (addNode)\n");
        return 1;
    }
    //Wenn der Knoten nicht im Graph existiert, erzeuge ich den Knoten.
    if(GRAPHSTART[values[0]]==NULL)
    {
        //erzeuge neuen Knoten
        newNode=malloc(sizeof(GraphBlock));
        //erzeuge eine Liste fuer 5 Kanten
        newEdge=calloc((size_t)5,sizeof(EdgeBlock));
        secEdge=calloc((size_t)5,sizeof(EdgeBlock));

        if(newNode==NULL || newEdge==NULL || secEdge==NULL)
        {
            printf("Failed to allocate memory (addNode)\n");
            return 1;
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
            if(enlargeEdgeList(newNode,0)==1)
                return 1;
        }

        newNode->edgelist[newNode->index1].toID=values[1];
        newNode->edgelist[newNode->index1++].cost=values[2];

    }

    if(GRAPHSTART[values[1]]==NULL)
    {
        //erzeuge neuen Knoten
        newNode=malloc(sizeof(GraphBlock));
        //erzeuge eine Liste fuer 5 Kanten
        newEdge=calloc((size_t)5,sizeof(EdgeBlock));
        secEdge=calloc((size_t)5,sizeof(EdgeBlock));

        if(newNode==NULL || newEdge==NULL || secEdge==NULL)
        {
            printf("Failed to allocate memory (addNode)\n");
            return 1;
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
            if(enlargeEdgeList(newNode,1)==1)
                return 1;
        }

        newNode->fromedgelist[newNode->index2].toID=values[0];
        newNode->fromedgelist[newNode->index2++].cost=values[2];

    }


    return 0;
}

/**
 * Die Funktion vergrosst die globale Liste der Knoten
 * @return 0 ERFOLG 1 MISSGLUECK
 */
int enlargeLists()
{

    int newGSize; //neue Grosse der Listen
    int failed=0;
    GraphBlock **newGraph=NULL;//neue Knotenliste

    if(GRAPHSTART==NULL || HUTLIST==NULL)
    {
        printf("The Graph or Hut list is not initiated (enlargeLists)\n");
    	return 1;
    }
    //************vergroesse die Knoten Liste
    
	    if(MAXNODE*2<10000000)
		    newGSize=MAXNODE*10;
	    else
		    failed=1;
	
    if(!failed)
   	 newGraph=realloc(GRAPHSTART,sizeof(GraphBlock*)*newGSize);

    if(newGraph==NULL)
    {
        printf("FAILED to allocate memmory (enlargeLists)\n");
        return 1;
    }

    GRAPHSTART=newGraph;
    newGraph=calloc((size_t)newGSize-GRAPHSIZE,sizeof(GraphBlock*));

    if(newGraph==NULL)
    {
        printf("FAILED to allocate memmory (enlargeLists)\n");
        return 1;
    }
    //fulle den neuen Platz mit Nullen
    memcpy(GRAPHSTART+GRAPHSIZE,newGraph,sizeof(GraphBlock*)*(newGSize-GRAPHSIZE));
    GRAPHSIZE=newGSize;
    free(newGraph);



    return 0;

}

/**
 * Die Funktion liest und prueft die Datei, und baut die Knoten Liste und Hutte Liste
 * @param fp pointer zu der Datei (ich gebe STDIN ueber); firstmax der grossere Knoten von Start- und End- Knoten
 * @return 0 ERFOLD 1 MISSGLUECK
 */
int initiateGraph(FILE* fp)
{
    unsigned int ** values,*valuesArray,getvalue,getvalue1; // eingelesene Werte
    int* newhutlist;
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
            if(getvalue/(sizeof(int)*8)>=HUTLENGTH) {

                printf("realloc %d ", HUTLENGTH);
		    newhutlist=realloc(HUTLIST,(size_t)HUTLENGTH*2);
                if(newhutlist==NULL)
                {
                    printf("Failed to enlarge HUTLIST (addNode)\n");
                    errorFlag=1;
		    free(values[0]);
            	    free(values);
		    continue;
                }
		for(int i=0;i<HUTLENGTH;i++)
			newhutlist[i]=0;
		
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
                if(enlargeLists()==-1)
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

    return 0;

ERROR:
	printf("line: %d \n",linesread);
        return 1;

}

void oneRecursiv()
{
    NodeDay* node=dequeue(&MAINPQ);
    NodeDay newnode;
    while(node!=NULL) {


        if(isCheck(DONELIST,node->ID)==0)
        {
            setCheck(DONELIST, node->ID);

            if (isCheck(HUTLIST, node->ID)) {
                setCheck(HITLIST, node->ID);
            }

            if (GRAPHSTART[node->ID] != NULL ) {
                for (int i = 0; i < GRAPHSTART[node->ID]->index1; i++) {

                    if(isCheck(DONELIST,GRAPHSTART[node->ID]->edgelist[i].toID))
			    continue;
		    
                    if(node->dayleft > (node->dayleft + GRAPHSTART[node->ID]->edgelist[i].cost))
			    continue;

                    if((node->dayleft + GRAPHSTART[node->ID]->edgelist[i].cost) > DMAX)
                        continue;
		    
			
                    newnode.ID = GRAPHSTART[node->ID]->edgelist[i].toID;
                    newnode.dayleft = node->dayleft + GRAPHSTART[node->ID]->edgelist[i].cost;


                    if(enqueue(&MAINPQ, newnode))
                    {
                        printf("Priority Queue is too big \n");
                        free(node);
			return;
		    }
                }

            }
        }
        free(node);
        node=dequeue(&MAINPQ);
    }
}

void backRecursiv()
{
    NodeDay* node=dequeue(&MAINPQ);
    NodeDay newnode;
    unsigned int checkvalue;
    while(node!=NULL) {

        if(isCheck(DONELIST2,node->ID)==0 )
        {
            setCheck(DONELIST2, node->ID);

            if (isCheck(HUTLIST, node->ID)) {
                setCheck(HITLIST2, node->ID);
            }

            if (GRAPHSTART[node->ID] != NULL) {
                for (int i = 0; i < GRAPHSTART[node->ID]->index2; i++) {

                   if (isCheck(DONELIST2,GRAPHSTART[node->ID]->fromedgelist[i].toID))
                        continue;

                    checkvalue=node->dayleft + GRAPHSTART[node->ID]->fromedgelist[i].cost;

                    if (node->dayleft > checkvalue)
                       continue;

                    if((node->dayleft + GRAPHSTART[node->ID]->fromedgelist[i].cost) > DMAX)
                        continue;

                    newnode.ID = GRAPHSTART[node->ID]->fromedgelist[i].toID;
                    newnode.dayleft = node->dayleft + GRAPHSTART[node->ID]->fromedgelist[i].cost;

                    if(enqueue(&MAINPQ, newnode))
                    {
                        printf("Priority Queue is too big \n");
                        free(node);
			return;
		    }
                }

            }
        }
        free(node);
        node=dequeue(&MAINPQ);
    }
}


int DFSGraph()
{
    NodeDay start={STARTID,0};
    NodeDay* point;
    	
    HITLIST=calloc((size_t)HUTLENGTH,sizeof(int));
    HITLIST2=calloc((size_t)HUTLENGTH,sizeof(int));
    DONELIST=calloc((size_t)HUTLENGTH,sizeof(int));
    DONELIST2=calloc((size_t)HUTLENGTH,sizeof(int));

    initPQ(&MAINPQ);
    enqueue(&MAINPQ,start);
    
    oneRecursiv();
    point=dequeue(&MAINPQ);
       
    while(point!=NULL)
    {
	free(point);
      	point=dequeue(&MAINPQ);
    }
    
    start.dayleft=0;start.ID=ENDID;
    enqueue(&MAINPQ,start);
    backRecursiv();
    
    for(int i=0;i<=MAXNODE;i++)
    { 
	    if(isCheck(HITLIST,i) && isCheck(HITLIST2,i) )  {
            	printf("%d\n",i);
        }
    }
    
    point=dequeue(&MAINPQ);
    while(point!=NULL)
    {
	free(point);
      	point=dequeue(&MAINPQ);
    }
    
    free(MAINPQ.stack);
    free(HITLIST);
    free(HITLIST2);
    free(DONELIST);
    free(DONELIST2);

    return 0;
}

int main(int argc, char* argv[])
{
        unsigned int** values; // Die Zeile die wir lesen.
        int initialsize; // Die upspruengliche Grosse 
        int statusvalue; // Der zuruekgegebene Wert der Funktion

	HUTLIST=NULL;GRAPHSTART=NULL;

	//Lese die Datei
        FILE* readfile= stdin;

        //Lese die erste Zeile
       if(readfile==NULL)
       {
              printf("Invalid file \n");
	       return 1;
       }
       	values=getValidLine(readfile);

        if(values[0]==NULL)
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
        initialsize=1000>initialsize?1000:initialsize;

	//Die erste Grosse der Graph ist zweimal den maximalen Knoten
        GRAPHSIZE=(initialsize+1)*2;
	
        //Die Huetten sind in ein BIT muster gespeichert, jeder Bit ist ein Index. so es ist GRAPHSIZE/32(normalerweise)bit + 1
        HUTLENGTH=(GRAPHSIZE/(sizeof(int)*8))+1;

        //Wir reservieren so viel Platz wie oben fuer Huetteliste und Knotenliste
        HUTLIST=calloc((size_t)HUTLENGTH,sizeof(int));
        GRAPHSTART=calloc((size_t)GRAPHSIZE,sizeof(GraphBlock*));

        if(HUTLIST==NULL || GRAPHSTART==NULL)
        {
            printf("Failed to allocate memory\n");
            return 1;
        }
        //Wir lesen die Datei und speichern den grossesten KnotenID
        statusvalue=initiateGraph(readfile);

        if(statusvalue==1)
        {
            printf("Error initiating Graph. \n");
	    printf("maxnode=%d graphsize=%d \n",MAXNODE,GRAPHSIZE);
	    freeGraph();
	    free(GRAPHSTART);
	    free(HUTLIST);
	    fclose(readfile);

            return 1;
        
	}
        
	DFSGraph();
	freeGraph();
        //befreie die Liste von Knoten
        free(GRAPHSTART);
        //befreie die Liste von Huetten
        free(HUTLIST);
        fclose(readfile);
        return 0;

}
