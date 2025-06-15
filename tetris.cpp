#include<conio.h>
#include <random>
#include<thread>
#include<vector>
#include<stdio.h>
#include<chrono>
#include<iostream>


#define FIELD_WIDTH  (10)
#define FIELD_HEIGHT (20)

using namespace std;
using namespace chrono;

int score = 0;
int rmr = 0;
int posx = FIELD_WIDTH / 2 - 2;
int posy = 0;
int turnn = 0;//ブロックが3段階の回転をしているときのどれか、
bool runnning = true;
int mky = 0;// 0はdefalt ,1はblockをchange,2はブロックのｙ座標を+1,3はブロックを回転,4は左に移動,5は右に移動,6は落下

vector<vector<int>> fixation(FIELD_HEIGHT, vector<int>(FIELD_WIDTH, 0));//落ちた(最終的の)ブロックを記憶
vector<vector<int>> moment;//一時的に回転させたブロックなどを記憶

vector<vector<int>>i{ { 1,1,1,1 } };       //I block
vector<vector<int>>o{ {1,1},{1,1} };     //O block
vector<vector<int>>t{ {0,1,0},{1,1,1} }; //T block
vector<vector<int>>s{ {0,1,1},{1,1,0} }; //S block
vector<vector<int>>z{ {1,1,0},{0,1,1} }; //Z block
vector<vector<int>>j{ {1,0,0},{1,1,1} }; //J block
vector<vector<int>>l{ {0,0,1},{1,1,1} }; //L block

void draw(int& posx, int& posy, int& mky);


void turn(const int& direction, int& posy) {//directionが1の時は回転させる
	int rows = moment.size();
	int cols = moment[0].size();
	int n = turnn % 3;
	vector<vector<int>> rotated(cols, vector<int>(rows, 0));

	//1
	if (direction == 1) {

		for (int y = 0; y < rows; y++) {
			for (int x = 0; x < cols; x++) {
				rotated[x][rows - 1 - y] = moment[y][x];
			}
		}
		for (int i = posx - moment.size(); i < -4; i++) {//ブロックが壁に埋まらないため
			posx++;
		}
		for (int i = posx + moment.size(); i > FIELD_WIDTH; i--) {
			posx--;
		}
	}

	moment = rotated;
}

void blockset(int posy, int posx) {

	for (int y = 0; y < moment.size(); y++) {
		for (int x = 0; x < moment[0].size(); x++) {
			// 範囲内チェック
			if (moment[y][x] == 1 &&
				posy + y >= 0 && posy + y < FIELD_HEIGHT &&
				posx + x >= 0 && posx + x < FIELD_WIDTH) {
				fixation[posy + y][posx + x] = 1;
			}
		}
	}

	for (int y = 0; y < FIELD_HEIGHT; y++) {
		bool fullline = true;
		for (int x = 0; x < FIELD_WIDTH; x++) {
			if (fixation[y][x] != 1) {
				fullline = false;
				break;
			}
		}
		if (fullline) {
			score++;
			for (int shiftY = y; shiftY > 0; shiftY--) { //埋まっている行を除去
				for (int x = 0; x < FIELD_WIDTH; x++) {
					fixation[shiftY][x] = fixation[shiftY - 1][x];
				}
			}
			for (int x = 0; x < FIELD_WIDTH; x++) {
				fixation[0][x] = 0;
			}
			y--;

		}
	}
}

void collision() {//ブロックがブロックに衝突したとき

	for (int y = 0; y < moment.size(); y++) {
		for (int x = 0; x < moment[0].size(); x++) {
			if (moment[y][x] == 1
				&& posy + y + 1 < FIELD_HEIGHT && posx + x + 1 < FIELD_WIDTH &&
				fixation[posy + y + 1][posx + x] == 1) {
				blockset(posy, posx);
				mky = 1;
				return;
			}
		}
	}
	if (posy == FIELD_HEIGHT - moment.size()) {//ブロックが端に当たった時
		blockset(posy, posx);
		mky = 1;
	}
}

