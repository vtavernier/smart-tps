#ifndef _GUI_HPP_
#define _GUI_HPP_

enum class GuiStateKind {
	Splash,
	Stats,
};

struct SplashData {};
struct StatsData {
	float frequency;
	float duty_cycle;
	float expected_duty_cycle;
	unsigned long screen_lasts;
};

struct GuiState {
	GuiStateKind kind;

	union {
		SplashData splash;
		StatsData stats;
	} data;

	static GuiState Splash(SplashData data);
	static GuiState Stats(StatsData data);

	bool operator==(const GuiState &rhs) const;
	bool operator!=(const GuiState &rhs) const;

	GuiState() = delete;
};

/**
 * Implementation of the GUI class for the current hardware
 */
class GuiImpl {
      public:
	virtual ~GuiImpl() = default;

	// Base functions
	virtual void begin() = 0;
	virtual void clear() = 0;
	virtual void display() = 0;

	// Screen drawing functions
	virtual void show_splash(const SplashData &data) = 0;
	virtual void show_stats(const StatsData &data) = 0;
};

extern GuiImpl *new_gui_impl();

class Gui {
	GuiState state_;
	GuiImpl *impl_;
	bool dirty_;

      public:
	Gui();
	~Gui();

	void begin();
	void display();

	void show_splash(const SplashData &data);
	void show_stats(const StatsData &data);
};

#endif /* _GUI_HPP_ */
