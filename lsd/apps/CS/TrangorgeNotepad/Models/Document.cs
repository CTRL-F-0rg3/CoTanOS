using System;

namespace TrangorgeNotepad.Models
{
    public class Document
    {
        public string FilePath { get; set; } = string.Empty;
        public string Content { get; set; } = string.Empty;
        public bool IsModified { get; set; } = false;
        public DateTime LastModified { get; set; } = DateTime.Now;

        public string FileName => string.IsNullOrEmpty(FilePath) 
            ? "Nowy dokument" 
            : System.IO.Path.GetFileName(FilePath);

        public void MarkAsSaved(string path)
        {
            FilePath = path;
            IsModified = false;
            LastModified = DateTime.Now;
        }

        public void Modify(string newContent)
        {
            Content = newContent;
            IsModified = true;
            LastModified = DateTime.Now;
        }
    }
}