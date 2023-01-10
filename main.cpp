#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define Width  400
#define Height 400

#define NewLineSpace 30
#define NewCharSpace 22

#define BGColor 47, 60, 126
#define BaseColor 251, 200, 204
#define HighLightColor 251, 100, 108
#define CommentsColor 151, 100, 104

#define TabSize 2

#define CursorHeight NewLineSpace
#define CursorWidth  3
#define CursorBlinking 100

using namespace std;

struct int_vec2 {
    int x, y;
    int_vec2() {
        this->x = 0;
        this->y = 0;
    }
    int_vec2(int a, int b) {
        this->x = a;
        this->y = b;
    }
};

struct int_vec3 {
    int x, y, z;
    int_vec3() {
        this->x = 0.f;
        this->y = 0.f;
        this->z = 0.f;
    }
    int_vec3(int a, int b, int c) {
        this->x = a;
        this->y = b;
        this->z = c;
    }
};

struct variable {
    string name;
    float value;
    variable(string a, float b) {
        this->name = a;
        this->value = b;
    }
    variable(string a) {
        this->name = a;
        this->value = 0.f;
    }
};

struct arr {
    string name;
    vector<float> value;
    arr(string a, vector<float> b) {
        this->name = a;
        this->value = b;
    }
    arr(string a) {
        name = a;
    }
};

struct structure {
    string name;
    vector<string> vars;
    structure() {
        this->name = "test";
    }
    structure(string a) {
        this->name = a;
    }
};

vector<variable> vars;
vector<arr> arrs;
vector<structure> structs;

vector<string> split(string str);
float math(vector<string> nums);
int find(string var_name);
string to_str(float num);
void create_struct(string struct_name, string var_name);

void Run(SDL_Renderer *renderer, TTF_Font *font, SDL_Texture *texture, string code, SDL_Event &event);

void DrawChar(SDL_Renderer* renderer, char text, TTF_Font *font,int x, int y, int_vec3 color, SDL_Rect &m_rectangle, SDL_Texture *m_texture) {
    SDL_Surface *m_surface;

    int texture_width, texture_height;

    SDL_Color Color = {color.x, color.y, color.z, 0};
    char t[2];
    t[0] = text;
    t[1] = 0;
    m_surface = TTF_RenderText_Solid(font, t, Color);

    m_texture = SDL_CreateTextureFromSurface(renderer, m_surface);
    
    texture_height = m_surface->h;
    texture_width = m_surface->w;

    SDL_FreeSurface(m_surface);

    m_rectangle.x = x;
    m_rectangle.y = y;
    m_rectangle.w = texture_width;
    m_rectangle.h = texture_height;    

    SDL_RenderCopy(renderer, m_texture,NULL, &m_rectangle);
    if(m_texture != nullptr){
        SDL_DestroyTexture(m_texture);
    }

}

void Save(vector<string> lines, string file_dir) {
    ofstream file(file_dir);
    for (int i = 0; i < lines.size(); i++)
        file << lines[i] << "\n";
    file.close();
}

void Update(SDL_Renderer *renderer, TTF_Font *font, SDL_Texture *texture, int_vec2 cursor, int_vec2 cursor_camera, vector<string> text, vector<string> keywords, int &cursor_state) {
    SDL_Rect rect;
    SDL_SetRenderDrawColor(renderer, BGColor, 0);
    SDL_RenderClear(renderer);
    for (int l = cursor_camera.y; l < text.size() && l < cursor_camera.y + Height / NewLineSpace; l++) {
        string word = "";
        int_vec3 color;
        bool is_comment = false;

        for (int i = 0; i < text[l].size() /*&& i < Width / NewCharSpace*/; i++) {
            if (text[l][i] == '/' && text[l][i + 1] == '/')
                is_comment = true;
            if (text[l][i] == ' ' || text[l][i] == ',' || 
                text[l][i] == '{' || text[l][i] == '}' ||
                text[l][i] == '(' || text[l][i] == ')' ||
                text[l][i] == '[' || text[l][i] == ']' ||
                text[l][i] == '|' || text[l][i] == ':' ||
                i == text[l].size() - 1/* || i == Width / NewCharSpace - 1*/) {
                if (word != "" || text[l][i] != ' ') {
                    color = int_vec3(BaseColor);
                    int word_size = word.size();
                    if (text[l][i] != ' ' && text[l][i] != ',' && 
                        text[l][i] != '{' && text[l][i] != '}' && 
                        text[l][i] != '(' && text[l][i] != ')' && 
                        text[l][i] != '[' && text[l][i] != ']' && 
                        text[l][i] != '|' && text[l][i] != ':')
                        word += text[l][i];
                    for (int key = 0; key < keywords.size(); key++)
                        if (word == keywords[key])
                            color = int_vec3(HighLightColor);
                    if (is_comment)
                        color = int_vec3(CommentsColor);
                    for (int c = 0; c < word.size(); c++) {
                        DrawChar(renderer, word[c], font, (i - word_size + c - cursor_camera.x) * NewCharSpace, (l - cursor_camera.y) * NewLineSpace, color, rect, texture);
                    }
                    if (text[l][i] == ',' ||
                        text[l][i] == '{' || text[l][i] == '}' || 
                        text[l][i] == '(' || text[l][i] == ')' || 
                        text[l][i] == '[' || text[l][i] == ']' || 
                        text[l][i] == '|' || text[l][i] == ':')
                        DrawChar(renderer, text[l][i], font, (i - cursor_camera.x) * NewCharSpace, (l - cursor_camera.y) * NewLineSpace, int_vec3(BaseColor), rect, texture);
                    word = "";
                }
            }
            else
                word += text[l][i];
        }
    }

    rect.x = (cursor.x - cursor_camera.x) * NewCharSpace;
    rect.y = (cursor.y - cursor_camera.y) * NewLineSpace;
    rect.w = CursorWidth;
    rect.h = CursorHeight;

    if (cursor_state % CursorBlinking < CursorBlinking / 2) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        SDL_RenderFillRect(renderer, &rect);
    }

    cursor_state++;
    if (cursor_state >= CursorBlinking)
        cursor_state = 0;

    SDL_RenderPresent(renderer);
}

