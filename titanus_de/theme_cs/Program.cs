using System.Text.Json;

namespace TitanusTheme;

public static class Program
{
    public static int Main(string[] args)
    {
        bool light = args.Contains("--light");
        string output = args.FirstOrDefault(a => !a.StartsWith("--")) ?? "theme.json";

        Theme theme = light ? Theme.Light() : Theme.Dark();

        var options = new JsonSerializerOptions
        {
            WriteIndented = true,
            // `GradientLeft` -> `gradient_left` itd. — dokładnie nazwy pól w Rust.
            PropertyNamingPolicy = JsonNamingPolicy.SnakeCaseLower,
        };

        File.WriteAllText(output, JsonSerializer.Serialize(theme, options) + "\n");
        Console.WriteLine($"[theme_cs] zapisano {output} ({ (light ? "jasny" : "ciemny") })");
        return 0;
    }
}
