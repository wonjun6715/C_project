#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> // ����� ��� ���� �߰�
#include <windows.h>
#include <string>
#include <vector>
#include <iostream>

using namespace sf;
using namespace std;

class LunarLander {
private:
	double x, y; // ���� ��ġ
	double velocity; // �ӵ�
	double fuel; // ����
	string status; // ���� ����
	Texture t1, t2; // �ؽ�ó ��ü
	Sprite spaceship, burst; // ��������Ʈ ��ü
	Font font; // ��Ʈ ��ü
	Text text; // �ؽ�Ʈ ��ü
public:
	// ����, �浹, �̵��� ȿ������ ���� SoundBuffer�� SoundŬ���� ���
	SoundBuffer thrustBuffer;
	Sound thrustSound;
	SoundBuffer explosionBuffer;
	Sound explosionSound;
	SoundBuffer fallingBuffer;
	Sound fallingSound;
	LunarLander(double h, double v, double f); // ������
	bool checkLanded(); // ���� ���θ� Ȯ���ϱ� ���� bool �ڷ��� �÷��� �Լ�
	bool checkExplosion(); // �浹 ���θ� Ȯ���ϱ� ���� bool �ڷ��� �÷��� �Լ�
	void update(double rate);
	void draw(RenderWindow& window);
};
// ������ �Լ�
LunarLander::LunarLander(double h, double v, double f) {
	x = 300;
	y = h;
	velocity = v;
	fuel = f;
	t1.loadFromFile("images/spaceship.png");
	t2.loadFromFile("images/burst.png");
	spaceship.setTexture(t1);
	burst.setTexture(t2);
	spaceship.setPosition(x, y);
	burst.setPosition(x + 20, y + 50);
	if (!font.loadFromFile("OpenSans-Bold.ttf")) {
		cout << "��Ʈ ������ ������ �� ����!" << endl;
	}
	text.setFont(font);

	// SFML���� �����ϴ� loadFromFile()�� ����� ���� ������ �ε�, �÷��� ���� �� ������ ���
// ���� ��ü�� setBuffer()�� ���� ����
	if (!thrustBuffer.loadFromFile("Thrust.wav")) {
		cout << "���� ȿ���� ������ ������ �� ����!" << endl;
	}
	thrustSound.setBuffer(thrustBuffer);

	if (!explosionBuffer.loadFromFile("explosion.wav")) {
		cout << "�浹 ȿ���� ������ ������ �� ����!" << endl;
	}
	explosionSound.setBuffer(explosionBuffer);

	if (!fallingBuffer.loadFromFile("falling.wav")) {
		cout << "�̵� ȿ���� ������ ������ �� ����!" << endl;
	}
	fallingSound.setBuffer(fallingBuffer);
}

// spaceship�� �����ߴ����� �˻��ϴ� bool �ڷ��� �Լ�. ���� ���̰� 0���� ������ ������ ��
bool LunarLander::checkLanded() {
	if (y + 50 >= 450) { // spaceship�� �ٴڿ� ������ true�� ��ȯ  
		return true;
	}
	return false;
}

// spaceship �浹 ���θ� �˻��ϴ� bool �ڷ��� �Լ�, spaceship�� ȭ�� ��ܿ� ������ �浹
bool LunarLander::checkExplosion() {
	if (y + 50 >= -50 && y <= 10) {
		return true; // �浹
	}
	return false; // �浹���� ����
}

// ���� ���¸� ������Ʈ
void LunarLander::update(double amount) {
	if (fuel <= 0) {
		fuel = 0;
		amount = 0;
	}
	fuel = fuel - amount;
	velocity = velocity - amount + 0.8;
	y = y + velocity;
	if (y > 420) y = 420; // spaceship�� ȭ�� �ϴܿ� ������ �����ϵ��� ���� ����
	spaceship.setPosition(x, y);
	burst.setPosition(x + 20, y + 50);
	status = "Press up key!\nheight: " + to_string(y) + "\nspeed: " + to_string(velocity) + "\nfuel: " + to_string(fuel);
	if (checkLanded()) { // ������ �����ϸ� status�� Success�� �����Ͽ� ���
		status = "Success!";
	}
	else if (checkExplosion()) {
		status = "Game Over!"; // �浹�ϸ� status�� Game Over�� �����Ͽ� ���

	}
}

// ȭ�鿡 �������� �Ҳ�, ���� ���¸� �׸���.
void LunarLander::draw(RenderWindow& window) {
	window.draw(spaceship);
	window.draw(burst);
	text.setString(status);
	text.setCharacterSize(20);
	text.setPosition(10, 100);
	window.draw(text);
}

int main() {
	RenderWindow window(VideoMode(600, 480), "LUNAR LANDER");
	window.setFramerateLimit(60);

	Texture t;
	Sprite background;
	t.loadFromFile("images/background.png");
	background.setTexture(t);

	bool explosionOccurred = false; // �浹�� �Ͼ���� Ȯ���ϴ� ����

	LunarLander lander(300.0, 1.0, 100.0); // ������ ��ü ����
	while (window.isOpen()) {
		Event e;
		while (window.pollEvent(e)) {
			if (e.type == Event::Closed)
				window.close();
		}
		// �浹�� �ְ� �浹�� ���� �浹�� �Ͼ�� ���� ���¿��ٸ�
		if (lander.checkExplosion() && !explosionOccurred) {
			explosionOccurred = true; // true�� ����
			lander.explosionSound.play(); // �浹 ���� play
		}
		if (!explosionOccurred) { // �浹�� �Ͼ�� ���� ���¶��
			// Ű���� �̺�Ʈ�� �߻��ϰ�, �� �̺�Ʈ�� �� ����Ű���
			if (e.type == Event::KeyPressed && e.key.code == Keyboard::Up) {
				lander.update(3.0);
				lander.thrustSound.play(); // ���� ���� play
			}
			else { // �� �ܿ� ��Ȳ�̶��
				lander.update(0.0);
				lander.fallingSound.play(); // �������� sound play
				if (lander.checkLanded()) { // �������ٰ� spaceship�� �����ߴٸ�
					lander.fallingSound.stop(); // �������� sound stop
				}
			}
		}
		window.clear();
		window.draw(background);
		lander.draw(window);
		window.display();
		Sleep(100); // 0.1�� ���� ������.
	}

	return 0;
}
