# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 

module Heels
  class App
    attr_reader :location
    def initialize
        @location = nil
        @canvas   = Canvas.new(self);
        @nesting  = []
        @timers   = []
        @styles   = nil
        @title    = nil
        @width    = 200
        @height   = 200
        @resizable= true
        @window   = nil
    end
    
    private
    def sleep
      sleep(0.001)
    end
    
    def idle
      begin
        self.sleep
      rescue nil
      end
      return true
    end
    
    public
    
    def style_set(klass, key, value)
      @styles         ||= {}  
      @styles[klass]  ||= {}  
      @styles[key]      = value 
    end  
    
    def reset_styles()
      style_set(Banner,      size, 48);
      style_set(Title,       size, 34);
      style_set(Subtitle,    size, 26);
      style_set(Tagline,     size, 18);
      style_set(Caption,     size, 14);
      style_set(Para,        size, 12);
      style_set(Inscription, size, 10);
      style_set(Code,        family, :monospace);
      style_set(Del,         strikethrough, :single);
      style_set(Em,          emphasis, :italic);
      style_set(Ins,         underline, :single);
      style_set(Link,        underline, :single);
      style_set(Link,        stroke, "#06E");
      style_set(LinkHover,   underline, :single);
      style_set(LinkHover,   stroke, "#039");
      style_set(LinkHover,   fill,   "#EEE");
      style_set(Strong,      weight, :bold);
      style_set(Sup,         rise,   10);
      style_set(Sup,         size,   :x-small);
      style_set(Sub,         rise,   -10);
      style_set(Sub,         size,   :x-small);
    end
    
    def style_class(klass, style)
      @styles         ||= {}  
      @styles[klass]  ||= {}
      style.each { |k,v|  set_style(klass, k, v) }
    end
    
  end
end


__END__

#ifdef SHOES_GTK

static gboolean 
shoes_app_gtk_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data) 
{  
  GdkModifierType state; 
  shoes_app *app = (shoes_app *)data; 
  if (!event->is_hint) 
  { 
    state = (GdkModifierType)event->state; 
    shoes_app_motion(app, (int)event->x, (int)event->y);
  } 
  return TRUE; 
} 

static void
shoes_app_gtk_paint_children(GtkWidget *widget, gpointer data)
{
  shoes_app *app = (shoes_app *)data;
  gtk_container_propagate_expose(GTK_CONTAINER(app->os.window), widget, app->slot.expose);
}

