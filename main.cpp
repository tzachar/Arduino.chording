#include <Arduino.h>

#define BTN_I_PIN 5
#define BTN_M_PIN 4
#define BTN_R_PIN 3
#define BTN_P_PIN 2
#define BTN_N_PIN 6
#define BTN_C_PIN 7
#define BTN_F_PIN 8

struct chord_t
{
	bool index : 1;
	bool middle : 1;
	bool ring : 1;
	bool pinky : 1;
	bool near : 1;
	bool center : 1;
	bool far : 1;
	bool ignored : 1;

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
		if (index)
			ans += (1u<<3);
		if (middle)
			ans += (1u<<2);
		if (ring)
			ans += (1u<<1);
		if (pinky)
			ans += (1u<<0);

		if (near)
			ans += (1u<<4);
		if (center)
			ans += (1u<<5);
		if (far)
			ans += (1u<<6);
		return ans;
	}

	bool empty() const;

	bool only_thumb() const
	{
		return index == false &&
			middle == false && 
			ring == false &&
			pinky == false &&
			(near == true || center == true || far == true);
	}

	void print() const
	{
		Serial.print(index);
		Serial.print(middle);
		Serial.print(ring);
		Serial.print(pinky);
		Serial.print(near);
		Serial.print(center);
		Serial.println(far);
	}
};

chord_t empty_chord = {0, 0, 0, 0, 0, 0, 0, };
bool chord_t::empty() const
{
	return *this == empty_chord;
}

#define READ_CHORD()\
	{\
		digitalRead(BTN_I_PIN) == LOW,\
		digitalRead(BTN_M_PIN) == LOW,\
		digitalRead(BTN_R_PIN) == LOW,\
		digitalRead(BTN_P_PIN) == LOW,\
		digitalRead(BTN_N_PIN) == LOW,\
		digitalRead(BTN_C_PIN) == LOW,\
		digitalRead(BTN_F_PIN) == LOW,\
		0,\
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

unsigned char no_prefix_chars[128];
static struct no_prefix_chars_initor
{
	no_prefix_chars_initor()
	{
		no_prefix_chars[39] = 'a';
		no_prefix_chars[9] = 'b';
		no_prefix_chars[34] = 'c';
		no_prefix_chars[3] = 'd';
		no_prefix_chars[7] = 'e';
		no_prefix_chars[40] = 'f';
		no_prefix_chars[36] = 'g';
		no_prefix_chars[6] = 'h';
		no_prefix_chars[1] = 'i';
		no_prefix_chars[37] = 'j';
		no_prefix_chars[5] = 'k';
		no_prefix_chars[33] = 'l';
		no_prefix_chars[35] = 'm';
		no_prefix_chars[46] = 'n';
		no_prefix_chars[47] = 'o';
		no_prefix_chars[38] = 'p';
		no_prefix_chars[45] = 'q';
		no_prefix_chars[2] = 'r';
		no_prefix_chars[14] = 's';
		no_prefix_chars[15] = 't';
		no_prefix_chars[12] = 'u';
		no_prefix_chars[44] = 'v';
		no_prefix_chars[8] = 'w';
		no_prefix_chars[41] = 'x';
		no_prefix_chars[4] = 'y';
		no_prefix_chars[13] = 'z';

		no_prefix_chars[9] = 'b';
		no_prefix_chars[9] = 'b';
		no_prefix_chars[9] = 'b';
		no_prefix_chars[9] = 'b';
	
	}

} no_prefix_chars_initor;

chord_t prefix_chord = empty_chord;
chord_t last_chord_sent = empty_chord;
unsigned long autorepeat_delay = 550;
unsigned long autorepeat_between = 20;
bool in_autorepeat = false;
unsigned long last_key_send_time = 0;
void chord_to_scancode(const chord_t &chord)
{
	//if (! chord.empty())
		//chord.print();
	if (! prefix_chord.empty() && chord.only_thumb()){
		//ok, this is a prefix chord.
		//save it for next chord.
		prefix_chord = chord;
		last_chord_sent = empty_chord;
		last_key_send_time = millis();
		Keyboard.releaseAll();
		return;
	}

	//ok, we are starting a key press.
	if (chord != last_chord_sent){
		//Keyboard.releaseAll();
		prefix_chord = empty_chord;
	}

	if (chord.empty()){
		last_chord_sent = empty_chord;
		last_key_send_time = millis();
		Keyboard.releaseAll();
		return;
	}

	unsigned char keys_to_press[10] = {0};
	unsigned int keys_to_press_count = 0;
	if (prefix_chord.empty()){
		keys_to_press[0] = no_prefix_chars[chord.to_int()];
		keys_to_press_count = 1;
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

	pinMode(BTN_I_PIN, INPUT_PULLUP);
	pinMode(BTN_M_PIN, INPUT_PULLUP);
	pinMode(BTN_R_PIN, INPUT_PULLUP);
	pinMode(BTN_P_PIN, INPUT_PULLUP);
	pinMode(BTN_N_PIN, INPUT_PULLUP);
	pinMode(BTN_C_PIN, INPUT_PULLUP);
	pinMode(BTN_F_PIN, INPUT_PULLUP);

	Keyboard.begin();
}

void loop(){
	chord_t chord = from_btns();
	chord_to_scancode(chord);
	//delay(50);
}