void draw(int& posx, int& posy, int& mky) { //ブロックのメイン処理
	
	if (mky == 2 && moment.size() + posy < FIELD_HEIGHT) {
		posy++;
	}
	else if (mky == 3) {
		int k = 1;
		turn(k, posy);
	}
	else if (mky == 4 && posx >= 1) {
		posx--;
	}
	else if (mky == 5 && moment[0].size() + posx < FIELD_WIDTH) {
		posx++;
	}
	else if (mky == 6 && moment.size() + posy < FIELD_HEIGHT) {
		posy++;
	}

	printf("score: %d\n", score);
	for (int y = 0; y < FIELD_HEIGHT; y++) {//ブロック絵画
		for (int x = 0; x < FIELD_WIDTH; x++) {
			if (y >= posy && y < posy + moment.size()
				&& x >= posx && x < posx + moment[0].size() &&
				moment[y - posy][x - posx] == 1) {
				printf("■");
			}
			else if (fixation[y][x] == 1) {
				printf("■");
			}
			else {
				printf(".");
			}
		}
		printf("\n");
	}

	collision();

}

void operation() {
	if (_kbhit()) {
		char key = _getch();
		switch (key) {
		case'w'://回転
			mky = 3;
			turnn++;
			break;
		case'a'://右
			mky = 4;
			break;
		case'd'://左
			mky = 5;
			break;
		case's'://下
			mky = 6;
			break;
		default://入力信号なし
			mky = 0;
			break;
		}
	}
}

void loop(vector<vector<int>>& block) {

	auto oc = high_resolution_clock::now();//ブロックの絵画間隔、操作間隔のカウント開始
	moment = block;
	system("cls");
	draw(posx, posy, mky);

	while (mky != 1) {

		auto end = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(end - oc);

		if (duration.count() < 500) {
			mky = 0;//初期化
			operation();
			if (mky != 0) {
				system("cls");
				draw(posx, posy, mky);
				this_thread::sleep_for(milliseconds(50));
			}
		}
		else if (duration.count() >= 500) {
			system("cls");
			mky = 2;//初期化
			draw(posx, posy, mky);
			oc = high_resolution_clock::now();
		}

	}
}

void gameover(bool& quiet) {
	for (int x = 0; x < FIELD_WIDTH; x++) {
		if (fixation[0][x] == 1) {
			quiet = true;
			return;
		}
	}
	quiet = false;
}

void gameLoop() {
	std::mt19937 engine(std::random_device{}()); // シードを設定
	// 乱数の範囲を指定（例: 1から100までの整数）
	std::uniform_int_distribution<int> dist(1, 7);
	bool quiet = false;//ゲームオーバーになるとtrueにする
	vector<vector<int>>block;
	while (!quiet) {

		rmr = dist(engine);
		switch (rmr)
		{
		case 1:block = i; break;
		case 2:block = o; break;
		case 3:block = t; break;
		case 4:block = s; break;
		case 5:block = z; break;
		case 6:block = j; break;
		case 7:block = l; break;
		default:break;
		}
		mky = 0;                    //初期化
		posx = FIELD_WIDTH / 2 - 2;
		posy = 0;
		turnn = 0;
		loop(block);
		gameover(quiet);
	}
}

void Gameover() {
	system("cls");
	printf("GAME OVER Your score is %d", score);
	while (1) {}
}

int main() {
	char i;
	bool o = true;
	printf("これは、Wで回転、Aで左、Sで下、Dで右に動きます いいですか？Please enter y or n\n");
	while (o) {
		cin >> i;
		switch (i) {
		case 'y':
			o = false;
			break;
		case 'n':
			printf("わかりましたか？あなたの選択肢は一つしかないのです\n");
			break;
		default:
			printf("もう一回言いますよ？これは、Wで回転、Aで左、Sで下、Dで右に動きます いいですか？Please enter y or n\n");
			break;
		}
	}
	gameLoop();
	Gameover();
	return 0;
}
