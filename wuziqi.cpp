#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

#define INF 50000
#define SIZE 15
#define MaxStepNum 5
#define ChoiceNum 10
int check=0;///////////////

struct Point
{
	int x,y;
	int score;
	int t; 
};


int S[SIZE*SIZE][2];
int top;

int checkBoard[SIZE][SIZE];
int dir[4][2]={{1,0},{0,1},{1,1},{1,-1}};//控制向四个方向扩展

int optX,optY,X,Y;		//分别为计算机计算出的最佳落子点，和人移动光标的位置坐标
int incode[8];	//六种情况的,1:五连，2：四连并且两边无障碍，3：四连并且一边有障碍，
	//4：三连并且两边无障碍，5：三连并且一边有障碍，6：两连并且两边无障碍
int value[8]={0,INF,8000,480,500,180,200};	//每种情况的价值

void gotoxy(int x, int y)         //定义光标移动函数
{ 
	COORD c; 
	c.X=x*2; 
	c.Y=y; 
	SetConsoleCursorPosition (GetStdHandle(STD_OUTPUT_HANDLE), c); 
}

int cmp1( const void *a , const void *b )  
{  
	struct Point *c = (struct Point *)a;  
	struct Point *d = (struct Point *)b;  
	if(c->score != d->score) return d->score - c->score;  
	else return d->t - c->t;  
}  

int checkBorder(int x,int y)
{
	if(x>=0 && x<SIZE && y>=0 && y<SIZE)return 1;
	return 0;
}

int cmp2( const void *a , const void *b )  
{  
	struct Point *c = (struct Point *)a;  
	struct Point *d = (struct Point *)b;  
	if(c->score != d->score) return d->score - c->score;  
	else return c->t - d->t;  
}

int getPointScore(int checkBoard[SIZE][SIZE],int x,int y)
{
	int score=0;
	int i,tag,cntP;
	int leftLive,rightLive,leftOneEmptyLive,rightOneEmptyLive;//判断是否是活子
	int extendX,extendY;
	int extendNum,potentNum;
	int leftOneEmptyNum,rightOneEmptyNum;	//可以有一个空子的连续个数
	int leftNum,rightNum;		//连续的个数
	int leftPotentNum,rightPotentNum;	//可扩展的个数

	int leftX,rightX,leftY,rightY;

	int incode[10];
	memset(incode,0,sizeof(incode[0])*10);

	for(i=0;i<4;i++)
	{
		leftLive=rightLive=leftOneEmptyLive=rightOneEmptyLive=0; //判断两端的邻接点是否还可扩展
		cntP=1;	//扩展范围内同色点的数量


		for(tag=0,extendY=y+dir[i][0],extendX=x+dir[i][1],leftNum=1,leftPotentNum=1,leftOneEmptyNum=1;
		checkBorder(extendX,extendY) && checkBoard[extendY][extendX]!=-checkBoard[y][x];
		extendY+=dir[i][0],extendX+=dir[i][1],leftPotentNum++)
			if(checkBoard[extendY][extendX]==checkBoard[y][x])	//tag=0表示同色联通，tag=1表示中间有一个空位置
			{
				cntP++;
				if(!tag)leftNum++,leftOneEmptyNum++;
				else if(tag==1)leftOneEmptyNum++;
			}
			else if(checkBoard[extendY][extendX]==0)
			{
				if(tag==0)
				{
					leftX=extendX;
					leftY=extendY;
					leftLive=1;
				}
				else if(tag==1)leftOneEmptyLive=1;
				tag++;
			}


		for(tag=0,extendY=y-dir[i][0],extendX=x-dir[i][1],rightNum=1,rightPotentNum=1,rightOneEmptyNum=1;
		checkBorder(extendX,extendY) && checkBoard[extendY][extendX]!=-checkBoard[y][x];
		extendY-=dir[i][0],extendX-=dir[i][1],rightPotentNum++)
			if(checkBoard[extendY][extendX]==checkBoard[y][x])
			{
				cntP++;
				if(!tag)rightNum++,rightOneEmptyNum++;
				else if(tag==1)rightOneEmptyNum++;
			}
			else if(checkBoard[extendY][extendX]==0)
			{
				if(tag==0)
				{
					rightX=extendX;
					rightY=extendY;
					rightLive=1;
				}
				else if(tag==1)rightOneEmptyLive=1;
				tag++;
			}


		extendNum=leftNum+rightNum-1;
		potentNum=rightPotentNum+leftPotentNum-1;

		if(potentNum<5)continue;

		int lNum=leftOneEmptyNum+rightNum-1;	
		int rNum=rightOneEmptyNum+leftNum-1;

		if(extendNum>=5)incode[1]++;
		else if(extendNum==4)
		{
			if(rightLive && leftLive)incode[2]++;
			else if(rightLive || leftLive)incode[3]++;
		}
		else 
		{
			if(lNum>extendNum || rNum>extendNum)
			{
				if(lNum>rNum) //当leftOneEmptyNum==leftNum时，lNum==extendNum，实际上此时考虑lNum实际上会两次考虑extendNum
				{
					if(lNum==5)incode[3]++;
					else if(lNum==4)
					{
						if(leftOneEmptyLive && rightLive)incode[4]++;
						else incode[5]++;
					}
					else if(lNum==3 && leftOneEmptyLive && rightLive)incode[6]++;
				}
				else 
				{
					if(rNum==5)incode[3]++;
					else if(rNum==4)
					{
						if(rightOneEmptyLive && leftLive)incode[4]++;
						else incode[5]++;
					}
					else if(rNum==3 && rightOneEmptyLive && leftLive)incode[6]++;
				}
			}

			else// if(rNum==extendNum && lNum==extendNum)
			{
				if(extendNum==3)
				{
					if(leftLive && rightLive && potentNum>5)incode[4]++;
					else if((leftLive || rightLive) && potentNum>=5)incode[5]++;
				}
				else if(extendNum==2 && leftLive && rightLive && potentNum>5)incode[6]++;
			}
		}


		if(potentNum>10)potentNum=10;/////////////
		score+=potentNum+cntP*5;
	}

	for(i=1;i<=7;i++)score+=value[i]*incode[i];
	
	if(incode[3]+incode[2]>=2)score+=8000;	//有两个第三种情况是必胜的，切比有两个第四种情况获胜要快
	else if(incode[3]+incode[2]>=1 && incode[4]>=1)score+=8000;
	else if(incode[4]>=2)score+=4000;
	else if(incode[6]>=3)score+=400;

	return score;

}


