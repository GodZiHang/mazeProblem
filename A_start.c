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
    //节点结构体
    int f,g,h;
    int row;    //该节点所在行
    int col;    //该节点所在列
    int direction;//parent节点要移动的方向就能到达本节点
    struct Node * parent;
} Node, *Lnode;

typedef struct Stack
{
    //OPEN CLOSED 表结构体
    Node * npoint;
    struct Stack * next;
} Stack, *Lstack;
int rows;          //地图行数
int cols;          //地图列数
int G_OFFSET = 1;       //每个图块G值的增加值
int endx;     //目标所在行
int endy;     //目标所在列
int tileSize = 1;       //图块大小
int startx;
int starty;
int count = 0;//查找步数
int pathlength = 0;//查找总节点数
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
    //选取OPEN表上f值最小的节点，返回该节点地址
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
    //判断节点是否相等，相等，不相等
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
    //判断节点是否属于OPEN表或CLOSED表，是则返回节点地址，否则返回空地址
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
    //判断节点是否属于OPEN表或CLOSED表，是则返回节点地址，否则返回空地址
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
    //把节点放入OPEN 或CLOSED 表中
    Stack * temp;
    temp =(Stack *) malloc(sizeof(Stack));
    temp->npoint = suc;

    temp->next = Open->next;
    Open->next = temp;
}
void PutintoClosed(Node * suc )
{
    //把节点放入OPEN 或CLOSED 表中
    Stack * temp;
    temp =(Stack *) malloc(sizeof(Stack));
    temp->npoint = suc;
    temp->next = Closed->next;
    Closed->next = temp;
}
//接下来一些函数是用于拓展程序功能，但目前还没用到
//得到该图块的H值
int getH(int row, int col)
{
    return (abs(endx - row) + abs(endy - col));
}

//得到该位置所在地图行
int getRowPosition(int y)
{
    return (y / tileSize);
}

//得到该位置所在地图列
int getColPosition(int x)
{
    return (x / tileSize);
}
//检测该图块是否可通行
BOOL isCanMove(int col, int row)
{
    if(maze[col][row] == '%')
        return FALSE;
    //else maze[col][row] = '>';
    return TRUE;
}

Node* checkOpen(int row, int col)
{//判断当前节点是否位于open表
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
{//判断当前节点是否位于close表
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
{//将该节点加入到open表中
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
 * 根据传入的节点生成子节点
 * @param bestNode
 * @param endx
 * @param endy
 */
void seachSeccessionNode(Node *bestNode)
{//扩展当前节点，然后将其放入close表
    int row, col;
//      Node *bestNodeInOpen = NULL;
    //上部节点
    if(isCanMove(row = bestNode->row - 1, col = bestNode->col))
    {
        creatSeccessionNode(bestNode, row, col);
    }
    //下部节点
    if(isCanMove(row = bestNode->row + 1, col = bestNode->col))
    {
        creatSeccessionNode(bestNode, row, col);
    }
    //左部节点
    if(isCanMove(row = bestNode->row, col = bestNode->col - 1))
    {
        creatSeccessionNode(bestNode, row, col);
    }
    //右部节点
    if(isCanMove(row = bestNode->row, col = bestNode->col + 1))
    {
        creatSeccessionNode(bestNode, row, col);
    }
    PutintoClosed( bestNode );
}
//正序遍历链表
void intoOpenData()
{
    //判断节点是否属于OPEN表或CLOSED表，是则返回节点地址，否则返回空地址
    Lstack temp = Open -> next;
    Node *p_node = (Node *) malloc(sizeof(Node));
    if(temp == NULL)
        return;
    while(temp != NULL)
    {
        Lstack head = temp;
        temp = temp->next;
        p_node = head->npoint;
        //printf("Open库数据！[%d,%d]\n", p_node->col, p_node->row );
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
    //判断节点是否属于OPEN表或CLOSED表，是则返回节点地址，否则返回空地址
    Lstack temp = Closed -> next ;
    Node *p_node = (Node *) malloc(sizeof(Node));
    if(temp == NULL)
        return;
    while(temp != NULL)
    {
        Lstack head = temp;
        temp = temp->next;
        p_node = head->npoint;
        //printf("Closed库数据！[%d,%d]\n", p_node->col, p_node->row );
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
        printf("Closed库数据！节点");
        temp = temp->next;
    }*/
    return;
}

void getPath(int startX, int StartY, int destinationX, int destinationY)
{
    Node *startNode = (Node *) malloc(sizeof(Node));//初始化Node
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
        bestNode = getNodeFromOpen(); //从OPEN表中取出f值最小的节点
        if(bestNode == NULL)//未找到路径
        {
            printf("未找到路径%d\n",count);
            return;
        }
        if(bestNode->row == endy
                && bestNode->col == endx )
        {
            Node *_Node = (Node *) malloc(sizeof(Node));
            _Node = bestNode;
            int nodeIndex =0;
            //printf("程序运行次数=%d\n",count);
            while( _Node->parent != NULL )
            {
                //printf("x:%d  y:%d  direction = %d \n", _Node->col, _Node->row, _Node->direction );
                maze[_Node->row][_Node->col]='>';
                _Node = _Node->parent;
                pathlength += 1;
            }
            //printf("节点数量=%d\n",pathlength);
            _Node = bestNode;
            nodeIndex = pathlength-1;
            while( _Node->parent != NULL && nodeIndex>=0)
            {
                Node *_NodeParent = _Node->parent;
                //printf("x:%d  y:%d  direction = %d \n", _Node->col, _Node->row, _Node->direction );
                maze[_Node->row][_Node->col]='>';
                if( _NodeParent->col - _Node->col == 0 && _NodeParent->row - _Node->row == +1 )
                {
                    //从父节点到本节点的操作是  上
                    direction[nodeIndex] = 1;
                }
                else if( _NodeParent->col - _Node->col == 0 && _NodeParent->row - _Node->row == -1 )
                {
                    //从父节点到本节点的操作是  下
                    direction[nodeIndex] = 2;
                }
                else if( _NodeParent->col - _Node->col == +1 && _NodeParent->row - _Node->row == 0 )
                {
                    //从父节点到本节点的操作是  左
                    direction[nodeIndex] = 3;
                }
                else if( _NodeParent->col - _Node->col == -1 && _NodeParent->row - _Node->row == 0 )
                {
                    //从父节点到本节点的操作是  右
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
    printf("OPEN表节点数：%d\nCLOSE表节点数：%d\n路径长度：%d\n经过的节点数：%d\n查找步数：%d\n",open,close,pathlength,open+close,count);
}
void main()
{
    //主函数
    //初始操作，建立open和closed表
    Open = (Stack *) malloc(sizeof(Stack));
    Open->next = NULL;
    Closed = (Stack *) malloc(sizeof(Stack));
    Closed->next = NULL;
    //初始化maze
    get_maze("mediumMaze.txt");
    print_maze();
    getPath( starty, startx, endx, endy );//进行路径查找
    //根据open和close表中的坐标改变maze
    intoOpenData();
    intoCloseData();

    print_maze();
    stati_data();
    free(Open);
    free(Closed);
}
