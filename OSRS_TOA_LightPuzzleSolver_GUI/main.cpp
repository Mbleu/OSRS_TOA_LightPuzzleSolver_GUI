#include <SFML/Graphics.hpp>

unsigned char solve(unsigned char start_lights)	// Return pressure plate press pattern, for a given starting light pattern
{
	static constexpr int n{ 8 };									// number of pressure plates
	static constexpr unsigned int combos{ 1 << n };					// number of pressure plate press combinations
	static constexpr unsigned char goal{ 0b11111111 };

	for (int i{ 0 }; i < combos; ++i)								// for each press combo
	{
		unsigned char lights{ unsigned char(start_lights) };

		for (unsigned char j{ 0 }; j < n; ++j)						// for each plate
			if (i & (1 << j))
				lights ^= 0b10000011 << j | 0b10000011 >> (n - j);	// if plate flagged in combo, toggle adjacent lights

		if (!(lights ^ goal)) { return i; }							// if press combo yields success, return solution
	}
}

int main()
{
	constexpr int tile_size{ 108 };
	constexpr int grid_size{ 7 };
	const sf::Vector2i window_size{ grid_size * tile_size, grid_size * tile_size };

	sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "Tombs of Amascut, Light Puzzle Solver");

	sf::Texture txtr_blank;
	sf::Texture txtr_off;
	sf::Texture txtr_on;
	sf::Texture txtr_button;
	sf::Texture txtr_solve;
	sf::Texture txtr_reset;
	sf::Texture txtr_click;

	txtr_blank.loadFromFile("..\\textures.png", sf::IntRect(0, 0, tile_size, tile_size));
	txtr_off.loadFromFile("..\\textures.png", sf::IntRect(tile_size, 0, tile_size, tile_size));
	txtr_on.loadFromFile("..\\textures.png", sf::IntRect(2 * tile_size, 0, tile_size, tile_size));
	txtr_button.loadFromFile("..\\textures.png", sf::IntRect(0, tile_size, tile_size, tile_size / 2));
	txtr_solve.loadFromFile("..\\textures.png", sf::IntRect(tile_size, tile_size, 32, 12));
	txtr_reset.loadFromFile("..\\textures.png", sf::IntRect(tile_size, 120, 32, 12));
	txtr_click.loadFromFile("..\\textures.png", sf::IntRect(tile_size, 132, 14, 14));

	txtr_blank.setRepeated(true);

	sf::RectangleShape background(sf::Vector2f(grid_size * tile_size, grid_size * tile_size));
	background.setTexture(&txtr_blank);
	background.setTextureRect(sf::IntRect(0, 0, grid_size * tile_size, grid_size * tile_size));

	sf::RectangleShape plate[8];
	for (sf::RectangleShape& p : plate) {
		p.setSize(sf::Vector2f(tile_size,tile_size));
		p.setTexture(&txtr_off);
	}
	plate[0].setPosition(tile_size * 1, tile_size * 1);
	plate[1].setPosition(tile_size * 3, tile_size * 1);
	plate[2].setPosition(tile_size * 5, tile_size * 1);
	plate[3].setPosition(tile_size * 5, tile_size * 3);
	plate[4].setPosition(tile_size * 5, tile_size * 5);
	plate[5].setPosition(tile_size * 3, tile_size * 5);
	plate[6].setPosition(tile_size * 1, tile_size * 5);
	plate[7].setPosition(tile_size * 1, tile_size * 3);

	sf::RectangleShape button(sf::Vector2f(tile_size * 2, tile_size));
	button.setTexture(&txtr_button);
	button.setPosition(background.getSize().x - tile_size - (button.getSize().x / 2), background.getSize().y - (tile_size / 2) - (button.getSize().y / 2));

	sf::RectangleShape text(sf::Vector2f(32 * 4, 12 * 4));
	text.setTexture(&txtr_solve);
	text.setPosition(button.getPosition().x + button.getSize().x / 2 - text.getSize().x / 2, button.getPosition().y + button.getSize().y / 2 - text.getSize().y / 2);

	std::vector<sf::RectangleShape> click;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
			{
				sf::View view({ 0, 0, grid_size * tile_size , grid_size * tile_size });

				if (event.size.width > event.size.height) {
					float aspect_ratio = float(event.size.height) / float(event.size.width);
					view.setViewport({ (1 - aspect_ratio) / 2, 0, aspect_ratio, 1 });
					window.setView(view);
				}
				else {
					float aspect_ratio = float(event.size.width) / float(event.size.height);
					view.setViewport({ 0, (1 - aspect_ratio) / 2, 1, aspect_ratio });
					window.setView(view);
				}
				break;
			}
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left) {
					sf::Vector2f mousePos{ sf::Mouse::getPosition(window) };
					mousePos = window.mapPixelToCoords(sf::Vector2i(mousePos));
					if (text.getTexture() == &txtr_solve) {
						for (sf::RectangleShape& t : plate) {
							if (t.getGlobalBounds().contains(sf::Vector2f(mousePos)))
								t.getTexture() == &txtr_on ? t.setTexture(&txtr_off) : t.setTexture(&txtr_on);
						}
					}
					if (button.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						if (text.getTexture() == &txtr_solve) {
							text.setTexture(&txtr_reset);
							unsigned char lights{ 0 };
							for (int i{ 0 }; i < std::size(plate); ++i)
								if (plate[i].getTexture() == &txtr_on) lights |= 1 << i;

							unsigned char presses{ solve(lights) };

							for (int i{ 0 }; i < std::size(plate); ++i)
								if (presses & (1 << i)) {
									click.push_back(sf::RectangleShape{ {14 * 5, 14 * 5} });
									click[click.size() - 1].setTexture(&txtr_click);
									click[click.size() - 1].setPosition(plate[i].getGlobalBounds().left + (plate[i].getGlobalBounds().width / 2) - (click[click.size() - 1].getSize().x / 2), plate[i].getGlobalBounds().top + (plate[i].getGlobalBounds().height / 2) - (click[click.size() - 1].getSize().y / 2));
								}
						}
						else {
							text.setTexture(&txtr_solve);
							click.clear();
							for (sf::RectangleShape& p : plate)
								p.setTexture(&txtr_off);
						}
					}
				}
				break;
			}
		}

		window.clear();
		window.draw(background);
		for (const sf::RectangleShape& p : plate) window.draw(p);
		window.draw(button);
		window.draw(text);
		for (const sf::RectangleShape& c : click) window.draw(c);
		window.display();
	}

	return 0;
}