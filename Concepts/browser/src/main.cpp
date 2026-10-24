#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <cstdio>
#include <cstring>

// ====== TQZTR ======
static const char* SEARCH_URL = "https://lite.duckduckgo.com/lite/?q=";

static const char* HOME_HTML = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>TQZTR</title>
<style>
  * { box-sizing: border-box; }
  html, body { height: 100%; margin: 0; }
  body {
    background: radial-gradient(ellipse at top, #101020 0%, #060608 70%);
    color: #e8e8f0; font-family: system-ui, -apple-system, sans-serif;
    display: flex; flex-direction: column; align-items: center;
    justify-content: center;
  }
  h1 {
    font-size: 4em; margin: 0 0 0.15em 0; letter-spacing: -2px;
    font-weight: 800;
    background: linear-gradient(135deg, #8aa4ff, #d08aff);
    -webkit-background-clip: text; -webkit-text-fill-color: transparent;
  }
  p { color: #55556a; margin: 0 0 2.2em 0; font-size: 1.05em; }
  form { display: flex; gap: 10px; width: 620px; max-width: 92vw; }
  input {
    flex: 1; padding: 16px 20px; font-size: 1.1em; border-radius: 12px;
    border: 1px solid #242440; background: #0c0c18; color: #fff;
    outline: none; transition: border-color .15s;
  }
  input:focus { border-color: #6a8cff; }
  button {
    padding: 16px 26px; font-size: 1.1em; border-radius: 12px; border: none;
    background: linear-gradient(135deg, #4a6cf7, #8a5cf7); color: #fff;
    cursor: pointer; font-weight: 600;
  }
  button:hover { filter: brightness(1.15); }
  .hint { margin-top: 1.4em; color: #3a3a4e; font-size: .82em; }
</style>
</head>
<body>
  <h1>TQZTR</h1>
  <p>szybka wyszukiwarka internetowa</p>
  <form onsubmit="event.preventDefault();
      const q = document.getElementById('q').value.trim();
      if (q) window.location.href = '__SEARCH_URL__' + encodeURIComponent(q);">
    <input id="q" type="text" placeholder="Szukaj..." autofocus autocomplete="off">
    <button type="submit">Szukaj</button>
  </form>
  <div class="hint">Enter = szukaj · Ctrl+T = nowa karta</div>
</body>
</html>
)HTML";

static char* build_home_html() {
    const char* placeholder = "__SEARCH_URL__";
    const char* pos = strstr(HOME_HTML, placeholder);
    if (!pos) return g_strdup(HOME_HTML);
    GString* out = g_string_new(nullptr);
    g_string_append_len(out, HOME_HTML, pos - HOME_HTML);
    g_string_append(out, SEARCH_URL);
    g_string_append(out, pos + strlen(placeholder));
    return g_string_free(out, FALSE);
}

// ====== STAN ======
struct AppState {
    GtkWidget* window          = nullptr;
    GtkWidget* sidebar_list    = nullptr;
    GtkWidget* stack           = nullptr;
    GtkWidget* url_entry       = nullptr;
    GtkWidget* back_btn        = nullptr;
    GtkWidget* forward_btn     = nullptr;
    GtkWidget* reload_btn      = nullptr;
    int        next_tab_id     = 0;
    GtkWidget* current_webview = nullptr;
    WebKitWebContext* web_context = nullptr;
};

// deklaracje
static WebKitWebView* add_new_tab_internal(AppState* st, bool switch_to);
static WebKitWebView* add_new_tab(AppState* st, const char* url, bool switch_to);
static void close_tab(AppState* st, GtkListBoxRow* row);

// ====== POMOCNICZE ======
static WebKitWebView* row_get_webview(GtkListBoxRow* row) {
    return (WebKitWebView*)g_object_get_data(G_OBJECT(row), "webview");
}

static char* normalize_url(const char* text) {
    if (!text || !*text) return g_strdup("");
    if (g_str_has_prefix(text, "http://") || g_str_has_prefix(text, "https://") ||
        g_str_has_prefix(text, "about:")  || g_str_has_prefix(text, "file:")   ||
        g_str_has_prefix(text, "data:"))
        return g_strdup(text);
    if (strchr(text, '.') && !strchr(text, ' '))
        return g_strdup_printf("https://%s", text);
    char* escaped = g_uri_escape_string(text, nullptr, TRUE);
    char* url = g_strdup_printf("%s%s", SEARCH_URL, escaped);
    g_free(escaped);
    return url;
}

static void sync_url_entry(AppState* st) {
    if (!st->current_webview) return;
    const char* uri = webkit_web_view_get_uri(WEBKIT_WEB_VIEW(st->current_webview));
    gtk_editable_set_text(GTK_EDITABLE(st->url_entry), uri ? uri : "");
}

static void sync_nav_buttons(AppState* st) {
    if (!st->current_webview) return;
    WebKitWebView* wv = WEBKIT_WEB_VIEW(st->current_webview);
    gtk_widget_set_sensitive(st->back_btn,    webkit_web_view_can_go_back(wv));
    gtk_widget_set_sensitive(st->forward_btn, webkit_web_view_can_go_forward(wv));
}

static void update_row_favicon(GtkListBoxRow* row, GdkTexture* favicon) {
    GtkWidget* img = (GtkWidget*)g_object_get_data(G_OBJECT(row), "favicon");
    if (!img) return;
    if (favicon)
        gtk_image_set_from_paintable(GTK_IMAGE(img), GDK_PAINTABLE(favicon));
    else
        gtk_image_set_from_icon_name(GTK_IMAGE(img), "text-x-generic-symbolic");
}

// ====== SYGNAŁY ======
static void on_row_selected(GtkListBox* box, GtkListBoxRow* row, gpointer data) {
    (void)box; AppState* st = (AppState*)data;
    if (!row) return;
    const char* page_name = (const char*)g_object_get_data(G_OBJECT(row), "page_name");
    if (!page_name) return;
    gtk_stack_set_visible_child_name(GTK_STACK(st->stack), page_name);
    WebKitWebView* wv = row_get_webview(row);
    st->current_webview = GTK_WIDGET(wv);
    sync_url_entry(st); sync_nav_buttons(st);
    const char* title = webkit_web_view_get_title(wv);
    if (title) gtk_window_set_title(GTK_WINDOW(st->window), title);
}

static void on_title_changed(GObject* obj, GParamSpec* pspec, gpointer data) {
    (void)pspec; AppState* st = (AppState*)data;
    GtkListBoxRow* row = (GtkListBoxRow*)g_object_get_data(obj, "row");
    if (!row) return;
    const char* title = webkit_web_view_get_title(WEBKIT_WEB_VIEW(obj));
    if (!title || !*title) {
        const char* uri = webkit_web_view_get_uri(WEBKIT_WEB_VIEW(obj));
        title = (uri && *uri) ? uri : "Nowa karta";
    }
    GtkWidget* label = (GtkWidget*)g_object_get_data(G_OBJECT(row), "label");
    if (label) gtk_label_set_text(GTK_LABEL(label), title);
    if (st->current_webview == GTK_WIDGET(obj))
        gtk_window_set_title(GTK_WINDOW(st->window), title);
}

static void on_uri_changed(GObject* obj, GParamSpec* pspec, gpointer data) {
    (void)pspec; AppState* st = (AppState*)data;
    if (st->current_webview == GTK_WIDGET(obj)) { sync_url_entry(st); sync_nav_buttons(st); }
}

static void on_favicon_changed(GObject* obj, GParamSpec* pspec, gpointer data) {
    (void)pspec; (void)data;
    GtkListBoxRow* row = (GtkListBoxRow*)g_object_get_data(obj, "row");
    if (!row) return;
    GdkTexture* favicon = webkit_web_view_get_favicon(WEBKIT_WEB_VIEW(obj));
    update_row_favicon(row, favicon);
}

static void on_load_changed(WebKitWebView* wv, WebKitLoadEvent event, gpointer data) {
    (void)data;
    if (event == WEBKIT_LOAD_FINISHED) {
        AppState* st = (AppState*)g_object_get_data(G_OBJECT(wv), "app");
        if (!st) return;
        GtkListBoxRow* row = (GtkListBoxRow*)g_object_get_data(G_OBJECT(wv), "row");
        if (row) {
            GdkTexture* f = webkit_web_view_get_favicon(wv);
            if (f) update_row_favicon(row, f);
        }
        if (st->current_webview == GTK_WIDGET(wv)) sync_nav_buttons(st);
    }
}

static gboolean on_load_failed(WebKitWebView* wv, WebKitLoadEvent event, const char* uri,
                               GError* error, gpointer data) {
    (void)wv; (void)event; (void)data;
    g_printerr("[load-failed] %s: %s\n", uri ? uri : "(null)", error->message);
    return FALSE;
}

static GtkWidget* on_create(WebKitWebView* wv, WebKitNavigationAction* action, gpointer data) {
    (void)wv; (void)action; AppState* st = (AppState*)data;
    WebKitWebView* new_wv = add_new_tab_internal(st, true);
    return GTK_WIDGET(new_wv);
}

// ====== OPTYMALIZACJE WEBKIT ======
static void configure_webview_for_speed(WebKitWebView* wv) {
    WebKitSettings* s = webkit_web_view_get_settings(wv);

    // JavaScript + nowoczesne API
    webkit_settings_set_enable_javascript(s, TRUE);
    webkit_settings_set_enable_webgl(s, TRUE);
    webkit_settings_set_enable_webaudio(s, TRUE);
    webkit_settings_set_enable_media_stream(s, TRUE);
    webkit_settings_set_enable_html5_local_storage(s, TRUE);
    webkit_settings_set_enable_html5_database(s, TRUE);

    // Page Cache — kluczowe dla wstecz/dalej i ponownych wizyt
    webkit_settings_set_enable_page_cache(s, TRUE);

    // DNS prefetch — rozwiązuje domeny zanim klikniesz
    webkit_settings_set_enable_dns_prefetching(s, TRUE);

    // Płynne przewijanie (subiektywnie szybciej)
    webkit_settings_set_enable_smooth_scrolling(s, TRUE);

    // Akceleracja sprzętowa zawsze
    webkit_settings_set_hardware_acceleration_policy(
        s, WEBKIT_HARDWARE_ACCELERATION_POLICY_ALWAYS);

    // Wyłącz zbędne w produkcji
    webkit_settings_set_enable_developer_extras(s, FALSE);
    webkit_settings_set_enable_write_console_messages_to_stdout(s, FALSE);
}

// ====== KARTY ======
static WebKitWebView* add_new_tab_internal(AppState* st, bool switch_to) {
    int tab_id = st->next_tab_id++;
    char* page_name = g_strdup_printf("tab_%d", tab_id);

    WebKitWebView* wv = WEBKIT_WEB_VIEW(webkit_web_view_new());
    configure_webview_for_speed(wv);

    gtk_stack_add_named(GTK_STACK(st->stack), GTK_WIDGET(wv), page_name);

    GtkWidget* row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget* favicon = gtk_image_new_from_icon_name("text-x-generic-symbolic");
    gtk_image_set_pixel_size(GTK_IMAGE(favicon), 16);
    gtk_box_append(GTK_BOX(row_box), favicon);

    GtkWidget* label = gtk_label_new("Nowa karta");
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(row_box), label);

    GtkWidget* close_btn = gtk_button_new_from_icon_name("window-close-symbolic");
    gtk_button_set_has_frame(GTK_BUTTON(close_btn), FALSE);
    gtk_widget_add_css_class(close_btn, "flat");
    gtk_widget_set_tooltip_text(close_btn, "Zamknij kartę (Ctrl+W)");
    gtk_box_append(GTK_BOX(row_box), close_btn);

    GtkWidget* row = gtk_list_box_row_new();
    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), row_box);

    g_object_set_data(G_OBJECT(row), "webview", wv);
    g_object_set_data_full(G_OBJECT(row), "page_name", page_name, g_free);
    g_object_set_data(G_OBJECT(row), "label", label);
    g_object_set_data(G_OBJECT(row), "favicon", favicon);
    g_object_set_data(G_OBJECT(wv), "row", row);
    g_object_set_data(G_OBJECT(wv), "app", st);

    g_signal_connect(wv, "notify::title",   G_CALLBACK(on_title_changed), st);
    g_signal_connect(wv, "notify::uri",     G_CALLBACK(on_uri_changed), st);
    g_signal_connect(wv, "notify::favicon", G_CALLBACK(on_favicon_changed), st);
    g_signal_connect(wv, "load-changed",    G_CALLBACK(on_load_changed), st);
    g_signal_connect(wv, "load-failed",     G_CALLBACK(on_load_failed), st);
    g_signal_connect(wv, "create",          G_CALLBACK(on_create), st);

    g_object_set_data(G_OBJECT(close_btn), "row", row);
    g_signal_connect(close_btn, "clicked", G_CALLBACK(+[](GtkButton* btn, gpointer data){
        AppState* s = (AppState*)data;
        GtkListBoxRow* r = (GtkListBoxRow*)g_object_get_data(G_OBJECT(btn), "row");
        if (r) close_tab(s, r);
    }), st);

    gtk_list_box_append(GTK_LIST_BOX(st->sidebar_list), row);

    if (switch_to)
        gtk_list_box_select_row(GTK_LIST_BOX(st->sidebar_list), GTK_LIST_BOX_ROW(row));

    return wv;
}

static WebKitWebView* add_new_tab(AppState* st, const char* url, bool switch_to) {
    WebKitWebView* wv = add_new_tab_internal(st, switch_to);
    if (url && *url) {
        char* full = normalize_url(url);
        webkit_web_view_load_uri(wv, full);
        g_free(full);
    } else {
        char* html = build_home_html();
        webkit_web_view_load_html(wv, html, nullptr);
        g_free(html);
    }
    return wv;
}

static void close_tab(AppState* st, GtkListBoxRow* row) {
    const char* page_name = (const char*)g_object_get_data(G_OBJECT(row), "page_name");
    if (page_name) {
        GtkWidget* child = gtk_stack_get_child_by_name(GTK_STACK(st->stack), page_name);
        if (child) gtk_stack_remove(GTK_STACK(st->stack), child);
    }
    gtk_list_box_remove(GTK_LIST_BOX(st->sidebar_list), GTK_WIDGET(row));
    if (!gtk_list_box_get_row_at_index(GTK_LIST_BOX(st->sidebar_list), 0))
        add_new_tab(st, nullptr, true);
}

// ====== TOOLBAR ======
static void on_new_tab_clicked(GtkButton* btn, gpointer data) { (void)btn; add_new_tab((AppState*)data, nullptr, true); }
static void on_back_clicked(GtkButton* btn, gpointer data) { (void)btn; AppState* st = (AppState*)data; if (st->current_webview) webkit_web_view_go_back(WEBKIT_WEB_VIEW(st->current_webview)); }
static void on_forward_clicked(GtkButton* btn, gpointer data) { (void)btn; AppState* st = (AppState*)data; if (st->current_webview) webkit_web_view_go_forward(WEBKIT_WEB_VIEW(st->current_webview)); }
static void on_reload_clicked(GtkButton* btn, gpointer data) { (void)btn; AppState* st = (AppState*)data; if (st->current_webview) webkit_web_view_reload(WEBKIT_WEB_VIEW(st->current_webview)); }
static void on_home_clicked(GtkButton* btn, gpointer data) { (void)btn; AppState* st = (AppState*)data; if (!st->current_webview) return; char* html = build_home_html(); webkit_web_view_load_html(WEBKIT_WEB_VIEW(st->current_webview), html, nullptr); g_free(html); }

static void on_url_activate(GtkEntry* entry, gpointer data) {
    AppState* st = (AppState*)data;
    if (!st->current_webview) return;
    const char* text = gtk_editable_get_text(GTK_EDITABLE(entry));
    if (!text || !*text) return;
    char* url = normalize_url(text);
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(st->current_webview), url);
    g_free(url);
}

// ====== SKRÓTY ======
static gboolean on_key_pressed(GtkEventControllerKey* c, guint keyval, guint keycode, GdkModifierType state, gpointer data) {
    (void)c; (void)keycode; AppState* st = (AppState*)data;
    if ((state & GDK_CONTROL_MASK) != 0) {
        switch (keyval) {
            case GDK_KEY_t: add_new_tab(st, nullptr, true); return TRUE;
            case GDK_KEY_w: { GtkListBoxRow* row = gtk_list_box_get_selected_row(GTK_LIST_BOX(st->sidebar_list)); if (row) close_tab(st, row); return TRUE; }
            case GDK_KEY_l: gtk_widget_grab_focus(st->url_entry); return TRUE;
            case GDK_KEY_r: if (st->current_webview) webkit_web_view_reload(WEBKIT_WEB_VIEW(st->current_webview)); return TRUE;
        }
    }
    return FALSE;
}

// ====== AKTYWACJA ======
static void on_activate(GtkApplication* app, gpointer user_data) {
    AppState* st = (AppState*)user_data;

    st->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(st->window), "TQZTR");
    gtk_window_set_default_size(GTK_WINDOW(st->window), 1400, 900);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(st->window), vbox);

    // Toolbar
    GtkWidget* toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_margin_start(toolbar, 6); gtk_widget_set_margin_end(toolbar, 6);
    gtk_widget_set_margin_top(toolbar, 6); gtk_widget_set_margin_bottom(toolbar, 6);
    gtk_box_append(GTK_BOX(vbox), toolbar);

    st->back_btn    = gtk_button_new_from_icon_name("go-previous-symbolic");
    st->forward_btn = gtk_button_new_from_icon_name("go-next-symbolic");
    st->reload_btn  = gtk_button_new_from_icon_name("view-refresh-symbolic");
    GtkWidget* home_btn = gtk_button_new_from_icon_name("go-home-symbolic");
    GtkWidget* new_tab_btn = gtk_button_new_from_icon_name("list-add-symbolic");

    gtk_widget_set_tooltip_text(st->back_btn, "Wstecz");
    gtk_widget_set_tooltip_text(st->forward_btn, "Dalej");
    gtk_widget_set_tooltip_text(st->reload_btn, "Odśwież (Ctrl+R)");
    gtk_widget_set_tooltip_text(home_btn, "Strona główna TQZTR");
    gtk_widget_set_tooltip_text(new_tab_btn, "Nowa karta (Ctrl+T)");

    gtk_box_append(GTK_BOX(toolbar), st->back_btn);
    gtk_box_append(GTK_BOX(toolbar), st->forward_btn);
    gtk_box_append(GTK_BOX(toolbar), st->reload_btn);
    gtk_box_append(GTK_BOX(toolbar), home_btn);
    gtk_box_append(GTK_BOX(toolbar), new_tab_btn);

    st->url_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(st->url_entry), "Szukaj w TQZTR lub wpisz adres...");
    gtk_widget_set_hexpand(st->url_entry, TRUE);
    gtk_box_append(GTK_BOX(toolbar), st->url_entry);

    GtkWidget* paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_vexpand(paned, TRUE);
    gtk_box_append(GTK_BOX(vbox), paned);

    st->sidebar_list = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(st->sidebar_list), GTK_SELECTION_SINGLE);
    gtk_widget_add_css_class(st->sidebar_list, "sidebar");

    GtkWidget* sidebar_scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sidebar_scrolled), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(sidebar_scrolled), st->sidebar_list);
    gtk_paned_set_start_child(GTK_PANED(paned), sidebar_scrolled);
    gtk_paned_set_resize_start_child(GTK_PANED(paned), FALSE);
    gtk_paned_set_shrink_start_child(GTK_PANED(paned), FALSE);

    st->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(st->stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_stack_set_transition_duration(GTK_STACK(st->stack), 100);
    gtk_widget_set_hexpand(st->stack, TRUE);
    gtk_widget_set_vexpand(st->stack, TRUE);
    gtk_paned_set_end_child(GTK_PANED(paned), st->stack);

    g_signal_connect(paned, "map", G_CALLBACK(+[](GtkWidget* w, gpointer){ gtk_paned_set_position(GTK_PANED(w), 240); }), nullptr);

    g_signal_connect(st->sidebar_list, "row-selected", G_CALLBACK(on_row_selected), st);
    g_signal_connect(st->url_entry, "activate", G_CALLBACK(on_url_activate), st);
    g_signal_connect(st->back_btn, "clicked", G_CALLBACK(on_back_clicked), st);
    g_signal_connect(st->forward_btn, "clicked", G_CALLBACK(on_forward_clicked), st);
    g_signal_connect(st->reload_btn, "clicked", G_CALLBACK(on_reload_clicked), st);
    g_signal_connect(home_btn, "clicked", G_CALLBACK(on_home_clicked), st);
    g_signal_connect(new_tab_btn, "clicked", G_CALLBACK(on_new_tab_clicked), st);

    GtkEventController* key_ctrl = gtk_event_controller_key_new();
    g_signal_connect(key_ctrl, "key-pressed", G_CALLBACK(on_key_pressed), st);
    gtk_widget_add_controller(st->window, key_ctrl);

    // CSS
    GtkCssProvider* css = gtk_css_provider_new();
    gtk_css_provider_load_from_string(css,
        ".sidebar { background: #0f0f1a; }"
        ".sidebar row { padding: 8px 10px; border-radius: 8px; margin: 2px 4px; }"
        ".sidebar row:selected { background: #22223a; }"
        ".sidebar row:hover { background: #1a1a2e; }"
        ".sidebar image { opacity: 0.95; }"
    );
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Cache model WEB_BROWSER — największy zysk przy ponownych wizytach
    WebKitWebContext* ctx = webkit_web_context_get_default();
    webkit_web_context_set_cache_model(ctx, WEBKIT_CACHE_MODEL_WEB_BROWSER);
    st->web_context = ctx;

    add_new_tab(st, nullptr, true);
    gtk_window_present(GTK_WINDOW(st->window));
}

int main(int argc, char** argv) {
    AppState st;
    GtkApplication* app = gtk_application_new("org.tqztr.browser", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), &st);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}