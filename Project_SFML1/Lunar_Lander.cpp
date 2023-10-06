#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> // 오디오 헤더 파일 추가
#include <windows.h>
#include <string>
#include <vector>
#include <iostream>

using namespace sf;
using namespace std;

class LunarLander {
private:
	double x, y; // 현재 위치
	double velocity; // 속도
	double fuel; // 연료
	string status; // 현재 상태
	Texture t1, t2; // 텍스처 객체
	Sprite spaceship, burst; // 스프라이트 객체
	Font font; // 폰트 객체
	Text text; // 텍스트 객체
public:
	// 분출, 충돌, 이동시 효과음을 위한 SoundBuffer와 Sound클래스 사용
	SoundBuffer thrustBuffer;
	Sound thrustSound;
	SoundBuffer explosionBuffer;
	Sound explosionSound;
	SoundBuffer fallingBuffer;
	Sound fallingSound;
	LunarLander(double h, double v, double f); // 생성자
	bool checkLanded(); // 착륙 여부를 확인하기 위한 bool 자료형 플래그 함수
	bool checkExplosion(); // 충돌 여부를 확인하기 위한 bool 자료형 플래그 함수
	void update(double rate);
	void draw(RenderWindow& window);
};
// 생성자 함수
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
		cout << "폰트 파일을 오픈할 수 없음!" << endl;
	}
	text.setFont(font);

	// SFML에서 제공하는 loadFromFile()을 사용해 음성 파일을 로드, 플레이 못할 시 오류문 출력
// 사운드 객체에 setBuffer()로 버퍼 연결
	if (!thrustBuffer.loadFromFile("Thrust.wav")) {
		cout << "분출 효과음 파일을 오픈할 수 없음!" << endl;
	}
	thrustSound.setBuffer(thrustBuffer);

	if (!explosionBuffer.loadFromFile("explosion.wav")) {
		cout << "충돌 효과음 파일을 오픈할 수 없음!" << endl;
	}
	explosionSound.setBuffer(explosionBuffer);

	if (!fallingBuffer.loadFromFile("falling.wav")) {
		cout << "이동 효과음 파일을 오픈할 수 없음!" << endl;
	}
	fallingSound.setBuffer(fallingBuffer);
}

// spaceship이 착륙했는지를 검사하는 bool 자료형 함수. 만약 높이가 0보다 작으면 착륙한 것
bool LunarLander::checkLanded() {
	if (y + 50 >= 450) { // spaceship이 바닥에 닿으면 true값 반환  
		return true;
	}
	return false;
}

// spaceship 충돌 여부를 검사하는 bool 자료형 함수, spaceship이 화면 상단에 닿으면 충돌
bool LunarLander::checkExplosion() {
	if (y + 50 >= -50 && y <= 10) {
		return true; // 충돌
	}
	return false; // 충돌하지 않음
}

// 게임 상태를 업데이트
void LunarLander::update(double amount) {
	if (fuel <= 0) {
		fuel = 0;
		amount = 0;
	}
	fuel = fuel - amount;
	velocity = velocity - amount + 0.8;
	y = y + velocity;
	if (y > 420) y = 420; // spaceship이 화면 하단에 닿으면 정지하도록 숫자 수정
	spaceship.setPosition(x, y);
	burst.setPosition(x + 20, y + 50);
	status = "Press up key!\nheight: " + to_string(y) + "\nspeed: " + to_string(velocity) + "\nfuel: " + to_string(fuel);
	if (checkLanded()) { // 착륙에 성공하면 status를 Success로 변경하여 출력
		status = "Success!";
	}
	else if (checkExplosion()) {
		status = "Game Over!"; // 충돌하면 status를 Game Over로 변경하여 출력

	}
}

// 화면에 착륙선과 불꽃, 현재 상태를 그린다.
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

	bool explosionOccurred = false; // 충돌이 일어났는지 확인하는 변수

	LunarLander lander(300.0, 1.0, 100.0); // 착륙선 객체 생성
	while (window.isOpen()) {
		Event e;
		while (window.pollEvent(e)) {
			if (e.type == Event::Closed)
				window.close();
		}
		// 충돌이 있고 충돌이 현재 충돌이 일어나지 않은 상태였다면
		if (lander.checkExplosion() && !explosionOccurred) {
			explosionOccurred = true; // true로 변경
			lander.explosionSound.play(); // 충돌 사운드 play
		}
		if (!explosionOccurred) { // 충돌이 일어나지 않은 상태라면
			// 키보드 이벤트가 발생하고, 그 이벤트가 위 방향키라면
			if (e.type == Event::KeyPressed && e.key.code == Keyboard::Up) {
				lander.update(3.0);
				lander.thrustSound.play(); // 분출 사운드 play
			}
			else { // 이 외에 상황이라면
				lander.update(0.0);
				lander.fallingSound.play(); // 떨어지는 sound play
				if (lander.checkLanded()) { // 떨어지다가 spaceship이 착륙했다면
					lander.fallingSound.stop(); // 떨어지는 sound stop
				}
			}
		}
		window.clear();
		window.draw(background);
		lander.draw(window);
		window.display();
		Sleep(100); // 0.1초 동안 잠재운다.
	}

	return 0;
}
