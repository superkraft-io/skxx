#pragma once

#include "../sk_common.hxx"


BEGIN_SK_NAMESPACE

class SK_Color;

static inline std::unordered_map<std::string, SK_Color> SK_Colors;

enum class SK_Color_Format {
    hex,
    rgb,
    rgba,
    hsl,
    hsla,
    named,
    unknown
};

class SK_Color {
public:
    int r, g, b; // Red, Green, Blue components (0-255)
    double a;    // Alpha component (0.0-1.0)


    // Constructor to initialize default color (black)
    SK_Color() : r(0), g(0), b(0), a(1.0) {}

    // Constructor to initialize with specific color values
    SK_Color(int red, int green, int blue, double alpha = 1.0) : r(red), g(green), b(blue), a(alpha) {}

    // Constructor to initialize with specific color values
    SK_Color(const std::string& clrStr) {
        *this = clrStr; // Use the assignment operator
    }
    
    SK_Color(char* clrStr) {
        *this = std::string(clrStr);
    }

    // Assignment operator for std::string
    SK_Color& operator=(const std::string& clrStr) {
        std::optional<SK_Color> color = parse(clrStr);

        if (!color) {
            throw std::invalid_argument("Invalid color format");
        }

        // Assign the parsed color values to the current object
        r = color->r;
        g = color->g;
        b = color->b;
        a = color->a;

        return *this; // Return the current object for chaining
    }

    #if defined(SK_OS_windows)
        //for windows
    #elif defined(SK_OS_macos) || defined(SK_OS_ios)
        operator NSColor*(){
            return [NSColor
                colorWithRed:r
                       green:g
                        blue:b
                       alpha:a
            ];
        }
    #endif


    int getAlphaAs255() {
        if (a < 0.0) a = 0.0;
        if (a > 1.0) a = 1.0;
        return static_cast<int>(a * 255.0);
    }

    // Static method to parse a color string and return a SK_Color object
    static std::optional<SK_Color> parse(const SK_String& color) {
        if (auto result = parseHex(color)) return result;
        if (auto result = parseRGB(color)) return result;
        if (auto result = parseRGBA(color)) return result;
        if (auto result = parseHSL(color)) return result;
        if (auto result = parseHSLA(color)) return result;
        if (auto result = parseNamedColor(color)) return result;

        return std::nullopt; // Invalid color format
    }


    static inline SK_Color_Format detectColorFormat(const std::string& color) {
        // Hex color detection
        std::regex hexRegex("^#([0-9a-fA-F]{3,4}|[0-9a-fA-F]{6}|[0-9a-fA-F]{8})$");
        if (std::regex_match(color, hexRegex)) {
            return SK_Color_Format::hex;
        }

        // RGB color detection
        std::regex rgbRegex("^rgb\\(\\s*(\\d+)\\s*,?\\s*(\\d+)\\s*,?\\s*(\\d+)\\s*\\)$");
        if (std::regex_match(color, rgbRegex)) {
            return SK_Color_Format::rgb;
        }

        // RGBA color detection
        std::regex rgbaRegex("^rgba\\(\\s*(\\d+)\\s*,?\\s*(\\d+)\\s*,?\\s*(\\d+)\\s*,?\\s*([\\d.]+)\\s*\\)$");
        if (std::regex_match(color, rgbaRegex)) {
            return SK_Color_Format::rgba;
        }

        // HSL color detection
        std::regex hslRegex("^hsl\\(\\s*(-?[\\d.]+)\\s*(?:deg)?\\s*,?\\s*([\\d.]+)%\\s*,?\\s*([\\d.]+)%\\s*\\)$");
        if (std::regex_match(color, hslRegex)) {
            return SK_Color_Format::hsl;
        }

        // HSLA color detection
        std::regex hslaRegex("^hsla\\(\\s*(-?[\\d.]+)\\s*(?:deg)?\\s*,?\\s*([\\d.]+)%\\s*,?\\s*([\\d.]+)%\\s*,?\\s*([\\d.]+)\\s*\\)$");
        if (std::regex_match(color, hslaRegex)) {
            return SK_Color_Format::hsla;
        }

        // Named color detection
        if (SK_Colors.find(color) != SK_Colors.end()) {
            return SK_Color_Format::named;
        }

        // If no format matches
        return SK_Color_Format::unknown;
    }

