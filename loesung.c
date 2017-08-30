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
    NodeDay* queue;
    int length;
    int current;
}PQ;

//Die Globalevariabeln

PQ MAINPQ; //Prioritaetschalnge
unsigned int STARTID,ENDID, DMAX; //Start- und End- Knoten und DMAX
GraphBlock** GRAPHSTART; //Zeiger auf die Hauptdatenstruktur fuer den Graph
int GRAPHSIZE,MAXNODE; //Die Groesse des Graphens und die maximale KnotenID
int *HUTLIST, HUTLENGTH; // Bitliste der markierten Huetten, und die Groesse deren
int *HITLIST,*HITLIST2,*DONELIST,*DONELIST2; // Treffliste der Huetten und Knoten

//Ordnen Speicherplatz fuer die Prioritaetschlange zu
int initPQ(PQ *inqueue)
{
    NodeDay* newqueue=malloc(sizeof(NodeDay)*(MAXNODE+1));
    if(inqueue==NULL || newqueue==NULL)
        return 1;
    inqueue->queue=newqueue;
    inqueue->length=MAXNODE+1;
    inqueue->current=0;
    return 0;
}

//Aktualiesern den Heap, wenn wir einen neuen Knoten hinzufuegen
void siftUP(PQ *inqueue, int index)
{
    NodeDay swap;
    int parentindx=(index-1)/2;
    while(parentindx>=0 && inqueue->queue[index].dayleft < inqueue->queue[parentindx].dayleft)
    {
        swap=inqueue->queue[parentindx];
        inqueue->queue[parentindx]=inqueue->queue[index];
        inqueue->queue[index]=swap;
        index=parentindx;
        parentindx=(index-1)/2;
    }

}
//Fuegen einen neuen Knoten zu dem Heap, und aktualieseren den
int enqueue(PQ *inqueue, NodeDay node)
{
    NodeDay* newqueue;
    if(inqueue->current==inqueue->length)
    {
        newqueue=realloc(inqueue->queue,(size_t)inqueue->length*2);
        if(newqueue==NULL)
            return 1;
        inqueue->length*=2;
        inqueue->queue=newqueue;
    }
    inqueue->queue[inqueue->current]=node;
    siftUP(inqueue,inqueue->current);
    inqueue->current++;
    return 0;
}

//Aktualiesern den Heap, wenn wir einen neuen Knoten loeschen
void siftDOWN(PQ *inqueue, int index)
{
    NodeDay swap;
    int childindx=index*2+1;
    while(childindx<inqueue->current)
    {

        if(childindx+1 < inqueue->current )
           {
               if(inqueue->queue[index].dayleft > inqueue->queue[childindx].dayleft &&
                  inqueue->queue[index].dayleft > inqueue->queue[childindx+1].dayleft  )
               {
                   if(inqueue->queue[childindx].dayleft > inqueue->queue[childindx+1].dayleft)
                   {
                       swap=inqueue->queue[++childindx];
                   }
                   else
                   {
                       swap=inqueue->queue[childindx];
                   }
               }
               else
               {
                   if(inqueue->queue[index].dayleft > inqueue->queue[childindx].dayleft)
                       swap=inqueue->queue[childindx];
                   else
                       if(inqueue->queue[index].dayleft > inqueue->queue[childindx+1].dayleft)
                           swap=inqueue->queue[++childindx];
                       else
                            return;
               }


            inqueue->queue[childindx]=inqueue->queue[index];
            inqueue->queue[index]=swap;
        }
        else
        {

            if(inqueue->queue[index].dayleft <= inqueue->queue[childindx].dayleft)
                return;
            swap=inqueue->queue[childindx];
            inqueue->queue[childindx]=inqueue->queue[index];
            inqueue->queue[index]=swap;
        }


        index=childindx;
        childindx=index*2+1;
    }

}
//Speicheren den gewuenschten Knoten, loeschen aus dem Heap und aktualieseren den Heap. Danach geben wir den gespeicherten Knoten zurueck
NodeDay* dequeue(PQ *inqueue)
{
    NodeDay* ret= malloc(sizeof(NodeDay));
    if(ret==NULL || inqueue->current==0)
    {
	free(ret);
        return NULL;
    }
    ret->dayleft=inqueue->queue[0].dayleft;
    ret->ID=inqueue->queue[0].ID;
    inqueue->queue[0]=inqueue->queue[--inqueue->current];
    siftDOWN(inqueue,0);

    return ret;
}


