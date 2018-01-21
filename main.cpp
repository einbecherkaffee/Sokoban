#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>

extern "C" {
#include"./sdl-2.0.7/include/SDL.h"
#include"./sdl-2.0.7/include/SDL_main.h"
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

#define BOK 64

class CMain {
	class Object {
	protected:
		int _posX;
		int _posY;
		SDL_Surface* _img;
		char symbol;
	public:
		Object() {
			_posX = 1;
			_posY = 1;
		}
		Object(int posx, int posy) {
			_posX = posx;
			_posY = posy;
		}
		int getPosX() {
			return _posX;
		}
		int getPosY() {
			return _posY;
		}
		void setPosX(int posx) {
			_posX = posx;
		}
		void setPosY(int posy) {
			_posY = posy;
		}
		void setPos(int x, int y) {
			_posX = x;
			_posY = y;
		}
		char getSymbol() {
			return symbol;
		}
		void draw(SDL_Surface *screen) {
			DrawSurface_(screen, _img, _posX, _posY);
		}
		SDL_Surface* getSurface() {
			return _img;
		}

		void setSurface(SDL_Surface* surface) {
			_img = surface;
		}
	};

	class Board {
		Object*** _board;
		int width;
		int height;
	public:
		Board(int x, int y) {
			width = x;
			height = y;
			_board = new Object**[height];
			for (size_t i = 0; i < height; i++)
			{
				_board[i] = new Object*[width];
				for (size_t j = 0; j < width; j++)
				{
					_board[i][j] = nullptr;
				}
			}
		}
		~Board() {
			for (size_t i = 0; i < height; i++)
			{
				for (size_t j = 0; j < width; j++)
				{
					if (_board[i][j] && _board[i][j]->getSymbol() != 'P')
						delete _board[i][j];
				}
				delete _board[i];
			}
			delete _board;
		}
		int getWidth() {
			return width;
		}
		int getHeight() {
			return height;
		}
		void setSize(int x, int y) {
			width = x;
			height = y;
		}
		Object*** getBoard() {
			return _board;
		}
		Object* getObject(int x, int y) {
			return _board[y][x];
		}
		void setObject(int x, int y, Object* obj) {
			_board[y][x] = obj;
		}
		void clearObject(int x, int y) {
			_board[y][x] = nullptr;
		}

	};



	enum player_move {
		MOVE_UP,
		MOVE_DOWN,
		MOVE_LEFT,
		MOVE_RIGHT
	};
	class Player : public Object {
		Board* _board;
	public:
		Player(Board* board) {
			_board = board;
			_img = SDL_LoadBMP("./eti.bmp");
			_board->setObject(_posX, _posY, this);
			symbol = 'P';
		}

		void move(player_move move) {
			int x, y;
			x = y = 0;
			switch (move) {
			case MOVE_UP:
				y--;
				break;
			case MOVE_DOWN:
				y++;
				break;
			case MOVE_LEFT:
				x--;
				break;
			case MOVE_RIGHT:
				x++;
				break;
			}
			int x2 = x;
			int y2 = y;
			x += _posX;
			y += _posY;
			if (x < 0 || x > _board->getWidth() - 1)
				return;
			if (y < 0 || y > _board->getHeight() - 1)
				return;
			Object* obj = _board->getObject(x, y);
			if (obj) {
				if (obj->getSymbol() == 'W')
					return;
				if (obj->getSymbol() == 'B') {
					if (x2 + x < 0 || x2 + x> _board->getWidth() - 1)
						return;
					if (y2 + y < 0 || y2 + y > _board->getHeight() - 1)
						return;
					if (_board->getObject(x2 + x, y2 + y) != nullptr)
						return;
					Object* w = _board->getObject(x, y);
					w->setPos(x2 + x, y2 + y);
					_board->setObject(x2 + x, y2 + y, w);
					_board->clearObject(_posX, _posY);
					_posX = x;
					_posY = y;
					_board->setObject(_posX, _posY, this);
				}
			}
			else {
				_board->clearObject(_posX, _posY);
				_posX = x;
				_posY = y;
				_board->setObject(_posX, _posY, this);
			}

		}
		void setBoard(Board* board) {
			_board = board;
		}
	};
	class Wall : public Object {
	public:
		Wall(int x, int y, SDL_Surface* s) : Object(x, y) {
			_img = s;
			symbol = 'W';
		}
		~Wall() {

		}
	};
	class Box : public Object {
	public:
		Box(int x, int y, SDL_Surface* s) : Object(x, y) {
			_img = s;
			symbol = 'B';
		}
		~Box() {

		}
	};
public:
	CMain() {
		_board = new Board(10, 10);
	};
	~CMain() {
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
	}
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Surface *eti;
	SDL_Surface *wall;
	SDL_Surface *box;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
	Board* _board;
	Player* gracz;
	// narysowanie napisu txt na powierzchni screen, zaczynajπc od punktu (x, y)
	// charset to bitmapa 128x128 zawierajπca znaki
	// draw a text txt on surface screen, starting from the point (x, y)
	// charset is a 128x128 bitmap containing character images
public:
	void DrawString(SDL_Surface *screen, int x, int y, const char *text,
		SDL_Surface *charset) {
		int px, py, c;
		SDL_Rect s, d;
		s.w = 8;
		s.h = 8;
		d.w = 8;
		d.h = 8;
		while (*text) {
			c = *text & 255;
			px = (c % 16) * 8;
			py = (c / 16) * 8;
			s.x = px;
			s.y = py;
			d.x = x;
			d.y = y;
			SDL_BlitSurface(charset, &s, screen, &d);
			x += 8;
			text++;
		};
	};
	// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
	// (x, y) to punkt úrodka obrazka sprite na ekranie
	// draw a surface sprite on a surface screen in point (x, y)
	// (x, y) is the center of sprite on screen
	static void DrawSurface_(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
		DrawSurface(screen, sprite, x*BOK + BOK / 2, y*BOK + BOK / 2);
	}
	static void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
		SDL_Rect dest;
		dest.x = x - sprite->w / 2;
		dest.y = y - sprite->h / 2;
		dest.w = sprite->w;
		dest.h = sprite->h;
		SDL_BlitSurface(sprite, NULL, screen, &dest);
	};
	// rysowanie pojedynczego pixela
	// draw a single pixel
	void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
		int bpp = surface->format->BytesPerPixel;
		Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
		*(Uint32 *)p = color;
	};
	// rysowanie linii o d≥ugoúci l w pionie (gdy dx = 0, dy = 1) 
	// bπdü poziomie (gdy dx = 1, dy = 0)
	// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
	void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
		for (int i = 0; i < l; i++) {
			DrawPixel(screen, x, y, color);
			x += dx;
			y += dy;
		};
	};
	// rysowanie prostokπta o d≥ugoúci bokÛw l i k
	// draw a rectangle of size l by k
	void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
		Uint32 outlineColor, Uint32 fillColor) {
		int i;
		DrawLine(screen, x, y, k, 0, 1, outlineColor);
		DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
		DrawLine(screen, x, y, l, 1, 0, outlineColor);
		DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
		for (i = y + 1; i < y + k - 1; i++)
			DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};
	void DrawBoard() {
		int width = _board->getWidth();
		int height = _board->getHeight();
		for (size_t i = 0; i < height; i++)
		{
			for (size_t j = 0; j < width; j++)
			{
				Object* obj = (Object*)_board->getObject(j, i);
				if (obj)
					obj->draw(screen);
			}
		}
	}
	void load() {
		char nazwa_pliku[] = { "default.txt" };
		FILE* plik = fopen(nazwa_pliku, "r");
		if (plik != NULL) {
			int size = 0;
			fscanf(plik, "%d\n", &size);
			char* linia = new char[size + 2];

			delete _board;
			_board = new Board(10, 10);

			for (int j = 0; j < size; j++) {
				fgets(linia, size + 2, plik);
				for (int i = 0; i < size; i++) {
					if (linia[i] == 'P') {
						_board->setObject(i, j, gracz);
						gracz->setBoard(_board);
						gracz->setPos(i, j);
					}
					else if (linia[i] == 'W') {
						_board->setObject(i, j, new Wall(i, j, wall));
					}
					else if (linia[i] == 'B') {
						_board->setObject(i, j, new Box(i, j, box));
					}
					else {
						_board->clearObject(i, j);
					}
				}

			}
			delete[] linia;
			fclose(plik);
			//DrawBoard();
		}
		else {
			system("pause");
		}
	}
	int start() {
		int t1, t2, quit, frames, rc;
		double delta, worldTime, fpsTimer, fps, distance, etiSpeed;

		// okno konsoli nie jest widoczne, jeøeli chcemy zobaczyÊ
		// komunikaty wypisywane printf-em trzeba w opcjach:
		// project -> szablon2 properties -> Linker -> System -> Subsystem
		// zmieniÊ na "Console"
		// console window is not visible, to see the printf output
		// the option:
		// project -> szablon2 properties -> Linker -> System -> Subsystem
		// must be changed to "Console"
		printf("wyjscie printfa trafia do tego okienka\n");
		printf("printf output goes here\n");

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			printf("SDL_Init error: %s\n", SDL_GetError());
			return 1;
		}

		// tryb pe≥noekranowy / fullscreen mode
		//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
		//	                                 &window, &renderer);
		rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
			&window, &renderer);
		if (rc != 0) {
			SDL_Quit();
			printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
			return 1;
		};

		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		SDL_SetWindowTitle(window, "Szablon do zdania drugiego 2017");


		screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
			0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

		scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			SCREEN_WIDTH, SCREEN_HEIGHT);


		// wy≥πczenie widocznoúci kursora myszy
		SDL_ShowCursor(SDL_DISABLE);

		// wczytanie obrazka cs8x8.bmp
		charset = SDL_LoadBMP("./cs8x8.bmp");
		if (charset == NULL) {
			printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
			SDL_FreeSurface(screen);
			SDL_DestroyTexture(scrtex);
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_Quit();
			return 1;
		};
		SDL_SetColorKey(charset, true, 0x000000);

		eti = SDL_LoadBMP("./eti.bmp");
		if (eti == NULL) {
			printf("SDL_LoadBMP(eti.bmp) error: %s\n", SDL_GetError());
			SDL_FreeSurface(charset);
			SDL_FreeSurface(screen);
			SDL_DestroyTexture(scrtex);
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_Quit();
			return 1;
		};
		wall = SDL_LoadBMP("./wall.bmp");
		if (wall == NULL) {
			printf("SDL_LoadBMP(wall.bmp) error: %s\n", SDL_GetError());
			SDL_FreeSurface(charset);
			SDL_FreeSurface(screen);
			SDL_DestroyTexture(scrtex);
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_Quit();
			return 1;
		};
		box = SDL_LoadBMP("./box.bmp");
		if (box == NULL) {
			printf("SDL_LoadBMP(box.bmp) error: %s\n", SDL_GetError());
			SDL_FreeSurface(charset);
			SDL_FreeSurface(screen);
			SDL_DestroyTexture(scrtex);
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_Quit();
			return 1;
		};
		//Player gracz(_board);
		gracz = new Player(_board);
		if (gracz->getSurface() == NULL) {
			printf("SDL_LoadBMP(eti.bmp) error: %s\n", SDL_GetError());
			SDL_FreeSurface(charset);
			SDL_FreeSurface(screen);
			SDL_DestroyTexture(scrtex);
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_Quit();
			return 1;
		};

		char text[128];
		int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
		int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
		int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
		int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

		t1 = SDL_GetTicks();

		frames = 0;
		fpsTimer = 0;
		fps = 0;
		quit = 0;
		worldTime = 0;
		distance = 0;
		etiSpeed = 1;
		load();
		while (!quit) {
			t2 = SDL_GetTicks();

			// w tym momencie t2-t1 to czas w milisekundach,
			// jaki uplyna≥ od ostatniego narysowania ekranu
			// delta to ten sam czas w sekundach
			// here t2-t1 is the time in milliseconds since
			// the last screen was drawn
			// delta is the same time in seconds
			delta = (t2 - t1) * 0.001;
			t1 = t2;

			worldTime += delta;

			distance += etiSpeed * delta;

			SDL_FillRect(screen, NULL, czarny);

			//DrawSurface(screen, eti,
			//	SCREEN_WIDTH / 2 + sin(distance) * SCREEN_HEIGHT / 3,
			//	SCREEN_HEIGHT / 2 + cos(distance) * SCREEN_HEIGHT / 3);
			//gracz->draw(screen);
			DrawBoard();
			fpsTimer += delta;
			if (fpsTimer > 0.5) {
				fps = frames * 2;
				frames = 0;
				fpsTimer -= 0.5;
			};

			// tekst informacyjny / info text
			DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
			//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"
			sprintf(text, "Szablon drugiego zadania, czas trwania = %.1lf s  %.0lf klatek / s", worldTime, fps);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
			//	      "Esc - exit, \030 - faster, \031 - slower"
			sprintf(text, "Esc - wyjscie, \030 - przyspieszenie, \031 - zwolnienie");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			//		SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);

			// obs≥uga zdarzeÒ (o ile jakieú zasz≥y) / handling of events (if there were any)
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_UP) {
						gracz->move(MOVE_UP);
						etiSpeed = 2.0;
					}
					else if (event.key.keysym.sym == SDLK_DOWN) {
						gracz->move(MOVE_DOWN);
						etiSpeed = 0.3;
					}
					else if (event.key.keysym.sym == SDLK_LEFT) {
						gracz->move(MOVE_LEFT);

					}
					else if (event.key.keysym.sym == SDLK_RIGHT) {
						gracz->move(MOVE_RIGHT);

					}
					else if (event.key.keysym.sym == SDLK_q) {
						quit = 1;
					}
					else if (event.key.keysym.sym == SDLK_l) {
						load();
						worldTime = 0;
					}
					else if (event.key.keysym.sym == SDLK_n) {
						load();
						worldTime = 0;
					}
					break;
				case SDL_KEYUP:
					etiSpeed = 1.0;
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				};
			};
			frames++;
		};
		return 0;
	}

};





// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	CMain game;
	game.start();

	SDL_Quit();
	return 0;
};
// TODO: wczytanie obrazka o mniejszym rozmiarze (skalowanie)