namespace TitanusTheme;

/// <summary>
/// Motyw wyglądu Titanusa — kontrakt danych, który czyta Rust (`theme.rs`).
///
/// Kolory to łańcuchy "#RRGGBB"; Rust zamienia je na 0xAARRGGBB (alfa = 0xFF).
/// Nazwy pól po serializacji (snake_case) pokrywają się 1:1 z polami struktury
/// <c>Theme</c> w Rust — to jest cały „szew" między C# a Rust: JSON.
/// </summary>
public sealed class Theme
{
    public string Background { get; set; } = "#101218";
    public string GradientLeft { get; set; } = "#1A202E";
    public string GradientRight { get; set; } = "#0D0E14";
    public string WindowBg { get; set; } = "#1E222C";
    public string WindowBorder { get; set; } = "#3A445A";
    public string WindowFocus { get; set; } = "#7CF09C";
    public string TitleBg { get; set; } = "#2A354A";
    public string TitleText { get; set; } = "#D0D8E0";
    public string Text { get; set; } = "#C8D0DC";
    public string DockBg { get; set; } = "#161820";
    public string DockEntry { get; set; } = "#2A354A";
    public string DockActive { get; set; } = "#7CF09C";
    public string TrayBg { get; set; } = "#161820";
    public string TrayText { get; set; } = "#D0D8E0";
    public uint FontSize { get; set; } = 14;
    public uint DockWidth { get; set; } = 64;
    public uint TrayMargin { get; set; } = 8;

    /// Ciemny motyw (domyślny) — te same wartości co <c>Theme::dark()</c> w Rust.
    public static Theme Dark() => new Theme();

    /// Jasny motyw.
    public static Theme Light() => new Theme
    {
        Background = "#ECEFF4",
        GradientLeft = "#FFFFFF",
        GradientRight = "#D8DEE9",
        WindowBg = "#FFFFFF",
        WindowBorder = "#B8C0CC",
        WindowFocus = "#2E7D32",
        TitleBg = "#D8DEE9",
        TitleText = "#2E3440",
        Text = "#2E3440",
        DockBg = "#E5E9F0",
        DockEntry = "#D8DEE9",
        DockActive = "#2E7D32",
        TrayBg = "#E5E9F0",
        TrayText = "#2E3440",
    };
}