int main(int argc, char **argv) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Vector", 900, 20, Width, Height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    string code_dir = "test.vec";
    if (argc > 1)
        code_dir = argv[1];
    
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("config/8bit.ttf", 24);
    if (font == NULL)
        cout << "Error with font" << endl;

    SDL_Texture *texture = NULL;

    bool run = true;

    int_vec2 cursor = int_vec2();
    int_vec2 cursor_camera = int_vec2(6, 0);

    vector<string> keywords;
    string line;
    ifstream keywords_file("config/keywords.txt");
    while (getline(keywords_file, line)) {
        if (line != "")
        keywords.push_back(line);
    }
    keywords_file.close();

    vector<string> text;
    ifstream code_file(code_dir);
    while (getline(code_file, line)) {
        for (int i = 0; i < line.size(); i++) {
            if (line[i] == '\t')
                line[i] = ' ';
        }
        text.push_back(line);
    }
    code_file.close();

    int cursor_state = 0;
    
    SDL_Event event;
    bool command = false;
    bool shift = false;
    bool ctrl = false;
    while (run) {
        while (SDL_PollEvent(&event) == 1) {
            string newline = text[cursor.y];
            int lastlinesize = text[cursor.y - 1].size();
            string code = "";
            int_vec2 mouse;
            switch (event.type) {
                case SDL_QUIT:
                    run = false;
                    break;
                case SDL_TEXTINPUT:
                    text[cursor.y].insert(cursor.x, event.text.text);
                    cursor.x += 1;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_LEFT:
                            if (!command) {
                                cursor.x -= 1;
                                if (cursor.y > 0 && cursor.x == -1) {
                                    cursor.y -= 1;
                                    cursor.x = lastlinesize;
                                }
                                if (cursor.x < 0)
                                    cursor.x += 1;
                            }
                            else
                                cursor.x = 0;
                            cursor_state = 0;
                            break;
                        case SDLK_RIGHT:
                            if (!command) {
                                cursor.x += 1;
                                if (cursor.x > text[cursor.y].size()) {
                                    cursor.y += 1;
                                    cursor.x = 0;
                                    if (cursor.y >= text.size())
                                        text.push_back("");
                                    if (text[cursor.y].size() < cursor.x + 1)
                                        cursor.x = 0;
                                }
                                if (cursor.x > text[cursor.y].size())
                                    cursor.x -= 1;
                            }
                            else
                                cursor.x = text[cursor.y].size();
                            cursor_state = 0;
                            break;
                        case SDLK_UP:
                            if (!command) {
                                cursor.y -= 1;
                                if (cursor.y < 0)
                                    cursor.y = 0;
                            }
                            else
                                cursor.y = 0;
                            cursor_state = 0;
                            break;
                        case SDLK_DOWN:
                            if (!command) {
                                cursor.y += 1;
                                if (cursor.y >= text.size())
                                    text.push_back("");
                            }
                            else
                                cursor.y = text.size() - 1;
                            cursor_state = 0;
                            break;
                        case SDLK_BACKSPACE:
                            if (cursor.x > 0) {
                                text[cursor.y].erase(text[cursor.y].begin() + cursor.x - 1, text[cursor.y].begin() + cursor.x);
                                cursor.x -= 1;
                            }
                            else if (cursor.y > 0) {
                                if (text[cursor.y].size() > 0) {
                                    text[cursor.y - 1] += text[cursor.y];
                                    text.erase(text.begin() + cursor.y);
                                    cursor.y -= 1;
                                    cursor.x = lastlinesize;
                                }
                                else {
                                    text[cursor.y - 1] += text[cursor.y];
                                    text.erase(text.begin() + cursor.y);
                                    cursor.y -= 1;
                                    cursor.x = lastlinesize;
                                }
                            }
                            break;
                        case SDLK_RETURN:
                            if (cursor.x < text[cursor.y].size())
                                text[cursor.y].erase(text[cursor.y].begin() + cursor.x, text[cursor.y].begin() + text[cursor.y].size());
                            if (cursor.x - 1 >= 0)
                                newline.erase(newline.begin(), newline.begin() + cursor.x);
                            text.insert(text.begin() + cursor.y + 1, newline);
                            cursor.y += 1;
                            cursor.x = 0;
                            break;
                        case SDLK_TAB:
                            for (int i = 0; i < TabSize; i++)
                                text[cursor.y].insert(cursor.x, " ");
                            cursor.x += TabSize;
                            break;
                        case SDLK_k:
                            if (ctrl && shift) {
                                text.erase(text.begin() + cursor.y);
                                if (cursor.y >= text.size())
                                    text.push_back("");
                                cursor.x = 0;
                            }
                            else if (ctrl)
                                text[cursor.y].erase(text[cursor.y].begin() + cursor.x, text[cursor.y].begin() + text[cursor.y].size());
                            break;
                        case SDLK_F5:
                            Save(text, code_dir);
                            for (int i = 0; i < text.size(); i++)
                                code += text[i] + '\n';
                            Run(renderer, font, texture, code, event);
                            break;
                        case SDLK_F7:
                            Save(text, code_dir);
                            break;
                        case SDLK_LGUI:
                            command = true;
                            break;
                        case SDLK_LSHIFT:
                            shift = true;
                            break;
                        case SDLK_LCTRL:
                            ctrl = true;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_LGUI:
                            command = false;
                            break;
                        case SDLK_LSHIFT:
                            shift = false;
                            break;
                        case SDLK_LCTRL:
                            ctrl = false;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    if (event.wheel.y != 0) {
                        cursor.y += event.wheel.y;
                        if (cursor.y < 0)
                            cursor.y = 0;
                        if (cursor.y >= text.size())
                            cursor.y = text.size() - 1;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        SDL_GetMouseState(&mouse.x, &mouse.y);
                        mouse.x /= NewCharSpace;
                        mouse.y /= NewLineSpace;
                        mouse.x += cursor_camera.x;
                        mouse.y += cursor_camera.y;
                        cursor.x = mouse.x;
                        cursor.y = mouse.y;

                        if (cursor.y >= text.size())
                            cursor.y = text.size() - 1;
                    }
                    break;
                default:
                    break;
            }
        }
        if (text[cursor.y].size() < cursor.x + 1)
            cursor.x = text[cursor.y].size();
        cursor_camera.y = cursor.y - Height / NewLineSpace / 5 * 4;
        if (cursor_camera.y < 0)
            cursor_camera.y = 0;
        cursor_camera.x = cursor.x - Width / NewCharSpace / 5 * 4;
        if (cursor_camera.x < 0)
            cursor_camera.x = 0;
        Update(renderer, font, texture, cursor, cursor_camera, text, keywords, cursor_state);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    TTF_Quit();
    SDL_Quit();
    return 0;
}