    // Static helper methods for parsing specific color formats
    static inline std::optional<SK_Color> parseHex(const std::string& color) {
        std::regex hexRegex("^#([0-9a-fA-F]{3,4}|[0-9a-fA-F]{6}|[0-9a-fA-F]{8})$");
        if (!std::regex_match(color, hexRegex)) return std::nullopt;

        std::string hex = color.substr(1);
        if (hex.length() == 3 || hex.length() == 4) {
            hex = std::string() + hex[0] + hex[0] + hex[1] + hex[1] + hex[2] + hex[2];
            if (hex.length() == 4) hex += hex[3] + hex[3];
        }

        int r = std::stoi(hex.substr(0, 2), nullptr, 16);
        int g = std::stoi(hex.substr(2, 2), nullptr, 16);
        int b = std::stoi(hex.substr(4, 2), nullptr, 16);
        double a = hex.length() == 8 ? std::stoi(hex.substr(6, 2), nullptr, 16) / 255.0 : 1.0;

        return SK_Color(r, g, b, a);
    }

    static inline std::optional<SK_Color> parseRGB(const std::string& color) {
        std::regex rgbRegex("^rgb\\(\\s*(\\d+)\\s*,?\\s*(\\d+)\\s*,?\\s*(\\d+)\\s*\\)$");
        std::smatch match;
    
        if (!std::regex_match(color, match, rgbRegex)) {
            return std::nullopt;
        }

        int r = std::stoi(match[1]);
        int g = std::stoi(match[2]);
        int b = std::stoi(match[3]);

        return SK_Color(r, g, b);
    }

    static inline std::optional<SK_Color> parseRGBA(const std::string& color) {
        std::regex rgbaRegex("^rgba\\(\\s*(\\d+)\\s*,?\\s*(\\d+)\\s*,?\\s*(\\d+)\\s*,?\\s*([\\d.]+)\\s*\\)$");
        std::smatch match;

        if (!std::regex_match(color, match, rgbaRegex)) {
            return std::nullopt;
        }

        int r = std::stoi(match[1]);
        int g = std::stoi(match[2]);
        int b = std::stoi(match[3]);
        double a = std::stod(match[4]);

        return SK_Color(r, g, b, a);
    }

    static inline std::optional<SK_Color> parseHSL(const std::string& color) {
        std::regex hslRegex("^hsl\\(\\s*(-?[\\d.]+)\\s*(?:deg)?\\s*,?\\s*([\\d.]+)%\\s*,?\\s*([\\d.]+)%\\s*\\)$");
        std::smatch match;
        if (!std::regex_match(color, match, hslRegex)) return std::nullopt;

        double h = std::stod(match[1]);
        double s = std::stod(match[2]) / 100.0;
        double l = std::stod(match[3]) / 100.0;

        // Convert HSL to RGB
        double c = (1 - std::abs(2 * l - 1)) * s;
        double x = c * (1 - std::abs(std::fmod(h / 60.0, 2) - 1));
        double m = l - c / 2;

        double r_, g_, b_;
        if (h >= 0 && h < 60) {
            r_ = c; g_ = x; b_ = 0;
        }
        else if (h >= 60 && h < 120) {
            r_ = x; g_ = c; b_ = 0;
        }
        else if (h >= 120 && h < 180) {
            r_ = 0; g_ = c; b_ = x;
        }
        else if (h >= 180 && h < 240) {
            r_ = 0; g_ = x; b_ = c;
        }
        else if (h >= 240 && h < 300) {
            r_ = x; g_ = 0; b_ = c;
        }
        else {
            r_ = c; g_ = 0; b_ = x;
        }

        int r = static_cast<int>((r_ + m) * 255);
        int g = static_cast<int>((g_ + m) * 255);
        int b = static_cast<int>((b_ + m) * 255);

        return SK_Color(r, g, b);
    }

