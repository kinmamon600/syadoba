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
const int HAND_Y = 570;
//フィールドカード
const int FIELD_CARD_Y = 410;
//相手のフィールドカード
const int ENEMY_CARD_Y = 190;
//フィールド
const int FIELD_X = 250;
const int FIELD_Y = 190;
const int FIELD_WIDTH = 780;
const int FIELD_HEIGHT = 360;

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
	//カードの当たり判定
	bool HitDetection(int x, int y);
	//攻撃力を取得
	int GetAttack();
	//体力を取得
	int GetHp();
	//ＨＰを減らす
	void TakeDamage(int damage);
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
	DrawString(x + 20, y + 30, m_name.c_str(), GetColor(0, 0, 0));
	DrawString(x + 10, y + 20, costText.c_str(), GetColor(66, 214, 107));
	DrawString(x + 20, y + 120, attackText.c_str(), GetColor(74, 168, 255));
	DrawString(x + 80, y + 120, hpText.c_str(), GetColor(255, 85, 85));
}

bool Card::HitDetection(int x, int y) {
	if (Input::mouseX >= x && Input::mouseY >= y && Input::mouseX <= x + CARD_WIDTH && Input::mouseY <= y + CARD_HEIGHT) { return true; }
	else { return false; }
}

int Card::GetAttack() {
	return m_attack;
}

int Card::GetHp() {
	return m_hp;
}

void Card::TakeDamage(int damage) {
	m_hp -= damage;

	if (m_hp < 0) {
		m_hp = 0;
	}
}




class GameObserver {
public:
	//フィールドの描画
	void DrawField();
	//フィールドの当たり判定
	bool FieldDetection();
};

void GameObserver::DrawField() {
	DrawBox(FIELD_X, FIELD_Y, FIELD_X + FIELD_WIDTH, FIELD_Y + FIELD_HEIGHT, GetColor(100, 100, 100), false);
}

bool GameObserver::FieldDetection() {
	if (Input::mouseX >= FIELD_X && Input::mouseY >= FIELD_Y && Input::mouseX <= FIELD_X + FIELD_WIDTH && Input::mouseY <= FIELD_Y + FIELD_HEIGHT) {
		return true;
	}
	else {
		return false;
	}
}


////////--------------------------------------------------------------------------------------------------------------------------------------


