// g++ alphacolourblend.cpp -o alphacolourblend.exe -lgdi32

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdint>
#include <windows.h>

struct RGBA { int r, g, b, a; };
struct Fragment { RGBA colour; int mask; };


uint32_t div255_round(uint32_t x){
    uint32_t t = x + 128u;
    return (t + (t >> 8)) >> 8;
}

uint32_t blend(uint32_t top, uint32_t bottom, uint32_t ta, uint32_t ba){
    uint32_t bp = div255_round(bottom * ba);
    return div255_round(top * ta + bp * (255u - ta));
}

RGBA over(RGBA t, RGBA b){
    RGBA o;
    o.r = blend(t.r, b.r, t.a, b.a);
    o.g = blend(t.g, b.g, t.a, b.a);
    o.b = blend(t.b, b.b, t.a, b.a);
    o.a = t.a + div255_round((uint32_t)b.a * (255u - t.a));
    return o;
}

RGBA resolve(RGBA bg, Fragment A, Fragment B){
    uint32_t sr = 0, sg = 0, sb = 0, sa = 0;
    for (int s = 0; s < 4; s++) {
        RGBA c = bg;
        if (A.mask & (1 << s)) c = over(A.colour, c);
        if (B.mask & (1 << s)) c = over(B.colour, c);
        sr += c.r; sg += c.g; sb += c.b; sa += c.a;
    }
    return { (int)((sr + 2) / 4), (int)((sg + 2) / 4),
             (int)((sb + 2) / 4), (int)((sa + 2) / 4) };
}


std::vector<std::string> split(std::string s, const std::string& splitter){
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(splitter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + splitter.length());
    }
    tokens.push_back(s);
    return tokens;
}

int parseMask(const std::string& m){
    int mask = 0;
    for (int s = 0; s < 4; s++)
        if (m[s] == '1') mask |= (1 << s);
    return mask;
}

bool validMask(const std::string& m){
    if (m.length() != 4) return false;
    for (char c : m)
        if (c != '0' && c != '1') return false;
    return true;
}

bool validRGBA(const std::vector<std::string>& tokens){
    if (tokens.size() != 4) return false;
    for (const std::string& t : tokens) {
        try {
            int v = std::stoi(t);
            if (v < 0 || v > 255) return false;
        } catch (...) {
            return false;
        }
    }
    return true;
}

RGBA parseRGBA(const std::vector<std::string>& tokens){
    return { std::stoi(tokens[0]),
             std::stoi(tokens[1]),
             std::stoi(tokens[2]),
             std::stoi(tokens[3]) };
}

static RGBA g_bg, g_a, g_b, g_avg;

