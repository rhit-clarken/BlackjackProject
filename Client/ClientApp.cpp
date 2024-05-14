#define NOMINMAX
#include <SFML/Graphics.hpp>

int run_Window() {
	sf::RenderWindow window(sf::VideoMode(1000, 1000), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);
	sf::Texture texture;
	if (!texture.create(640, 480))
		return -1;

	sf::Sprite sprite(texture);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		// update the texture
		//sf::Uint8* pixels = ...; // get a fresh chunk of pixels (the next frame of a movie, for example)
		//texture.update(pixels);

		// draw it
		window.draw(sprite);
		window.draw(shape);

		window.display();
	}

	return 0;
}