//Markieren einen Bit an der Position indicie in dem Array
void setCheck(int* array,int indicie)
{
    if(array==NULL)
        return;
    array[(int)(indicie/(sizeof(int)*8))]|=1<<(indicie%(sizeof(int)*8));
}
//Pruefen ob ein Bit an der Position indicie in dem Array markiert ist
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
    char lastdigit=0; //Eingabe Char
	int c;
    unsigned int count=0;
    unsigned int *num=NULL; //Indicie des Chars, und unsigned int den wir anehmen von der Datei
    unsigned int **list=NULL; // Bekommen wir hier 3 oder 1 wert zurueck;
    //Lesen ein Char
       //Wenn Pointer zeigt auf keine Datei
    if(fp==NULL) {
        printf("The File is invalid.\n");
        goto ERROR;
    }

    num=malloc(sizeof(unsigned int));
    list=malloc(3* sizeof(unsigned int*)); 
    
    if(num== NULL|| list== NULL)
    {
        printf("Failed to allocate memory.\n");
        goto ERROR;
    }


    list[0]=NULL;list[1]=NULL;list[2]=NULL;
    *num=0;
   
    //Wir lesen bis:
    // wir zum Ende der Datei sind
    // die Zahl ist zu gross
    // eine neue Linie kommt
    // ein Space ist betroffen
	
    c=(char)fgetc(fp);

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
        
         count++;

        c=(char)fgetc(fp);
    }
	
	//Das Ende der Datei
    if(count==0 && c==EOF) {
        *num=EOF;
        list[0] = num;
        return list;
    }

    if( count==0 || count==10 )
    {
        printf("No1: Value is too big. \n");
        goto ERROR;
    }

    list[0] = num;

    //Nur eine Zahl ist gelesen, dh eine Zeile wo nur ein Knoten vorkommt.
    if(c=='\n' ) {
        return list;
    }
	//Falls der letzte Hutte mit EOF beendet ist
	if(c==EOF)
	{
		//printf("EOF\n");
		num=malloc(sizeof(unsigned int));
		if(num== NULL)
		{
			printf("Failed to allocate memory.\n");
			goto ERROR;
		}
		*num=EOF;
		list[1]=num;
		return list;
	}

    count=0;
    num=malloc(sizeof(*num));
    if(num==NULL)
    {
        printf("Failed to allocate memory.\n");
        goto ERROR;
    }
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
    if(num==NULL)
    {
        printf("Failed to allocate memory.\n");
        goto ERROR;
    }
    *num=0;

    c=(char)fgetc(fp);
    //Jetzt wir die dritte Zahl einlesen
    while(c!=EOF && count<10 && c!='\n' && c!='\r')
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
    if(c=='\r')
    {
        c=(char)fgetc(fp);
        if(c!='\n')
        {
            printf("Invalid character after \\r \n");
            goto ERROR;
        }
    }
    if(count==0)
    {
        printf("Invalid format: 2 values were given. \n");
        goto ERROR;
    }
    if( count==11 ) {
        printf("No3: Value is too Big. \n");
        goto ERROR;
    }

    list[2] = num;
    return list;

    ERROR:
    if(list!=NULL)
    {
        free(list[0]);
        free(list[1]);
        free(list[2]);
        list[0]=NULL;
    }

	free(num);

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
    int linesread=1;
    int errorFlag=0;

    if(GRAPHSTART==NULL || HUTLIST==NULL) {
        printf("GRAPH or HUT list are not initiated (readGraph)\n");
       	goto ERROR;
    }

    //liest eine Zeile von der Datei
    values = getValidLine(fp);
    if(values!=NULL && values[0]==NULL)
    {
        errorFlag=1;
        free(values);
    }
    else
    	linesread++;

    while(values!=NULL && errorFlag==0)
    {    
		if(values[0]==NULL)
		{
		
			errorFlag=1;
			free(values);
			continue;
		}
        if(*values[0]==EOF && values[1]==NULL && values[2]==NULL)
        {
            errorFlag=2;
	    
	   //Falls wir zum Ende gekommen sind, und es keine Hutten gibt 
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
	    
			free(values[0]);
            free(values);
            continue;
        }
	   
        //wenn Es eine Zeile mit Huette Nummer
        if(values[1]==NULL || *values[1]==EOF)
        {
			//Wenn die letzte Huette mit EOF beendet ist
			if(values[1]!=NULL && *values[1]==EOF)
			{
				errorFlag=2;
				free(values[1]);
			}
			
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
                if(enlargeLists()==1)
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
		if(errorFlag==0)
		{
			values = getValidLine(fp);
			linesread++;
		}
        
    }

	if(errorFlag<2)
	    goto ERROR;
    return 0;

ERROR:
	printf("line: %d \n",linesread);
    return 1;

}
/**
 * Die Funktion fuehrt DFS suche mit eine Prioritaetschlange. Wir suechen nach erreichbare Huette aus dem Startknoten.
 * Alle gefuendene Knoten sind in HITLIST markiert
 */
