#include "gba1.h"
#include "characters.h"			//猫のドット絵描きました
#include "japanese.h"

typedef unsigned char byte;

#define TRUE  1
#define FALSE 0
#define KEY_IN	 (~(*(hword *) KEY_STATUS)&KEY_ALL)
#define CURRENT_TIME *((hword *)0x0400010C)
#define LIMIT_TIME 600																//制限時間設定

/*以下色の定義*/
#define WHITE 				BGR(0x1F,0x1F,0x1F)
#define BLACK 				BGR(0,0,0)
#define RED 					BGR(0x1F,0,0)
#define BLUE					BGR(0,0,0x1F)
#define GREEN 				BGR(0,0x1F,0)
#define FIELD 				BGR(0x15,0x1E,0x10)
#define FIELD2 				BGR(0x01,0x1A,0x01)
#define FIELD3 				BGR(0x07,0x13,0x15)
#define YELLOW 				BGR(0x1F,0x1F,0)
#define CYAN					BGR(0,0x1F,0x1F)
#define MAZENTA 			BGR(0x1F,0,0x1F)
#define DOSHISHA			BGR(0x0c,0,0x0c)
#define CAT_BLACK 		BGR(0x05,0x05,0x05)
#define BROWN					BGR(0x19,0x07,0x02)
#define HOLE					BGR(0x19,0x13,0x13)
#define SHADE 				BGR(0x09,0x09,0x09)
#define SHADE2 				BGR(0x13,0x13,0x13)
#define SHADE3 				BGR(0x1A,0x1A,0x1A)
#define COLOR_SHADE		BGR(0x15,0x15,0x15)
#define COLOR_SHADE2 	BGR(0x09,0x09,0x09)

/*吹き出し文字表示開始位置座標*/
#define SX	2
#define	SY	14

/*時間表示の位置座標*/
#define TX	23
#define	TY	18


/*treasure:お宝の位置*/
hword treasure[12][8] = {
	{0,5,5,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,5,0,0,0,0},
	{0,0,0,DOSHISHA,0,0,0,0},
	{0,5,0,0,0,0,0,0},
	{0,0,0,5,0,0,0,0},
	{5,0,5,0,0,0,0,0},
	{5,0,0,0,0,0,0,0},
	{0,0,BROWN,0,0,0,0,0},
	{0,0,0,MAZENTA,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,5,0,0,0,0}
};

hword treasure2[12][8] = {
	{0,0,GREEN,0,0,0,0},
	{0,0,0,5,0,0,0,0},
	{0,0,5,0,0,0,0,0},
	{5,5,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,WHITE,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,5,0,0,0,0},
	{RED,0,0,0,0,0,0,0},
	{0,0,CAT_BLACK,0,0,0,0,0},
	{5,0,0,5,0,0,0,0},
	{0,0,5,0,0,0,0,0}
};

hword treasure3[12][8] = {
	{0,0,0,0,0,0,0,0},
	{0,5,5,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,CYAN,0,YELLOW,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,5,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,5,0,0,0,0,0,0},
	{0,0,BLUE,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,5,0,0,0,0,0}
};

hword treasure4[12][8] = {
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,BLUE,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0}
};			//ルール説明のデモ用



hword copy[4][12][8];						//お宝の穴の初期状態保持用の配列
hword map[240][160];						//マップの状態保持


void start();																					//スタート画面
void display_time(hword);															//時間表示
void turn_timer(byte sw);															//タイマON,OFF
void paintAll(hword);																	//画面塗りつぶし
void paint_edge();																		//枠線描画
void init();																					//タイマーとフラッグ初期化
void late(hword);																			//遅延
void character_paint(hword cat[20][20],hword);				//動いてる時の猫描画
void locate_c(hword,hword);														//キャラクターの位置
void delete(hword,hword);															//前のキャラの位置の絵を消す
void move();																					//キャラの移動
void pressKey(hword);																	//離すまで留まる
void choose_color();																	//猫の色選択
void setFlag(hword);																	//クリアしたかしていないかの判断の旗のTRUE,FALSE設定
void discover(hword t[12][8]);												//お宝の当たり判定
void treasurePaint(hword t[12][8]);
void mapping();																				//マップ描画
void mainAction();
void dig(hword t[12][8]);															//穴を掘るかどうか判定
hword check(hword, hword, hword t[12][8]);						//お宝かどうか判定
void prepare(hword t[12][8]);													//マップ移動の際などの準備
void end();																						//エンディング
void setMap(hword);																		//マップを変える
void pitfallPaint(hword t[12][8] , hword);						//落とし穴に落ちた猫描画
void key_wait(hword);																	//引数に指定したキー入力待ち
void locate_ja(hword,hword);													//日本語の位置
void jaPrint(byte);																		//日本語描画
void panic_catPaint();																//プロローグの猫
void prologue();																			//プロローグ
void explain();																				//実例を用いてルール、操作方法を説明
void paintStartDisplay();															//スタート画面描画
void allAction();																			//全ての関数を一つにまとめてすっきりさせた
void cloudPaint();
void canPaint();

point c;			//キャラクター用座標変数
point h;			//穴の位置用座標変数
point ja;			//日本語の位置用座標変数
point cloud1;	//雲の位置用変数
point cloud2;	//雲の位置
point cloud3;	//雲の位置
point ca;

hword mapFlag;											//マップ切り替えに使用
hword flag;													//クリアかどうか判定
hword cat_color;										//選んだ猫の色を保持
hword bestTime=LIMIT_TIME;					//最短時間を記録
hword stopFlag;											//3回間違えたカウント
hword gameFlag;											//ゲームを開始したかどうか
hword treasureFlag;									//treasure配列3つを初期状態のときのみコピーする
hword timeFlag;											//猫の色選択中に時間を表示してしまわないようにするため
/*---------------------main------------------------*/
int main(void) {

	allAction();											//全てのアクションが詰まっている

	return 0;
}
/*************************************************************/


/***********************大きな役割を持つ関数******************************/
/*全てのアクション*/
void allAction(){

	*((hword *)IOBASE) = 0xF03;

	while(1){
		start();													//開始画面
		explain();												//説明

		while(gameFlag==1){

			init();												//初期化
			choose_color();								//色を選ぶ
			prologue();										//プロローグ

			prepare(treasure2);						//初期の真ん中のマップの準備
			mainAction();									//メインとなる動き

			end();
		}
	}
}

/************************************************************/

/*スタート画面*/
void start(){

	/*宝の位置情報の初期状態を保持*/
	hword tx,ty;
	if(treasureFlag==0){
		for(tx=0; tx<12; tx++){
			for(ty=0; ty<8; ty++){
				copy[0][tx][ty]=treasure[tx][ty];
				copy[1][tx][ty]=treasure2[tx][ty];
				copy[2][tx][ty]=treasure3[tx][ty];
				copy[3][tx][ty]=treasure4[tx][ty];
			}
		}
	}

	treasureFlag = 1;					//ゲームを始める前の配列だけコピーするため

	paintStartDisplay();			//スタート画面表示

	locate(10,1);
	prints("START");
	locate_ja(15,1);
	jaPrint(36);
	jaPrint(8);
	jaPrint(37);
	jaPrint(38);
	jaPrint(12);
	locate_ja(13,18);
	jaPrint(23);
	jaPrint(24);
	jaPrint(16);
	jaPrint(25);

	mapping();

	cloud1.x = 120;
	cloud2.x = 40;
	cloud3.x = 60;

	cloud1.y = 70;
	cloud2.y = 40;
	cloud3.y = 100;

	while(KEY_IN!=KEY_START){

		cloudPaint();

		late(4);

	}

	gameFlag=1;								//ゲーム開始の合図

	cat_color = BLUE;					//ルール説明時の猫の色初期設定

	setMap(0);								//マップの初期値

}

