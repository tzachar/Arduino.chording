#include <Arduino.h>

#define BTN_0_PIN 5
#define BTN_1_PIN 5
#define BTN_2_PIN 4
#define BTN_3_PIN 3
#define BTN_4_PIN 2
#define BTN_5_PIN 6
//thumbs
#define BTN_6_PIN 7
#define BTN_7_PIN 8

struct chord_t
{
	bool btn_0 : 1;
	bool btn_1 : 1;
	bool btn_2 : 1;
	bool btn_3 : 1;
	bool btn_4 : 1;
	bool btn_5 : 1;
	bool btn_6 : 1;
	bool btn_7 : 1;

	bool operator==(const chord_t &other) const
	{
		return memcmp(this, &other, sizeof(*this)) == 0;
	}

	bool operator!=(const chord_t &other) const
	{
		return memcmp(this, &other, sizeof(*this)) != 0;
	}

	unsigned int to_int() const
	{
		unsigned int ans = 0;
		if (btn_0)
			ans += (1u<<0);
		if (btn_1)
			ans += (1u<<1);
		if (btn_2)
			ans += (1u<<2);
		if (btn_3)
			ans += (1u<<3);

		if (btn_4)
			ans += (1u<<4);
		if (btn_5)
			ans += (1u<<5);
		if (btn_6)
			ans += (1u<<6);
		if (btn_7)
			ans += (1u<<7);
		return ans;
	}

	bool empty() const;

	bool only_thumb() const
	{
		return btn_0 == false &&
			btn_1 == false && 
			btn_2 == false && 
			btn_3 == false && 
			btn_4 == false && 
			btn_5 == false && 
			(btn_6 == true || btn_7 == true);
	}

	void print() const
	{
		Serial.print(btn_0);
		Serial.print(btn_1);
		Serial.print(btn_2);
		Serial.print(btn_3);
		Serial.print(btn_4);
		Serial.print(btn_5);
		Serial.print(btn_6);
		Serial.println(btn_7);
	}
};

chord_t empty_chord = {0, 0, 0, 0, 0, 0, 0, 0};
bool chord_t::empty() const
{
	return *this == empty_chord;
}

#define READ_CHORD()\
	{\
		digitalRead(BTN_0_PIN) == LOW,\
		digitalRead(BTN_1_PIN) == LOW,\
		digitalRead(BTN_2_PIN) == LOW,\
		digitalRead(BTN_3_PIN) == LOW,\
		digitalRead(BTN_4_PIN) == LOW,\
		digitalRead(BTN_5_PIN) == LOW,\
		digitalRead(BTN_6_PIN) == LOW,\
		digitalRead(BTN_7_PIN) == LOW,\
	}

static const unsigned long debounce_delay = 60;
unsigned long last_update = 0;
chord_t last_chord;
chord_t last_chord_returned;


//return the currently pressed chord, using debouncing
chord_t from_btns()
{
	unsigned long time = millis();
	chord_t current_chord = READ_CHORD(); 
		
	if (current_chord == last_chord){
	    if ((time - last_update) > debounce_delay){
		//ok, return this as our chord.
		last_update = time - debounce_delay - 1;
		last_chord_returned = current_chord;
		return current_chord;
	    } else {
	    	//not enough time has passed...
		return last_chord_returned;
	    }
	} else {
		//user is changing the chord.
		last_chord = current_chord;
		last_update = time;
		//return the previous chord
		return last_chord_returned;
	}
}