void Run(SDL_Renderer *renderer, TTF_Font *font, SDL_Texture *texture, string code, SDL_Event &event) {
    vector<string> words = split(code);
    vector<int> call_places;
    vector<int> used_vars;
    vector<int> used_arrs;
    used_arrs.push_back(0);
    used_vars.push_back(0);

    bool button_pressed = false;
    char button = '.';

    SDL_Rect rect;

    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);

    SDL_RenderSetLogicalSize(renderer, 200, 200);

    vector<float> returns;
    vector<vector<float> > returns_arr;
    int w = 0;
    for (; words[w] != "{" && words[w - 1] != "main" && words[w - 2] != "fn" && w < words.size(); w++) {
        if (words[w] == "#define") {
            w++;
            string name = words[w];
            w++;
            vector<string> value;
            for (; words[w] != "\n" && words[w] != "|"; w++)
                value.push_back(words[w]);
            for (int i = w + 1; i < words.size(); i++) {
                if (words[i] == name) {
                    words.erase(words.begin() + i);
                    for (int val = value.size() - 1; val >= 0; val--) {
                        words.insert(words.begin() + i, value[val]);
                    }
                }
            }
        }
        else if (words[w] == "#include") {
            w++;
            ifstream include_file(words[w]);

            string include_text = "";
            string line;
            while (getline(include_file, line))
                include_text += line + '\n';

            vector<string> include_words = split(include_text);

            for (string word : include_words)
                words.push_back(word);

            include_file.close();
        }
        else if (words[w] == "struct") {
            w++;
            string name = words[w];
            structs.push_back(structure(name));
            w += 2;
            for (; words[w] != "}"; w++) {
                if (words[w] == "num") {
                    w++;
                    structs[structs.size() - 1].vars.push_back(words[w]);
                }
            }
        }
    }
    w++;
    bool run = true;

    for (; w < words.size() && run; w++) {
        while (SDL_PollEvent(&event) == 1) {
            switch (event.type) {
                case SDL_QUIT:
                    run = false;
                    break;
                case SDL_KEYDOWN:
                    button_pressed = true;
                    switch(event.key.keysym.sym) {
                        case SDLK_UP:
                            button = '^';
                            break;
                        case SDLK_DOWN:
                            button = 'V';
                            break;
                        case SDLK_LEFT:
                            button = '<';
                            break;
                        case SDLK_RIGHT:
                            button = '>';
                            break;
                        case SDLK_w:
                            button = '^';
                            break;
                        case SDLK_s:
                            button = 'V';
                            break;
                        case SDLK_a:
                            button = '<';
                            break;
                        case SDLK_d:
                            button = '>';
                            break;
                        case SDLK_j:
                            button = 'A';
                            break;
                        case SDLK_k:
                            button = 'B';
                            break;
                        default:
                            button = '.';
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    button_pressed = false;
                default:
                    break;
            }
        }
        if (words[w] == "num") {
            used_vars[used_vars.size() - 1]++;
            w++;
            string name = words[w];
            if (words[w + 1] == "=") {
                vector<string> insides;
                w += 2;
                for (; words[w] != "\n" && words[w] != "|"; w++)
                    insides.push_back(words[w]);
                vars.push_back(variable(name, math(insides)));
            }
            else
                vars.push_back(variable(name));
        }
        else if (words[w] == "nums") {
            used_arrs[used_arrs.size() - 1]++;
            w++;
            string name = words[w];
            w += 2;
            if (words[w] == "]") {
                w++;
                if (words[w] == "=") {
                    w++;
                    vector<float> insides;
                    if (words[w] == "{") {
                        w++;
                        vector<string> nums;
                        for (; words[w] != "}"; w++) {
                            if (words[w] != ",")
                                nums.push_back(words[w]);
                            else {
                                insides.push_back(math(nums));
                                for (int i = 0; i < nums.size(); i++)
                                    nums.pop_back();
                            }
                            if (words[w + 1] == "}")
                                insides.push_back(math(nums));
                        }
                    }
                    arrs.push_back(arr(name, insides));
                }
            }
            else {
                int brackets = 1;
                vector<string> insides;
                for (; brackets != 0; w++) {
                    if (words[w] == "[")
                        brackets++;
                    else if (words[w] == "]")
                        brackets--;
                    if (brackets != 0)
                        insides.push_back(words[w]);
                }
                w--;
                int size = math(insides);
                if (words[w + 1] == "=") {
                    w += 2;
                    vector<float> insides;
                    if (words[w] == "{") {
                        w++;
                        vector<string> nums;
                        for (; words[w] != "}"; w++) {
                            if (words[w] != ",")
                                nums.push_back(words[w]);
                            else {
                                insides.push_back(math(nums));
                                for (int i = 0; i < nums.size(); i++)
                                    nums.pop_back();
                            }
                            if (words[w + 1] == "}")
                                insides.push_back(math(nums));
                        }
                    }
                    while (insides.size() > size)
                        insides.pop_back();
                    arrs.push_back(arr(name, insides));
                }
                else {
                    vector<float> a;
                    for (int i = 0; i < size; i++)
                        a.push_back(0);
                    arrs.push_back(arr(name, a));
                }
            }
        }
        else if (words[w] == "str") {
            used_arrs[used_arrs.size() - 1]++;
            w++;
            string name = words[w];
            w += 2;
            string s;
            if (words[w] == "input")
                getline(cin, s);
            else {
                for (int i = 1; i < words[w].size() - 1; i++)
                    s += words[w][i];
            }
            vector<float> insides;
            for (int i = 0; i < s.size(); i++)
                insides.push_back(float(s[i]));
            arrs.push_back(arr(name, insides));
        }
        else if (words[w] == "struct") {
            w++;
            string struct_name = words[w];
            w++;
            string var_name = words[w];
            create_struct(struct_name, var_name);
        }
        else if (words[w] == ":") {
            w++;
            string name = words[w];
            string sign;
            w++;
            if (words[w] == "[") {
                w++;
                int brackets = 1;
                vector<string> insides;
                for (; brackets != 0; w++) {
                    if (words[w] == "[")
                        brackets++;
                    if (words[w] == "]")
                        brackets--;
                    if (brackets != 0)
                        insides.push_back(words[w]);
                }
                sign = words[w];
                int el = math(insides);
                while (insides.size() > 0)
                    insides.pop_back();
                w++;
                for (; words[w] != "\n" && words[w] != "|"; w++)
                    insides.push_back(words[w]);
                if (sign == "=")
                    arrs[find(name)].value[el] = math(insides);
                else if (sign == "+=")
                    arrs[find(name)].value[el] += math(insides);
                else if (sign == "-=")
                    arrs[find(name)].value[el] -= math(insides);
                else if (sign == "*=")
                    arrs[find(name)].value[el] *= math(insides);
                else if (sign == "/=")
                    arrs[find(name)].value[el] /= math(insides);
            }
            else {
                sign = words[w];
                w++;
                vector<string> insides;
                for (; words[w] != "\n" && words[w] != "|"; w++)
                    insides.push_back(words[w]);
                if (sign == "=")
                    vars[find(name)].value = math(insides);
                else if (sign == "+=")
                    vars[find(name)].value += math(insides);
                else if (sign == "-=")
                    vars[find(name)].value -= math(insides);
                else if (sign == "*=")
                    vars[find(name)].value *= math(insides);
                else if (sign == "/=")
                    vars[find(name)].value /= math(insides);
            }
        }
        else if (words[w] == "print") {
            w++;
            int_vec2 place;
            vector<string> NUMS;
            for (; words[w] != ","; w++) {
                NUMS.push_back(words[w]);
            }
            place.x = math(NUMS);
            while (NUMS.size() > 0)
                NUMS.pop_back();
            w++;
            for (; words[w] != ","; w++) {
                NUMS.push_back(words[w]);
            }
            place.y = math(NUMS);
            w++;
            string str = "";
            for (;words[w] != "\n" && words[w] != "|" && w < words.size(); w++) {
                if (words[w][0] == '"') {
                    for (int i = 1; i < words[w].size() - 1; i++)
                        str += words[w][i];
                }
                else if (words[w] == "num") {
                    w++;
                    vector<string> nums;
                    for (; words[w] != "," && words[w] != "\n"; w++)
                        nums.push_back(words[w]);
                    str += to_str(math(nums));
                    w--;
                }
                else if (words[w] == "char") {
                    w++;
                    vector<string> nums;
                    for (; words[w] != "," && words[w] != "\n" && words[w] != "|"; w++)
                        nums.push_back(words[w]);
                    str += (char)(int)math(nums);
                    w--;
                }
                else if (words[w] == "str") {
                    w++;
                    int id = find(words[w]);
                    for (float letter : arrs[id].value)
                        str += (char)letter;
                }
            }
            for (int i = 0; i < str.size(); i++) {
                DrawChar(renderer, str[i], font, place.x + i * NewCharSpace, place.y, int_vec3(255, 255, 255), rect, texture);
            }
        }
        else if (words[w] == "log") {
            w++;
            string str = "";
            for (;words[w] != "\n" && words[w] != "|" && w < words.size(); w++) {
                if (words[w][0] == '"') {
                    for (int i = 1; i < words[w].size() - 1; i++)
                        str += words[w][i];
                }
                else if (words[w] == "num") {
                    w++;
                    vector<string> nums;
                    for (; words[w] != "," && words[w] != "\n" && words[w] != "|"; w++)
                        nums.push_back(words[w]);
                    str += to_str(math(nums));
                    w--;
                }
                else if (words[w] == "char") {
                    w++;
                    vector<string> nums;
                    for (; words[w] != "," && words[w] != "\n" && words[w] != "|"; w++)
                        nums.push_back(words[w]);
                    str += (char)(int)math(nums);
                    w--;
                }
                else if (words[w] == "str") {
                    w++;
                    int id = find(words[w]);
                    for (float letter : arrs[id].value)
                        str += (char)letter;
                }
            }
            cout << str << endl;
            for (variable v : vars) {
                cout << v.name << " = " << v.value << endl;
            }
        }
        else if (words[w] == "input") {
            w++;
            string name = words[w];
            if (words[w] == "[") {
                w++;
                int brackets = 1;
                vector<string> insides;
                for (; brackets != 0; w++) {
                    if (words[w] == "[")
                        brackets++;
                    if (words[w] == "]")
                        brackets--;
                    if (brackets != 0)
                        insides.push_back(words[w]);
                }
                int el = math(insides);
                arrs[find(name)].value[el] = button;
            }
            else {
                vars[find(name)].value = button;
            }
        }
        else if (words[w] == "is_input") {
            w++;
            string name = words[w];
            if (words[w] == "[") {
                w++;
                int brackets = 1;
                vector<string> insides;
                for (; brackets != 0; w++) {
                    if (words[w] == "[")
                        brackets++;
                    if (words[w] == "]")
                        brackets--;
                    if (brackets != 0)
                        insides.push_back(words[w]);
                }
                int el = math(insides);
                arrs[find(name)].value[el] = button_pressed;
            }
            else {
                vars[find(name)].value = button_pressed;
            }
        }
        else if (words[w] == "render")
            SDL_RenderPresent(renderer);
        else if (words[w] == "clear")
            SDL_RenderClear(renderer);
        else if (words[w] == "color") {
            w++;
            int_vec3 color;
            vector<string> nums;
            for (; words[w] != "," && words[w] != "\n" && words[w] != "|"; w++)
                nums.push_back(words[w]);
            color.x = math(nums);
            while (nums.size() > 0)
                nums.pop_back();
            w++;
            for (; words[w] != "," && words[w] != "\n" && words[w] != "|"; w++)
                nums.push_back(words[w]);
            color.y = math(nums);
            while (nums.size() > 0)
                nums.pop_back();
            w++;
            for (; words[w] != "," && words[w] != "\n" && words[w] != "|"; w++)
                nums.push_back(words[w]);
            color.z = math(nums);
            SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 0);
        }
        else if (words[w] == "putpix") {
            w++;
            int_vec2 place;
            vector<string> nums;
            for (; words[w] != "," && words[w] != "\n" && words[w] != "|"; w++)
                nums.push_back(words[w]);
            place.x = math(nums);
            while (nums.size() > 0)
                nums.pop_back();
            w++;
            for (; words[w] != "," && words[w] != "\n" && words[w] != "|"; w++)
                nums.push_back(words[w]);
            place.y = math(nums);
            SDL_RenderDrawPoint(renderer, place.x, place.y);
        }
        else if (words[w] == "putline") {
            w++;
            int_vec2 place1, place2;
            vector<string> nums;
            for (; words[w] != "," && words[w] != "\n" && words[w] != "|"; w++)
                nums.push_back(words[w]);
            place1.x = math(nums);
            while (nums.size() > 0)
                nums.pop_back();
            w++;
            for (; words[w] != "," && words[w] != "\n" && words[w] != "|"; w++)
                nums.push_back(words[w]);
            place1.y = math(nums);
            while (nums.size() > 0)
                nums.pop_back();
            w++;
            for (; words[w] != "," && words[w] != "\n"; w++)
                nums.push_back(words[w]);
            place2.x = math(nums);
            while (nums.size() > 0)
                nums.pop_back();
            w++;
            for (; words[w] != "," && words[w] != "\n" && words[w] != "|"; w++)
                nums.push_back(words[w]);
            place2.y = math(nums);
            SDL_RenderDrawLine(renderer, place1.x, place1.y, place2.x, place2.x);
        }
        else if (words[w] == "if") {
            w++;
            vector<string> insides;
            for (; words[w] != "\n" && words[w] != "|" && words[w] != "=>"; w++)
                insides.push_back(words[w]);
            if (words[w] == "\n" || words[w] == "|")
                w++;
            if (math(insides) <= 0) {
                int brackets = 1;
                w++;
                for (; brackets != 0; w++) {
                    if (words[w] == "=>")
                        brackets++;
                    else if (words[w] == "end")
                        brackets--;
                }
            }
        }
        else if (words[w] == "end") {
            int end = w;
            int brackets = 1;
            w--;
            for (; brackets != 0; w--) {
                if (words[w] == "end")
                    brackets++;
                else if (words[w] == "=>")
                    brackets--;
            }
            for (; words[w] != "while" && words[w] != "if"; w--);
            if (words[w] == "while") {
                w++;
                vector<string> insides;
                for (; words[w] != "\n" && words[w] != "|" && words[w] != "=>"; w++)
                    insides.push_back(words[w]);
                if (math(insides) <= 0)
                    w = end;
            }
            else
                w = end;
        }
        else if (words[w] == "}") {
            for (; words[w] != "{"; w--);
            for (; words[w] != "fn"; w--);
            if (words[w + 1] == "main") {
                break;
            }
            else {
                w = call_places[call_places.size() - 1];
                call_places.pop_back();
                for (; used_vars[used_vars.size() - 1] > 0; used_vars[used_vars.size() - 1]--)
                    vars.pop_back();
                used_vars.pop_back();
                for (; used_arrs[used_arrs.size() - 1] > 0; used_arrs[used_arrs.size() - 1]--)
                    arrs.pop_back();
                used_arrs.pop_back();
            }
        }
        else if (words[w] == "@") {
            w++;
            vector<vector<string> > call_vars;
            vector<string> call_var;
            string name = words[w];
            w++;
            for (; words[w] != "\n" && words[w] != "|" && words[w] != "@"; w++) {
                call_var.push_back(words[w]);
                if (words[w] == ",") {
                    call_var.pop_back();
                    call_vars.push_back(call_var);
                    while (call_var.size() > 0)
                        call_var.pop_back();
                }
            }
            call_vars.push_back(call_var);
            call_places.push_back(w);
            used_vars.push_back(0);
            used_arrs.push_back(0);
            for (; !(words[w - 1] == "fn" && words[w] == name); w++);
            w++;
            for (; words[w] != "{"; w++) {
                if (words[w] == "num") {
                    vars.push_back(variable(words[w + 1], math(call_vars[0])));
                    used_vars[used_vars.size() - 1]++;
                    call_vars.erase(call_vars.begin());
                }  
                else if (words[w] == "nums") {
                    arrs.push_back(arr(words[w + 1], arrs[find(call_vars[0][0])].value));
                    used_arrs[used_arrs.size() - 1]++;
                    call_vars.erase(call_vars.begin());
                }
            }
        }
        else if (words[w] == "return") {
            vector<string> insides;
            w++;
            if (words[w] == "num") {
                w++;
                for (; words[w] != "\n" && words[w] != "|"; w++)
                    insides.push_back(words[w]);
                returns.push_back(math(insides));
            }
            else if (words[w] == "nums") {
                w++;
                returns_arr.push_back(arrs[find(words[w])].value);
            }
            for (int i = 0; i < used_vars[used_vars.size() - 1]; i++)
                vars.pop_back();
            for (int i = 0; i < used_arrs[used_arrs.size() - 1]; i++)
                arrs.pop_back();
            used_vars.pop_back();
            used_arrs.pop_back();
            w = call_places[call_places.size() - 1];
            call_places.pop_back();
        }
        else if (words[w] == "->") {
            w++;
            if (words[w + 1] == "[") {
                string name = words[w];
                vector<string> insides;
                w+= 2;
                int brackets = 1;
                for (; brackets != 0; w++) {
                    if (words[w] == "[")
                        brackets++;
                    else if (words[w] == "]")
                        brackets--;
                    if (brackets != 0)
                        insides.push_back(words[w]);
                }
                arrs[find(name)].value[(int)math(insides)] = returns[returns.size() - 1];
                returns.pop_back();
            }
            else {
                vars[find(words[w])].value = returns[returns.size() - 1];
                returns.pop_back();
            }
        }
        else if (words[w] == "->>") {
            w++;
            int size = arrs[find(words[w])].value.size();
            for (int i = 0; i < size; i++)
                arrs[find(words[w])].value[i] = returns_arr[returns_arr.size() - 1][i];
            returns_arr.pop_back();
        }
    }
    while (vars.size() > 0)
        vars.pop_back();
    while (structs.size() > 0)
        structs.pop_back();
    while (arrs.size() > 0)
        arrs.pop_back();
    SDL_RenderSetLogicalSize(renderer, Width, Height);
}