/*ルールと操作方法を説明*/
void explain(){

	hword i;

	paintAll(WHITE);
	paintAll(FIELD2);

	paint_edge();


	treasurePaint(treasure4);

	mapping();

	locate_c(20,40);
	character_paint(characters,BLUE);

	late(5);
	locate_ja(SX,SY);
	jaPrint(26);
	late(5);
	jaPrint(2);
	late(5);
	jaPrint(26);
	late(5);
	jaPrint(47);
	late(5);
	jaPrint(14);
	late(5);
	jaPrint(28);
	late(5);
	jaPrint(29);
	late(5);
	late(5);
	late(5);
	locate(SX,SY+1);

	prints("(press START)");
	key_wait(KEY_START);


	locate(SX,SY);
	prints("                        ");
	locate(SX,SY+1);
	prints("                        ");

	locate_ja(SX,SY);
	jaPrint(45);
	late(5);
	jaPrint(46);
	late(5);
	jaPrint(31);
	late(5);
	jaPrint(2);
	late(5);
	jaPrint(12);
	late(5);
	jaPrint(23);
	late(5);
	jaPrint(32);
	late(5);
	jaPrint(17);
	late(5);

	locate_ja(SX,SY+1);

	jaPrint(25);
	late(5);
	jaPrint(20);
	late(5);
	jaPrint(5);
	late(5);
	jaPrint(8);
	late(5);
	jaPrint(29);
	late(5);
	jaPrint(17);
	late(5);
	jaPrint(8);
	late(5);
	jaPrint(33);
	late(5);
	jaPrint(32);
	late(5);
	jaPrint(18);
	late(5);
	jaPrint(35);
	late(5);

	for(i=0; i<80; i++){
		late(3);
		c.x++;
		character_paint(characters_r,BLUE);
	}

	character_paint(characters,BLUE);

	key_wait(KEY_START);

	locate(SX,SY);
	prints("                        ");
	locate(SX,SY+1);
	prints("                        ");

	locate(SX,SY);
	prints("A");

	locate_ja(SX+1,SY);
	jaPrint(36);
	late(5);
	jaPrint(8);
	late(5);
	jaPrint(37);
	late(5);
	jaPrint(38);
	late(5);
	jaPrint(12);
	late(5);
	jaPrint(23);
	late(5);
	jaPrint(32);
	late(5);
	jaPrint(17);
	late(5);

	locate_ja(SX,SY+1);

	jaPrint(0);
	late(5);
	jaPrint(39);
	late(5);
	jaPrint(12);
	late(5);
	jaPrint(40);
	late(5);
	jaPrint(18);
	late(5);
	jaPrint(35);
	late(5);

	dig(treasure4);
	treasurePaint(treasure4);
	character_paint(characters,BLUE);

	for(i = 0; i < 20; i++){
		late(4);
		c.x--;
		character_paint(characters_l,BLUE);
	}

	character_paint(characters,BLUE);
	for(i = 0; i < 5; i++){late(5);}

	for(i = 0; i < 20; i++){
		late(4);
		c.x++;
		character_paint(characters_r,BLUE);
	}

	character_paint(characters,BLUE);
	late(5);

	key_wait(KEY_START);

	locate(SX,SY);
	prints("                        ");
	locate(SX,SY+1);
	prints("                        ");


	locate_ja(SX+1,SY);
	locate(SX,SY);
	prints("3");
	late(5);
	jaPrint(5);
	late(5);
	jaPrint(29);
	late(5);
	jaPrint(52);
	late(5);
	jaPrint(51);
	late(5);
	jaPrint(5);
	late(5);
	jaPrint(8);
	late(5);
	jaPrint(53);
	late(5);
	jaPrint(18);
	late(5);
	jaPrint(17);
	late(5);

	locate_ja(SX,SY+1);

	jaPrint(23);
	late(5);
	jaPrint(51);
	late(5);
	jaPrint(51);
	late(5);
	jaPrint(55);
	late(5);
	jaPrint(33);
	late(5);
	jaPrint(35);
	late(5);

	pitfallPaint(treasure4,FIELD2);

	key_wait(KEY_START);

	locate(SX,SY);
	prints("                        ");
	locate(SX,SY+1);
	prints("                        ");

	locate(SX,SY);
	prints("B");
	late(5);

	locate_ja(SX+1,SY);

	jaPrint(36);
	late(5);
	jaPrint(8);
	late(5);
	jaPrint(37);
	late(5);
	jaPrint(38);
	late(5);
	jaPrint(12);
	late(5);
	jaPrint(23);
	late(5);
	jaPrint(24);
	late(5);
	jaPrint(39);
	late(5);
	jaPrint(5);
	late(5);
	jaPrint(8);
	late(5);
	jaPrint(6);
	late(5);

	locate_ja(SX,SY+1);

	jaPrint(45);
	late(5);
	jaPrint(46);
	late(5);
	jaPrint(31);
	late(5);
	jaPrint(2);
	late(5);
	jaPrint(12);
	late(5);
	jaPrint(23);
	late(5);
	jaPrint(32);
	late(5);
	jaPrint(17);
	late(5);

	locate(SX,SY+2);

	jaPrint(41);
	late(5);
	jaPrint(42);
	late(5);
	jaPrint(43);
	late(5);
	jaPrint(44);
	late(5);
	jaPrint(12);
	late(5);
	jaPrint(29);
	late(5);
	jaPrint(17);
	late(5);
	jaPrint(8);
	late(5);
	jaPrint(33);
	late(5);
	jaPrint(32);
	late(5);
	jaPrint(18);
	late(5);
	jaPrint(35);
	late(5);

	key_wait(KEY_START);

	locate(SX,SY);
	prints("                        ");
	locate(SX,SY+1);
	prints("                        ");
	locate(SX,SY+2);
	prints("                        ");

	locate_ja(SX,SY);

	jaPrint(41);
	late(5);
	jaPrint(42);
	late(5);
	jaPrint(43);
	late(5);
	jaPrint(44);
	late(5);
	jaPrint(54);
	late(5);
	jaPrint(47);
	late(5);
	jaPrint(8);
	late(5);
	jaPrint(48);
	late(5);
	jaPrint(49);
	late(5);
	jaPrint(8);
	late(5);
	jaPrint(16);
	late(5);
	jaPrint(8);
	late(5);

	locate_ja(SX+1,SY+1);
	locate(SX,SY+1);
	prints("3");
	late(5);
	jaPrint(14);
	late(5);
	jaPrint(0);
	late(5);
	jaPrint(18);
	late(5);
	jaPrint(35);
	late(5);

	key_wait(KEY_START);

	locate(SX,SY);
	prints("                        ");
	locate(SX,SY+1);
	prints("                        ");

	locate_ja(SX,SY);

	jaPrint(1);
	late(5);
	jaPrint(2);
	late(5);
	jaPrint(5);
	late(5);
	jaPrint(8);
	late(5);
	jaPrint(39);
	late(5);
	jaPrint(34);
	late(5);
	jaPrint(24);
	late(5);
	jaPrint(4);
	late(5);
	jaPrint(23);
	late(5);
	jaPrint(4);
	late(5);
	jaPrint(5);
	late(5);
	jaPrint(6);
	late(5);
	jaPrint(12);
	late(5);

	locate_ja(SX,SY+1);

	jaPrint(13);
	late(5);
	jaPrint(14);
	late(5);
	jaPrint(15);
	late(5);
	jaPrint(16);
	late(5);
	jaPrint(0);
	late(5);
	jaPrint(15);
	late(5);
	jaPrint(8);
	late(5);
	jaPrint(35);
	late(5);
	jaPrint(33);
	late(5);
	jaPrint(22);
	late(5);

	key_wait(KEY_START);

}

/*初期化*/
void init(){
	hword tx,ty;

	turn_timer(FALSE);

	/* タイマカウンタ設定レジスタ */
	*((unsigned short *)0x04000100) = 0xFF00;	    // タイマ0 カウンタ設定(元クロック1/2^24秒 約60n秒)
	*((unsigned short *)0x04000104) = 0xFF00;	    // タイマ1 カウンタ設定(元クロック1/2^16秒 約15.4u秒)
	*((unsigned short *)0x04000108) = 0xFFFF - 25;	// タイマ2 カウンタ設定(元クロック1/2^8秒  約3.93m秒)25回カウントで0.1秒
	*((unsigned short *)0x0400010C) = 0x0000;	    // タイマ3 カウンタ設定(元クロック1秒)

	/* タイマ制御レジスタ */
	*((unsigned short *)0x04000102) = 0x0000;	// タイマ0 制御設定（タイマOFF，割込みOFF，カスケードOFF，プリスケーラなし）
	*((unsigned short *)0x04000106) = 0x0084;	// タイマ1 制御設定（タイマON，割込みOFF，カスケードON，プリスケーラなし）
	*((unsigned short *)0x0400010A) = 0x0084;	// タイマ2 制御設定（タイマON，割込みOFF，カスケードON，プリスケーラなし）
	*((unsigned short *)0x0400010E) = 0x0084;	// タイマ3 制御設定（タイマON，割込みOFF，カスケードON，プリスケーラなし）

	setFlag(FALSE);					//お宝を見つけたか否か


	for(tx=0; tx<12; tx++){
		for(ty=0; ty<8; ty++){
			treasure[tx][ty] = copy[0][tx][ty];
			treasure2[tx][ty] = copy[1][tx][ty];
			treasure3[tx][ty] = copy[2][tx][ty];
			treasure4[tx][ty] = copy[3][tx][ty];
		}
	}

	stopFlag = 0;

	timeFlag = 0;

	setMap(2);
}

/*初めに色を選ぶ*/
void choose_color(){
	paintAll(WHITE);
	paintAll(BLACK);
	mapping();
	locate(8,2);
	prints("Choose color!");
	locate(3,13);
	prints("Right or Left and pressA!");
	locate_c(20,70);
	character_paint(characters,RED);
	locate_c(40,70);
	character_paint(characters,BLUE);
	locate_c(60,70);
	character_paint(characters,GREEN);
	locate_c(80,70);
	character_paint(characters,YELLOW);
	locate_c(100,70);
	character_paint(characters,WHITE);
	locate_c(120,70);
	character_paint(characters,CAT_BLACK);
	locate_c(140,70);
	character_paint(characters,CYAN);
	locate_c(160,70);
	character_paint(characters,MAZENTA);
	locate_c(180,70);
	character_paint(characters,DOSHISHA);
	locate_c(200,70);
	character_paint(characters,BROWN);
	locate_c(20,50);
	cat_color = RED;
	character_paint(pad,cat_color);
	while(KEY_IN!=KEY_A){
		switch(KEY_IN){
			case KEY_RIGHT:
			if(c.x<200){
				character_paint(pad,BLACK);
				c.x+=20;
				if(c.x==20){cat_color=RED;
				}else if(c.x==40){cat_color=BLUE;
				}else if(c.x==60){cat_color=GREEN;
				}else if(c.x==80){cat_color=YELLOW;
				}else if(c.x==100){cat_color=WHITE;
				}else if(c.x==120){cat_color=CAT_BLACK;
				}else if(c.x==140){cat_color=CYAN;
				}else if(c.x==160){cat_color=MAZENTA;
				}else if(c.x==180){cat_color=DOSHISHA;
				}else if(c.x==200){cat_color=BROWN;
				}
				character_paint(pad,cat_color);
			}
			pressKey(KEY_RIGHT);

			break;

			case KEY_LEFT:
			if(c.x>20){
				character_paint(pad,BLACK);
				c.x-=20;
				if(c.x==20){cat_color=RED;
				}else if(c.x==40){cat_color=BLUE;
				}else if(c.x==60){cat_color=GREEN;
				}else if(c.x==80){cat_color=YELLOW;
				}else if(c.x==100){cat_color=WHITE;
				}else if(c.x==120){cat_color=CAT_BLACK;
				}else if(c.x==140){cat_color=CYAN;
				}else if(c.x==160){cat_color=MAZENTA;
				}else if(c.x==180){cat_color=DOSHISHA;
				}else if(c.x==200){cat_color=BROWN;
				}
				character_paint(pad,cat_color);
			}
			pressKey(KEY_LEFT);

			break;

			default : break;
		}
	}
}