int oneRecursiv()
{
	//entnehme den Erstenknoten in der Schlange
    NodeDay* node=dequeue(&MAINPQ);
    NodeDay newnode;
    unsigned int checkvalue;
	
    while(node!=NULL) {

		
		//Falls wir diesen Knoten noch nicht getroffen haben
        if(isCheck(DONELIST,node->ID)==0)
        {
			
			//Merken den als getroffen
            setCheck(DONELIST, node->ID);

			
			//Merken wenn wir eine Huette begegnet haben
            if (isCheck(HUTLIST, node->ID)) {
                setCheck(HITLIST, node->ID);
            }

			
			//Falls dieser Knote Kanten hat
            if (GRAPHSTART[node->ID] != NULL ) {
				
				//Wir gehen durch die ausgehende Kanten des Knotens
                for (int i = 0; i < GRAPHSTART[node->ID]->index1; i++) {

					
					//Falls diese Kante fuehrt zu eine besuchte Knote, suchen wir nach der naechsten Kante
                    if(isCheck(DONELIST,GRAPHSTART[node->ID]->edgelist[i].toID))
						continue;
					
					checkvalue=node->dayleft + GRAPHSTART[node->ID]->edgelist[i].cost;
						
					//Falls es einen Ueberlauf bei der Addition gibt, suchen wir nach der naechsten Kante
                    if(node->dayleft > checkvalue)
						continue;
					
					//Falls die Kante mehr als DMAX kostet, suchen wir nach der naechsten Kante
                    if((node->dayleft + GRAPHSTART[node->ID]->edgelist[i].cost) > DMAX)
                        continue;
		    
					
					//Erzeugen den neuen Knoten mit der aktualisierten Zeit
                    newnode.ID = GRAPHSTART[node->ID]->edgelist[i].toID;
                    newnode.dayleft = node->dayleft + GRAPHSTART[node->ID]->edgelist[i].cost;

					//Fuegen zu der Schlange, und pruefen es keine Fehlern gab
                    if(enqueue(&MAINPQ, newnode))
                    {
                        printf("Priority Queue is too big \n");
                        free(node);
						return 1;
					}
                }

            }
        }
		
		//Schalten den besuchten Knoten frei, und nehmen den Naechsten
        free(node);
        node=dequeue(&MAINPQ);
    }
    return 0;
}

/**
 * Die Funktion fuehrt DFS suche mit eine Prioritaetschlange. Wir suechen nach erreichbare Huette aus dem EndKnoten.
 * Alle gefuendene Knoten sind in HITLIST2 markiert
 */
int backRecursiv()
{
	//entnehme den Erstenknoten in der Schlange
    NodeDay* node=dequeue(&MAINPQ);
    NodeDay newnode;
    unsigned int checkvalue;
	
	
    while(node!=NULL) {

		//Falls wir diesen Knoten noch nicht getroffen haben
        if(isCheck(DONELIST2,node->ID)==0 )
        {
			//Merken den als getroffen
            setCheck(DONELIST2, node->ID);

			//Merken wenn wir eine Huette begegnet haben
            if (isCheck(HUTLIST, node->ID)) {
                setCheck(HITLIST2, node->ID);
            }

			//Falls dieser Knote Kanten hat
            if (GRAPHSTART[node->ID] != NULL) {
                
				//Wir gehen durch die eingehende Kanten des Knotens
				for (int i = 0; i < GRAPHSTART[node->ID]->index2; i++) {

					//Falls diese Kante fuehrt zu eine besuchte Knote, suchen wir nach der naechsten Kante
					if (isCheck(DONELIST2,GRAPHSTART[node->ID]->fromedgelist[i].toID))
						continue;

					checkvalue=node->dayleft + GRAPHSTART[node->ID]->fromedgelist[i].cost;
						
					//Falls es einen Ueberlauf bei der Addition gibt, suchen wir nach der naechsten Kante
					if (node->dayleft > checkvalue)
						continue;

					//Falls die Kante mehr als DMAX kostet, suchen wir nach der naechsten Kante
					if((node->dayleft + GRAPHSTART[node->ID]->fromedgelist[i].cost) > DMAX)
						 continue;

					//Erzeugen den neuen Knoten mit der aktualisierten Zeit
					newnode.ID = GRAPHSTART[node->ID]->fromedgelist[i].toID;
					newnode.dayleft = node->dayleft + GRAPHSTART[node->ID]->fromedgelist[i].cost;

					//Fuegen zu der Schlange, und pruefen es keine Fehlern gab
					if(enqueue(&MAINPQ, newnode))
					{
						printf("Priority Queue is too big \n");
						free(node);
						return 1;
					}
				}

			}
        }
		//Schalten den besuchten Knoten frei, und nehmen den Naechsten
        free(node);
        node=dequeue(&MAINPQ);
    }
    return 0;
}

