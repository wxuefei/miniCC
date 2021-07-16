#include "PreProcessor.hpp"

#include <cassert>
#include "PPLexer.hpp"

std::string WhiteSpaceChars("\t ");

void PreProcessor::ParseDirective(std::string &Line) {
  PPLexer lexer(Line);
  lexer.Lex(); // eat '#'

  auto Directive = lexer.Lex();
  assert(Directive.IsKeyword() && "Must be a keyword at this point");

  if (Directive.GetKind() == PPToken::Define) {
    auto DefinedID = lexer.Lex();
    assert(DefinedID.GetKind() != PPToken::EndOfFile &&
           DefinedID.GetKind() == PPToken::Identifier);

    // must be called here otherwise the token lookaheads mess up the lineindex
    // TODO: solve this problem, maybe with giving tokens the linenumber
    auto RemainingText = lexer.GetRemainingText();

    if (lexer.Is(PPToken::EndOfFile))
      DefinedMacros[DefinedID.GetString()] = {"", 0};
    else if (lexer.Is(PPToken::LeftParen)) {
      lexer.Lex(); // eat '('
      std::vector<std::string> Params;

      do {
        auto Param = lexer.Lex();
        assert(Param.GetKind() == PPToken::Identifier);
        Params.push_back(Param.GetString());

        if (lexer.IsNot(PPToken::Colon))
          break;
        lexer.Lex(); // eat ','
      } while (true);

      assert(lexer.Is(PPToken::RightParen));
      lexer.Lex(); // eat ')'

      auto Body = lexer.GetRemainingText();
      for (size_t i = 0; i < Params.size(); i++) {
        // replacing the parameters with their index eg.: with the below macro
        //    #define MAX(A,B) (((A) > (B)) ? (A) : (B))
        // the Body is "(((A) > (B)) ? (A) : (B))"
        // and it became "((($0) > ($1)) ? ($0) : ($1))"
        // this will make the substitution easier later
        while (Body.find(Params[i]) != std::string::npos)
          Body.replace(Body.find(Params[i]), Params[i].length(), "$" + std::to_string(i));
      }

      DefinedMacros[DefinedID.GetString()] = {Body, Params.size()};
    }
    // plain define (eg.: #define TRUE 1)
    else {
      DefinedMacros[DefinedID.GetString()] = {RemainingText, 0};
    }
  }
}

void PreProcessor::SubstituteMacros(std::string &Line) {
  for (auto &[MacroID, MacroData] : DefinedMacros) {
    auto &[MacroBody, MacroParam] = MacroData;

    // simple search and replace of plain macros
    if (MacroParam == 0)
      while (Line.find(MacroID) != std::string::npos)
        Line.replace(Line.find(MacroID), MacroID.length(), MacroBody);
    // otherwise it a function macro and have to substitute the right values
    // into its parameters
    else {
      auto Pos = Line.find(MacroID); // macro start pos
      if (Pos == std::string::npos)
        continue;
      Pos += MacroID.length();
      assert(Line[Pos] == '(');
      Pos++;

      auto RemainingLine = Line.substr(Pos);
      size_t StartPos = 0;
      std::vector<std::string> ActualParams;
      for (size_t i = 0; i < MacroParam; i++) {
        size_t EndPos = i != MacroParam - 1 ? RemainingLine.find(",")
                                            : RemainingLine.find(")");
        ActualParams.push_back(RemainingLine.substr(StartPos, EndPos - StartPos));
        StartPos = EndPos + 1;
      }

      for (size_t i = 0; i < ActualParams.size(); i++) {
        auto Param = "$" + std::to_string(i);
        while (MacroBody.find(Param) != std::string::npos)
          MacroBody.replace(MacroBody.find(Param), Param.length(), ActualParams[i]);
      }

      if (Line.find(MacroID) != std::string::npos)
        Line.replace(Line.find(MacroID), MacroID.length() + StartPos + 1, MacroBody);
    }
  }
}

void PreProcessor::Run() {
  for (size_t LineIdx = 0; LineIdx < Source.size(); LineIdx++) {
    auto &Line = Source[LineIdx];
    if (Line.empty())
      continue;

    if (!Line.empty() && Line[0] == '#') {
      ParseDirective(Line);
      // delete current line, assuming the directive only used one line
      Source.erase(Source.begin() + LineIdx);
      LineIdx--; // since the erase we have to check again the same LineIdx
    }
    else if (!DefinedMacros.empty())
      SubstituteMacros(Line);
  }
}
