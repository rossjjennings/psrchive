#include "rhythm.h"

void rhythm::menubarConstruct ()
{
  if (verbose) cerr << "rhythm::menubarConstruct () entered\n";

  Gtk_ItemFactory* itemf = new Gtk_ItemFactory_MenuBar("<Main>");

  // /////////////////////////////////////////////////////////////////////////
  // create the main headings
  GtkItemFactoryEntry menuf_items[] =
  {
    {"/File",              0, 0, 0, "<Branch>" },
    {"/Options",           0, 0, 0, "<Branch>" },
    {"/Fit",               0, 0, 0, "<Branch>" },
    {"/Help",              0, 0, 0, "<LastBranch>" }
  };
  int nmenuf_items = sizeof(menuf_items)/sizeof(menuf_items[0]);

  itemf->create_items (nmenuf_items, menuf_items, 0);

  // /////////////////////////////////////////////////////////////////////////
  // create the File menu

  itemf->create_item ("/File/Load toas", "Alt+O", "<Item>",
		      ItemFactoryConnector<rhythm, int> (this,&fileload,TIM));
  itemf->create_item ("/File/Load ephemeris", "Alt+P", "<Item>",
		      ItemFactoryConnector<rhythm, int> (this,&fileload,EPH));

  itemf->create_item ("/File/", 0, "<Separator>", 0);

  // a pointer to the save-blah menu items is kept so that they may be
  // enabled/disabled when appropriate
  Gtk_Widget* menuitem = NULL;
  menuitem = itemf->create_item ("/File/Save toas", "Alt+S", "<Item>",
		      ItemFactoryConnector<rhythm, int> (this,&filesave,TIM));

  // save arrival times
  save_tim = dynamic_cast <Gtk_MenuItem*> (menuitem);
  if (save_tim == NULL) {
    cerr << "rhythm::menubarConstruct error dynamic_cast save_tim\n";
    throw ("dynamic_cast");
  }
  save_tim->deselect();

  menuitem = itemf->create_item ("/File/Save ephemeris", "Alt+D", "<Item>",
		      ItemFactoryConnector<rhythm, int> (this,&filesave,EPH));

  // save ephemeris
  save_eph = dynamic_cast <Gtk_MenuItem*> (menuitem);
  if (save_tim == NULL) {
    cerr << "rhythm::menubarConstruct error dynamic_cast save_tim\n";
    throw ("dynamic_cast");
  }
  save_eph->deselect();

  itemf->create_item ("/File/", 0, "<Separator>", 0);

  itemf->create_item ("/File/Quit", "Alt+Q", "<Item>",
		      ItemFactoryConnector<rhythm, int> (this, &exit, 1) );

  menubar = itemf->get_menubar_widget("");
  g_return_if_fail(menubar.get_object());
  
  if (verbose) cerr << "rhythm::menubarConstruct () returns\n";
}

void rhythm::fileload (int type)
{
  if (file_modified) {
    prompt_save (type);
  }

  if (fileselect == NULL) {
    fileselect = new Gtk_FileSelection ("Rhythm File Selection");
    connect_to_method (fileselect->get_ok_button()->clicked,
		       this, &fileselected);
    connect_to_method (fileselect->get_cancel_button()->clicked, 
		       this, &filecancel);
    connect_to_method (fileselect->delete_event,
		       this, &fileselect_deleted);
    fileselect->hide_fileop_buttons();
  }

  fileio_code = type;

  if (fileio_code == TIM)
    fileselect->complete ("*.tim");
  else if (fileio_code == EPH)
    fileselect->complete ("*.eph");
  
  fileselect->show();
}

void rhythm::prompt_save (int type)
{
}

void rhythm::filesave (int type)
{
}

gint rhythm::fileselect_deleted (GdkEventAny*)
{
  if (verbose) cerr << "rhythm::fileselect_deleted\n";
  // fileselect = NULL;
  return TRUE;
}

void rhythm::filecancel()
{
  if (verbose) cerr << "rhythm::fileselect canceled\n";
  fileselect->hide();
}

void rhythm::fileselected()
{
  if (verbose) cerr << "rhythm::file selected" << 
		 fileselect->get_filename() << endl;

  fileselect->hide();
}

void rhythm::exit (int type)
{
  // TO-DO - prompt for saving files
  Gtk_Main::instance()->quit();
}
