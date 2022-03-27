#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include "Util.h"

namespace {
	class ColorStateMachine {
	private:
		enum class State { sStart, sSharp, s0x1, s0x2, sR, sG, sB, sA, sR1, sR2, sG1, sG2, sB1, sB2, sA1, sA2 };
		static const std::unordered_map<State, std::unordered_map<char, std::vector<State>>> stateTransitions;
		static const std::unordered_set<State> checkPointStates;

		static constexpr unsigned char hexToNum(const char character) {
			switch (character)
			{
			case '0': return 0;
			case '1': return 1;
			case '2': return 2;
			case '3': return 3;
			case '4': return 4;
			case '5': return 5;
			case '6': return 6;
			case '7': return 7;
			case '8': return 8;
			case '9': return 9;
			case 'A': case 'a': return 0xA;
			case 'B': case 'b': return 0xB;
			case 'C': case 'c': return 0xC;
			case 'D': case 'd': return 0xD;
			case 'E': case 'e': return 0xE;
			case 'F': case 'f': return 0xF;
			}
			return 0;
		}

		static std::tuple<unsigned char, unsigned char, unsigned char, unsigned char> stateAction(const State& state, const char character, std::tuple<unsigned char, unsigned char, unsigned char, unsigned char> data) {
			const auto charVal = hexToNum(character) & 0xF;
			switch (state) {
			case State::sR:
				std::get<0>(data) = charVal * 0x11;
				break;
			case State::sG:
				std::get<1>(data) = charVal * 0x11;
				break;
			case State::sB:
				std::get<2>(data) = charVal * 0x11;
				break;
			case State::sA:
				std::get<3>(data) = charVal * 0x11;
				break;
			case State::sR1:
				std::get<0>(data) = charVal * 0x10;
				break;
			case State::sR2:
				std::get<0>(data) = (std::get<0>(data) & 0xF0) | charVal;
				break;
			case State::sG1:
				std::get<1>(data) = charVal * 0x10;
				break;
			case State::sG2:
				std::get<1>(data) = (std::get<1>(data) & 0xF0) | charVal;
				break;
			case State::sB1:
				std::get<2>(data) = charVal * 0x10;
				break;
			case State::sB2:
				std::get<2>(data) = (std::get<2>(data) & 0xF0) | charVal;
				break;
			case State::sA1:
				std::get<3>(data) = charVal * 0x10;
				break;
			case State::sA2:
				std::get<3>(data) = (std::get<3>(data) & 0xF0) | charVal;
				break;
			}
			return data;
		}

