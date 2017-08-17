#include<iostream>
#include<conio.h>
#include <windows.h>
#include<string>
#include<vector>
#include<algorithm>
#include<math.h>
#include<time.h>
#include<unordered_set>
#include <fstream>

#include "sqlite3.h"

#define RATE 0.05

//#include<set>
using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;
void gotoxy(int x, int y)
{
	CursorPosition.X = x; // Locates column
	CursorPosition.Y = y; // Locates Row
	SetConsoleCursorPosition(console, CursorPosition); // Sets position for next thing to be printed 
}


void fullscreen()
{
	keybd_event(VK_MENU, 0x38, 0, 0); //Press down Alt
	keybd_event(VK_RETURN, 0x1c, 0, 0); //Press down Enter

	keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0); //Release Alternate
	keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);//Release Enter
}

//typedef vector< vector<int> > gtype;
int b2[2] = { 0 };
//int grid[2][100];
vector< vector<int> > grid(10, vector<int>(10));

vector< vector<int> > oppgrid(10, vector<int>(10));


unordered_set <int> remaining;

//set<gtype> shots(grid.front(),grid.back());
void dpgrid();
void dpgrid2();
void insert(int, int);
void dpchar();
void ai();
void shoot(int, int);
int place(int, int, int);

int X = 11, Y = 7, count = 0, turn = 0;
int xc = 0, yc = 0;
int pl_rem = 0, rot = 0;
int nexti, nextj, h_mode = 0;
int phit = 0, ohit = 0;

int oppship[5] = { 5,4,3,3,2 };


//vector<int> dir;
//dir.push_back(-2);
//dir.push_back(2);
//dir.push_back(-1);
//dir.push_back(1);

int dir[4] = { -2,2,-1,1 };
//dir[0]=-2;
//dir[1]=2;
//dir[2]=-1;
//dir[3]=1;

int d_index = 0;

/*class node
{
public:
class node * left;
class node * right;
class node * up;
class node * down;
int c;
int f;

class node * pos[2];
node *n[2][25];
*/



//sql variables
sqlite3 *db;


//Neural Network Variables
int net[12]; //Neurons array {0,1} 12 neurons respectively for left hit,right hit, down hit, up hit, left miss, right miss, down miss, inner, outer, vertical orient, horizontal orient
double weights[12];
double output;

bool orient_hor=false, orient_vert=false;


double checkrating(int, int);
void updateweights(double, double);

//file handling variable
fstream file;

class ships
{
public:
	int size, health, snum;
	string type;

	void start(int s, string t, int sno)			//the index number of the ship, the health remaining of each ship and its size
	{
		health = s;
		size = s;
		type = t;
		snum = sno;
		//remaining.insert(snum);
		while(place(size, rand() % 2, snum) == -1);
	}

};

ships s[5];



/*
void init2()
{
	int p, i;
	for (i = 0; i<10; i++)
	{
		for (p = 0; p<10; p++)
		{
			oppgrid[i][p] = 0;
		}
	}
}

void init()
{
	/*
	int p, i;
	for (i = 0; i<10; i++)
	{
		for (p = 0; p<10; p++)
		{
			grid[i][p] = 0;
		}
	}
	gotoxy(10, 40);
	cout << "Ship Orientation                   ";
	gotoxy(1
	//init2();
}
*/


void trav()       //Function to move through the grid
{
	string st;
	char ch;
	if (pl_rem < 5)
	{
		gotoxy(10, 40);
		cout << "Ship Orientation              ";
		gotoxy(29, 40);
		if (rot % 2 == 0)
			cout << "Vertical";
		else
			cout << "Horizontal";
		//gotoxy(X, Y);
	}
	gotoxy(X, Y);
	ch = _getch();
	int m = 0;
	while (m<25)
	{
		switch (ch)
		{
		case 'w':
		case 'W': if (Y - 7 != 0)
		{
			Y = Y - 3;
			yc -= 1;
			gotoxy(X, Y);
		}
				  break;

		case 'a':
		case 'A': if (X - 11 != 0)
		{
			X = X - 6;
			xc -= 1;
			gotoxy(X, Y);
		}
				  break;

		case 's':
		case 'S': if (Y + 3<37)
		{
			Y = Y + 3;
			yc += 1;
			gotoxy(X, Y);
		}
				  break;

		case 'd':
		case 'D':if (X + 6<71)
		{
			X = X + 6;
			xc += 1;
			gotoxy(X, Y);
		}
				 break;

		case 't': dpchar();
			_getch();
			system("cls");
			dpgrid();
			break;

		case 'r':
		case 'R': rot++;
			if (pl_rem < 5)
			{
				gotoxy(10, 40);
				cout << "Ship Orientation                   ";
				gotoxy(29, 40);
				if (rot % 2 == 0)
					cout << "Vertical";
				else
					cout << "Horizontal";
			}
				  break;

		case 'i':
		case 'I': if (pl_rem<5)											// if no of ships placed are less than 5, pressing i will insert a ship in order in r rotation
		{
			if ((rot % 2 == 0 && yc + s[pl_rem].size<=10) || (rot % 2 == 1 && xc + s[pl_rem].size<=10))
			{
				insert(pl_rem, rot);
				pl_rem++;
				system("cls");
				dpgrid();
			}
		}
				  break;

		case ' ': if (pl_rem == 5)				// after all ships placed, you ca hit the desired block in the enemy grid by travelling there and hitting space.
		{									// The ai will play the turn after that *** change for if you have a hit
			turn = 1;
			shoot(xc, yc);
			ai();
			system("cls");
			dpgrid();
		}
				  break;

		case 27: fullscreen();
			break;

		default:  gotoxy(5, 40);
			cout << "Please enter a valid input";
			_getch();
			gotoxy(5, 40);
			cout << "                          ";
			system("cls");
			dpgrid();
		}

		ch = _getch();
	}

}




