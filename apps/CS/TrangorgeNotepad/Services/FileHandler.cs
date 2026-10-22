using System;
using System.IO;
using System.Threading.Tasks;
using TrangorgeNotepad.Models;

namespace TrangorgeNotepad.Services
{
    public class FileHandler
    {
        public async Task<string> LoadFileAsync(string path)
        {
            if (!File.Exists(path))
                throw new FileNotFoundException($"Plik nie istnieje: {path}");

            return await File.ReadAllTextAsync(path);
        }

        public async Task SaveFileAsync(string path, string content)
        {
            var directory = Path.GetDirectoryName(path);
            if (!string.IsNullOrEmpty(directory) && !Directory.Exists(directory))
            {
                Directory.CreateDirectory(directory);
            }

            await File.WriteAllTextAsync(path, content);
        }

        public string GetFileExtension(string path)
        {
            return Path.GetExtension(path).ToLower();
        }

        public bool IsTextFile(string path)
        {
            var extension = GetFileExtension(path);
            var textExtensions = new[] { ".txt", ".md", ".cs", ".cpp", ".c", ".h", ".java", ".py", ".js", ".html", ".css", ".xml", ".json", ".yaml", ".yml" };
            return Array.Exists(textExtensions, ext => ext == extension);
        }
    }
}