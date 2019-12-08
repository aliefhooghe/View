#ifndef VIEW_EVENT_H_
#define VIEW_EVENT_H_

#include <cstdint>

namespace View {

	/**
	 *	\brief Return status of event processing
	 **/

    enum class mouse_button {
		left,
		right,
		wheel
	};

    enum keycode {
		key_unknown,

		//------------
		keycode_count
	};

}

#endif