    static inline std::optional<SK_Color> parseHSLA(const std::string& color) {
        std::regex hslaRegex("^hsla\\(\\s*(-?[\\d.]+)\\s*(?:deg)?\\s*,?\\s*([\\d.]+)%\\s*,?\\s*([\\d.]+)%\\s*,?\\s*([\\d.]+)\\s*\\)$");
        std::smatch match;
        if (!std::regex_match(color, match, hslaRegex)) return std::nullopt;

        double h = std::stod(match[1]);
        double s = std::stod(match[2]) / 100.0;
        double l = std::stod(match[3]) / 100.0;
        double a = std::stod(match[4]);

        // Convert HSL to RGB
        double c = (1 - std::abs(2 * l - 1)) * s;
        double x = c * (1 - std::abs(std::fmod(h / 60.0, 2) - 1));
        double m = l - c / 2;

        double r_, g_, b_;
        if (h >= 0 && h < 60) {
            r_ = c; g_ = x; b_ = 0;
        }
        else if (h >= 60 && h < 120) {
            r_ = x; g_ = c; b_ = 0;
        }
        else if (h >= 120 && h < 180) {
            r_ = 0; g_ = c; b_ = x;
        }
        else if (h >= 180 && h < 240) {
            r_ = 0; g_ = x; b_ = c;
        }
        else if (h >= 240 && h < 300) {
            r_ = x; g_ = 0; b_ = c;
        }
        else {
            r_ = c; g_ = 0; b_ = x;
        }

        int r = static_cast<int>((r_ + m) * 255);
        int g = static_cast<int>((g_ + m) * 255);
        int b = static_cast<int>((b_ + m) * 255);

        return SK_Color(r, g, b, a);
    }

    static inline std::optional<SK_Color> parseNamedColor(const SK_String& color) {
        auto it = SK_Colors.find(color);
        if (it != SK_Colors.end()) {
            return it->second;
        }

        return std::nullopt;
    }
};




