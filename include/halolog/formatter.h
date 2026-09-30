//
// Created by Administrator on 2026/9/22.
//

#ifndef HALOLOG_FORMATTER_H
#define HALOLOG_FORMATTER_H

#include <vector>
#include <string>
#include <sstream>
#include <format>
#include <stdexcept>
#include <unordered_map>

#include <halolog/record.h>
namespace halolog
{
    class Formatter
    {
        enum class TokenType : int
        {
            FixedText = 0,
            Message,
            Level,
            ShortLevel,
            ThreadId,
            Date,
            FullFile,
            FileName,
            Line,
            Fun,
            LineBreak
        };

        struct Token
        {
            TokenType type;
            std::string value;
            std::string format;

            Token(TokenType t, std::string v = "", std::string f = "")
                : type(t), value(std::move(v)), format(std::move(f)) {};
        };

    public:
        explicit Formatter(const std::string &pattern)
            : mTokens(parsePattern(pattern)) {}

        std::string format(const halolog::Record &record) const
        {
            std::string result;
            for (const auto &token : mTokens)
            {
                switch (token.type)
                {
                case TokenType::FixedText:
                {
                    result += token.value;
                    break;
                }
                case TokenType::Message:
                {
                    auto msg = record.message.str();
                    result += std::vformat("{:" + token.format + "}", std::make_format_args(msg));
                    break;
                }
                case TokenType::Level:
                {
                    auto lvl = levelToString(record.level);
                    result += std::vformat("{:" + token.format + "}", std::make_format_args(lvl));
                    break;
                }
                case TokenType::ShortLevel:
                {
                    auto slvl = levelToShortString(record.level);
                    result += std::vformat("{:" + token.format + "}", std::make_format_args(slvl));
                    break;
                }
                case TokenType::ThreadId:
                {
                    std::ostringstream oss;
                    oss << record.threadId;
                    auto tid = oss.str();
                    result += std::vformat("{:" + token.format + "}", std::make_format_args(tid));
                    break;
                }
                case TokenType::Date:
                {
                    const auto date = mZone->to_local(std::chrono::floor<std::chrono::milliseconds>(record.timestamp));
                    result += std::vformat("{:" + token.format + "}", std::make_format_args(date));
                    break;
                }
                case TokenType::FileName:
                {
                    result += std::vformat("{:" + token.format + "}", std::make_format_args(record.fileName));
                    break;
                }
                case TokenType::FullFile:
                {
                    result += std::vformat("{:" + token.format + "}", std::make_format_args(record.fullFile));
                    break;
                }
                case TokenType::Line:
                {
                    result += std::vformat("{:" + token.format + "}", std::make_format_args(record.line));
                    break;
                }
                case TokenType::Fun:
                {
                    result += std::vformat("{:" + token.format + "}", std::make_format_args(record.function));
                    break;
                }
                case TokenType::LineBreak:
                {
                    result += "\n";
                    break;
                }
                default:
                    throw std::invalid_argument("unknown field type");
                }
            }
            return result;
        }

    private:
        void validateFormat(TokenType type, const std::string &fmt)
        {
            std::string fmtstr = "{:" + fmt + "}";
            try
            {
                switch (type)
                {
                case TokenType::Message:
                case TokenType::FileName:
                case TokenType::FullFile:
                case TokenType::Fun:
                case TokenType::Level:
                case TokenType::ShortLevel:
                {
                    std::string test_str("test");
                    static_cast<void>(std::vformat(fmtstr, std::make_format_args(test_str)));
                    break;
                }
                case TokenType::Line:
                {
                    uint_least32_t test_line = 0;
                    static_cast<void>(std::vformat(fmtstr, std::make_format_args(test_line)));
                    break;
                }
                case TokenType::ThreadId:
                {
                    std::ostringstream oss;
                    oss << std::this_thread::get_id();
                    auto tid = oss.str();
                    static_cast<void>(std::vformat(fmtstr, std::make_format_args(tid)));
                    break;
                }
                case TokenType::Date:
                {
                    const auto date = mZone->to_local(std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()));
                    static_cast<void>(std::vformat(fmtstr, std::make_format_args(date)));
                    break;
                }
                case TokenType::LineBreak:
                {
                    if (!fmt.empty())
                        throw std::invalid_argument("field does not accept format spec");
                    break;
                }
                default:
                    throw std::invalid_argument("unknown field type");
                }
            }
            catch (const std::format_error &e)
            {
                throw std::invalid_argument(
                    "invalid format spec ':" + fmt + "' — " + e.what());
            }
        }

