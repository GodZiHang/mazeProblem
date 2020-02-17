#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define FALSE   0
#define TRUE    1
//#define NULL    0
typedef int BOOL;
char** maze;
typedef struct Node
{
    //�ڵ�ṹ��
    int f,g,h;
    int row;    //�ýڵ�������
    int col;    //�ýڵ�������
    int direction;//parent�ڵ�Ҫ�ƶ��ķ�����ܵ��ﱾ�ڵ�
    struct Node * parent;
} Node, *Lnode;

typedef struct Stack
{
    //OPEN CLOSED ��ṹ��
    Node * npoint;
    struct Stack * next;
} Stack, *Lstack;
int rows;          //��ͼ����
int cols;          //��ͼ����
int G_OFFSET = 1;       //ÿ��ͼ��Gֵ������ֵ
int endx;     //Ŀ��������
int endy;     //Ŀ��������
int tileSize = 1;       //ͼ���С
int startx;
int starty;
int count = 0;//���Ҳ���
int pathlength = 0;//�����ܽڵ���
Lstack Open = NULL;
Lstack Closed = NULL;
void alloc_maze()
{
	maze = malloc(rows * sizeof(char*));

	int i;
	for (i = 0; i < rows; ++i){
		maze[i] = malloc(cols * sizeof(char*));
	}
}
void get_maze(char* file_name)
{
	char c;
	char rows_s[3] = { '\0' };
	char cols_s[3] = { '\0' };
	int rows_i = 0;
	int cols_i = 0;
	int swap = 0;

	FILE* maze_file = fopen(file_name, "r");

	if (maze_file) {
		while ((c = getc(maze_file)) != EOF) {
			if (c == '\n') {
				break;
			} else if (c == ','){
				swap = 1;
			} else if (!swap) {
				rows_s[rows_i] = c;
				rows_i++;
			} else {
				cols_s[cols_i] = c;
				cols_i++;
			}
		}
	} else {
		printf("No such file!");
		return;
	}

	rows = atoi(rows_s);
	cols = atoi(cols_s);

	alloc_maze();

	int i,j;

	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {

			if ((c = getc(maze_file)) == '\n') {
				c = getc(maze_file);
			}

			maze[i][j] = c;
			if (c =='P') {
				startx = i;
				starty = j;
			}
			else if(c == '.'){
                endx = i;
                endy = j;
			}
		}
	}

	fclose(maze_file);
}
void print_maze()
{
	int i, j;
	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			printf("%c", maze[i][j]);
		}
		printf("\n");
	}
	//printf("%d,%d\n",startx,starty);
}
Node * getNodeFromOpen()
{
    //ѡȡOPEN����fֵ��С�Ľڵ㣬���ظýڵ��ַ
    Lstack temp = Open->next,min = Open->next,minp = Open;
    Node * minx;
    if( temp == NULL )
        return NULL;

    while(temp->next != NULL)
    {
        if( (temp->next ->npoint->f) < (min->npoint->f) )
        {
            min = temp->next;
            minp = temp;
        }
        temp = temp->next;
    }
    minx = min->npoint;
    temp = minp->next;
    minp->next = minp->next->next;
    free(temp);
    return minx;
}

