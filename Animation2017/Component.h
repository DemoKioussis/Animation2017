#pragma once


class Component {

private:
	bool enabled;

public:
	void enable() {
		enabled = true;
	}
	void disable() {
		enabled = false;
	}
	const bool isEnabled() {
		return enabled;
	}


};