void printCheckBoard(int checkBoard[SIZE][SIZE])
{
	int i,j;
	for(i=0;i<SIZE;i++)
	{
		for(j=0;j<SIZE;j++)
		{
			if(checkBoard[i][j]==0)printf("十");
			else if(checkBoard[i][j]==1)printf("●");
			else printf("○");
		}
		printf("\n");
	}
}

void move(int checkBoard[SIZE][SIZE])	//光标移动
{
	char c;
	while(c=getch())   
	{

		if(c=='p' && top>1)	/////////////////////////////////
		{
			
			top--;
			checkBoard[S[top][1]][S[top][0]]=0;
			top--;
			checkBoard[S[top][1]][S[top][0]]=0;
			system("cls");
			printCheckBoard(checkBoard);
			X=S[top-1][0];Y=S[top-1][1];
		}					//////////////////////////////////////////

		switch( c )
		{
		case 'w':
		case 72:
			Y--;if(Y<0) Y=SIZE-1;break;
		case 's':
		case 80:
			Y++;if(Y==SIZE) Y=0;break;
		case 'a':
		case 75:
			X--;if(X<0) X=SIZE-1;break;
		case 'd':
		case 77:
			X++;if(X==SIZE) X=0;break;
		}
		if((c==13||c==32) && checkBoard[Y][X]==0)return ;
		gotoxy(X,Y);
	}
}

int judge(int checkBoard[SIZE][SIZE],int x,int y)                     //获胜判断函数
{
	int cnt=0,i,extendX,extendY;
	for(i=0;i<4;i++)
	{
			for(extendX=x,extendY=y,cnt=0;checkBorder(extendX,extendY) && checkBoard[extendY][extendX]==checkBoard[y][x];extendX+=dir[i][1],extendY+=dir[i][0],cnt++);
			for(extendX=x,extendY=y;checkBorder(extendX,extendY) && checkBoard[extendY][extendX]==checkBoard[y][x];extendX-=dir[i][1],extendY-=dir[i][0],cnt++);
			if(cnt>=6)return 1;		//(x,y)点算了两遍
	}

	return 0;
}

