 #include <SDL/SDL.h>
 #include <SDL/SDL_image.h>
 #include <stdio.h>
 #include <time.h>
 #include <sys/timeb.h>
 
 int main()
 {
     SDL_Rect full  = { 0 , 0, 640, 480} ;
     SDL_Rect srect = { 0 , 0, 32 , 64 } ;
     SDL_Surface *screen, *image;
     SDL_Event   event;
     long frames = 0;
     struct timeb start, stop;
     double elapsed = 0;
     double rate    = 0;
     int    ok      = 1;
     /* putenv("SDL_VIDEODRIVER=dga"); */
     SDL_Init(SDL_INIT_EVERYTHING);
     screen = SDL_SetVideoMode(640, 480, 32, SDL_FULLSCREEN | SDL_ASYNCBLIT);
     image  = IMG_Load("test_tile.png");
     image  = SDL_DisplayFormatAlpha(image);

     ftime(&start);

     /* Start the game loop */
     while (ok)
     {
        int xx = 0;
        int yy = 0;
        /* Process events */
        while (SDL_PollEvent(&event))
         {
             /* Close window : exit */
             if (event.type == SDL_QUIT)
                 ok = 0;
             else { if (event.type == SDL_KEYDOWN)
                 ok = 0; }   
         }
 
         /* Clear the screen */
         SDL_FillRect(screen, &full, 0);
 
         /* Draw the sprite */
         for (srect.x=0; srect.x < 640; srect.x+=32) {
          for (srect.y=0; srect.y < 480; srect.y+=32) {
            SDL_BlitSurface(image, NULL, screen, &srect);
          }
         }  
         
         SDL_Flip(screen);
         
         frames ++;
     }

     ftime(&stop); 
     elapsed   = ((double) (stop.time - start.time)) + ((double) stop.millitm - start.millitm ) / 1000.0 ;
     printf("Frames: %ld, %lf %lf\n", frames, elapsed, frames / elapsed);
 
     /* Cleanup resources */
     SDL_FreeSurface(image);
     SDL_Quit();
 
     return EXIT_SUCCESS;
 }
