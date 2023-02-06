#include "load_game.hpp"
#include <iostream>

Player load_game(std::string docName) {
	std::ifstream arquivo(docName);
	std::string card;
	Player player;

	


	if (!arquivo.is_open()) return player;

	arquivo >> player.playerName;
	arquivo >> player.score;
	arquivo >> player.matches;

	std::cout << "LOAD GAME" << std::endl;
	for (int i = 0; i < NUM_CARDS; i++) {
		arquivo >> player.cards[i].is_flipped;
		arquivo >> player.cards[i].id;
		arquivo >> player.cards[i].x;
		arquivo >> player.cards[i].y;
		//arquivo >> player.cards[i].front_image;
		arquivo >> player.cards[i].card_name; //aqui t� devolvendo o valor correto do arquivo
		std::cout << player.cards[i].card_name << std::endl;
		//player.cards[i].front_image = al_load_bitmap(card.c_str());

	}
	std::cout << " " << std::endl;

	arquivo.close();

	return player;
}