void dpgrid()               //function to display the grid
{
	int i, j, z = 0;
	//system("cls");

	for (i = 5; i <= 35; i = i + 3)
	{
		gotoxy(8, i);
		cout << "____________________________________________________________";
	}
	for (i = 8; i<70; i = i + 6)
	{
		for (j = 6; j <= 35; j++)
		{
			gotoxy(i, j);
			cout << "|";
		}
	}

	dpchar();
	dpgrid2();
	trav();

}

void dpgrid2()               //function to display the grid
{
	int i, j, z = 0;
	//system("cls");

	for (i = 5; i <= 35; i = i + 3)
	{
		gotoxy(8 + 75, i);
		cout << "____________________________________________________________";
	}
	for (i = 8; i<70; i = i + 6)
	{
		for (j = 6; j <= 35; j++)
		{
			gotoxy(i + 75, j);
			cout << "|";
		}
	}

}



void insert(int p, int r)			// insert ships according to rotation value at the beginning
{
	int i;
	int x = xc, y = yc;
	for (i = 0; i<s[p].size; i++)
	{
		grid[x][y] = s[p].snum;
		if (r % 2 == 0)
		{
			y++;
		}
		else
		{
			x++;
		}
	}
}



void dpchar2()			// function for displaying hits and misses on opponents grid
{
	int i, j;
	int x = 0, y = 0;
	for (i = 11; i<70; i += 6, x += 1)
	{
		for (j = 7; j<35; j += 3, y += 1)
		{
			if (oppgrid[x][y] != 0)
			{
				gotoxy(i, j);
				if (oppgrid[x][y]<=-1 && oppgrid[x][y] != -10)
				{
					cout << "x";
					/*if ()
						cout << "x";
					else
						cout << "X";*/
				}
				else if (oppgrid[x][y] == -10)
				{
					cout << "..";
				}
				/*else
				{
					cout << oppgrid[x][y];
				}*/
			}

		}
		y = 0;
	}

}


void dpchar()				// func to display opponents hits and misses on your grid
{
	int i, j;
	int x = 0, y = 0;
	for (i = 11; i<70; i += 6, x += 1)
	{
		for (j = 7; j<35; j += 3, y += 1)
		{
			if (grid[x][y] != 0)
			{
				gotoxy(i + (turn * 75), j);
				if (grid[x][y]>0)
				{
					cout << s[grid[x][y] - 1].type[0];
					//cout<<x<<","<<y;
				}
				else if (grid[x][y]<=-1 && grid[x][y] != -10)	//already hit on ship
				{
					if (s[abs(grid[x][y]) - 1].health > 0 )
						cout << "x";
					else
						cout << "X";
				}
				else if (grid[x][y] == -10)      // already hit on blank
				{
					cout << "..";
				}
			}

		}
		y = 0;
	}

	dpchar2();
}





int place(int length, int orient, int sno)					// ai function to place opponents ships in its grid randomly
{
	int i, j;										//these are coordinates of the first cell of the ship. the rest of the length of the ship is either below it or to its right (orient dependent)
	int p;
	srand(time(NULL));							 // randomizing seed
	if (orient == 0)								// here if orient is 0, the ship is placed horizontally.
	{
		i = rand() % (11 - length);			//10-length+1 will make it select a number between 0 and 10-length. for eg, if the length is 3, the xcoordinate  
		j = rand() % 10;							// for ship origin will be chosen from 0 to 7

		for (p = i; p<i + length; p++)			 // checking if ship already placed along the length of the ship
		{
			if (oppgrid[p][j] > 0)
			{
				return -1;
			}
		}

		for (p = i; p<i + length; p++)					//placing ship
		{
			oppgrid[p][j] = sno;
		}
	}
	else
	{
		i = rand() % 10;
		j = rand() % (11 - length );

		for (p = j; p<j + length; p++)				// checking if ship already placed along the length of the ship
		{
			if (oppgrid[i][p] > 0)
			{
				return -1;
			}
		}

		for (p = j; p<j + length; p++)			//placing ship
		{
			oppgrid[i][p] = sno;
		}
	}
	return 1;
}