static COLORREF cref(const RGBA& c){ return RGB(c.r, c.g, c.b); }
static COLORREF contrastText(const RGBA& c){
    int lum = (c.r*299 + c.g*587 + c.b*114) / 1000;
    return lum < 128 ? RGB(255,255,255) : RGB(0,0,0);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp){
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            SetBkMode(hdc, TRANSPARENT);

            RECT rcB = { 70,  60, 340, 300 };
            RECT rcA = { 230, 150, 500, 390 };

            auto fill = [&](const RECT& r, const RGBA& c){
                HBRUSH br = CreateSolidBrush(cref(c));
                FillRect(hdc, &r, br);
                DeleteObject(br);
            };

            RECT client;
            GetClientRect(hwnd, &client);
            fill(client, g_bg);
            auto label = [&](RECT r, const RGBA& shown, const RGBA& vals, const char* name){
                SetTextColor(hdc, contrastText(shown));
                std::string s = std::string(name) + "\n"
                    + std::to_string(vals.r) + "," + std::to_string(vals.g) + ","
                    + std::to_string(vals.b) + "," + std::to_string(vals.a);
                DrawTextA(hdc, s.c_str(), -1, &r, DT_CENTER | DT_TOP);
            };

            RECT ov;
            BOOL hasOverlap = IntersectRect(&ov, &rcA, &rcB);

            RGBA aOnBg = over(g_a, g_bg);
            RGBA bOnBg = over(g_b, g_bg);

            fill(rcB, bOnBg);
            fill(rcA, aOnBg);
            if (hasOverlap) fill(ov, g_avg);

            RECT bl = { rcB.left, rcB.top + 6, rcB.right, hasOverlap ? ov.top : rcB.bottom };
            label(bl, bOnBg, g_b, "B (over)");

            RECT al = { rcA.left, hasOverlap ? ov.bottom : rcA.top, rcA.right, rcA.bottom };
            label(al, aOnBg, g_a, "A (under)");

            if (hasOverlap) { RECT ol = ov; ol.top += 6; label(ol, g_avg, g_avg, "Resolved"); }

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

void showOverlap(const RGBA& bg, const RGBA& a, const RGBA& b, const RGBA& avg){
    g_bg = bg; g_a = a; g_b = b; g_avg = avg;

    HINSTANCE hInst = GetModuleHandle(NULL);
    const char* cls = "BlendWindow";
    static bool reg = false;
    if (!reg) {
        WNDCLASSA wc = {};
        wc.lpfnWndProc   = WndProc;
        wc.hInstance     = hInst;
        wc.lpszClassName = cls;
        wc.hCursor       = LoadCursorA(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        RegisterClassA(&wc);
        reg = true;
    }
    HWND hwnd = CreateWindowExA(0, cls, "Alpha Blend", WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, CW_USEDEFAULT, 620, 480,
                                NULL, NULL, hInst, NULL);
    ShowWindow(hwnd, SW_SHOW);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

int main ()
{
    std::cout << "Enter: <Background> <A> <A mask> <B> <B mask>\n"
                 "  colours are R,G,B,A (each 0-255), masks are 4 bits TL TR BL BR (e.g. 1100).\n"
                 "  Background is opaque and covers the whole pixel. B is over A.\n"
                 "  Example: 0,0,0,255 255,0,0,255 1100 0,255,0,255 0011\n"
                 "Values: ";
    std::string str;

    while (std::getline(std::cin, str)) {
        if (str == "quit" or str == "break" or str == "exit") break;

        std::istringstream ss(str);
        std::string bg_str, a_str, a_mask, b_str, b_mask;

        if (!(ss >> bg_str >> a_str >> a_mask >> b_str >> b_mask)) {
            std::cout << "Invalid input\n";
            continue;
        }

        if (!validMask(a_mask) || !validMask(b_mask)) {
            std::cout << "Invalid mask\n";
            continue;
        }

        std::vector<std::string> bg_tokens = split(bg_str, ",");
        std::vector<std::string> a_tokens  = split(a_str,  ",");
        std::vector<std::string> b_tokens  = split(b_str,  ",");

        if (!validRGBA(bg_tokens) || !validRGBA(a_tokens) || !validRGBA(b_tokens)) {
            std::cout << "Colour values must be 4 numbers, each 0-255\n";
            continue;
        }

        RGBA bg = parseRGBA(bg_tokens);
        Fragment A = { parseRGBA(a_tokens), parseMask(a_mask) };
        Fragment B = { parseRGBA(b_tokens), parseMask(b_mask) };

        RGBA result = resolve(bg, A, B);

        showOverlap(bg, A.colour, B.colour, result);

        std::cout << "Background: " << bg.r << " " << bg.g << " " << bg.b << " " << bg.a << "\n"
                  << "A: " << A.colour.r << " " << A.colour.g << " " << A.colour.b << " " << A.colour.a << "\n"
                  << "B: " << B.colour.r << " " << B.colour.g << " " << B.colour.b << " " << B.colour.a << "\n"
                  << "Resolved: " << result.r << " " << result.g << " " << result.b << " " << result.a << "\n"
                  << "Values: ";
    }
    return 0;
}
