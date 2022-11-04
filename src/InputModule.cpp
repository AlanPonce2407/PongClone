#include "InputModule.h"
#include "StructsDef.h"

void onKeyUp(int keyCode, InputState& input) {
    switch (keyCode) {
    case SDLK_UP:
        input.up = false;
        break;
    case SDLK_DOWN:
        input.down = false;
        break;
    case SDLK_LEFT:
        input.left = false;
        break;
    case SDLK_RIGHT:
        input.right = false;
        break;
	case SDLK_w:
		input.up2 = false;
		break;
	case SDLK_s:
		input.down2 = false;
		break;
	case SDLK_a:
		input.left2 = false;
		break;
	case SDLK_d:
		input.right2 = false;
		break;
	case SDLK_SPACE:
		input.start = false;
		break;
    default:
        break;
    }
}

void onKeyDown(int keyCode, InputState& input) {
	switch (keyCode) {
	case SDLK_UP:
		input.up = true;
		break;
	case SDLK_DOWN:
		input.down = true;
		break;
	case SDLK_LEFT:
		input.left = true;
		break;
	case SDLK_RIGHT:
		input.right = true;
		break;
	case SDLK_w:
		input.up2 = true;
		break;
	case SDLK_s:
		input.down2 = true;
		break;
	case SDLK_a:
		input.left2 = true;
		break;
	case SDLK_d:
		input.right2 = true;
		break;
	case SDLK_SPACE:
		input.start = true;
		break;
	default:
		break;
	}
}