int main() {

	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);

	if (DxLib_Init() == -1) {
		return -1;
	}


	Card slime("slime", 2, 3, 2);
	Card goblin("goblkin", 1, 2, 1);
	Card skeleton("skeleton", 3, 3, 5);

	std::vector<Card>hand;
	std::vector<Card>playerField;
	std::vector<Card>enemyField;

	hand.push_back(slime);
	hand.push_back(goblin);
	hand.push_back(skeleton);

	enemyField.push_back(slime);
	enemyField.push_back(goblin);
	enemyField.push_back(skeleton);

	//手札で選択しているカード
	int selectHandCard = -1;
	int pastSelectHandCard = -1;

	//フィールドで選択されているカード
	int selectFieldCard = -1;

	//相手フィールドで選択されているカード
	int selectEnemyCard = -1;

	bool isDragging = false;
	int dragX = 0;
	int dragY = 0;

	//クリックしているか確かめるだけ
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
		//マウスカーソル描画のため
		bool mouseOnCard = false;
		int mouseColor;

		int handWidth = (hand.size() - 1) * CARD_WIDTH + CARD_INTERVAL;
		int handX = (SCREEN_WIDTH - handWidth) / 2;

		int fieldCardWidth = (playerField.size() - 1) * CARD_WIDTH + CARD_INTERVAL;
		int fieldCardX = (SCREEN_WIDTH - fieldCardWidth) / 2;

		int enemyCardWidth = (enemyField.size() - 1) * CARD_WIDTH + CARD_INTERVAL;
		int enemyCardX = (SCREEN_WIDTH - enemyCardWidth) / 2;

		bool clickOnFieldCard = false;

		bool canAttack = false;

		GameObserver field;
		field.DrawField();

		//左クリックしていない
		if (!(mouseInput & MOUSE_INPUT_LEFT)) {
			selectHandCard = -1;
			isDragging = false;
			currentClick = false;
		}
		

		//カードの描画
		for (int i = 0; i < hand.size(); i++) {
			int x = handX + i * CARD_INTERVAL;
			int y = HAND_Y;

			//カードの上にマウスポインタがあるかの判別
			if (hand[i].HitDetection(x, y) && !isDragging) {
				mouseOnCard = true;
				//左クリック
				if (mouseInput & MOUSE_INPUT_LEFT) {
					isDragging = true;
					selectHandCard = i;
					currentClick = true;
				}
			}
			//カードがマウスについてくる
			if (isDragging && selectHandCard == i) {
				x = Input::mouseX - CARD_WIDTH / 2;
				y = Input::mouseY - CARD_HEIGHT / 2;
				dragX = x;
				dragY = y;
			}
			//ドラッグしていないカードの描画
			if (!(isDragging && selectHandCard == i)) {
				hand[i].DrawCard(x, y);
			}
		}

		//クリックを離した
		if (!currentClick && pastClick) {
			//フィールド上で
			if (field.FieldDetection() && pastSelectHandCard != -1) {
				playerField.push_back(hand[pastSelectHandCard]);
				hand.erase(hand.begin() + pastSelectHandCard);
			}
		}

		//フィールドガードの描画
		for (int i = 0; i < playerField.size(); i++) {
			int x = fieldCardX + i * CARD_INTERVAL;
			int y = FIELD_CARD_Y;

			//カードの上にマウスポインタがあるかの判別
			if (playerField[i].HitDetection(x, y) && !isDragging) {
				mouseOnCard = true;
				//左クリック
				if (mouseInput & MOUSE_INPUT_LEFT) {
					selectFieldCard = i;
					currentClick = true;
					clickOnFieldCard = true;
				}
			}
			if (selectFieldCard == i) {
				y -= 20;
			}

			//体力０のカードを削除
			if (playerField[i].GetHp() == 0) {
				playerField.erase(playerField.begin() + i);
			}

			//ドラッグしていないカードの描画
			if (!(isDragging && selectFieldCard == i)) {
				playerField[i].DrawCard(x, y);
			}
		}


		//相手のフィールドガードの描画
		for (int i = 0; i < enemyField.size(); i++) {
			int x = enemyCardX + i * CARD_INTERVAL;
			int y = ENEMY_CARD_Y;

			//カードの上にマウスポインタがあるかの判別
			if (enemyField[i].HitDetection(x, y) && !isDragging) {
				mouseOnCard = true;
				//左クリック
				if (mouseInput & MOUSE_INPUT_LEFT) {
					if (selectFieldCard != -1) {
						selectEnemyCard = i;
						canAttack = true;
					}
				}
			}


			enemyField[i].DrawCard(x, y);

		}

		if (canAttack) {
			enemyField[selectEnemyCard].TakeDamage(playerField[selectFieldCard].GetAttack());

			//体力０の相手カードを削除
			if (enemyField[selectEnemyCard].GetHp() == 0) {
				enemyField.erase(enemyField.begin() + selectEnemyCard);
				selectEnemyCard = -1;
			}
		}

		if (mouseInput & MOUSE_INPUT_LEFT) {
			if (!clickOnFieldCard) {
				selectFieldCard = -1;
				selectEnemyCard = -1;
			}
		}

		//ドラッグ中のカードの描画
		if (isDragging && selectHandCard != -1) {
			hand[selectHandCard].DrawCard(dragX, dragY);
		}
		if (isDragging && selectFieldCard != -1) {
			playerField[selectFieldCard].DrawCard(dragX, dragY);
		}

		pastClick = currentClick;
		pastSelectHandCard = selectHandCard;
		//pastSelectFieldCard = currentSelectFieldCard;

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