void shoot(int i, int j)			// The player shooting mechanism. The location given in argument is the coordinate that will be hit in the opponents grid
{
	if (oppgrid[i][j] >= 0)				// if not already hit
	{
		if (oppgrid[i][j] == 0)
		{
			oppgrid[i][j] = -10;			//if a blank
		}
		else
		{
			oppgrid[i][j] = -oppgrid[i][j];			//ship hit
			oppship[abs(oppgrid[i][j]) - 1]--;
			phit++;

			if (oppship[abs(oppgrid[i][j]) - 1] == 0)    //checking if a complete ship has been drowned
			{
				gotoxy(30, 45);
				cout <<"You sunk"<< s[abs(oppgrid[i][j]) - 1].type;
				_getch();
			}
			if (phit == 17)
			{
				gotoxy(20, 40);
				cout << "You Win!!!";
				for (int cunt = 0; cunt < 12; cunt++)
				{
					cout << endl << weights[cunt];
				}
				_getch();
				exit(0);
			}
		}
	}
}



int aishoot(int i, int j)				// ai function to shoot our grid. similar to shoot().
{
	int ctp, ctq;
	if (grid[i][j] >= 0)		// returns 0 if the chosen coordinate is already hit, -1 if shot's a miss, 10 if hit but not sunk, "ship val" when sunk
	{
		if (grid[i][j] == 0)
		{
			grid[i][j] = -10;
			return -1;
		}
		else
		{
			grid[i][j] = -grid[i][j];			//returns 1 if hit
			ohit++;
			if (ohit == 17)
			{
				gotoxy(20, 40);
				cout << "You Lose!!!";
				for (int cunt = 0; cunt < 12; cunt++)
				{
					cout << endl << weights[cunt];
				}
				_getch();
				exit(0);
			}
			s[abs(grid[i][j]) - 1].health--;
			if (s[abs(grid[i][j]) -1].health > 0 )
			{
				return 10;			//when ship's been shot but not drowned
			}
			else
			{
				gotoxy(30, 35);
				cout <<"Your "<<s[abs(grid[i][j]) - 1].type << " sunk!";
				_getch();
				for (ctp = i - (abs(grid[i][j])-1); ctp <= i + (abs(grid[i][j]) - 1); ctp++)
				{
					for (ctq = j - (abs(grid[i][j]) - 1); ctp <= i + (abs(grid[i][j]) - 1); ctp++)			//treating fully hit ships as miss
					{
						if (!(ctp < 0 || ctq < 0 || ctp>9 || ctq>9) && abs(grid[ctp][ctq]) == abs(grid[i][j]))
						{
							grid[ctp][ctq] = -10;
						}
					}
				}
				return abs(grid[i][j]);
			}
			//hunt(i,j,-1);
		}
	}
	return 0;
}

void hunt(int i, int j, int shot)		// ai hunt mode. this funtion is called when a hit is encountered by the ai.
{
	int p, q,probp,probq;
	double max=-100, rating;
	if (h_mode == 0)			// checks if hunt mode was already on. it's not when first hit for a ship is found.
	{
		if (shot == 10)
		{
			h_mode = 1;
		}
		else if (shot > 0 && shot < 10)			//when ship drowns, implement hit stack here later
		{
			return;
		}
	}
	/*else if (h_mode == 1)			// if already in hunt mode, and if the shot was a miss, change the direction index for the next shot.
	{
		if (shot == -1 || shot == 0)
		{
			d_index++;
		}
	}*/
	if (h_mode == 1)			// choosing the next shot according to the current searching direction
	{
		//nexti = i + int(dir[d_index] / 2);
		//nextj = j + (-((dir[d_index] * 2) / abs(dir[d_index])) + dir[d_index]);       //setting next i and j values according to direction array index number.
	
		for (p = i - 1; p <= i + 1; p++)	//checking most probable out of 8 places around hit box
		{
			for (q = j - 1; q <= j + 1; q++)
			{
				if (!(p < 0 || p > 9 || q < 0 || q > 9) && !(p==i && q==j)&& (grid[p][q]>=0))		//if out of bounds, don't calc rating
				{
					//cout << "ll";
					//_getch();
					rating = checkrating(p, q);
					cout << rating;
					if (rating > max)
					{
						//cout << "cl";
						max = rating;
						probp = p;
						probq = q;
					}
				}
			}
		}
		_getch();
		nexti = probp;
		nextj = probq;
		output = max;
		max = -100;
	
	}

	/*if ((nexti < 0 || nexti > 9 || nextj < 0 || nextj > 9) && h_mode == 1)	//increase dir if out of bounds
	{
		hunt(i, j, -1);
	}*/
}


