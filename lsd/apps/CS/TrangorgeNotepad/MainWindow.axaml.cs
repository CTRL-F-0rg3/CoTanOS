using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Platform.Storage;
using System;
using System.Threading.Tasks;
using TrangorgeNotepad.Models;
using TrangorgeNotepad.Services;

namespace TrangorgeNotepad
{
    public partial class MainWindow : Window
    {
        private Document _document = new Document();
        private FileHandler _fileHandler = new FileHandler();
        private TextAnalyzer _textAnalyzer = new TextAnalyzer();

        public MainWindow()
        {
            InitializeComponent();
            UpdateStatusBar();
        }

        private void TxtEditor_TextChanged(object? sender, TextChangedEventArgs e)
        {
            if (TxtEditor == null) return;

            var newContent = TxtEditor.Text ?? string.Empty;
            _document.Modify(newContent);
            UpdateTitle();
            UpdateStatusBar();
        }

        private void UpdateTitle()
        {
            var modified = _document.IsModified ? " *" : "";
            Title = $"{_document.FileName}{modified} - TrangorgeOS Notatnik";
        }

        private void UpdateStatusBar()
        {
            if (TxtEditor?.Text == null) return;

            var stats = _textAnalyzer.Analyze(TxtEditor.Text);
            TxtLines.Text = $"Linie: {stats.lines}";
            TxtWords.Text = $"Słowa: {stats.words}";
            TxtChars.Text = $"Znaki: {stats.chars}";
        }

        private async void MenuNew_Click(object? sender, RoutedEventArgs e)
        {
            if (_document.IsModified)
            {
                var result = await ShowSaveDialog();
                if (result == DialogResult.Cancel) return;
            }

            _document = new Document();
            TxtEditor.Text = string.Empty;
            UpdateTitle();
            UpdateStatusBar();
            TxtStatus.Text = "Nowy dokument";
        }

        private async void MenuOpen_Click(object? sender, RoutedEventArgs e)
        {
            if (_document.IsModified)
            {
                var result = await ShowSaveDialog();
                if (result == DialogResult.Cancel) return;
            }

            var files = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
            {
                Title = "Otwórz plik",
                AllowMultiple = false,
                FileTypeFilter = new[] 
                { 
                    FilePickerFileTypes.TextPlain,
                    FilePickerFileTypes.All 
                }
            });

            if (files.Count > 0)
            {
                var path = files[0].Path.LocalPath;
                try
                {
                    var content = await _fileHandler.LoadFileAsync(path);
                    _document = new Document { FilePath = path, Content = content };
                    TxtEditor.Text = content;
                    UpdateTitle();
                    UpdateStatusBar();
                    TxtStatus.Text = $"Otwarto: {path}";
                }
                catch (Exception ex)
                {
                    TxtStatus.Text = $"Błąd: {ex.Message}";
                }
            }
        }

        private async void MenuSave_Click(object? sender, RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(_document.FilePath))
            {
                await SaveAsAsync();
            }
            else
            {
                await SaveAsync(_document.FilePath);
            }
        }

        private async void MenuSaveAs_Click(object? sender, RoutedEventArgs e)
        {
            await SaveAsAsync();
        }

        private async Task SaveAsAsync()
        {
            var file = await StorageProvider.SaveFilePickerAsync(new FilePickerSaveOptions
            {
                Title = "Zapisz jako",
                DefaultExtension = ".txt",
                FileTypeChoices = new[] 
                { 
                    new FilePickerFileType("Plik tekstowy") { Patterns = new[] { "*.txt" } },
                    new FilePickerFileType("Wszystkie pliki") { Patterns = new[] { "*.*" } }
                }
            });

            if (file != null)
            {
                await SaveAsync(file.Path.LocalPath);
            }
        }

        private async Task SaveAsync(string path)
        {
            try
            {
                await _fileHandler.SaveFileAsync(path, TxtEditor.Text ?? string.Empty);
                _document.MarkAsSaved(path);
                UpdateTitle();
                TxtStatus.Text = $"Zapisano: {path}";
            }
            catch (Exception ex)
            {
                TxtStatus.Text = $"Błąd zapisu: {ex.Message}";
            }
        }

        private void MenuExit_Click(object? sender, RoutedEventArgs e)
        {
            Close();
        }

        private void MenuUndo_Click(object? sender, RoutedEventArgs e)
        {
            TxtEditor.Undo();
        }

        private void MenuRedo_Click(object? sender, RoutedEventArgs e)
        {
            TxtEditor.Redo();
        }

        private void MenuSelectAll_Click(object? sender, RoutedEventArgs e)
        {
            TxtEditor.SelectAll();
        }

        private void MenuToggleStatusBar_Click(object? sender, RoutedEventArgs e)
        {
            StatusBar.IsVisible = !StatusBar.IsVisible;
        }

        private enum DialogResult { Yes, No, Cancel }

        private async Task<DialogResult> ShowSaveDialog()
        {
            // Tutaj możesz dodać okno dialogowe z pytaniem "Czy zapisać zmiany?"
            // Na razie zwracamy No (nie zapisuj)
            return DialogResult.No;
        }
    }
}