/*プロローグ*/
void prologue(){
	hword i;

	paintAll(cat_color);
	paintAll(BLACK);

	locate_ja(12,10);
	jaPrint(17);
	jaPrint(0);
	jaPrint(18);
	jaPrint(19);
	jaPrint(3);
	jaPrint(20);
	jaPrint(17);

	for(i=0; i < 15; i++){late(5);}

	locate(10,10);
	prints("                   ");

	paint_edge();
	locate_ja(SX,SY);
	jaPrint(1);
	late(5);
	jaPrint(2);
	late(5);
	jaPrint(3);
	late(5);
	jaPrint(4);
	late(5);
	jaPrint(5);
	late(5);
	jaPrint(6);
	late(5);
	jaPrint(7);
	late(5);
	jaPrint(3);
	late(5);
	jaPrint(5);
	late(5);
	jaPrint(8);
	late(5);
	jaPrint(21);
	late(5);
	jaPrint(21);
	late(5);
	jaPrint(21);
	late(5);

	for(i=0; i < 15; i++){late(5);}

	panic_catPaint();
	locate(SX,SY);
	prints("                     ");
	locate_ja(SX,SY);
	jaPrint(9);
	late(5);
	jaPrint(10);
	late(5);
	jaPrint(11);
	late(5);
	jaPrint(22);
	late(5);
	jaPrint(22);
	late(5);
	jaPrint(22);
	late(5);
	jaPrint(22);
	late(5);

	for(i=0; i < 15; i++){late(5);}

	locate(SX,SY);
	prints("                      ");
	locate_ja(SX,SY);
	jaPrint(21);
	late(5);
	late(5);
	late(5);
	jaPrint(21);
	late(5);
	late(5);
	late(5);
	jaPrint(21);
	late(5);
	late(5);
	late(5);
	late(5);
	late(5);
	jaPrint(22);
	late(5);
	for(i=0; i < 15; i++){late(5);}

	locate(SX,SY);
	prints("                      ");
	locate_ja(SX,SY);

	jaPrint(64);			//そ
	late(5);
	jaPrint(20);			//こ
	late(5);
	jaPrint(3);				//の
	late(5);
	jaPrint(58);			//ひ
	late(5);
	jaPrint(17);			//と
	late(5);
	locate_ja(SX,SY+1);
	jaPrint(4);				//た
	late(5);
	jaPrint(32);			//す
	late(5);
	jaPrint(15);			//け
	late(5);
	jaPrint(16);			//て
	late(5);
	jaPrint(40);			//ほ
	late(5);
	jaPrint(24);			//し
	late(5);
	jaPrint(29);			//い
	late(5);
	jaPrint(9);				//ニ
	late(5);
	jaPrint(10);				//ｬ
	late(5);
	jaPrint(21);				//.
	late(5);
	late(5);
	late(5);
	jaPrint(21);				//.
	late(5);
	late(5);
	late(5);
	jaPrint(21);				//.
	late(5);
	late(5);
	late(5);

	for(i=0; i < 15; i++){late(5);}

	locate(SX,SY);
	prints("                      ");
	locate(SX,SY+1);
	prints("                      ");

	locate_ja(SX,SY);
	jaPrint(16);
	late(5);
	jaPrint(8);
	late(5);
	jaPrint(65);
	late(5);
	jaPrint(4);
	late(5);
	jaPrint(6);

	locate(SX+5,SY);
	prints("1");
	locate_ja(SX+6,SY);
	jaPrint(63);
	late(5);
	jaPrint(16);
	jaPrint(8);
	late(5);

	locate_ja(SX,SY+1);
	jaPrint(4);
	late(5);
	jaPrint(3);
	late(5);
	jaPrint(48);
	late(5);
	jaPrint(4);
	late(5);
	jaPrint(8);
	late(5);
	jaPrint(9);
	late(5);
	jaPrint(10);
	late(5);
	jaPrint(22);
	late(5);
	jaPrint(22);
	late(5);
	jaPrint(22);
	late(5);

	for(i=0; i < 15; i++){late(5);}

	locate(19,0);
	prints("READY?");
	locate(16,1);
	prints("(press START)");
	key_wait(KEY_START);

	timeFlag = 1;
}

/*準備*/
void prepare(hword t[12][8]){

	//選択した猫の色に塗りつぶす
	paintAll(cat_color);
	paintAll(BLACK);
	if(mapFlag==1){paintAll(FIELD);}
	else if(mapFlag==2){paintAll(FIELD2);}
	else{paintAll(FIELD3);}
	treasurePaint(t);
	mapping();

	locate_c(0,0);														//初期位置に移動
	character_paint(characters,cat_color);
	paint_edge();
	locate(SX,SY);
	prints("LET'S FIND TREASURE!");

	// turn_timer(TRUE);													//タイマオン
}

/*メインの動き*/
void mainAction() {
	hword tx,ty;
	hword i;
	hword c;
	while(CURRENT_TIME<=LIMIT_TIME){						//時間制限
		turn_timer(TRUE);													//タイマオン
		locate(TX,TY);														//時間を表示する位置座標設定
		display_time(LIMIT_TIME-CURRENT_TIME);		//時間が減っていくように見せかける
		if(stopFlag==3){													//3回間違えたら3秒間静止
			locate(SX,SY);
			prints("                      ");
			locate(SX,SY);
			prints("Oops!");
			if(mapFlag==1){pitfallPaint(treasure,FIELD);}
			else if(mapFlag==2){pitfallPaint(treasure2,FIELD2);}
			else if(mapFlag==3){pitfallPaint(treasure3,FIELD3);}
			c=CURRENT_TIME;
			while(CURRENT_TIME-c<=30){							//3秒間静止
				if(CURRENT_TIME==LIMIT_TIME)break;
				locate(TX,TY);
				display_time(LIMIT_TIME-CURRENT_TIME);
			}
			locate(SX,SY);
			prints("                      ");
			locate(SX,SY);
			prints("LET'S FIND TREASURE!");
			stopFlag=0;
		}else if(mapFlag==1){											//マップ1

			move();

			if(KEY_IN==KEY_A){
				discover(treasure);											//おたからを発見したかどうか
				dig(treasure);													//穴を掘る
				treasurePaint(treasure);								//穴を描画する
				character_paint(characters,cat_color);	//消えた猫を再度表示

				if(flag==TRUE){													//おたからを発見すると
					turn_timer(FALSE);										//タイマを止める
					locate(SX,SY);
					prints("                      ");
					locate(SX,SY);
					prints(".");
					for(i=0;i<5;i++){late(5);}
					prints(".");
					for(i=0;i<5;i++){late(5);}
					prints(".");
					for(i=0;i<5;i++){late(5);}
					prints("!");
					for(i=0;i<5;i++){late(5);}
					break;
				}else{																	//お宝ではなかった場合
					locate(SX,SY);
					prints("                      ");
					locate(SX,SY);
					prints("NOT FOUND...");
					pressKey(KEY_A);
				}
			}else if(KEY_IN==(KEY_B)+(KEY_RIGHT)){		//マップ移動
				mapFlag+=1;
				prepare(treasure2);											//次のマップの準備
				pressKey((KEY_B)+(KEY_RIGHT));					//繰り返し防止
			}
		}else if(mapFlag==2){												//マップ2

			move();

			if(KEY_IN==KEY_A){
				discover(treasure2);
				dig(treasure2);
				treasurePaint(treasure2);
				character_paint(characters,cat_color);

				if(flag==TRUE){
					turn_timer(FALSE);
					locate(SX,SY);
					prints("                      ");
					locate(SX,SY);
					prints(".");
					for(i=0;i<5;i++){late(5);}
					prints(".");
					for(i=0;i<5;i++){late(5);}
					prints(".");
					for(i=0;i<5;i++){late(5);}
					prints("!");
					for(i=0;i<5;i++){late(5);}
					break;
				}else{
					locate(SX,SY);
					prints("                      ");
					locate(SX,SY);
					prints("NOT FOUND...");
					pressKey(KEY_A);
				}
			}else if(KEY_IN==(KEY_B)+(KEY_RIGHT)){
				mapFlag+=1;
				prepare(treasure3);
				pressKey((KEY_B)+(KEY_RIGHT));
			}else if(KEY_IN==(KEY_B)+(KEY_LEFT)){
				mapFlag-=1;
				prepare(treasure);
				pressKey((KEY_B)+(KEY_LEFT));
			}
		}else if(mapFlag==3){

			move();

			if(KEY_IN==KEY_A){
				discover(treasure3);
				dig(treasure3);
				treasurePaint(treasure3);
				character_paint(characters,cat_color);

				if(flag==TRUE){
					turn_timer(FALSE);
					locate(SX,SY);
					prints("                      ");
					locate(SX,SY);
					prints(".");
					for(i=0;i<5;i++){late(5);}
					prints(".");
					for(i=0;i<5;i++){late(5);}
					prints(".");
					for(i=0;i<5;i++){late(5);}
					prints("!");
					for(i=0;i<5;i++){late(5);}
					break;

				}else{
					locate(SX,SY);
					prints("                      ");
					locate(SX,SY);
					prints("NOT FOUND...");
					pressKey(KEY_A);
				}

			}else if(KEY_IN==(KEY_B)+(KEY_LEFT)){
				mapFlag-=1;
				prepare(treasure2);
				pressKey((KEY_B)+(KEY_LEFT));
			}
		}
	}
}

/*エンディング*/
void end(){

	hword i,j;

	// turn_timer(FALSE);
	if(flag == TRUE){
		paintAll(cat_color);
		paintAll(WHITE);
		paintAll(BLACK);

		locate(5,0);
		prints("RESULT TIME:");
		locate(17,0);
		display_time(CURRENT_TIME);

		if(CURRENT_TIME<bestTime)bestTime=CURRENT_TIME;				//記録更新したかどうか

		locate(7,2);
		prints("BEST TIME:");
		locate(17,2);
		display_time(bestTime);
		locate_c(110,60);

		mapping();
		character_paint(characters,cat_color);
		ca.x=130;
		ca.y=60;
		canPaint();
		locate(7,10);
		prints("Congraturation!!!");

		for(i=0; i < 20; i++){late(5);}
		locate(21,18);
		prints("Retry?");
		locate(17,19);
		prints("(Press START)");
		locate(4,18);
		prints("Title?");
		locate(0,19);
		prints("(Press SELECT)");
		while(1){
			if(KEY_IN==KEY_START){break;}
			else if(KEY_IN==KEY_SELECT){
				paintAll(WHITE);
				paintAll(BLACK);
				gameFlag=0;
				break;
			}
		}

	}else{
		paintAll(cat_color);
		paintAll(BLACK);

		locate(10,10);
		prints("GAME OVER...");
		for(i=0; i < 20; i++){late(5);}
		locate(21,18);
		prints("Retry?");
		locate(17,19);
		prints("(Press Start)");
		locate(4,18);
		prints("Title?");
		locate(0,19);
		prints("(Press SELECT)");
		while(1){
			if(KEY_IN==KEY_START){break;}
			else if(KEY_IN==KEY_SELECT){
				paintAll(WHITE);
				paintAll(BLACK);
				gameFlag=0;
				break;
			}
		}
	}
}