static void
shoes_app_gtk_paint (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{ 
  shoes_app *app = (shoes_app *)data;
  gtk_window_get_size(GTK_WINDOW(app->os.window), &app->width, &app->height);
  shoes_canvas_size(app->canvas, app->width, app->height);
  app->slot.expose = event;
  gtk_container_forall(GTK_CONTAINER(app->os.window), shoes_app_gtk_paint_children, app);
  app->slot.expose = NULL;
}

#define KEY_SYM(name, sym) \
  else if (event->keyval == GDK_##name) \
    v = ID2SYM(rb_intern("" # sym))

static gboolean
shoes_app_gtk_keypress (GtkWidget *widget, GdkEventKey *event, gpointer data)
{ 
  VALUE v = Qnil;
  guint modifiers;
  shoes_app *app = (shoes_app *)data;
  if (event->length > 0)
  {
    if (event->string[0] == '\r' && event->length == 1)
      v = rb_str_new2("\n");
    else
      v = rb_str_new(event->string, event->length);
  }
  KEY_SYM(BackSpace, backspace);
  KEY_SYM(Tab, tab);
  KEY_SYM(Page_Up, page_up);
  KEY_SYM(Page_Down, page_down);
  KEY_SYM(Home, home);
  KEY_SYM(End, end);
  KEY_SYM(Left, left);
  KEY_SYM(Up, up);
  KEY_SYM(Right, right);
  KEY_SYM(Down, down);
  KEY_SYM(F1, f1);
  KEY_SYM(F2, f2);
  KEY_SYM(F3, f3);
  KEY_SYM(F4, f4);
  KEY_SYM(F5, f5);
  KEY_SYM(F6, f6);
  KEY_SYM(F7, f7);
  KEY_SYM(F8, f8);
  KEY_SYM(F9, f9);
  KEY_SYM(F10, f10);
  KEY_SYM(F11, f11);
  KEY_SYM(F12, f12);

  if (SYMBOL_P(v))
  {
    if (event->state & GDK_MOD1_MASK)
      KEY_STATE(alt);
    if (event->state & GDK_SHIFT_MASK)
      KEY_STATE(shift);
    if (event->state & GDK_CONTROL_MASK)
      KEY_STATE(control);
  }
  else
  {
    if (event->state & GDK_MOD1_MASK)
      KEY_STATE(alt);
  }

  if (v != Qnil)
    shoes_app_keypress(app, v);
  return FALSE;
}
#endif


shoes_code
shoes_app_cursor(shoes_app *app, ID cursor)
{
#ifdef SHOES_GTK
  if (app->os.window == NULL || app->os.window->window == NULL || app->cursor == cursor)
    goto done;

  GdkCursor *c;
  if (cursor == s_hand)
  {
    c = gdk_cursor_new(GDK_HAND2);
  }
  else if (cursor == s_arrow)
  {
    c = gdk_cursor_new(GDK_ARROW);
  }
  else
    goto done;

  gdk_window_set_cursor(app->os.window->window, c);
#endif


  app->cursor = cursor;

done:
  return SHOES_OK;
}

shoes_code
shoes_app_resize(shoes_app *app, int width, int height)
{
  app->width = width;
  app->height = height;

#ifdef SHOES_GTK
  if (app->os.window != NULL)
    gtk_widget_set_size_request(app->os.window, app->width, app->height);
#endif

#ifdef SHOES_QUARTZ
  Rect gRect;
  GetWindowBounds(app->os.window, kWindowContentRgn, &gRect);
  gRect.right = app->width + gRect.left;
  gRect.bottom = app->height + gRect.top;
  SetWindowBounds(app->os.window, kWindowContentRgn, &gRect);
#endif

#ifdef SHOES_WIN32
  if (app->slot.window != NULL)
  {
    RECT r;
    GetWindowRect(app->slot.window, &r);
    r.right = r.left + app->width;
    r.bottom = r.top + app->height;
    AdjustWindowRect(&r, WINDOW_STYLE, FALSE);
    MoveWindow(app->slot.window, r.left, r.top, r.right - r.left, r.bottom - r.top, TRUE);
  }
#endif

  return SHOES_OK;
}

VALUE
shoes_app_main(int argc, VALUE *argv, VALUE self)
{
  VALUE attr, block;
  GLOBAL_APP(app);

  rb_scan_args(argc, argv, "01&", &attr, &block);
  rb_iv_set(self, "@main_app", block);

  app->title = ATTR(attr, title);
  app->resizable = (ATTR(attr, resizable) != Qfalse);
  shoes_app_resize(app, ATTR2(int, attr, width, SHOES_APP_WIDTH), ATTR2(int, attr, height, SHOES_APP_HEIGHT));
  shoes_canvas_init(app->canvas, app->slot, attr, app->width, app->height);
  return self;
}

void
shoes_app_title(shoes_app *app, VALUE title)
{
  char *msg;
  app->title = rb_str_new2(SHOES_APPNAME);
  if (!NIL_P(title))
  {
    rb_str_cat2(app->title, " - ");
    rb_str_append(app->title, title);
  }
  msg = RSTRING_PTR(app->title);

#ifdef SHOES_GTK
  gtk_window_set_title(GTK_WINDOW(app->os.window), _(msg));
#endif

#ifdef SHOES_QUARTZ
  CFStringRef cfmsg = CFStringCreateWithCString(NULL, msg, kCFStringEncodingUTF8);
  SetWindowTitleWithCFString(app->os.window, cfmsg);
#endif

#ifdef SHOES_WIN32
  SetWindowText(app->slot.window, msg);
#endif
}

shoes_code
shoes_app_start(VALUE appobj, char *uri)
{
  shoes_code code;
  shoes_app *app;
  Data_Get_Struct(appobj, shoes_app, app);

  code = shoes_app_open(app);
  if (code != SHOES_OK)
    return code;

  code = shoes_app_loop(app, uri);
  if (code != SHOES_OK)
    return code;

  return shoes_app_close(app);
}

shoes_code
shoes_app_open(shoes_app *app)
{
  shoes_code code = SHOES_OK;

#ifdef SHOES_GTK
  shoes_app_gtk *gk = &app->os;
  shoes_slot_gtk *gs = &app->slot;

  gtk_init(NULL, NULL);

  gtk_window_set_default_icon_from_file("static/shoes-icon.png", NULL);
  gk->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  if (!app->resizable)
    gtk_window_set_resizable(GTK_WINDOW(gk->window), FALSE);
  g_signal_connect(G_OBJECT(gk->window), "expose-event",
                   G_CALLBACK(shoes_app_gtk_paint), app);
  g_signal_connect(G_OBJECT(gk->window), "motion-notify-event", 
                   G_CALLBACK(shoes_app_gtk_motion), app);
  g_signal_connect(G_OBJECT(gk->window), "delete-event",
                   G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(gk->window), "key-press-event",
                   G_CALLBACK(shoes_app_gtk_keypress), app);
  app->slot.canvas = gk->window;
#endif

#ifdef SHOES_QUARTZ
  const EventTypeSpec  windowEvents[]  =   {   
    { kEventClassCommand,   kEventCommandProcess },
    { kEventClassTextInput, kEventTextInputUpdateActiveInputArea },
    { kEventClassTextInput, kEventTextInputUnicodeForKeyEvent },
    { kEventClassMouse,    kEventMouseMoved },
    { kEventClassMouse,    kEventMouseDragged },
    { kEventClassMouse,    kEventMouseDown },
    { kEventClassMouse,    kEventMouseUp },
    { kEventClassWindow,   kEventWindowBoundsChanged }
  };

  Rect                   gRect;
  static EventHandlerUPP gTestWindowEventProc = NULL;
  OSStatus               err;
  EventLoopTimerRef      redrawTimer;

  app->slot.controls = Qnil;
  SetRect(&gRect, 100, 100, app->width + 100, app->height + 100);

  INFO("Draw QUARTZ window.\n", 0);
  err = CreateNewWindow(kDocumentWindowClass,
      kWindowCompositingAttribute
    | kWindowLiveResizeAttribute
    | kWindowStandardDocumentAttributes
    | kWindowStandardHandlerAttribute
    ^ (app->resizable ? kWindowResizableAttribute : 0),
    &gRect,
    &app->os.window);

  if (err != noErr)
  {
    QUIT("Couldn't make a new window.", 0);
  }

  InitCursor();

  err = AEInstallEventHandler(kCoreEventClass, kAEQuitApplication, 
    NewAEEventHandlerUPP(shoes_app_quartz_quit), 0, false);
  if (err != noErr)
  {
    QUIT("Out of memory.", 0);
  }

  err = AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, 
    NewAEEventHandlerUPP(shoes_app_quartz_open), 0, false);
  if (err != noErr)
  {
    QUIT("Out of memory.", 0);
  }

  gTestWindowEventProc = NewEventHandlerUPP(shoes_app_quartz_handler);
  if (gTestWindowEventProc == NULL)
  {
    QUIT("Out of memory.", 0);
  }

  INFO("Event handler.\n", 0);
  err = InstallWindowEventHandler(app->os.window,
    gTestWindowEventProc, GetEventTypeCount(windowEvents),
    windowEvents, app, NULL);

  HIViewFindByID(HIViewGetRoot(app->os.window), kHIViewWindowContentID, &app->slot.view);

  if (PasteboardCreate(kPasteboardClipboard, &shoes_world->os.clip) != noErr) {
    INFO("Apple Pasteboard create failed.\n", 0);
  }
#endif

#ifdef SHOES_WIN32
  RECT rect;

  app->slot.controls = Qnil;
  app->slot.focus = Qnil;
  app->os.ctrlkey = false;
  app->os.altkey = false;
  app->os.shiftkey = false;
  app->os.classex.hInstance = shoes_world->os.instance;
  app->os.classex.lpszClassName = SHOES_SHORTNAME;
  app->os.classex.lpfnWndProc = shoes_app_win32proc;
  app->os.classex.style = CS_HREDRAW | CS_VREDRAW;
  app->os.classex.cbSize = sizeof(WNDCLASSEX);
  app->os.classex.hIcon = LoadIcon(shoes_world->os.instance, IDI_APPLICATION);
  app->os.classex.hIconSm = LoadIcon(shoes_world->os.instance, IDI_APPLICATION);
  app->os.classex.hCursor = LoadCursor(NULL, IDC_ARROW);
  app->os.classex.lpszMenuName = NULL;
  app->os.classex.cbClsExtra = 0;
  app->os.classex.cbWndExtra = 0;
  app->os.classex.hbrBackground = 0;

  if (!RegisterClassEx(&app->os.classex))
  {
    QUIT("Couldn't register WIN32 window class.");
  }

  app->os.vlclassex.hInstance = app->os.slotex.hInstance = shoes_world->os.instance;
  app->os.vlclassex.lpszClassName = SHOES_VLCLASS;
  app->os.slotex.lpszClassName = SHOES_SLOTCLASS;
  app->os.vlclassex.style = app->os.slotex.style = CS_NOCLOSE;
  app->os.vlclassex.lpfnWndProc = DefWindowProc;
  app->os.slotex.lpfnWndProc = shoes_slot_win32proc;
  app->os.vlclassex.cbSize = app->os.slotex.cbSize = sizeof(WNDCLASSEX);
  app->os.vlclassex.hIcon = app->os.slotex.hIcon = NULL;
  app->os.vlclassex.hIconSm = app->os.slotex.hIconSm = NULL;
  app->os.vlclassex.hCursor = app->os.slotex.hCursor = LoadCursor(NULL, IDC_ARROW);
  app->os.vlclassex.lpszMenuName = app->os.slotex.lpszMenuName = NULL;
  app->os.vlclassex.cbClsExtra = app->os.slotex.cbClsExtra = 0;
  app->os.vlclassex.cbWndExtra = app->os.slotex.cbWndExtra = 0;
  app->os.vlclassex.hbrBackground = app->os.slotex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

  if (!RegisterClassEx(&app->os.slotex) || !RegisterClassEx(&app->os.vlclassex))
  {
    QUIT("Couldn't register VLC window class.");
  }

  // remove the menu
  rect.left = 0;
  rect.top = 0;
  rect.right = app->width;
  rect.bottom = app->height;
  AdjustWindowRect(&rect, WINDOW_STYLE, FALSE);

  app->slot.window = CreateWindowEx(
    WS_EX_CLIENTEDGE, SHOES_SHORTNAME, SHOES_APPNAME,
    WINDOW_STYLE | WS_CLIPCHILDREN |
      (app->resizable ? (WS_THICKFRAME | WS_MAXIMIZEBOX) : WS_DLGFRAME) |
      WS_VSCROLL | ES_AUTOVSCROLL,
    CW_USEDEFAULT, CW_USEDEFAULT,
    rect.right-rect.left, rect.bottom-rect.top,
    HWND_DESKTOP,
    NULL,
    shoes_world->os.instance,
    NULL);

  SetWindowLong(app->slot.window, GWL_USERDATA, (long)app);

  SCROLLINFO si;
  si.cbSize = sizeof(SCROLLINFO);
  si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
  si.nMin = 0;
  si.nMax = 0; 
  si.nPage = 0;
  si.nPos = 0;
  SetScrollInfo(app->slot.window, SB_VERT, &si, TRUE);
#endif

  shoes_app_title(app, app->title);

quit:
  return code;
}

shoes_code
shoes_app_loop(shoes_app *app, char *path)
{
  shoes_code code = SHOES_OK;
#ifndef SHOES_GTK
  app->slot.controls = rb_ary_new();
  rb_gc_register_address(&app->slot.controls);
#endif
  shoes_slot_init(app->canvas, &app->slot, 0, 0, app->width, app->height, TRUE);
  code = shoes_app_goto(app, path);
  if (code != SHOES_OK)
    return code;
  INFO("RUNNING LOOP.\n", 0);

#ifdef SHOES_QUARTZ
  ShowWindow(app->os.window);
  TextEncoding utf8Encoding, unicodeEncoding;
  utf8Encoding = CreateTextEncoding(kTextEncodingUnicodeDefault,
    kUnicodeNoSubset, kUnicodeUTF8Format);
  unicodeEncoding = CreateTextEncoding(kTextEncodingUnicodeDefault,
    kUnicodeNoSubset, kUnicode16BitFormat);
  TECCreateConverter(&shoes_world->os.converter, unicodeEncoding, utf8Encoding);
  RunApplicationEventLoop();
#endif

#ifdef SHOES_GTK
  gtk_widget_show_all(app->os.window);
  g_idle_add(shoes_app_gtk_idle, app);
  gtk_main();
#endif

#ifdef SHOES_WIN32
  MSG msgs;
  ShowWindow(app->slot.window, SW_SHOWNORMAL);
  while (WM_QUIT != msgs.message)
  {
    BOOL msg = PeekMessage(&msgs, NULL, 0, 0, PM_REMOVE);
    if (msg)
    {
      if (msgs.message == WM_KEYDOWN || msgs.message == WM_KEYUP)
      {
        if (RARRAY_LEN(app->slot.controls) > 0)
        {
          switch (msgs.wParam)
          {
            case VK_TAB: case VK_UP: case VK_LEFT: case VK_DOWN:
            case VK_RIGHT: case VK_PRIOR: case VK_NEXT:
              break;
            default:
              msg = false;
          }
        }
        else msg = false;
      }
      else if (msgs.message == WM_SYSCHAR || msgs.message == WM_CHAR)
        msg = false;
      if (msg) msg = IsDialogMessage(app->slot.window, &msgs);
      if (!msg)
      {
        TranslateMessage(&msgs);
        DispatchMessage(&msgs);
      }
    }
    else
    {
      rb_eval_string("sleep(0.001)");
    }
  }
#endif

quit:
  return SHOES_OK;
}

typedef struct
{
  shoes_app *app;
  VALUE canvas;
  VALUE block;
  char ieval;
  VALUE args;
} shoes_exec;

struct METHOD {
    VALUE klass, rklass;
    VALUE recv;
    ID id, oid;
    int safe_level;
    NODE *body;
};

static VALUE
rb_unbound_get_class(VALUE method)
{
  struct METHOD *data;
  Data_Get_Struct(method, struct METHOD, data);
  return data->rklass;
}

static VALUE
shoes_app_run(VALUE rb_exec)
{
  shoes_exec *exec = (shoes_exec *)rb_exec;
  rb_ary_push(exec->app->nesting, exec->canvas);
  if (exec->ieval)
  {
    VALUE obj;
    obj = mfp_instance_eval(exec->app->self, exec->block);
    return obj;
  }
  else
  {
    int i;
    VALUE vargs[10];
    for (i = 0; i < RARRAY_LEN(exec->args); i++)
      vargs[i] = rb_ary_entry(exec->args, i);
    return rb_funcall2(exec->block, s_call, RARRAY_LEN(exec->args), vargs);
  }
}

static VALUE
shoes_app_exception(VALUE rb_exec, VALUE e)
{
  shoes_exec *exec = (shoes_exec *)rb_exec;
  rb_ary_clear(exec->app->nesting);
  rb_iv_set(exec->canvas, "@exc", e);
  return mfp_instance_eval(exec->canvas, exception_proc);
}

shoes_code
shoes_app_visit(shoes_app *app, char *path)
{
  long i;
  shoes_exec exec;
  shoes_canvas *canvas;
  VALUE meth;
  VALUE ary = rb_ary_dup(app->timers);
  Data_Get_Struct(app->canvas, shoes_canvas, canvas);

  canvas->scrolly = 0;
#ifndef SHOES_GTK
  rb_ary_clear(app->slot.controls);
#endif
  for (i = 0; i < RARRAY_LEN(ary); i++) 
  {
    VALUE timer = rb_ary_entry(ary, i);
    if (!NIL_P(timer))
      rb_funcall(timer, s_remove, 0);
  }

  shoes_canvas_clear(app->canvas);
  shoes_app_reset_styles(app);
  meth = rb_funcall(cShoes, s_run, 1, app->location = rb_str_new2(path));
  if (NIL_P(rb_ary_entry(meth, 0)))
  {
    VALUE script = shoes_dialog_open(app->canvas);
    if (NIL_P(script))
      return SHOES_QUIT;
    rb_funcall(cShoes, rb_intern("load"), 1, script);
    meth = rb_funcall(cShoes, s_run, 1, app->location);
  }

  exec.app = app;
  exec.block = rb_ary_entry(meth, 0);
  exec.args = rb_ary_entry(meth, 1);
  if (rb_obj_is_kind_of(exec.block, rb_cUnboundMethod)) {
    VALUE klass = rb_unbound_get_class(exec.block);
    exec.canvas = shoes_slot_new(klass, Qnil, app->canvas);
    exec.block = rb_funcall(exec.block, s_bind, 1, exec.canvas);
    exec.ieval = 0;
    rb_ary_push(canvas->contents, exec.canvas);
  } else {
    exec.canvas = app->canvas;
    exec.ieval = 1;
  }
  rb_rescue2(CASTHOOK(shoes_app_run), (VALUE)&exec, CASTHOOK(shoes_app_exception), (VALUE)&exec, rb_cObject, 0);
  rb_ary_clear(exec.app->nesting);
  return SHOES_OK;
}

shoes_code
shoes_app_paint(shoes_app *app)
{
  shoes_canvas_paint(app->canvas);
  return SHOES_OK;
}

shoes_code
shoes_app_motion(shoes_app *app, int x, int y)
{
  app->mousex = x; app->mousey = y;
  shoes_canvas_send_motion(app->canvas, x, y, Qnil);
  return SHOES_OK;
}

shoes_code
shoes_app_click(shoes_app *app, int button, int x, int y)
{
  shoes_canvas_send_click(app->canvas, button, x, y);
  return SHOES_OK;
}

shoes_code
shoes_app_release(shoes_app *app, int button, int x, int y)
{
  shoes_canvas_send_release(app->canvas, button, x, y);
  return SHOES_OK;
}

shoes_code
shoes_app_keypress(shoes_app *app, VALUE key)
{
  shoes_canvas_send_keypress(app->canvas, key);
  return SHOES_OK;
}

shoes_code
shoes_app_close(shoes_app *app)
{
  return SHOES_OK;
}

VALUE
shoes_sys(char *cmd, int detach)
{
  if (detach)
    return rb_funcall(rb_mKernel, rb_intern("system"), 1, rb_str_new2(cmd));
  else
    return rb_funcall(rb_mKernel, '`', 1, rb_str_new2(cmd));
}

void
shoes_browser_open(char *url)
{
#ifdef SHOES_GTK
  VALUE browser = rb_str_new2("/etc/alternatives/x-www-browser '");
  rb_str_cat2(browser, url);
  rb_str_cat2(browser, "' 2>/dev/null &");
  shoes_sys(RSTRING_PTR(browser), 1);
#endif

#ifdef SHOES_QUARTZ
  VALUE browser = rb_str_new2("open ");
  rb_str_cat2(browser, url);
  shoes_sys(RSTRING_PTR(browser), 1);
#endif

#ifdef SHOES_WIN32
  ShellExecute(0, "open", url, 0, 0, 0);
#endif
}

shoes_code
shoes_app_goto(shoes_app *app, char *path)
{
  shoes_code code = SHOES_OK;
  const char http_scheme[] = "http://";
  if (strlen(path) > strlen(http_scheme) && strncmp(http_scheme, path, strlen(http_scheme)) == 0) {
    shoes_browser_open(path);
  } else {
    code = shoes_app_visit(app, path);
    if (code == SHOES_OK)
    {
      shoes_app_motion(app, app->mousex, app->mousey);
      shoes_slot_repaint(&app->slot);
    }
  }
  return code;
}

shoes_code
shoes_slot_repaint(SHOES_SLOT_OS *slot)
{
#ifdef SHOES_GTK
  gtk_widget_queue_draw(slot->canvas);
#endif
#ifdef SHOES_QUARTZ
  HIViewSetNeedsDisplay(slot->view, true);
#endif
#ifdef SHOES_WIN32
  InvalidateRgn(slot->window, NULL, TRUE);
  UpdateWindow(slot->window);
#endif
  return SHOES_OK;
}

static void
shoes_style_set(VALUE styles, VALUE klass, VALUE k, VALUE v)
{
  VALUE hsh = rb_hash_aref(styles, klass);
  if (NIL_P(hsh))
    rb_hash_aset(styles, klass, hsh = rb_hash_new());
  rb_hash_aset(hsh, k, v);
}

#define STYLE(klass, k, v) \
  shoes_style_set(app->styles, klass, \
    ID2SYM(rb_intern("" # k)), rb_str_new2("" # v))

void
shoes_app_reset_styles(shoes_app *app)
{
  app->styles = rb_hash_new();
  style_set(Banner,      size, 48);
  style_set(Title,       size, 34);
  style_set(Subtitle,    size, 26);
  style_set(Tagline,     size, 18);
  style_set(Caption,     size, 14);
  style_set(Para,        size, 12);
  style_set(Inscription, size, 10);

  style_set(Code,        family, monospace);
  style_set(Del,         strikethrough, single);
  style_set(Em,          emphasis, italic);
  style_set(Ins,         underline, single);
  style_set(Link,        underline, single);
  style_set(Link,        stroke, #06E);
  style_set(LinkHover,   underline, single);
  style_set(LinkHover,   stroke, #039);
  style_set(LinkHover,   fill,   #EEE);
  style_set(Strong,      weight, bold);
  style_set(Sup,         rise,   10);
  style_set(Sup,         size,   x-small);
  style_set(Sub,         rise,   -10);
  style_set(Sub,         size,   x-small);
}

void
shoes_app_style(shoes_app *app, VALUE klass, VALUE hsh)
{
  long i;
  VALUE keys = rb_funcall(hsh, s_keys, 0);
  for ( i = 0; i < RARRAY(keys)->len; i++ )
  {
    VALUE key = rb_ary_entry(keys, i);
    VALUE val = rb_hash_aref(hsh, key);
    if (!SYMBOL_P(key)) key = rb_str_intern(key);
    shoes_style_set(app->styles, klass, key, val);
  }
}

VALUE
shoes_app_location(VALUE self)
{
  shoes_app *app;
  Data_Get_Struct(self, shoes_app, app);
  return app->location;
}

VALUE
shoes_app_quit(VALUE self)
{
#ifdef SHOES_GTK
  gtk_main_quit();
#endif
#ifdef SHOES_QUARTZ
  QuitApplicationEventLoop();
#endif
#ifdef SHOES_WIN32
  PostQuitMessage(0);
#endif
  return self;
}