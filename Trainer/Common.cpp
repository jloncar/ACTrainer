#include "Common.h"

Features::Feature::Feature(std::string p_name, Features::Feature::Widget p_widget, Features::Feature::Group p_group): name(p_name), widget(p_widget), group(p_group)
{
}

Features::Collection::Collection() {};

std::vector<Features::Feature*> Features::Collection::All() {
	return m_Features;
};
std::vector<Features::Feature*> Features::Collection::InGroup(Features::Feature::Group group) {
	std::vector<Features::Feature*> filtered;
	for (auto feature : m_Features) {
		if (feature->group == group) {
			filtered.push_back(feature);
		}
	}
	return filtered;
};

Features::Feature* Features::Collection::Add(std::string name, Features::Feature::Widget widget, Features::Feature::Group group) {
	Feature* f = new Feature(name, widget, group);
	m_Features.push_back(f);
	return f;
};

std::vector<Features::Feature::Group> Features::Collection::Groups() {
	std::vector<Features::Feature::Group> groups;
	for (auto feature : m_Features)
	{
		if (std::find(groups.begin(), groups.end(), feature->group) != groups.end())
			continue;

		groups.push_back(feature->group);
	}
	return groups;
};

Features::Collection::~Collection() {
	for (auto feature : m_Features)
	{
		delete feature;
	}
};