/*スタート画面描画*/
void paintStartDisplay(){
	hword tx,ty;
	for(tx = 0; tx < 128; tx++){
		for(ty = 0; ty < 128; ty++){
			if(start_display[tx][ty]==0){
				draw_point(56+tx,16+ty,CYAN);
			}else if(start_display[tx][ty]==1){
				draw_point(56+tx,16+ty,BLACK);
			}else if(start_display[tx][ty]==2){
				draw_point(56+tx,16+ty,SHADE);
			}else if(start_display[tx][ty]==3){
				draw_point(56+tx,16+ty,FIELD2);
			}else if(start_display[tx][ty]==4){
				draw_point(56+tx,16+ty,WHITE);
			}else if(start_display[tx][ty]==5){
				draw_point(56+tx,16+ty,GREEN);
			}else if(start_display[tx][ty]==6){
				draw_point(56+tx,16+ty,RED);
			}else if(start_display[tx][ty]==7){
				draw_point(56+tx,16+ty,BLUE);
			}else if(start_display[tx][ty]==8){
				draw_point(56+tx,16+ty,MAZENTA&COLOR_SHADE);
			}else if(start_display[tx][ty]==9){
				draw_point(56+tx,16+ty,MAZENTA);
			}else if(start_display[tx][ty]==10){
				draw_point(56+tx,16+ty,SHADE3);
			}else if(start_display[tx][ty]==11){
				draw_point(56+tx,16+ty,SHADE2);
			}else if(start_display[tx][ty]==12){
				draw_point(56+tx,16+ty,BLUE&COLOR_SHADE);
			}else if(start_display[tx][ty]==13){
				draw_point(56+tx,16+ty,BLUE&COLOR_SHADE2);
			}else if(start_display[tx][ty]==14){
				draw_point(56+tx,16+ty,DOSHISHA);
			}else if(start_display[tx][ty]==15){
				draw_point(56+tx,16+ty,YELLOW);
			}
		}
	}
}

/*移動処理の本体*/
void move(){
	hword cx,cy;
	hword *ptr;
	ptr = (hword *)VRAM;
	switch (KEY_IN) {
		case KEY_RIGHT:

		if(c.x<219){
			ptr += c.x + 20 + c.y * 240;
			for(cy = 0; cy < 20; cy++){
				map[c.x+20][c.y+cy]=*ptr;
				ptr += 240;
			}
			delete(c.x,c.y);
			c.x++;
			character_paint(characters_r,cat_color);
			late(2);
		}

		break;

		case KEY_LEFT:

		if(c.x>0){
			ptr += c.x-1 + c.y * 240;
			for(cy = 0; cy < 20; cy++){
				map[c.x-1][c.y+cy]=*ptr;
				ptr += 240;
			}
			delete(c.x,c.y);
			c.x--;
			character_paint(characters_l,cat_color);
			late(2);
		}

		break;

		case KEY_UP:

		if(c.y>0){
			ptr += c.x + (c.y-1) * 240;
			for(cx = 0; cx < 20; cx++){map[c.x+cx][c.y-1]=*ptr++;}
			delete(c.x,c.y);
			c.y--;
			character_paint(characters_u,cat_color);
			late(2);
		}

		break;

		case KEY_DOWN:

		if(c.y<LCD_HEIGHT-76){
			ptr += c.x + (c.y + 20)*240;
			for(cx = 0; cx < 20; cx++){map[c.x+cx][c.y+20]=*ptr++;}
			delete(c.x,c.y);
			c.y++;
			character_paint(characters_d,cat_color);
			late(2);
		}

		break;

		case (KEY_RIGHT)+(KEY_UP):

		if(c.x<LCD_WIDTH-20&&c.y>0){
			ptr += c.x + 20 + c.y * 240;
			for(cy = 0; cy < 20; cy++){
				map[c.x+20][c.y+cy]=*ptr;
				ptr += 240;
			}

			ptr = (hword *)VRAM;
			ptr += c.x + (c.y-1) * 240;
			for(cx = 0; cx < 20; cx++){map[c.x+cx][c.y-1]=*ptr++;}

			delete(c.x,c.y);
			c.x++;
			c.y--;
		}else if(c.x<LCD_WIDTH-20){
			ptr += c.x + 20 + c.y * 240;
			for(cy = 0; cy < 20; cy++){
				map[c.x+20][c.y+cy]=*ptr;
				ptr += 240;
			}
			delete(c.x,c.y);
			c.x++;
		}else if(c.y>0){
			ptr += c.x + (c.y-1) * 240;
			for(cx = 0; cx < 20; cx++){map[c.x+cx][c.y-1]=*ptr++;}
			delete(c.x,c.y);
			c.y--;
		}else{break;}

		character_paint(characters_ru,cat_color);
		late(2);

		break;

		case (KEY_LEFT)+(KEY_UP):

		if(c.x>0&&c.y>0){
			ptr += c.x-1 + c.y * 240;
			for(cy = 0; cy < 20; cy++){
				map[c.x-1][c.y+cy]=*ptr;
				ptr += 240;
			}

			ptr = (hword *)VRAM;
			ptr += c.x + (c.y-1) * 240;
			for(cx = 0; cx < 20; cx++){map[c.x+cx][c.y-1]=*ptr++;}
			delete(c.x,c.y);
			c.x--;
			c.y--;
		}else if(c.x>0){
			ptr += c.x-1 + c.y * 240;
			for(cy = 0; cy < 20; cy++){
				map[c.x-1][c.y+cy]=*ptr;
				ptr += 240;
			}
			delete(c.x,c.y);
			c.x--;
		}else if(c.y>0){
			ptr += c.x + (c.y-1) * 240;
			for(cx = 0; cx < 20; cx++){map[c.x+cx][c.y-1]=*ptr++;}
			delete(c.x,c.y);
			c.y--;
		}else{break;}

		character_paint(characters_lu,cat_color);
		late(2);

		break;

		case (KEY_RIGHT)+(KEY_DOWN):

		if(c.x<LCD_WIDTH-20&&c.y<LCD_HEIGHT-76){
			ptr += c.x + 20 + c.y * 240;
			for(cy = 0; cy < 20; cy++){
				map[c.x+20][c.y+cy]=*ptr;
				ptr += 240;
			}

			ptr = (hword *)VRAM;
			ptr += c.x + (c.y + 20)*240;
			for(cx = 0; cx < 20; cx++){map[c.x+cx][c.y+20]=*ptr++;}

			delete(c.x,c.y);
			c.x++;
			c.y++;
		}else if(c.x<LCD_WIDTH-20){
			ptr += c.x + 20 + c.y * 240;
			for(cy = 0; cy < 20; cy++){
				map[c.x+20][c.y+cy]=*ptr;
				ptr += 240;
			}
			delete(c.x,c.y);
			c.x++;
		}else if(c.y<LCD_HEIGHT-76){
			ptr += c.x + (c.y + 20)*240;
			for(cx = 0; cx < 20; cx++){map[c.x+cx][c.y+20]=*ptr++;}
			delete(c.x,c.y);
			c.y++;
		}else{break;}

		character_paint(characters_rd,cat_color);
		late(2);

		break;

		case (KEY_LEFT)+(KEY_DOWN):

		if(c.x>0&&c.y<LCD_HEIGHT-76){
			ptr += c.x-1 + c.y * 240;
			for(cy = 0; cy < 20; cy++){
				map[c.x-1][c.y+cy]=*ptr;
				ptr += 240;
			}

			ptr = (hword *)VRAM;
			ptr += c.x + (c.y + 20)*240;
			for(cx = 0; cx < 20; cx++){map[c.x+cx][c.y+20]=*ptr++;}

			delete(c.x,c.y);
			c.x--;
			c.y++;
		}else if(c.x>0){
			ptr += c.x-1 + c.y * 240;
			for(cy = 0; cy < 20; cy++){
				map[c.x-1][c.y+cy]=*ptr;
				ptr += 240;
			}

			delete(c.x,c.y);
			c.x--;
		}else if(c.y<LCD_HEIGHT-76){
			ptr += c.x + (c.y + 20)*240;
			for(cx = 0; cx < 20; cx++){map[c.x+cx][c.y+20]=*ptr++;}
			delete(c.x,c.y);
			c.y++;
		}else{break;}

		character_paint(characters_ld,cat_color);
		late(2);

		break;

		default:
		character_paint(characters,cat_color);
		break;
	}
}

/*移動した後塗りつぶし*/
void delete(hword x, hword y){
	hword i,j;
	hword *ptr;
	ptr = (hword *)VRAM;
	switch (KEY_IN) {
		case KEY_LEFT:
		ptr += x + 19 + y*240;
		for(j=y; j < y+20; j++){
			*ptr = map[x + 19][j];
			ptr += 240;
		}
		break;

		case	KEY_RIGHT:
		ptr += x + y * 240;
		for(j=y; j < y+20; j++){
			*ptr = map[x][j];
			ptr += 240;
		}
		break;

		case KEY_DOWN:
		ptr += x + y * 240;
		for(i=x; i < x+20; i++){
			*ptr++ = map[i][y];
		}
		break;

		case KEY_UP:
		ptr += x + (y+19) * 240;
		for(i=x; i < x+20; i++){
			*ptr++ = map[i][y+19];
		}
		break;

		case (KEY_LEFT)+(KEY_UP):
		ptr += x + 19 + y*240;
		for(j=y; j < y+20; j++){
			*ptr = map[x + 19][j];
			ptr += 240;
		}
		ptr = (hword *)VRAM;
		ptr += x + (y+19) * 240;
		for(i=x; i < x+20; i++){
			*ptr++ = map[i][y+19];
		}
		break;

		case (KEY_RIGHT)+(KEY_UP):
		ptr += x + y * 240;
		for(j=y; j < y+20; j++){
			*ptr = map[x][j];
			ptr += 240;
		}
		ptr = (hword *)VRAM;
		ptr += x + (y+19) * 240;
		for(i=x; i < x+20; i++){
			*ptr++ = map[i][y+19];
		}
		break;

		case (KEY_LEFT)+(KEY_DOWN):
		ptr += x + 19 + y * 240;
		for(j=y; j < y+20; j++){
			*ptr = map[x + 19][j];
			ptr += 240;
		}
		ptr = (hword *)VRAM;
		ptr += x + y * 240;
		for(i=x; i < x+20; i++){
			*ptr = map[i][y];
			ptr += 1;
		}
		break;

		case (KEY_RIGHT)+(KEY_DOWN):
		ptr += x + y * 240;
		for(j=y; j < y+20; j++){
			*ptr = map[x][j];
			ptr += 240;
		}
		ptr = (hword *)VRAM;
		ptr += x + y * 240;
		for(i=x; i < x+20; i++){
			*ptr = map[i][y];
			ptr += 1;
		}
		break;

		default:

		break;
	}
}

