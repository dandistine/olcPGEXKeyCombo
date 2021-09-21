/*
	olcPGEX_KeyCombo.h
	+-------------------------------------------------------------+
	|         OneLoneCoder Pixel Game Engine Extension            |
	|              Basic Key Combo Handling - v1.0                |
	+-------------------------------------------------------------+
	What is this?
	~~~~~~~~~~~~~
	This is an extension to the olcPixelGameEngine, which provides
	a method to utilize key combinations such as Ctrl-C with minimal
	boilerplate.  Key Combos should function nearly the same as olc::Key
	Requires PGE > 2.10 - PGEX break-ins within PGE
	Tested with PGE 2.16

	Author
	~~~~~~
	Dandistine

	License (OLC-3)
	~~~~~~~~~~~~~~~
	Copyright 2018 - 2019 OneLoneCoder.com
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:
	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.
	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.
	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
Versions:
1.0 - Initial release
*/

/*
Key Combo Manager

This is a basic extension for handling the input and usage of Key Combos
such as CTRL-C, Shift-X, etc with minimal boiler plate in the application.
A Key Combo is defined in 3 parts, although only 2 need to be provided to
the constructor (Key and Modifiers):
	Key
	ModifierCount
	Modifiers
The Key is the final button which triggers the combination.  In CTRL-C
the Key is C.  ModifierCount is the number of Modifiers which must be in the
bHeld state.  The constructor should automatically assign this.  Modifiers 
is an array of olc::Key describing the modifier keys. Modifiers can be *any* 
key in the system, not just the usual items.  This means that both CTRL-C and 
A-SPACE are equally valid key combinations.  The first requires CTRL to be 
held down while C is pressed.  The second requires A to be held down while 
SPACE is pressed.

Key Combos behave very similarly to normal olc::Key and they should behave the
same in nearly all circumstances.  A Key Combo will become Pressed if all Modifier
keys are in the Held state and Key becomes Pressed.  A Key Combo will be in the
Held state every frame that all keys are in the Held state and the Key Combo was
in either the Pressed or Held state on the previous frame.  A Key Combo will be
in the Released state if any of the Modifiers or Key leaves the Held state and
the Key Combo was in the Held state on the previous frame.  Basically, they
work like normal olc::Key and behave as close to the usual expectation for key
combos as possible.


Key Combo Manager Integration

This PGEX follows the same basic integration steps as most other PGEX.  It is
a single header which has an implementation macro.  Include this header and
define the macro in exactly one place to create the implementation.  Then
include only the header in any other locations required.

A Key Combo is registered into the system with RegisterKeyCombo.  This
function also returns the identifier for the Key Combo.  This identifier needs
to be saved to retrieve the Key Combo's state later; it is analogous to the
olc::Key used in PGE's GetKey() function.  This value should be passed to
the GeyKeyCombo() function to return the Key Combo's current state.


Basic Integration Example


#include "olcPixelGameEngine.h"
#define OLC_PGEX_KEY_COMBO_IMPLEMENTATION
#include "olcPGEX_KeyCombo.h"

class Example : public olc::PixelGameEngine
{
	olc::keycombo::KeyComboManager pge_keycombo;
	size_t ctrl_c_combo;

public:
	Example()
	{
		sAppName = "Test Application";
	}

public:
	bool OnUserCreate() override
	{
		//Register the CTRL-C key combination and save the identifier
		ctrl_c_combo = pge_keycombo.RegisterKeyCombo({ olc::Key::C, {olc::Key::CTRL} });

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		std::string test_str = "None";
		//Check the combo's state and do something with it
		if (pge_keycombo.GetKeyCombo(ctrl_c_combo).bHeld) {
			test_str = "Held";
		}

		if (pge_keycombo.GetKeyCombo(ctrl_c_combo).bPressed) {
			test_str = "Pressed";
		}

		if (pge_keycombo.GetKeyCombo(ctrl_c_combo).bReleased) {
			test_str = "Released";
		}

		DrawString(10, 10, test_str, olc::WHITE);

		return true;
	}
};
*/

#pragma once
#ifndef OLC_PGEX_KEY_COMBO_H_
#define OLC_PGEX_KEY_COMBO_H_
#include "olcPixelGameEngine.h"
#include <array>
#include <algorithm>

namespace olc {
	namespace keycombo {
		//Structure which defines what a key combination actually is
		struct KeyComboDefinition {
			//The main key which triggers the changes in KeyCombo state
			olc::Key Key;

			//Number of modifier keys which must be held to trigger the KeyCombo
			int ModifierCount;

			//Actual modifier keys
			std::array<olc::Key, 4> Modifiers;

			//ty slavka for the brain power
			template<typename T, size_t NumMods>
			KeyComboDefinition(olc::Key MainKey, const T(&Mods)[NumMods]) {
				static_assert(NumMods <= 4); // use a named constant for better error messages
				Key = MainKey;
				ModifierCount = NumMods;
				std::copy(std::begin(Mods), std::end(Mods), std::begin(Modifiers));
			}

		};

		struct KeyCombo {
			KeyComboDefinition Definition;
			olc::HWButton State;
			bool StateOld = false;
			bool StateNew = false;
		};

		class olcPGEX_KeyComboManager : public olc::PGEX {
		public:

			olcPGEX_KeyComboManager();

			size_t RegisterKeyCombo(const KeyComboDefinition def);

			//Automatically run prior to OnUserUpdate and will determine the state of every registered key combo
			void OnBeforeUserUpdate(float& fElapsedTime) override;

			HWButton GetKeyCombo(const int i) const;
		
		private:
			std::vector<KeyCombo> KeyCombos;
		};
	}
}

#ifdef OLC_PGEX_KEY_COMBO_IMPLEMENTATION
namespace olc::keycombo {
	//Passing true to PGEX() will add this into the PGE hooks to be run automatically
	olcPGEX_KeyComboManager::olcPGEX_KeyComboManager() : PGEX(true) {};

	size_t olcPGEX_KeyComboManager::RegisterKeyCombo(const KeyComboDefinition def) {
		KeyCombos.push_back({ def , {} });
		return KeyCombos.size() - 1;
	}

	HWButton olcPGEX_KeyComboManager::GetKeyCombo(const int i) const {
		return KeyCombos[i].State;
	}

	void olcPGEX_KeyComboManager::OnBeforeUserUpdate(float& fElapsedTime) {
		for (auto& kc : KeyCombos) {
			bool mods_held = std::all_of(kc.Definition.Modifiers.begin(),
				kc.Definition.Modifiers.begin() + kc.Definition.ModifierCount,
				[](auto k) {return pge->GetKey(k).bHeld; });

			kc.State.bPressed = false;
			kc.State.bReleased = false;

			olc::HWButton keyState = pge->GetKey(kc.Definition.Key);

			//The combo will become active if all the modifiers are held down and the Key is Pressed
			//or all the modifiers are held down the the combo is already held
			kc.StateNew = mods_held && (keyState.bPressed || (kc.State.bHeld && keyState.bHeld));

			//This is just the same logic as in PGE today for normal key presses.
			if (kc.StateNew != kc.StateOld) {
				if (kc.StateNew) {
					kc.State.bPressed = !kc.State.bHeld;
					kc.State.bHeld = true;
				}
				else {
					kc.State.bReleased = true;
					kc.State.bHeld = false;
				}
			}

			kc.StateOld = kc.StateNew;
		}
	}
}
#endif
#endif