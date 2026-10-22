using System;
using System.Linq;

namespace TrangorgeNotepad.Services
{
    public class TextAnalyzer
    {
        public int CountCharacters(string text)
        {
            return text.Length;
        }

        public int CountWords(string text)
        {
            if (string.IsNullOrWhiteSpace(text))
                return 0;

            return text.Split(new[] { ' ', '\t', '\n', '\r' }, StringSplitOptions.RemoveEmptyEntries).Length;
        }

        public int CountLines(string text)
        {
            if (string.IsNullOrEmpty(text))
                return 0;

            return text.Split('\n').Length;
        }

        public (int lines, int words, int chars) Analyze(string text)
        {
            return (CountLines(text), CountWords(text), CountCharacters(text));
        }
    }
}