static void SK_Colors_Init(){
    SK_Colors = std::unordered_map<std::string, SK_Color>{
        {"transparent", SK_Color(0, 0, 0, 1.0)},
        {"aliceblue", SK_Color(0xf0, 0xf8, 0xff)},
        {"antiquewhite", SK_Color(0xfa, 0xeb, 0xd7)},
        {"aqua", SK_Color(0x00, 0xff, 0xff)},
        {"aquamarine", SK_Color(0x7f, 0xff, 0xd4)},
        {"azure", SK_Color(0xf0, 0xff, 0xff)},
        {"beige", SK_Color(0xf5, 0xf5, 0xdc)},
        {"bisque", SK_Color(0xff, 0xe4, 0xc4)},
        {"black", SK_Color(0x00, 0x00, 0x00)},
        {"blanchedalmond", SK_Color(0xff, 0xeb, 0xcd)},
        {"blue", SK_Color(0x00, 0x00, 0xff)},
        {"blueviolet", SK_Color(0x8a, 0x2b, 0xe2)},
        {"brown", SK_Color(0xa5, 0x2a, 0x2a)},
        {"burlywood", SK_Color(0xde, 0xb8, 0x87)},
        {"cadetblue", SK_Color(0x5f, 0x9e, 0xa0)},
        {"chartreuse", SK_Color(0x7f, 0xff, 0x00)},
        {"chocolate", SK_Color(0xd2, 0x69, 0x1e)},
        {"coral", SK_Color(0xff, 0x7f, 0x50)},
        {"cornflowerblue", SK_Color(0x64, 0x95, 0xed)},
        {"cornsilk", SK_Color(0xff, 0xf8, 0xdc)},
        {"crimson", SK_Color(0xdc, 0x14, 0x3c)},
        {"cyan", SK_Color(0x00, 0xff, 0xff)},
        {"darkblue", SK_Color(0x00, 0x00, 0x8b)},
        {"darkcyan", SK_Color(0x00, 0x8b, 0x8b)},
        {"darkgoldenrod", SK_Color(0xb8, 0x86, 0x0b)},
        {"darkgray", SK_Color(0xa9, 0xa9, 0xa9)},
        {"darkgreen", SK_Color(0x00, 0x64, 0x00)},
        {"darkkhaki", SK_Color(0xbd, 0xb7, 0x6b)},
        {"darkmagenta", SK_Color(0x8b, 0x00, 0x8b)},
        {"darkolivegreen", SK_Color(0x55, 0x6b, 0x2f)},
        {"darkorange", SK_Color(0xff, 0x8c, 0x00)},
        {"darkorchid", SK_Color(0x99, 0x32, 0xcc)},
        {"darkred", SK_Color(0x8b, 0x00, 0x00)},
        {"darksalmon", SK_Color(0xe9, 0x96, 0x7a)},
        {"darkseagreen", SK_Color(0x8f, 0xbc, 0x8f)},
        {"darkslateblue", SK_Color(0x48, 0x3d, 0x8b)},
        {"darkslategray", SK_Color(0x2f, 0x4f, 0x4f)},
        {"darkturquoise", SK_Color(0x00, 0xce, 0xd1)},
        {"darkviolet", SK_Color(0x94, 0x00, 0xd3)},
        {"deeppink", SK_Color(0xff, 0x14, 0x93)},
        {"deepskyblue", SK_Color(0x00, 0xbf, 0xff)},
        {"dimgray", SK_Color(0x69, 0x69, 0x69)},
        {"dodgerblue", SK_Color(0x1e, 0x90, 0xff)},
        {"firebrick", SK_Color(0xb2, 0x22, 0x22)},
        {"floralwhite", SK_Color(0xff, 0xfa, 0xf0)},
        {"forestgreen", SK_Color(0x22, 0x8b, 0x22)},
        {"fuchsia", SK_Color(0xff, 0x00, 0xff)},
        {"gainsboro", SK_Color(0xdc, 0xdc, 0xdc)},
        {"ghostwhite", SK_Color(0xf8, 0xf8, 0xff)},
        {"gold", SK_Color(0xff, 0xd7, 0x00)},
        {"goldenrod", SK_Color(0xda, 0xa5, 0x20)},
        {"gray", SK_Color(0x80, 0x80, 0x80)},
        {"green", SK_Color(0x00, 0x80, 0x00)},
        {"greenyellow", SK_Color(0xad, 0xff, 0x2f)},
        {"honeydew", SK_Color(0xf0, 0xff, 0xf0)},
        {"hotpink", SK_Color(0xff, 0x69, 0xb4)},
        {"indianred", SK_Color(0xcd, 0x5c, 0x5c)},
        {"indigo", SK_Color(0x4b, 0x00, 0x82)},
        {"ivory", SK_Color(0xff, 0xff, 0xf0)},
        {"khaki", SK_Color(0xf0, 0xe6, 0x8c)},
        {"lavender", SK_Color(0xe6, 0xe6, 0xfa)},
        {"lavenderblush", SK_Color(0xff, 0xf0, 0xf5)},
        {"lawngreen", SK_Color(0x7c, 0xfc, 0x00)},
        {"lemonchiffon", SK_Color(0xff, 0xfa, 0xcd)},
        {"lightblue", SK_Color(0xad, 0xd8, 0xe6)},
        {"lightcoral", SK_Color(0xf0, 0x80, 0x80)},
        {"lightcyan", SK_Color(0xe0, 0xff, 0xff)},
        {"lightgoldenrodyellow", SK_Color(0xfa, 0xfa, 0xd2)},
        {"lightgreen", SK_Color(0x90, 0xee, 0x90)},
        {"lightgrey", SK_Color(0xd3, 0xd3, 0xd3)},
        {"lightpink", SK_Color(0xff, 0xb6, 0xc1)},
        {"lightsalmon", SK_Color(0xff, 0xa0, 0x7a)},
        {"lightseagreen", SK_Color(0x20, 0xb2, 0xaa)},
        {"lightskyblue", SK_Color(0x87, 0xce, 0xfa)},
        {"lightslategray", SK_Color(0x77, 0x88, 0x99)},
        {"lightsteelblue", SK_Color(0xb0, 0xc4, 0xde)},
        {"lightyellow", SK_Color(0xff, 0xff, 0xe0)},
        {"lime", SK_Color(0x00, 0xff, 0x00)},
        {"limegreen", SK_Color(0x32, 0xcd, 0x32)},
        {"linen", SK_Color(0xfa, 0xf0, 0xe6)},
        {"magenta", SK_Color(0xff, 0x00, 0xff)},
        {"maroon", SK_Color(0x80, 0x00, 0x00)},
        {"mediumaquamarine", SK_Color(0x66, 0xcd, 0xaa)},
        {"mediumblue", SK_Color(0x00, 0x00, 0xcd)},
        {"mediumorchid", SK_Color(0xba, 0x55, 0xd3)},
        {"mediumpurple", SK_Color(0x93, 0x70, 0xdb)},
        {"mediumseagreen", SK_Color(0x3c, 0xb3, 0x71)},
        {"mediumslateblue", SK_Color(0x7b, 0x68, 0xee)},
        {"mediumspringgreen", SK_Color(0x00, 0xfa, 0x9a)},
        {"mediumturquoise", SK_Color(0x48, 0xd1, 0xcc)},
        {"mediumvioletred", SK_Color(0xc7, 0x15, 0x85)},
        {"midnightblue", SK_Color(0x19, 0x19, 0x70)},
        {"mintcream", SK_Color(0xf5, 0xff, 0xfa)},
        {"mistyrose", SK_Color(0xff, 0xe4, 0xe1)},
        {"moccasin", SK_Color(0xff, 0xe4, 0xb5)},
        {"navajowhite", SK_Color(0xff, 0xde, 0xad)},
        {"navy", SK_Color(0x00, 0x00, 0x80)},
        {"oldlace", SK_Color(0xfd, 0xf5, 0xe6)},
        {"olive", SK_Color(0x80, 0x80, 0x00)},
        {"olivedrab", SK_Color(0x6b, 0x8e, 0x23)},
        {"orange", SK_Color(0xff, 0xa5, 0x00)},
        {"orangered", SK_Color(0xff, 0x45, 0x00)},
        {"orchid", SK_Color(0xda, 0x70, 0xd6)},
        {"palegoldenrod", SK_Color(0xee, 0xe8, 0xaa)},
        {"palegreen", SK_Color(0x98, 0xfb, 0x98)},
        {"paleturquoise", SK_Color(0xaf, 0xee, 0xee)},
        {"palevioletred", SK_Color(0xdb, 0x70, 0x93)},
        {"papayawhip", SK_Color(0xff, 0xef, 0xd5)},
        {"peachpuff", SK_Color(0xff, 0xda, 0xb9)},
        {"peru", SK_Color(0xcd, 0x85, 0x3f)},
        {"pink", SK_Color(0xff, 0xc0, 0xcb)},
        {"plum", SK_Color(0xdd, 0xa0, 0xdd)},
        {"powderblue", SK_Color(0xb0, 0xe0, 0xe6)},
        {"purple", SK_Color(0x80, 0x00, 0x80)},
        {"red", SK_Color(0xff, 0x00, 0x00)},
        {"rosybrown", SK_Color(0xbc, 0x8f, 0x8f)},
        {"royalblue", SK_Color(0x41, 0x69, 0xe1)},
        {"saddlebrown", SK_Color(0x8b, 0x45, 0x13)},
        {"salmon", SK_Color(0xfa, 0x80, 0x72)},
        {"sandybrown", SK_Color(0xf4, 0xa4, 0x60)},
        {"seagreen", SK_Color(0x2e, 0x8b, 0x57)},
        {"seashell", SK_Color(0xff, 0xf5, 0xee)},
        {"sienna", SK_Color(0xa0, 0x52, 0x2d)},
        {"silver", SK_Color(0xc0, 0xc0, 0xc0)},
        {"skyblue", SK_Color(0x87, 0xce, 0xeb)},
        {"slateblue", SK_Color(0x6a, 0x5a, 0xcd)},
        {"slategray", SK_Color(0x70, 0x80, 0x90)},
        {"snow", SK_Color(0xff, 0xfa, 0xfa)},
        {"springgreen", SK_Color(0x00, 0xff, 0x7f)},
        {"steelblue", SK_Color(0x46, 0x82, 0xb4)},
        {"tan", SK_Color(0xd2, 0xb4, 0x8c)},
        {"teal", SK_Color(0x00, 0x80, 0x80)},
        {"thistle", SK_Color(0xd8, 0xbf, 0xd8)},
        {"tomato", SK_Color(0xff, 0x63, 0x47)},
        {"turquoise", SK_Color(0x40, 0xe0, 0xd0)},
        {"violet", SK_Color(0xee, 0x82, 0xee)},
        {"wheat", SK_Color(0xf5, 0xde, 0xb3)},
        {"white", SK_Color(0xff, 0xff, 0xff)},
        {"whitesmoke", SK_Color(0xf5, 0xf5, 0xf5)},
        {"yellow", SK_Color(0xff, 0xff, 0x00)},
        {"yellowgreen", SK_Color(0x9a, 0xcd, 0x32)}
    };
};



END_SK_NAMESPACE