unsigned char regular_scancodes[128] = {0};
unsigned char keypad_scancodes[128] = {0};
static struct regular_scancodes_initor
{
	regular_scancodes_initor()
	{
		regular_scancodes[1] = 't';
		regular_scancodes[2] = 'e';
		regular_scancodes[4] = 'n';
		regular_scancodes[8] = 'r';
		regular_scancodes[16] = 's';
		regular_scancodes[32] = 'i';
		regular_scancodes[3] = 'o';
		regular_scancodes[5] = 'l';
		regular_scancodes[6] = 'a';
		regular_scancodes[9] = 'd';
		regular_scancodes[10] = 'c';
		regular_scancodes[12] = 'u';
		regular_scancodes[17] = 'p';
		regular_scancodes[18] = 'f';
		regular_scancodes[20] = 'm';
		regular_scancodes[24] = 'v';
		regular_scancodes[33] = 'h';
		regular_scancodes[34] = 'z';
		regular_scancodes[36] = 'x';
		regular_scancodes[40] = 'b';
		regular_scancodes[48] = 'y';
		regular_scancodes[7] = 'k';
		regular_scancodes[11] = 'w';
		regular_scancodes[13] = 'j';
		regular_scancodes[14] = 'g';
		regular_scancodes[19] = 'q';
		regular_scancodes[21] = '0';
		regular_scancodes[22] = '/';
		regular_scancodes[25] = '1';
		regular_scancodes[26] = '3';
		regular_scancodes[28] = '2';
		regular_scancodes[35] = '7';
		regular_scancodes[37] = '9';
		regular_scancodes[38] = '5';
		regular_scancodes[41] = '6';
		regular_scancodes[42] = '8';
		regular_scancodes[44] = '4';
		regular_scancodes[49] = '.';
		regular_scancodes[50] = ',';
		regular_scancodes[52] = '(';
		regular_scancodes[56] = ')';
		regular_scancodes[15] = '-';
		regular_scancodes[23] = ';';
		regular_scancodes[27] = '=';
		regular_scancodes[29] = KEY_RETURN;
		regular_scancodes[30] = KEY_ESC;
		regular_scancodes[39] = KEY_TAB;
		regular_scancodes[43] = KEY_BACKSPACE;
		regular_scancodes[45] = ' ';
		regular_scancodes[46] = '>';
		regular_scancodes[51] = '[';
		regular_scancodes[53] = ']';
		regular_scancodes[54] = '\\';
		regular_scancodes[57] = '\'';



		keypad_scancodes[1] = KEY_LEFT_CTRL;
		keypad_scancodes[2] = KEY_LEFT_SHIFT;
		keypad_scancodes[4] = KEY_LEFT_ALT;
		keypad_scancodes[8] = KEY_LEFT_GUI;
		keypad_scancodes[16] = KEY_RIGHT_CTRL;
		keypad_scancodes[32] = KEY_RIGHT_SHIFT;
		keypad_scancodes[3] = KEY_RIGHT_ALT;
		keypad_scancodes[5] = KEY_RIGHT_GUI;
		keypad_scancodes[6] = KEY_UP_ARROW;
		keypad_scancodes[9] = KEY_DOWN_ARROW;
		keypad_scancodes[10] = KEY_LEFT_ARROW;
		keypad_scancodes[12] = KEY_RIGHT_ARROW;
		keypad_scancodes[17] = KEY_BACKSPACE;
		keypad_scancodes[18] = KEY_TAB;
		keypad_scancodes[20] = KEY_RETURN;
		keypad_scancodes[24] = KEY_ESC;
		keypad_scancodes[33] = KEY_INSERT;
		keypad_scancodes[34] = KEY_DELETE;
		keypad_scancodes[36] = KEY_PAGE_UP;
		keypad_scancodes[40] = KEY_PAGE_DOWN;
		keypad_scancodes[48] = KEY_HOME;
		keypad_scancodes[7] = KEY_END;
		keypad_scancodes[11] = KEY_CAPS_LOCK;
		keypad_scancodes[13] = KEY_F1;
		keypad_scancodes[14] = KEY_F2;
		keypad_scancodes[19] = KEY_F3;
		keypad_scancodes[21] = KEY_F4;
		keypad_scancodes[22] = KEY_F5;
		keypad_scancodes[25] = KEY_F6;
		keypad_scancodes[26] = KEY_F7;
		keypad_scancodes[28] = KEY_F8;
		keypad_scancodes[35] = KEY_F9;
		keypad_scancodes[37] = KEY_F10;
		keypad_scancodes[38] = KEY_F11;
		keypad_scancodes[41] = KEY_F12;
	}

} regular_scancodes_initor;


chord_t last_chord_sent = empty_chord;
unsigned long autorepeat_delay = 550;
unsigned long autorepeat_between = 20;
bool in_autorepeat = false;
unsigned long last_key_send_time = 0;
enum mode_t {NORMAL, KEYPAD};
mode_t mode = NORMAL;

void chord_to_scancode(const chord_t &chord)
{
	//if (! chord.empty())
		//chord.print();

	//check for a thumbs only press.
	if (chord.only_thumb()){
		//mode switch...
		if (mode == NORMAL)
			mode == KEYPAD;
		else if (mode == KEYPAD)
			mode = NORMAL;
	
		last_chord_sent = empty_chord;
		last_key_send_time = millis();
		Keyboard.releaseAll();
		return;
	}

	//ok, we are starting a key press.
	if (chord != last_chord_sent){
		//Keyboard.releaseAll();
	}

	if (chord.empty()){
		last_chord_sent = empty_chord;
		last_key_send_time = millis();
		Keyboard.releaseAll();
		return;
	}

	unsigned char keys_to_press[10] = {0};
	unsigned int keys_to_press_count = 0;


	if (mode == NORMAL){
		keys_to_press[0] = regular_scancodes[chord.to_int()];
		keys_to_press_count = 1;
	} else if (mode == KEYPAD) {
		keys_to_press[0] = keypad_scancodes[chord.to_int()];
		keys_to_press_count = 1;
	}

	if (chord.btn_6 && chord.btn_7){
		//both thumbs are pressed
		keys_to_press[keys_to_press_count++] = 
			KEY_LEFT_ALT;
	} else if (chord.btn_6) {
		keys_to_press[keys_to_press_count++] = 
			KEY_LEFT_CTRL;
	} else if (chord.btn_7) {
		keys_to_press[keys_to_press_count++] = 
			KEY_LEFT_SHIFT;
	}

	//handle send and autorepeat logic
	bool need_send = true;
	if (chord == last_chord_sent){
		if (in_autorepeat){
			//only send if we passed autorepeat delay
			if (millis() - last_key_send_time < autorepeat_between){
				need_send = false;
			}
		} else {
			//check if we need to turn autorepeat on
			if (millis() - last_key_send_time < autorepeat_delay){
				need_send = false;
			} else
				in_autorepeat = true;
		}
	} else {
		in_autorepeat = false;
	}
	if (need_send){
		last_key_send_time = millis();
		last_chord_sent = chord;
		for (unsigned i = 0; i < keys_to_press_count; ++i)
			Keyboard.press(keys_to_press[i]);
	}

	Keyboard.releaseAll();
}

void setup(){
	//start serial connection
	Serial.begin(9600);
	pinMode(13, OUTPUT);

	pinMode(BTN_0_PIN, INPUT_PULLUP);
	pinMode(BTN_1_PIN, INPUT_PULLUP);
	pinMode(BTN_2_PIN, INPUT_PULLUP);
	pinMode(BTN_3_PIN, INPUT_PULLUP);
	pinMode(BTN_4_PIN, INPUT_PULLUP);
	pinMode(BTN_5_PIN, INPUT_PULLUP);
	pinMode(BTN_6_PIN, INPUT_PULLUP);
	pinMode(BTN_7_PIN, INPUT_PULLUP);

	Keyboard.begin();
}

void loop(){
	chord_t chord = from_btns();
	chord_to_scancode(chord);
	//delay(50);
}