int dfs(int checkBoard[SIZE][SIZE] ,int deep,int preScore)
{
	int i,j,s,t;
	int who;
	struct Point P[SIZE*SIZE*2+1];
	if(check<deep)check=deep;///////////

	if(deep%2)who=-1;
	else who=1;	//who==1,表示机器一方

	int cnt=0;
	for(i=0;i<SIZE;i++)
	{
		for(j=0;j<SIZE;j++)if(checkBoard[i][j]==0)
		{
			checkBoard[i][j]=1;
			P[cnt].score=getPointScore(checkBoard,j,i);
			P[cnt].t=1;
			checkBoard[i][j]=-1;
			cnt++;
			P[cnt].score=getPointScore(checkBoard,j,i);
			P[cnt].t=-1;
			cnt++;
			checkBoard[i][j]=0;
			P[cnt-2].x=P[cnt-1].x=j;
			P[cnt-2].y=P[cnt-1].y=i;
			if(who==-1)P[cnt-1].score+=P[cnt-2].score/10;
			else P[cnt-2].score+=P[cnt-1].score/10;
		}
	}

	if(who==1)qsort(P,cnt,sizeof(P[0]),cmp1);//正方，在这里是说电脑	
	else qsort(P,cnt,sizeof(P[0]),cmp2);

	if(deep>=MaxStepNum)		//超过指定搜索步数，终止
	{
		s=0;
		for(i=0;i<4;i++)
		{
			if(who==P[i].t)
				s+=P[i].score*P[i].t;
			else s+=P[i].score*P[i].t/10;
		}
		return s;
	}

	if(P[0].score>=4000)	
	{
		if(P[0].t==who)
		{
			if(deep==0)optX=P[0].x,optY=P[0].y;
			return P[0].score*P[0].t;
		}

		int deg;
		if(P[0].score>=INF)deg=INF;
		else if(P[0].score>=8000)deg=8000;
		else if(P[0].score>=4000)deg=4000;

		for(i=1;P[i].score>=deg;i++)if(P[i].t==who)
		{
			if(deep==0)optX=P[0].x,optY=P[0].y;
			return P[i].score*P[i].t;
		}

	}

	if(who==1)	//正方，在这里是说电脑
	{
		s=-INF;
		for(i=0;i<ChoiceNum;i++)
		{
			checkBoard[P[i].y][P[i].x]=1;
			t=dfs(checkBoard,deep+1,s);
			checkBoard[P[i].y][P[i].x]=0;
			if(t>s)
			{
				s=t;
				if(deep==0)
					optX=P[i].x,optY=P[i].y;
			}
			if(s>=preScore)return s;//beta剪支
		}
	}
	else		//反方
	{
		s=INF;
		for(i=0;i<ChoiceNum;i++)
		{
			checkBoard[P[i].y][P[i].x]=-1;
			t=dfs(checkBoard,deep+1,s);
			checkBoard[P[i].y][P[i].x]=0;
			if(t<s)
			{
				s=t;
				if(deep==0)
					optX=P[i].x,optY=P[i].y;
			}
			if(s<=preScore)return s;//剪支
		}
	}

	return s;
}

void wuziqu(int checkBoard[SIZE][SIZE])
{
	char c;
	int so;

	S[0][0]=SIZE/2;S[0][1]=SIZE/2;top++;

	printf("\n\n    方向键  w,s,a,d控制光标移动\n\n    enter，空格落子 \n\n    任意键开始。。。");
	getch();
	
	do
	{
		top=1;/////////////////////////////////
		memset(checkBoard,0,sizeof(checkBoard[0][0])*SIZE*SIZE);
		system("cls");
		printf("先手？ y\n");
		c=getch();
		if(c!='y' && c!='Y')checkBoard[SIZE/2][SIZE/2]=1;

		system("cls");
		printCheckBoard(checkBoard);
		X=SIZE/2;Y=SIZE/2;
		gotoxy(X,Y);
		while(1)
		{
			move(checkBoard);
			gotoxy(X,Y);
			checkBoard[Y][X]=-1;
			S[top][0]=X;S[top][1]=Y;top++;	////////////////////////////////
			printf("○");
			gotoxy(X,Y);
			if(judge(checkBoard,X,Y))
			{
				gotoxy(7,SIZE+2);				
				printf("win\n");

				getch();
				break ;
			}
			so=dfs(checkBoard,0,INF);		/////////////***************************
			checkBoard[optY][optX]=1;
			S[top][0]=optX;S[top][1]=optY;top++;	////////////////////////////////
			gotoxy(3,SIZE+1);////////////
			printf("%d  :  %d        \n",check,so);///////////
			gotoxy(optX,optY);
			printf("●");
			gotoxy(optX,optY);
			X=optX;Y=optY;
			if(judge(checkBoard,optX,optY))
			{
				gotoxy(7,SIZE+2);
				printf("lost\n");

				getch();
				break ;
			}
		}
		printf("again? y");
		c=getch();
	}while(c=='y' || c=='Y');
}


int main()
{
	wuziqu(checkBoard);
	return 0;
}