/*時間を描画*/
void display_time(hword val) {


/* 引数を10進数として表示する関数（最大値16ビット）*/

	if(val>=600){val=600;}

	byte char_data[] = "0123456789";
	byte buf[10];
	hword tmp;
	//hword min  = div(val, 600);           // 分
	hword sec  = div(mod(val, 600), 10);  // 秒
	hword dsec = mod(mod(val, 600), 10);  // デシ秒(0.1秒)
	int i;

	/* 入力値valの桁数設定 */
	i = 3;

	/* 文字列の最後にNULLコード挿入 */
	buf[i+1] = 0;

	/* 最下位桁の文字（10で割った余り）を挿入し，入力値を10で割る */
	buf[i--] = char_data[dsec];
	buf[i--] = '.';
	for(; i >= 0; i--) {
		buf[i] = char_data[mod(sec, 10)];
		sec = div(sec, 10);
	}
	/*buf[i--] = ':';
	for(; i >= 0; i--) {
		buf[i] = char_data[mod(min, 10)];
		min = div(min, 10);
	}*/

	/* 文字列全体を表示 */
	prints(buf);

	return;
}

/*タイマのオンオフ*/
void turn_timer(byte sw) {
	if (sw == TRUE) {  /* タイマON */
		/* タイマ制御レジスタ */
		*((unsigned short *)0x04000102) = 0x0080;	// タイマ0 制御設定（タイマON，割込みOFF，カスケードOFF，プリスケーラなし）
		*((unsigned short *)0x04000106) = 0x0084;	// タイマ1 制御設定（タイマON，割込みOFF，カスケードON，プリスケーラなし）
		*((unsigned short *)0x0400010A) = 0x0084;	// タイマ2 制御設定（タイマON，割込みOFF，カスケードON，プリスケーラなし）
		*((unsigned short *)0x0400010E) = 0x0084;	// タイマ3 制御設定（タイマON，割込みOFF，カスケードON，プリスケーラなし）
	} else {           /* タイマOFF */
		/* タイマ制御レジスタ */
		*((unsigned short *)0x04000102) = 0x0000;	// タイマ0 制御設定（タイマOFF，割込みOFF，カスケードOFF，プリスケーラなし）
		*((unsigned short *)0x04000106) = 0x0004;	// タイマ1 制御設定（タイマOFF，割込みOFF，カスケードON，プリスケーラなし）
		*((unsigned short *)0x0400010A) = 0x0004;	// タイマ2 制御設定（タイマOFF，割込みOFF，カスケードON，プリスケーラなし）
		*((unsigned short *)0x0400010E) = 0x0004;	// タイマ3 制御設定（タイマOFF，割込みOFF，カスケードON，プリスケーラなし）
	}
}

/*グルグルと塗りつぶす*/
void paintAll(hword color){
	hword x,y;
	hword count;
	for( y = 0; y < LCD_HEIGHT; y++){
		for(x = 160; x < LCD_WIDTH; x++){
			draw_point(x, y, color);
		}
		late(1);
	}
	for( x = 161; x > 0; x--){
		for( y = 103; y < LCD_HEIGHT; y++){
			draw_point(x-1, y, color);
		}
		late(1);
	}
	for( y = 104; y > 0; y--){
		for(x = 0; x < 80; x++){
			draw_point(x, y-1, color);
		}
		late(1);
	}
	for( x = 80; x < 160; x++){
		for(y = 0; y < 53; y++ ){
			draw_point(x, y, color);
		}
		late(1);
	}
	for( y = 53; y < 103; y++){
		for( x = 80; x < 160; x++){
			draw_point(x, y, color);
		}
		late(1);
	}
}

//下の吹き出し描画
void paint_edge(){

	hword i,j;

	for(i = 0; i < LCD_CHAR_WIDTH; i++){
		for(j = 0; j < LCD_CHAR_HEIGHT; j++){

			if(edge[i][j]==1){
				locate(i,j);
				prints("-");
			}else if(edge[i][j]==2){
				locate(i,j);
				prints("|");
			}else if(edge[i][j]==3){
				locate(i,j);
				prints(" ");																	//セリフの吹き出し描画
			}
		}
	}
}

/*遅延関数*/
void late(hword a) {
	hword count, i;
	switch(a){
		case 1: for ( i = 0; i < 500; i++){count++;} break;
		case 2: for ( i = 0; i < 4000; i++){count++;} break;
		case 3: for ( i = 0; i < 10000; i++){count++;} break;
		case 4: for ( i = 0; i < 30000; i++){count++;} break;
		case 5: for ( i = 0; i < 0xFFFF; i++){count++;} break;
	}
}

/*主人公猫描画*/
void character_paint(hword cat[20][20], hword color){
	hword cx, cy;

	for (cx = 0; cx < 20; cx++) {

		for (cy = 0; cy < 20; cy++) {

			if (cat[cx][cy] == 0){
				draw_point(cx + c.x, cy + c.y, map[cx+c.x][cy+c.y]);
			}else if (cat[cx][cy] == 1) {
				draw_point(cx + c.x, cy + c.y, BLACK);				//輪郭
			}else if (cat[cx][cy] == 2) {
				draw_point(cx + c.x, cy + c.y, SHADE);	//影
			}else if (cat[cx][cy] == 3) {
				draw_point(cx + c.x, cy + c.y, WHITE);	//体
			}else if (cat[cx][cy] == 4) {
				draw_point(cx + c.x, cy + c.y, color);	//色
			}else if (cat[cx][cy] == 5) {
				draw_point(cx + c.x, cy + c.y,  color & BGR(0x15, 0x15, 0x15));	//色の影
			}else if (cat[cx][cy] == 6) {
				draw_point(cx + c.x, cy + c.y, SHADE2);		//影2
			}else if (cat[cx][cy] == 7){
				draw_point(cx + c.x, cy + c.y, SHADE3);				//体の影
			}
		}
	}
}

/*猫の位置を指定する関数*/																	//20×20マス毎に動かそうとしたがよりなめらかに動かすために1ピクセル毎にした
void locate_c(hword cx,hword cy) {
if(cx < LCD_WIDTH){
	c.x	=	cx;
}else{
	c.x	=	LCD_WIDTH-1;
}
if(cy < LCD_HEIGHT){
	c.y = cy;
}else{
	c.y = LCD_HEIGHT-1;
}
/*
if(cx < 12){
	c.x	=	cx  * 20;
}else{
	c.x	=	11 * 20;
}
if(cy < 8){
	c.y = cy * 20;
}else{
	c.y = 9 * 20;
}*/
}


/*お宝かどうか調べる*/
hword check(hword tx,hword ty,hword t[12][8]){
	if(t[tx][ty]==cat_color){
		return TRUE;
	}else if(t[tx][ty]==2||t[tx][ty]==0){
	 	return FALSE;
	}else{
		stopFlag++;
		return FALSE;
	}
}

/*今いる位置とお宝の当たり判定*/
void discover(hword t[12][8]){
	hword x = c.x+10;					//判定基準を猫の中心にするため
	hword y = c.y+10;					//判定基準を猫の中心にするため
	if(x<=19){
		if(y<=19){setFlag(check(0,0,t));
		}else if(y<=39){setFlag(check(0,1,t));
		}else if(y<=59){setFlag(check(0,2,t));
		}else if(y<=79){setFlag(check(0,3,t));
		}else if(y<=99){setFlag(check(0,4,t));
		}else if(y<=119){setFlag(check(0,5,t));
		}else if(y<=139){setFlag(check(0,6,t));
		}
	}else if(x<=39){
		if(y<=19){setFlag(check(1,0,t));
		}else if(y<=39){setFlag(check(1,1,t));
		}else if(y<=59){setFlag(check(1,2,t));
		}else if(y<=79){setFlag(check(1,3,t));
		}else if(y<=99){setFlag(check(1,4,t));
		}else if(y<=119){setFlag(check(1,5,t));
		}else if(y<=139){setFlag(check(1,6,t));
		}
	}else if(x<=59){
		if(y<=19){setFlag(check(2,0,t));
		}else if(y<=39){setFlag(check(2,1,t));
		}else if(y<=59){setFlag(check(2,2,t));
		}else if(y<=79){setFlag(check(2,3,t));
		}else if(y<=99){setFlag(check(2,4,t));
		}else if(y<=119){setFlag(check(2,5,t));
		}else if(y<=139){setFlag(check(2,6,t));
		}
	}else if(x<=79){
		if(y<=19){setFlag(check(3,0,t));
		}else if(y<=39){setFlag(check(3,1,t));
		}else if(y<=59){setFlag(check(3,2,t));
		}else if(y<=79){setFlag(check(3,3,t));
		}else if(y<=99){setFlag(check(3,4,t));
		}else if(y<=119){setFlag(check(3,5,t));
		}else if(y<=139){setFlag(check(3,6,t));
		}
	}else if(x<=99){
		if(y<=19){setFlag(check(4,0,t));
		}else if(y<=39){setFlag(check(4,1,t));
		}else if(y<=59){setFlag(check(4,2,t));
		}else if(y<=79){setFlag(check(4,3,t));
		}else if(y<=99){setFlag(check(4,4,t));
		}else if(y<=119){setFlag(check(4,5,t));
		}else if(y<=139){setFlag(check(4,6,t));
		}
	}else if(x<=119){
		if(y<=19){setFlag(check(5,0,t));
		}else if(y<=39){setFlag(check(5,1,t));
		}else if(y<=59){setFlag(check(5,2,t));
		}else if(y<=79){setFlag(check(5,3,t));
		}else if(y<=99){setFlag(check(5,4,t));
		}else if(y<=119){setFlag(check(5,5,t));
		}else if(y<=139){setFlag(check(5,6,t));
		}
	}else if(x<=139){
		if(y<=19){setFlag(check(6,0,t));
		}else if(y<=39){setFlag(check(6,1,t));
		}else if(y<=59){setFlag(check(6,2,t));
		}else if(y<=79){setFlag(check(6,3,t));
		}else if(y<=99){setFlag(check(6,4,t));
		}else if(y<=119){setFlag(check(6,5,t));
		}else if(y<=139){setFlag(check(6,6,t));
		}
	}else if(x<=159){
		if(y<=19){setFlag(check(7,0,t));
		}else if(y<=39){setFlag(check(7,1,t));
		}else if(y<=59){setFlag(check(7,2,t));
		}else if(y<=79){setFlag(check(7,3,t));
		}else if(y<=99){setFlag(check(7,4,t));
		}else if(y<=119){setFlag(check(7,5,t));
		}else if(y<=139){setFlag(check(7,6,t));
		}
	}else if(x<=179){
		if(y<=19){setFlag(check(8,0,t));
		}else if(y<=39){setFlag(check(8,1,t));
		}else if(y<=59){setFlag(check(8,2,t));
		}else if(y<=79){setFlag(check(8,3,t));
		}else if(y<=99){setFlag(check(8,4,t));
		}else if(y<=119){setFlag(check(8,5,t));
		}else if(y<=139){setFlag(check(8,6,t));
		}
	}else if(x<=199){
		if(y<=19){setFlag(check(9,0,t));
		}else if(y<=39){setFlag(check(9,1,t));
		}else if(y<=59){setFlag(check(9,2,t));
		}else if(y<=79){setFlag(check(9,3,t));
		}else if(y<=99){setFlag(check(9,4,t));
		}else if(y<=119){setFlag(check(9,5,t));
		}else if(y<=139){setFlag(check(9,6,t));
		}
	}else if(x<=219){
		if(y<=19){setFlag(check(10,0,t));
		}else if(y<=39){setFlag(check(10,1,t));
		}else if(y<=59){setFlag(check(10,2,t));
		}else if(y<=79){setFlag(check(10,3,t));
		}else if(y<=99){setFlag(check(10,4,t));
		}else if(y<=119){setFlag(check(10,5,t));
		}else if(y<=139){setFlag(check(10,6,t));
		}
	}else if(x<=239){
		if(y<=19){setFlag(check(11,0,t));
		}else if(y<=39){setFlag(check(11,1,t));
		}else if(y<=59){setFlag(check(11,2,t));
		}else if(y<=79){setFlag(check(11,3,t));
		}else if(y<=99){setFlag(check(11,4,t));
		}else if(y<=119){setFlag(check(11,5,t));
		}else if(y<=139){setFlag(check(11,6,t));
		}
	}
}