BOOL Equal(Node * suc,Node * goal)
{
    //�жϽڵ��Ƿ���ȣ���ȣ������
    if ( (suc->row == goal->row) && (suc->col == goal->col)  )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

Node * BelongInOpen( Node * suc )
{
    //�жϽڵ��Ƿ�����OPEN���CLOSED�����򷵻ؽڵ��ַ�����򷵻ؿյ�ַ
    Lstack temp = Open -> next ;
    if(temp == NULL)
        return NULL;
    while( temp != NULL )
    {
        if( Equal(suc,temp->npoint) )
        {
            return temp -> npoint;
        }
        else
        {
            temp = temp->next;
        }
    }
    return NULL;
}

Node * BelongInClosed( Node * suc )
{
    //�жϽڵ��Ƿ�����OPEN���CLOSED�����򷵻ؽڵ��ַ�����򷵻ؿյ�ַ
    Lstack temp = Closed -> next ;
    if(temp == NULL)
        return NULL;
    while(temp != NULL)
    {
        if( Equal(suc,temp->npoint) )
        {
            return temp -> npoint;
        }
        else
        {
            temp = temp->next;
        }
    }
    return NULL;
}

void PutintoOpen(Node * suc )
{
    //�ѽڵ����OPEN ��CLOSED ����
    Stack * temp;
    temp =(Stack *) malloc(sizeof(Stack));
    temp->npoint = suc;

    temp->next = Open->next;
    Open->next = temp;
}
void PutintoClosed(Node * suc )
{
    //�ѽڵ����OPEN ��CLOSED ����
    Stack * temp;
    temp =(Stack *) malloc(sizeof(Stack));
    temp->npoint = suc;
    temp->next = Closed->next;
    Closed->next = temp;
}
//������һЩ������������չ�����ܣ���Ŀǰ��û�õ�
//�õ���ͼ���Hֵ
int getH(int row, int col)
{
    return (abs(endx - row) + abs(endy - col));
}

//�õ���λ�����ڵ�ͼ��
int getRowPosition(int y)
{
    return (y / tileSize);
}

//�õ���λ�����ڵ�ͼ��
int getColPosition(int x)
{
    return (x / tileSize);
}
//����ͼ���Ƿ��ͨ��
BOOL isCanMove(int col, int row)
{
    if(maze[col][row] == '%')
        return FALSE;
    //else maze[col][row] = '>';
    return TRUE;
}

Node* checkOpen(int row, int col)
{//�жϵ�ǰ�ڵ��Ƿ�λ��open��
    Lstack temp = Open -> next;
    if ( temp == NULL )
        return NULL;
    while (temp != NULL)
    {
        if ( (temp->npoint->row==row) && (temp->npoint->col == col)  )
        {
            return temp -> npoint;
        }
        else
        {
            temp = temp->next;
        }
    }
    return NULL;
}

BOOL isInClose(int row, int col)
{//�жϵ�ǰ�ڵ��Ƿ�λ��close��
    Lstack temp = Closed -> next;
    if ( temp == NULL )
        return FALSE;
    while (temp != NULL)
    {
        if ( (temp->npoint->row==row) && (temp->npoint->col == col)  )
        {
            return TRUE;
        }
        else
        {
            temp = temp->next;
        }
    }
    return FALSE;
}
int directionIndex =0;
int direction[1500];
void creatSeccessionNode(Node *bestNode, int row, int col)
{//���ýڵ���뵽open����
    int g = bestNode->g + G_OFFSET;
    if(!isInClose(row, col))
    {
        Node *oldNode = NULL;
        if((oldNode = checkOpen(row, col)) != NULL)
        {
            if(oldNode->g < g)
            {
                oldNode->parent = bestNode;
                oldNode->g = g;
                oldNode->f = g + oldNode->h;
            }
        }
        else
        {
            Node *node = (Node *) malloc(sizeof(Node));
            node->parent = bestNode;
            node->g = g;
            node->h = getH(row, col);
            node->f = node->g + node->h;
            node->row = row;
            node->col = col;
            directionIndex++;
            node->direction = directionIndex;
//              openNode.addElement(node);
            PutintoOpen( node );
        }
    }
}

/**
 * ���ݴ���Ľڵ������ӽڵ�
 * @param bestNode
 * @param endx
 * @param endy
 */
void seachSeccessionNode(Node *bestNode)
{//��չ��ǰ�ڵ㣬Ȼ�������close��
    int row, col;
//      Node *bestNodeInOpen = NULL;
    //�ϲ��ڵ�
    if(isCanMove(row = bestNode->row - 1, col = bestNode->col))
    {
        creatSeccessionNode(bestNode, row, col);
    }
    //�²��ڵ�
    if(isCanMove(row = bestNode->row + 1, col = bestNode->col))
    {
        creatSeccessionNode(bestNode, row, col);
    }
    //�󲿽ڵ�
    if(isCanMove(row = bestNode->row, col = bestNode->col - 1))
    {
        creatSeccessionNode(bestNode, row, col);
    }
    //�Ҳ��ڵ�
    if(isCanMove(row = bestNode->row, col = bestNode->col + 1))
    {
        creatSeccessionNode(bestNode, row, col);
    }
    PutintoClosed( bestNode );
}
//�����������
void intoOpenData()
{
    //�жϽڵ��Ƿ�����OPEN���CLOSED�����򷵻ؽڵ��ַ�����򷵻ؿյ�ַ
    Lstack temp = Open -> next;
    Node *p_node = (Node *) malloc(sizeof(Node));
    if(temp == NULL)
        return;
    while(temp != NULL)
    {
        Lstack head = temp;
        temp = temp->next;
        p_node = head->npoint;
        //printf("Open�����ݣ�[%d,%d]\n", p_node->col, p_node->row );
        //maze[55][20] = 'O';
        if(maze[p_node->row][p_node->col] == ' ')
            maze[p_node->row][p_node->col] = 'O';
        free(p_node);
        free( head );
        Open->next = temp;
    }
    free(p_node);
    printf("\n <-------------> \n");
    return;
}
void intoCloseData()
{
    //�жϽڵ��Ƿ�����OPEN���CLOSED�����򷵻ؽڵ��ַ�����򷵻ؿյ�ַ
    Lstack temp = Closed -> next ;
    Node *p_node = (Node *) malloc(sizeof(Node));
    if(temp == NULL)
        return;
    while(temp != NULL)
    {
        Lstack head = temp;
        temp = temp->next;
        p_node = head->npoint;
        //printf("Closed�����ݣ�[%d,%d]\n", p_node->col, p_node->row );
        if(maze[p_node->row][p_node->col] == ' ')
            maze[p_node->row][p_node->col] = 'C';
        free(p_node);
        free( head );
        Closed -> next = temp;
    }
    free(p_node);
    printf("\n <-------------> \n");
    /*
    temp = Closed -> next;
    while(temp != NULL)
    {
        printf("Closed�����ݣ��ڵ�");
        temp = temp->next;
    }*/
    return;
}

void getPath(int startX, int StartY, int destinationX, int destinationY)
{
    Node *startNode = (Node *) malloc(sizeof(Node));//��ʼ��Node
    Node *bestNode  = NULL;
    endx = getRowPosition(destinationY);
    endy = getColPosition(destinationX);

    startNode->parent= NULL;
    startNode->row = getRowPosition(StartY);
    startNode->col = getColPosition(startX);
    startNode->g = 0;
    startNode->h = getH( startNode->row, startNode->col );
    startNode->f = startNode->g + startNode->h;
    startNode->direction = 0;
    PutintoOpen( startNode );// openNode.add(startNode);

    while(TRUE)
    {
        bestNode = getNodeFromOpen(); //��OPEN����ȡ��fֵ��С�Ľڵ�
        if(bestNode == NULL)//δ�ҵ�·��
        {
            printf("δ�ҵ�·��%d\n",count);
            return;
        }
        if(bestNode->row == endy
                && bestNode->col == endx )
        {
            Node *_Node = (Node *) malloc(sizeof(Node));
            _Node = bestNode;
            int nodeIndex =0;
            //printf("�������д���=%d\n",count);
            while( _Node->parent != NULL )
            {
                //printf("x:%d  y:%d  direction = %d \n", _Node->col, _Node->row, _Node->direction );
                maze[_Node->row][_Node->col]='>';
                _Node = _Node->parent;
                pathlength += 1;
            }
            //printf("�ڵ�����=%d\n",pathlength);
            _Node = bestNode;
            nodeIndex = pathlength-1;
            while( _Node->parent != NULL && nodeIndex>=0)
            {
                Node *_NodeParent = _Node->parent;
                //printf("x:%d  y:%d  direction = %d \n", _Node->col, _Node->row, _Node->direction );
                maze[_Node->row][_Node->col]='>';
                if( _NodeParent->col - _Node->col == 0 && _NodeParent->row - _Node->row == +1 )
                {
                    //�Ӹ��ڵ㵽���ڵ�Ĳ�����  ��
                    direction[nodeIndex] = 1;
                }
                else if( _NodeParent->col - _Node->col == 0 && _NodeParent->row - _Node->row == -1 )
                {
                    //�Ӹ��ڵ㵽���ڵ�Ĳ�����  ��
                    direction[nodeIndex] = 2;
                }
                else if( _NodeParent->col - _Node->col == +1 && _NodeParent->row - _Node->row == 0 )
                {
                    //�Ӹ��ڵ㵽���ڵ�Ĳ�����  ��
                    direction[nodeIndex] = 3;
                }
                else if( _NodeParent->col - _Node->col == -1 && _NodeParent->row - _Node->row == 0 )
                {
                    //�Ӹ��ڵ㵽���ڵ�Ĳ�����  ��
                    direction[nodeIndex] = 4;
                }
                else
                {
                    direction[nodeIndex] = 0;
                }
                nodeIndex -= 1;
                _Node = _Node->parent;
            }
            /*
            for( nodeIndex=0; nodeIndex<pathlength; nodeIndex++ )
            {
                printf("direction[%d]=%d\n",nodeIndex,direction[nodeIndex]);
            }*/
            free(_Node);
            return ;
        }
        count++;
        seachSeccessionNode(bestNode);
    }
}
void stati_data()
{
    int open = 0;
    int close = 0;
    for(int i = 1;i < rows; i++){
        for(int j = 1;j< cols;j++){
            if(maze[i][j]=='O')
                open++;
            if(maze[i][j]=='C')
                close++;
        }
    }
    printf("OPEN��ڵ�����%d\nCLOSE��ڵ�����%d\n·�����ȣ�%d\n�����Ľڵ�����%d\n���Ҳ�����%d\n",open,close,pathlength,open+close,count);
}
void main()
{
    //������
    //��ʼ����������open��closed��
    Open = (Stack *) malloc(sizeof(Stack));
    Open->next = NULL;
    Closed = (Stack *) malloc(sizeof(Stack));
    Closed->next = NULL;
    //��ʼ��maze
    get_maze("mediumMaze.txt");
    print_maze();
    getPath( starty, startx, endx, endy );//����·������
    //����open��close���е�����ı�maze
    intoOpenData();
    intoCloseData();

    print_maze();
    stati_data();
    free(Open);
    free(Closed);
}
