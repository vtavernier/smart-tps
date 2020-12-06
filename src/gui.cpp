#include <string.h>

#include "gui.hpp"

GuiState GuiState::Splash(SplashData data) { return GuiState{GuiStateKind::Splash, {.splash = data}}; }

GuiState GuiState::Stats(StatsData data) { return GuiState{GuiStateKind::Stats, {.stats = data}}; }

bool GuiState::operator==(const GuiState &rhs) const {
	if (kind != rhs.kind)
		return false;

	switch (kind) {
	case GuiStateKind::Splash:
		return memcmp(&data.splash, &rhs.data.splash, sizeof(data.splash)) == 0;
	case GuiStateKind::Stats:
		return memcmp(&data.splash, &rhs.data.splash, sizeof(data.splash)) == 0;
	}

	return false;
}

bool GuiState::operator!=(const GuiState &rhs) const { return !(*this == rhs); }

Gui::Gui() : state_{GuiState::Splash({})}, impl_{new_gui_impl()}, dirty_{false} {}

Gui::~Gui() {
	delete impl_;
	impl_ = nullptr;
}

void Gui::show_splash(const SplashData &data) {
	auto new_state{GuiState::Splash(data)};

	if (state_ != new_state) {
		state_ = new_state;
		dirty_ = true;

		impl_->clear();
		impl_->show_splash(state_.data.splash);
	}
}

void Gui::show_stats(const StatsData &data) {
	auto new_state{GuiState::Stats(data)};

	if (state_ != new_state) {
		state_ = new_state;
		dirty_ = true;

		impl_->clear();
		impl_->show_stats(state_.data.stats);
	}
}

void Gui::begin() { impl_->begin(); }

void Gui::display() {
	if (dirty_) {
		dirty_ = false;
		impl_->display();
	}
}
