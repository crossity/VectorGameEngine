# VectorGameEngine
small game engine with its own language and text editor

to build this use "g++ file_name -o file_base_name -Wall -I include -lsdl2 -lsdl2_ttf"

- WRITING CODE:
	a) Create variable:
		. "num var_name = number"
	b) Create array:
		. "nums arr_name[size] = {number, number}"
	c) Print text:
		. "print x, y, "text", char 10"
		. "print x, y, num number"
		. "print x, y, char 100"
	d) Change variable:
		. ": var_name = number"
		. ": var_name += number"
	e) Change array:
		. ": arr_name[el] = number"
		. ": arr_name[el] += number"
	f) Change variable:
		. ": var_name = number"
		. ": var_name += number"
	g) Loops:
		. "while ecuasion => ... end"
		. "if ecuasion => ... end"
	h) Functions:
		. setting - "fn function_name num var ... nums var {"
		. ending - "}"
		. calling - "@ function_name var, ..., var @"
		. returning variable - "return num value"
		. returning array - "return nums value"
		. getting from return - "-> variable"
		. getting from return - "->> array"
	i) "Hello, World!" example:
		. easy:
			"fn main {
				print "Hello, World!\n"
			}"
		. hard:
			"fn main {
				@ Hello_World @
			}

			fn Hello_World {
				str a = "Hello, World!"
				print str a, char 10
			}"
	g) Graphics:
		. Choosing color - "color r, g, b"
		. Clearing buffer - "clear"
		. Showing buffer - "render"
		. Draw Pixel - "putpix x, y"
		. Draw Line - "putine x1, y1, x2, y2"
	h) Input handling:
		. Key is pressed - "is_input var"
		. Key that was pressed - "input var"
		. Keys:
			'<' - A/LeftKey
			'>' - D/RightKey
			'V' - S/DownKey
			'^' - W/UpKey
			'A' - J
			'B' - K
	i) Macros:
		. constants - "#define Height 10"
		. macro - "#define SetToTen = 10"
	j) Including:
		1) put library in the same dir as your code file
		2) include it - "#include lib.vec"

- USING TEXT EDITOR:
	a) Navigation:
		. To the end of the file - "cmnd + down"
		. To the beggining of the file - "cmnd + up"
		. To the end of the line - "cmnd + left"
		. To the beggining of the line - "cmnd + right"
	b) Editing:
		. Cut line ufter cursor - "ctrl + K"
		. Cut the hole line - "ctrl + shift + K"
