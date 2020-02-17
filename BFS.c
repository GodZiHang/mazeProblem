#include <stdio.h>
#include <stdlib.h>


int head=0,tail=1,nextx,nexty,n,m,start_row,start_col,endx,endy;
int pre[100000],a[100000],b[100000];
int x[4]={-1,1,0,0},y[4]={0,0,-1,1};
char** maze;
int** visited;
int** distance;
int rows;
int cols;
int count = 0;
enum terrain {
	empty,
	wall,
	goal,
	crumb,
	path
};
void alloc_maze()
{
	maze = malloc(rows * sizeof(char*));

	int i;
	for (i = 0; i < rows; ++i){
		maze[i] = malloc(cols * sizeof(char*));
	}
}

void alloc_visited()
{
	visited = malloc(rows * sizeof(int*));
	int i;
	for (i = 0; i < rows; ++i){
		visited[i] = malloc(cols * sizeof(int*));
	}
}
void alloc_distance()
{
	distance = malloc(rows * sizeof(int*));
	int i;
	for (i = 0; i < rows; ++i){
		distance[i] = malloc(cols * sizeof(int*));
	}
}
void init_distance()
{
    alloc_distance();

    for(int i = 0;i < rows; ++i){
        for(int j = 0; j < cols; ++j){//启发函数：欧几里得距离
            distance[i][j] = (i-endx)*(i-endx)+(j-endy)*(j-endy);
            printf("%d ",distance[i][j]);
        }
        printf("\n");
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
				start_row = i;
				start_col = j;
			}
		}
	}

	fclose(maze_file);
}

void init_visited()
{
	alloc_visited();

	int i, j;
	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			if (maze[i][j] == '%') {
				visited[i][j] = wall;
			} else if (maze[i][j] == '.') {
				visited[i][j] = goal;
				endx = i; endy = j;
			} else {
				visited[i][j] = empty;
			}
		}
	}
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
	printf("\n");
}
/*
void add_crumbs()
{
	int i, j;
	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			if (maze[i][j] != 's' && visited[i][j] == crumb) {
				maze[i][j] = '$';
			}
			if (maze[i][j] != 's' && visited[i][j] == path){
                maze[i][j] = '>';
			}
		}
	}
}*/
void find(int x)
{
    if(pre[x]!=0)
    {
        visited[a[x]][b[x]]=path;
        find(pre[x]);
    }
}
int check(int x,int y)
{
    if(x<=rows&&y<=cols&&x>0&&y>0) return 1;
    return 0;
}
void bfs()
{
    a[1]=start_row;
    b[1]=start_col;
    visited[start_row][start_col]=crumb;
    while(head!=tail)
    {
        count++;
        head++;
        if(head==1999){
            printf("can't arrive the target!\n");
            break;
        }
        for(int i=0;i<4;i++)
        {
            int flag = 0;
            int tempx=a[head]+x[i];
            int tempy=b[head]+y[i];
            if(flag==0&&maze[tempx][tempy]!='%'){
                nextx=tempx;
                nexty=tempy;
                flag = 1;
            }
            else if(distance[nextx][nexty]==distance[tempx][tempy]&&maze[tempx][tempy]!='%'){

            }
            else if(distance[nextx][nexty]>distance[tempx][tempy]&&maze[tempx][tempy]!='%'){
                nextx=tempx;
                nexty=tempy;
            }
        }
        if(visited[nextx][nexty]==goal)
        {
            tail++;
            pre[tail]=head;
            find(tail);
            break;
        }
        else if(check(nextx,nexty)&&visited[nextx][nexty]==empty)
        {
            visited[nextx][nexty]=crumb;
            tail++;//更新并记录节点，分别存放x,y坐标，它的前一个节点，然后将节点标记为已到达
            a[tail]=nextx;
            b[tail]=nexty;
            pre[tail]=head;
        }
    }
}
void stati_data()
{
    int pathed = 0;
    int pathlenth = 0;
    for(int i = 1;i < rows; i++){
        for(int j = 1;j< cols;j++){
            if(maze[i][j]=='>'||maze[i][j]=='$')
                pathed++;
            if(maze[i][j]=='>')
                pathlenth++;
        }
    }
    if(pathed == 0){
        printf("未找到路径!\n查找步数：%d\n",count);
    }else
        printf("路径长度：%d\n经过的节点数：%d\n查找步数：%d\n",pathlenth+1,pathed,count);
    free(maze);
    free(visited);
    free(distance);
}

int main()
{
    get_maze("mediumMaze.txt");//读取迷宫数据
	init_visited();//舒适化visited表
	init_distance();
	print_maze();
    bfs();//进行贪心搜索
    //add_crumbs();//根据搜索后的visited表更新迷宫
    print_maze();
    stati_data();//打印统计数据
}
