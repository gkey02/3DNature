/* WCS.c (ne gis.c 14 Jan 1994 CXH)
** The Main Vein, The Real Deal. The Right Stuff. The terrain renderer program.
** Hacked up into little bitty bloody bits and header files and reintegrated
** with stuff from map...* on 24 Jul 1993 by Chris "Xenon" Hanson.
** Original code and subsequent wrinkles, crinkles and crenulations
** by Gary R. Huber.
** Initialization and event loop here, in support.c and gui.c recoded on
** 23 Mar 1994 by CXH.
*/

#define MAIN

#include "WCS.h"
#include "Version.h"

#define MIN_LIBRARY_REV 37
#define DITHER_TABLE_SIZE 4096

void main(void)
{
short ResetScrn = 0;
struct WCSApp *WCSRootApp;
struct WCSScreenMode *ScreenModes, *ModeSelect;
char *AppBaseName;

ResetScreenMode:

if (IntuitionBase = (struct IntuitionBase *)
 OpenLibrary("intuition.library", MIN_LIBRARY_REV))
 {
 if (GfxBase = (struct GfxBase *)
  OpenLibrary("graphics.library", MIN_LIBRARY_REV))
  {
  if (AslBase = OpenLibrary("asl.library", MIN_LIBRARY_REV))
   {
   if (GadToolsBase = OpenLibrary("gadtools.library", MIN_LIBRARY_REV))
    {
    if(MUIMasterBase = OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN))
     {
     getcwd(path, 255);

     memset(&ScrnData, 0, sizeof (struct WCSScreenData));
     if (! LoadProject("WCS.Prefs", &ScrnData, 0))
      {
      strcpy(dirname, "WCSProjects:");
      strcpy(dbasepath, "WCSProjects:");
      strcpy(parampath, "WCSProjects:");
      strcpy(framepath, "WCSFrames:");
      strcpy(framefile, "Frame");
      strcpy(linepath, "WCSFrames:");
      strcpy(linefile, "Lines");
      strcpy(zbufferpath, "WCSProjects:");
      strcpy(backgroundpath, "WCSProjects:");
      strcpy(graphpath, "WCSFrames:");
      strcpy(colormappath, "WCSProjects:");
      strcpy(statpath, "WCSProjects:");
      strcpy(temppath, "WCSFrames:");
      strcpy(tempfile, "TempFrame");
      strcpy(modelpath, "WCSProjects:");
      strcpy(cloudpath, "WCSProjects:");
      strcpy(wavepath, "WCSProjects:");
      strcpy(deformpath, "WCSProjects:");
      strcpy(imagepath, "WCSFrames:");
      } /* if */
     if (ResetScrn)
      {
      ResetScrn = 0;
      ScrnData.ModeID = 0;
      } /* if user wishes to reset screen mode */
     
     if(WCSRootApp = WCS_App_New())
      {
      app = WCSRootApp->MUIApp;
      
      get(app, MUIA_Application_Base, &AppBaseName);
      
      if (ScrnData.ModeID == 0)
       {
       ModeSelect = NULL;
       ScrnData.AutoTag = TAG_IGNORE;
       ScrnData.AutoVal = NULL;
       if(ScreenModes = ModeList_New())
        {
        if(ModeSelect = ModeList_Choose(ScreenModes, &ScrnData))
         {
         if(ModeSelect->OX > ModeSelect->X)
          { /* Enable Oscan */
          ScrnData.OTag = SA_Overscan;
          ScrnData.OVal = OSCAN_TEXT;
          if(ModeSelect->OX == ModeSelect->OScans[1].x)
           ScrnData.OVal = OSCAN_STANDARD;
          if(ModeSelect->OX == ModeSelect->OScans[2].x)
           ScrnData.OVal = OSCAN_MAX;
          if(ModeSelect->OX == ModeSelect->OScans[3].x)
           ScrnData.OVal = OSCAN_VIDEO;
          if(ModeSelect->UX > ModeSelect->OX)
           {
           ScrnData.AutoTag = SA_AutoScroll;
           ScrnData.AutoVal = TRUE;
           } /* if */
          } /* if */
         else
          { /* Turn off OverScan */
          ScrnData.OTag = TAG_IGNORE;
          ScrnData.OVal = NULL;
          } /* else */
         WCSScrn = OpenScreenTags(NULL, SA_DisplayID, ModeSelect->ModeID,
          SA_Width, ModeSelect->UX, SA_Height, ModeSelect->UY,
          SA_Depth, 4, SA_Title, APP_TITLE, SA_Type, CUSTOMSCREEN,
          ScrnData.OTag, ScrnData.OVal, ScrnData.AutoTag, ScrnData.AutoVal, SA_Colors, NewAltColors,
          SA_Pens, PenSpec, SA_PubName, AppBaseName, TAG_END);
         } /* if */
        else
         {
         WCSScrn = NULL; /* This'll make it exit. */
         } /* else */
        ModeList_Del(ScreenModes);
        ScreenModes = NULL;
        } /* if */
       else
        { /* Can't get screenmodes, default: NTSC-Hires-Lace */
        WCSScrn = OpenScreenTags(NULL, SA_DisplayID, HIRESLACE_KEY,
         SA_Width, STDSCREENWIDTH, SA_Height, STDSCREENHEIGHT,
         SA_Depth, 4, SA_Title, APP_TITLE, SA_Type, CUSTOMSCREEN,
         SA_Overscan, OSCAN_MAX, SA_Colors, NewAltColors,
         SA_Pens, PenSpec, SA_PubName, AppBaseName, TAG_END);
        } /* else */
       } /* if no screen data in WCS.Prefs */
      else
       {
       WCSScrn = OpenScreenTags(NULL, SA_DisplayID, ScrnData.ModeID,
        SA_Width, ScrnData.Width, SA_Height, ScrnData.Height,
        SA_Depth, 4, SA_Title, APP_TITLE, SA_Type, CUSTOMSCREEN,
        ScrnData.OTag, ScrnData.OVal, ScrnData.AutoTag, ScrnData.AutoVal, SA_Colors, NewAltColors,
        SA_Pens, PenSpec, SA_PubName, AppBaseName, TAG_END);
       } /* else read screen data from prefs file */

      if(WCSScrn)
       {
       DTable = DitherTable_New(DITHER_TABLE_SIZE);
       PubScreenStatus(WCSScrn, 0);

       if (DTable)
        {
        if (! DL)
         DL = DirList_New(dirname, 0);

        if (DL)
         {
         if(WCS_App_Startup(WCSRootApp))
          {
          ResetScrn = WCS_App_EventLoop(WCSRootApp); /* Fa la la la la, la la la la! */
          WCS_App_Del(WCSRootApp); /* Erk. Aaaaarrrrggggh... */
          WCSRootApp = NULL;
          app = NULL;
          } /* if */
         SaveProject(0, "WCS.Prefs", &ScrnData);
         DirList_Del(DL);
         DL = NULL;
	 } /* if */
        DitherTable_Del(DTable, DITHER_TABLE_SIZE);
        DTable = NULL;
        } /* if dither table */

       CloseScreen(WCSScrn);
       WCSScrn = NULL;
       } /* if */
      if(WCSRootApp) /* May have already shut down above. */
       {
       WCS_App_Del(WCSRootApp);
       WCSRootApp = NULL;
       } /* if */
      } /* if */
     CloseLibrary(MUIMasterBase);
     MUIMasterBase = NULL;
     } /* if */
    else
     {
     printf("FATAL ERROR: MUIMaster.Library revision %d required. Aborting.\n", MUIMASTER_VMIN);
     } /* else */
    CloseLibrary(GadToolsBase);
    GadToolsBase = NULL;
    }
   else
    {
    printf("FATAL ERROR: GadTools.Library revision %d required. Aborting.\n", MUIMASTER_VMIN);
    } /* else */
   CloseLibrary(AslBase);
   AslBase = NULL;
   } /* if */
  else
   {
   printf("FATAL ERROR: ASL.Library revision %d required. Aborting.\n", MIN_LIBRARY_REV);
   } /* else */
  CloseLibrary(GfxBase);
  GfxBase = NULL;
  } /* if */
 else
  {
  printf("FATAL ERROR: Graphics.Library revision %d required. Aborting.\n", MIN_LIBRARY_REV);
  } /* else */
 CloseLibrary((struct Library *)IntuitionBase);
 IntuitionBase = NULL;
 } /* if */
else
 {
 printf("FATAL ERROR: Intuition.library revision %d required. Aborting.\n", MIN_LIBRARY_REV);
 } /* else */


Cleanup:
 if (DL) DirList_Del(DL);
 DL = NULL;
 if (KF) free_Memory(KF, KFsize);
 KF = NULL;
 if (UndoKF) free_Memory(UndoKF, UndoKFsize);
 UndoKF = NULL;
 if (KT) FreeKeyTable();
 KT = NULL;
 DisposeEcotypes();
#ifdef MEMTRACK
 if (MemTrack)
  printf("\n**** Memory Not Returned to System : %d bytes ****\n\n", MemTrack);
#endif /* MEMTRACK */

 if (ResetScrn)
  {
  dbaseloaded = 0;
  paramsloaded = 0;
  goto ResetScreenMode;
  } /* if user wishes to reset screen mode */

} /* main() */