/*今の画面の状態を配列に保持*/
void mapping(){
	hword *ptr;
	hword i,j;
	ptr = (hword *)VRAM;

	for(j=0; j < LCD_HEIGHT; j++){
		for(i=0; i < LCD_WIDTH; i++){
			map[i][j] = *ptr++;
		}
	}
}

/*お宝の穴を描画する*/
void treasurePaint(hword t[12][8]){
	hword i,j,cx,cy;
	for(i=0; i < 12; i++){
		for(j=0; j < 8; j++){
			h.x=i*20,h.y=j*20;
			if(t[i][j]==2){
				for(cx=0; cx<20; cx++){
					for(cy=0; cy<20; cy++){
						if(hole2[cx][cy]==1){
							map[h.x+cx][h.y+cy] = BLACK;
							draw_point(h.x + cx, h.y + cy, BLACK);
						}else if(hole2[cx][cy]==2){
							map[h.x+cx][h.y+cy] = HOLE&COLOR_SHADE;
							draw_point(h.x + cx, h.y + cy, HOLE&COLOR_SHADE);
						}else if(hole2[cx][cy]==3){
							map[h.x+cx][h.y+cy] = HOLE;
							draw_point(h.x + cx, h.y + cy, HOLE);
						}else{
							if(mapFlag==1){
								map[h.x+cx][h.y+cy] = FIELD;
								draw_point(h.x + cx, h.y + cy, FIELD);
							}else if(mapFlag==2 || mapFlag==0){
								map[h.x+cx][h.y+cy] = FIELD2;
								draw_point(h.x + cx, h.y + cy, FIELD2);
							}else{
								map[h.x+cx][h.y+cy] = FIELD3;
								draw_point(h.x + cx, h.y + cy, FIELD3);
							}
						}
					}
				}
			}
			else if(t[i][j]!=0){
				for(cx=0; cx<20; cx++){
					for(cy=0; cy<20; cy++){
						if(hole[cx][cy]==1){
							draw_point(h.x + cx, h.y + cy, BLACK);
						}else if(hole[cx][cy]==2){
							draw_point(h.x + cx, h.y + cy, HOLE&COLOR_SHADE);
						}else if(hole[cx][cy]==3){
							draw_point(h.x + cx, h.y + cy, HOLE);
						}else{
							if(mapFlag==1){
								map[h.x+cx][h.y+cy] = FIELD;
								draw_point(h.x + cx, h.y + cy, FIELD);
							}else if(mapFlag==2 || mapFlag==0){
								map[h.x+cx][h.y+cy] = FIELD2;
								draw_point(h.x + cx, h.y + cy, FIELD2);
							}else{
								map[h.x+cx][h.y+cy] = FIELD3;
								draw_point(h.x + cx, h.y + cy, FIELD3);
							}
						}
					}
				}
			}
		}
	}
}

