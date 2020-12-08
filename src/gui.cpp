#include <string.h>

#include "gui.hpp"

bool StatsData::operator==(const StatsData &rhs) const {
	return frequency == rhs.frequency && duty_cycle == rhs.duty_cycle && expected_enabled == rhs.expected_enabled &&
	       screen_lasts == rhs.screen_lasts && current_state == rhs.current_state &&
	       (expected_enabled
		    ? (expected_load_type == rhs.expected_load_type && expected_duty_cycle == rhs.expected_duty_cycle)
		    : true);
}

GuiState GuiState::Splash(SplashData data) { return GuiState{GuiStateKind::Splash, {.splash = data}}; }

GuiState GuiState::Stats(StatsData data) { return GuiState{GuiStateKind::Stats, {.stats = data}}; }

bool GuiState::operator==(const GuiState &rhs) const {
	if (kind != rhs.kind)
		return false;

	switch (kind) {
	case GuiStateKind::Splash:
		return memcmp(&data.splash, &rhs.data.splash, sizeof(data.splash)) == 0;
	case GuiStateKind::Stats:
		return data.stats == rhs.data.stats;
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