	public:
		static std::tuple<unsigned char, unsigned char, unsigned char, unsigned char> convert(const std::string& colorText)
		{
			std::list<std::pair<ColorStateMachine::State, std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>>> threads = { {ColorStateMachine::State::sStart, {0, 0, 0, 0xff}} };
			std::tuple<unsigned char, unsigned char, unsigned char, unsigned char> result = { 0, 0, 0, 0xff };
			for (const auto& character : colorText) { // итерация по тексту
				if (threads.empty())
					break;
				for (auto it = threads.begin(); it != threads.end();)
				{
					std::vector<std::pair<ColorStateMachine::State, std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>>> availableTransitions; // следующие состояния из текущего
					if (const auto transitionIt = stateTransitions.find(it->first); transitionIt != stateTransitions.end()) // проверка наличия транзиции из этого состояния
					{
						if (const auto hasTransitionIt = transitionIt->second.find(character); hasTransitionIt != transitionIt->second.end()) // проверка наличия транзиции из этого состояния с текущим символом
						{
							for (const auto& state : hasTransitionIt->second) // пихаем новые состояния
							{
								const auto actionResult = stateAction(state, character, it->second);
								availableTransitions.push_back({ state, actionResult });
								if (checkPointStates.count(state)) // сохраняемся, если надо
									result = actionResult;
							}
						}
					}
					if (availableTransitions.empty()) // убираем состояние, т.к. он в тупике
					{
						it = threads.erase(it);
						continue;
					}
					if (availableTransitions.size() == 1) // просто заменяем, если следующее состояние однозначное
					{
						*it = availableTransitions.front();
						it++;
					}
					else // пихаем вместо текущего, если следующих состояний из текущего несколько (надо чтоб итератор на них не указывал)
					{
						it = threads.erase(it);
						threads.insert(it, availableTransitions.begin(), availableTransitions.end());
					}
				}
			}
			return result;
		}
	};
	const std::unordered_map<ColorStateMachine::State, std::unordered_map<char, std::vector<ColorStateMachine::State>>> ColorStateMachine::stateTransitions = {
		{ ColorStateMachine::State::sStart, {
			{'#', {ColorStateMachine::State::sSharp}},
			{'0', {ColorStateMachine::State::s0x1, ColorStateMachine::State::sR, ColorStateMachine::State::sR1}}, {'1', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'2', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'3', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'4', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'5', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'6', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'7', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'8', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'9', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'a', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'A', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'b', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'B', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'c', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'C', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'d', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'D', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'e', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'E', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'f', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'F', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}
			} },
		{ ColorStateMachine::State::s0x1, {{'x', {ColorStateMachine::State::s0x2}}}},
		{ ColorStateMachine::State::s0x2, { {'0', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'1', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'2', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'3', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'4', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'5', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'6', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'7', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'8', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'9', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'a', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'A', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'b', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'B', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'c', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'C', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'d', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'D', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'e', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'E', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'f', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'F', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }} } },
		{ ColorStateMachine::State::sSharp, { {'0', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'1', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'2', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'3', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'4', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'5', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'6', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'7', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'8', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'9', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'a', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'A', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'b', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'B', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'c', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'C', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'d', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'D', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'e', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'E', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'f', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }}, {'F', { ColorStateMachine::State::sR, ColorStateMachine::State::sR1 }} } },
		{ ColorStateMachine::State::sR, { {'0', {ColorStateMachine::State::sG}}, {'1', {ColorStateMachine::State::sG}}, {'2', {ColorStateMachine::State::sG}}, {'3', {ColorStateMachine::State::sG}}, {'4', {ColorStateMachine::State::sG}}, {'5', {ColorStateMachine::State::sG}}, {'6', {ColorStateMachine::State::sG}}, {'7', {ColorStateMachine::State::sG}}, {'8', {ColorStateMachine::State::sG}}, {'9', {ColorStateMachine::State::sG}}, {'a', {ColorStateMachine::State::sG}}, {'A', {ColorStateMachine::State::sG}}, {'b', {ColorStateMachine::State::sG}}, {'B', {ColorStateMachine::State::sG}}, {'c', {ColorStateMachine::State::sG}}, {'C', {ColorStateMachine::State::sG}}, {'d', {ColorStateMachine::State::sG}}, {'D', {ColorStateMachine::State::sG}}, {'e', {ColorStateMachine::State::sG}}, {'E', {ColorStateMachine::State::sG}}, {'f', {ColorStateMachine::State::sG}}, {'F', {ColorStateMachine::State::sG}} } },
		{ ColorStateMachine::State::sG, { {'0', {ColorStateMachine::State::sB}}, {'1', {ColorStateMachine::State::sB}}, {'2', {ColorStateMachine::State::sB}}, {'3', {ColorStateMachine::State::sB}}, {'4', {ColorStateMachine::State::sB}}, {'5', {ColorStateMachine::State::sB}}, {'6', {ColorStateMachine::State::sB}}, {'7', {ColorStateMachine::State::sB}}, {'8', {ColorStateMachine::State::sB}}, {'9', {ColorStateMachine::State::sB}}, {'a', {ColorStateMachine::State::sB}}, {'A', {ColorStateMachine::State::sB}}, {'b', {ColorStateMachine::State::sB}}, {'B', {ColorStateMachine::State::sB}}, {'c', {ColorStateMachine::State::sB}}, {'C', {ColorStateMachine::State::sB}}, {'d', {ColorStateMachine::State::sB}}, {'D', {ColorStateMachine::State::sB}}, {'e', {ColorStateMachine::State::sB}}, {'E', {ColorStateMachine::State::sB}}, {'f', {ColorStateMachine::State::sB}}, {'F', {ColorStateMachine::State::sB}} } },
		{ ColorStateMachine::State::sB, { {'0', {ColorStateMachine::State::sA}}, {'1', {ColorStateMachine::State::sA}}, {'2', {ColorStateMachine::State::sA}}, {'3', {ColorStateMachine::State::sA}}, {'4', {ColorStateMachine::State::sA}}, {'5', {ColorStateMachine::State::sA}}, {'6', {ColorStateMachine::State::sA}}, {'7', {ColorStateMachine::State::sA}}, {'8', {ColorStateMachine::State::sA}}, {'9', {ColorStateMachine::State::sA}}, {'a', {ColorStateMachine::State::sA}}, {'A', {ColorStateMachine::State::sA}}, {'b', {ColorStateMachine::State::sA}}, {'B', {ColorStateMachine::State::sA}}, {'c', {ColorStateMachine::State::sA}}, {'C', {ColorStateMachine::State::sA}}, {'d', {ColorStateMachine::State::sA}}, {'D', {ColorStateMachine::State::sA}}, {'e', {ColorStateMachine::State::sA}}, {'E', {ColorStateMachine::State::sA}}, {'f', {ColorStateMachine::State::sA}}, {'F', {ColorStateMachine::State::sA}} } },
		{ ColorStateMachine::State::sR1, { {'0', {ColorStateMachine::State::sR2}}, {'1', {ColorStateMachine::State::sR2}}, {'2', {ColorStateMachine::State::sR2}}, {'3', {ColorStateMachine::State::sR2}}, {'4', {ColorStateMachine::State::sR2}}, {'5', {ColorStateMachine::State::sR2}}, {'6', {ColorStateMachine::State::sR2}}, {'7', {ColorStateMachine::State::sR2}}, {'8', {ColorStateMachine::State::sR2}}, {'9', {ColorStateMachine::State::sR2}}, {'a', {ColorStateMachine::State::sR2}}, {'A', {ColorStateMachine::State::sR2}}, {'b', {ColorStateMachine::State::sR2}}, {'B', {ColorStateMachine::State::sR2}}, {'c', {ColorStateMachine::State::sR2}}, {'C', {ColorStateMachine::State::sR2}}, {'d', {ColorStateMachine::State::sR2}}, {'D', {ColorStateMachine::State::sR2}}, {'e', {ColorStateMachine::State::sR2}}, {'E', {ColorStateMachine::State::sR2}}, {'f', {ColorStateMachine::State::sR2}}, {'F', {ColorStateMachine::State::sR2}} } },
		{ ColorStateMachine::State::sR2, { {'0', {ColorStateMachine::State::sG1}}, {'1', {ColorStateMachine::State::sG1}}, {'2', {ColorStateMachine::State::sG1}}, {'3', {ColorStateMachine::State::sG1}}, {'4', {ColorStateMachine::State::sG1}}, {'5', {ColorStateMachine::State::sG1}}, {'6', {ColorStateMachine::State::sG1}}, {'7', {ColorStateMachine::State::sG1}}, {'8', {ColorStateMachine::State::sG1}}, {'9', {ColorStateMachine::State::sG1}}, {'a', {ColorStateMachine::State::sG1}}, {'A', {ColorStateMachine::State::sG1}}, {'b', {ColorStateMachine::State::sG1}}, {'B', {ColorStateMachine::State::sG1}}, {'c', {ColorStateMachine::State::sG1}}, {'C', {ColorStateMachine::State::sG1}}, {'d', {ColorStateMachine::State::sG1}}, {'D', {ColorStateMachine::State::sG1}}, {'e', {ColorStateMachine::State::sG1}}, {'E', {ColorStateMachine::State::sG1}}, {'f', {ColorStateMachine::State::sG1}}, {'F', {ColorStateMachine::State::sG1}} } },
		{ ColorStateMachine::State::sG1, { {'0', {ColorStateMachine::State::sG2}}, {'1', {ColorStateMachine::State::sG2}}, {'2', {ColorStateMachine::State::sG2}}, {'3', {ColorStateMachine::State::sG2}}, {'4', {ColorStateMachine::State::sG2}}, {'5', {ColorStateMachine::State::sG2}}, {'6', {ColorStateMachine::State::sG2}}, {'7', {ColorStateMachine::State::sG2}}, {'8', {ColorStateMachine::State::sG2}}, {'9', {ColorStateMachine::State::sG2}}, {'a', {ColorStateMachine::State::sG2}}, {'A', {ColorStateMachine::State::sG2}}, {'b', {ColorStateMachine::State::sG2}}, {'B', {ColorStateMachine::State::sG2}}, {'c', {ColorStateMachine::State::sG2}}, {'C', {ColorStateMachine::State::sG2}}, {'d', {ColorStateMachine::State::sG2}}, {'D', {ColorStateMachine::State::sG2}}, {'e', {ColorStateMachine::State::sG2}}, {'E', {ColorStateMachine::State::sG2}}, {'f', {ColorStateMachine::State::sG2}}, {'F', {ColorStateMachine::State::sG2}} } },
		{ ColorStateMachine::State::sG2, { {'0', {ColorStateMachine::State::sB1}}, {'1', {ColorStateMachine::State::sB1}}, {'2', {ColorStateMachine::State::sB1}}, {'3', {ColorStateMachine::State::sB1}}, {'4', {ColorStateMachine::State::sB1}}, {'5', {ColorStateMachine::State::sB1}}, {'6', {ColorStateMachine::State::sB1}}, {'7', {ColorStateMachine::State::sB1}}, {'8', {ColorStateMachine::State::sB1}}, {'9', {ColorStateMachine::State::sB1}}, {'a', {ColorStateMachine::State::sB1}}, {'A', {ColorStateMachine::State::sB1}}, {'b', {ColorStateMachine::State::sB1}}, {'B', {ColorStateMachine::State::sB1}}, {'c', {ColorStateMachine::State::sB1}}, {'C', {ColorStateMachine::State::sB1}}, {'d', {ColorStateMachine::State::sB1}}, {'D', {ColorStateMachine::State::sB1}}, {'e', {ColorStateMachine::State::sB1}}, {'E', {ColorStateMachine::State::sB1}}, {'f', {ColorStateMachine::State::sB1}}, {'F', {ColorStateMachine::State::sB1}} } },
		{ ColorStateMachine::State::sB1, { {'0', {ColorStateMachine::State::sB2}}, {'1', {ColorStateMachine::State::sB2}}, {'2', {ColorStateMachine::State::sB2}}, {'3', {ColorStateMachine::State::sB2}}, {'4', {ColorStateMachine::State::sB2}}, {'5', {ColorStateMachine::State::sB2}}, {'6', {ColorStateMachine::State::sB2}}, {'7', {ColorStateMachine::State::sB2}}, {'8', {ColorStateMachine::State::sB2}}, {'9', {ColorStateMachine::State::sB2}}, {'a', {ColorStateMachine::State::sB2}}, {'A', {ColorStateMachine::State::sB2}}, {'b', {ColorStateMachine::State::sB2}}, {'B', {ColorStateMachine::State::sB2}}, {'c', {ColorStateMachine::State::sB2}}, {'C', {ColorStateMachine::State::sB2}}, {'d', {ColorStateMachine::State::sB2}}, {'D', {ColorStateMachine::State::sB2}}, {'e', {ColorStateMachine::State::sB2}}, {'E', {ColorStateMachine::State::sB2}}, {'f', {ColorStateMachine::State::sB2}}, {'F', {ColorStateMachine::State::sB2}} } },
		{ ColorStateMachine::State::sB2, { {'0', {ColorStateMachine::State::sA1}}, {'1', {ColorStateMachine::State::sA1}}, {'2', {ColorStateMachine::State::sA1}}, {'3', {ColorStateMachine::State::sA1}}, {'4', {ColorStateMachine::State::sA1}}, {'5', {ColorStateMachine::State::sA1}}, {'6', {ColorStateMachine::State::sA1}}, {'7', {ColorStateMachine::State::sA1}}, {'8', {ColorStateMachine::State::sA1}}, {'9', {ColorStateMachine::State::sA1}}, {'a', {ColorStateMachine::State::sA1}}, {'A', {ColorStateMachine::State::sA1}}, {'b', {ColorStateMachine::State::sA1}}, {'B', {ColorStateMachine::State::sA1}}, {'c', {ColorStateMachine::State::sA1}}, {'C', {ColorStateMachine::State::sA1}}, {'d', {ColorStateMachine::State::sA1}}, {'D', {ColorStateMachine::State::sA1}}, {'e', {ColorStateMachine::State::sA1}}, {'E', {ColorStateMachine::State::sA1}}, {'f', {ColorStateMachine::State::sA1}}, {'F', {ColorStateMachine::State::sA1}} } },
		{ ColorStateMachine::State::sA1, { {'0', {ColorStateMachine::State::sA2}}, {'1', {ColorStateMachine::State::sA2}}, {'2', {ColorStateMachine::State::sA2}}, {'3', {ColorStateMachine::State::sA2}}, {'4', {ColorStateMachine::State::sA2}}, {'5', {ColorStateMachine::State::sA2}}, {'6', {ColorStateMachine::State::sA2}}, {'7', {ColorStateMachine::State::sA2}}, {'8', {ColorStateMachine::State::sA2}}, {'9', {ColorStateMachine::State::sA2}}, {'a', {ColorStateMachine::State::sA2}}, {'A', {ColorStateMachine::State::sA2}}, {'b', {ColorStateMachine::State::sA2}}, {'B', {ColorStateMachine::State::sA2}}, {'c', {ColorStateMachine::State::sA2}}, {'C', {ColorStateMachine::State::sA2}}, {'d', {ColorStateMachine::State::sA2}}, {'D', {ColorStateMachine::State::sA2}}, {'e', {ColorStateMachine::State::sA2}}, {'E', {ColorStateMachine::State::sA2}}, {'f', {ColorStateMachine::State::sA2}}, {'F', {ColorStateMachine::State::sA2}} } }
	};
	const std::unordered_set<ColorStateMachine::State> ColorStateMachine::checkPointStates = { ColorStateMachine::State::sB, ColorStateMachine::State::sA, ColorStateMachine::State::sB2, ColorStateMachine::State::sA2 };
};

std::tuple<unsigned char, unsigned char, unsigned char, unsigned char> Util::textToColor(const std::string& colorText)
{
	return ColorStateMachine::convert(colorText);
}