/*穴に落ちた猫を描画する*/
void pitfallPaint(hword t[12][8], hword field){
	hword x = c.x+10;
	hword y = c.y+10;
	hword tx,ty;

	/*穴とずれた位置にいる猫を消す*/
	for(tx=0; tx < 20; tx++){
		for(ty=0; ty < 20; ty++){
			draw_point(c.x+tx, c.y+ty, map[c.x+tx][c.y+ty]);			//マップ情報を格納した配列で塗りつぶす
		}
	}

	/*猫を穴の上に動かす*/
	if(x<=19){
		if(y<=19){if(t[0][0]!=0){c.x = 0,c.y = 0;}
	}else if(y<=39){if(t[0][1]!=0){c.x = 0,c.y = 20;}
}else if(y<=59){if(t[0][2]!=0){c.x = 0,c.y = 40;}
}else if(y<=79){if(t[0][3]!=0){c.x = 0,c.y = 60;}
}else if(y<=99){if(t[0][4]!=0){c.x = 0,c.y = 80;}
}else if(y<=119){if(t[0][5]!=0){c.x = 0,c.y = 100;}
}else if(y<=139){if(t[0][6]!=0){c.x = 0,c.y = 120;}
}
}else if(x<=39){
	if(y<=19){if(t[1][0]!=0){c.x = 20,c.y = 0;}
}else if(y<=39){if(t[1][1]!=0){c.x = 20,c.y = 20;}
}else if(y<=59){if(t[1][2]!=0){c.x = 20,c.y = 40;}
}else if(y<=79){if(t[1][3]!=0){c.x = 20,c.y = 60;}
}else if(y<=99){if(t[1][4]!=0){c.x = 20,c.y = 80;}
}else if(y<=119){if(t[1][5]!=0){c.x = 20,c.y = 100;}
}else if(y<=139){if(t[1][6]!=0){c.x = 20,c.y = 120;}
}
}else if(x<=59){
	if(y<=19){if(t[2][0]!=0){c.x = 40,c.y = 0;}
}else if(y<=39){if(t[2][1]!=0){c.x = 40,c.y = 20;}
}else if(y<=59){if(t[2][2]!=0){c.x = 40,c.y = 40;}
}else if(y<=79){if(t[2][3]!=0){c.x = 40,c.y = 60;}
}else if(y<=99){if(t[2][4]!=0){c.x = 40,c.y = 80;}
}else if(y<=119){if(t[2][5]!=0){c.x = 40,c.y = 100;}
}else if(y<=139){if(t[2][6]!=0){c.x = 40,c.y = 120;}
}
}else if(x<=79){
	if(y<=19){if(t[3][0]!=0){c.x = 60,c.y = 0;}
}else if(y<=39){if(t[3][1]!=0){c.x = 60,c.y = 20;}
}else if(y<=59){if(t[3][2]!=0){c.x = 60,c.y = 40;}
}else if(y<=79){if(t[3][3]!=0){c.x = 60,c.y = 60;}
}else if(y<=99){if(t[3][4]!=0){c.x = 60,c.y = 80;}
}else if(y<=119){if(t[3][5]!=0){c.x = 60,c.y = 100;}
}else if(y<=139){if(t[3][6]!=0){c.x = 60,c.y = 120;}
}
}else if(x<=99){
	if(y<=19){if(t[4][0]!=0){c.x = 80,c.y = 0;}
}else if(y<=39){if(t[4][1]!=0){c.x = 80,c.y = 20;}
}else if(y<=59){if(t[4][2]!=0){c.x = 80,c.y = 40;}
}else if(y<=79){if(t[4][3]!=0){c.x = 80,c.y = 60;}
}else if(y<=99){if(t[4][4]!=0){c.x = 80,c.y = 80;}
}else if(y<=119){if(t[4][5]!=0){c.x = 80,c.y = 100;}
}else if(y<=139){if(t[4][6]!=0){c.x = 80,c.y = 120;}
}
}else if(x<=119){
	if(y<=19){if(t[5][0]!=0){c.x = 100,c.y = 0;}
}else if(y<=39){if(t[5][1]!=0){c.x = 100,c.y = 20;}
}else if(y<=59){if(t[5][2]!=0){c.x = 100,c.y = 40;}
}else if(y<=79){if(t[5][3]!=0){c.x = 100,c.y = 60;}
}else if(y<=99){if(t[5][4]!=0){c.x = 100,c.y = 80;}
}else if(y<=119){if(t[5][5]!=0){c.x = 100,c.y = 100;}
}else if(y<=139){if(t[5][6]!=0){c.x = 100,c.y = 120;}
}
}else if(x<=139){
	if(y<=19){if(t[6][0]!=0){c.x = 120,c.y = 0;}
}else if(y<=39){if(t[6][1]!=0){c.x = 120,c.y = 20;}
}else if(y<=59){if(t[6][2]!=0){c.x = 120,c.y = 40;}
}else if(y<=79){if(t[6][3]!=0){c.x = 120,c.y = 60;}
}else if(y<=99){if(t[6][4]!=0){c.x = 120,c.y = 80;}
}else if(y<=119){if(t[6][5]!=0){c.x = 120,c.y = 100;}
}else if(y<=139){if(t[6][6]!=0){c.x = 120,c.y = 120;}
}
}else if(x<=159){
	if(y<=19){if(t[7][0]!=0){c.x = 140,c.y = 0;}
}else if(y<=39){if(t[7][1]!=0){c.x = 140,c.y = 20;}
}else if(y<=59){if(t[7][2]!=0){c.x = 140,c.y = 40;}
}else if(y<=79){if(t[7][3]!=0){c.x = 140,c.y = 60;}
}else if(y<=99){if(t[7][4]!=0){c.x = 140,c.y = 80;}
}else if(y<=119){if(t[7][5]!=0){c.x = 140,c.y = 100;}
}else if(y<=139){if(t[7][6]!=0){c.x = 140,c.y = 120;}
}
}else if(x<=179){
	if(y<=19){if(t[8][0]!=0){c.x = 160,c.y = 0;}
}else if(y<=39){if(t[8][1]!=0){c.x = 160,c.y = 20;}
}else if(y<=59){if(t[8][2]!=0){c.x = 160,c.y = 40;}
}else if(y<=79){if(t[8][3]!=0){c.x = 160,c.y = 60;}
}else if(y<=99){if(t[8][4]!=0){c.x = 160,c.y = 80;}
}else if(y<=119){if(t[8][5]!=0){c.x = 160,c.y = 100;}
}else if(y<=139){if(t[8][6]!=0){c.x = 160,c.y = 120;}
}
}else if(x<=199){
	if(y<=19){if(t[9][0]!=0){c.x = 180,c.y = 0;}
}else if(y<=39){if(t[9][1]!=0){c.x = 180,c.y = 20;}
}else if(y<=59){if(t[9][2]!=0){c.x = 180,c.y = 40;}
}else if(y<=79){if(t[9][3]!=0){c.x = 180,c.y = 60;}
}else if(y<=99){if(t[9][4]!=0){c.x = 180,c.y = 80;}
}else if(y<=119){if(t[9][5]!=0){c.x = 180,c.y = 100;}
}else if(y<=139){if(t[9][6]!=0){c.x = 180,c.y = 120;}
}
}else if(x<=219){
	if(y<=19){if(t[10][0]!=0){c.x = 200,c.y = 0;}
}else if(y<=39){if(t[10][1]!=0){c.x = 200,c.y = 20;}
}else if(y<=59){if(t[10][2]!=0){c.x = 200,c.y = 40;}
}else if(y<=79){if(t[10][3]!=0){c.x = 200,c.y = 60;}
}else if(y<=99){if(t[10][4]!=0){c.x = 200,c.y = 80;}
}else if(y<=119){if(t[10][5]!=0){c.x = 200,c.y = 100;}
}else if(y<=139){if(t[10][6]!=0){c.x = 200,c.y = 120;}
}
}else if(x<=239){
	if(y<=19){if(t[11][0]!=0){c.x = 220,c.y = 0;}
}else if(y<=39){if(t[11][1]!=0){c.x = 220,c.y = 20;}
}else if(y<=59){if(t[11][2]!=0){c.x = 220,c.y = 40;}
}else if(y<=79){if(t[11][3]!=0){c.x = 220,c.y = 60;}
}else if(y<=99){if(t[11][4]!=0){c.x = 220,c.y = 80;}
}else if(y<=119){if(t[11][5]!=0){c.x = 220,c.y = 100;}
}else if(y<=139){if(t[11][6]!=0){c.x = 220,c.y = 120;}
}
}

for(tx=0; tx < 20; tx++){
	for(ty=0; ty < 20; ty++){
		if(hole3[tx][ty]==0){
			draw_point(c.x+tx,c.y+ty,field);
		}else if(hole3[tx][ty]==1){
			draw_point(c.x+tx,c.y+ty,BLACK);
		}else if(hole3[tx][ty]==2){
			draw_point(c.x+tx,c.y+ty,HOLE&COLOR_SHADE);
		}else if(hole3[tx][ty]==3){
			draw_point(c.x+tx,c.y+ty,HOLE);
		}else if(hole3[tx][ty]==4){
			draw_point(c.x+tx,c.y+ty,WHITE);
		}else if(hole3[tx][ty]==5){
			draw_point(c.x+tx,c.y+ty,SHADE2);	//影
		}else if(hole3[tx][ty]==6){
			draw_point(c.x+tx,c.y+ty,cat_color);
		}else if(hole3[tx][ty]==7){
			draw_point(c.x+tx,c.y+ty,cat_color & COLOR_SHADE);
		}else if(hole3[tx][ty]==8){
			draw_point(c.x+tx,c.y+ty,RED);
		}
	}
}
}

/*穴を掘る判定*/
void dig(hword t[12][8]){
	hword x = c.x+10;
	hword y = c.y+10;
	if(x<=19){
		if(y<=19){if(t[0][0]!=0){t[0][0]=2;}
		}else if(y<=39){if(t[0][1]!=0){t[0][1]=2;}
		}else if(y<=59){if(t[0][2]!=0){t[0][2]=2;}
		}else if(y<=79){if(t[0][3]!=0){t[0][3]=2;}
		}else if(y<=99){if(t[0][4]!=0){t[0][4]=2;}
		}else if(y<=119){if(t[0][5]!=0){t[0][5]=2;}
		}else if(y<=139){if(t[0][6]!=0){t[0][6]=2;}
		}
	}else if(x<=39){
		if(y<=19){if(t[1][0]!=0){t[1][0]=2;}
		}else if(y<=39){if(t[1][1]!=0){t[1][1]=2;}
		}else if(y<=59){if(t[1][2]!=0){t[1][2]=2;}
		}else if(y<=79){if(t[1][3]!=0){t[1][3]=2;}
		}else if(y<=99){if(t[1][4]!=0){t[1][4]=2;}
		}else if(y<=119){if(t[1][5]!=0){t[1][5]=2;}
		}else if(y<=139){if(t[1][6]!=0){t[1][6]=2;}
		}
	}else if(x<=59){
		if(y<=19){if(t[2][0]!=0){t[2][0]=2;}
		}else if(y<=39){if(t[2][1]!=0){t[2][1]=2;}
		}else if(y<=59){if(t[2][2]!=0){t[2][2]=2;}
		}else if(y<=79){if(t[2][3]!=0){t[2][3]=2;}
		}else if(y<=99){if(t[2][4]!=0){t[2][4]=2;}
		}else if(y<=119){if(t[2][5]!=0){t[2][5]=2;}
		}else if(y<=139){if(t[2][6]!=0){t[2][6]=2;}
		}
	}else if(x<=79){
		if(y<=19){if(t[3][0]!=0){t[3][0]=2;}
		}else if(y<=39){if(t[3][1]!=0){t[3][1]=2;}
		}else if(y<=59){if(t[3][2]!=0){t[3][2]=2;}
		}else if(y<=79){if(t[3][3]!=0){t[3][3]=2;}
		}else if(y<=99){if(t[3][4]!=0){t[3][4]=2;}
		}else if(y<=119){if(t[3][5]!=0){t[3][5]=2;}
		}else if(y<=139){if(t[3][6]!=0){t[3][6]=2;}
		}
	}else if(x<=99){
		if(y<=19){if(t[4][0]!=0){t[4][0]=2;}
		}else if(y<=39){if(t[4][1]!=0){t[4][1]=2;}
		}else if(y<=59){if(t[4][2]!=0){t[4][2]=2;}
		}else if(y<=79){if(t[4][3]!=0){t[4][3]=2;}
		}else if(y<=99){if(t[4][4]!=0){t[4][4]=2;}
		}else if(y<=119){if(t[4][5]!=0){t[4][5]=2;}
		}else if(y<=139){if(t[4][6]!=0){t[4][6]=2;}
		}
	}else if(x<=119){
		if(y<=19){if(t[5][0]!=0){t[5][0]=2;}
		}else if(y<=39){if(t[5][1]!=0){t[5][1]=2;}
		}else if(y<=59){if(t[5][2]!=0){t[5][2]=2;}
		}else if(y<=79){if(t[5][3]!=0){t[5][3]=2;}
		}else if(y<=99){if(t[5][4]!=0){t[5][4]=2;}
		}else if(y<=119){if(t[5][5]!=0){t[5][5]=2;}
		}else if(y<=139){if(t[5][6]!=0){t[5][6]=2;}
		}
	}else if(x<=139){
		if(y<=19){if(t[6][0]!=0){t[6][0]=2;}
		}else if(y<=39){if(t[6][1]!=0){t[6][1]=2;}
		}else if(y<=59){if(t[6][2]!=0){t[6][2]=2;}
		}else if(y<=79){if(t[6][3]!=0){t[6][3]=2;}
		}else if(y<=99){if(t[6][4]!=0){t[6][4]=2;}
		}else if(y<=119){if(t[6][5]!=0){t[6][5]=2;}
		}else if(y<=139){if(t[6][6]!=0){t[6][6]=2;}
		}
	}else if(x<=159){
		if(y<=19){if(t[7][0]!=0){t[7][0]=2;}
		}else if(y<=39){if(t[7][1]!=0){t[7][1]=2;}
		}else if(y<=59){if(t[7][2]!=0){t[7][2]=2;}
		}else if(y<=79){if(t[7][3]!=0){t[7][3]=2;}
		}else if(y<=99){if(t[7][4]!=0){t[7][4]=2;}
		}else if(y<=119){if(t[7][5]!=0){t[7][5]=2;}
		}else if(y<=139){if(t[7][6]!=0){t[7][6]=2;}
		}
	}else if(x<=179){
		if(y<=19){if(t[8][0]!=0){t[8][0]=2;}
		}else if(y<=39){if(t[8][1]!=0){t[8][1]=2;}
		}else if(y<=59){if(t[8][2]!=0){t[8][2]=2;}
		}else if(y<=79){if(t[8][3]!=0){t[8][3]=2;}
		}else if(y<=99){if(t[8][4]!=0){t[8][4]=2;}
		}else if(y<=119){if(t[8][5]!=0){t[8][5]=2;}
		}else if(y<=139){if(t[8][6]!=0){t[8][6]=2;}
		}
	}else if(x<=199){
		if(y<=19){if(t[9][0]!=0){t[9][0]=2;}
		}else if(y<=39){if(t[9][1]!=0){t[9][1]=2;}
		}else if(y<=59){if(t[9][2]!=0){t[9][2]=2;}
		}else if(y<=79){if(t[9][3]!=0){t[9][3]=2;}
		}else if(y<=99){if(t[9][4]!=0){t[9][4]=2;}
		}else if(y<=119){if(t[9][5]!=0){t[9][5]=2;}
		}else if(y<=139){if(t[9][6]!=0){t[9][6]=2;}
		}
	}else if(x<=219){
		if(y<=19){if(t[10][0]!=0){t[10][0]=2;}
		}else if(y<=39){if(t[10][1]!=0){t[10][1]=2;}
		}else if(y<=59){if(t[10][2]!=0){t[10][2]=2;}
		}else if(y<=79){if(t[10][3]!=0){t[10][3]=2;}
		}else if(y<=99){if(t[10][4]!=0){t[10][4]=2;}
		}else if(y<=119){if(t[10][5]!=0){t[10][5]=2;}
		}else if(y<=139){if(t[10][6]!=0){t[10][6]=2;}
		}
	}else if(x<=239){
		if(y<=19){if(t[11][0]!=0){t[11][0]=2;}
		}else if(y<=39){if(t[11][1]!=0){t[11][1]=2;}
		}else if(y<=59){if(t[11][2]!=0){t[11][2]=2;}
		}else if(y<=79){if(t[11][3]!=0){t[11][3]=2;}
		}else if(y<=99){if(t[11][4]!=0){t[11][4]=2;}
		}else if(y<=119){if(t[11][5]!=0){t[11][5]=2;}
		}else if(y<=139){if(t[11][6]!=0){t[11][6]=2;}
		}
	}
}

