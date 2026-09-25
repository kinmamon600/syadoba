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
//ターン終了ボタン
const int TURN_BUTTON_X = 1200;
const int TURN_BUTTON_Y = 370;
const int TURN_BUTTON_RADIUS = 60;

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
	bool m_hasAttack;      //すでに攻撃済みか
public:
	Card(std::string _name = "null", int _cost = 0, int _attack = 0, int _hp = 0, bool _hasAttack = false);
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
	//攻撃済みか確認
	bool HasAttacked();
	//攻撃フラグ書き換え
	void SetHasAttack(bool value);
};

Card::Card(std::string _name, int _cost, int _attack, int _hp, bool _hasAttack) {
	m_name = _name;
	m_cost = _cost;
	m_attack = _attack;
	m_hp = _hp;
	m_hasAttack = _hasAttack;
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

bool Card::HasAttacked() {
	return m_hasAttack;
}

void Card::SetHasAttack(bool value) {
	m_hasAttack = value;
}



class Field {
public:
	//フィールドの描画
	void DrawField();
	//フィールドの当たり判定
	bool FieldDetection();
};

void Field::DrawField() {
	DrawBox(FIELD_X, FIELD_Y, FIELD_X + FIELD_WIDTH, FIELD_Y + FIELD_HEIGHT, GetColor(100, 100, 100), false);
}

bool Field::FieldDetection() {
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

	int selectHandCard = -1;         //現在手札で選択しているカード
	int pastSelectHandCard = -1;     //１Ｆ前手札で選択していたカード

	int selectFieldCard = -1;        //フィールドで選択されているカード

	int selectEnemyCard = -1;        //相手フィールドで選択されているカード

	bool isDragging = false;         //ドラッグしているか判別
	int dragX = 0;                   //マウス座標Ｘ
	int dragY = 0;                   //マウス座標Ｙ
	
	bool currentClick = false;       //現在クリックしているか
	bool pastClick = false;          //１Ｆ前にクリックしていたか

	bool playerTurn = true;         //現在のターン


	while (ProcessMessage() == 0) {
		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			break;
		}

		ClearDrawScreen();

		//変数の宣言    
		GetMousePoint(&Input::mouseX, &Input::mouseY);
		int mouseInput = GetMouseInput();
		
		bool mouseOnCard = false;                                                       //マウスカーソル描画のため
		int mouseColor;                                                                 //マウスカラー

		int handWidth = (hand.size() - 1) * CARD_WIDTH + CARD_INTERVAL;	                //ハンド描画の横幅
		int handX = (SCREEN_WIDTH - handWidth) / 2;						                //ハンドの開始位置

		int fieldCardWidth = (playerField.size() - 1) * CARD_WIDTH + CARD_INTERVAL;		//フィールドカード描画の横幅
		int fieldCardX = (SCREEN_WIDTH - fieldCardWidth) / 2;							//フィールドカードの開始位置

		int enemyCardWidth = (enemyField.size() - 1) * CARD_WIDTH + CARD_INTERVAL;		//相手フィールドカード描画の横幅
		int enemyCardX = (SCREEN_WIDTH - enemyCardWidth) / 2;							//相手フィールドカードの開始位置

		bool clickedFieldCard = false;													//今のフレームでフィールドカードをクリックしたか

		bool hasAttack = false;															//今から攻撃するかどうか

		Field field;
		field.DrawField();

		//左クリックしていない
		if (!(mouseInput & MOUSE_INPUT_LEFT)) {
			selectHandCard = -1;
			isDragging = false;
			currentClick = false;
		}
		
		//ターン終了ボタンの描画と判定
		DrawCircle(TURN_BUTTON_X, TURN_BUTTON_Y, TURN_BUTTON_RADIUS, GetColor(100, 100, 100), true);

		int dx = Input::mouseX - TURN_BUTTON_X;
		int dy = Input::mouseY - TURN_BUTTON_Y;

		if (dx * dx + dy * dy <= TURN_BUTTON_RADIUS * TURN_BUTTON_RADIUS) {
			if ((mouseInput & MOUSE_INPUT_LEFT)) {
				playerTurn = false;
			}
		}

		//プレイヤーの行動---------------------------------------------------------
		if (playerTurn) {

			//ハンドの行動
			for (int i = 0; i < hand.size(); i++) {
				int x = handX + i * CARD_INTERVAL;
				int y = HAND_Y;

				//ハンドの上にマウスポインタがあるかの判別
				if (hand[i].HitDetection(x, y) && !isDragging) {
					mouseOnCard = true;
					//左クリック
					if (mouseInput & MOUSE_INPUT_LEFT) {
						isDragging = true;
						selectHandCard = i;
						currentClick = true;
					}
				}

				// 選択中のカードならドラッグ位置を更新
				if (playerTurn && isDragging && selectHandCard == i)
				{
					dragX = Input::mouseX - CARD_WIDTH / 2;
					dragY = Input::mouseY - CARD_HEIGHT / 2;
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

			//フィールドガードの行動
			for (int i = 0; i < playerField.size(); i++) {
				int x = fieldCardX + i * CARD_INTERVAL;
				int y = FIELD_CARD_Y;

				//カードの上にマウスポインタがあるかの判別
				if (playerField[i].HitDetection(x, y) && !isDragging) {
					mouseOnCard = true;
					//左クリック
					if (mouseInput & MOUSE_INPUT_LEFT) {
						currentClick = true;
						clickedFieldCard = true;
						selectFieldCard = i;
					}
				}
			}

			//相手のフィールドガードの行動
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
							hasAttack = true;
						}
					}
				}
			}

			//攻撃処理
			if (hasAttack && !playerField[selectFieldCard].HasAttacked() &&selectFieldCard != -1 && selectEnemyCard != -1) {
				enemyField[selectEnemyCard].TakeDamage(playerField[selectFieldCard].GetAttack());
				playerField[selectFieldCard].SetHasAttack(true);

				//体力０の相手カードを削除
				if (enemyField[selectEnemyCard].GetHp() == 0) {
					enemyField.erase(enemyField.begin() + selectEnemyCard);
				}
				selectEnemyCard = -1;
			}
		}


		//相手の行動---------------------------------------------------------
		else {
			playerTurn = true;
		}


		//何もないところをクリックすると選択が解除される
		if (mouseInput & MOUSE_INPUT_LEFT) {
			if (!clickedFieldCard) {
				selectFieldCard = -1;
				selectEnemyCard = -1;
			}
		}

		//カードの描画--------------------------------------------------------

		//ハンド
		for (int i = 0; i < hand.size(); i++) {
			int x = handX + i * CARD_INTERVAL;
			int y = HAND_Y;

			//選択中のハンドがマウスについてくる
			if (isDragging && selectHandCard == i) {
				x = dragX;
				y = dragY;
			}

			//ドラッグしていないハンドの描画
			if (!(isDragging && selectHandCard == i)) {
				hand[i].DrawCard(x, y);
			}
		}

		//フィールドガード
		for (int i = 0; i < playerField.size(); i++) {
			int x = fieldCardX + i * CARD_INTERVAL;
			int y = FIELD_CARD_Y;

			if (selectFieldCard == i) {
				y -= 20;
				if (playerField[i].HitDetection(x, y) && !isDragging) {
					mouseOnCard = true;
				}
			}

			playerField[i].DrawCard(x, y);
		}

		//相手のフィールドガード
		for (int i = 0; i < enemyField.size(); i++) {
			int x = enemyCardX + i * CARD_INTERVAL;
			int y = ENEMY_CARD_Y;

			enemyField[i].DrawCard(x, y);
		}

		//ドラッグ中のカード
		if (isDragging && selectHandCard != -1) {
			hand[selectHandCard].DrawCard(dragX, dragY);
		}
		if (isDragging && selectFieldCard != -1) {
			playerField[selectFieldCard].DrawCard(dragX, dragY);
		}

		//描画終わり---------------------------------------------------------------------

		pastClick = currentClick;
		pastSelectHandCard = selectHandCard;

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