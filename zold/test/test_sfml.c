 #include <SFML/Audio.h>
 #include <SFML/Graphics.h>
 #include <SFML/Graphics/Font.h>
 #include <stdio.h>
 #include <time.h>
 #include <sys/timeb.h>
 
 int main()
 {
     sfWindowSettings Settings = {24, 8, 0};
     sfVideoMode Mode          = {640, 480, 32};
     sfRenderWindow* App;
     sfImage* Image;
     sfSprite* Sprite;
     sfFont* Font;
     sfString* Text;
     sfMusic* Music;
     sfEvent Event;
     struct timeb start, stop;
     long frames    = 0;
     double elapsed = 0;
     double rate    = 0;


     /* Create the main window */
     App = sfRenderWindow_Create(Mode, "SFML window", sfFullscreen, Settings);
     if (!App)
         return EXIT_FAILURE;

     /* Load a sprite to display */
     Image = sfImage_CreateFromFile("test_tile.png");
     if (!Image)
         return EXIT_FAILURE;
     Sprite = sfSprite_Create();
     sfSprite_SetImage(Sprite, Image);
 
     /* Create a graphical string to display */
     Font = sfFont_CreateFromFile("test_font.ttf", 20, NULL);
     if (!Font)
         return EXIT_FAILURE;
     Text = sfString_Create();
     sfString_SetText(Text, "Hello SFML");
     sfString_SetFont(Text, Font);
     sfString_SetSize(Text, 50);
 
     /* Load a music to play */
     Music = sfMusic_CreateFromFile("test_music.ogg");
     if (!Music)
         return EXIT_FAILURE;

     /* Play the music */
     /* sfMusic_Play(Music); */
     ftime(&start);

     /* Start the game loop */
     while (sfRenderWindow_IsOpened(App))
     {
        int xx = 0;
        int yy = 0;
         /* Process events */
         while (sfRenderWindow_GetEvent(App, &Event))
         {
             /* Close window : exit */
             if (Event.Type == sfEvtClosed)
                 sfRenderWindow_Close(App);
             else { if (Event.Type == sfEvtKeyPressed)
                 sfRenderWindow_Close(App); }   
         }
 
         /* Clear the screen */
         sfRenderWindow_Clear(App, sfBlack);
 
         /* Draw the sprite */
         for (xx=0; xx < 640; xx+=32) {
          for (yy=0; yy < 480; yy+=32) {
            sfSprite_SetX(Sprite, xx);
            sfSprite_SetY(Sprite, yy);            
            sfRenderWindow_DrawSprite(App, Sprite);
          }
         }  
         
 
         /* Draw the string */
         /* sfRenderWindow_DrawString(App, Text); */
 
         /* Update the window */
         sfRenderWindow_Display(App);
         frames ++;
     }
     ftime(&stop);
     elapsed  = ((double) (stop.time - start.time)) + ((double) stop.millitm - start.millitm ) / 1000.0 ;
     printf("Frames: %ld, %lf %lf\n", frames, elapsed, frames / elapsed);
 
     /* Cleanup resources */
     sfMusic_Destroy(Music);
     sfString_Destroy(Text);
     sfFont_Destroy(Font);
     sfSprite_Destroy(Sprite);
     sfImage_Destroy(Image);
     sfRenderWindow_Destroy(App);
 
     return EXIT_SUCCESS;
 }