/**
 * Die Hauptsuchfunktion stosst die beide Suchverfahren und die Listen den getroffenen Knoten an.
 */
int DFSGraph()
{
	int status=0;
	//Der Startknote, den wir in die Prioritaetschalnge schieben
    NodeDay start={STARTID,0};
    MAINPQ.queue=NULL;
    	
	//Listen der getroffene und schon besuchte Knoten, bei der erste und zweite Suche.
    HITLIST=calloc((size_t)HUTLENGTH,sizeof(int));
    HITLIST2=calloc((size_t)HUTLENGTH,sizeof(int));
    DONELIST=calloc((size_t)HUTLENGTH,sizeof(int));
    DONELIST2=calloc((size_t)HUTLENGTH,sizeof(int));

    if(HITLIST==NULL || HITLIST2 == NULL || DONELIST==NULL || DONELIST2==NULL)
    {
	    status=1;
	    goto END;
    }
	//stossen die Schlange und fuegen den Startknoten hinzu
    if(initPQ(&MAINPQ))
    {
	    status=1;
	    goto END;
    }
    if(enqueue(&MAINPQ,start))
    {
	    status=1;
	    goto END;
    }
    
	//Aufrufe die erste Suche
    if(oneRecursiv())
    {
	    status=1;
	    goto END;
    }
	
	//Freie die Liste. (lasse einfach die Schalnge neu ueberschrieben) 
    MAINPQ.current=0;
	
	//fuege den Endknoten zu der Schlange hinzu
    start.dayleft=0;start.ID=ENDID;
    enqueue(&MAINPQ,start);
	
	//Aufrufe die zweite Suche, aus dem Endknoten zurueck
    if(backRecursiv())
    {
	    status=1;
	    goto END;
    }
    
	//Schreibe die Huette, die in beiden Suchen getroffen sind
    for(int i=0;i<=MAXNODE;i++)
    { 
	    if(isCheck(HITLIST,i) && isCheck(HITLIST2,i) )  {
            	printf("%d",i);
				if(i<MAXNODE)
					printf("\n");
        }
    }
    
    //freischalte alle Datenstrukturen, die in der Suche benuetzt sind
END:
    free(MAINPQ.queue);
    free(HITLIST);
    free(HITLIST2);
    free(DONELIST);
    free(DONELIST2);
    if(status) 
    	printf("Could not allocate more memory.\n");
    return status;
}

int main(int argc, char* argv[])
{
	unsigned int** values; // Die Zeile die wir lesen.
	int initialsize; // Die upspruengliche Grosse 
	int statusvalue; // Der zuruekgegebene Wert der Initiationsfunktion

	//einfuehre die Globalenvariabeln
	//Huetteliste
	HUTLIST=NULL;
	
	//Zieger auf die Hauptstruktur des Graphens GraphBlock
	GRAPHSTART=NULL;

	//Lese die Datei
	FILE* readfile= stdin;
    //readfile=fopen("graph4edit.txt", "r");
	//Lese die erste Zeile
	values=getValidLine(readfile);

	//Es gibt eine Fehler bei dem Lesen
	if(values==NULL)
	{
		printf("Cannot read the file.\n");
		return 1;
	}
	
	if(values[0]==NULL)
	{
		printf("Cannot read the first line in the file.\n");
		free(values);
		return 1;
	}
	if(*values[0]==EOF && values[1]==NULL && values[2]==NULL)	
	{
			printf("The file is empty.\n");
			free(values[0]);
			free(values);
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
	
	//Am Anfang am Mindestens 1000 Knoten Speicherplatz wird zugewiesen
	initialsize=1000>initialsize?1000:initialsize;

	//Die erste Grosse des Graphens ist zweimal den maximalen Knoten oder 1000 Knoten, je nachdem was groesser ist
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
	
	//Wir lesen die Datei und speichern in die GraphBlock Struktur
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
	//Anstelle den Suchalgorithmus
	DFSGraph();
	
	
	freeGraph();
    //befreie die Liste von Knoten
    free(GRAPHSTART);
    //befreie die Liste von Huetten
    free(HUTLIST);
    fclose(readfile);
    
	return 0;

}