float math(vector<string> nums) {
    vector<string> signs, numbers;
    for (int i = 0; i < nums.size(); i++) {
        if (nums[i] == "[") {
            int start = i, end;
            int brackets = 1;
            i++;
            vector<string> insides;
            for (; brackets != 0; i++) {
                if (nums[i] == "[")
                    brackets++;
                else if (nums[i] == "]")
                    brackets--;
                if (brackets != 0)
                    insides.push_back(nums[i]);
            }
            end = i;
            for (int j = 0; j < end - start; j++)
                nums.erase(nums.begin() + start);
            nums[start - 1] = to_string(arrs[find(nums[start - 1])].value[math(insides)]);
        }
    }
    for (int i = 0; i < nums.size(); i++) {
        if (nums[i] == "(") {
            int start = i, end;
            int brackets = 1;
            vector<string> insides;
            i++;
            for (; brackets != 0; i++) {
                if (nums[i] == "(")
                    brackets++;
                else if (nums[i] == ")")
                    brackets--;
                if (brackets != 0)
                    insides.push_back(nums[i]);
            }
            end = i;
            for (int j = 1; j < end-start; j++) {
                nums.erase(nums.begin() + start + 1);
                i--;
            }
            nums[start] = to_string(math(insides));
        }
    }
    for (int i = 0; i < nums.size(); i++) {
        if (i % 2 == 0)
            numbers.push_back(nums[i]);
        else
            signs.push_back(nums[i]);
    }
    for (int i = 0; i < signs.size(); i++) {
        if (signs[i] == "%") {
            signs.erase(signs.begin() + i);
            int num1, num2;
            num1 = find(numbers[i]);
            num2 = find(numbers[i + 1]);
            num1 = (num1 == -1) ? stoi(numbers[i]) : (int)vars[num1].value;
            num2 = (num2 == -1) ? stoi(numbers[i + 1]) : (int)vars[num2].value;
            float res = num1 * num2;
            numbers.erase(numbers.begin() + i);
            numbers[i] = to_string(res);
            i--;
        }
    }
    for (int i = 0; i < signs.size(); i++) {
        if (signs[i] == "*") {
            signs.erase(signs.begin() + i);
            float num1, num2;
            num1 = find(numbers[i]);
            num2 = find(numbers[i + 1]);
            num1 = (num1 == -1) ? stof(numbers[i]) : vars[(int)num1].value;
            num2 = (num2 == -1) ? stof(numbers[i + 1]) : vars[(int)num2].value;
            float res = num1 * num2;
            numbers.erase(numbers.begin() + i);
            numbers[i] = to_string(res);
            i--;
        }
        else if (signs[i] == "/") {
            signs.erase(signs.begin() + i);
            float num1, num2;
            num1 = find(numbers[i]);
            num2 = find(numbers[i + 1]);
            num1 = (num1 == -1) ? stof(numbers[i]) : vars[(int)num1].value;
            num2 = (num2 == -1) ? stof(numbers[i + 1]) : vars[(int)num2].value;
            float res = num1 / num2;
            numbers.erase(numbers.begin() + i);
            numbers[i] = to_string(res);
            i--;
        }
    }
    for (int i = 0; i < signs.size(); i++) {
        if (signs[i] == "+") {
            signs.erase(signs.begin() + i);
            float num1, num2;
            num1 = find(numbers[i]);
            num2 = find(numbers[i + 1]);
            num1 = (num1 == -1) ? stof(numbers[i]) : vars[(int)num1].value;
            num2 = (num2 == -1) ? stof(numbers[i + 1]) : vars[(int)num2].value;
            float res = num1 + num2;
            numbers.erase(numbers.begin() + i);
            numbers[i] = to_string(res);
            i--;
        }
        else if (signs[i] == "-") {
            signs.erase(signs.begin() + i);
            float num1, num2;
            num1 = find(numbers[i]);
            num2 = find(numbers[i + 1]);
            num1 = (num1 == -1) ? stof(numbers[i]) : vars[(int)num1].value;
            num2 = (num2 == -1) ? stof(numbers[i + 1]) : vars[(int)num2].value;
            float res = num1 - num2;
            numbers.erase(numbers.begin() + i);
            numbers[i] = to_string(res);
            i--;
        }
    }
    for (int i = 0; i < signs.size(); i++) {
        if (signs[i] == "==") {
            signs.erase(signs.begin() + i);
            float num1, num2;
            num1 = find(numbers[i]);
            num2 = find(numbers[i + 1]);
            num1 = (num1 == -1) ? stof(numbers[i]) : vars[(int)num1].value;
            num2 = (num2 == -1) ? stof(numbers[i + 1]) : vars[(int)num2].value;
            float res = num1 == num2;
            numbers.erase(numbers.begin() + i);
            numbers[i] = to_string(res);
            i--;
        }
        else if (signs[i] == "<") {
            signs.erase(signs.begin() + i);
            float num1, num2;
            num1 = find(numbers[i]);
            num2 = find(numbers[i + 1]);
            num1 = (num1 == -1) ? stof(numbers[i]) : vars[(int)num1].value;
            num2 = (num2 == -1) ? stof(numbers[i + 1]) : vars[(int)num2].value;
            float res = num1 < num2;
            numbers.erase(numbers.begin() + i);
            numbers[i] = to_string(res);
            i--;
        }
        else if (signs[i] == ">") {
            signs.erase(signs.begin() + i);
            float num1, num2;
            num1 = find(numbers[i]);
            num2 = find(numbers[i + 1]);
            num1 = (num1 == -1) ? stof(numbers[i]) : vars[(int)num1].value;
            num2 = (num2 == -1) ? stof(numbers[i + 1]) : vars[(int)num2].value;
            float res = num1 > num2;
            numbers.erase(numbers.begin() + i);
            numbers[i] = to_string(res);
            i--;
        }
        else if (signs[i] == "<=") {
            signs.erase(signs.begin() + i);
            float num1, num2;
            num1 = find(numbers[i]);
            num2 = find(numbers[i + 1]);
            num1 = (num1 == -1) ? stof(numbers[i]) : vars[(int)num1].value;
            num2 = (num2 == -1) ? stof(numbers[i + 1]) : vars[(int)num2].value;
            float res = num1 <= num2;
            numbers.erase(numbers.begin() + i);
            numbers[i] = to_string(res);
            i--;
        }
        else if (signs[i] == ">=") {
            signs.erase(signs.begin() + i);
            float num1, num2;
            num1 = find(numbers[i]);
            num2 = find(numbers[i + 1]);
            num1 = (num1 == -1) ? stof(numbers[i]) : vars[(int)num1].value;
            num2 = (num2 == -1) ? stof(numbers[i + 1]) : vars[(int)num2].value;
            float res = num1 >= num2;
            numbers.erase(numbers.begin() + i);
            numbers[i] = to_string(res);
            i--;
        }
    }
    for (int i = 0; i < signs.size(); i++) {
        if (signs[i] == "and") {
            signs.erase(signs.begin() + i);
            float num1, num2;
            num1 = find(numbers[i]);
            num2 = find(numbers[i + 1]);
            num1 = (num1 == -1) ? stof(numbers[i]) : vars[(int)num1].value;
            num2 = (num2 == -1) ? stof(numbers[i + 1]) : vars[(int)num2].value;
            float res = num1 && num2;
            numbers.erase(numbers.begin() + i);
            numbers[i] = to_string(res);
            i--;
        }
        if (signs[i] == "or") {
            signs.erase(signs.begin() + i);
            float num1, num2;
            num1 = find(numbers[i]);
            num2 = find(numbers[i + 1]);
            num1 = (num1 == -1) ? stof(numbers[i]) : vars[(int)num1].value;
            num2 = (num2 == -1) ? stof(numbers[i + 1]) : vars[(int)num2].value;
            float res = num1 || num2;
            numbers.erase(numbers.begin() + i);
            numbers[i] = to_string(res);
            i--;
        }
    }
    float ret = (find(numbers[0]) == -1) ? stof(numbers[0]) : vars[find(numbers[0])].value;
    return ret;
}

