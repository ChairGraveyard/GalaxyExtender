#include "stdafx.h"

#include "MessageQueue.h"
#include "GroundScene.h"
#include "Game.h"
#include "Graphics.h"
#include "InputMap.h"

void GroundScene::parseMessages(InputMap* map) {
	MessageQueue* queue = map->getMessageQueue();

	bool reset = false;

	for (uint32_t i = 0; i < queue->getNumberOfMessages(); ++i) {
		int message; float value;

		queue->getMessage(i, &message, &value);

		switch (message) {
		case 125: {
			if (value == 0)
				value = 2;

			setView((int)value, 0);
			reset = true;
			break;
		}
		case 228:
			setView(5, 0);
			reset = true;
			break;
		case 147: {
			static bool solid = true;

			if (solid) {
				solid = false;
				Graphics::setFillMode(0);
			} else {
				solid = true;
				Graphics::setFillMode(1);
			}
			break;
		}
		default:
			break;
		}
	}

	if (reset) {
		map->handleInputReset();
	} else {
		originalParseMessages::run(this, map);
	}
}