/*クリアしたか否かのフラグを設定する*/
void setFlag(hword val){
	flag = val;
}

/*マップ切り替えフラグを設定*/
void setMap(hword value){
	mapFlag = value;
}

/*引数に渡したキーは長押ししても一回しか押されていないようにする*/
void pressKey(hword key){
	while(1){
		if(timeFlag!=0){
			locate(TX,TY);
			display_time(LIMIT_TIME-CURRENT_TIME);}			//ゲーム中はキーが押されている間でも時間を表示する
			if(KEY_IN!=key){
				break;
			}
		}
	}

/*キー入力待ち*/
void key_wait(hword value){
	while(1)if(KEY_IN==value){break;}
}

/*日本語の位置*/
void locate_ja(hword jx, hword jy){
	if(jx<LCD_CHAR_WIDTH){
		ja.x = jx << 3;
	}else{
		ja.x = (LCD_CHAR_WIDTH - 1) << 3;
	}
	if(jy<LCD_CHAR_HEIGHT){
		ja.y = jy << 3;
	}else{
		ja.y = (LCD_CHAR_HEIGHT - 1) << 3;
	}
}

/*日本語描画*/
void jaPrint(byte num) {

	hword tx, ty;							/* 文字パターンの座標指定 */
	byte cbit;								/* ビット判定 */

	for(ty = 0; ty < 8; ty++) {				/* 文字パターンのy座標指定 */

		cbit = 0x80;						/* 判定ビットを8ビットの最上位にセット */

		for(tx = 0; tx < 8; tx++) {							/* 文字パターンのx座標指定 */
			if((japanese[num][ty] & cbit) == cbit){			/* ビット位置の判定 */
				draw_point((ja.x + tx), (ja.y + ty), WHITE);	// 1なら白
			}else {
				draw_point((ja.x + tx), (ja.y + ty),  BLACK);	// 0なら黒
			}
			cbit = cbit >> 1;					/* 判定ビットを1ビット右へシフト */
		}
	}
	ja.x+=8;												//位置を右へ
}

/*穴に落ちた猫を描画*/
void panic_catPaint(){
	hword tx,ty,cx,cy;
	for(tx=0; tx < 24; tx++){
		for(ty=0; ty < 24; ty++){
			for(cx=0; cx < 4; cx++){
				for(cy=0; cy < 4; cy++){
					if(panic_cat[tx][ty]==0){
						draw_point(72+tx*4+cx,10+ty*4+cy,map[tx][ty]);
					}else if(panic_cat[tx][ty]==1){
						draw_point(72+tx*4+cx,10+ty*4+cy,BLACK);
					}else if(panic_cat[tx][ty]==2){
						draw_point(72+tx*4+cx,10+ty*4+cy,SHADE2);
					}else if(panic_cat[tx][ty]==3){
						draw_point(72+tx*4+cx,10+ty*4+cy,WHITE);
					}else if(panic_cat[tx][ty]==4){
						draw_point(72+tx*4+cx,10+ty*4+cy,cat_color);
					}else if(panic_cat[tx][ty]==5){
						draw_point(72+tx*4+cx,10+ty*4+cy,cat_color & COLOR_SHADE);
					}else if(panic_cat[tx][ty]==6){
						draw_point(72+tx*4+cx,10+ty*4+cy,RED);
					}else if(panic_cat[tx][ty]==8){
						draw_point(72+tx*4+cx,10+ty*4+cy,CYAN);
					}
				}
			}
		}
	}
}

/*スタート画面の雲をペイントする*/
void cloudPaint(){

	hword tx,ty;
	hword cx1,cy1;
	hword cx2,cy2;
	hword cx3,cy3;

	for(ty = 0; ty < 8; ty++){
		draw_point(cloud1.x,cloud1.y+ty,map[cloud1.x][cloud1.y+ty]);

		draw_point(cloud2.x,cloud2.y+ty,map[cloud2.x][cloud2.y+ty]);

		draw_point(cloud3.x,cloud3.y+ty,map[cloud3.x][cloud3.y+ty]);
		draw_point(cloud3.x+1,cloud3.y+ty,map[cloud3.x+1][cloud3.y+ty]);
	}

	cloud1.x++;
	cloud2.x++;
	cloud3.x+=2;

	if(cloud1.x >= 184){cloud1.x = 56;}
	if(cloud2.x >= 184){cloud2.x = 56;}
	if(cloud3.x >= 184){cloud3.x = 56;}

	for(tx=0; tx < 14; tx++){
		for(ty=0; ty < 8; ty++){

			cx1 = cloud1.x+tx,cy1 = cloud1.y+ty;
			cx2 = cloud2.x+tx,cy2 = cloud2.y+ty;
			cx3 = cloud3.x+tx,cy3 = cloud3.y+ty;

			if(cx1 >= 184){cx1 -= 128;}
			if(cx2 >= 184){cx2 -= 128;}
			if(cx3 >= 184){cx3 -= 128;}

			if(cloud[tx][ty]==0){
				draw_point(cx1,cy1,map[cx1][cy1]);
				draw_point(cx2,cy2,map[cx2][cy2]);
				draw_point(cx3,cy3,map[cx3][cy3]);
			}else if(cloud[tx][ty]==1){

				if(map[cx1][cy1]==CYAN){
					draw_point(cx1,cy1,BLACK);
				}else{
					draw_point(cx1,cy1,map[cx1][cy1]);
				}

				if(map[cx2][cy2]==CYAN){
					draw_point(cx2,cy2,BLACK);
				}else{
					draw_point(cx2,cy2,map[cx2][cy2]);
				}

				if(map[cx3][cy3]==CYAN){
					draw_point(cx3,cy3,BLACK);
				}else{
					draw_point(cx3,cy3,map[cx3][cy3]);
				}

			}else if(cloud[tx][ty]==2){

				if(map[cx1][cy1]==CYAN){
					draw_point(cx1,cy1,SHADE);
				}else{
					draw_point(cx1,cy1,map[cx1][cy1]);
				}

				if(map[cx2][cy2]==CYAN){
					draw_point(cx2,cy2,SHADE);
				}else{
					draw_point(cx2,cy2,map[cx2][cy2]);
				}

				if(map[cx3][cy3]==CYAN){
					draw_point(cx3,cy3,SHADE);
				}else{
					draw_point(cx3,cy3,map[cx3][cy3]);
				}

			}else if(cloud[tx][ty]==3){

				if(map[cx1][cy1]==CYAN){
					draw_point(cx1,cy1,WHITE);
				}else{
					draw_point(cx1,cy1,map[cx1][cy1]);
				}

				if(map[cx2][cy2]==CYAN){
					draw_point(cx2,cy2,WHITE);
				}else{
					draw_point(cx2,cy2,map[cx2][cy2]);
				}

				if(map[cx3][cy3]==CYAN){
					draw_point(cx3,cy3,WHITE);
				}else{
					draw_point(cx3,cy3,map[cx3][cy3]);
				}

			}else if(cloud[tx][ty]==6){

				if(map[cx1][cy1]==CYAN){
					draw_point(cx1,cy1,SHADE2);
				}else{
					draw_point(cx1,cy1,map[cx1][cy1]);
				}

				if(map[cx2][cy2]==CYAN){
					draw_point(cx2,cy2,SHADE2);
				}else{
					draw_point(cx2,cy2,map[cx2][cy2]);
				}

				if(map[cx3][cy3]==CYAN){
					draw_point(cx3,cy3,SHADE2);
				}else{
					draw_point(cx3,cy3,map[cx3][cy3]);
				}
			}
		}
	}
}

/*エンディングの缶を描画するメソッド*/
void canPaint(){
	hword tx,ty;
	for(tx=0; tx<20; tx++){
		for(ty=0; ty<20; ty++){
			if(can[tx][ty]==0){
				draw_point(ca.x+tx,ca.y+ty,map[ca.x+tx][ca.y+ty]);
			}else if(can[tx][ty]==1){
				draw_point(ca.x+tx,ca.y+ty,BLACK);
			}else if(can[tx][ty]==2){
				draw_point(ca.x+tx,ca.y+ty,SHADE);
			}else if(can[tx][ty]==3){
				draw_point(ca.x+tx,ca.y+ty,SHADE2);
			}else if(can[tx][ty]==4){
				draw_point(ca.x+tx,ca.y+ty,cat_color);
			}else if(can[tx][ty]==5){
				draw_point(ca.x+tx,ca.y+ty,BLUE);
			}
		}
	}
}