vector<string> split(string str) {
    string word;
    vector<string> words;
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '/' && str[i + 1] == '/') {
            for (; str[i] != '\n'; i++);
        }
        if (str[i] == '\n' || str[i] == ',' || 
            str[i] == '['  || str[i] == '{' ||
            str[i] == ']'  || str[i] == '}' ||
            str[i] == '('  || str[i] == ')' ||
            str[i] == '|'  || str[i] == ':') {
            if (word != "")
                words.push_back(word);
            word = "";
            word += str[i];
            words.push_back(word);
            word = "";
        }
        else if (str[i] == '"') {
            word += str[i];
            i++;
            while (str[i] != '"') {
                if (str[i] == '\\') {
                    i++;
                    if (str[i] == 'n')
                        word += "\n";
                }
                else
                    word += str[i];
                i++;
            }
            word += str[i];
            words.push_back(word);
            word = "";
        }
        else if (str[i] == '\'') {
            i++;
            words.push_back(to_str(str[i]));
            i++;
        }
        else if (str[i] == ' ' || str[i] == '\t' || i == str.size() - 1) {
            if (word != "")
                words.push_back(word);
            word = "";
        }
        else {
            word += str[i];
        }
    }
    return words;
}

int find(string var_name) {
    for (int i = vars.size() - 1; i >= 0; i--) {
        if (var_name == vars[i].name)
            return i;
    }
    for (int i = arrs.size() - 1; i >= 0; i--) {
        if (var_name == arrs[i].name)
            return i;
    }
    return -1;
}

string to_str(float num) {
    string ret = to_string(num);
    for (int i = 0; i < 6; i++) {
        if (ret[ret.size() - 1] == '0') {
            string s = "";
            for (int j = 0; j < ret.size() - 1; j++)
                s += ret[j];
            ret = s;
        }
        else
            break;
    }
    if (ret[ret.size() - 1] == '.') {
        string s = "";
        for (int j = 0; j < ret.size() - 1; j++)
            s += ret[j];
        ret = s;
    }
    return ret;
}

void create_struct(string struct_name, string var_name) {
    for (int i = 0; i < structs.size(); i++) {
        if (structs[i].name == struct_name) {
            for (int j = 0; j < structs[i].vars.size(); j++)
                vars.push_back(variable(var_name + "." + structs[i].vars[j]));
            return;
        }
    }
}