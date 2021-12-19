#pragma once
#include <unordered_map>

/*
enum TRAINER_UI_MSG {
	ADD_GRANADE = 1
};

struct Config {
	bool b_freezeHp;
	bool b_invulnerableEveryone;
	bool b_invulnerableSelf;
	bool b_infiniteAmmo;
	bool b_healingGranades;

	bool b_noRecoil;
	bool b_aimbot;
};
*/
namespace Features {

	struct Feature {
		enum class Group {
			Core,
			BotOnly,
		};

		enum class Widget {
			Checkbox
		};

		std::string name;
		Widget widget;
		Group group;
		bool isEnabled = false;
		bool isPatched = false;

		Feature(std::string p_name, Feature::Widget p_widget, Feature::Group p_group);
	};


	struct Collection {
	private:
		std::vector<Feature*> m_Features;

	public:
		Collection();
		~Collection();
		std::vector<Feature*> All();
		std::vector<Feature*> InGroup(Feature::Group group);
		Feature* Add(std::string name, Feature::Widget widget, Feature::Group group);
		std::vector<Feature::Group> Groups();
	};
}