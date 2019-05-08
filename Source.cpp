#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>

const double PI = 3.141592653589793238463;

class Bird : public sf::Drawable
{
private:
	sf::CircleShape m_bird;
	float m_velocity = 500;
	sf::Time m_jumptime = sf::milliseconds(1);
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(m_bird);
	}
public:
	void update(sf::Time t_elapsed) {
		if (m_jumptime > sf::Time::Zero) {
			m_jumptime -= t_elapsed;
		}
		m_bird.setPosition(m_bird.getPosition().x, m_bird.getPosition().y + m_velocity * t_elapsed.asSeconds() * std::sin(PI / 2 - m_jumptime.asSeconds() * PI));
	}

	Bird() :
		m_bird(20, 20)
	{
		m_bird.setFillColor(sf::Color::Yellow);
		m_bird.setOrigin(20, 20);
		m_bird.setPosition(400, 300);
	}

	void jump()
	{
		if (m_jumptime.asSeconds() < 0.8) {
			m_jumptime = sf::seconds(1);
		}
	}

	~Bird()
	{
	}

	sf::Vector2f getPosition()
	{
		return m_bird.getPosition();
	}

	bool dead()
	{
		if (m_bird.getPosition().y < 0 || m_bird.getPosition().y>600) {
			return true;
		}
		return false;
	}
};

class Pipes : public sf::Drawable
{
private :
	std::vector<sf::RectangleShape> m_pipesup;
	std::vector<sf::RectangleShape> m_pipesdown;
	float m_traveleddistance;
	int m_score;
	float m_velocity = 200;
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		for (int i = 0; i < 3; ++i) {
			target.draw(m_pipesup[i]);
			target.draw(m_pipesdown[i]);
		}
	}
public:
	Pipes() :
		m_traveleddistance(0),
		m_score(0)
	{
		sf::RectangleShape temp;
		temp.setFillColor(sf::Color::Color(4,132,40));
		for (int i = 0; i < 3; ++i) {
			int random = rand() % 300 + 50;
			temp.setSize(sf::Vector2f(80, random));
			temp.setPosition(sf::Vector2f(800 + i * 380, 0));
			temp.setOrigin(0, 0);
			m_pipesup.push_back(temp);
			temp.setSize(sf::Vector2f(80, 600 - random - 200));
			temp.setOrigin(0, temp.getGlobalBounds().height);
			temp.setPosition(sf::Vector2f(800 + i * 380, 600));
			m_pipesdown.push_back(temp);
		}
	}

	~Pipes()
	{
	}

	void update(sf::Time t_elapsed)
	{
		m_traveleddistance += m_velocity * t_elapsed.asSeconds();
		m_score = ((int)m_traveleddistance - 100) / 380;
		for (int i = 0; i < 3; ++i) {
			m_pipesup[i].setPosition(m_pipesup[i].getPosition().x - m_velocity * t_elapsed.asSeconds(), m_pipesup[i].getPosition().y);
			m_pipesdown[i].setPosition(m_pipesdown[i].getPosition().x - m_velocity * t_elapsed.asSeconds(), m_pipesdown[i].getPosition().y);
		}
		if (m_pipesup.front().getPosition().x < -80) {
			m_pipesup.erase(m_pipesup.begin());
			m_pipesdown.erase(m_pipesdown.begin());
			int random = rand() % 300 + 50;
			sf::RectangleShape temp;
			temp.setSize(sf::Vector2f(80, random));
			temp.setFillColor(sf::Color::Color(4, 132, 40));
			temp.setPosition(sf::Vector2f(m_pipesup.back().getPosition().x + 380, 0));
			m_pipesup.push_back(temp);
			temp.setSize(sf::Vector2f(80, 600 - random - 200));
			temp.setOrigin(0, temp.getGlobalBounds().height);
			temp.setPosition(sf::Vector2f(m_pipesdown.back().getPosition().x + 380, 600));
			m_pipesdown.push_back(temp);
		}
	}

	bool hits(sf::Vector2f t_posion)
	{
		for (size_t i = 0; i < m_pipesup.size(); ++i) {
			if (m_pipesup[i].getPosition().x + 100 > t_posion.x) {
				if (t_posion.x >= m_pipesup[i].getPosition().x - 20 && t_posion.x <= m_pipesup[i].getPosition().x + 100) {
					if (t_posion.y <= m_pipesup[i].getGlobalBounds().height + 20 || t_posion.y >= 600 - m_pipesdown[i].getGlobalBounds().height - 20) {
						return true;
					}
				}
			}
		}
		return false;
	}

	double getDistanceToPipe(sf::Vector2f t_position)
	{
		for (size_t i = 0; i < m_pipesup.size(); ++i) {
			if (m_pipesup[i].getPosition().x + 100 > t_position.x) {
				return m_pipesup[i].getPosition().x + 100 - t_position.x;
			}
		}
	}

	double getPipeHeight(sf::Vector2f t_position)
	{
		for (size_t i = 0; i < m_pipesup.size(); ++i) {
			if (m_pipesup[i].getPosition().x + 100 > t_position.x) {
				return m_pipesup[i].getGlobalBounds().height;
			}
		}
	}

	float getTraveledDistance()
	{
		return m_traveleddistance;
	}

	int getScore()
	{
		return m_score;
	}
};

int main() {
	srand(time(NULL));
	sf::RenderWindow window(sf::VideoMode(800, 600), "FlappyShit");
	Bird bird;
	Pipes pipes;
	sf::Clock clock;
	bool started = false;
	bool jumped = false;
	std::fstream file;
	file.open("trainingdata.txt", std::ios::out);
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Space) {
					started = true;
					jumped = true;
					bird.jump();
				}
				if (event.key.code == sf::Keyboard::P) {
					started = !started;
				}

			}
		}
		sf::Time elapsed = clock.restart();
		if (started) {
			file << bird.getPosition().y << ';';
			file << pipes.getDistanceToPipe(bird.getPosition()) << ';';
			file << pipes.getPipeHeight(bird.getPosition()) << ';';
			if (jumped) {
				file << '1' << ';' << std::endl;
				jumped = false;
			}
			else {
				file << '0' << ';' << std::endl;
			}
			bird.update(elapsed);
			pipes.update(elapsed);
			if (pipes.hits(bird.getPosition()) || bird.dead()) {
				bird = Bird();
				pipes = Pipes();
				started = false;
				//window.close();
			}
		}
		sf::Font font;
		font.loadFromFile("C:/Windows/Fonts/Arial.ttf");
		sf::Text distance;
		distance.setFont(font);
		distance.setString(std::to_string(pipes.getTraveledDistance()));
		sf::Text score;
		score.setFont(font);
		score.setString(std::to_string(pipes.getScore()));
		score.setPosition(0, 50);
		window.clear(sf::Color::Color(0,191,255));
		window.draw(pipes);
		window.draw(bird);
		window.draw(distance);
		window.draw(score);
		window.display();
	}
		file.close();
	return 0;
}