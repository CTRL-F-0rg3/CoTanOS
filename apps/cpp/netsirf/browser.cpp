#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// Struktura przechowująca stan globalny przeglądarki
struct BrowserApp {
    GtkWidget* window;
    GtkWidget* notebook;
    GtkWidget* url_entry;
};

// Zapis historii do pliku
void append_to_history(const std::string& url) {
    std::ofstream file("history.txt", std::ios::app);
    if (file.is_open()) {
        file << url << "\n";
    }
}

// Zapis aktualnej sesji (otwartych kart) przy wyjściu
void save_session(GtkNotebook* notebook) {
    std::ofstream file("session.txt");
    if (!file.is_open()) return;

    int n_pages = gtk_notebook_get_n_pages(notebook);
    for (int i = 0; i < n_pages; i++) {
        GtkWidget* page = gtk_notebook_get_nth_page(notebook, i);
        GtkWidget* web_view = static_cast<GtkWidget*>(g_object_get_data(G_OBJECT(page), "web_view"));
        if (WEBKIT_IS_WEB_VIEW(web_view)) {
            const char* uri = webkit_web_view_get_uri(WEBKIT_WEB_VIEW(web_view));
            if (uri) {
                file << uri << "\n";
            }
        }
    }
}

GtkWidget* add_new_tab(BrowserApp* app, const std::string& url);

// Callback zmiany adresu w aktywnej karcie
static void on_uri_changed(WebKitWebView* web_view, GParamSpec* pspec, gpointer user_data) {
    BrowserApp* app = static_cast<BrowserApp*>(user_data);
    GtkWidget* current_page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(app->notebook), 
                                                       gtk_notebook_get_current_page(GTK_NOTEBOOK(app->notebook)));
    if (!current_page) return;

    GtkWidget* active_webview = static_cast<GtkWidget*>(g_object_get_data(G_OBJECT(current_page), "web_view"));
    if (active_webview && WEBKIT_WEB_VIEW(web_view) == WEBKIT_WEB_VIEW(active_webview)) {
        const char* uri = webkit_web_view_get_uri(web_view);
        if (uri) {
            gtk_entry_set_text(GTK_ENTRY(app->url_entry), uri);
            append_to_history(uri);
        }
    }
}

// Obsługa wpisania nowego URL w pasku adresu
static void on_url_activate(GtkEntry* entry, gpointer user_data) {
    BrowserApp* app = static_cast<BrowserApp*>(user_data);
    const gchar* url_text = gtk_entry_get_text(entry);

    GtkWidget* current_page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(app->notebook), 
                                                       gtk_notebook_get_current_page(GTK_NOTEBOOK(app->notebook)));
    if (!current_page) return;

    GtkWidget* web_view = static_cast<GtkWidget*>(g_object_get_data(G_OBJECT(current_page), "web_view"));
    if (WEBKIT_IS_WEB_VIEW(web_view)) {
        std::string final_url = url_text;
        if (final_url.find("http://") != 0 && final_url.find("https://") != 0) {
            final_url = "https://lite.duckduckgo.com/lite?q=" + final_url;
        }
        webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), final_url.c_str());
    }
}

// Tworzenie nowej karty
GtkWidget* add_new_tab(BrowserApp* app, const std::string& url) {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    GtkWidget* web_view = webkit_web_view_new();
    
    WebKitSettings* settings = webkit_web_view_get_settings(WEBKIT_WEB_VIEW(web_view));
    webkit_settings_set_enable_javascript(settings, TRUE);

    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), web_view);
    gtk_widget_set_vexpand(scrolled, TRUE);
    gtk_widget_set_hexpand(scrolled, TRUE);

    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    gtk_widget_show_all(vbox);

    g_object_set_data(G_OBJECT(vbox), "web_view", web_view);

    GtkWidget* label = gtk_label_new("Nowa karta");
    int page_num = gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook), vbox, label);
    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(app->notebook), vbox, TRUE);
    gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(app->notebook), vbox, TRUE);

    g_signal_connect(web_view, "notify::uri", G_CALLBACK(on_uri_changed), app);

    std::string load_url = url.empty() ? "https://lite.duckduckgo.com" : url;
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), load_url.c_str());

    gtk_notebook_set_current_page(GTK_NOTEBOOK(app->notebook), page_num);
    return vbox;
}

// Przycisk nowej karty (+)
static void on_new_tab_clicked(GtkButton* button, gpointer user_data) {
    BrowserApp* app = static_cast<BrowserApp*>(user_data);
    add_new_tab(app, "https://lite.duckduckgo.com");
}

// Zamykanie okna i zapis sesji
static gboolean on_window_delete(GtkWidget* widget, GdkEvent* event, gpointer user_data) {
    BrowserApp* app = static_cast<BrowserApp*>(user_data);
    save_session(GTK_NOTEBOOK(app->notebook));
    gtk_main_quit();
    return FALSE;
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    // Wymuszenie globalnego ciemnego motywu GTK3
    g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);

    BrowserApp app;
    app.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app.window), "TrangorgeOS Zen Browser");
    gtk_window_set_default_size(GTK_WINDOW(app.window), 1200, 800);
    g_signal_connect(app.window, "delete-event", G_CALLBACK(on_window_delete), &app);

    GtkWidget* main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(app.window), main_vbox);

    GtkWidget* toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(toolbar), 6);
    gtk_box_pack_start(GTK_BOX(main_vbox), toolbar, FALSE, FALSE, 0);

    app.url_entry = gtk_entry_new();
    gtk_widget_set_hexpand(app.url_entry, TRUE);
    g_signal_connect(app.url_entry, "activate", G_CALLBACK(on_url_activate), &app);
    gtk_box_pack_start(GTK_BOX(toolbar), app.url_entry, TRUE, TRUE, 0);

    GtkWidget* new_tab_btn = gtk_button_new_with_label(" + ");
    g_signal_connect(new_tab_btn, "clicked", G_CALLBACK(on_new_tab_clicked), &app);
    gtk_box_pack_start(GTK_BOX(toolbar), new_tab_btn, FALSE, FALSE, 0);

    app.notebook = gtk_notebook_new();
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(app.notebook), TRUE);
    gtk_box_pack_start(GTK_BOX(main_vbox), app.notebook, TRUE, TRUE, 0);

    // Odczyt zaprzyjaźnionej sesji lub otwarcie domyślnej karty
    std::ifstream session_file("session.txt");
    if (session_file.is_open()) {
        std::string line;
        bool loaded_any = false;
        while (std::getline(session_file, line)) {
            if (!line.empty()) {
                add_new_tab(&app, line);
                loaded_any = true;
            }
        }
        if (!loaded_any) {
            add_new_tab(&app, "https://lite.duckduckgo.com");
        }
        session_file.close();
    } else {
        add_new_tab(&app, "https://lite.duckduckgo.com");
    }

    gtk_widget_show_all(app.window);
    std::cout << "[Browser] Zen-style WebKit Browser uruchomiony pomyślnie." << std::endl;
    
    gtk_main();
    return 0;
}