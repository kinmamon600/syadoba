#include "DXLib.h"
#include<iostream>
#include<string>
#include<vector>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

//カード
const int CARD_WIDTH = 100;
const int CARD_HEIGHT = 140;
//手札
const int CARD_INTERVAL = 120;
const int HAND_Y = 550;
//フィールドカード
const int FIELD_CARD_Y = 300;
//フィールド
const int FIELD_X = 250;
const int FIELD_Y = 190;
const int FIELD_WIDTH = 1030;
const int FIELD_HEIGHT = 450;

class Input {
public:
	static int mouseX;
	static int mouseY;
};

int Input::mouseX{ -1 }; int Input::mouseY{ -1 };


class Card {
private:
	std::string m_name;
	int m_cost;
	int m_attack;
	int m_hp;
public:
	Card(std::string _name = "null", int _cost = 0, int _attack = 0, int _hp = 0);
	//カード描画
	void DrawCard(int x, int y);
	//手札の当たり判定
	bool HandDetection(int x, int y);
	//フィールドのカードの当たり判定
	bool FieldCardDetection(int x, int y);
};

Card::Card(std::string _name, int _cost, int _attack, int _hp) {
	m_name = _name;
	m_cost = _cost;
	m_attack = _attack;
	m_hp = _hp;
}

void Card::DrawCard(int x, int y) {
	std::string costText = std::to_string(m_cost);
	std::string attackText = std::to_string(m_attack);
	std::string hpText = std::to_string(m_hp);

	DrawBox(x, y, x + CARD_WIDTH, y + CARD_HEIGHT, GetColor(255, 255, 255), true);
	DrawString(x + 30, y + 30, m_name.c_str(), GetColor(0, 0, 0));
	DrawString(x + 10, y + 20, costText.c_str(), GetColor(66, 214, 107));
	DrawString(x + 20, y + 120, attackText.c_str(), GetColor(74, 168, 255));
	DrawString(x + 80, y + 120, hpText.c_str(), GetColor(255, 85, 85));
}

bool Card::HandDetection(int x, int y) {
	if (Input::mouseX >= x && Input::mouseY >= y && Input::mouseX <= x + CARD_WIDTH && Input::mouseY <= y + CARD_HEIGHT) { return true; }
	else {return false;	}
}

bool Card::FieldCardDetection(int x, int y) {
	if (Input::mouseX >= x && Input::mouseY >= y && Input::mouseX <= x + CARD_WIDTH && Input::mouseY <= y + CARD_HEIGHT) { return true; }
	else { return false; }
}


class GameObserver {
public:
	//フィールドの描画
	void DrawField();
	//フィールドの当たり判定
	bool FieldDetection();
};

void GameObserver::DrawField() {
	DrawBox(FIELD_X, FIELD_Y, FIELD_WIDTH, FIELD_HEIGHT, GetColor(100, 100, 100), false);
}

bool GameObserver::FieldDetection() {
	if (Input::mouseX >= FIELD_X && Input::mouseY >= FIELD_Y && Input::mouseX <= FIELD_X + FIELD_WIDTH && Input::mouseY <= FIELD_Y + FIELD_HEIGHT) {
		return true;
	}
	else {
		return false;
	}
}


////////--------------------------------------------------------------------------------------------------------------------------------------int main() {


int main(){

	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);

	if (DxLib_Init() == -1) {
		return -1;
	}


	Card fire("fire", 2, 3, 2);
	Card goblin("goblkin", 1, 2, 1);
	Card knight("knight", 3, 3, 5);

	std::vector<Card>hand;
	std::vector<Card>fieldCards;

	hand.push_back(fire);
	hand.push_back(goblin);
	hand.push_back(knight);

	int currentSelectCard = -1;
	int pastSelectCard = -1;
	bool isDragging = false;	
	int dragX = 0;
	int dragY = 0;
	bool currentClick = false;
	bool pastClick = false;

	while (ProcessMessage() == 0) {
		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			break;
		}

		ClearDrawScreen();

		//変数の宣言    
		GetMousePoint(&Input::mouseX, &Input::mouseY);
		int mouseInput = GetMouseInput();
		bool mouseOnCard = false;
		int mouseColor;
		int handWidth = (hand.size() - 1) * CARD_WIDTH + CARD_INTERVAL;
		int handX = (SCREEN_WIDTH - handWidth) / 2;
		int FieldCardX = FIELD_X + 10;

		GameObserver field;
		field.DrawField();

		//左クリックしていない
		if (!(mouseInput & MOUSE_INPUT_LEFT)) {
			currentSelectCard = -1;
			isDragging = false;
			currentClick = false;
		}

		//カードの描画
		for (int i = 0; i < hand.size(); i++) {
			int x = handX + i * CARD_INTERVAL;
			int y = HAND_Y;

			//カードの上にマウスポインタがあるかの判別
			if (hand[i].HandDetection(x, y)&&isDragging==false) {
				mouseOnCard = true;
				//左クリック
				if (mouseInput & MOUSE_INPUT_LEFT) {
					isDragging = true;
					currentSelectCard = i;
					currentClick = true;
				}
			}
			//カードがマウスについてくる
			if (isDragging && currentSelectCard == i) {
				x = Input::mouseX - CARD_WIDTH / 2;
				y = Input::mouseY - CARD_HEIGHT / 2;
				dragX = x;
				dragY = y;
			}
			//ドラッグしていないカードの描画
			if (!(isDragging && currentSelectCard == i)) {
				hand[i].DrawCard(x, y);
			}
		}
		//ドラッグ中のカードの描画
		if (isDragging && currentSelectCard != -1) {
			hand[currentSelectCard].DrawCard(dragX, dragY);
		}
		
		//クリックを離した
		if (currentClick == false && pastClick == true) {
			if (field.FieldDetection() && pastSelectCard != -1) {
				fieldCards.push_back(hand[pastSelectCard]);
				hand.erase(hand.begin() + pastSelectCard);
				hand[pastSelectCard].DrawCard(FieldCardX, FIELD_CARD_Y);
			}
		}

		//フィールドガードの描画
		for (int i = 0; i < fieldCards.size(); i++) {
			int x = FIELD_X + i * CARD_INTERVAL;
			int y = FIELD_CARD_Y;

			if (fieldCards[i].FieldCardDetection(x, y)) {

			}
		}

		pastClick = currentClick;
		pastSelectCard = currentSelectCard;

		//マウスポインタの描画
		if (mouseOnCard || isDragging) { 
			mouseColor = GetColor(0, 0, 0);
		}
		else {
			mouseColor = GetColor(255, 255, 255);
		}
		DrawLine(Input::mouseX - 5, Input::mouseY, Input::mouseX + 5, Input::mouseY, mouseColor);
		DrawLine(Input::mouseX, Input::mouseY - 5, Input::mouseX, Input::mouseY + 5, mouseColor);

		ScreenFlip();
	}

  DxLib_End();

  return 0; 
}