int tempi, tempj;

void ai()			// the main ai function
{
	int i, j, shot;
	double target;
	if (h_mode == 0)
	{
		srand(time(NULL));
		do
		{
			i = rand() % 10;
			j = rand() % 10;				//change later for checker strategy
			shot = aishoot(i, j);
		} while (!shot);			//repeat till unique shot is obtained
		tempi = i;				
		tempj = j;
	}
	else
	{
		//i=nexti;
		//j=nextj;
		shot = aishoot(nexti, nextj);
		if (shot > 0 && shot < 10)		
		{
			h_mode = 0;
			d_index = 0;
			target = 1.0;
			orient_hor = false;
			orient_vert = false;
			//check hit stack here
		}
		else if( shot == 10 )		//shot but hunt on
		{
				i = nexti;
				j = nextj;
				target = 1.0;
				if ((grid[i - 1][j]<0 && grid[i - 1][j]>-10) || (grid[i + 1][j]<0 && grid[i + 1][j]>-10))	//setting orientation after second hit
				{
					orient_hor = true;
					orient_vert = false;
				}
				else if ((grid[i][j - 1]<0 && grid[i][j - 1]>-10) || (grid[i][j + 1]<0 && grid[i][j + 1]>-10))
				{
					orient_hor = false;
					orient_vert = true;
				}
		}
		else
		{
			i = tempi;
			j = tempj;
			target = 0;
		}
		updateweights(target, output);
	}

	

	if (!(h_mode == 0 && shot > 0 && shot < 10))
	{
		hunt(i, j, shot);
	}
}

double checkrating(int p, int q)
{
	int l;
	double sum = 0;
	int cp, cq, cindex;			 //checkp, checkq for left right up down of location  being checked

	memset(net, 0, sizeof(net));		//resetting all inputs to 0;

	//update weights array of program from data base here.
	for (cindex=0;cindex<4;cindex++)
	{
		cp = p + int(dir[cindex] / 2);
		cq = q + (-((dir[cindex] * 2) / abs(dir[cindex])) + dir[cindex]);

		if (cp < 0 || cq < 0 || cp>9 || cq>9)
		{
			continue;
		}

		if (grid[cp][cq] < 0 && grid[cp][cq]!=-10)		//already hit ship pos
		{
			net[cindex] = 1;  //because you are a fuctard
			net[cindex + 4] = 0;
		}
		else if (grid[cp][cq] == -10)					//pos miss
		{
			net[cindex] = 0;
			net[cindex + 4] = 1;
		}
		else                                           //neither hit nor miss
		{
			net[cindex] = 0;
			net[cindex + 4] = 0;
		}
	}

	if (p > 2 && q > 2 && p < 7 && q < 7)             //if it lies in the inner square
	{
		net[8] = 1;
		net[9] = 0;
	}
	else                                          //or outer
	{
		net[8] = 0;
		net[9] = 1;
	}

	if (orient_vert)
	{
		net[10] = 1;
		net[11] = 0;
	}
	else if (orient_hor)
	{
		net[10] = 0;
		net[11] = 1;
	}

	for (l = 0; l < 12; l++)
	{
		sum += (net[l] * weights[l]);				//Summation of Xi x Wi
	}
	return sum;
}


void updateweights(double t, double o)
{
	int i;
	fstream file;
	file.open("BattleData.txt", fstream::out | fstream::trunc);
	for (i = 0; i < 12; i++)
	{
		if (file.is_open())
		{
			weights[i] = weights[i] - (RATE * (-(t - o) * net[i]));		//weight updating function using back-propogation
			file << weights[i]<<endl;
		}
		else
		{
			cout << "file closed";
		}
	}
	file.close();
}


int main()
{

	//sqlite3_open("BattleData.db", &db);
	//char *qr = "create table weighttable (weights REAL);";
	//sqlite3_exec(db, qr, NULL, NULL, NULL);
	//char *qr1 = "insert into weighttable"

	int linecount = 0;

	/*for (linecount = 0; linecount < 12; linecount++)
	{
		weights[linecount] = 0;
	}*/
	file.open("BattleData.txt");

	while (!file.eof())				//loading weights
	{
		file >> weights[linecount];
		linecount++;
	}
	
	file.close();
	
	//system("color F0");
	s[0].start(5, "acarrier",1);
	s[1].start(4, "battleship",2);
	s[2].start(3, "submarine",3);
	s[3].start(3, "cruiser",4);
	s[4].start(2, "destroyer",5);
	fullscreen();
	//getch();
	system("cls");
	//t.initialize();
	//t.init();
	dpgrid();
	_getch();
	return 0;
}