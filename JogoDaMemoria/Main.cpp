#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <thread>
#include <chrono>
#include "card.hpp"
#include "player.hpp"
#include "interface.hpp"
#include "draw_interface.hpp"
#include "get_player_name.hpp"
#include "get_file_name.hpp"
#include "scoreBoard.hpp"
#include "draw_card.hpp"
#include "shuffle.hpp"
#include "init_game_cards.hpp"
#include "init_game_interface.hpp"
#include "menu_inicial.hpp"
#include "memorize_card.hpp"
#include "constantes.hpp"
#include "difficulty.hpp"

int main(int argc, char** argv) {
	Card game_cards[NUM_CARDS];
	Player game_state;
	Interface interface[NUM_INTERF];

	int selected_card = -1;
	int last_card = -1;
	int num_matches = 0;

	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_EVENT_QUEUE* event_queue = NULL;
	ALLEGRO_TIMER* timer = NULL;
	//Player game_state;
	bool done = false;
	bool redraw = true;

	if (!al_init()) {
		al_show_native_message_box(display, "Error", "Could not initialize Allegro 5", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	al_install_mouse();
	al_init_image_addon();
	al_init_primitives_addon();
	al_install_keyboard();
	al_init_native_dialog_addon();
	al_init_font_addon();
	al_init_ttf_addon();

	if (!al_install_mouse()) {
		al_show_native_message_box(display, "Error", "Could not initialize the mouse", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	if (!al_init_primitives_addon()) {
		al_show_native_message_box(display, "Error", "Could not initialize the primitives addon", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	display = al_create_display(1600, 900);
	if (!display) {
		al_show_native_message_box(display, "Error", "Could not create display", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	int screen_w = al_get_display_width(display);
	int screen_h = al_get_display_height(display);

	event_queue = al_create_event_queue();
	if (!event_queue) {
		al_show_native_message_box(display, "Error", "Could not create event queue", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
		return -1;
	}

	timer = al_create_timer(1.0 / 60);
	if (!timer) {
		al_show_native_message_box(display, "Error", "Could not create timer", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
		al_destroy_event_queue(event_queue);
		return -1;
	}

	ALLEGRO_BITMAP* interf_images[NUM_INTERF];
	for (int i = 0; i < NUM_INTERF; i++) {
		char filename[256];
		sprintf_s(filename, "interf%d.png", i);
		interf_images[i] = al_load_bitmap(filename);
		if (!interf_images[i]) {
			al_show_native_message_box(display, "Error", "Could not load interface", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
			al_destroy_display(display);
			al_destroy_event_queue(event_queue);
			al_destroy_timer(timer);
			for (int j = 0; j < i; j++) {
				al_destroy_bitmap(interf_images[j]);
			}
			return -1;
		}
	}
	init_game_interface(interf_images, interface, 60, screen_w, screen_h);

	ALLEGRO_BITMAP* card_images[NUM_CARDS / 2];
	for (int i = 0; i < NUM_CARDS / 2; i++) {
		char filename[256];
		sprintf_s(filename, "card%d.png", i);
		card_images[i] = al_load_bitmap(filename);
		if (!card_images[i]) {
			al_show_native_message_box(display, "Error", "Could not load card image", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
			al_destroy_display(display);
			al_destroy_event_queue(event_queue);
			al_destroy_timer(timer);
			for (int j = 0; j < i; j++) {
				al_destroy_bitmap(card_images[j]);
			}
			return -1;
		}
	}
	init_game_cards(card_images, game_cards, 50, screen_w, screen_h);

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_start_timer(timer);

	int pair = 0; //par de 2 loops
	int score = 0;
	int difficulty = -1;
	int memorize_timer = 5000;
	int increase_score = 125;
	int decrease_score = 25;
	std::string nameScoreBoard;
	std::string player_name;

	game_state.score = 0;

	done = menu_inicial(done, redraw, interface, event_queue);

	if (!done) {
		bool name_gotten = false;
		ALLEGRO_FONT* name_font = al_load_font("arial.ttf", 50, 0);

		while (!name_gotten) {
			ALLEGRO_EVENT event;
			al_wait_for_event(event_queue, &event);

			al_clear_to_color(al_map_rgb(255, 255, 255));
			al_draw_rectangle(590, 485, 1010, 550, al_map_rgb(100, 100, 100), 3);

			al_draw_textf(name_font, al_map_rgb(0, 0, 0), 800, 200, ALLEGRO_ALIGN_CENTRE, "JOGO DA MEMORIA");
			al_draw_textf(name_font, al_map_rgb(0, 0, 0), 800, 400, ALLEGRO_ALIGN_CENTRE, "Type your name: ");
			al_draw_text(name_font, al_map_rgb(0, 0, 0), 800, 490, ALLEGRO_ALIGN_CENTRE, nameScoreBoard.c_str());

			al_flip_display();

			switch (event.type) {
			case ALLEGRO_EVENT_KEY_DOWN:
				std::cout << "Pressionando a tecla: " << al_keycode_to_name(event.keyboard.keycode) << ", tamanho: " << nameScoreBoard.size() << std::endl;

				if (event.keyboard.keycode == 67) name_gotten = true;
				if (event.keyboard.keycode == 63) { if (nameScoreBoard.size() != 0) nameScoreBoard.pop_back(); }
				if (nameScoreBoard.size() < 12 && event.keyboard.keycode == 75) nameScoreBoard += " ";
				if (nameScoreBoard.size() < 12 && event.keyboard.keycode >= 1 && event.keyboard.keycode <= 36) nameScoreBoard += al_keycode_to_name(event.keyboard.keycode);

				break;
			}
		}

		difficulty = menu_dificuldades(redraw, interface, event_queue);
	}

	bool memorize = true;

	if (difficulty == -1) {
		done = true;
	}

	if (difficulty == 0) {
		memorize_timer = 10000;
	}

	//Jogo
	while (!done) {
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if (memorize && difficulty != 2) {
			memorizeCards(game_cards, interface, memorize_timer);
			memorize = false;
		}

		game_state.matches = num_matches;

		if (game_state.score <= score) {
			game_state.score = score;
		}

		if (pair == 2) {
			last_card = -1;
			selected_card = -1;
			pair = 0;
		}

		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			done = true;
		}
		else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			int x = event.mouse.x;
			int y = event.mouse.y;

			//for -> cartas
			for (int i = 0; i < NUM_CARDS; i++) {
				if (x > game_cards[i].x && x < game_cards[i].x + CARD_W && y > game_cards[i].y && y < game_cards[i].y + CARD_H && game_cards[i].is_flipped == false) {
					pair++;
					if (selected_card == -1) {
						game_cards[i].is_flipped = true;
						redraw = true;
						selected_card = i;
					}
					else if (selected_card != -1 && game_cards[i].is_flipped == false) {
						game_cards[i].is_flipped = true;
						redraw = true;
						last_card = selected_card;
						selected_card = i;
						//Se as cartas forem iguais, entra aqui.
						if (game_cards[i].id == game_cards[last_card].id) {
							score += increase_score;
							num_matches++;
							game_state.matches = num_matches;
							game_cards[i].is_flipped = true;
							game_cards[last_card].is_flipped = true;
							if (num_matches == (NUM_CARDS / 2)) {
								game_state.score = score;
								al_show_native_message_box(display, "Congratulations", "You have won the game!", "", NULL, ALLEGRO_MESSAGEBOX_WARN);
								num_matches = 0;
							}
						}
						else {
							game_cards[i].is_flipped = true;
							for (int i = 0; i < NUM_CARDS; i++) {
								draw_card(game_cards[i]);
							}
							al_flip_display();
							al_rest(1.0);
							score -= 25;
							game_cards[i].is_flipped = false;
							game_cards[last_card].is_flipped = false;
							last_card = -1;
							selected_card = -1;
						}
					}
					break;
				}
			}

			//for -> interface
			for (int i = 0; i < NUM_INTERF - 2; i++) { //NUM_INTERF - 2 porque remove-se os dois bot�es de PLAY e QUIT do menu inicial.
				//QUIT
				if (x > interface[1].x && x < interface[1].x + INTERF_W && y > interface[1].y && y < interface[1].y + INTERF_H) {
					done = true;
					break;
				}
				//RESET
				if (x > interface[2].x && x < interface[2].x + INTERF_W && y > interface[2].y && y < interface[2].y + INTERF_H) {
					for (int i = 0; i < NUM_CARDS; i++) {
						num_matches = 0;
						score = 0;
						game_cards[i].is_flipped = false;
						last_card = -1;
						selected_card = -1;

					}
					shuffle(card_images, game_cards);
					memorize = true;
					if (memorize && difficulty != 2) {
						memorizeCards(game_cards, interface, memorize_timer);
						memorize = false;
					}
					break;
				}
				//SAVE *****FAZER*****
				if (x > interface[3].x && x < interface[3].x + INTERF_W && y > interface[3].y && y < interface[3].y + INTERF_H) {
					player_name = get_player_name();
					std::string file_name = player_name + ".sav";
					//save_game(file_name, game_state);
					break;
				}
				//CHANGE PLAYER *****FAZER*****
				if (x > interface[4].x && x < interface[4].x + INTERF_W && y > interface[4].y && y < interface[4].y + INTERF_H) {
					std::string file_name = get_file_name(display);
					//game_state = load_game(file_name);
					std::cout << "Type your name: ";
					std::cin >> nameScoreBoard;
					break;
				}
			}
		}
		else if (event.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;
		}

		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			al_clear_to_color(al_map_rgb(255, 255, 255));
			for (int i = 0; i < NUM_CARDS; i++) {
				draw_card(game_cards[i]);
			}
			for (int i = 0; i < NUM_INTERF - 7; i++) {
				draw_interface(interface[i]);
			}
			scoreBoard(score, nameScoreBoard);
			al_flip_display();
		}
	}
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);

	for (int i = 0; i < (NUM_CARDS / 2); i++) {
		al_destroy_bitmap(card_images[i]);
	}

	return 0;
}