        std::vector<Token> parsePattern(const std::string &pattern)
        {
            std::vector<Token> tokens;

            size_t i = 0;
            std::string tmp_fixedtext;

            while (i < pattern.size())
            {
                const char ch = pattern.at(i);
                if (ch == '{')
                {
                    if (i + 1 < pattern.size() && pattern.at(i + 1) == '{')
                    {
                        tmp_fixedtext.push_back('{');
                        i += 2;
                        continue;
                    }
                    if (!tmp_fixedtext.empty())
                    {
                        tokens.push_back(Token(TokenType::FixedText, tmp_fixedtext));
                        tmp_fixedtext.clear();
                    }
                    auto back = pattern.find("}", i + 1);
                    if (std::string::npos == back)
                    {
                        throw std::invalid_argument("failed to parse: " + pattern + ", no find { (" + std::to_string(i + 1) + ") matching }");
                    }
                    else
                    {
                        auto fstart = pattern.find(':', i + 1);
                        std::string ttype;
                        std::string tformat;
                        if (fstart != std::string::npos && fstart < back)
                        {
                            ttype = pattern.substr(i + 1, fstart - i - 1);
                            tformat = pattern.substr(fstart + 1, back - fstart - 1);
                        }
                        else
                        {
                            ttype = pattern.substr(i + 1, back - i - 1);
                        }

                        if (ttype.empty())
                        {
                            throw std::invalid_argument("empty field name in: " + pattern + " at position " + std::to_string(i + 1));
                        }

                        auto it = TokenTypeMap.find(ttype);
                        if (it == TokenTypeMap.end())
                        {
                            throw std::invalid_argument("unknown field {" + ttype + "} in: " + pattern);
                        }
                        if (!tformat.empty())
                        {
                            validateFormat(it->second, tformat);
                        }
                        tokens.push_back(Token(it->second, "", tformat));
                        i = back + 1;
                    }
                }
                else if (ch == '}')
                {
                    if (i + 1 < pattern.size() && pattern.at(i + 1) == '}')
                    {
                        tmp_fixedtext.push_back('}');
                        i += 2;
                        continue;
                    }
                    else
                    {
                        throw std::invalid_argument("failed to parse: " + pattern + ", } (" + std::to_string(i + 1) + ") as the beginning");
                    }
                }
                else
                {
                    tmp_fixedtext.push_back(ch);
                    i++;
                    continue;
                }
            }
            if (!tmp_fixedtext.empty())
            {
                tokens.push_back(Token(TokenType::FixedText, tmp_fixedtext));
                tmp_fixedtext.clear();
            }
            return tokens;
        }

        const inline static std::unordered_map<std::string, TokenType> TokenTypeMap = {
            {"m", TokenType::Message},
            {"p", TokenType::Level},
            {"P", TokenType::ShortLevel},
            {"t", TokenType::ThreadId},
            {"d", TokenType::Date},
            {"F", TokenType::FileName},
            {"fp", TokenType::FullFile},
            {"L", TokenType::Line},
            {"M", TokenType::Fun},
            {"n", TokenType::LineBreak},
        };

        const std::chrono::time_zone *mZone = std::chrono::current_zone();
        std::vector<Token> mTokens;
    };
}

#endif // HALOLOG_FORMATTER_H
